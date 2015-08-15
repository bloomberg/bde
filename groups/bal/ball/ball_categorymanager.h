// ball_categorymanager.h                                             -*-C++-*-
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
//   ball::CategoryManagerIter: sequential read-only accessor of categories (*)
//  ball::CategoryManagerManip: sequential manipulator of categories (*)
//
// (*) Indicates that the class is deprecated.  See "Deprecation Notice" below.
//
//@SEE_ALSO: ball_loggermanager, ball_loggercategoryutil
//
//@AUTHOR: Hong Shi (hshi2), Mike Verschell (hverschell)
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

#ifndef INCLUDED_BALL_RULESET
#include <ball_ruleset.h>
#endif

#ifndef INCLUDED_BALL_THRESHOLDAGGREGATE
#include <ball_thresholdaggregate.h>
#endif

#ifndef INCLUDED_BDLQQ_MUTEX
#include <bdlqq_mutex.h>
#endif

#ifndef INCLUDED_BDLQQ_READLOCKGUARD
#include <bdlqq_readlockguard.h>
#endif

#ifndef INCLUDED_BDLQQ_READERWRITERLOCK
#include <bdlqq_readerwriterlock.h>
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

namespace bslma { class Allocator; }

                        // ===================
                        // class ball::Category
                        // ===================


namespace ball {class CategoryHolder;
class CategoryManager;
class Category_Proctor;

class Category {
    // This class provides a container to hold the name and threshold levels
    // of a category.  Instances of 'Category' are created and manipulated
    // by 'CategoryManager'.  All threshold levels are integral values in
    // the range '[0 .. 255]'.

    ThresholdAggregate  d_thresholdLevels;  // record, pass, trigger, and
                                                 // trigger-all levels

    int                      d_threshold;        // numerical maximum of the
                                                 // four levels

    bsl::string              d_categoryName;     // category name

    CategoryHolder     *d_categoryHolder;   // linked list of holders of
                                                 // this category
    mutable RuleSet::MaskType
                             d_relevantRuleMask; // the mask indicating which
                                                 // rules are relevant (i.e.,
                                                 // have been attached to this
                                                 // category)

    mutable int              d_ruleThreshold;    // numerical maximum of all
                                                 // four levels for all
                                                 // relevant rules

    // FRIENDS
    friend class CategoryManager;
    friend class Category_Proctor;

    // NOT IMPLEMENTED
    Category(const Category&);
    Category& operator=(const Category&);

  private:
    // PRIVATE CREATORS
    Category(const char       *categoryName,
                  int               recordLevel,
                  int               passLevel,
                  int               triggerLevel,
                  int               triggerAllLevel,
                  bslma::Allocator *basicAllocator = 0);
        // Create a category having the specified 'categoryName' and the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' threshold values, respectively.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless each of the specified
        // threshold levels is in the range '[0 .. 255]', and 'categoryName' is
        // null-terminated.

    ~Category();
        // Destroy this category.

    // PRIVATE MANIPULATORS
    void linkCategoryHolder(CategoryHolder *categoryHolder);
        // Load this category and its corresponding 'maxLevel()' into the
        // specified 'categoryHolder', and add 'categoryHolder' to the linked
        // list of category holders managed by this category.

    void resetCategoryHolders();
        // Reset the category holders to which this category is linked to
        // their default value.  See the function-level documentation of
        // 'CategoryHolder::reset' for further information on the
        // default value of category holders.

    void updateThresholdForHolders();
        // Update the threshold of all category holders that hold the address
        // of this object to the maximum of 'd_threshold' and
        // 'd_ruleThreshold'.

  public:
    // CLASS METHODS
    static bool areValidThresholdLevels(int recordLevel,
                                        int passLevel,
                                        int triggerLevel,
                                        int triggerAllLevel);
        // Return 'true' if each of the specified 'recordLevel', 'passLevel',
        // 'triggerLevel' and 'triggerAllLevel' threshold values are in the
        // range '[0 .. 255]', and 'false' otherwise.

