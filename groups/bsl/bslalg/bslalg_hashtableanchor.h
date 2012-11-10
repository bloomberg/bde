// bslalg_hashtableanchor.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#define INCLUDED_BSLALG_HASHTABLEANCHOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type holding the constituent parts of a hash table.
//
//@CLASSES:
// bslalg::HashTableAnchor: (in-core) bucket-array and node list
//
//@SEE_ALSO: bslstl_hashtable, bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides a single, complex-constrained
// *in*-*core* (value-semantic) attribute class, 'bslalg::HashTableAnchor',
// that is used to hold (not own) the array of buckets and the list of nodes
// that form the key data elements of a hash-table.  This class is typically
// used with the utilities provided in 'bslstl_hashtableimputil'.  Note that
// the decision to store nodes in a linked list (i.e., resolving collisions
// through chaining) is intended to facilitate a hash-table implementation
// meeting the requirements of a C++11 standard unordered container.
//
///Attributes
///----------
//..
//  Name                Type                  Simple Constraints
//  ------------------  -------------------   ------------------
//  bucketArrayAddress  HashTableBucket *     none
//
//  bucketArraySize     size_t                none
//
//  listRootAddress     BidirectionalLink *   none
//
//
//  Complex Constraint
//  -------------------------------------------------------------------------
//  'bucketArrayAddress' must refer to a contiguous sequence of valid
//  'bslalg::HashTableBucket' objects of at least the specified
//  'bucketArraySize' or both 'bucketArrayAddress' and 'bucketArraySize' must
//  be 0.
//..
//
//: o 'listRootAddress': address of the head of the linked list of nodes
//:   holding the elements contained in a hash table
//:
//: o 'bucketArrayAddress': address of the first element of the sequence of
//:   'HashTableBucket' objects, each of which refers to the first and last
//:   node (from 'listRootAddress') in that bucket
//:
//: o 'bucketArraySize': the number of (contiguous) buckets in the array of
//:   buckets at 'bucketArrayAddress'
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: ...
///- - - - - - -

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslalg {

struct HashTableBucket;  // This is known to be a POD struct.

                        // =============================
                        // class bslalg::HashTableAnchor
                        // =============================

class HashTableAnchor {
    // This complex constrained *in*-*core* (value-semantic) attribute class
    // characterizes the key data elements of a hash table.  See the
    // "Attributes" section under @DESCRIPTION in the component-level
    // documentation for/ information on the class attributes.  Note that the
    // class invariant is the identically the complex constraint of this
    // component.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization and default construction
    //: o is *in-core*
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    HashTableBucket     *d_bucketArrayAddress_p;  // address of the array of
                                                  // buckets (held, not owned)

    native_std::size_t   d_bucketArraySize;       // size of 'd_bucketArray'

    BidirectionalLink   *d_listRootAddress_p;     // head of the list of
                                                  // elements in the hash-table
                                                  // (held, not owned)

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(HashTableAnchor, TypeTraitBitwiseCopyable);

    // CREATORS
    HashTableAnchor(HashTableBucket    *bucketArrayAddress,
                    native_std::size_t  bucketArraySize,
                    BidirectionalLink  *listRootAddress);
        // Create a 'bslalg::HashTableAnchor' object having the specified
        // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress'
        // attributes.  The behavior is undefined unless 'bucketArrayAddress'
        // refers to a contiguous sequence of valid 'bslalg::HashTableBucket'
        // objects of at least 'bucketArraySize' or unless both
        // 'bucketArrayAddress' and 'bucketArraySize' are 0.

    HashTableAnchor(const HashTableAnchor& original);
        // Create a 'bslalg::HashTableAnchor' object having the same value
        // as the specified 'original' object.

    // ~bslalg::HashTableAnchor(); = default
        // Destroy this object.

    // MANIPULATORS
    bslalg::HashTableAnchor& operator=(const bslalg::HashTableAnchor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setBucketArrayAddressAndSize(HashTableBucket    *bucketArrayAddress,
                                      native_std::size_t  bucketArraySize);
        // Set the bucket array address and bucket array size attributes of
        // this object to the specified 'bucketArrayAddress' and
        // 'bucketArraySize' values.  The behavior is undefined unless
        // 'bucketArrayAddress' refers to a contiguous sequence of valid
        // 'bslalg::HashTableBucket' objects of at least 'bucketArraySize', or
        // unless both 'bucketArrayAddress' and 'bucketArraySize' are 0.

    void setListRootAddress(BidirectionalLink *value);
        // Set the 'listRootAddress' attribute of this object to the
        // specified 'value'.
                                  // Aspects

    void swap(HashTableAnchor& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    HashTableBucket *bucketArrayAddress() const;
        // Return the value of the 'bucketArrayAddress' attribute of this
        // object.

    native_std::size_t bucketArraySize() const;
        // Return the value of the 'bucketArraySize' attribute of this object.

    BidirectionalLink *listRootAddress() const;
        // Return the value 'listRootAddress' attribute of this object.
};

// FREE OPERATORS
bool operator==(const HashTableAnchor& lhs, const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor' objects
    // have the same value if all of the corresponding values of their
    // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress'
    // attributes are the same.

bool operator!=(const HashTableAnchor& lhs, const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor'
    // objects do not have the same value if any of the corresponding values of
    // their 'bucketArrayAddress', 'bucketArraySize', or 'listRootAddress'
    // attributes are not the same.

// FREE FUNCTIONS
void swap(HashTableAnchor& a, HashTableAnchor& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // class bslalg::HashTableAnchor
                        // -----------------------------

// CREATORS
inline
HashTableAnchor::HashTableAnchor(bslalg::HashTableBucket   *bucketArrayAddress,
                                 native_std::size_t         bucketArraySize,
                                 bslalg::BidirectionalLink *listRootAddress)
: d_bucketArrayAddress_p(bucketArrayAddress)
, d_bucketArraySize(bucketArraySize)
, d_listRootAddress_p(listRootAddress)
{
    BSLS_ASSERT_SAFE(   (!bucketArrayAddress && !bucketArraySize)
                     || (bucketArrayAddress && 0 < bucketArraySize));
    BSLS_ASSERT_SAFE(!listRootAddress || !(listRootAddress->previousLink()));
}

inline
HashTableAnchor::HashTableAnchor(const HashTableAnchor& original)
: d_bucketArrayAddress_p(original.d_bucketArrayAddress_p)
, d_bucketArraySize(original.d_bucketArraySize)
, d_listRootAddress_p(original.d_listRootAddress_p)
{
}

// MANIPULATORS
inline
HashTableAnchor& HashTableAnchor::operator=(const HashTableAnchor& rhs)
{
    d_bucketArrayAddress_p = rhs.d_bucketArrayAddress_p;
    d_bucketArraySize      = rhs.d_bucketArraySize;
    d_listRootAddress_p    = rhs.d_listRootAddress_p;
    return *this;
}

inline
void HashTableAnchor::setBucketArrayAddressAndSize(
                                        HashTableBucket    *bucketArrayAddress,
                                        native_std::size_t  bucketArraySize)
{
    BSLS_ASSERT_SAFE(( bucketArrayAddress && 0 < bucketArraySize)
                  || (!bucketArrayAddress &&    !bucketArraySize));

    d_bucketArrayAddress_p = bucketArrayAddress;
    d_bucketArraySize      = bucketArraySize;
}

inline
void HashTableAnchor::setListRootAddress(BidirectionalLink *value)
{
    BSLS_ASSERT_SAFE(!value || !value->previousLink());

    d_listRootAddress_p = value;
}

                                  // Aspects

inline
void HashTableAnchor::swap(HashTableAnchor& other)
{
    bslalg::ScalarPrimitives::swap(*this, other);
}

// ACCESSORS
inline
BidirectionalLink *HashTableAnchor::listRootAddress() const
{
    return d_listRootAddress_p;
}

inline
std::size_t HashTableAnchor::bucketArraySize() const
{
    return d_bucketArraySize;
}

inline
HashTableBucket *HashTableAnchor::bucketArrayAddress() const
{
    return d_bucketArrayAddress_p;
}

}  // close namespace bslalg

// FREE OPERATORS
inline
void bslalg::swap(bslalg::HashTableAnchor& a, bslalg::HashTableAnchor& b)
{
    a.swap(b);
}

inline
bool bslalg::operator==(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() == rhs.bucketArrayAddress()
        && lhs.bucketArraySize()    == rhs.bucketArraySize()
        && lhs.listRootAddress()    == rhs.listRootAddress();
}

inline
bool bslalg::operator!=(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() != rhs.bucketArrayAddress()
        || lhs.bucketArraySize()    != rhs.bucketArraySize()
        || lhs.listRootAddress()    != rhs.listRootAddress();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
