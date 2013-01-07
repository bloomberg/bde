// bael_categorymanager.cpp               -*-C++-*-
#include <bael_categorymanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_categorymanager_cpp,"$Id$ $CSID$")

#include <bael_severity.h>
#include <bael_thresholdaggregate.h>

#include <bcemt_lockguard.h>
#include <bcemt_readlockguard.h>
#include <bcemt_writelockguard.h>

#include <bdes_bitutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_map.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

// Note: on Windows -> WinDef.h:#define max(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(max)
#undef max
#endif

namespace BloombergLP {

typedef bsl::map<const char *, int>  CategoryMap;
typedef bsl::vector<bael_Category *> CategoryVector;
    // For convenience, 'CategoryMap' and 'CategoryVector' define types of
    // two 'bael_CategoryManager' data members.

                    // ===========================
                    // class bael_Category_Proctor
                    // ===========================

class bael_Category_Proctor {
    // This class facilitates exception neutrality by proctoring memory
    // management for 'bael_Category' objects.

    // DATA
    bael_Category   *d_category_p;    // category object to delete on failure

    CategoryVector  *d_categories_p;  // category collection to rollback on
                                      // failure

    bslma_Allocator *d_allocator_p;   // allocator for the category object

  public:
    // CREATORS
    bael_Category_Proctor(bael_Category *category, bslma_Allocator *allocator);
        // Create a proctor to rollback the owned objects to its initial state
        // if any of the steps during the creation and registering of the
        // category object fails.  The owned objects are: the category object
        // and the collection of categories.

    ~bael_Category_Proctor();
        // Rollback the owned objects to their initial state on failure.

    // MANIPULATORS
    void setCategories(CategoryVector *categories);
        // Take ownership of the 'categories' object to roll it back on
        // failure.

    void release();
        // Release the ownership of all objects currently managed by this
        // proctor.
};

// CREATORS
inline
bael_Category_Proctor::bael_Category_Proctor(bael_Category   *category,
                                             bslma_Allocator *allocator)
: d_category_p(category)
, d_categories_p(0)
, d_allocator_p(allocator)
{
}

inline
bael_Category_Proctor::~bael_Category_Proctor()
{
    if (d_category_p) {
        d_category_p->~bael_Category();
        d_allocator_p->deallocate(d_category_p);
    }

    if (d_categories_p) {
        d_categories_p->pop_back();
    }
}

// MANIPULATORS
inline
void bael_Category_Proctor::setCategories(CategoryVector *categories)
{
    d_categories_p = categories;
}

inline
void bael_Category_Proctor::release()
{
    d_category_p   = 0;
    d_categories_p = 0;
}

