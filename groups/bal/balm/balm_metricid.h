// balm_metricid.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_METRICID
#define INCLUDED_BALM_METRICID

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: balm_metricid.h,v 1.4 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide an identifier for a metric.
//
//@CLASSES:
//   balm::MetricId: an identifier for a metric
//
//@SEE_ALSO: balm_metricregistry, balm_metricdescription, balm_category
//
//@DESCRIPTION: This component implements an in-core value-semantic type used
// to identify a metric.  A 'balm::MetricId' object's value is the *address* of
// a (non-modifiable) 'balm::MetricDescription' object.  A 'balm::MetricId'
// object also provides auxiliary methods, 'category', 'categoryName', and
// 'metricName', that enables access to the properties of the held
// 'balm::MetricDescription'.  Two 'balm::MetricId' objects have the same value
// if the values of their respective 'balm::MetricDescription' object
// *addresses* are the same.
//
///Thread Safety
///-------------
// 'balm::MetricId' is *const* *thread-safe*, meaning that accessors may be
// invoked concurrently from different threads, but it is not safe to access
// or modify a 'balm::MetricId' in one thread while another thread modifies the
// same object.
//
///Usage
///-----
// The following example demonstrates how to create and use a 'balm::MetricId'
// object.  We start by creating a category and two metric description objects:
//..
//  balm::Category category("MyCategory");
//  balm::MetricDescription descriptionA(&category, "MetricA");
//  balm::MetricDescription descriptionB(&category, "MetricB");
//..
// Now we create three 'balm::MetricId' objects:
//..
//  balm::MetricId invalidId;
//  balm::MetricId metricIdA(&descriptionA);
//  balm::MetricId metricIdB(&descriptionB);
//..
// We can access and verify the properties of the 'balm::MetricId' objects we
// have created:
//..
//  assert(false == invalidId.isValid());
//  assert(true  == metricIdA.isValid());
//  assert(true  == metricIdB.isValid());
//
//  assert(0             == invalidId.description());
//  assert(&descriptionA == metricIdA.description());
//  assert(&descriptionB == metricIdB.description());
//
//  assert(invalidId != metricIdA);
//  assert(invalidId != metricIdB);
//  assert(metricIdA != metricIdB);
//..
// We now verify that copies of a metric id have the same value as the
// original:
//..
//  balm::MetricId copyMetricIdA(metricIdA);
//
//  assert(metricIdA == copyMetricIdA);
//..
// Note that two 'balm::MetricId' objects that have different
// 'balm::MetricDescription' object addresses are *not* equal, *even* if the
// descriptions have the same name and category.
//..
//  balm::MetricDescription newDescriptionB(&category, "MetricB");
//  balm::MetricId          differentIdB(&newDescriptionB);
//
//  assert(0 == bsl::strcmp(differentIdB.metricName(),metricIdB.metricName()));
//  assert(differentIdB.category() == metricIdB.category());
//
//  assert(metricIdB != differentIdB);   // The 'balm::MetricDescription'
//                                       // object addresses are not equal!
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_CATEGORY
#include <balm_category.h>
#endif

#ifndef INCLUDED_BALM_METRICDESCRIPTION
#include <balm_metricdescription.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace balm {
                               // ==============
                               // class MetricId
                               // ==============

class MetricId {
    // This class defines an in-core value-semantic type used to identify a
    // metric.  A 'MetricId' object's value is the *address* of a
    // non-modifiable 'MetricDescription' object.  In addition, a
    // 'MetricDescription' object provides auxiliary methods, 'category',
    // 'categoryName', and 'name', that enable access to the properties of the
    // held 'MetricDescription'.  Two 'MetricId' objects have the same value if
    // the values of their respective 'MetricDescription' object *addresses*
    // are the same.

    // DATA
    const MetricDescription *d_description_p;  // metric description
                                               // (held, not owned)
  public:
    // CREATORS
    MetricId();
        // Create an invalid metric id.

    MetricId(const MetricDescription *metricDescription);
        // Create a metric id whose value is the specified 'metricDescription'
        // address.  The behavior is undefined unless 'metricDescription' is
        // 0, or the indicated metric description object remains valid for the
        // lifetime of this metric id object.

    MetricId(const MetricId& original);
        // Create a metric id with the value of the specified 'original'
        // metric id.  The behavior is undefined unless the 'description()' of
        // 'original' is 0, or the indicated metric description object remains
        // valid for the lifetime of this metric id object.

