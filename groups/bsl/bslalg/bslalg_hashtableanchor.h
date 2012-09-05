// bslalg_hashtableanchor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#define INCLUDED_BSLALG_HASHTABLEANCHOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an in-core, simply-coinstrained attribute class.
//
//@CLASSES:
//  bslalg::HashTableAnchor : fundamental attributes of a hash table
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@SEE_ALSO: bslstl_hashtable, bslalg_hashtableutil
//
//@listRootAddress: This component provides a single, simply constrained
// *in*-*core* (value-semantic) attribute class, 'bslalg::HashTableAnchor',
// that is used to hold on to the fundumental attributes of a hash table
// structure.  This class is consistent with the hash table data structure
// used in the implementation of BDE 'unordered' container (see
// 'bslstl_hashtable').
//
///Attributes
///----------
//..
//  Name                Type                Constraints
//  ------------------  -----------------   --------------------------
//  bucketArrayAddress  HashTableBucket *   Each bucket of the array must refer
//                                          to elements reachable from
//                                          'listRootAddress' or must be empty.
//
//  bucketArraySize           size_t              Must be the size of the 
//                                          memory referred to by
//                                          'bucketArrayAddress'.
//
//  listRootAddress     BidirectionalLink *       none
//
//: o 'listRootAddress': Address of the head of the linked list of nodes
//                       holding the elements contained in a hash table.
//:
//: o 'bucketArrayAddress': Address of the first element of the sequence of
//:                         'HashTableBucket' objects, partitioning in buckets
//:                         the elements of the hash table referred by 
//:                         'listRootAddress'.
//:
//
//: o 'bucketArraySize': the size of the array starting at 'bucketArrayAddress'. 
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between UTC and Local Times
///- - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#endif

namespace BloombergLP {

namespace bslalg {
                        // ================================
                        // class bslalg::HashTableAnchor
                        // ================================

class HashTableAnchor {
    // This simply constrained (value-semantic) attribute class characterizes a
    // subset of local time values.  See the Attributes section under
    // @listRootAddress in the component-level documentation for information on
    // the class attributes.  Note that the class invariants are identically
    // the constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.
       
    // DATA
    HashTableBucket     *d_bucketArrayAddress_p; // address of the array of
                                                 // buckets of the hash table 
                                                 // (held not owned)

    native_std::size_t   d_bucketArraySize;      // size of 'd_bucketArray'

    BidirectionalLink   *d_listRootAddress_p;  // head of the list of elements
                                               // of the hash table (held not
                                               // owned)
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(HashTableAnchor, TypeTraitBitwiseCopyable);

    // CREATORS
    HashTableAnchor(HashTableBucket    *bucketArrayAddress,
                    native_std::size_t  bucketArraySize,
                    BidirectionalLink  *listRootAddress);
        // Create a 'bslalg::HashTableAnchor' object having the specified
        // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress'
        // attribute.
    
    HashTableAnchor(const HashTableAnchor&  original);
        // Create a 'bslalg::HashTableAnchor' object having the same value
        // as the specified 'original' object. 

    // ~bslalg::HashTableAnchor(); = default 
        // Destroy this object.

    // MANIPULATORS
    bslalg::HashTableAnchor& operator=(
                                        const bslalg::HashTableAnchor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setBucketArrayAndSize(HashTableBucket    *array, 
                               native_std::size_t  size);
    
    void setListRootAddress(BidirectionalLink *value);
        // Set the 'listRootAddress' attribute of this object to the
        // specified 'value'.
                                  // Aspects

    void swap(HashTableAnchor& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

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
    // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress' attributes
    // are the same.

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

                        // ---------------------
                        // class HashTableAnchor
                        // ---------------------

// CREATORS
inline
HashTableAnchor::HashTableAnchor(HashTableBucket    *bucketArrayAddress,
                                 native_std::size_t  bucketArraySize,
                                 BidirectionalLink  *listRootAddress)
: d_bucketArrayAddress_p(bucketArrayAddress)
, d_bucketArraySize(bucketArraySize)
, d_listRootAddress_p(listRootAddress)
{
    BSLS_ASSERT_SAFE(bucketArrayAddress);
    BSLS_ASSERT_SAFE(0 < bucketArraySize);
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
    d_bucketArraySize   = rhs.d_bucketArraySize;
    d_listRootAddress_p    = rhs.d_listRootAddress_p; 
    return *this;
}

inline
void HashTableAnchor::setBucketArrayAndSize(HashTableBucket    *array, 
                                            native_std::size_t  size)
{
    BSLS_ASSERT_SAFE(array);
    BSLS_ASSERT_SAFE(0 < size);
    
    d_bucketArrayAddress_p = array;
    d_bucketArraySize   = size;
}

inline
void HashTableAnchor::setListRootAddress(BidirectionalLink *value)
{
    d_listRootAddress_p = value;
}

                                  // Aspects

inline
void HashTableAnchor::swap(HashTableAnchor& other)
{
    SwapUtil::swap(&d_bucketArrayAddress_p, &other.d_bucketArrayAddress_p);
    SwapUtil::swap(&d_bucketArraySize,   &other.d_bucketArraySize);
    SwapUtil::swap(&d_listRootAddress_p,    &other.d_listRootAddress_p);
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

} // end namespace bslalg

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