                            // -------------------
                            // class bael_Category
                            // -------------------

// PRIVATE CREATORS
bael_Category::bael_Category(const char      *categoryName,
                             int              recordLevel,
                             int              passLevel,
                             int              triggerLevel,
                             int              triggerAllLevel,
                             bslma_Allocator *basicAllocator)
    // Note that this constructor is private, so the validation of the
    // threshold level values does not need to be repeated here.  They are
    // validated in 'bael_CategoryManager::addCategory', prior to creating an
    // instance of this class.
: d_thresholdLevels(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_threshold(bael_ThresholdAggregate::maxLevel(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel))
, d_categoryName(categoryName, basicAllocator)
, d_categoryHolder(0)
, d_relevantRuleMask(0)
, d_ruleThreshold(0)
{
    BSLS_ASSERT(categoryName);
}

bael_Category::~bael_Category()
{
}

// PRIVATE MANIPULATORS
void
bael_Category::linkCategoryHolder(bael_CategoryHolder *categoryHolder)
{
    BSLS_ASSERT(categoryHolder);

    if (!categoryHolder->category()) {
        categoryHolder->setThreshold(bsl::max(d_threshold, d_ruleThreshold));
        categoryHolder->setCategory(this);
        categoryHolder->setNext(d_categoryHolder);
        d_categoryHolder = categoryHolder;
    }
}

void bael_Category::resetCategoryHolders()
{
    bael_CategoryHolder *holder = d_categoryHolder;
    while (holder) {
        bael_CategoryHolder *nextHolder = holder->next();
        holder->reset();
        holder = nextHolder;
    }
    d_categoryHolder = 0;
}

// CLASS METHODS
bool bael_Category::areValidThresholdLevels(int recordLevel,
                                            int passLevel,
                                            int triggerLevel,
                                            int triggerAllLevel)
{
    enum { BITS_PER_CHAR = 8 };

    return !((recordLevel | passLevel | triggerLevel | triggerAllLevel)
             >> BITS_PER_CHAR);
}

void bael_Category::updateThresholdForHolders()
{
    if (d_categoryHolder) {
        bael_CategoryHolder *holder = d_categoryHolder;
        const int threshold = bsl::max(d_threshold, d_ruleThreshold);
        if (threshold != holder->threshold()) {
            do {
                holder->setThreshold(threshold);
                holder = holder->next();
            } while (holder);
        }
    }
}

// MANIPULATORS
int bael_Category::setLevels(int recordLevel,
                             int passLevel,
                             int triggerLevel,
                             int triggerAllLevel)
{
    if (bael_Category::areValidThresholdLevels(recordLevel,
                                               passLevel,
                                               triggerLevel,
                                               triggerAllLevel)) {

        d_thresholdLevels.setLevels(recordLevel,
                                    passLevel,
                                    triggerLevel,
                                    triggerAllLevel);

        d_threshold = bael_ThresholdAggregate::maxLevel(recordLevel,
                                                        passLevel,
                                                        triggerLevel,
                                                        triggerAllLevel);

        updateThresholdForHolders();
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
bool bael_Category::isEnabled(int level) const
{
    return d_threshold >= level;
}

                        // -------------------------
                        // class bael_CategoryHolder
                        // -------------------------

// MANIPULATORS
void bael_CategoryHolder::reset()
{
    d_threshold  = BAEL_UNINITIALIZED_CATEGORY;
    d_category_p = 0;
    d_next_p     = 0;
}

                    // --------------------------
                    // class bael_CategoryManager
                    // --------------------------

// PRIVATE MANIPULATORS
bael_Category *bael_CategoryManager::addNewCategory(
                                                   const char *categoryName,
                                                   int         recordLevel,
                                                   int         passLevel,
                                                   int         triggerLevel,
                                                   int         triggerAllLevel)
{
    // Create a new category and add it to the collection of categories
    // and the category registry.

    bael_Category *category =
                            new (*d_allocator_p) bael_Category(categoryName,
                                                               recordLevel,
                                                               passLevel,
                                                               triggerLevel,
                                                               triggerAllLevel,
                                                               d_allocator_p);
    // rollback on failure
    bael_Category_Proctor proctor(category, d_allocator_p);

    d_categories.push_back(category);
    proctor.setCategories(&d_categories);

    d_registry[category->categoryName()] = d_categories.size() - 1;
    proctor.release();

    return category;
}

// CREATORS
bael_CategoryManager::~bael_CategoryManager()
{
    BSLS_ASSERT(d_allocator_p);

    for (int i = 0; i < length(); ++i) {
        d_categories[i]->~bael_Category();
        d_allocator_p->deallocate(d_categories[i]);
    }
}

// MANIPULATORS
bael_Category *bael_CategoryManager::addCategory(const char *categoryName,
                                                 int         recordLevel,
                                                 int         passLevel,
                                                 int         triggerLevel,
                                                 int         triggerAllLevel)
{
    return addCategory(0,
                       categoryName,
                       recordLevel,
                       passLevel,
                       triggerLevel,
                       triggerAllLevel);
}

bael_Category *bael_CategoryManager::addCategory(
                                         bael_CategoryHolder *categoryHolder,
                                         const char          *categoryName,
                                         int                  recordLevel,
                                         int                  passLevel,
                                         int                  triggerLevel,
                                         int                  triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_ReaderWriterLock> registryGuard(
                                                              &d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);

    if (iter != d_registry.end()) {
        return 0;
    }
    else {
        bael_Category *category = addNewCategory(categoryName,
                                                      recordLevel,
                                                      passLevel,
                                                      triggerLevel,
                                                      triggerAllLevel);
        if (categoryHolder) {
            category->linkCategoryHolder(categoryHolder);
        }

        registryGuard.release()->unlock();

        bcemt_LockGuard<bcemt_Mutex> ruleSetGuard(&d_ruleSetMutex);

        for (int i = 0; i < bael_RuleSet::maxNumRules(); ++i) {
            const bael_Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                category->d_relevantRuleMask |= bdes_BitUtil::eqMask(i);
                int threshold = bael_ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
                if (threshold > category->d_ruleThreshold) {
                    category->d_ruleThreshold = threshold;
                }
            }
        }
        // We have a 'writeLock' on 'd_registryLock' so the supplied category
        // holder is the only category holder for the created category.
        if (categoryHolder) {
            categoryHolder->setThreshold(bsl::max(category->d_threshold,
                                                  category->d_ruleThreshold));
        }

        return category;
    }
}

bael_Category *bael_CategoryManager::lookupCategory(const char *categoryName)
{
    bcemt_ReadLockGuard<bcemt_ReaderWriterLock> registryGuard(&d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}

bael_Category *bael_CategoryManager::lookupCategory(
                                           bael_CategoryHolder *categoryHolder,
                                           const char          *categoryName)
{
    d_registryLock.lockReadReserveWrite();
    bcemt_WriteLockGuard<bcemt_ReaderWriterLock>
                                             registryGuard(&d_registryLock, 1);
    bael_Category *category = 0;
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    if (iter != d_registry.end()) {
        category = d_categories[iter->second];
        if (categoryHolder && !categoryHolder->category()) {
            d_registryLock.upgradeToWriteLock();
            category->linkCategoryHolder(categoryHolder);
        }
    }

    return category;
}

void bael_CategoryManager::resetCategoryHolders()
{
    // Intentionally not locking.  This method should only be called just prior
    // to destroying the category manager.

    const int numCategories = length();
    for (int i = 0; i < numCategories; ++i) {
        d_categories[i]->resetCategoryHolders();
    }
}

bael_Category *bael_CategoryManager::setThresholdLevels(
                                                   const char *categoryName,
                                                   int         recordLevel,
                                                   int         passLevel,
                                                   int         triggerLevel,
                                                   int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    d_registryLock.lockReadReserveWrite();
    bcemt_WriteLockGuard<bcemt_ReaderWriterLock> registryGuard(&d_registryLock,
                                                               1);
    CategoryMap::iterator iter = d_registry.find(categoryName);
    if (iter != d_registry.end()) {
        bael_Category *category = d_categories[iter->second];
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);

        return category;
    }
    else {
        d_registryLock.upgradeToWriteLock();

        bael_Category *category = addNewCategory(categoryName,
                                                      recordLevel,
                                                      passLevel,
                                                      triggerLevel,
                                                      triggerAllLevel);
        registryGuard.release();
        d_registryLock.unlock();
        bcemt_LockGuard<bcemt_Mutex> ruleSetGuard(&d_ruleSetMutex);

        for (int i = 0; i < bael_RuleSet::maxNumRules(); ++i) {
            const bael_Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                category->d_relevantRuleMask |= bdes_BitUtil::eqMask(i);
                int threshold = bael_ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
                if (threshold > category->d_ruleThreshold) {
                    category->d_ruleThreshold = threshold;
                }
            }
        }

        // No need to update holders since the category was just newly created
        // and thus does not have any linked holders.

        return category;
    }
}

int bael_CategoryManager::addRule(const bael_Rule& value)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.addRule(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const bael_Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        bael_Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            category->d_relevantRuleMask |= bdes_BitUtil::eqMask(ruleId);
            int threshold = bael_ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
            if (threshold > category->d_ruleThreshold) {
                category->d_ruleThreshold = threshold;
                category->updateThresholdForHolders();
            }
        }
    }
    return 1;
}

