// baem_metricregistry.h              -*-C++-*-
#ifndef INCLUDED_BAEM_METRICREGISTRY
#define INCLUDED_BAEM_METRICREGISTRY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a registry for metrics.
//
//@CLASSES:
//   baem_MetricRegistry: a registry for metrics
//
//@SEE_ALSO: baem_metricsmanager, baem_metricrecord
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a class, 'baem_MetricRegistry', that
// provides operations to register both metric categories and individual
// metrics.  A metric is uniquely identified by its name and category, and the
// metric registry provides a mapping from those identifying properties to a
// 'baem_MetricId'.  A 'baem_MetricRegistry' object also provides a
// mapping from a category name to the address of a non-modifiable
// 'baem_Category' object.
//
///Thread Safety
///-------------
// 'baem_MetricRegistry' is fully *thread-safe*, meaning that all non-creator
// operations on a given object can be safely invoked simultaneously from
// multiple threads.
//
///Usage
///-----
// The following example illustrates how to create and use a
// 'baem_MetricRegistry'.  We start by creating a 'baem_MetricRegistry'
// object, 'registry', and then using this registry to create a
// 'baem_MetricId' for a metric named "MetricA" belonging to the category
// "MyCategory" (i.e., "MyCategory.MetricA").
//..
//  bslma_Allocator     *allocator = bslma_Default::allocator(0);
//  baem_MetricRegistry  registry(allocator);
//
//  baem_MetricId idA = registry.addId("MyCategory", "MetricA");
//..
// Now that we have added a metric id, "MyCategory.MetricA", attempting to add
// the metric id again will return an invalid id.  We retrieve the same
// identifier we have created using either 'getId' or 'findId':
//..
//  baem_MetricId invalidId = registry.addId("MyCategory", "MetricA");
//        assert(!invalidId.isValid());
//
//  baem_MetricId idA_copy1 = registry.getId("MyCategory", "MetricA");
//        assert(idA_copy1.isValid());
//        assert(idA_copy1 == idA);
//
//  baem_MetricId idA_copy2 = registry.findId("MyCategory", "MetricA");
//        assert(idA_copy2.isValid());
//        assert(idA_copy2 == idA);
//..
// We use the 'getId' method to add a new metric to the registry, then verify
// we can lookup the metric:
//..
//  baem_MetricId idB = registry.getId("MyCategory", "MetricB");
//        assert(idB.isValid());
//        assert(idB == registry.getId("MyCategory", "MetricB"));
//        assert(idB == registry.findId("MyCategory", "MetricB"));
//        assert(!registry.addId("MyCategory", "MetricB").isValid());
//..
// Next we use 'getCategory' to find the address of the 'baem_Category' object
// corresponding to "MyCategory":
//..
//  const baem_Category *myCategory = registry.getCategory("MyCategory");
//        assert(myCategory == idA.category());
//        assert(myCategory == idB.category());
//        assert(myCategory->isEnabled());
//..
// Finally we use the 'setCategoryEnabled' method to disable the category
// "MyCategory":
//..
//  registry.setCategoryEnabled(myCategory, false);
//        assert(!myCategory->isEnabled());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_CATEGORY
#include <baem_category.h>
#endif

#ifndef INCLUDED_BAEM_METRICDESCRIPTION
#include <baem_metricdescription.h>
#endif

#ifndef INCLUDED_BAEM_METRICID
#include <baem_metricid.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class baem_MetricFormat;

                      // =========================
                      // class baem_MetricRegistry
                      // =========================

class baem_MetricRegistry {
    // The class defines a thread-aware mechanism for registering metrics
    // and metric categories.  A metric is uniquely identified by its name
    // and category, and the metric registry provides a mapping from those
    // identifying properties to a 'baem_MetricId'.  A 'baem_MetricRegistry'
    // object also provides a mapping from a category name to the address of
    // a non-modifiable 'baem_Category' object.

    // PRIVATE TYPES
    typedef bsl::pair<const char *, const char *> CategoryAndName;
        // 'CategoryAndName' is an alias for a pair of null-terminated
        // constant strings that represent the category and name of a metric.
        // The first element is the category and the second is the name.

