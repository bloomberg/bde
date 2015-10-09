// ball_categorymanager.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_CATEGORYMANAGER
#define INCLUDED_BALL_CATEGORYMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a manager of named categories each having "thresholds".
//
//@CLASSES:
//              ball::Category: container for name and threshold levels
//        ball::CategoryHolder: holder of a category and its maximum level
//       ball::CategoryManager: manager of category registry
//
// (*) Indicates that the class is deprecated.  See "Deprecation Notice" below.
//
//@SEE_ALSO: ball_loggermanager, ball_loggercategoryutil
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
// A category is represented by a 'ball::Category' object.  Instances of
// 'ball::Category' cannot be created directly; instead, they are created by
// the 'ball::CategoryManager' class.  'ball::CategoryManager' manages a registry
// of categories and exposes methods to add new categories to the registry
// ('addCategory') and modify the threshold levels of existing categories
// ('setThresholdLevels').  'ball::Category' provides accessors for direct
// access to the name and threshold levels of a given category, and a single
// manipulator to set the four threshold levels levels.
//
// This component also provides an iterator ('ball::CategoryManagerIter')
// and a manipulator ('ball::CategoryManagerManip') that give read-only and
// modifiable access, respectively, to the categories in the registry
// maintained by 'ball::CategoryManager'.
//
///Deprecation Notice
///------------------
// The classes 'ball::CategoryManagerIter' and 'ball::CategoryManagerManip'
// are deprecated in favor of indexed access to categories via the member
// operation 'ball::CategoryManager::operator[]()'.  Such iterator and
// manipulator objects are uncooperative in a multi-threaded environment.  In
// particular, the existence of either a 'ball::CategoryManagerIter' or a
// 'ball::CategoryManagerManip' object requires that write access to the
// associated category manager is blocked until the object is destroyed.
// Otherwise, the object might become invalid if the category manager state
// changes.  Indexed access to the category manager does not have this
// constraint since each successful access returns an object reference that
// is guaranteed to be valid for the lifetime of the category manager.
//
///Thread Safety
///-------------
// 'ball::Category' and 'ball::CategoryManager' are *thread-safe*, meaning
// that any operation on the same instance can be safely invoked from any
// thread concurrently with any other operation.
//
// 'ball::CategoryManagerIter' and 'ball::CategoryManagerManip' are const
// thread-safe,  meaning that accessors may be invoked concurrently from
// different threads, but it is not safe to access or modify an object in one
// thread while another thread modifies the same object.
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_CATEGORY
#include <ball_category.h>
#endif

#ifndef INCLUDED_BALL_RULESET
#include <ball_ruleset.h>
#endif

#ifndef INCLUDED_BALL_THRESHOLDAGGREGATE
#include <ball_thresholdaggregate.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_READLOCKGUARD
#include <bslmt_readlockguard.h>
#endif

#ifndef INCLUDED_BSLMT_READERWRITERLOCK
#include <bslmt_readerwriterlock.h>
#endif