int bael_CategoryManager::addRules(const bael_RuleSet& ruleSet)
{
    int count = 0;
    for (int i = 0; i < ruleSet.maxNumRules(); ++i) {
        const bael_Rule *rule = ruleSet.getRuleById(i);
        if (rule) {
            count += addRule(*rule);
        }
    }
    return count;
}

int bael_CategoryManager::removeRule(const bael_Rule& value)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.ruleId(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const bael_Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        bael_Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            category->d_relevantRuleMask = bdes_BitUtil::setBitZero(
                                                  category->d_relevantRuleMask,
                                                  ruleId);
            category->d_ruleThreshold = 0;

            bael_RuleSet::MaskType relevantRuleMask =
                                                  category->d_relevantRuleMask;
            int j = 0;
            int numBits = bdes_BitUtil::sizeInBits(relevantRuleMask);
            BSLS_ASSERT(numBits == bael_RuleSet::maxNumRules());
            while((j = bdes_BitUtil::find1AtSmallestIndex(relevantRuleMask))
                                                                  != numBits) {
                relevantRuleMask &= bdes_BitUtil::neMask(j);

                const bael_Rule *r = d_ruleSet.getRuleById(j);
                int threshold = bael_ThresholdAggregate::maxLevel(
                                                         r->recordLevel(),
                                                         r->passLevel(),
                                                         r->triggerLevel(),
                                                         r->triggerAllLevel());
                if (threshold > category->d_ruleThreshold) {
                    category->d_ruleThreshold = threshold;
                }
            }
            category->updateThresholdForHolders();
        }
    }

    d_ruleSet.removeRuleById(ruleId);
    return 1;
}

int bael_CategoryManager::removeRules(const bael_RuleSet& ruleSet)
{
    int count = 0;
    for (int i = 0; i < ruleSet.maxNumRules(); ++i) {
        const bael_Rule *rule = ruleSet.getRuleById(i);
        if (rule) {
            count += removeRule(*rule);
        }
    }
    return count;
}

void bael_CategoryManager::removeAllRules()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_ruleSetMutex);
    ++d_ruleSequenceNum;
    for (int i = 0; i < length(); ++i) {
        if (d_categories[i]->d_relevantRuleMask) {
            d_categories[i]->d_relevantRuleMask = 0;
            d_categories[i]->d_ruleThreshold    = 0;
            d_categories[i]->updateThresholdForHolders();
        }
    }
    d_ruleSet.removeAllRules();
}

// ACCESSORS
const bael_Category *bael_CategoryManager::lookupCategory(
                                                const char *categoryName) const
{
    bcemt_ReadLockGuard<bcemt_ReaderWriterLock> registryGuard(&d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
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
