// bael_attributecontext.cpp                 -*-C++-*-
#include <bael_attributecontext.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_attributecontext_cpp,"$Id$ $CSID$")

#include <bael_categorymanager.h>

#include <bcema_pool.h>
#include <bcemt_lockguard.h>
#include <bcemt_once.h>
#include <bces_threadlocalvariable.h>

#include <bdeu_print.h>
#include <bdes_bitutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_ostream.h>



namespace BloombergLP {

// Implementation Note:  The 'bael_AttributeContext' makes uses a cache of
// rule evaluations ('bael_AttributeContext_RuleEvaluationCache') when
// evaluating the 'hasRelevantActiveRules' and the 'getThresholdLevels' methods
// for a category.  When accessing this cache, the singleton rule mutex,
// 's_categoryManager_p->_ruleMutex()', is intentionally *not* locked.  For
// performance reasons this component (and the 'bael' package in general)
// attempts to be consistent, but does *not* provide a *guarantee* that
// messages will (or will not) be logged if any state is modified while the
// logging occurs.  When using the cached rule evaluations - if the
// rule-sequence number is correct at *any* point in the operation, that
// cached data is reasonably up to date (i.e., it's "good enough").  Note that
// if a client required strictly synchronized results for these methods, a
// lock would need to be held outside of this component (until the message was
// actually written to the log).

