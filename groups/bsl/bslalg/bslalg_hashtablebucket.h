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
//@SEE_ALSO: bslalg_hashtableutil
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP
{
namespace bslalg
{

class BidirectionalLink;

                          // ========================
                          //     Hash table nodes
                          // ========================

class HashTableBucket {
  private:
    // DATA
    BidirectionalLink *d_first_p;
    BidirectionalLink *d_last_p;

  public:
    // CREATORS
    HashTableBucket();
        // Create a link object having no next or previous node.

    // Trivial Copy operations
    // Trivial Destructor

    // MANIPULATORS
    void setFirst(BidirectionalLink *node);

    void setLast(BidirectionalLink *node);

    void createBucket(BidirectionalLink *first, BidirectionalLink *last);
        // Set 'first' and 'last' to the specified values.  Behavior is
        // undefined unless this bucket is empty.

    void fillNewBucket(BidirectionalLink *node);
        // Set 'first' and 'last' to the specified 'node'.  Behavior is
        // undefined unless this bucket is empty.

    void clearBucket();
        // Set 'first' and 'last' to a null pointer value.

    // ACCESSORS
    BidirectionalLink *first() const;
        // Return the address of the first element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    BidirectionalLink *last() const;
        // Return the address of the last element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    int size() const;
        // Return the number of nodes in this hash bucket.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //--------------------
                        // class BidirectionalLink
                        //--------------------

// CREATORS
inline
HashTableBucket::HashTableBucket()
: d_first_p()
, d_last_p()
{
}

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
void HashTableBucket::fillNewBucket(BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(0 == d_first_p);
    BSLS_ASSERT_SAFE(0 == d_last_p);
    d_first_p = d_last_p = node;
}

inline
void HashTableBucket::createBucket(BidirectionalLink *first, BidirectionalLink *last)
{
    BSLS_ASSERT_SAFE(0 == d_first_p);
    BSLS_ASSERT_SAFE(0 == d_last_p);
    d_first_p = first;
    d_last_p  = last;
}

inline
void HashTableBucket::clearBucket()
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
