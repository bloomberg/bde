// baem_metricid.h                                                    -*-C++-*-
#ifndef INCLUDED_BAEM_METRICID
#define INCLUDED_BAEM_METRICID

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: baem_metricid.h,v 1.4 2008/04/16 20:00:49 hversche Exp $")

//@PURPOSE: Provide an identifier for a metric.
//
//@CLASSES:
//   baem_MetricId: an identifier for a metric
//
//@SEE_ALSO: baem_metricregistry, baem_metricdescription, baem_category
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component implements an in-core value-semantic type used
// to identify a metric.  A 'baem_MetricId' object's value is the *address* of
// a (non-modifiable) 'baem_MetricDescription' object.  A 'baem_MetricId'
// object also provides auxiliary methods, 'category', 'categoryName', and
// 'metricName', that enables access to the properties of the held
// 'baem_MetricDescription'.  Two 'baem_MetricId' objects have the same value
// if the values of their respective 'baem_MetricDescription' object
// *addresses* are the same.
//
///Thread Safety
///-------------
// 'baem_MetricId' is *const* *thread-safe*, meaning that accessors may be
// invoked concurrently from different threads, but it is not safe to
// access or modify a 'baem_MetricId' in one thread while another thread
// modifies the same object.
//
///Usage
///-----
// The following example demonstrates how to create and use a 'baem_MetricId'
// object.  We start by creating a category and two metric description objects:
//..
//  baem_Category category("MyCategory");
//  baem_MetricDescription descriptionA(&category, "MetricA");
//  baem_MetricDescription descriptionB(&category, "MetricB");
//..
// Now we create three 'baem_MetricId' objects, representing distinct values:
//..
//  baem_MetricId invalidId;
//  baem_MetricId metricIdA(&descriptionA);
//  baem_MetricId metricIdB(&descriptionB);
//..
// We can access and verify the properties of the 'baem_MetricId' objects
// we have created:
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
//  baem_MetricId copyMetricIdA(metricIdA);
//
//  assert(metricIdA == copyMetricIdA);
//..
// Note that two 'bae_MetricId' objects that have different
// 'baem_MetricDescription' object addresses are *not* equal, *even* if the
// descriptions have the same name and category.
//..
//  baem_MetricDescription newDescriptionB(&category, "MetricB");
//  baem_MetricId          differentIdB(&newDescriptionB);
//
//  assert(0 == bsl::strcmp(differentIdB.metricName(),metricIdB.metricName()));
//  assert(differentIdB.category() == metricIdB.category());
//
//  assert(metricIdB != differentIdB);   // The 'baem_MetricDescription'
//                                       // object addresses are not equal!
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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                          // ===================
                          // class baem_MetricId
                          // ===================

class baem_MetricId {
    // This class defines an in-core value-semantic type used to identify a
    // metric.  A 'baem_MetricId' object's value is the *address* of a
    // non-modifiable 'baem_MetricDescription' object.  In addition, a
    // 'baem_MetricDescription' object provides auxiliary methods, 'category',
    // 'categoryName', and 'name', that enable access to the properties of
    // the held 'baem_MetricDescription'.  Two 'baem_MetricId' objects
    // have the same value if the values of their respective
    // 'baem_MetricDescription' object *addresses* are the same.

    // DATA
    const baem_MetricDescription *d_description_p;  // metric description
                                                    // (held, not owned)
  public:
    // CREATORS
    baem_MetricId();
        // Create an invalid metric id.

    baem_MetricId(const baem_MetricDescription *metricDescription);
        // Create a metric id whose value is the specified
        // 'metricDescription' address.  The behavior is undefined unless
        // 'metricDescription' is 0, or the indicated metric description object
        // remains valid for the lifetime of this metric id object.

    baem_MetricId(const baem_MetricId& original);
        // Create a metric id with the value of the specified 'original'
        // metricid.  The behavior is undefined unless the 'description()' of
        // 'original' is 0, or the indicated metric description object remains
        // valid for the lifetime of this metric id object.

