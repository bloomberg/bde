// ball_categorymanager.h                                             -*-C++-*-
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
// category name can be an arbitrary string, including the empty string.  Note
// that category names are case-sensitive.
//
// Associated with each category, besides its name, are four threshold levels
// known as "record", "pass", "trigger", and "trigger-all".  Threshold levels
// are values in the range `[0 .. 255]`.  (See the `ball_loggermanager`
// component-level documentation for a typical interpretation of these four
// thresholds.)
//
// A category is represented by a `ball::Category` object.  Although instances
// of `ball::Category` can be created directly, within the BALL logging
// framework they are generally created by the `ball::CategoryManager` class.
// `ball::CategoryManager` manages a registry of categories and exposes methods
// to add new categories to the registry (`addCategory`) and modify the
// threshold levels of existing categories (`setThresholdLevels`).
// `ball::Category` provides accessors for direct access to the name and
// threshold levels of a given category, and a single manipulator to set the
// four threshold levels levels (see `ball_category`).
//
/// Category Threshold Levels
///-------------------------
// Every category has four severity threshold levels that govern logging
// behavior: **Record**, **Pass**, **Trigger**, and **Trigger-All**.  These
// threshold levels can be set explicitly when a category is created (via
// `addCategory`) or derived from default values when using `addCategory` with
// fewer arguments or `addCategoryHierarchically`.
//
// The threshold levels have the following meanings in the logging framework:
//
// * **Record**: If a log message's severity is at least as severe as the
//   Record threshold, the message will be stored in the logger's record
//   buffer.
//
// * **Pass**: If a log message's severity is at least as severe as the Pass
//   threshold, the message will be immediately published to observers.
//
// * **Trigger**: If a log message's severity is at least as severe as the
//   Trigger threshold, the message will cause immediate publication of that
//   message and all messages in the logger's record buffer.
//
// * **Trigger-All**: If a log message's severity is at least as severe as the
//   Trigger-All threshold, the message will cause immediate publication of
//   that message and all messages stored by all active loggers.
//
///Non-Hierarchical Default Threshold Mechanisms
///---------------------------------------------
// The category manager provides several mechanisms for determining threshold
// levels when categories are created without explicit threshold values:
//
// * **Default Threshold Levels**: The category manager maintains a set of
//   default threshold levels that can be queried via
//   `defaultRecordThresholdLevel()`, `defaultPassThresholdLevel()`,
//   `defaultTriggerThresholdLevel()`, and
//   `defaultTriggerAllThresholdLevel()`.  These defaults can be modified
//   at any time using `setDefaultThresholdLevels`.
//
// * **Default Threshold Callback**: A `DefaultThresholdLevelsCallback`
//   functor can be installed (via `setDefaultThresholdLevelsCallback`) to
//   dynamically compute threshold levels for new categories.  When installed,
//   this callback takes precedence over the (above) default threshold levels
//   mechanism.  The callback receives the category name and loads four
//   threshold values into output parameters.  This allows for sophisticated
//   threshold management strategies.
//
// * **Factory Default Thresholds**: The "factory defaults" are the
//   threshold values that the category manager is initialized with at
//   construction.  These values can be restored at any time by calling
//   `resetDefaultThresholdLevels`.
//
// The precedence order for determining thresholds when creating categories is:
// 1. Explicit threshold values provided to `addCategory`
// 2. Default threshold callback (if installed)
// 3. Current default threshold levels
//
///Hierarchical Category Management
///---------------------------------
// While categories are fundamentally flat (category names have no intrinsic
// hierarchical structure), the category manager provides functions that
// support hierarchical naming conventions.  The `addCategoryHierarchically`
// method creates a new category that inherits threshold levels from an
// existing category whose name is the longest prefix match.  The
// `setThresholdLevelsHierarchically` method modifies thresholds for all
// categories whose names share a common prefix.
//
// For example, consider categories named "EQUITY", "EQUITY.MARKET", and
// "EQUITY.MARKET.NYSE".  Using `addCategoryHierarchically` to add
// "EQUITY.MARKET.NYSE" would cause it to inherit threshold levels from
// "EQUITY.MARKET" (the longest prefix match), not from "EQUITY" or the
// defaults.  Using `setThresholdLevelsHierarchically("EQUITY.MARKET", ...)`
// would update both "EQUITY.MARKET" and "EQUITY.MARKET.NYSE", but not
// "EQUITY".
//
// This hierarchical support facilitates organizing logging categories into
// logical groupings where related categories can share common threshold
// configurations while still allowing fine-grained control.
//
// In order to keep the hierarchical category management sane the empty
// category (the default category added by `LoggerManager`) is treated as if it
// did not exist for hierarchical settings.  So when looking for the longest
// matching prefix, if it is found to be the empty string, we use the default
// threshold levels instead of the levels of the empty category.  Similarly,
// setting hierarchical levels with an empty prefix will never store an
// orphaned setting (even if the default category does not exist) but instead
// we just update the threshold levels of all existing categories and drop all
// orphaned settings.
//
///Category Name Filtering
///------------------------
// Category names can be transformed by a `CategoryNameFilterCallback` functor
// before being stored in the registry.  This allows for normalization of
// category names, such as converting all names to lowercase.  When a name
// filter is installed (via `setCategoryNameFilterCallback`), it is applied to
// every category name on addition and lookup operations, ensuring consistent
// naming regardless of how client code specifies names.
//
///Registry Capacity Management
///----------------------------
// The category registry can have a maximum capacity limit set via
// `setMaxNumCategories`.  A value of 0 (the default) means no limit is
// imposed.  When the limit is reached, attempts to add new categories will
// fail (methods that add categories will return null pointers).  The current
// capacity limit can be queried via `maxNumCategories`, and the current number
// of categories via `length`.
//
///Thread Safety
///-------------
// `ball::CategoryManager` is *thread-safe*, meaning that any operation on the
// same instance can be safely invoked from any thread concurrently with any
// other operation.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The code fragments in the following example illustrate some basic operations
// of category management including (1) adding categories to the registry,
// (2) accessing and modifying the threshold levels of existing categories,
// and (3) iterating over the categories in the registry.
//
// First we define some hypothetical category names:
// ```
//   const char *myCategories[] = {
//       "EQUITY.MARKET.NYSE",
//       "EQUITY.MARKET.NASDAQ",
//       "EQUITY.GRAPHICS.MATH.FACTORIAL",
//       "EQUITY.GRAPHICS.MATH.ACKERMANN"
//   };
// ```
// Next we create a `ball::CategoryManager` named `manager` and use the
// `addCategory` method to define a category for each of the names in
// `myCategories`.  The threshold levels of each of the categories are set to
// slightly different values to help distinguish them when they are displayed
// later:
// ```
//   ball::CategoryManager manager;
//
//   const int NUM_CATEGORIES = sizeof myCategories / sizeof *myCategories;
//   for (int i = 0; i < NUM_CATEGORIES; ++i) {
//       manager.addCategory(myCategories[i],
//                           192 + i, 96 + i, 64 + i, 32 + i);
//   }
// ```
// In the following, each of the new categories is accessed from the registry
// and their names and threshold levels printed:
// ```
//   for (int i = 0; i < NUM_CATEGORIES; ++i) {
//       const ball::Category *category =
//                                    manager.lookupCategory(myCategories[i]);
//       bsl::cout << "[ " << myCategories[i]
//                 << ", " << category->recordLevel()
//                 << ", " << category->passLevel()
//                 << ", " << category->triggerLevel()
//                 << ", " << category->triggerAllLevel()
//                 << " ]" << bsl::endl;
//   }
// ```
// The following is printed to `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 192, 96, 64, 32 ]
//   [ EQUITY.MARKET.NASDAQ, 193, 97, 65, 33 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 194, 98, 66, 34 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 195, 99, 67, 35 ]
// ```
// We next use the `setLevels` method of `ball::Category` to adjust the
// threshold levels of our categories.  The following also demonstrates use of
// the `recordLevel`, etc., accessors of `ball::Category`:
// ```
//   for (int i = 0; i < NUM_CATEGORIES; ++i) {
//       ball::Category *category = manager.lookupCategory(myCategories[i]);
//       category->setLevels(category->recordLevel() + 1,
//                           category->passLevel() + 1,
//                           category->triggerLevel() + 1,
//                           category->triggerAllLevel() + 1);
//   }
// ```
// Repeating the second `for` loop from above generates the following output
// on `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 193, 97, 65, 33 ]
//   [ EQUITY.MARKET.NASDAQ, 194, 98, 66, 34 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 195, 99, 67, 35 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 196, 100, 68, 36 ]
// ```
// Next we illustrate use of the index operator as a means of iterating over
// the registry of categories.  In particular, we illustrate an alternate
// approach to modifying the threshold levels of our categories by iterating
// over the categories in the registry of `manager` to increment their
// threshold levels a second time:
// ```
//   for (int i = 0; i < manager.length(); ++i) {
//       ball::Category& category = manager[i];
//       category.setLevels(category.recordLevel() + 1,
//                          category.passLevel() + 1,
//                          category.triggerLevel() + 1,
//                          category.triggerAllLevel() + 1);
//   }
// ```
// Finally, we iterate over the categories in the registry to print them out
// one last time:
// ```
//   for (int i = 0; i < manager.length(); ++i) {
//       const ball::Category& category = manager[i];
//       bsl::cout << "[ " << category.categoryName()
//                 << ", " << category.recordLevel()
//                 << ", " << category.passLevel()
//                 << ", " << category.triggerLevel()
//                 << ", " << category.triggerAllLevel()
//                 << " ]" << bsl::endl;
//   }
// ```
// This iteration produces the following output on `stdout`:
// ```
//   [ EQUITY.MARKET.NYSE, 194, 98, 66, 34 ]
//   [ EQUITY.MARKET.NASDAQ, 195, 99, 67, 35 ]
//   [ EQUITY.GRAPHICS.MATH.FACTORIAL, 196, 100, 68, 36 ]
//   [ EQUITY.GRAPHICS.MATH.ACKERMANN, 197, 101, 69, 37 ]
// ```
//
///Example 2: Hierarchical Category Management
/// - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates hierarchical category management using
// `addCategoryHierarchically` and `setThresholdLevelsHierarchically`.  These
// methods support a hierarchical naming scheme where categories can inherit
// threshold levels from ancestor categories based on prefix matching.
//
// First, we create a category manager and set default threshold levels:
// ```
//   ball::CategoryManager manager;
//   manager.setDefaultThresholdLevels(191, 95, 63, 31);
// ```
// Then, we create two new categories, `"EQ"` and `"EQ.MARKET"`, with
// explicitly set threshold levels (different from the defaults):
// ```
//   manager.addCategory("EQ", 192, 96, 64, 32);
//   manager.addCategory("EQ.MARKET", 193, 97, 65, 33);
// ```
// Next, we add a new category using `addCategoryHierarchically`.  This method
// finds the longest prefix match among existing categories and inherits
// threshold levels from that category:
// ```
//   ball::Category *nyseCategory =
//                        manager.addCategoryHierarchically("EQ.MARKET.NYSE");
// ```
// The new category `"EQ.MARKET.NYSE"` inherits its threshold levels from
// `"EQ.MARKET"` (rather than from `"EQ"` or the defaults) because
// `"EQ.MARKET"` is the longest prefix match:
// ```
//   assert(193 == nyseCategory->recordLevel());
//   assert( 97 == nyseCategory->passLevel());
//   assert( 65 == nyseCategory->triggerLevel());
//   assert( 33 == nyseCategory->triggerAllLevel());
// ```
// Then, we use `setThresholdLevelsHierarchically` to adjust the threshold
// levels for all categories whose name starts with `"EQ.MARKET"`:
// ```
//   int numUpdated = manager.setThresholdLevelsHierarchically("EQ.MARKET",
//                                                              194,
//                                                              98,
//                                                              66,
//                                                              34);
//   assert(2 == numUpdated);  // Updated "EQ.MARKET" and "EQ.MARKET.NYSE"
// ```
// We can verify that both `"EQ.MARKET"` and `"EQ.MARKET.NYSE"` have been
// updated, while `"EQ"` remains unchanged:
// ```
//   const ball::Category *eqCategory = manager.lookupCategory("EQ");
//   const ball::Category *marketCategory =
//                                         manager.lookupCategory("EQ.MARKET");
//   const ball::Category *nyseCategory2 =
//                                    manager.lookupCategory("EQ.MARKET.NYSE");
//
//   assert(192 == eqCategory->recordLevel());       // unchanged
//   assert(194 == marketCategory->recordLevel());   // updated
//   assert(194 == nyseCategory2->recordLevel());    // updated
// ```
// Finally, if we add another category under `"EQ.MARKET"` using
// `addCategoryHierarchically`, it will inherit the updated thresholds:
// ```
//   ball::Category *nasdaqCategory =
//                      manager.addCategoryHierarchically("EQ.MARKET.NASDAQ");
//   assert(194 == nasdaqCategory->recordLevel());
//   assert( 98 == nasdaqCategory->passLevel());
//   assert( 66 == nasdaqCategory->triggerLevel());
//   assert( 34 == nasdaqCategory->triggerAllLevel());
// ```
// Note that hierarchical category management facilitates organizing logging
// categories into logical groupings where related categories can share common
// threshold configurations while still allowing fine-grained control over
// individual categories.