               // -----------------------------------------------
               // class bael_AttributeContext_RuleEvaluationCache
               // -----------------------------------------------

// MANIPULATORS
bael_RuleSet::MaskType
bael_AttributeContext_RuleEvaluationCache::update(
                        int                                 sequenceNumber,
                        bael_RuleSet::MaskType              relevantRuleMask,
                        const bael_RuleSet&                 rules,
                        const bael_AttributeContainerList&  attributes)

{
    // If the sequence number as changed, the cache is entirely out of data.
    if (d_sequenceNumber != sequenceNumber) {
        d_resultMask     = 0;
        d_evalMask       = 0;
        d_sequenceNumber = sequenceNumber;
    }

    bael_RuleSet::MaskType needEvaluations = (~d_evalMask) & relevantRuleMask;

    int i;
    int numBits = bdes_BitUtil::sizeInBits(needEvaluations);
    BSLS_ASSERT(numBits == bael_RuleSet::maxNumRules());

    // Get the index of the relevant rules and store it in 'i'.
    while((i = bdes_BitUtil::find1AtSmallestIndex(needEvaluations))
                                                                  != numBits) {
        const bael_Rule *rule;
        needEvaluations &= bdes_BitUtil::neMask(i);

        // If the rule needs to be evaluated, and the rule is not null.
        if ((rule = rules.getRuleById(i))) {
            bael_RuleSet::MaskType result = rule->evaluate(attributes) ? 1 : 0;
            d_resultMask |= (result << i);  // or-in result bit.
            d_evalMask   |= bdes_BitUtil::eqMask(i);
        }
    }
    return d_resultMask;
}

// ACCESSORS
bsl::ostream&
bael_AttributeContext_RuleEvaluationCache::print(
                                           bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << d_sequenceNumber << EL;

    bael_RuleSet::printMask(stream, d_resultMask, level + 1, spacesPerLevel);

    bael_RuleSet::printMask(stream, d_evalMask, level + 1, spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;

}

                        // ----------------------------------
                        // class bael_AttributeContextProctor
                        // ----------------------------------

// CREATORS
bael_AttributeContextProctor::~bael_AttributeContextProctor()
{
    const bcemt_ThreadUtil::Key& contextKey =
                                           bael_AttributeContext::contextKey();

    bael_AttributeContext *context = (bael_AttributeContext*)
                                  bcemt_ThreadUtil::getSpecific(contextKey);

    if (context) {
        bael_AttributeContext::removeContext(context);
        bcemt_ThreadUtil::setSpecific(contextKey, 0);
    }
}

                        // ---------------------------
                        // class bael_AttributeContext
                        // ---------------------------

namespace {

// Define a thread-local variable, 'g_threadLocalContext', (on supported
// platforms) that will serve as cache for 'bcemt_ThreadUtil::getSpecific'.
// Note that the memory will still be managed by 'bcemt_ThreadUtil' thread
// specific storage.
#ifdef BCES_THREAD_LOCAL_VARIABLE
BCES_THREAD_LOCAL_VARIABLE(bael_AttributeContext*, g_threadLocalContext, 0);
#endif

}

// CLASS MEMBERS
bael_CategoryManager *bael_AttributeContext::s_categoryManager_p = 0;
bslma_Allocator      *bael_AttributeContext::s_globalAllocator_p = 0;

// PRIVATE CREATORS
bael_AttributeContext
::bael_AttributeContext(bslma_Allocator *basicAllocator)
: d_containerList(basicAllocator)
, d_allocator_p(bslma_Default::globalAllocator(basicAllocator))
{
}

bael_AttributeContext::~bael_AttributeContext()
{
}

// PRIVATE CLASS METHODS
const bcemt_ThreadUtil::Key& bael_AttributeContext::contextKey()
{
    static bcemt_ThreadUtil::Key s_contextKey;
    BCEMT_ONCE_DO {
        bcemt_ThreadUtil::createKey(&s_contextKey,
                                    (bcemt_ThreadUtil::Destructor)
                                    bael_AttributeContext::removeContext);
    }
    return s_contextKey;

}

void bael_AttributeContext::removeContext(void *arg)
{
#ifdef BCES_THREAD_LOCAL_VARIABLE
        g_threadLocalContext = 0;
#endif

    bael_AttributeContext *context = (bael_AttributeContext*)arg;
    if (context) {
        bslma_Allocator *allocator = context->d_allocator_p;

        // Note that we can't call 'bslma_Allocator::deleteObject' because the
        // destructor is private.

        context->~bael_AttributeContext();
        allocator->deallocate(context);
    }
}

// CLASS METHODS
void
bael_AttributeContext::initialize(bael_CategoryManager *categoryManager,
                                  bslma_Allocator      *globalAllocator)
{
    if (s_globalAllocator_p) {
        bsl::fprintf(
                 stderr,
                 "Attempt to re-initialize 'bael_AttributeContext'. %s:%d.\n",
                 __FILE__,
                 __LINE__);
        return;                                                       // RETURN
    }
    s_globalAllocator_p = bslma_Default::globalAllocator(globalAllocator);
    s_categoryManager_p = categoryManager;
}

bael_AttributeContext *bael_AttributeContext::lookupContext()
{
#ifdef BCES_THREAD_LOCAL_VARIABLE
    return g_threadLocalContext;
#else
    return (bael_AttributeContext*)
                                bcemt_ThreadUtil::getSpecific(contextKey());
#endif
}

bael_AttributeContext *bael_AttributeContext::getContext()
{
#ifdef BCES_THREAD_LOCAL_VARIABLE
    if (g_threadLocalContext) {
        return g_threadLocalContext;                                  // RETURN
    }

    bslma_Allocator *allocator =
                          bslma_Default::globalAllocator(s_globalAllocator_p);
    bael_AttributeContext *context =
                            new (*allocator) bael_AttributeContext(allocator);
    if (0 != bcemt_ThreadUtil::setSpecific(contextKey(), context)) {
        bsl::fprintf(stderr,
                     "Failed to add 'bael_AttributeContext' to thread "
                     "specific storage. %s : %d\n",
                     __FILE__,
                     __LINE__);
        BSLS_ASSERT(false);
    }
    g_threadLocalContext = context;
    return context;                                                   // RETURN
#else
    const bcemt_ThreadUtil::Key& key = contextKey();

    bael_AttributeContext *context =
        (bael_AttributeContext*)bcemt_ThreadUtil::getSpecific(key);

    if (!context) {
        bslma_Allocator *allocator =
                           bslma_Default::globalAllocator(s_globalAllocator_p);
        context = new (*allocator) bael_AttributeContext(allocator);
        if (0 != bcemt_ThreadUtil::setSpecific(key, context)) {
            bsl::fprintf(stderr,
                         "Failed to add 'bael_AttributeContext' to thread "
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
bool bael_AttributeContext::hasRelevantActiveRules(
                                          const bael_Category *category) const
{
    bael_RuleSet::MaskType relevantRuleMask = category->relevantRuleMask();

    if (!relevantRuleMask) {
        return false;
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
    bcemt_LockGuard<bcemt_Mutex> ruleGuard(
                                        &s_categoryManager_p->rulesetMutex());

    return relevantRuleMask &
           d_ruleCache_p.update(s_categoryManager_p->ruleSequenceNumber(),
                                relevantRuleMask,
                                s_categoryManager_p->ruleSet(),
                                d_containerList);
}

void
bael_AttributeContext::determineThresholdLevels(
                                     bael_ThresholdAggregate *levels,
                                     const bael_Category     *category) const
{
    if (!category) {
        return;                                                       // RETURN
    }

    // Set the default levels for 'category'.
    levels->setLevels(category->recordLevel(),
                      category->passLevel(),
                      category->triggerLevel(),
                      category->triggerAllLevel());

    bael_RuleSet::MaskType relevantRuleMask = category->relevantRuleMask();

    if (!relevantRuleMask) {
        return;
    }

    // test on the cache outside the lock, and return if there are no active
    // rules for 'category'.  We do not lock on the 'rulesetMutex' before
    // testing the cache (see implementation notes at the top).
    bael_RuleSet::MaskType activeAndRelevantRules;
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
    bcemt_LockGuard<bcemt_Mutex> ruleGuard(
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
    int numBits = bdes_BitUtil::sizeInBits(activeAndRelevantRules);
    BSLS_ASSERT(numBits == bael_RuleSet::maxNumRules());

    // Get the index of the relevant rules and store it in 'i'.
    while((i = bdes_BitUtil::find1AtSmallestIndex(activeAndRelevantRules))
                                                                  != numBits) {
        activeAndRelevantRules &= bdes_BitUtil::neMask(i);
        const bael_Rule *rule = s_categoryManager_p->ruleSet().getRuleById(i);
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
bsl::ostream& bael_AttributeContext::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    char EL = (spacesPerLevel < 0) ? ' ' : '\n';

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << EL;

    d_containerList.print(stream, level + 1, spacesPerLevel);

    d_ruleCache_p.print(stream, level + 1, spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << EL;
    return stream << bsl::flush;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