    struct CategoryAndNameLess {
        // This 'struct' defines an ordering on 'CategoryAndName' values
        // allowing them to be included in sorted containers such as
        // 'bsl::map'.  Note that the category and name strings are compared
        // by value.

        typedef bsl::pair<const char *, const char *> CategoryAndName;

        bool operator()(const CategoryAndName& lhs,
                        const CategoryAndName& rhs) const
            // Return 'true' if the value of the specified 'lhs' is less than
            // (ordered before) the value of the specified 'rhs', and 'false'
            // otherwise.  The 'lhs' value is considered less than
            // the 'rhs' value if the first value in the 'lhs' pair (the
            // category) is less than the first value in the 'rhs' pair or, if
            // the first values are equal, if the second value in the 'lhs'
            // pair (the name) is less than the second value in the 'rhs'
            // pair.
        {
            int cmp = bsl::strcmp(lhs.first, rhs.first);
            if (0 == cmp) {
                cmp = bsl::strcmp(lhs.second, rhs.second);
            }
            return cmp < 0;
        }
    };

    struct CStringLess {
        // This 'struct' defines an ordering on null-terminated character
        // strings, allowing them to be included in sorted containers such as
        // 'bsl::map'.  Note that the strings are compared by value.

        bool operator()(const char *lhs, const char *rhs) const
            // Return 'true' if the value of the specified 'lhs' is less than
            // (lexicographically ordered before) the value of the specified
            // 'rhs', and 'false' otherwise.
        {
            return bsl::strcmp(lhs, rhs) < 0;
        }
    };

    typedef bsl::map<CategoryAndName,
                     bcema_SharedPtr<baem_MetricDescription>,
                     CategoryAndNameLess>                MetricRegistry;
        // A 'MetricRegistry' is a type that maps a category and name to a
        // 'baem_MetricDescription' object address.

    typedef bsl::map<const char *,
                     bcema_SharedPtr<baem_Category>,
                     CStringLess>                        CategoryRegistry;
        // A 'CategoryRegistry' is a type that maps a name to a
        // 'baem_Category' object address.

    typedef bsl::map<const char *, bsl::vector<const void *>, CStringLess>
                                                          UserDataRegistry;
        // 'UserDataRegistry' is an alias for a type that maps a category (or
        // category prefix) to the user data set for that category (or group of
        // categories).

    // DATA
    bsl::set<bsl::string>  d_uniqueStrings;  // unique string memory

    CategoryRegistry       d_categories;     // map category -> 'baem_Category'

    MetricRegistry         d_metrics;        // map (category,name) -> MetricId

    bool                   d_defaultEnabled; // default enabled status

    UserDataRegistry       d_categoryUserData;
                                             // map category -> user data

    UserDataRegistry       d_categoryPrefixUserData;
                                             // map category-prefix ->
                                             // user data

    int                    d_nextKey;        // next valid user data key

    mutable bcemt_RWMutex  d_lock;           // read-write property lock

    bslma_Allocator       *d_allocator_p;    // allocator (held, not owned)

    // NOT IMPLEMENTED
    baem_MetricRegistry(const baem_MetricRegistry&);
    baem_MetricRegistry& operator=(const baem_MetricRegistry&);

  private:
    // PRIVATE MANIPULATORS
    bsl::pair<baem_MetricId, bool> insertId(const char *category,
                                            const char *name);
        // Insert a metric id having the specified 'category' and 'name' into
        // this metric registry.  Return a pair whose first member is the id
        // of the metric, and whose second member is 'true' if the returned
        // metric id is newly-created and 'false' otherwise.  The behavior is
        // undefined unless the calling thread has a *write* lock on 'd_lock'.

    void setCurrentUserData(const char                          *category,
                            baem_MetricDescription::UserDataKey  key,
                            const void                          *value);
        // Associate the specified 'value' with the specified 'key' for every
        // metric belonging to the specified 'category'.  Note that this
        // operation modifies existing metrics, but does not affect metrics
        // created after this method is called.  The behavior is undefined
        // unless the calling thread has a *write* lock on 'd_lock'.

