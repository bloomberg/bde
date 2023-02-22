// ball_categorymanager.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_CATEGORYMANAGER
#define INCLUDED_BALL_CATEGORYMANAGER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a manager of named categories each having "thresholds".
//
//@CLASSES:
//  ball::CategoryManager: manager of category registry
//
//@SEE_ALSO: ball_category, ball_loggermanager, ball_loggercategoryutil
//
//@DESCRIPTION: This component provides a registry for category information and
// functions to manage the registry and its members.  By "category" we mean a
// named entity that identifies a region or functional area of a program.  A
// category name can be an arbitrary string, including the empty string.
// Note that category names are case-sensitive.
//
// Associated with each category, besides its name, are four threshold levels
// known as "record", "pass", "trigger", and "trigger-all".  Threshold
// levels are values in the range '[0 .. 255]'.  (See the 'ball_loggermanager'
// component-level documentation for a typical interpretation of these four
// thresholds.)
//
// A category is represented by a 'ball::Category' object.  Although instances
// of 'ball::Category' can be created directly, within the BALL logging
// framework they are generally created by the 'ball::CategoryManager' class.
// 'ball::CategoryManager' manages a registry of categories and exposes methods
// to add new categories to the registry ('addCategory') and modify the
// threshold levels of existing categories ('setThresholdLevels').
// 'ball::Category' provides accessors for direct access to the name and
// threshold levels of a given category, and a single manipulator to set the
// four threshold levels levels (see 'ball_category').
//
///Thread Safety
///-------------
// 'ball::CategoryManager' is *thread-safe*, meaning that any operation on the
// same instance can be safely invoked from any thread concurrently with any
// other operation.
//
///Usage
///-----
// The code fragments in the following example illustrate some basic operations
// of category management including (1) adding categories to the registry,
// (2) accessing and modifying the threshold levels of existing categories,
// and (3) iterating over the categories in the registry.
//
// First we define some hypothetical category names:
//..
//    const char *myCategories[] = {
//        "EQUITY.MARKET.NYSE",
//        "EQUITY.MARKET.NASDAQ",
//        "EQUITY.GRAPHICS.MATH.FACTORIAL",
//        "EQUITY.GRAPHICS.MATH.ACKERMANN"
//    };
//..
// Next we create a 'ball::CategoryManager' named 'manager' and use the
// 'addCategory' method to define a category for each of the names in
// 'myCategories'.  The threshold levels of each of the categories are set to
// slightly different values to help distinguish them when they are displayed
// later:
//..
//    ball::CategoryManager manager;
//
//    const int NUM_CATEGORIES = sizeof myCategories / sizeof *myCategories;
//    for (int i = 0; i < NUM_CATEGORIES; ++i) {
//        manager.addCategory(myCategories[i],
//                            192 + i, 96 + i, 64 + i, 32 + i);
//    }
//..
// In the following, each of the new categories is accessed from the registry
// and their names and threshold levels printed:
//..
//    for (int i = 0; i < NUM_CATEGORIES; ++i) {
//        const ball::Category *category =
//                                     manager.lookupCategory(myCategories[i]);
//        bsl::cout << "[ " << myCategories[i]
//                  << ", " << category->recordLevel()
//                  << ", " << category->passLevel()
//                  << ", " << category->triggerLevel()
//                  << ", " << category->triggerAllLevel()
//                  << " ]" << bsl::endl;
//    }
//..
// The following is printed to 'stdout':
//..
//    [ EQUITY.MARKET.NYSE, 192, 96, 64, 32 ]
//    [ EQUITY.MARKET.NASDAQ, 193, 97, 65, 33 ]
//    [ EQUITY.GRAPHICS.MATH.FACTORIAL, 194, 98, 66, 34 ]
//    [ EQUITY.GRAPHICS.MATH.ACKERMANN, 195, 99, 67, 35 ]
//..
// We next use the 'setLevels' method of 'ball::Category' to adjust the
// threshold levels of our categories.  The following also demonstrates use
// of the 'recordLevel', etc., accessors of 'ball::Category':
//..
//    for (int i = 0; i < NUM_CATEGORIES; ++i) {
//        ball::Category *category = manager.lookupCategory(myCategories[i]);
//        category->setLevels(category->recordLevel() + 1,
//                            category->passLevel() + 1,
//                            category->triggerLevel() + 1,
//                            category->triggerAllLevel() + 1);
//    }
//..
// Repeating the second 'for' loop from above generates the following output
// on 'stdout':
//..
//    [ EQUITY.MARKET.NYSE, 193, 97, 65, 33 ]
//    [ EQUITY.MARKET.NASDAQ, 194, 98, 66, 34 ]
//    [ EQUITY.GRAPHICS.MATH.FACTORIAL, 195, 99, 67, 35 ]
//    [ EQUITY.GRAPHICS.MATH.ACKERMANN, 196, 100, 68, 36 ]
//..
// Next we illustrate use of the index operator as a means of iterating over
// the registry of categories.  In particular, we illustrate an alternate
// approach to modifying the threshold levels of our categories by iterating
// over the categories in the registry of 'manager' to increment their
// threshold levels a second time:
//..
//    for (int i = 0; i < manager.length(); ++i) {
//        ball::Category& category = manager[i];
//        category.setLevels(category.recordLevel() + 1,
//                           category.passLevel() + 1,
//                           category.triggerLevel() + 1,
//                           category.triggerAllLevel() + 1);
//    }
//..
// Finally, we iterate over the categories in the registry to print them out
// one last time:
//..
//    for (int i = 0; i < manager.length(); ++i) {
//        const ball::Category& category = manager[i];
//        bsl::cout << "[ " << category.categoryName()
//                  << ", " << category.recordLevel()
//                  << ", " << category.passLevel()
//                  << ", " << category.triggerLevel()
//                  << ", " << category.triggerAllLevel()
//                  << " ]" << bsl::endl;
//    }
//..
// This iteration produces the following output on 'stdout':
//..
//    [ EQUITY.MARKET.NYSE, 194, 98, 66, 34 ]
//    [ EQUITY.MARKET.NASDAQ, 195, 99, 67, 35 ]
//    [ EQUITY.GRAPHICS.MATH.FACTORIAL, 196, 100, 68, 36 ]
//    [ EQUITY.GRAPHICS.MATH.ACKERMANN, 197, 101, 69, 37 ]
//..

