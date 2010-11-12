// baem_metricdescription.h                                           -*-C++-*-
#ifndef INCLUDED_BAEM_METRICDESCRIPTION
#define INCLUDED_BAEM_METRICDESCRIPTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a description for a metric.
//
//@CLASSES:
//   baem_MetricDescription: describes a metric
//
//@SEE_ALSO: baem_metricregistry, baem_metricid, baem_category
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a class, 'baem_MetricDescription', used
// to describe a metric.  A 'baem_MetricDescription' object contains the
// address of the category to which the metric belongs and also the address of
// the null-terminated string holding the name of the metric.  The
// 'baem_MetricDescription' class suppresses copy construction and assignment,
// and does not provide equality operators: Applications should use a *single*
// 'baem_MetricDescription' object per metric (such as one provided by the
// *'baem_MetricRegistry'* component).
//
// IMPORTANT: The metric description's 'name', whose type is 'const char *',
// must remain constant and valid throughout the lifetime of the
// 'baem_MetricDescription' object.
//
///Thread Safety
///-------------
// 'baem_MetricDescription' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'baem_MetricDescription' in one thread while another
// thread modifies the same object.  However, clients of the 'baem' package
// accessing a non-modifiable 'baem_MetricDescription' supplied by a
// 'baem_MetricRegistry' (by way of a 'baem_MetricId') can safely access the
// properties of that metric description at any time.
//
///Usage
///-----
// The following example demonstrates how to create and access a
// 'baem_MetricDescription' object.  We start by creating a category:
//..
//  baem_Category myCategory("MyCategory");
//..
// Then we use that category to create three metric description objects with
// different names:
//..
//  baem_MetricDescription metricA(&myCategory, "A");
//  baem_MetricDescription metricB(&myCategory, "B");
//  baem_MetricDescription metricC(&myCategory, "C");
//..
// Once the metric description objects have been created we can use the
// 'category' and 'name' methods to access their values:
//..
//  assert(&myCategory == metricA.category());
//  assert(&myCategory == metricB.category());
//  assert(&myCategory == metricC.category());
//
//  assert(0 == bsl::strcmp("A", metricA.name()));
//  assert(0 == bsl::strcmp("B", metricB.name()));
//  assert(0 == bsl::strcmp("C", metricC.name()));
//..
// Finally, we write all three metric descriptions to the console:
//..
//  bsl::cout << "metricA: " << metricA << bsl::endl
//            << "metricB: " << metricB << bsl::endl
//            << "metricC: " << metricC << bsl::endl;
//..
// With the following console output:
//..
//  metricA: MyCategory.A
//  metricB: MyCategory.B
//  metricC: MyCategory.C
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_PUBLICATIONTYPE
#include <baem_publicationtype.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class baem_Category;
class baem_MetricFormat;

                          // ============================
                          // class baem_MetricDescription
                          // ============================

class baem_MetricDescription {
    // This class provides a mechanism for describing a metric.  A
    // 'baem_MetricDescription' holds the category to which the metric
    // belongs, and a null-terminated string containing the name of the metric.

    // DATA
    const baem_Category *d_category_p;  // category of metric (held, not
                                        // owned)

    const char          *d_name_p;      // name of metric (held, not owned)

    baem_PublicationType::Value
                         d_preferredPublicationType;
                                        // preferred publication type

    bcema_SharedPtr<const baem_MetricFormat>
                         d_format;      // format for this metric

    bsl::vector<const void *>
                         d_userData;    // user data, indexed by keys

    mutable bcemt_Mutex  d_mutex;       // synchronize non-const elements
                                        // (publication type, format, user
                                        // data)

    // NOT IMPLEMENTED
    baem_MetricDescription(const baem_MetricDescription&);
    baem_MetricDescription& operator=(const baem_MetricDescription&);

  public:
    // PUBLIC TYPES
    typedef int UserDataKey;
        // A key used to refer to a data value associated with a metric.  Note
        // that a 'UserDataKey' can be used by clients of 'baem' to associate
        // additional information with a metric.  See 'baem_metricregistry'
        // for information on obtaining a unique key.

    // CREATORS
    baem_MetricDescription(const baem_Category *category,
                           const char          *name,
                           bslma_Allocator     *basicAllocator = 0);
        // Create a metric description for the specified 'category' and the
        // specified 'name'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The initial value for
        // 'preferredPublicationType' is 'BAEM_UNSPECIFIED', and the initial
        // value for 'format' is 0.  The behavior is undefined unless 'name'
        // and 'category' remain valid, and the contents of 'name' remain
        // unmodified, for the lifetime of this object.

    // ~baem_MetricDescription();
        // Destroy this metric description.  Note that this trivial destructor
        // is generated by the compiler.

    // MANIPULATORS
    void setName(const char *name);
        // Set the name of this metric description to the specified 'name'.
        // The behavior is undefined unless the contents of 'name' remains
        // valid and unmodified for the lifetime of this object.

    void setCategory(const baem_Category *category);
        // Set the category of this metric description to the object at the
        // specified 'category' address.  The behavior is undefined unless
        // 'category' remains valid for the lifetime of this object.