    // ~MetricId();
        // Destroy this metric id.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    MetricId& operator=(const MetricId& rhs);
        // Assign this metric id the value of the specified 'rhs' metric id
        // object, and return a reference to this object.  The behavior is
        // undefined unless the 'description()' of 'rhs' is 0, or the
        // metric-description object remains valid for the lifetime of this
        // metric-id object.

    const MetricDescription *&description();
        // Return a reference to the modifiable address of the *held*
        // non-modifiable 'MetricDescription', which defines the value of this
        // metric id.

     // ACCESSORS
    const MetricDescription * const & description() const;
        // Return a reference to the non-modifiable address of the *held*
        // non-modifiable 'MetricDescription', which defines the value of this
        // metric id.

    bool isValid() const;
        // Return 'true' if this metric id object contains the address of a
        // valid 'MetricDescription' object, and false 'otherwise'.  Note that
        // this method is logically equivalent to '0 != description()'.

    const Category *category() const;
        // Return the address of the non-modifiable 'Category' object
        // identifying the category with which this metric id is associated.
        // The behavior is undefined unless 'isValid()' is 'true'.  Note that
        // this method is logically equivalent to 'description()->category()'.

    const char *categoryName() const;
        // Return the address of the non-modifiable name for the category with
        // which this metric id is associated.  The behavior is undefined
        // unless 'isValid()' is 'true'.  Note that this method is logically
        // equivalent to 'description()->category()->name()'.

    const char *metricName() const;
        // Return the address of the non-modifiable name of this metric id.
        // The behavior is undefined unless 'isValid()' is 'true'.  Note that
        // this method is logically equivalent to 'description()->name()'.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Print this metric id to the specified output 'stream' in some
        // single-line human-readable form, and return a reference to the
        // modifiable 'stream'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bool operator==(const MetricId& lhs, const MetricId& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric ids have the same
    // value and 'false' otherwise.  Two metric ids have the same value if they
    // each hold the address of the same 'MetricDescription' object, or both
    // addresses are 0.

inline
bool operator!=(const MetricId& lhs, const MetricId& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric ids do not have
    // the same value and 'false' otherwise.  Two metric ids do not have same
    // value if they refer to different 'MetricDescription' object *addresses*,
    // or exactly one address is 0.

inline
bool operator<(const MetricId& lhs, const MetricId& rhs);
    // Return 'true' if the specified 'lhs' metric id is less than (ordered
    // before) the specified 'rhs' metric id, and return 'false' otherwise'.
    // The 'lhs' is considered less if the (unsigned) *address* of the
    // contained 'MetricDescription' object is numerically less than the 'rhs'
    // description.  Note that this function does *not* provide any form of
    // alphabetic ordering, and is logically equivalent to the expression
    // 'lhs.description() < rhs.description()'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const MetricId& rhs);
    // Write a description of the specified 'rhs' metric id to the specified
    // 'stream' in some human-readable format, and return a reference to the
    // modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // --------------
                               // class MetricId
                               // --------------

// CREATORS
inline
MetricId::MetricId()
: d_description_p(0)
{
}

inline
MetricId::MetricId(const MetricDescription *metricDescription)
: d_description_p(metricDescription)
{
}

inline
MetricId::MetricId(const MetricId& original)
: d_description_p(original.d_description_p)
{
}

// MANIPULATORS
inline
MetricId& MetricId::operator=(const MetricId& rhs)
{
    d_description_p = rhs.d_description_p;
    return *this;
}

inline
const MetricDescription *&MetricId::description()
{
    return d_description_p;
}

// ACCESSORS
inline
const MetricDescription * const & MetricId::description() const
{
    return d_description_p;
}

inline
bool MetricId::isValid() const
{
    return 0 != d_description_p;
}

inline
const Category *MetricId::category() const
{
    return d_description_p->category();
}

inline
const char *MetricId::categoryName() const
{
    return d_description_p->category()->name();
}

inline
const char *MetricId::metricName() const
{
    return d_description_p->name();
}
}  // close package namespace

// FREE OPERATORS
inline
bool balm::operator==(const MetricId& lhs, const MetricId& rhs)
{
    return lhs.description() == rhs.description();
}

inline
bool balm::operator!=(const MetricId& lhs, const MetricId& rhs)
{
    return !(lhs == rhs);
}

inline
bool balm::operator<(const MetricId& lhs, const MetricId& rhs)
{
    return lhs.description() < rhs.description();
}

inline
bsl::ostream& balm::operator<<(bsl::ostream& stream, const MetricId& rhs)
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
