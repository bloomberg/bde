// balm_metricdescription.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_METRICDESCRIPTION
#define INCLUDED_BALM_METRICDESCRIPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a description for a metric.
//
//@CLASSES:
//   balm::MetricDescription: describes a metric
//
//@SEE_ALSO: balm_metricregistry, balm_metricid, balm_category
//
//@DESCRIPTION: This component provides a class, 'balm::MetricDescription',
// used to describe a metric.  A 'balm::MetricDescription' object contains the
// address of the category to which the metric belongs and also the address of
// the null-terminated string holding the name of the metric.  The
// 'balm::MetricDescription' class suppresses copy construction and assignment,
// and does not provide equality operators: Applications should use a *single*
// 'balm::MetricDescription' object per metric (such as one provided by the
// *'balm::MetricRegistry'* component).
//
// IMPORTANT: The metric description's 'name', whose type is 'const char *',
// must remain constant and valid throughout the lifetime of the
// 'balm::MetricDescription' object.
//
///Thread Safety
///-------------
// 'balm::MetricDescription' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'balm::MetricDescription' in one thread while another
// thread modifies the same object.  However, clients of the 'balm' package
// accessing a non-modifiable 'balm::MetricDescription' supplied by a
// 'balm::MetricRegistry' (by way of a 'balm::MetricId') can safely access the
// properties of that metric description at any time.
//
///Usage
///-----
// The following example demonstrates how to create and access a
// 'balm::MetricDescription' object.  We start by creating a category:
//..
//  balm::Category myCategory("MyCategory");
//..
// Then we use that category to create three metric description objects with
// different names:
//..
//  balm::MetricDescription metricA(&myCategory, "A");
//  balm::MetricDescription metricB(&myCategory, "B");
//  balm::MetricDescription metricC(&myCategory, "C");
//..
// We can use the 'category' and 'name' methods to access their values:
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

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_PUBLICATIONTYPE
#include <balm_publicationtype.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {


namespace balm {

class Category;
class MetricFormat;

                          // =======================
                          // class MetricDescription
                          // =======================

class MetricDescription {
    // This class provides a mechanism for describing a metric.  A
    // 'MetricDescription' holds the category to which the metric belongs, and
    // a null-terminated string containing the name of the metric.

    // DATA
    const Category *d_category_p;  // category of metric (held, not owned)

    const char     *d_name_p;      // name of metric (held, not owned)

    PublicationType::Value
                    d_preferredPublicationType;
                                   // preferred publication type

    bsl::shared_ptr<const MetricFormat>
                    d_format;      // format for this metric

    bsl::vector<const void *>
                    d_userData;    // user data, indexed by keys

    mutable bslmt::Mutex
                    d_mutex;       // synchronize non-const elements
                                   // (publication type, format, user data)

    // NOT IMPLEMENTED
    MetricDescription(const MetricDescription&);
    MetricDescription& operator=(const MetricDescription&);

  public:
    // PUBLIC TYPES
    typedef int UserDataKey;
        // A key used to refer to a data value associated with a metric.  Note
        // that a 'UserDataKey' can be used by clients of 'balm' to associate
        // additional information with a metric.  See 'balm_metricregistry'
        // for information on obtaining a unique key.

    // CREATORS
    MetricDescription(const Category   *category,
                      const char       *name,
                      bslma::Allocator *basicAllocator = 0);
        // Create a metric description for the specified 'category' and the
        // specified 'name'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The initial value for
        // 'preferredPublicationType' is 'e_UNSPECIFIED', and the initial
        // value for 'format' is 0.  The behavior is undefined unless 'name'
        // and 'category' remain valid, and the contents of 'name' remain
        // unmodified, for the lifetime of this object.

    // ~MetricDescription();
        // Destroy this metric description.  Note that this trivial destructor
        // is generated by the compiler.

    // MANIPULATORS
    void setName(const char *name);
        // Set the name of this metric description to the specified 'name'.
        // The behavior is undefined unless the contents of 'name' remains
        // valid and unmodified for the lifetime of this object.

    void setCategory(const Category *category);
        // Set the category of this metric description to the object at the
        // specified 'category' address.  The behavior is undefined unless
        // 'category' remains valid for the lifetime of this object.

