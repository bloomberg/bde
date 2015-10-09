// ball_attributecontext.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_attributecontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attributecontext_cpp,"$Id$ $CSID$")

#include <ball_categorymanager.h>

#include <bdlma_concurrentpool.h>
#include <bslmt_lockguard.h>
#include <bslmt_once.h>
#include <bslmt_threadlocalvariable.h>

#include <bdlb_print.h>
#include <bdlb_bitutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_ostream.h>



namespace BloombergLP {

namespace ball {
// Implementation Note: The 'AttributeContext' makes uses a cache of rule
// evaluations ('AttributeContext_RuleEvaluationCache') when evaluating the
// 'hasRelevantActiveRules' and the 'getThresholdLevels' methods for a
// category.  When accessing this cache, the singleton rule mutex,
// 's_categoryManager_p->_ruleMutex()', is intentionally *not* locked.  For
// performance reasons this component (and the 'ball' package in general)
// attempts to be consistent, but does *not* provide a *guarantee* that
// messages will (or will not) be logged if any state is modified while the
// logging occurs.  When using the cached rule evaluations - if the
// rule-sequence number is correct at *any* point in the operation, that cached
// data is reasonably up to date (i.e., it's "good enough").  Note that if a
// client required strictly synchronized results for these methods, a lock
// would need to be held outside of this component (until the message was
// actually written to the log).

               // ------------------------------------------
               // class AttributeContext_RuleEvaluationCache
               // ------------------------------------------

// MANIPULATORS
RuleSet::MaskType
AttributeContext_RuleEvaluationCache::update(
                               int                            sequenceNumber,
                               RuleSet::MaskType              relevantRuleMask,
                               const RuleSet&                 rules,
                               const AttributeContainerList&  attributes)

{
    // If the sequence number as changed, the cache is entirely out of data.
    if (d_sequenceNumber != sequenceNumber) {
        d_resultMask     = 0;
        d_evalMask       = 0;
        d_sequenceNumber = sequenceNumber;
    }

    RuleSet::MaskType needEvaluations = (~d_evalMask) & relevantRuleMask;

    int i;
    int numBits = bdlb::BitUtil::sizeInBits(needEvaluations);
    BSLS_ASSERT(numBits == RuleSet::maxNumRules());

    // Get the index of the relevant rules and store it in 'i'.
    while((i = bdlb::BitUtil::numTrailingUnsetBits(needEvaluations))
                                                                  != numBits) {
        const Rule *rule;
        needEvaluations &= ~(1 << i);

        // If the rule needs to be evaluated, and the rule is not null.
        if ((rule = rules.getRuleById(i))) {
            RuleSet::MaskType result = rule->evaluate(attributes) ? 1 : 0;
            d_resultMask |= (result << i);  // or-in result bit.
            d_evalMask   |= 1 << i;
        }
    }
    return d_resultMask;
}

// ACCESSORS
bsl::ostream&
AttributeContext_RuleEvaluationCache::print(
                                           bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << d_sequenceNumber << EL;

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
    const bslmt::ThreadUtil::Key& contextKey =
                                           AttributeContext::contextKey();

    AttributeContext *context = (AttributeContext*)
                                  bslmt::ThreadUtil::getSpecific(contextKey);

    if (context) {
        AttributeContext::removeContext(context);
        bslmt::ThreadUtil::setSpecific(contextKey, 0);
    }
}
}  // close package namespace

                        // ----------------------------
                        // class ball::AttributeContext
                        // ----------------------------

namespace {

// Define a thread-local variable, 'g_threadLocalContext', (on supported
// platforms) that will serve as cache for 'bslmt::ThreadUtil::getSpecific'.
// Note that the memory will still be managed by 'bslmt::ThreadUtil' thread
// specific storage.

#ifdef BSLMT_THREAD_LOCAL_VARIABLE
BSLMT_THREAD_LOCAL_VARIABLE(ball::AttributeContext*, g_threadLocalContext, 0);
#endif

}  // close unnamed namespace

// CLASS MEMBERS
ball::CategoryManager *ball::AttributeContext::s_categoryManager_p = 0;

bslma::Allocator      *ball::AttributeContext::s_globalAllocator_p = 0;

namespace ball {

// PRIVATE CREATORS
AttributeContext::AttributeContext(bslma::Allocator *globalAllocator)
: d_containerList(globalAllocator)
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