#include <balscm_version.h>

#include <ball_category.h>
#include <ball_categorymanager_radixtree.h>
#include <ball_categorycallbacks.h>
#include <ball_hierarchicalcategorysetting.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_ruleset.h>
#include <ball_thresholdaggregate.h>
#include <ball_thresholddefaults.h>

#include <bdlb_cstringequalto.h>
#include <bdlb_cstringhash.h>

#include <bslma_allocator.h>

#include <bslmt_mutex.h>
#include <bslmt_readlockguard.h>
#include <bslmt_readerwriterlock.h>
#include <bslmt_readerwritermutex.h>
#include <bslmt_writelockguard.h>

#include <bsls_atomic.h>
#include <bsls_types.h>

#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ball {

                           // =====================
                           // class CategoryManager
                           // =====================

/// This class manages a set (or "registry") of categories.  Categories may be
/// added to the registry, but they cannot be removed.  However, the threshold
/// levels of existing categories may be accessed and modified directly.
class CategoryManager {

  public:
    // TYPES

    /// `CategoryNameFilterCallback` is the type of the user-supplied functor
    /// that translates external category names to internal names.
    typedef CategoryCallbacks::NameFilter       CategoryNameFilterCallback;

    /// `DefaultThresholdLevelsCallback` is the type of the functor that
    /// determines default threshold levels for categories added to the
    /// registry by the `setCategory(const char *)` method.
    typedef CategoryCallbacks::DefaultThresholdLevels
                                                DefaultThresholdLevelsCallback;