#include <balscm_version.h>

#include <ball_category.h>
#include <ball_ruleset.h>
#include <ball_thresholdaggregate.h>

#include <bdlb_cstringequalto.h>
#include <bdlb_cstringhash.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmt_mutex.h>
#include <bslmt_readlockguard.h>
#include <bslmt_readerwriterlock.h>

#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_new.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ball {

                        // =====================
                        // class CategoryManager
                        // =====================

class CategoryManager {
    // This class manages a set (or "registry") of categories.  Categories may
    // be added to the registry, but they cannot be removed.  However, the
    // threshold levels of existing categories may be accessed and modified
    // directly.

    // TYPES
    typedef bsl::unordered_map<const char *,
                               int,
                               bdlb::CStringHash,
                               bdlb::CStringEqualTo>
        CategoryMap;

    // DATA
    CategoryMap                      d_registry;      // mapping names to
                                                      // indices in
                                                      // 'd_categories'

    bsls::AtomicInt64                d_ruleSetSequenceNumber;
                                                      // sequence number that
                                                      // is incremented each
                                                      // time the rule set is
                                                      // changed

    RuleSet                          d_ruleSet;       // rule set that contains
                                                      // all registered rules

    bslmt::Mutex                     d_ruleSetMutex;  // serialize access to
                                                      // 'd_ruleset'

    bsl::vector<Category *>          d_categories;    // providing random
                                                      // access to categories

    mutable bslmt::ReaderWriterLock  d_registryLock;  // ensuring MT-safety of
                                                      // category map

    bslma::Allocator                *d_allocator_p;   // memory allocator
                                                      // (held, not owned)

  private:
    // NOT IMPLEMENTED
    CategoryManager(const CategoryManager&);
    CategoryManager& operator=(const CategoryManager&);

    // PRIVATE MANIPULATORS
    Category *addNewCategory(const char *categoryName,
                             int         recordLevel,
                             int         passLevel,
                             int         triggerLevel,
                             int         triggerAllLevel);
        // Add to the registry of this category manager a category having the
        // specified 'categoryName' and the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively.  Return the address of the newly-created, modifiable
        // category.  The behavior is undefined unless a category having
        // 'categoryName' does not already exist in the registry and each of
        // the specified threshold values is in the range '[0 .. 255]'.  Note
        // that the category registry should be properly synchronized before
        // calling this method.