    void setPreferredPublicationType(PublicationType::Value type);
        // Set the preferred publication type of this metric to the specified
        // 'type'.  The preferred publication type of this metric indicates the
        // preferred aggregate to publish for this metric, or
        // 'PublicationType::UNSPECIFIED' if there is no preference.  Note
        // that there is no uniform definition for how publishers will
        // interpret this value; an 'UNSPECIFIED' value generally indicates
        // that all the collected aggregates (total, count, minimum, and
        // maximum value) should be published.

    void setFormat(const bsl::shared_ptr<const MetricFormat>& format);
        // Set the format for this metric description to the specified
        // 'format'.

    void setUserData(UserDataKey key, const void *value);
        // Associate the specified 'value' with the specified data 'key'.  The
        // behavior is undefined unless 'key >= 0'.  Note that this method
        // allows clients of 'balm' to associate (opaque) application-specific
        // information with a metric.

    // ACCESSORS
    const char *name() const;
        // Return the address of the non-modifiable, null-terminated string
        // containing the name of the described metric.

    const Category *category() const;
        // Return the address of the non-modifiable category object indicating
        // the category of the metric described by this object.

    PublicationType::Value preferredPublicationType() const;
        // Return the preferred publication type of this metric.  The
        // preferred publication type of this metric indicates the preferred
        // aggregate to publish for this metric, or
        // 'PublicationType::UNSPECIFIED' if there is no preference.  Note
        // that there is no uniform definition for how publishers will
        // interpret this value; an 'UNSPECIFIED' value generally indicates
        // that the all the collected aggregates (total, count, minimum, and
        // maximum value) should be published.

    bsl::shared_ptr<const MetricFormat> format() const;
        // Return a shared pointer to the non-modifiable format for this
        // metric description.  Note that the returned shared pointer *may*
        // *be* *null* if a format has not been provided for the described
        // metric.

    const void *userData(UserDataKey key) const;
        // Return the non-modifiable value associated with the specified
        // user-data 'key'.  If the data for 'key' has not been set, a value of
        // 0 is returned, which is indistinguishable from a valid 'key' with a
        // 0 value.  The behavior is undefined unless 'key >= 0'.    Note that
        // this method allows clients of 'balm' to access the (opaque)
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
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const MetricDescription& rhs);
    // Write a formatted single-line description of the specified 'rhs' metric
    // description to the specified 'stream', and return a reference to the
    // modifiable 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class MetricDescription
                          // -----------------------

// CREATORS
inline
MetricDescription::MetricDescription(const Category   *category,
                                     const char       *name,
                                     bslma::Allocator *basicAllocator)
: d_category_p(category)
, d_name_p(name)
, d_preferredPublicationType(PublicationType::e_UNSPECIFIED)
, d_format()
, d_userData(basicAllocator)
, d_mutex()
{
}

// MANIPULATORS
inline
void MetricDescription::setName(const char *name)
{
    d_name_p = name;
}

inline
void MetricDescription::setCategory(const Category *category)
{
    d_category_p = category;
}

inline
void MetricDescription::setPreferredPublicationType(
                                              PublicationType::Value type)
{
    // This guard is not strictly required on any supported platform.
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_preferredPublicationType = type;
}

inline
void MetricDescription::setFormat(
                        const bsl::shared_ptr<const MetricFormat>& format)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_format = format;
}

inline
void MetricDescription::setUserData(UserDataKey key, const void *value)
{
    BSLS_ASSERT_SAFE(key >= 0);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    if ((unsigned int)key >= d_userData.size()) {
        d_userData.resize(key + 1, 0);
    }
    d_userData[key] = value;
}

// ACCESSORS
inline
const char *MetricDescription::name() const
{
    return d_name_p;
}

inline
const Category *MetricDescription::category() const
{
    return d_category_p;
}

inline
PublicationType::Value
MetricDescription::preferredPublicationType() const
{
    // This guard is not strictly required on any supported platform.
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_preferredPublicationType;
}

inline
bsl::shared_ptr<const MetricFormat>
MetricDescription::format() const

{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_format;
}

inline
const void *MetricDescription::userData(UserDataKey key) const
{
    BSLS_ASSERT_SAFE(key >= 0);
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return ((unsigned int)key < d_userData.size()) ? d_userData[key] : 0;
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& balm::operator<<(bsl::ostream&      stream,
                         const MetricDescription& rhs)
{
    return rhs.print(stream);
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