    // PRIVATE ACCESSORS
    void defaultUserData(bsl::vector<const void *> *result,
                         const char                *categoryName) const;
        // Load into the specified 'result' the user data associated (via
        // 'setUserData') with a category having the specified 'categoryName'.
        // Each index position in 'result' will contain 0, or an (opaque)
        // application-specific data value provided by the client, either for
        // 'categoryName' or a prefix of 'categoryName'.  If there is more
        // than one non-null user-supplied data value applicable to an index
        // position in 'result', it is unspecified which value will be
        // returned.  The behavior is undefined unless the calling thread has a
        // lock on 'd_lock'.

  public:
    // PUBLIC TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baem_MetricRegistry,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    baem_MetricRegistry(bslma_Allocator *basicAllocator = 0);
        // Create an empty metric registry.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baem_MetricRegistry();
        // Destroy this metric registry.

    // MANIPULATORS
    baem_MetricId addId(const char *category, const char *name);
        // Add the specified 'category' and 'name' to this registry, unless it
        // has already been registered, and return a 'baem_MetricId' object
        // identifying the newly-registered metric.  If the indicated metric
        // has already been registered, the returned 'baem_MetricId' object
        // will *not* be valid (i.e., 'isValid' will return 'false').  The
        // behavior is undefined unless 'category' and 'name' are
        // null-terminated.

    baem_MetricId getId(const char *category, const char *name);
        // Return a 'baem_MetricId' object for the metric identified by the
        // specified 'category' and 'name'.  If no corresponding metric has
        // already been registered, register a new metric and return a
        // 'baem_MetricId' object identifying that newly-registered metric.
        // The behavior is undefined unless 'category' and 'name' are
        // null-terminated.  Note that this operation is guaranteed to return
        // a valid 'baem_MetricId' object.

    const baem_Category *addCategory(const char *category);
        // Add the specified 'category' to this registry, unless it has already
        // been registered.  Return the address of the newly-created
        // non-modifiable 'baem_Category' object on success, and 0 otherwise.
        // The behavior is undefined unless 'category' is null-terminated.

    const baem_Category *getCategory(const char *category);
        // Return the address of the non-modifiable 'baem_Category' object for
        // the specified 'category'.  If no corresponding category exists,
        // register a new category and return the address of the newly-created
        // 'baem_Category' object.  The behavior is undefined unless 'category'
        // is null-terminated.  Note that this operation is guaranteed to
        // return a valid address.

    void setCategoryEnabled(const baem_Category* category,
                            bool                 value);
        // Set whether the specified 'category' is enabled to the specified
        // 'value'.  The behavior is undefined unless 'category' is a
        // valid address of a category previously returned by this metric
        // registry.  Note that this operation is thread-safe, but *not*
        // atomic: Other threads may simultaneously access the current enabled
        // value for 'category' while this operation completes.  Also note
        // that this operation has *linear* runtime performance with respect
        // to the number of registered category holders for 'category'.

     void setAllCategoriesEnabled(bool value);
        // Set whether each currently registered category is enabled to the
        // specified 'value', and ensure that categories registered
        // after this call are initialized as either enabled or disabled,
        // accordingly.  This operation is logically equivalent to iterating
        // over the list of currently registed categories and calling
        // 'setCategoryEnabled' on each category individually, and also
        // setting a default 'enabled' value (for newly-created
        // categories).  Hence, subsequent calls 'setCategoryEnabled' will
        // override this value for a particular category.  Note that this
        // operation is thread-safe, but *not* atomic: Other threads may
        // simultaneously access the current enabled status for registered
        // categories while this operation completes.  Also note that this
        // operation has *linear* runtime performance with respect to the
        // total number of category holders registered with this repository.

     void registerCategoryHolder(const baem_Category *category,
                                 baem_CategoryHolder *holder);
        // Load into the specified  'holder' the address of the specified
        // 'category', its 'enabled' status, and the address of the next holder
        // in the linked list of category holders maintained by 'category'
        // (prepending 'holder' to the linked list of category holders for
        // 'category').  The supplied 'category' will update the value returned
        // by 'holder->enabled()' when its enabled state changes, and will
        // reset 'holder' (i.e., 'holder->reset()') when 'category' is
        // destroyed.  The behavior is undefined unless 'holder' remains valid
        // and *unmodified* (by the client) for the lifetime of this object.
        //
        // This method should *not* be used directly by client code.  It is an
        // implementation detail of the 'baem' metric collection system.