  public:
    // CREATORS
    explicit CategoryManager(bslma::Allocator *basicAllocator = 0);
        // Create a category manager.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~CategoryManager();
        // Destroy this category manager.

    // MANIPULATORS
    Category& operator[](int index);
        // Return a non-'const' reference to the category at the specified
        // 'index' in the registry of this category manager.  The behavior is
        // undefined unless '0 <= index < length()'.

    Category *addCategory(const char *categoryName,
                          int         recordLevel,
                          int         passLevel,
                          int         triggerLevel,
                          int         triggerAllLevel);
        // Add to the registry of this category manager a category having the
        // specified 'categoryName' and the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively, if there is no category having 'categoryName' and each
        // of the specified threshold values is in the range '[0 .. 255]'.
        // Return the address of the newly-created, modifiable category on
        // success, and 0 otherwise.  The behavior is undefined unless a lock
        // is not held by this thread on the mutex returned by 'rulesetMutex'.
        // Note that if a category having 'categoryName' already exists in the
        // registry, 0 is returned.

    Category *addCategory(CategoryHolder *categoryHolder,
                          const char     *categoryName,
                          int             recordLevel,
                          int             passLevel,
                          int             triggerLevel,
                          int             triggerAllLevel);
        // Add to the registry of this category manager a category having the
        // specified 'categoryName' and the specified 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively, if there is no category having 'categoryName' and each
        // of the specified threshold values is in the range '[0 .. 255]'.
        // Return the address of the newly-created, modifiable category on
        // success, and 0 otherwise.  If a newly-created category is returned
        // and the specified 'categoryHolder' is non-null, then also load into
        // 'categoryHolder' the returned category and its maximum level and
        // link 'categoryHolder' to the category.  The behavior is undefined
        // unless a lock is not held by this thread on the mutex returned by
        // 'rulesetMutex'.  Note that if a category having 'categoryName'
        // already exists in the registry, 0 is returned.

    Category *lookupCategory(const char *categoryName);
        // Return the address of the modifiable category having the specified
        // 'categoryName' in the registry of this category manager, or 0 if no
        // such category exists.

    Category *lookupCategory(CategoryHolder *categoryHolder,
                             const char     *categoryName);
        // Return the address of the modifiable category having the specified
        // 'categoryName' in the registry of this category manager, or 0 if no
        // such category exists.  If a category is returned and the specified
        // 'categoryHolder' is non-null, then also load into 'categoryHolder'
        // the returned category and its maximum level and link
        // 'categoryHolder' to the category if it has not yet been linked.

    void resetCategoryHolders();
        // Reset the category holders to which all categories in the registry
        // of this category manager are linked to their default value.  See the
        // function-level documentation of 'CategoryHolder::reset()' for
        // further information on the default value of category holders.

    Category *setThresholdLevels(const char *categoryName,
                                 int         recordLevel,
                                 int         passLevel,
                                 int         triggerLevel,
                                 int         triggerAllLevel);
        // Set the threshold levels of the category having the specified
        // 'categoryName' in the registry of this category manager to the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values, respectively, if a category having
        // 'categoryName' exists and each of the specified threshold values is
        // in the range '[0 .. 255]'.  Otherwise, add to the registry a
        // category having the specified 'categoryName' and the specified
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // threshold values, respectively, if there is no category having
        // 'categoryName' and each of the specified threshold values is in the
        // range '[0 .. 255]'.  Return the address of the (possibly
        // newly-created) modifiable category on success, and 0 otherwise (with
        // no effect on any category).  The behavior is undefined unless a lock
        // is not held by this thread on the mutex returned by 'rulesetMutex'.

    int addRule(const Rule& rule);
        // Add the specified 'rule' to the set of (unique) rules maintained by
        // this object.  Return the number of rules added (i.e., 1 on success
        // and 0 if a rule with the same value is already present).  The
        // behavior is undefined unless a lock is not held by this thread on
        // the mutex returned by 'rulesetMutex'.

