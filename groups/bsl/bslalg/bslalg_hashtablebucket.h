// bslalg_hashtablebucket.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#define INCLUDED_BSLALG_HASHTABLEBUCKET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bucket represantation for hash table data structures..
//
//@CLASSES:
//   bslalg::HashTableBucket : hash-table that manages externally allocated nodes
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides ...
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#endif

namespace BloombergLP
{
namespace bslalg
{

class BidirectionalLink;

                          // =====================
                          // class HashTableBucket
                          // =====================

struct HashTableBucket {
  public:
    // DATA
    BidirectionalLink *d_first_p;
    BidirectionalLink *d_last_p;
    
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(HashTableBucket, bslalg::TypeTraitsGroupPod);

  public:
    // CREATORS
    //HashTableBucket();
        // Create a 'HashTableBucket' object having the 'first' and 'last'
        // attributes set to 0.

   // HashTableBucket(BidirectionalLink *first, BidirectionalLink *last);
        // Create a 'HashTableBucket' object having the specified 'first' and
        // 'last' attributes.

    // Trivial Copy operations
    // Trivial Destructor

    // MANIPULATORS
    void setFirst(BidirectionalLink *node);
       // Set the 'first' element of this bucket to the specified 'node'.  The
       // behavior is undefined unless 'node' is an element from the same
       // birectional list as the 'last' element in this bucket, and 'node'
       // either precedes 'last' in that list, or is the same node, or this
       // bucket is empty and 'node' has a null pointer value.

    void setLast(BidirectionalLink *node);
       // Set the 'last' element of this bucket to the specified 'node'.  The
       // behavior is undefined unless 'node' is an element from the same
       // birectional list as the 'first' element in this bucket, and 'node'
       // either follows 'first' in that list, or is the same node, or this
       // bucket is empty and 'node' has a null pointer value.


    void setFirstAndLast(BidirectionalLink *first, BidirectionalLink *last);
        // Set 'first' and 'last' to the specified values.  Behavior is
        // undefined unless unless 'first == last', or unless 'first' and
        // 'last' are links from the same list, where 'first' precedes 'last'
        // in the list.  Note that 'first' and 'last' may both have a null
        // pointer value, indicating an empty bucket.

    void reset();
        // Set 'first' and 'last' to a null pointer value.

    // ACCESSORS
    BidirectionalLink *first() const;
        // Return the address of the first element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    BidirectionalLink *last() const;
        // Return the address of the last element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    native_std::size_t countElements() const;
        // Return the number of nodes in this hash bucket.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //----------------------
                        // class HashTableBucket
                        //----------------------

// MANIPULATORS
inline
void HashTableBucket::setFirst(BidirectionalLink *node)
{
    d_first_p = node;
}

inline
void HashTableBucket::setLast(BidirectionalLink *node)
{
    d_last_p = node;
}

inline
void HashTableBucket::setFirstAndLast(BidirectionalLink *first,
                                      BidirectionalLink *last)
{
    d_first_p = first;
    d_last_p  = last;
}

inline
void HashTableBucket::reset()
{
    d_first_p = d_last_p = 0;
}

// ACCESSORS
inline
BidirectionalLink *HashTableBucket::first() const
{
    return d_first_p;
}

inline
BidirectionalLink *HashTableBucket::last() const
{
    return d_last_p;
}

} // namespace BloombergLP::bslalg

} // namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