  private:
    // PRIVATE TYPES
    typedef bsl::unordered_map<const char *,
                               int,
                               bdlb::CStringHash,
                               bdlb::CStringEqualTo>      CategoryMap;

    typedef bsl::vector<Category *>                       CategoryPtrVector;

    typedef ball::CategoryManager_RadixTree<HierarchicalCategorySetting>
                                                          HierarchicalSettings;

    typedef ball::CategoryManager_RadixTree<int>          CategoryPrefixer;

    // DATA
    Category                         *d_defaultCategory_p;
                                                      // holds *Default*
                                                      // *Category*

    mutable bslmt::ReaderWriterMutex  d_defaultThresholdsCallbackMutex;
                                                      // protector

    DefaultThresholdLevelsCallback    d_defaultThresholdsCallback;
                                                      // user defined functor
                                                      // for obtaining default
                                                      // threshold levels for a
                                                      // category name

    mutable bslmt::ReaderWriterMutex  d_defaultThresholdsLevelsMutex;
                                                      // protector

    ThresholdAggregate                d_defaultThresholdLevels;
                                                      // default threshold
                                                      // level values

    const ThresholdAggregate          d_factoryThresholdLevels;
                                                      // factory default
                                                      // threshold levels

    CategoryNameFilterCallback        d_categoryNameFilterCallback;
                                                       // category name filter
                                                       // functor

    unsigned int                      d_maxNumCategoriesMinusOne;
                                                       // one less than the
                                                       // current allowed
                                                       // capacity of this
                                                       // registry

    CategoryMap                       d_registry;      // mapping names to
                                                       // indices in
                                                       // `d_categories`

    bsls::AtomicInt64                 d_ruleSetSequenceNumber;
                                                       // sequence number that
                                                       // is incremented each
                                                       // time the rule set is
                                                       // changed

    RuleSet                           d_ruleSet;       // rule set that contains
                                                       // all registered rules