    int addRules(const RuleSet& ruleSet);
        // Add each rule in the specified 'ruleSet' to the set of (unique)
        // rules maintained by this object.  Return the number of rules added.
        // The behavior is undefined unless a lock is not held by this thread
        // on the mutex returned by 'rulesetMutex'.  Note that each rule having
        // the same value as an existing rule will be ignored.

    int removeRule(const Rule& rule);
        // Remove the specified 'rule' from the set of (unique) rules
        // maintained by this object.  Return the number of rules removed
        // (i.e., 1 on success and 0 if no rule having the same value is
        // found).  The behavior is undefined unless a lock is not held by this
        // thread on the mutex returned by 'rulesetMutex'.

    int removeRules(const RuleSet& ruleSet);
        // Remove each rule in the specified 'ruleSet' from the set of rules
        // maintained by this object.  Return the number of rules removed.  The
        // behavior is undefined unless a lock is not held by this thread on
        // the mutex returned by 'rulesetMutex'.

    void removeAllRules();
        // Remove every rule from the set of rules maintained by this object.
        // The behavior is undefined unless a lock is not held by this thread
        // on the mutex returned by 'rulesetMutex'.

    bslmt::Mutex& rulesetMutex();
        // Return a non-'const' reference to the mutex that is used to guard
        // against concurrent access to the rule set.  A lock on the returned
        // mutex should be acquired before accessing the properties of the rule
        // set returned by 'ruleSet'.  The behavior is undefined unless a lock
        // is acquired solely for the purpose of calling 'ruleSet'.

    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor);
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, supplying that functor modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(Category *);
        //..

    // ACCESSORS
    const Category& operator[](int index) const;
        // Return a 'const' reference to the category at the specified 'index'
        // in the registry of this category manager.  The behavior is undefined
        // unless '0 <= index < length()'.

    int length() const;
        // Return the number of categories in the registry of this category
        // manager.

    const Category *lookupCategory(const char *categoryName) const;
        // Return the address of the non-modifiable category having the
        // specified 'categoryName' in the registry of this category manager,
        // or 0 if no such category exists.

    const RuleSet& ruleSet() const;
        // Return a 'const' reference to the rule set maintained by this
        // category manager.  The mutex returned by 'rulesetMutex' should be
        // locked prior to accessing the rule set.

    bsls::Types::Int64 ruleSetSequenceNumber() const;
        // Return the sequence number that tracks changes to the rule set
        // maintained by this category manager.  The value returned by this
        // method is guaranteed to monotonically increase between calls before
        // and after the rule set is changed, and is otherwise implementation
        // defined.

    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor) const;
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, supplying that functor non-modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(const Category *);
        //..
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                        // =========================
                        // class CategoryManagerIter
                        // =========================

class CategoryManagerIter {
    // This class defines an iterator providing sequential, read-only access to
    // the categories in the registry of a category manager.  The order of the
    // iteration is undefined.
    //
    // !DEPRECATED!: Use the 'CategoryManager::visitCategories' accessor
    // instead.

    // DATA
    const CategoryManager *d_cm_p;   // associated category manager (held)
    int                    d_index;  // index into category manager

  private:
    // NOT IMPLEMENTED
    CategoryManagerIter(const CategoryManagerIter&);
    CategoryManagerIter& operator=(const CategoryManagerIter&);

  public:
    // CREATORS
    explicit CategoryManagerIter(const CategoryManager& categoryManager);
        // Create an iterator providing non-modifiable access to the categories
        // in the specified 'categoryManager' that is initialized to refer to
        // the first category in the sequence of categories in the registry of
        // 'categoryManager', if one exists, and is initialized to be invalid
        // otherwise.  The order of iteration is undefined.  The behavior is
        // undefined unless the lifetime of 'categoryManager' is at least as
        // long as the lifetime of this iterator.

    //! ~CategoryManagerIter() = default;
        // Destroy this iterator.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next unvisited category.  If
        // no such category exists, this iterator becomes invalid.  The
        // behavior is undefined unless this iterator is initially valid.  Note
        // that the order of iteration is undefined.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this iterator is valid, and 0 otherwise.

    const Category& operator()() const;
        // Return a 'const' reference to the category currently referred to by
        // this iterator.  The behavior is undefined unless this iterator is
        // valid.
};

                        // ==========================
                        // class CategoryManagerManip
                        // ==========================