    // MANIPULATORS
    int setLevels(int recordLevel,
                  int passLevel,
                  int triggerLevel,
                  int triggerAllLevel);
        // Set the threshold levels of this category to the specified
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // values, respectively, if each of the specified values is in the
        // range '[0 .. 255]'.  Return 0 on success, and a non-zero value
        // otherwise (with no effect on the threshold levels of this category).

    // ACCESSORS
    const char *categoryName() const;
        // Return the name of this category.

    bool isEnabled(int level) const;
        // Return 'true' if logging at the specified 'level' is enabled for
        // this category, and 'false' otherwise.  Logging is enabled if 'level'
        // is numerically less than or equal to any of the four threshold
        // levels of this category.

    int maxLevel() const;
        // Return the numerical maximum of the four levels of this category.

    int recordLevel() const;
        // Return the record level of this category.

    int passLevel() const;
        // Return the pass level of this category.

    int triggerLevel() const;
        // Return the trigger level of this category.

    int triggerAllLevel() const;
        // Return the trigger-all level of this category.

    const ThresholdAggregate& thresholdLevels() const;
        // Return the aggregate threshold levels of this category.

    const RuleSet::MaskType& relevantRuleMask() const;
        // Return a reference to the non-modifiable relevant rule mask for
        // this category.  The returned 'RuleSet::MaskType' value is a
        // bit-mask, where each bit is a boolean value indicating whether the
        // rule at the corresponding index (in the rule set of the category
        // manager that owns this category) applies at this category.  Note
        // that a rule applies to this category if the rule's pattern matches
        // the name returned by 'categoryName'.
};

                        // =========================
                        // class CategoryHolder
                        // =========================

class CategoryHolder {
    // This class, informally referred to as a "category holder" (or simply
    // "holder"), holds a category, a threshold level, and a pointer to a
    // "next" holder.  Both the category and next pointer may be null.  The
    // intended use is as follows: (1) instances of this class are (only)
    // declared in contexts where logging occurs; (2) if the held category is
    // non-null, then the held threshold is the numerical maximum of the four
    // levels of that category; (3) if the next pointer is non-null, then the
    // holder pointed to holds the same category and threshold.  Instances of
    // this class must be *statically* initializable.  Hence, the data members
    // are 'public', and no constructors or destructor are defined.
    //
    // This class should *not* be used directly by client code.  It is an
    // implementation detail of the 'bael' logging system.

    // NOT IMPLEMENTED
    CategoryHolder& operator=(const CategoryHolder&);

  public:
    // PUBLIC TYPES
    enum {
        e_UNINITIALIZED_CATEGORY = 256, // indicates no logger manager
        e_DYNAMIC_CATEGORY       = 257  // corresponding category is dynamic
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BAEL_UNINITIALIZED_CATEGORY = e_UNINITIALIZED_CATEGORY
      , BAEL_DYNAMIC_CATEGORY = e_DYNAMIC_CATEGORY
      , UNINITIALIZED_CATEGORY = BAEL_UNINITIALIZED_CATEGORY
      , DYNAMIC_CATEGORY       = BAEL_DYNAMIC_CATEGORY
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
        // This enumeration defines distinguished values for category holder
        // threshold levels.  Note that these values are intentionally outside
        // the range '[0 .. 255]'.

    // PUBLIC DATA MEMBERS
    int                  d_threshold;   // threshold level
    const Category *d_category_p;  // held category (not owned)
    CategoryHolder *d_next_p;      // next category holder in linked list

    // CREATORS

    // No constructors or destructors are declared in order to allow for static
    // initialization of instances of this class.

    // MANIPULATORS
    void reset();
        // Reset this object to its default value.  The default value is:
        //..
        //   { BAEL_UNINITIALIZED_CATEGORY, 0, 0 }
        //..

    void setCategory(const Category *category);
        // Set the address of the category held by this holder to the specified
        // 'category'.

    void setThreshold(int threshold);
        // Set the threshold level held by this holder to the specified
        // 'threshold'.