    // ~baem_MetricId();
        // Destroy this metric id.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    baem_MetricId& operator=(const baem_MetricId& rhs);
        // Assign this metric id the value of the specified 'rhs' metric id
        // object, and return a reference to this object.  The behavior is
        // undefined unless the 'description()' of 'rhs' is 0, or the
        // metric-description object remains valid for the lifetime of this
        // metric-id object.

    const baem_MetricDescription *&description();
        // Return a reference to the modifiable address of the *held*
        // non-modifiable 'baem_MetricDescription', which defines the
        // value of this metric id.

     // ACCESSORS
    const baem_MetricDescription * const & description() const;
        // Return a reference to the non-modifiable address of the *held*
        // non-modifiable 'baem_MetricDescription', which defines the
        // value of this metric id.

    bool isValid() const;
        // Return 'true' if this metric id object contains the address of a
        // valid 'baem_MetricDescription' object, and false 'otherwise'.  Note
        // that this method is logically equivalent to '0 != description()'.

    const baem_Category *category() const;
        // Return the address of the non-modifiable 'baem_Category' object
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

// FREE OPERATORS
inline
bool operator==(const baem_MetricId& lhs, const baem_MetricId& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric ids have the same
    // value and 'false' otherwise.  Two metric ids have the same value if
    // they each hold the address of the same 'baem_MetricDescription' object,
    // or both addresses are 0.

inline
bool operator!=(const baem_MetricId& lhs, const baem_MetricId& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' metric ids do not have
    // the same value and 'false' otherwise.  Two metric ids do not have same
    // value if they refer to different 'baem_MetricDescription' object
    // *addresses*, or exactly one address is 0.

inline
bool operator<(const baem_MetricId& lhs, const baem_MetricId& rhs);
    // Return 'true' if the specified 'lhs' metric id is less than (ordered
    // before) the specified 'rhs' metric id, and return 'false' otherwise'.
    // The 'lhs' is considered less if the (unsigned) *address* of the
    // contained 'baem_MetricDescription' object is numerically less than the
    // 'rhs' description.  Note that this function does *not* provide any form
    // of alphabetic ordering, and is logically equivalent to the expression
    // 'lhs.description() < rhs.description()'.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricId& rhs);
    // Write a description of the specified 'rhs' metric id to the specified
    // 'stream' in some human-readable format, and return a reference to the
    // modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // -------------------
                          // class baem_MetricId
                          // -------------------

// CREATORS
inline
baem_MetricId::baem_MetricId()
: d_description_p(0)
{
}

inline
baem_MetricId::baem_MetricId(const baem_MetricDescription *metricDescription)
: d_description_p(metricDescription)
{
}

inline
baem_MetricId::baem_MetricId(const baem_MetricId& original)
: d_description_p(original.d_description_p)
{
}

// MANIPULATORS
inline
baem_MetricId& baem_MetricId::operator=(const baem_MetricId& rhs)
{
    d_description_p = rhs.d_description_p;
    return *this;
}

inline
const baem_MetricDescription *&baem_MetricId::description()
{
    return d_description_p;
}

// ACCESSORS
inline
const baem_MetricDescription * const & baem_MetricId::description() const
{
    return d_description_p;
}

inline
bool baem_MetricId::isValid() const
{
    return 0 != d_description_p;
}

inline
const baem_Category *baem_MetricId::category() const
{
    return d_description_p->category();
}

inline
const char *baem_MetricId::categoryName() const
{
    return d_description_p->category()->name();
}

inline
const char *baem_MetricId::metricName() const
{
    return d_description_p->name();
}

// FREE OPERATORS
inline
bool operator==(const baem_MetricId& lhs, const baem_MetricId& rhs)
{
    return lhs.description() == rhs.description();
}

inline
bool operator!=(const baem_MetricId& lhs, const baem_MetricId& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator<(const baem_MetricId& lhs, const baem_MetricId& rhs)
{
    return lhs.description() < rhs.description();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const baem_MetricId& rhs)
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
