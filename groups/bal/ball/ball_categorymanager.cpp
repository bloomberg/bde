// ball_categorymanager.cpp               -*-C++-*-
#include <ball_categorymanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_categorymanager_cpp,"$Id$ $CSID$")

#include <ball_severity.h>
#include <ball_thresholdaggregate.h>

#include <bdlqq_lockguard.h>
#include <bdlqq_readlockguard.h>
#include <bdlqq_writelockguard.h>

#include <bdlb_bitutil.h>
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
typedef bsl::vector<ball::Category *> CategoryVector;

namespace ball {    // For convenience, 'CategoryMap' and 'CategoryVector' define types of
    // two 'CategoryManager' data members.

                    // ===========================
                    // class Category_Proctor
                    // ===========================

class Category_Proctor {
    // This class facilitates exception neutrality by proctoring memory
    // management for 'Category' objects.

    // DATA
    Category    *d_category_p;    // category object to delete on failure

    CategoryVector   *d_categories_p;  // category collection to rollback on
                                       // failure

    bslma::Allocator *d_allocator_p;   // allocator for the category object

  public:
    // CREATORS
    Category_Proctor(Category    *category,
                          bslma::Allocator *allocator);
        // Create a proctor to rollback the owned objects to its initial state
        // if any of the steps during the creation and registering of the
        // category object fails.  The owned objects are: the category object
        // and the collection of categories.

    ~Category_Proctor();
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
Category_Proctor::Category_Proctor(Category    *category,
                                             bslma::Allocator *allocator)
: d_category_p(category)
, d_categories_p(0)
, d_allocator_p(allocator)
{
}

inline
Category_Proctor::~Category_Proctor()
{
    if (d_category_p) {
        d_category_p->~Category();
        d_allocator_p->deallocate(d_category_p);
    }

    if (d_categories_p) {
        d_categories_p->pop_back();
    }
}

// MANIPULATORS
inline
void Category_Proctor::setCategories(CategoryVector *categories)
{
    d_categories_p = categories;
}

inline
void Category_Proctor::release()
{
    d_category_p   = 0;
    d_categories_p = 0;
}