    bslmt::Mutex                      d_ruleSetMutex;  // serialize access to
                                                       // `d_ruleset`

    CategoryPtrVector                 d_categories;    // providing random
                                                       // access to categories

    CategoryPrefixer                  d_categoryPrefixer;
                                                       // Data structure that
                                                       // maps category names
                                                       // as prefixes to
                                                       // threshold settings to
                                                       // support quick
                                                       // hierarchical
                                                       // operations on
                                                       // prefixes of category
                                                       // name (not O(N)).
                                                       // Note that it maps to
                                                       // indexes into
                                                       // `d_categories`.

    HierarchicalSettings              d_orphanHierarchicalSettings;
                                                       // A data structure that
                                                       // contains hierarchical
                                                       // category threshold
                                                       // level settings that
                                                       // have no corresponding
                                                       // category name for
                                                       // their category name
                                                       // prefix.  The data
                                                       // structure efficiently
                                                       // maps the category
                                                       // prefix to the
                                                       // settings object for
                                                       // optimal prefix-based
                                                       // operations.

    mutable bslmt::ReaderWriterLock   d_registryLock;  // ensuring MT-safety of
                                                       // category map

    bslma::Allocator                 *d_allocator_p;   // memory allocator
                                                       // (held, not owned)

  private:
    // NOT IMPLEMENTED
    CategoryManager(const CategoryManager&);
    CategoryManager& operator=(const CategoryManager&);

    // PRIVATE MANIPULATORS

    /// Add to the registry of this category manager a category having the
    /// specified `filteredCategoryName` and the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` threshold values,
    /// respectively, if there is no category having `filteredCategoryName` and
    /// each of the specified threshold values is in the range `[0 .. 255]`.
    /// Return the address of the newly-created, modifiable category on
    /// success, and 0 otherwise.  If a newly-created category is returned and
    /// the specified `categoryHolder` is non-null, then also load into
    /// `categoryHolder` the returned category and its maximum level and link
    /// `categoryHolder` to the category.  Note that if a category having
    /// `filteredCategoryName` already exists in the registry, 0 is returned.
    Category *addFilteredCategory(CategoryHolder *categoryHolder,
                                  const char     *filteredCategoryName,
                                  int             recordLevel,
                                  int             passLevel,
                                  int             triggerLevel,
                                  int             triggerAllLevel);

    /// Add to the registry of this category manager a category having the
    /// specified `filteredCategoryName` and the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` threshold values,
    /// respectively.  Return the address of the newly-created, modifiable
    /// category.  The behavior is undefined unless a category having
    /// `filteredCategoryName` does not already exist in the registry and each
    /// of the specified threshold values is in the range `[0 .. 255]`.  The
    /// behavior is also undefined unless the caller holds a write lock on the
    /// `d_registryLock` mutex.
    Category *addNewCategory(const char *filteredCategoryName,
                             int         recordLevel,
                             int         passLevel,
                             int         triggerLevel,
                             int         triggerAllLevel);

    /// Return the address of the modifiable category having the specified
    /// `filteredCategoryName` in the registry of this category manager, or 0
    /// if no such category exists.  The behavior is undefined if the caller
    /// holds a read or write lock on the `d_registryLock` mutex.
    Category *lookupFilteredCategory(const char *filteredCategoryName);

    /// Return the address of the modifiable category having the specified
    /// `filteredCategoryName` in the registry of this category manager, or 0
    /// if no such category exists.  If a category is returned and the
    /// specified `categoryHolder` is non-null, then also load into
    /// `categoryHolder` the returned category and its maximum level and link
    /// `categoryHolder` to the category if it has not yet been linked.  The
    /// behavior is undefined if the caller holds a read or write lock on the
    /// `d_registryLock` mutex.
    Category *lookupFilteredCategory(CategoryHolder *categoryHolder,
                                     const char     *filteredCategoryName);

    /// Apply all rules in the rule set to the specified `category`.  The
    /// behavior is undefined unless the caller holds a lock on the
    /// `d_ruleSetMutex` mutex.
    void privateApplyRulesToCategory(Category *category);

    /// Apply all rules in the rule set to all categories.  Use the
    /// specified `ruleGuard` to provide synchronization.  The behavior is
    /// undefined unless `ruleGuard` holds a lock on `d_ruleSetMutex`.  Note
    /// that all methods that modify the rule set must lock `d_ruleSetMutex`
    /// to perform that modification, so passing the guard avoids needlessly
    /// unlocking and re-locking the mutex.
    void privateApplyRulesToAllCategories(
                                    bslmt::LockGuard<bslmt::Mutex>& ruleGuard);

    // PRIVATE ACCESSORS

    /// If `d_categoryNameFilter` is a non-null functor, apply
    /// `d_categoryNameFilter` to the specified `originalName`, store the
    /// translated result in the specified `filteredNameBuffer`, and return the
    /// address of the non-modifiable data of `filteredNameBuffer`; return
    /// `originalName` otherwise (i.e., if `d_categoryNameFilter` is null).
    const char *filterCategoryName(bsl::string *filteredNameBuffer,
                                   const char  *originalName) const;

    /// Return the address of the non-modifiable category having the specified
    /// `filteredCategoryName` in the registry of this category manager, or 0
    /// if no such category exists.  The behavior is undefined if the caller
    /// holds a read or write lock on the `d_registryLock` mutex.
    const Category *lookupFilteredCategory(
                                       const char *filteredCategoryName) const;

  public:
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -FABC01 // Functions not in alphanumeric order

    // CREATORS

    /// Create a category manager with the hardwired default levels for new
    /// categories and no defaults or namefilter callback set.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If `basicAllocator`
    /// is 0, the currently installed default allocator is used.
    explicit CategoryManager(bslma::Allocator *basicAllocator = 0);