#ifndef INCLUDED_BDLB_CSTRINGLESS
#include <bdlb_cstringless.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

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

    // DATA
    bsl::map<const char *, int, bdlb::CStringLess>
                                    d_registry;      // mapping names to
                                                     // indices in
                                                     // 'd_categories'

    volatile int                    d_ruleSequenceNum;
                                                     // sequence number
                                                     // indicating the number
                                                     // of rule changes

    RuleSet                    d_ruleSet;       // rule set that contains
                                                     // all registered rules

    bslmt::Mutex                     d_ruleSetMutex;  // serialize access to
                                                     // 'd_ruleset'

    bsl::vector<Category *>    d_categories;    // providing random-access
                                                     // to categories

    mutable bslmt::ReaderWriterLock  d_registryLock;  // ensuring MT-safety of
                                                     // category map

    bslma::Allocator               *d_allocator_p;   // memory allocator (held,
                                                     // not owned)

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
        // specified 'categoryName' and the specified 'recordLevel,
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively.  Return the address of the newly-created, modifiable
        // category.  The behavior is undefined unless 'categoryName' is
        // null-terminated, a category having 'categoryName' does not already
        // exist in the registry, and each of the specified threshold values is
        // in the range '[0 .. 255]'.  Note that the category registry should
        // be properly synchronized before calling this method.

  public:
    // CREATORS
    CategoryManager(bslma::Allocator *basicAllocator = 0);
        // Create a category manager.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~CategoryManager();
        // Destroy this category manager.

    // MANIPULATORS
    Category& operator[](int index);
        // Return a reference to the modifiable category at the specified
        // 'index' in the registry of this category manager.  The behavior is
        // undefined unless '0 <= index < length()'.

    Category *addCategory(const char *categoryName,
                          int         recordLevel,
                          int         passLevel,
                          int         triggerLevel,
                          int         triggerAllLevel);
        // Add to the registry of this category manager a category having the
        // specified 'categoryName' and the specified 'recordLevel,
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively, if there is no category having 'categoryName' and each
        // of the specified threshold values is in the range '[0 .. 255]'.
        // Return the address of the newly-created, modifiable category on
        // success, and 0 otherwise.  The behavior is undefined unless
        // 'categoryName' is null-terminated.  Note that if a category having
        // 'categoryName' already exists in the registry, 0 is returned.

    Category *addCategory(CategoryHolder *categoryHolder,
                          const char     *categoryName,
                          int             recordLevel,
                          int             passLevel,
                          int             triggerLevel,
                          int             triggerAllLevel);
        // Add to the registry of this category manager a category having the
        // specified 'categoryName' and the specified 'recordLevel,
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' threshold values,
        // respectively, if there is no category having 'categoryName' and each
        // of the specified threshold values is in the range '[0 .. 255]'.
        // Return the address of the newly-created, modifiable category on
        // success, and 0 otherwise.  If a newly-created category is returned
        // and the specified 'categoryHolder' is non-null, then also load into
        // 'categoryHolder' the returned category and its maximum level and
        // link 'categoryHolder' to the category.  The behavior is undefined
        // unless 'categoryName' is null-terminated.  Note that if a category
        // having 'categoryName' already exists in the registry, 0 is returned.

    Category *lookupCategory(const char *categoryName);
        // Return the address of the modifiable category having the specified
        // 'categoryName' in the registry of this category manager, or 0 if
        // no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    Category *lookupCategory(CategoryHolder *categoryHolder,
                                  const char          *categoryName);
        // Return the address of the modifiable category having the specified
        // 'categoryName' in the registry of this category manager, or 0 if no
        // such category exists.  If a category is returned and the specified
        // 'categoryHolder' is non-null, then also load into 'categoryHolder'
        // the returned category and its maximum level and link
        // 'categoryHolder' to the category if it has not yet been linked.  The
        // behavior is undefined unless 'categoryName' is null-terminated.

    void resetCategoryHolders();
        // Reset the category holders to which all categories in the registry
        // of this category manager are linked to their default value.  See
        // the function-level documentation of 'CategoryHolder::reset()'
        // for further information on the default value of category holders.

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
        // 'recordLevel, 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // threshold values, respectively, if there is no category having
        // 'categoryName' and each of the specified threshold values is in the
        // range '[0 .. 255]'.  Return the address of the (possibly
        // newly-created) modifiable category on success, and 0 otherwise (with
        // no effect on any category).  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    int addRule(const Rule& rule);
        // Add the specified 'rule' to the set of (unique) rules maintained by
        // this object.  Return the number of rules added (i.e., 1 on success
        // and 0 if a rule with the same value is already present).

    int addRules(const RuleSet& ruleSet);
        // Add each rule in the specified 'ruleSet' to the set of
        // (unique) rules maintained by this object.  Return the number of
        // rules added.  Note that each rule having the same value as an
        // existing rule will be ignored.

    int removeRule(const Rule& rule);
        // Remove the specified 'rule' from the set of (unique) rules
        // maintained by this object.  Return the number of rules removed
        // (i.e., 1 on success and 0 if no rule having the same value is
        // found).

    int removeRules(const RuleSet& ruleSet);
        // Remove each rule in the specified 'ruleSet' from the set of
        // rules maintained by this object.  Return the number of rules
        // removed.

    void removeAllRules();
        // Remove every rule from the set of rules maintained by this object.

    bslmt::Mutex& rulesetMutex();
        // Return a reference to the modifiable mutex that is used to guard
        // against concurrent accesses to the rule set.  A lock to the
        // returned mutex should be acquired before accessing the properties
        // of 'ruleSet()'.  The behavior is undefined if a lock is acquired
        // and any of the rule methods on this object (other than 'ruleSet()')
        // are called.

    template <class CATEGORY_VISITOR>
    void visitCategories(const CATEGORY_VISITOR& visitor);
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, supplying that functor modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(Category *);
        //..

    // ACCESSORS
    int ruleSequenceNumber() const;
        // Return the rule sequence number indicating the number of rule
        // changes.

    const Category& operator[](int index) const;
        // Return a reference to the non-modifiable category at the specified
        // 'index' in the registry of this category manager.  The behavior is
        // undefined unless '0 <= index < length()'.

    int length() const;
        // Return the number of categories in the registry of this category
        // manager.

    const Category *lookupCategory(const char *categoryName) const;
        // Return the address of the non-modifiable category having the
        // specified 'categoryName' in the registry of this category manager,
        // or 0 if no such category exists.  The behavior is undefined unless
        // 'categoryName' is null-terminated.

    const RuleSet& ruleSet() const;
        // Return a reference to the non-modifiable rule set maintained by
        // this object.  Note that the 'rulesetMutex()' should be locked prior
        // to accessing this set.

    template <class CATEGORY_VISITOR>
    void visitCategories(const CATEGORY_VISITOR& visitor) const;
        // Invoke the specified 'visitor' functor on each category managed by
        // this object, supplying that functor non-modifiable access to each
        // category.  'visitor' must be a functor that can be called as if it
        // had the following signature:
        //..
        //  void operator()(const Category *);
        //..
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class CategoryManager
                        // ---------------------

// CREATORS
inline
CategoryManager::CategoryManager(bslma::Allocator *basicAllocator)
: d_registry(bdlb::CStringLess(), basicAllocator)
, d_ruleSet(bslma::Default::allocator(basicAllocator))
, d_categories(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

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

template <class CATEGORY_VISITOR>
void CategoryManager::visitCategories(const CATEGORY_VISITOR& visitor)
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
int CategoryManager::ruleSequenceNumber() const
{
    return d_ruleSequenceNum;
}

inline
int CategoryManager::length() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    const int length = static_cast<int>(d_categories.size());
    return length;
}

inline
const Category& CategoryManager::operator[](int index) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    const Category& category = *d_categories[index];
    return category;
}

inline
const RuleSet& CategoryManager::ruleSet() const
{
    return d_ruleSet;
}

template <class CATEGORY_VISITOR>
void CategoryManager::visitCategories(const CATEGORY_VISITOR& visitor) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    for (bsl::vector<Category *>::const_iterator it = d_categories.begin();
         it != d_categories.end();
         ++it) {
        visitor(*it);
    }
}

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