    void setPreferredPublicationType(const baem_MetricId&        metric,
                                     baem_PublicationType::Value type);
        // Set the preferred publication type of the specified 'metric' to the
        // specified 'type'.  The preferred publication type of a metric
        // indicates the preferred aggregate to publish for that metric, or
        // 'baem_PublicationType::UNSPECIFIED' if there is no preference.  The
        // behavior is undefined unless 'metric' was previously returned by
        // this metric registry.  Note that there is no uniform definition for
        // how publishers will interpret this value; an 'UNSPECIFIED' value
        // generally indicates that the all the collected aggregates (total,
        // count, minimum, and maximum value) should be published.  Also note
        // that the preferred publication type is accessed through the
        // 'baem_MetricDescription' (i.e.,
        // 'metric.description()->preferredPublicationType()').

    void setFormat(const baem_MetricId&     metricId,
                   const baem_MetricFormat& format);
        // Set the format for the specified 'metricId' to the specified
        // 'format'.  Note that there is no uniform specification for how
        // publisher implementations will interpret the supplied 'format'.
        // Also note that the format for a metric is accessed through
        // the 'baem_MetricDescription'.  For example:
        //..
        //  metric.description()->format();
        //..

    baem_MetricDescription::UserDataKey createUserDataKey();
        // Return a new unique key that can be used to associate (via
        // 'setUserData') and retrieve (via 'userData') a value with a metric
        // (or group of metrics).  Note that the returned key can be used by
        // clients of 'baem' to associate additional information with a metric.

    void setUserData(const baem_MetricId&                 metricId,
                     baem_MetricDescription::UserDataKey  key,
                     const void                          *value);
        // Associate the specified 'value' with the specified 'key' in the
        // description of the specified 'metricId'.  The behavior is undefined
        // unless 'key' was previously returned from 'createUserDataKey'.  Note
        // that this method allows clients of 'baem' to associate (opaque)
        // application-specific information with a metric.

    void setUserData(const char                          *categoryName,
                     baem_MetricDescription::UserDataKey  key,
                     const void                          *value,
                     bool                                 prefixFlag = false);
        // Associate the specified 'value' with the specified 'key' in any
        // metric belonging to a category having the specified 'categoryName',
        // or a category whose name begins with 'categoryName', as determined
        // by the optionally specified 'prefixFlag'.  If prefixFlag is 'false'
        // or is not specified, only those metrics belonging to a category
        // having 'categoryName' will be mapped; otherwise, 'value' will be
        // associated with 'key' for all metrics belonging to any category
        // whose name begins with 'categoryName'.  This association applies to
        // existing metrics as well as any subsequently created ones.  When a
        // metric is created that matches more than one registered category
        // prefix, it is not specified which supplied value will be associated
        // with 'key', unless only one of those values is non-null, in which
        // case the unique non-null value is used.  The behavior is undefined
        // unless 'key' was previously returned from 'createUserDataKey'.

    // ACCESSORS
    bsl::size_t numMetrics() const;
        // Return the number of metrics in this registry.

    bsl::size_t numCategories() const;
        // Return the number of categories in this registry.

    const baem_Category *findCategory(const char *category) const;
        // Find the specified 'category' in this registry.  Return
        // the address of the non-modifiable 'baem_Category' object
        // corresponding to the specified 'category', or 0 if no such category
        // has been registered.  The behavior is undefined unless 'category' is
        // null-terminated.

    baem_MetricId findId(const char *category, const char *name) const;
        // Find the specified 'category' and 'name' in this registry.  Return
        // the 'baem_MetricId' object corresponding to the metric having the
        // specified 'category' and 'name', if found, or an invalid metric id
        // if no such metric has been registered (i.e., 'isValid' will return
        // 'false').  The behavior is undefined unless 'category' and 'name'
        // are null-terminated.

    void getAllCategories(
                         bsl::vector<const baem_Category *> *categories) const;
        // Append to the specified 'categories' the addresses of all the
        // categories registered by this 'baem_MetricRegistry' object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -------------------------
                          // class baem_MetricRegistry
                          // -------------------------

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
