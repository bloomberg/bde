// ball_attributecontext.cpp                                          -*-C++-*-
#include <ball_attributecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attributecontext_cpp,"$Id$ $CSID$")

#include <ball_attributecontainer.h>   // for testing only
#include <ball_categorymanager.h>
#include <ball_predicate.h>            // for testing only
#include <ball_rule.h>
#include <ball_thresholdaggregate.h>

#include <bdlb_bitutil.h>
#include <bdlb_print.h>

#include <bdlma_concurrentpool.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmt_lockguard.h>
#include <bslmt_once.h>
#include <bslmt_threadlocalvariable.h>

#include <bsl_cstdio.h>
#include <bsl_ostream.h>

//=============================================================================
//                           IMPLEMENTATION NOTES
//-----------------------------------------------------------------------------
//
///Rule Evaluations Cache
///----------------------
// An 'AttributeContext' object uses a cache of rule evaluations
// ('AttributeContext_RuleEvaluationCache') when applying the
// 'hasRelevantActiveRules' and 'getThresholdLevels' methods to a category.
// When accessing this cache, the rule mutex returned by
// 's_categoryManager_p->rulesetMutex()' is intentionally *not* locked.  For
// performance reasons this component (and the 'ball' package in general)
// attempts to be consistent, but does *not* provide a *guarantee* that
// messages will (or will not) be logged if any state of the logger is modified
// while logging occurs.  When using cached rule evaluations, if the rule set
// timestamp stored in the cache matches the rule set timestamp of the category
// manager at *any* point in the operation, the cached data is considered to be
// reasonably up to date (i.e., it is "good enough").  Note that if a client
// required strictly synchronized results for these methods, a lock would need
// to be held (until the message was actually written to the log).

namespace BloombergLP {
namespace ball {

               // ------------------------------------------
               // class AttributeContext_RuleEvaluationCache
               // ------------------------------------------

// MANIPULATORS
RuleSet::MaskType
AttributeContext_RuleEvaluationCache::update(
                               bsls::Types::Int64            timestamp,
                               RuleSet::MaskType             relevantRulesMask,
                               const RuleSet&                rules,
                               const AttributeContainerList& attributes)
{
    // If the timestamp has changed, the cache is considered entirely out of
    // date.

    if (d_timestamp != timestamp) {
        d_resultMask = 0;
        d_evalMask   = 0;
        d_timestamp  = timestamp;
    }

    RuleSet::MaskType needEvaluations = ~d_evalMask & relevantRulesMask;

    int i;

    // Get the index, 'i', of each relevant rule to evaluate.
    while ((i = bdlb::BitUtil::numTrailingUnsetBits(needEvaluations))
                                                 != RuleSet::e_MAX_NUM_RULES) {
        const Rule *rule;

        // 'rules.getRuleById(i)' will return null if rule 'i' has been removed
        // from the category manager's rule set.

        if ((rule = rules.getRuleById(i))) {
            RuleSet::MaskType result = rule->evaluate(attributes) ? 1 : 0;
            d_resultMask |= result << i;  // OR in result of evaluation.
            d_evalMask   |=      1 << i;  // Mark rule as evaluated.
        }

        needEvaluations &= ~(1 << i);     // We are done with rule 'i'.
    }

    return d_resultMask;
}

// ACCESSORS
bsl::ostream&
AttributeContext_RuleEvaluationCache::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << d_timestamp << EL;

    RuleSet::printMask(stream, d_resultMask, level + 1, spacesPerLevel);

    RuleSet::printMask(stream, d_evalMask, level + 1, spacesPerLevel);

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;
}

                        // -----------------------------
                        // class AttributeContextProctor
                        // -----------------------------

// CREATORS
AttributeContextProctor::~AttributeContextProctor()
{
    const bslmt::ThreadUtil::Key& contextKey = AttributeContext::contextKey();

    AttributeContext *context = (AttributeContext *)
                                    bslmt::ThreadUtil::getSpecific(contextKey);

    if (context) {
        AttributeContext::removeContext(context);
        bslmt::ThreadUtil::setSpecific(contextKey, 0);
    }
}

}  // close package namespace

