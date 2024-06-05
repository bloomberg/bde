// ball_categorymanager.cpp                                           -*-C++-*-
#include <ball_categorymanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_categorymanager_cpp,"$Id$ $CSID$")

#include <ball_severity.h>
#include <ball_thresholdaggregate.h>

#include <bslmt_mutexassert.h>

#include <bdlb_bitutil.h>

#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
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
namespace ball {

namespace {

typedef bsls::AtomicOperations AtomicOps;

static
AtomicOps::AtomicTypes::Int64 categoryManagerSequenceNumber = { -1 };
    // This object is used for assigning a unique initial rule set sequence
    // number to each category manager that is created.

                    // =====================
                    // class CategoryProctor
                    // =====================

class CategoryProctor {
    // This class facilitates exception neutrality by proctoring memory
    // management for 'Category' objects.

    // PRIVATE TYPES
    typedef bsl::vector<ball::Category *> CategoryVector;

    // DATA
    Category         *d_category_p;    // category object to delete on failure

    CategoryVector   *d_categories_p;  // category collection to rollback on
                                       // failure

    bslma::Allocator *d_allocator_p;   // allocator for the category object

  private:
    // NOT IMPLEMENTED
    CategoryProctor(const CategoryProctor&);
    CategoryProctor& operator=(const CategoryProctor&);

  public:
    // CREATORS
    CategoryProctor(Category *category, bslma::Allocator *allocator);
        // Create a proctor to manage the specified 'category' object,
        // allocated with the specified 'allocator'.  On this proctor's
        // destruction, unless release has been called, the 'category' will be
        // destroyed and its footprint deallocated.

    ~CategoryProctor();
        // Rollback the owned objects to their initial state on failure.

    // MANIPULATORS

    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

    void setCategories(CategoryVector *categories);
        // Take ownership of the specified 'categories' object to roll it back
        // on failure.

    void release();
        // Release the ownership of all objects currently managed by this
        // proctor.

    // BDE_VERIFY pragma: pop
};

                        // ---------------------
                        // class CategoryProctor
                        // ---------------------

// CREATORS
inline
CategoryProctor::CategoryProctor(Category         *category,
                                 bslma::Allocator *allocator)
: d_category_p(category)
, d_categories_p(0)
, d_allocator_p(allocator)
{
}

inline
CategoryProctor::~CategoryProctor()
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

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

inline
void CategoryProctor::setCategories(CategoryVector *categories)
{
    d_categories_p = categories;
}

inline
void CategoryProctor::release()
{
    d_category_p   = 0;
    d_categories_p = 0;
}

// BDE_VERIFY pragma: pop

}  // close unnamed namespace

                    // ---------------------
                    // class CategoryManager
                    // ---------------------

// PRIVATE MANIPULATORS
Category *CategoryManager::addNewCategory(const char *categoryName,
                                          int         recordLevel,
                                          int         passLevel,
                                          int         triggerLevel,
                                          int         triggerAllLevel)
{
    // Create a new category and add it to the category registry.

    Category *category = new (*d_allocator_p) Category(categoryName,
                                                       recordLevel,
                                                       passLevel,
                                                       triggerLevel,
                                                       triggerAllLevel,
                                                       d_allocator_p);

    CategoryProctor proctor(category, d_allocator_p);  // rollback on exception

    d_categories.push_back(category);
    proctor.setCategories(&d_categories);

    d_registry[category->categoryName()] =
                                     static_cast<int>(d_categories.size() - 1);
    proctor.release();

    return category;
}

void CategoryManager::privateApplyRulesToCategory(Category* category)
{
    RuleSet::MaskType mask      = 0;
    int               threshold = 0;
    for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
        const Rule *rule = d_ruleSet.getRuleById(i);
        if (rule && rule->isMatch(category->categoryName())) {
            mask = bdlb::BitUtil::withBitSet(mask, i);
            threshold = bsl::max(threshold,
                                 ThresholdAggregate::maxLevel(
                                         rule->recordLevel(),
                                         rule->passLevel(),
                                         rule->triggerLevel(),
                                         rule->triggerAllLevel()));
        }
    }
    CategoryManagerImpUtil::setRelevantRuleMask(category, mask);
    if (threshold != category->ruleThreshold()) {
        CategoryManagerImpUtil::setRuleThreshold(category, threshold);
        CategoryManagerImpUtil::updateThresholdForHolders(category);
    }
}