    /// Create a category manager with the specified default threshold level
    /// values `defaultRecordLevel`, `defaultPassLevel`, `defaultTriggerLevel`,
    /// `defaultTriggerAllLevel`, the  `defaultThresholdLevelsCallback`, and
    /// the `categoryNameFilterCallback`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    CategoryManager(
         int                                    defaultRecordLevel,
         int                                    defaultPassLevel,
         int                                    defaultTriggerLevel,
         int                                    defaultTriggerAllLevel,
         const DefaultThresholdLevelsCallback&  defaultThresholdLevelsCallback,
         const CategoryNameFilterCallback&      categoryNameFilterCallback,
         bslma::Allocator                      *basicAllocator = 0);

    /// Destroy this category manager.
    ~CategoryManager();

    // MANIPULATORS

    /// Return a non-`const` reference to the category at the specified
    /// `index` in the registry of this category manager.  The behavior is
    /// undefined unless `0 <= index < length()`.
    Category& operator[](int index);

    /// Add to the registry of this category manager a category having the
    /// specified `categoryName`, possibly changed by the registered category
    /// name filter callback, and having the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` threshold values,
    /// respectively, if there is no category having (the possibly filtered)
    /// `categoryName` and each of the specified threshold values is in the
    /// range `[0 .. 255]`.  Return the address of the newly-created,
    /// modifiable category on success, and 0 otherwise.  The behavior is
    /// undefined unless a lock is not held by this thread on the mutex
    /// returned by `rulesetMutex`.  Note that if a category having (the
    /// possibly filtered) `categoryName` already exists in the registry, 0 is
    /// returned.  Note that an existing category will not be modified if a
    /// category with the (possibly filtered) `categoryName` already exists.
    Category *addCategory(const char *categoryName,
                          int         recordLevel,
                          int         passLevel,
                          int         triggerLevel,
                          int         triggerAllLevel);

    /// Add to the registry of this category manager a category having the
    /// specified `categoryName`, possibly changed by the registered category
    /// name filter callback, and having the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` threshold values,
    /// respectively, if there is no category having (the possibly filtered)
    /// `categoryName` and each of the specified threshold values is in the
    /// range `[0 .. 255]`.  Return the address of the newly-created,
    /// modifiable category on success, and 0 otherwise.  If a newly-created
    /// category is returned and the specified `categoryHolder` is non-null,
    /// then also load into `categoryHolder` the returned category and its
    /// maximum level and link `categoryHolder` to the category.  The behavior
    /// is undefined unless a lock is not held by this thread on the mutex
    /// returned by `rulesetMutex`.  Note that if a category having (the
    /// possibly filtered) `categoryName` already exists in the registry, 0 is
    /// returned.  Note that an existing category will not be modified if a
    /// category with the (possibly filtered) `categoryName` already exists.
    Category *addCategory(CategoryHolder *categoryHolder,
                          const char     *categoryName,
                          int             recordLevel,
                          int             passLevel,
                          int             triggerLevel,
                          int             triggerAllLevel);

    /// Add a new category with the specified `categoryName`, possibly changed
    /// by the registered category name filter callback, unless it already
    /// exists.  Return a pointer to the newly added category in case it did
    /// not already exist.  Determine the threshold levels for the category by
    /// finding the longest category name or category name prefix that is a
    /// prefix of this (possibly filtered) `categoryName` and use its settings.
    /// If no such category or category hierarchical setting exists use the
    /// defaults.  Otherwise, if a category already exists with (the possibly
    /// filtered) `categoryName` do nothing and return 0.
    Category *addCategoryHierarchically(const char *categoryName);

    /// If this method is called the first time (`d_defaultCategory_p` is
    /// null) add a new category with the (unspecified) default category name
    /// (possibly changed by the registered category name filter callback).
    /// Use the default threshold levels for the new category.  The behavior is
    /// undefined if the default category already exists but has not been added
    /// by this method.
    void addDefaultCategory();

    /// Return a non-`const` reference to the *Default* *Category* in this
    /// category registry.  The behavior is undefined unless
    /// `addDefaultCategory` has been called.
    Category& defaultCategory();

    /// Return the address of the modifiable category having the specified
    /// `categoryName`in the registry of this category manager, or 0 if no
    /// such category exists.  Note that the category name is possibly changed
    /// by the registered category name filter callback before the lookup.
    Category *lookupCategory(const char *categoryName);

    /// Return the address of the modifiable category having the specified
    /// `categoryName` in the registry of this category manager, or 0 if no
    /// such category exists.  If a category is returned and the specified
    /// `categoryHolder` is non-null, then also load into `categoryHolder`
    /// the returned category and its maximum level and link
    /// `categoryHolder` to the category if it has not yet been linked.  Note
    /// that the category name is possibly changed by the registered category
    /// name filter callback before the lookup.
    Category *lookupCategory(CategoryHolder *categoryHolder,
                             const char     *categoryName);

    /// Reset the category holders to which all categories in the registry
    /// of this category manager are linked to their default value.  See the
    /// function-level documentation of `CategoryHolder::reset()` for
    /// further information on the default value of category holders.
    void resetCategoryHolders();

    /// Reset the default threshold levels to the original "factory-supplied"
    /// default values or the factory overrides supplied at construction.
    void resetDefaultThresholdLevels();

    /// Add to this category registry a new category having the specified
    /// `categoryName`, possibly changed by the registered category name filter
    /// callback, and default threshold levels if (the possibly filtered)
    /// `categoryName` is not present in the registry and the number of
    /// categories in the registry is less than the registry capacity.  Return
    /// the address of the (possibly newly-created) non-modifiable category
    /// having (the possibly filtered) `categoryName`, if such a category
    /// exists, and the address of the non-modifiable *Default* *Category*
    /// otherwise.  The behavior is undefined unless `categoryName` is
    /// null-terminated.  Note that a valid category address is *always*
    /// returned.
    const Category *setCategory(const char *categoryName);

    /// Add to this category registry a new category having the specified
    /// `categoryName`, possibly changed by the registered category name filter
    /// callback, and default threshold levels if (the possibly filtered)
    /// `categoryName` is not present in the registry and the number of
    /// categories in the registry is less than the registry capacity.  Return
    /// the address of the (possibly newly-created) non-modifiable category
    /// having (the possibly filtered) `categoryName`, if such a category
    /// exists, and the address of the non-modifiable *Default* *Category*
    /// otherwise.  If the specified `categoryHolder` is non-null, then also
    /// load into `categoryHolder` the returned category and its maximum level
    /// and link `categoryHolder` to the category if it has not yet been
    /// linked.  The behavior is undefined unless `categoryName` is
    /// null-terminated.  Note that a valid category address is *always*
    /// returned.
    const Category *setCategory(CategoryHolder *categoryHolder,
                                const char     *categoryName);

    /// Add to this category registry a new category having the specified
    /// `categoryName`, possibly changed by the registered category name filter
    /// callback, and having `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel` threshold levels, respectively, if (1) (the possibly
    /// filtered) `categoryName` is not present in the registry, (2) the number
    /// of categories in the registry is less than the registry capacity, and
    /// (3) `recordLevel`, `passLevel`, `triggerLevel`, and `triggerAllLevel`
    /// are all within the range `[0 .. 255]`.  If (the possibly filtered)
    /// `categoryName` is already present and each threshold level is within
    /// the valid range then reset the threshold levels of (the possibly
    /// filtered) `categoryName` to the specified values.  Return the address
    /// of the (possibly newly-created) modifiable category having (the
    /// possibly filtered) `categoryName` if that `categoryName` was either
    /// created or its thresholds reset, and 0 otherwise.  The behavior is
    /// undefined unless `categoryName` is null-terminated.  Note that 0, and
    /// *not* the *Default* *Category*, is returned on failure.
    Category *setCategory(const char *categoryName,
                          int         recordLevel,
                          int         passLevel,
                          int         triggerLevel,
                          int         triggerAllLevel);

    /// Set the default threshold levels to the specified `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` values,
    /// respectively, if each threshold level is in the range `[0 .. 255]`.
    /// Return 0 on success, and a non-zero value otherwise (with no effect on
    /// any of the default threshold levels).
    int setDefaultThresholdLevels(int recordLevel,
                                  int passLevel,
                                  int triggerLevel,
                                  int triggerAllLevel);

    /// Set the default-thresholds callback to the specified
    /// `defaultThresholdLevelsCallback` if it is not null, and remove any
    /// existing callback if `defaultThresholdLevelsCallback` is null.
    void setDefaultThresholdLevelsCallback(
         const DefaultThresholdLevelsCallback *defaultThresholdLevelsCallback);

    /// Set the threshold levels of the category having the specified
    /// `categoryName`, possibly changed by the registered category name filter
    /// callback, in the registry of this category manager to the specified
    /// `recordLevel`, `passLevel`, `triggerLevel`, and `triggerAllLevel`
    /// values, respectively, if a category having (the possibly filtered)
    /// `categoryName` exists and each of the specified threshold values is in
    /// the range `[0 .. 255]`.  Otherwise, add to the registry a category
    /// having (the possibly filtered) `categoryName` and `recordLevel`,
    /// `passLevel`, `triggerLevel`, and `triggerAllLevel` threshold values,
    /// respectively, if there is no category having (the possibly filtered)
    /// `categoryName` and each of the specified threshold values is in the
    /// range `[0 .. 255]`.  Return the address of the (possibly newly-created)
    /// modifiable category on success, and 0 otherwise (with no effect on any
    /// category).  The behavior is undefined unless a lock is not held by this
    /// thread on the mutex returned by `rulesetMutex`.
    Category *setThresholdLevels(const char *categoryName,
                                 int         recordLevel,
                                 int         passLevel,
                                 int         triggerLevel,
                                 int         triggerAllLevel);

    /// Set the threshold levels of all existing and future categories whose
    /// name starts with the specified `categoryNamePrefix` unless it is empty,
    /// possibly changed by the registered category name filter callback, to
    /// the specified `recordLevel`, `passLevel`, `triggerLevel`, and
    /// `triggerAllLevel` if those levels form valid settings and return the
    /// number of categories updated (zero or a positive number).  In case
    /// `categoryNamePrefix` is empty update all categories with the threshold
    /// levels and delete all stored orphan hierarchical settings.  In other
    /// words an empty category setting will never overwrite the defaults for
    /// future hierarchical categories.  The behavior is undefined unless a
    /// lock is not held by this thread on the mutex returned by
    /// `rulesetMutex`.  If any of the specified threshold levels is invalid,
    /// return a negative value.
    int setThresholdLevelsHierarchically(const char *categoryNamePrefix,
                                         int         recordLevel,
                                         int         passLevel,
                                         int         triggerLevel,
                                         int         triggerAllLevel);

    /// Add the specified `ruleToAdd` to the set of (unique) rules maintained
    /// by this object.  Return the number of rules added (i.e., 1 on success
    /// and 0 if a rule with the same value is already present).  The behavior
    /// is undefined unless a lock is not held by this thread on the mutex
    /// returned by `rulesetMutex`.
    int addRule(const Rule& ruleToAdd);

    /// Add each rule in the specified `ruleSet` to the set of (unique)
    /// rules maintained by this object.  Return the number of rules added.
    /// The behavior is undefined unless a lock is not held by this thread
    /// on the mutex returned by `rulesetMutex`.  Note that each rule having
    /// the same value as an existing rule will be ignored.
    int addRules(const RuleSet& ruleSet);

    /// Remove the specified `ruleToRemove` from the set of (unique) rules
    /// maintained by this object.  Return the number of rules removed
    /// (i.e., 1 on success and 0 if no rule having the same value is
    /// found).  The behavior is undefined unless a lock is not held by this
    /// thread on the mutex returned by `rulesetMutex`.
    int removeRule(const Rule& ruleToRemove);

    /// Remove each rule in the specified `ruleSet` from the set of rules
    /// maintained by this object.  Return the number of rules removed.  The
    /// behavior is undefined unless a lock is not held by this thread on
    /// the mutex returned by `rulesetMutex`.
    int removeRules(const RuleSet& ruleSet);

    /// Remove every rule from the set of rules maintained by this object.
    /// The behavior is undefined unless a lock is not held by this thread
    /// on the mutex returned by `rulesetMutex`.
    void removeAllRules();

    /// Return a non-`const` reference to the mutex that is used to guard
    /// against concurrent access to the rule set.  A lock on the returned
    /// mutex should be acquired before accessing the properties of the rule
    /// set returned by `ruleSet`.  The behavior is undefined unless a lock
    /// is acquired solely for the purpose of calling `ruleSet`.
    bslmt::Mutex& rulesetMutex();

    /// Set the capacity of this category registry to the specified `length`.
    /// If `length` is 0, no limit will be imposed.  No categories are removed
    /// from the registry if the current number of categories exceeds `length`.
    /// However, subsequent attempts to add categories to the registry will
    /// fail.  The behavior is undefined unless `0 <= length`.
    void setMaxNumCategories(int length);

    /// Invoke the specified `visitor` functor on each category managed by
    /// this object, supplying that functor modifiable access to each
    /// category.  `visitor` must be a functor that can be called as if it
    /// had the following signature:
    /// ```
    /// void operator()(Category *);
    /// ```
    /// The behavior is undefined if `visitor` calls any method on this
    /// `CategoryManager` object.
    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor);

    // ACCESSORS

    /// Return a `const` reference to the category at the specified `index`
    /// in the registry of this category manager.  The behavior is undefined
    /// unless `0 <= index < length()`.
    const Category& operator[](int index) const;

    /// Return a `const` reference to the *Default* *Category* in this category
    /// registry.  The behavior is undefined unless `addDefaultCategory` has
    /// been called.
    const Category& defaultCategory() const;

    /// Return the number of categories in the registry of this category
    /// manager.
    int length() const;

    /// Return the address of the non-modifiable category having the specified
    /// `categoryName`, possibly changed by the registered category name filter
    /// callback, in the registry of this category manager,
    /// or 0 if no such category exists.
    const Category *lookupCategory(const char *categoryName) const;

    /// Return the current capacity of this category registry.  A capacity of 0
    /// implies that no limit will be imposed; otherwise, new categories may be
    /// added only if `numCategories() < maxNumCategories()`.  Note that
    /// `0 < maxNumCategories() < numCategories()` *is* a valid state, implying
    /// no new categories may be added.
    int maxNumCategories() const;

    /// Return a `const` reference to the rule set maintained by this
    /// category manager.  The mutex returned by `rulesetMutex` should be
    /// locked prior to accessing the rule set.
    const RuleSet& ruleSet() const;

    /// Return the sequence number that tracks changes to the rule set
    /// maintained by this category manager.  The value returned by this
    /// method is guaranteed to monotonically increase between calls before
    /// and after the rule set is changed, and is otherwise implementation
    /// defined.
    bsls::Types::Int64 ruleSetSequenceNumber() const;

    /// Set the threshold levels of the specified `category` in this category
    /// registry of this category manager to the current default threshold
    /// values.  The behavior is undefined unless `category` is non-null.
    void setCategoryThresholdsToCurrentDefaults(Category *category) const;

    /// Set the threshold levels of the specified `category` in the category
    /// registry of this category manager to the original "factory-supplied"
    /// default values or the factory overrides supplied at construction.  The
    /// behavior is undefined unless `category` is non-null.
    void setCategoryThresholdsToFactoryDefaults(Category *category) const;

    /// Invoke the specified `visitor` functor on each category managed by
    /// this object, supplying that functor non-modifiable access to each
    /// category.  `visitor` must be a functor that can be called as if it
    /// had the following signature:
    /// ```
    /// void operator()(const Category *);
    /// ```
    /// The behavior is undefined if `visitor` calls any method on this
    /// `CategoryManager` object.
    template <class t_CATEGORY_VISITOR>
    void visitCategories(const t_CATEGORY_VISITOR& visitor) const;

                        // Individual Default Levels

    /// Return the default pass threshold level of this object.
    int defaultPassThresholdLevel() const;

    /// Return the default record threshold level of this object.
    int defaultRecordThresholdLevel() const;

    /// Return the default trigger-all threshold level of this object.
    int defaultTriggerAllThresholdLevel() const;

    /// Return the default trigger threshold level of this object.
    int defaultTriggerThresholdLevel() const;

                         // Default Levels Aggregate

    /// Return the default threshold levels associated with this object.
    ThresholdAggregate defaultThresholdLevels() const;

                   // Determine Default Threshold Levels

    /// Load into the specified `*levels` the threshold levels that would be
    /// set for a newly and non-hierarchically created category.  Return 0 on
    /// success and a non-zero value otherwise.  If the client has configured a
    /// default threshold levels callback, the `categoryName`, possibly changed
    /// by the registered category name filter callback, will be supplied to
    /// that callback which will set `*levels`.  Otherwise, if no default
    /// threshold levels callback has been provided, the default threshold
    /// levels are used.  Note that this function will report an error if the
    /// callback returns invalid levels.
    int thresholdLevelsForNewCategory(ThresholdAggregate *levels,
                                      const char         *categoryName) const;

    // BDE_VERIFY pragma: pop
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                        // =========================
                        // class CategoryManagerIter
                        // =========================