    void setNext(CategoryHolder *holder);
        // Set this holder to point to the specified 'holder'.

    // ACCESSORS
    const Category *category() const;
        // Return the address of the non-modifiable category held by this
        // holder.

    int threshold() const;
        // Return the threshold level held by this holder.

    CategoryHolder *next() const;
        // Return the address of the modifiable holder held by this holder.
};

                        // ==========================
                        // class CategoryManager
                        // ==========================

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

    bdlqq::Mutex                     d_ruleSetMutex;  // serialize access to
                                                     // 'd_ruleset'

    bsl::vector<Category *>    d_categories;    // providing random-access
                                                     // to categories

    mutable bdlqq::ReaderWriterLock  d_registryLock;  // ensuring MT-safety of
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
                               const char          *categoryName,
                               int                  recordLevel,
                               int                  passLevel,
                               int                  triggerLevel,
                               int                  triggerAllLevel);
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

    bdlqq::Mutex& rulesetMutex();
        // Return a reference to the modifiable mutex that is used to guard
        // against concurrent accesses to the rule set.  A lock to the
        // returned mutex should be acquired before accessing the properties
        // of 'ruleSet()'.  The behavior is undefined if a lock is acquired
        // and any of the rule methods on this object (other than 'ruleSet()')
        // are called.

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
};

                        // ==============================
                        // class CategoryManagerIter
                        // ==============================

class CategoryManagerIter {
    // This class provides sequential, read-only access to the categories in
    // the registry of a category manager.  The order of the iteration is
    // undefined.
    //
    // WARNING: THIS CLASS IS DEPRECATED!

    const CategoryManager& d_cm;     // associated category manager (held)
    int                         d_index;  // index into category manager

    // NOT IMPLEMENTED
    CategoryManagerIter(const CategoryManagerIter& original);
    CategoryManagerIter& operator=(const CategoryManagerIter& rhs);

    bool operator==(const CategoryManagerIter&) const;
    bool operator!=(const CategoryManagerIter&) const;

  public:
    // CREATORS
    explicit CategoryManagerIter(
                                  const CategoryManager& categoryManager);
        // Create an iterator for the specified 'categoryManager' initialized
        // to refer to the first category in the sequence of categories in the
        // registry of 'categoryManager', if one exists, and initialized to be
        // invalid otherwise.  The order of iteration is undefined.  The
        // behavior is undefined unless the lifetime of 'categoryManager'
        // is at least as long as the lifetime of this iterator.

    ~CategoryManagerIter();
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
        // Return a reference to the non-modifiable category currently
        // referred to by this iterator.  The behavior is undefined unless this
        // iterator is valid.
};

                        // ===============================
                        // class CategoryManagerManip
                        // ===============================

class CategoryManagerManip {
    // This class provides sequential, modifiable access to the categories in
    // the registry of a category manager.  The order of the iteration is
    // undefined.
    //
    // WARNING: THIS CLASS IS DEPRECATED!

    CategoryManager *d_cm_p;   // associated category manager (held)
    int                   d_index;  // index into category manager

    // NOT IMPLEMENTED
    CategoryManagerManip(const CategoryManagerManip& original);
    CategoryManagerManip& operator=(const CategoryManagerManip& rhs);

    bool operator==(const CategoryManagerManip&) const;
    bool operator!=(const CategoryManagerManip&) const;

  public:
    // CREATORS
    explicit CategoryManagerManip(CategoryManager *categoryManager);
        // Create a manipulator for the specified 'categoryManager' initialized
        // to refer to the first category in the sequence of categories in the
        // registry of 'categoryManager', if one exists, and initialized to be
        // invalid otherwise.  The order of iteration is undefined.  The
        // behavior is undefined unless 'categoryManager' is non-null and the
        // lifetime of 'categoryManager' is at least as long as the lifetime
        // of this manipulator.

    ~CategoryManagerManip();
        // Destroy this manipulator.

    // MANIPULATORS
    void advance();
        // Advance this manipulator to refer to the next unvisited category.
        // If no such category exists, this manipulator becomes invalid.  The
        // behavior is undefined unless this manipulator is initially valid.
        // Note that the order of iteration is undefined.