    void setPreferredPublicationType(baem_PublicationType::Value type);
        // Set the preferred publication type of this metric to the specified
        // 'type'.  The preferred publication type of this metric indicates the
        // preferred aggregate to publish for this metric, or
        // 'baem_PublicationType::UNSPECIFIED' if there is no preference.  Note
        // that there is no uniform definition for how publishers will
        // interpret this value; an 'UNSPECIFIED' value generally indicates
        // that all the collected aggregates (total, count, minimum, and
        // maximum value) should be published.

    void setFormat(const bcema_SharedPtr<const baem_MetricFormat>& format);
        // Set the format for this metric description to the specified
        // 'format'.

    void setUserData(UserDataKey key, const void *value);
        // Associate the specified 'value' with the specified data 'key'.  The
        // behavior is undefined unless 'key >= 0'.  Note that this method
        // allows clients of 'baem' to associate (opaque) application-specific
        // information with a metric.

    // ACCESSORS
    const char *name() const;
        // Return the address of the non-modifiable, null-terminated string
        // containing the name of the described metric.

    const baem_Category *category() const;
        // Return the address of the non-modifiable category object indicating
        // the category of the metric described by this object.

    baem_PublicationType::Value preferredPublicationType() const;
        // Return the preferred publication type of this metric.  The
        // preferred publication type of this metric indicates the preferred
        // aggregate to publish for this metric, or
        // 'baem_PublicationType::UNSPECIFIED' if there is no preference.  Note
        // that there is no uniform definition for how publishers will
        // interpret this value; an 'UNSPECIFIED' value generally indicates
        // that the all the collected aggregates (total, count, minimum, and
        // maximum value) should be published.

    bcema_SharedPtr<const baem_MetricFormat> format() const;
        // Return a shared pointer to the non-modifiable format for this
        // metric description.  Note that the returned shared pointer *may*
        // *be* *null* if a format has not been provided for the described
        // metric.

    const void *userData(UserDataKey key) const;
        // Return the non-modifiable value associated with the specified
        // user-data 'key'.  If the data for 'key' has not been set, a value of
        // 0 is returned, which is indistinguishable from a valid 'key' with a
        // 0 value.  The behavior is undefined unless 'key >= 0'.    Note that
        // this method allows clients of 'baem' to access the (opaque)
        // application-specific information that they have previously
        // associated with a metric (via 'setUserData').

    bsl::ostream& print(bsl::ostream& stream) const;
        // Print the category and name of this metric description to the
        // specified output 'stream' in some single-line human-readable form,
        // and return a reference to the modifiable 'stream'.

    bsl::ostream& printDescription(bsl::ostream& stream) const;
        // Print the properties of this metric description to the specified
        // output 'stream' in some single-line human-readable form, and return
        // a reference to the modifiable 'stream'.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const baem_MetricDescription& rhs);
    // Write a formatted single-line description of the specified 'rhs' metric
    // description to the specified 'stream', and return a reference to
    // the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ----------------------------
                       // class baem_MetricDescription
                       // ----------------------------

// CREATORS
inline
baem_MetricDescription::baem_MetricDescription(
                                           const baem_Category *category,
                                           const char          *name,
                                           bslma_Allocator     *basicAllocator)
: d_category_p(category)
, d_name_p(name)
, d_preferredPublicationType(baem_PublicationType::BAEM_UNSPECIFIED)
, d_format()
, d_userData(basicAllocator)
, d_mutex()
{
}

// MANIPULATORS
inline
void baem_MetricDescription::setName(const char *name)
{
    d_name_p = name;
}

inline
void baem_MetricDescription::setCategory(const baem_Category *category)
{
    d_category_p = category;
}

inline
void baem_MetricDescription::setPreferredPublicationType(
                                              baem_PublicationType::Value type)
{
    // This guard is not strictly required on any supported platform.
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_preferredPublicationType = type;
}

inline
void baem_MetricDescription::setFormat(
                        const bcema_SharedPtr<const baem_MetricFormat>& format)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    d_format = format;
}

inline
void baem_MetricDescription::setUserData(UserDataKey key,  const void *value)
{
    BSLS_ASSERT_SAFE(key >= 0);

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    if ((unsigned int)key >= d_userData.size()) {
        d_userData.resize(key + 1, 0);
    }
    d_userData[key] = value;
}

// ACCESSORS
inline
const char *baem_MetricDescription::name() const
{
    return d_name_p;
}

inline
const baem_Category *baem_MetricDescription::category() const
{
    return d_category_p;
}

inline
baem_PublicationType::Value
baem_MetricDescription::preferredPublicationType() const
{
    // This guard is not strictly required on any supported platform.
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_preferredPublicationType;
}

inline
bcema_SharedPtr<const baem_MetricFormat>
baem_MetricDescription::format() const

{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_format;
}

inline
const void *baem_MetricDescription::userData(UserDataKey key) const
{
    BSLS_ASSERT_SAFE(key >= 0);
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return ((unsigned int)key < d_userData.size()) ? d_userData[key] : 0;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const baem_MetricDescription& rhs)
{
    return rhs.print(stream);
}

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