/// This class defines an iterator providing sequential, read-only access to
/// the categories in the registry of a category manager.  The order of the
/// iteration is undefined.
///
/// @DEPRECATED: Use the `CategoryManager::visitCategories` accessor
/// instead.
class CategoryManagerIter {

    // DATA
    const CategoryManager *d_cm_p;   // associated category manager (held)
    int                    d_index;  // index into category manager

  private:
    // NOT IMPLEMENTED
    CategoryManagerIter(const CategoryManagerIter&);
    CategoryManagerIter& operator=(const CategoryManagerIter&);

  public:
    // CREATORS

    /// Create an iterator providing non-modifiable access to the categories
    /// in the specified `categoryManager` that is initialized to refer to
    /// the first category in the sequence of categories in the registry of
    /// `categoryManager`, if one exists, and is initialized to be invalid
    /// otherwise.  The order of iteration is undefined.  The behavior is
    /// undefined unless the lifetime of `categoryManager` is at least as
    /// long as the lifetime of this iterator.
    explicit CategoryManagerIter(const CategoryManager& categoryManager);

    /// Destroy this iterator.
    //! ~CategoryManagerIter() = default;

    // MANIPULATORS

    /// Advance this iterator to refer to the next unvisited category.  If
    /// no such category exists, this iterator becomes invalid.  The
    /// behavior is undefined unless this iterator is initially valid.  Note
    /// that the order of iteration is undefined.
    void operator++();