    AttributeContext *context = (AttributeContext*)arg;
    if (context) {
        bslma::Allocator *allocator = context->d_allocator_p;

        // Note that we can't call 'bslma::Allocator::deleteObject' because the
        // destructor is private.

        context->~AttributeContext();
        allocator->deallocate(context);
    }
}

// CLASS METHODS
void
AttributeContext::initialize(CategoryManager  *categoryManager,
                             bslma::Allocator *globalAllocator)
{
    if (s_globalAllocator_p) {
        bsl::fprintf(
                 stderr,
                 "Attempt to re-initialize 'AttributeContext'. %s:%d.\n",
                 __FILE__,
                 __LINE__);
        return;                                                       // RETURN
    }
    s_globalAllocator_p = bslma::Default::globalAllocator(globalAllocator);
    s_categoryManager_p = categoryManager;
}

AttributeContext *AttributeContext::lookupContext()
{
#ifdef BSLMT_THREAD_LOCAL_VARIABLE
    return g_threadLocalContext;
#else
    return (AttributeContext*) bslmt::ThreadUtil::getSpecific(contextKey());
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
    AttributeContext *context =
                            new (*allocator) AttributeContext(allocator);
    if (0 != bslmt::ThreadUtil::setSpecific(contextKey(), context)) {
        bsl::fprintf(stderr,
                     "Failed to add 'AttributeContext' to thread "
                     "specific storage. %s : %d\n",
                     __FILE__,
                     __LINE__);
        BSLS_ASSERT(false);
    }
    g_threadLocalContext = context;
    return context;                                                   // RETURN
#else
    const bslmt::ThreadUtil::Key& key = contextKey();

    AttributeContext *context =
        (AttributeContext*)bslmt::ThreadUtil::getSpecific(key);

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
    return context;                                                   // RETURN
#endif
}

// ACCESSORS
bool AttributeContext::hasRelevantActiveRules(const Category *category) const
{
    RuleSet::MaskType relevantRuleMask = category->relevantRuleMask();

    if (!relevantRuleMask) {
        return false;                                                 // RETURN
    }

    // The 'rulesetMutex' is intentionally *not* locked before returning a
    // cached value (see implementation note at the top).
    if (d_ruleCache_p.isDataAvailable(
                                  s_categoryManager_p->ruleSequenceNumber(),
                                  relevantRuleMask)) {
        return relevantRuleMask & d_ruleCache_p.knownActiveRules();   // RETURN
    }

    // We lock the mutex to ensure the rules are not modified as we evaluate
    // them.
    bslmt::LockGuard<bslmt::Mutex> ruleGuard(
                                        &s_categoryManager_p->rulesetMutex());

    return relevantRuleMask &
           d_ruleCache_p.update(s_categoryManager_p->ruleSequenceNumber(),
                                relevantRuleMask,
                                s_categoryManager_p->ruleSet(),
                                d_containerList);
}

void
AttributeContext::determineThresholdLevels(ThresholdAggregate *levels,
                                           const Category     *category) const
{
    if (!category) {
        return;                                                       // RETURN
    }

    // Set the default levels for 'category'.
    levels->setLevels(category->recordLevel(),
                      category->passLevel(),
                      category->triggerLevel(),
                      category->triggerAllLevel());

    RuleSet::MaskType relevantRuleMask = category->relevantRuleMask();

    if (!relevantRuleMask) {
        return;                                                       // RETURN
    }

    // test on the cache outside the lock, and return if there are no active
    // rules for 'category'.  We do not lock on the 'rulesetMutex' before
    // testing the cache (see implementation notes at the top).
    RuleSet::MaskType activeAndRelevantRules;
    if (d_ruleCache_p.isDataAvailable(
                                    s_categoryManager_p->ruleSequenceNumber(),
                                    relevantRuleMask)) {
        activeAndRelevantRules = d_ruleCache_p.knownActiveRules() &
                                 relevantRuleMask;
        if (!activeAndRelevantRules) {
            return;                                                   // RETURN
        }
    }

    // We obtain the lock because we will need to process the rules.  Note
    // that we must again call the 'isDataAvailable' method in case the rules
    // have been modified since the lock was obtained.
    bslmt::LockGuard<bslmt::Mutex> ruleGuard(
                                        &s_categoryManager_p->rulesetMutex());

    // If the 'isDataAvailable' method returns 'true', the rule data has not
    // changed and we must have assigned 'activeAndRelevantRules' before
    // obtaining the lock.
    if (!d_ruleCache_p.isDataAvailable(
                                  s_categoryManager_p->ruleSequenceNumber(),
                                  relevantRuleMask)) {
          activeAndRelevantRules =
                 relevantRuleMask &
                 d_ruleCache_p.update(
                                    s_categoryManager_p->ruleSequenceNumber(),
                                    relevantRuleMask,
                                    s_categoryManager_p->ruleSet(),
                                    d_containerList);
    }

    int i;
    int numBits = bdlb::BitUtil::sizeInBits(activeAndRelevantRules);
    BSLS_ASSERT(numBits == RuleSet::maxNumRules());

    // Get the index of the relevant rules and store it in 'i'.
    while((i = bdlb::BitUtil::numTrailingUnsetBits(activeAndRelevantRules))
                                                                  != numBits) {
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