    Category& operator()();
        // Return a reference to the modifiable category currently referred to
        // by this manipulator.  The behavior is undefined unless this
        // manipulator is valid.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this manipulator is valid, and 0
        // otherwise.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------
                        // class Category
                        // -------------------

// ACCESSORS
inline
const char *Category::categoryName() const
{
    return d_categoryName.c_str();
}

inline
int Category::maxLevel() const
{
    return d_threshold;
}

inline
int Category::recordLevel() const
{
    return d_thresholdLevels.recordLevel();
}

inline
int Category::passLevel() const
{
    return d_thresholdLevels.passLevel();
}

inline
int Category::triggerLevel() const
{
    return d_thresholdLevels.triggerLevel();
}

inline
int Category::triggerAllLevel() const
{
    return d_thresholdLevels.triggerAllLevel();
}

inline
const ThresholdAggregate& Category::thresholdLevels() const
{
    return d_thresholdLevels;
}

inline
const RuleSet::MaskType& Category::relevantRuleMask() const
{
    return d_relevantRuleMask;
}

                        // -------------------------
                        // class CategoryHolder
                        // -------------------------

// MANIPULATORS
inline
void CategoryHolder::setCategory(const Category *category)
{
    d_category_p = category;
}

inline
void CategoryHolder::setThreshold(int threshold)
{
    d_threshold = threshold;
}

inline
void CategoryHolder::setNext(CategoryHolder *holder)
{
    d_next_p = holder;
}

// ACCESSORS
inline
const Category *CategoryHolder::category() const
{
    return d_category_p;
}

inline
int CategoryHolder::threshold() const
{
    return d_threshold;
}

inline
CategoryHolder *CategoryHolder::next() const
{
    return d_next_p;
}

                        // --------------------------
                        // class CategoryManager
                        // --------------------------

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
    bdlqq::ReadLockGuard<bdlqq::ReaderWriterLock> guard(&d_registryLock);
    return *d_categories[index];
}

inline
bdlqq::Mutex& CategoryManager::rulesetMutex()
{
    return d_ruleSetMutex;
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
    bdlqq::ReadLockGuard<bdlqq::ReaderWriterLock> guard(&d_registryLock);
    const int length = static_cast<int>(d_categories.size());
    return length;
}

inline
const Category& CategoryManager::operator[](int index) const
{
    bdlqq::ReadLockGuard<bdlqq::ReaderWriterLock> guard(&d_registryLock);
    const Category& category = *d_categories[index];
    return category;
}

inline
const RuleSet& CategoryManager::ruleSet() const
{
    return d_ruleSet;
}

                        // ------------------------------
                        // class CategoryManagerIter
                        // ------------------------------

// CREATORS
inline
CategoryManagerIter::CategoryManagerIter(
                                   const CategoryManager& categoryManager)
: d_cm(categoryManager)
, d_index(0)
{
}

inline
CategoryManagerIter::~CategoryManagerIter()
{
}

// MANIPULATORS
inline
void CategoryManagerIter::operator++()
{
    ++d_index;
}
}  // close package namespace

// ACCESSORS
inline
ball::CategoryManagerIter::operator const void *() const
{
    return (void *)(0 <= d_index && d_index < d_cm.length());
}

namespace ball {
inline
const Category& CategoryManagerIter::operator()() const
{
    return d_cm[d_index];
}

                        // -------------------------------
                        // class CategoryManagerManip
                        // -------------------------------

// CREATORS
inline
CategoryManagerManip::CategoryManagerManip(
                                         CategoryManager *categoryManager)
: d_cm_p(categoryManager)
, d_index(0)
{
}

inline
CategoryManagerManip::~CategoryManagerManip()
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
}  // close package namespace

// ACCESSORS
inline
ball::CategoryManagerManip::operator const void *() const
{
    return (const void *)(0 <= d_index && d_index < d_cm_p->length()) ? this
                                                                      : 0;
}

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
