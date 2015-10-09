// ball_categorymanager.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_categorymanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_categorymanager_cpp,"$Id$ $CSID$")

#include <ball_severity.h>
#include <ball_thresholdaggregate.h>

#include <bslmt_lockguard.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

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

namespace ball {

namespace {
                    // =====================
                    // class CategoryProctor
                    // =====================

class CategoryProctor {
    // This class facilitates exception neutrality by proctoring memory
    // management for 'Category' objects.
    //
    // This class should *not* be used directly by client code.  It is an
    // implementation detail of the 'ball' logging system.

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
    CategoryProctor(Category         *category,
                    bslma::Allocator *allocator);
        // Create a proctor to manage the specified  'category'  object,
        // allocated with the specified 'allocator'.  On this proctor's
        // destruction, unless release has been called, the 'category' will be
        // destroyed and its footprint deallocated.

    ~CategoryProctor();
        // Rollback the owned objects to their initial state on failure.

    // MANIPULATORS
    void setCategories(CategoryVector *categories);
        // Take ownership of the 'categories' object to roll it back on
        // failure.

    void release();
        // Release the ownership of all objects currently managed by this
        // proctor.
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

} // close unnamed namespace

// For convenience, 'CategoryMap' and 'CategoryVector' define types of
// two 'CategoryManager' data members.

                    // --------------------------
                    // class CategoryManager
                    // --------------------------

// PRIVATE MANIPULATORS
Category *CategoryManager::addNewCategory(const char *categoryName,
                                          int         recordLevel,
                                          int         passLevel,
                                          int         triggerLevel,
                                          int         triggerAllLevel)
{
    // Create a new category and add it to the collection of categories
    // and the category registry.

    Category *category = new (*d_allocator_p) Category(categoryName,
                                                       recordLevel,
                                                       passLevel,
                                                       triggerLevel,
                                                       triggerAllLevel,
                                                       d_allocator_p);
    // rollback on failure
    CategoryProctor proctor(category, d_allocator_p);

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

        for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
            const Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                CategoryManagerImpUtil::enableRule(category, i);
                int threshold = ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
                if (threshold > category->ruleThreshold()) {
                    CategoryManagerImpUtil::setRuleThreshold(category,
                                                             threshold);
                }
            }
        }
        // We have a 'writeLock' on 'd_registryLock' so the supplied category
        // holder is the only category holder for the created category.
        if (categoryHolder) {
            categoryHolder->setThreshold(bsl::max(category->threshold(),
                                                  category->ruleThreshold()));
        }

        return category;                                              // RETURN
    }
}

Category *CategoryManager::lookupCategory(const char *categoryName)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(&d_registryLock);
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
    return iter != d_registry.end() ? d_categories[iter->second] : 0;
}

Category *CategoryManager::lookupCategory(
                                           CategoryHolder *categoryHolder,
                                           const char          *categoryName)
{
    d_registryLock.lockReadReserveWrite();
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock>
                                             registryGuard(&d_registryLock, 1);
    Category *category = 0;
    CategoryMap::const_iterator iter = d_registry.find(categoryName);
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
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> registryGuard(&d_registryLock,
                                                               1);
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

        for (int i = 0; i < RuleSet::maxNumRules(); ++i) {
            const Rule *rule = d_ruleSet.getRuleById(i);
            if (rule && rule->isMatch(category->categoryName())) {
                CategoryManagerImpUtil::enableRule(category, i);
                int threshold = ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
                if (threshold > category->ruleThreshold()) {
                    CategoryManagerImpUtil::setRuleThreshold(category,
                                                             threshold);
                }
            }
        }

        // No need to update holders since the category was just newly created
        // and thus does not have any linked holders.

        return category;                                              // RETURN
    }
}

int CategoryManager::addRule(const Rule& value)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.addRule(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            CategoryManagerImpUtil::enableRule(category, ruleId);
            int threshold = ThresholdAggregate::maxLevel(
                                                      rule->recordLevel(),
                                                      rule->passLevel(),
                                                      rule->triggerLevel(),
                                                      rule->triggerAllLevel());
            if (threshold > category->ruleThreshold()) {
                CategoryManagerImpUtil::setRuleThreshold(category, threshold);
                CategoryManagerImpUtil::updateThresholdForHolders(category);
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
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);
    int ruleId = d_ruleSet.ruleId(value);
    if (ruleId < 0) {
        return 0;                                                     // RETURN
    }

    ++d_ruleSequenceNum;

    const Rule *rule = d_ruleSet.getRuleById(ruleId);

    for (int i = 0; i < length(); ++i) {
        Category *category = d_categories[i];
        if (rule->isMatch(category->categoryName())) {
            CategoryManagerImpUtil::disableRule(category, ruleId);
            CategoryManagerImpUtil::setRuleThreshold(category, 0);

            RuleSet::MaskType relevantRuleMask = category->relevantRuleMask();

            int j = 0;
            int numBits = bdlb::BitUtil::sizeInBits(relevantRuleMask);
            BSLS_ASSERT(numBits == RuleSet::maxNumRules());
            while((j = bdlb::BitUtil::numTrailingUnsetBits(relevantRuleMask))
                                                                  != numBits) {
                relevantRuleMask =
                    bdlb::BitUtil::withBitCleared(relevantRuleMask, j);

                const Rule *r = d_ruleSet.getRuleById(j);
                int threshold = ThresholdAggregate::maxLevel(
                                                         r->recordLevel(),
                                                         r->passLevel(),
                                                         r->triggerLevel(),
                                                         r->triggerAllLevel());
                if (threshold > category->ruleThreshold()) {
                    CategoryManagerImpUtil::setRuleThreshold(category,
                                                             threshold);
                }
            }
            CategoryManagerImpUtil::updateThresholdForHolders(category);
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
    bslmt::LockGuard<bslmt::Mutex> guard(&d_ruleSetMutex);
    ++d_ruleSequenceNum;
    for (int i = 0; i < length(); ++i) {
        if (d_categories[i]->relevantRuleMask()) {
            CategoryManagerImpUtil::setRelevantRuleMask(d_categories[i], 0);
            CategoryManagerImpUtil::setRuleThreshold(d_categories[i], 0);
            CategoryManagerImpUtil::updateThresholdForHolders(d_categories[i]);
        }
    }
    d_ruleSet.removeAllRules();
}

// ACCESSORS
const Category *CategoryManager::lookupCategory(
                                                const char *categoryName) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> registryGuard(&d_registryLock);
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