    // ACCESSORS

    /// Return a non-zero value if this iterator is valid, and 0 otherwise.
    operator const void *() const;

    /// Return a `const` reference to the category currently referred to by
    /// this iterator.  The behavior is undefined unless this iterator is
    /// valid.
    const Category& operator()() const;
};

                        // ==========================
                        // class CategoryManagerManip
                        // ==========================

/// This class defines an iterator providing sequential, modifiable access
/// to the categories in the registry of a category manager.  The order of
/// the iteration is undefined.
///
/// @DEPRECATED: Use the `CategoryManager::visitCategories` manipulator
/// instead.
class CategoryManagerManip {

    // DATA
    CategoryManager *d_cm_p;   // associated category manager (held)
    int              d_index;  // index into category manager

  private:
    // NOT IMPLEMENTED
    CategoryManagerManip(const CategoryManagerManip&);
    CategoryManagerManip& operator=(const CategoryManagerManip&);

  public:
    // CREATORS

    /// Create an iterator providing modifiable access to the categories in
    /// the specified `categoryManager` that is initialized to refer to the
    /// first category in the sequence of categories in the registry of
    /// `categoryManager`, if one exists, and is initialized to be invalid
    /// otherwise.  The order of iteration is undefined.  The behavior is
    /// undefined unless the lifetime of `categoryManager` is at least as
    /// long as the lifetime of this iterator.
    explicit CategoryManagerManip(CategoryManager *categoryManager);