                            // -------------------
                            // class Category
                            // -------------------

// PRIVATE CREATORS
Category::Category(const char       *categoryName,
                             int               recordLevel,
                             int               passLevel,
                             int               triggerLevel,
                             int               triggerAllLevel,
                             bslma::Allocator *basicAllocator)
    // Note that this constructor is private, so the validation of the
    // threshold level values does not need to be repeated here.  They are
    // validated in 'CategoryManager::addCategory', prior to creating an
    // instance of this class.
: d_thresholdLevels(recordLevel, passLevel, triggerLevel, triggerAllLevel)
, d_threshold(ThresholdAggregate::maxLevel(recordLevel,
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

Category::~Category()
{
}

// PRIVATE MANIPULATORS
void
Category::linkCategoryHolder(CategoryHolder *categoryHolder)
{
    BSLS_ASSERT(categoryHolder);

    if (!categoryHolder->category()) {
        categoryHolder->setThreshold(bsl::max(d_threshold, d_ruleThreshold));
        categoryHolder->setCategory(this);
        categoryHolder->setNext(d_categoryHolder);
        d_categoryHolder = categoryHolder;
    }
}

void Category::resetCategoryHolders()
{
    CategoryHolder *holder = d_categoryHolder;
    while (holder) {
        CategoryHolder *nextHolder = holder->next();
        holder->reset();
        holder = nextHolder;
    }
    d_categoryHolder = 0;
}

// CLASS METHODS
bool Category::areValidThresholdLevels(int recordLevel,
                                            int passLevel,
                                            int triggerLevel,
                                            int triggerAllLevel)
{
    enum { BITS_PER_CHAR = 8 };

    return !((recordLevel | passLevel | triggerLevel | triggerAllLevel)
             >> BITS_PER_CHAR);
}

void Category::updateThresholdForHolders()
{
    if (d_categoryHolder) {
        CategoryHolder *holder = d_categoryHolder;
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
int Category::setLevels(int recordLevel,
                             int passLevel,
                             int triggerLevel,
                             int triggerAllLevel)
{
    if (Category::areValidThresholdLevels(recordLevel,
                                               passLevel,
                                               triggerLevel,
                                               triggerAllLevel)) {

        d_thresholdLevels.setLevels(recordLevel,
                                    passLevel,
                                    triggerLevel,
                                    triggerAllLevel);

        d_threshold = ThresholdAggregate::maxLevel(recordLevel,
                                                        passLevel,
                                                        triggerLevel,
                                                        triggerAllLevel);

        updateThresholdForHolders();
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
bool Category::isEnabled(int level) const
{
    return d_threshold >= level;
}

                        // -------------------------
                        // class CategoryHolder
                        // -------------------------

// MANIPULATORS
void CategoryHolder::reset()
{
    d_threshold  = BAEL_UNINITIALIZED_CATEGORY;
    d_category_p = 0;
    d_next_p     = 0;
}

                    // --------------------------
                    // class CategoryManager
                    // --------------------------

// PRIVATE MANIPULATORS
Category *CategoryManager::addNewCategory(
                                                   const char *categoryName,
                                                   int         recordLevel,
                                                   int         passLevel,
                                                   int         triggerLevel,
                                                   int         triggerAllLevel)
{
    // Create a new category and add it to the collection of categories
    // and the category registry.

    Category *category =
                            new (*d_allocator_p) Category(categoryName,
                                                               recordLevel,
                                                               passLevel,
                                                               triggerLevel,
                                                               triggerAllLevel,
                                                               d_allocator_p);
    // rollback on failure
    Category_Proctor proctor(category, d_allocator_p);

    d_categories.push_back(category);
    proctor.setCategories(&d_categories);

    d_registry[category->categoryName()] = d_categories.size() - 1;
    proctor.release();

    return category;
}

// CREATORS
CategoryManager::~CategoryManager()
{
    BSLS_ASSERT(d_allocator_p);

    for (int i = 0; i < length(); ++i) {
        d_categories[i]->~Category();
        d_allocator_p->deallocate(d_categories[i]);
    }
}

// MANIPULATORS
Category *CategoryManager::addCategory(const char *categoryName,
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

Category *CategoryManager::addCategory(
                                         CategoryHolder *categoryHolder,
                                         const char          *categoryName,
                                         int                  recordLevel,
                                         int                  passLevel,
                                         int                  triggerLevel,
                                         int                  triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bdlqq::WriteLockGuard<bdlqq::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);

    if (iter != d_registry.end()) {
        return 0;
    }
    else {
        Category *category = addNewCategory(categoryName,
                                                      recordLevel,
                                                      passLevel,
                                                      triggerLevel,
                                                      triggerAllLevel);
        if (categoryHolder) {
            category->linkCategoryHolder(categoryHolder);
        }

        registryGuard.release()->unlock();

        bdlqq::LockGuard<bdlqq::Mutex> ruleSetGuard(&d_ruleSetMutex);

        for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
            const Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                category->d_relevantRuleMask |= 1 << i;
                int threshold = ThresholdAggregate::maxLevel(
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

Category *CategoryManager::lookupCategory(const char *categoryName)
{
    bdlqq::ReadLockGuard<bdlqq::ReaderWriterLock> registryGuard(&d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}

Category *CategoryManager::lookupCategory(
                                           CategoryHolder *categoryHolder,
                                           const char          *categoryName)
{
    d_registryLock.lockReadReserveWrite();
    bdlqq::WriteLockGuard<bdlqq::ReaderWriterLock>
                                             registryGuard(&d_registryLock, 1);
    Category *category = 0;
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

void CategoryManager::resetCategoryHolders()
{
    // Intentionally not locking.  This method should only be called just prior
    // to destroying the category manager.

    const int numCategories = length();
    for (int i = 0; i < numCategories; ++i) {
        d_categories[i]->resetCategoryHolders();
    }
}

Category *CategoryManager::setThresholdLevels(
                                                   const char *categoryName,
                                                   int         recordLevel,
                                                   int         passLevel,
                                                   int         triggerLevel,
                                                   int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    d_registryLock.lockReadReserveWrite();
    bdlqq::WriteLockGuard<bdlqq::ReaderWriterLock> registryGuard(&d_registryLock,
                                                               1);
    CategoryMap::iterator iter = d_registry.find(categoryName);
    if (iter != d_registry.end()) {
        Category *category = d_categories[iter->second];
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);

        return category;
    }
    else {
        d_registryLock.upgradeToWriteLock();

        Category *category = addNewCategory(categoryName,
                                                      recordLevel,
                                                      passLevel,
                                                      triggerLevel,
                                                      triggerAllLevel);
        registryGuard.release();
        d_registryLock.unlock();
        bdlqq::LockGuard<bdlqq::Mutex> ruleSetGuard(&d_ruleSetMutex);

        for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
            const Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                category->d_relevantRuleMask |= 1 << i;
                int threshold = ThresholdAggregate::maxLevel(
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

int CategoryManager::addRule(const Rule& value)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.addRule(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            category->d_relevantRuleMask |= 1 << ruleId;
            int threshold = ThresholdAggregate::maxLevel(
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

int CategoryManager::addRules(const RuleSet& ruleSet)
{
    int count = 0;
    for (int i = 0; i < ruleSet.maxNumRules(); ++i) {
        const Rule *rule = ruleSet.getRuleById(i);
        if (rule) {
            count += addRule(*rule);
        }
    }
    return count;
}

int CategoryManager::removeRule(const Rule& value)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.ruleId(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            category->d_relevantRuleMask = bdlb::BitUtil::withBitCleared(
                                                  category->d_relevantRuleMask,
                                                  ruleId);
            category->d_ruleThreshold = 0;

            RuleSet::MaskType relevantRuleMask =
                                                  category->d_relevantRuleMask;
            int j = 0;
            int numBits = bdlb::BitUtil::sizeInBits(relevantRuleMask);
            BSLS_ASSERT(numBits == RuleSet::maxNumRules());
            while((j = bdlb::BitUtil::numTrailingUnsetBits(relevantRuleMask))
                                                                  != numBits) {
                relevantRuleMask &= ~(1 << j);

                const Rule *r = d_ruleSet.getRuleById(j);
                int threshold = ThresholdAggregate::maxLevel(
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

int CategoryManager::removeRules(const RuleSet& ruleSet)
{
    int count = 0;
    for (int i = 0; i < ruleSet.maxNumRules(); ++i) {
        const Rule *rule = ruleSet.getRuleById(i);
        if (rule) {
            count += removeRule(*rule);
        }
    }
    return count;
}

void CategoryManager::removeAllRules()
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_ruleSetMutex);
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
const Category *CategoryManager::lookupCategory(
                                                const char *categoryName) const
{
    bdlqq::ReadLockGuard<bdlqq::ReaderWriterLock> registryGuard(&d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