class CategoryManagerManip {
    // This class defines an iterator providing sequential, modifiable access
    // to the categories in the registry of a category manager.  The order of
    // the iteration is undefined.
    //
    // !DEPRECATED!: Use the 'CategoryManager::visitCategories' manipulator
    // instead.

    // DATA
    CategoryManager *d_cm_p;   // associated category manager (held)
    int              d_index;  // index into category manager

  private:
    // NOT IMPLEMENTED
    CategoryManagerManip(const CategoryManagerManip&);
    CategoryManagerManip& operator=(const CategoryManagerManip&);

  public:
    // CREATORS
    explicit CategoryManagerManip(CategoryManager *categoryManager);
        // Create an iterator providing modifiable access to the categories in
        // the specified 'categoryManager' that is initialized to refer to the
        // first category in the sequence of categories in the registry of
        // 'categoryManager', if one exists, and is initialized to be invalid
        // otherwise.  The order of iteration is undefined.  The behavior is
        // undefined unless the lifetime of 'categoryManager' is at least as
        // long as the lifetime of this iterator.

    //! ~CategoryManagerManip() = default;
        // Destroy this iterator.

    // MANIPULATORS
    void advance();
        // Advance this iterator to refer to the next unvisited category.  If
        // no such category exists, this iterator becomes invalid.  The
        // behavior is undefined unless this iterator is initially valid.  Note
        // that the order of iteration is undefined.

    Category& operator()();
        // Return a non-'const' reference to the category currently referred to
        // by this iterator.  The behavior is undefined unless this iterator is
        // valid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this iterator is valid, and 0 otherwise.
};

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class CategoryManager
                        // ---------------------

// MANIPULATORS
inline
Category& CategoryManager::operator[](int index)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return *d_categories[index];
}

inline
bslmt::Mutex& CategoryManager::rulesetMutex()
{
    return d_ruleSetMutex;
}

template <class t_CATEGORY_VISITOR>
void CategoryManager::visitCategories(const t_CATEGORY_VISITOR& visitor)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    for (bsl::vector<Category *>::iterator it = d_categories.begin();
         it != d_categories.end();
         ++it) {
        visitor(*it);
    }
}

// ACCESSORS
inline
int CategoryManager::length() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return static_cast<int>(d_categories.size());
}

inline
const Category& CategoryManager::operator[](int index) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return *d_categories[index];
}

inline
const RuleSet& CategoryManager::ruleSet() const
{
    return d_ruleSet;
}

inline
bsls::Types::Int64 CategoryManager::ruleSetSequenceNumber() const
{
    return d_ruleSetSequenceNumber;
}

template <class t_CATEGORY_VISITOR>
void CategoryManager::visitCategories(const t_CATEGORY_VISITOR& visitor) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    for (bsl::vector<Category *>::const_iterator it = d_categories.begin();
         it != d_categories.end();
         ++it) {
        visitor(*it);
    }
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                        // -------------------------
                        // class CategoryManagerIter
                        // -------------------------

// CREATORS
inline
CategoryManagerIter::CategoryManagerIter(
                                        const CategoryManager& categoryManager)
: d_cm_p(&categoryManager)
, d_index(0)
{
}

// MANIPULATORS
inline
void CategoryManagerIter::operator++()
{
    ++d_index;
}

// ACCESSORS
inline
CategoryManagerIter::operator const void *() const
{
    return (0 <= d_index && d_index < d_cm_p->length()) ? this : 0;
}

inline
const Category& CategoryManagerIter::operator()() const
{
    return d_cm_p->operator[](d_index);
}

                        // --------------------------
                        // class CategoryManagerManip
                        // --------------------------

// CREATORS
inline
CategoryManagerManip::CategoryManagerManip(CategoryManager *categoryManager)
: d_cm_p(categoryManager)
, d_index(0)
{
}

// MANIPULATORS
inline
void CategoryManagerManip::advance()
{
    ++d_index;
}

inline
Category& CategoryManagerManip::operator()()
{
    return d_cm_p->operator[](d_index);
}

// ACCESSORS
inline
CategoryManagerManip::operator const void *() const
{
    return (0 <= d_index && d_index < d_cm_p->length()) ? this : 0;
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

}  // close package namespace
}  // close enterprise namespace

#endif

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