    /// Destroy this iterator.
    //! ~CategoryManagerManip() = default;

    // MANIPULATORS

    /// Advance this iterator to refer to the next unvisited category.  If
    /// no such category exists, this iterator becomes invalid.  The
    /// behavior is undefined unless this iterator is initially valid.  Note
    /// that the order of iteration is undefined.
    void advance();

    /// Return a non-`const` reference to the category currently referred to
    /// by this iterator.  The behavior is undefined unless this iterator is
    /// valid.
    Category& operator()();

    // ACCESSORS

    /// Return a non-zero value if this iterator is valid, and 0 otherwise.
    operator const void *() const;
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
Category *CategoryManager::addCategory(const char *categoryName,
                                       int         recordLevel,
                                       int         passLevel,
                                       int         triggerLevel,
                                       int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    return addCategory(0,
                       categoryName,
                       recordLevel,
                       passLevel,
                       triggerLevel,
                       triggerAllLevel);
}

inline
Category& CategoryManager::defaultCategory()
{
    return *d_defaultCategory_p;
}

inline
bslmt::Mutex& CategoryManager::rulesetMutex()
{
    return d_ruleSetMutex;
}

inline
const Category *CategoryManager::setCategory(const char *categoryName)
{
    return setCategory(0, categoryName);
}

inline
void CategoryManager::setMaxNumCategories(int length)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    d_maxNumCategoriesMinusOne = length - 1;
}

template <class t_CATEGORY_VISITOR>
void CategoryManager::visitCategories(const t_CATEGORY_VISITOR& visitor)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    for (CategoryPtrVector::iterator it = d_categories.begin();
                                     it != d_categories.end();
                                   ++it) {
        visitor(*it);
    }
}

// ACCESSORS
inline
const Category& CategoryManager::operator[](int index) const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return *d_categories[index];
}

inline
const Category& CategoryManager::defaultCategory() const
{
    return *d_defaultCategory_p;
}

inline
int CategoryManager::length() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return static_cast<int>(d_categories.size());
}

inline
int CategoryManager::maxNumCategories() const
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterLock> guard(&d_registryLock);
    return static_cast<int>(d_maxNumCategoriesMinusOne) + 1;
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
    for (CategoryPtrVector::const_iterator it = d_categories.begin();
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