namespace {

// On supported platforms, define a thread-local variable,
// 'g_threadLocalContext', to serve as the cache for
// 'bslmt::ThreadUtil::getSpecific'.  Note that the memory is managed by
// 'bslmt::ThreadUtil' thread-specific storage.

#ifdef BSLMT_THREAD_LOCAL_VARIABLE
BSLMT_THREAD_LOCAL_VARIABLE(ball::AttributeContext*, g_threadLocalContext, 0);
#endif

}  // close unnamed namespace

namespace ball {

                        // ----------------------
                        // class AttributeContext
                        // ----------------------

// CLASS DATA
CategoryManager  *AttributeContext::s_categoryManager_p = 0;
bslma::Allocator *AttributeContext::s_globalAllocator_p = 0;

// PRIVATE CREATORS
AttributeContext::AttributeContext(bslma::Allocator *globalAllocator)
: d_containerList(bslma::Default::globalAllocator(globalAllocator))
, d_allocator_p(bslma::Default::globalAllocator(globalAllocator))
{
}

AttributeContext::~AttributeContext()
{
}

// PRIVATE CLASS METHODS
const bslmt::ThreadUtil::Key& AttributeContext::contextKey()
{
    static bslmt::ThreadUtil::Key s_contextKey;
    BSLMT_ONCE_DO {
        bslmt::ThreadUtil::createKey(&s_contextKey,
                                     (bslmt::ThreadUtil::Destructor)
                                     AttributeContext::removeContext);
    }
    return s_contextKey;
}

void AttributeContext::removeContext(void *arg)
{
#ifdef BSLMT_THREAD_LOCAL_VARIABLE
    g_threadLocalContext = 0;
#endif

    AttributeContext *context = (AttributeContext *)arg;
    if (context) {
        bslma::Allocator *allocator = context->d_allocator_p;

        // Note that we can't call 'bslma::Allocator::deleteObject' because the
        // destructor is private.

        context->~AttributeContext();
        allocator->deallocate(context);
    }
}

// CLASS METHODS
void AttributeContext::initialize(CategoryManager  *categoryManager,
                                  bslma::Allocator *globalAllocator)
{
    BSLS_ASSERT_OPT(categoryManager);

    if (s_categoryManager_p) {
        bsl::fprintf(stderr,
                     "Attempt to re-initialize 'AttributeContext'. %s:%d.\n",
                     __FILE__,
                     __LINE__);
        return;                                                       // RETURN
    }

    s_categoryManager_p = categoryManager;
    s_globalAllocator_p = bslma::Default::globalAllocator(globalAllocator);
}

void AttributeContext::reset()
{
    s_categoryManager_p = 0;
    s_globalAllocator_p = 0;
}

AttributeContext *AttributeContext::lookupContext()
{
#ifdef BSLMT_THREAD_LOCAL_VARIABLE
    return g_threadLocalContext;
#else
    return (AttributeContext *)bslmt::ThreadUtil::getSpecific(contextKey());
#endif
}

AttributeContext *AttributeContext::getContext()
{
#ifdef BSLMT_THREAD_LOCAL_VARIABLE
    if (g_threadLocalContext) {
        return g_threadLocalContext;                                  // RETURN
    }

    bslma::Allocator *allocator =
                          bslma::Default::globalAllocator(s_globalAllocator_p);
    AttributeContext *context = new (*allocator) AttributeContext(allocator);

    if (0 != bslmt::ThreadUtil::setSpecific(contextKey(), context)) {
        bsl::fprintf(stderr,
                     "Failed to add 'AttributeContext' to thread "
                     "specific storage. %s : %d\n",
                     __FILE__,
                     __LINE__);
        BSLS_ASSERT(false);
    }
    g_threadLocalContext = context;
#else
    const bslmt::ThreadUtil::Key& key = contextKey();

    AttributeContext *context =
                       (AttributeContext *)bslmt::ThreadUtil::getSpecific(key);

    if (!context) {
        bslma::Allocator *allocator =
                          bslma::Default::globalAllocator(s_globalAllocator_p);
        context = new (*allocator) AttributeContext(allocator);
        if (0 != bslmt::ThreadUtil::setSpecific(key, context)) {
            bsl::fprintf(stderr,
                         "Failed to add 'AttributeContext' to thread "
                         "specific storage. %s:%d\n",
                         __FILE__,
                         __LINE__);
            BSLS_ASSERT(false);
        }
    }
#endif
    return context;
}

// ACCESSORS
bool AttributeContext::hasRelevantActiveRules(const Category *category) const
{
    BSLS_ASSERT(category);

    RuleSet::MaskType relevantRulesMask = category->relevantRuleMask();

    if (!relevantRulesMask) {
        return false;                                                 // RETURN
    }

    // The 'rulesetMutex' is intentionally *not* locked before checking the
    // cache (see implementation note at the top).

    if (d_ruleCache_p.isDataAvailable(s_categoryManager_p->ruleSetTimestamp(),
                                      relevantRulesMask)) {
        return relevantRulesMask & d_ruleCache_p.knownActiveRules();  // RETURN
    }

    // We lock the mutex to ensure the rules are not modified as we evaluate
    // them.

    bslmt::LockGuard<bslmt::Mutex> ruleGuard(
                                         &s_categoryManager_p->rulesetMutex());

    return relevantRulesMask
           & d_ruleCache_p.update(s_categoryManager_p->ruleSetTimestamp(),
                                  relevantRulesMask,
                                  s_categoryManager_p->ruleSet(),
                                  d_containerList);
}

void
AttributeContext::determineThresholdLevels(ThresholdAggregate *levels,
                                           const Category     *category) const
{
    BSLS_ASSERT(levels);
    BSLS_ASSERT(category);

    // Set the default levels for 'category'.
    levels->setLevels(category->recordLevel(),
                      category->passLevel(),
                      category->triggerLevel(),
                      category->triggerAllLevel());

    RuleSet::MaskType relevantRulesMask = category->relevantRuleMask();

    if (!relevantRulesMask) {
        return;                                                       // RETURN
    }

    // The 'rulesetMutex' is intentionally *not* locked before checking the
    // cache (see implementation note at the top).  Return if there are no
    // active rules for 'category'.

    RuleSet::MaskType activeAndRelevantRules = 0;
    if (d_ruleCache_p.isDataAvailable(s_categoryManager_p->ruleSetTimestamp(),
                                      relevantRulesMask)) {
        activeAndRelevantRules = d_ruleCache_p.knownActiveRules()
                                 & relevantRulesMask;
        if (!activeAndRelevantRules) {
            return;                                                   // RETURN
        }
    }

    // We obtain the lock because we will need to process the rules.  Note that
    // we must again call 'isDataAvailable' in case the rules were modified
    // before the lock was obtained.

    bslmt::LockGuard<bslmt::Mutex> ruleGuard(
                                         &s_categoryManager_p->rulesetMutex());

    // If 'isDataAvailable' returns 'true', the rule set has not changed and we
    // must have assigned 'activeAndRelevantRules' before obtaining the lock.

    if (!d_ruleCache_p.isDataAvailable(s_categoryManager_p->ruleSetTimestamp(),
                                       relevantRulesMask)) {
          activeAndRelevantRules = relevantRulesMask
                & d_ruleCache_p.update(s_categoryManager_p->ruleSetTimestamp(),
                                       relevantRulesMask,
                                       s_categoryManager_p->ruleSet(),
                                       d_containerList);
    }

    int i;

    // Get the index, 'i', of each active and relevant rule to process.
    while ((i = bdlb::BitUtil::numTrailingUnsetBits(activeAndRelevantRules))
                                                 != RuleSet::e_MAX_NUM_RULES) {
        activeAndRelevantRules &= ~(1 << i);

        const Rule *rule = s_categoryManager_p->ruleSet().getRuleById(i);
        BSLS_ASSERT(0 != rule);

        if (rule->recordLevel() > levels->recordLevel()) {
            levels->setRecordLevel(rule->recordLevel());
        }
        if (rule->passLevel() > levels->passLevel()) {
            levels->setPassLevel(rule->passLevel());
        }
        if (rule->triggerLevel() > levels->triggerLevel()) {
            levels->setTriggerLevel(rule->triggerLevel());
        }
        if (rule->triggerAllLevel() > levels->triggerAllLevel()) {
            levels->setTriggerAllLevel(rule->triggerAllLevel());
        }
    }
}

// ACCESSORS
bsl::ostream& AttributeContext::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    d_containerList.print(stream, level + 1, spacesPerLevel);

    d_ruleCache_p.print(stream, level + 1, spacesPerLevel);

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