void CategoryManager::privateApplyRulesToAllCategories(
                                     bslmt::LockGuard<bslmt::Mutex>& ruleGuard)
{
    typedef bsls::Types::Int64 Int64;
    BSLMT_MUTEXASSERT_IS_LOCKED(&d_ruleSetMutex);
    BSLS_ASSERT(ruleGuard.ptr() == &d_ruleSetMutex);
    bsls::Types::Int64 localRulesetSequenceNumber = ++d_ruleSetSequenceNumber;
    ruleGuard.release()->unlock();

    const Int64 k_BATCH_SIZE = 4096;
    Int64       offset       = 0;
    Int64       batch;
    bool        done         = false;

    bsl::vector<Category *> cachedCategories(d_allocator_p);
    while (!done) {
        cachedCategories.clear();
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(
                                                              &d_registryLock);
            Int64 remaining = d_categories.size() - offset;
            batch = std::min(k_BATCH_SIZE, remaining);
            done = (remaining == batch);
            cachedCategories.assign(d_categories.begin() + offset,
                                    d_categories.begin() + offset + batch);
        }
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);
            if (localRulesetSequenceNumber != d_ruleSetSequenceNumber) {
                // Another thread is applying newer rules.
                return;                                               // RETURN
            }
            for (bsl::vector<Category *>::iterator iter =
                                               cachedCategories.begin();
                                               iter != cachedCategories.end();
                                               ++iter) {
                privateApplyRulesToCategory(*iter);
            }
        }
    }
}

// CREATORS
CategoryManager::CategoryManager(bslma::Allocator *basicAllocator)
: d_registry(1, bdlb::CStringHash(), bdlb::CStringEqualTo(), basicAllocator)
, d_ruleSetSequenceNumber(
      AtomicOps::incrementInt64Nv(&categoryManagerSequenceNumber) << 48)
, d_ruleSet(bslma::Default::allocator(basicAllocator))
, d_categories(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CategoryManager::~CategoryManager()
{
    BSLS_ASSERT(d_allocator_p);

    for (int i = 0; i < length(); ++i) {
        d_categories[i]->~Category();
        d_allocator_p->deallocate(d_categories[i]);
    }
}

// MANIPULATORS

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01: Functions not in alphanumeric order

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

Category *CategoryManager::addCategory(CategoryHolder *categoryHolder,
                                       const char     *categoryName,
                                       int             recordLevel,
                                       int             passLevel,
                                       int             triggerLevel,
                                       int             triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    CategoryMap::const_iterator iter = d_registry.find(categoryName);

    if (iter != d_registry.end()) {
        return 0;                                                     // RETURN
    }
    else {
        Category *category = addNewCategory(categoryName,
                                            recordLevel,
                                            passLevel,
                                            triggerLevel,
                                            triggerAllLevel);
        if (categoryHolder) {
            CategoryManagerImpUtil::linkCategoryHolder(category,
                                                       categoryHolder);
        }

        registryGuard.release()->unlock();

        bslmt::LockGuard<bslmt::Mutex> ruleSetGuard(&d_ruleSetMutex);

        privateApplyRulesToCategory(category);

        // We have a write lock on 'd_registryLock', so the supplied category
        // holder is the only holder for the created category.

        // Despite the above comment, no such lock is held.
        if (categoryHolder) {
            categoryHolder->setThreshold(bsl::max(category->threshold(),
                                                  category->ruleThreshold()));
        }

        return category;                                              // RETURN
    }
}

Category *CategoryManager::lookupCategory(const char *categoryName)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}

Category *CategoryManager::lookupCategory(CategoryHolder *categoryHolder,
                                          const char     *categoryName)
{
    d_registryLock.lockReadReserveWrite();

    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                           &d_registryLock, 1);

    Category                    *category = 0;
    CategoryMap::const_iterator  iter = d_registry.find(categoryName);
    if (iter != d_registry.end()) {
        category = d_categories[iter->second];
        if (categoryHolder && !categoryHolder->category()) {
            d_registryLock.upgradeToWriteLock();
            CategoryManagerImpUtil::linkCategoryHolder(category,
                                                       categoryHolder);
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
        CategoryManagerImpUtil::resetCategoryHolders(d_categories[i]);
    }
}

Category *CategoryManager::setThresholdLevels(const char *categoryName,
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
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                           &d_registryLock, 1);

    CategoryMap::iterator iter = d_registry.find(categoryName);
    if (iter != d_registry.end()) {
        Category *category = d_categories[iter->second];
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);

        return category;                                              // RETURN
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
        bslmt::LockGuard<bslmt::Mutex> ruleSetGuard(&d_ruleSetMutex);

        privateApplyRulesToCategory(category);

        return category;                                              // RETURN
    }
}

int CategoryManager::addRule(const Rule& ruleToAdd)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    const int ruleId = d_ruleSet.addRule(ruleToAdd);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    privateApplyRulesToAllCategories(guard);

    return 1;
}

int CategoryManager::addRules(const RuleSet& ruleSet)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    int count = d_ruleSet.addRules(ruleSet);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

int CategoryManager::removeRule(const Rule& ruleToRemove)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    int count = d_ruleSet.removeRule(ruleToRemove);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

int CategoryManager::removeRules(const RuleSet& ruleSet)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    int count = d_ruleSet.removeRules(ruleSet);

    if (count) {
        privateApplyRulesToAllCategories(guard);
    }

    return count;
}

void CategoryManager::removeAllRules()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);

    int count = d_ruleSet.numRules();

    if (count) {
        d_ruleSet.removeAllRules();
        privateApplyRulesToAllCategories(guard);
    }
}

// BDE_VERIFY pragma: pop

// ACCESSORS
const Category *CategoryManager::lookupCategory(const char *categoryName) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(
                                                              &d_registryLock);

    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
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
