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
//  Name                Type                Default  Constraints
//  ------------        -----------------   -------  --------------------------
//  bucketArrayAddress  HashTableBucket *      0     The element of the array
//                                                   must refer to elements
//                                                   reachable from
//                                                   'listRootAddress'.
//
//  arraySize    size_t                        0     Must be the size of the
//                                                   memory referred to by
//                                                   'bucketArrayAddress'.
//
//  listRootAddress     BidirectionalLink *    0           none
//
//: o 'listRootAddress': Address of the head of the linked list of nodes
//                       holding the elements contained in a hash table.
//:
//: o 'bucketArrayAddress': Address of the first element of the sequence of
//:                         'HashTableBucket' objects, partitioning in buckets
//:                         the elements of the hash table referred by 
//:                         'listRootAddress'.
//:
//: o 'arraySize': the size of the array starting at 'bucketArrayAddress'. 
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
    HashTableBucket   *d_bucketArrayAddress;  // address of the array of buckets
                                              // of the hash table 

    std::size_t        d_arraySize;  // size of 'd_bucketArray'

    BidirectionalLink *d_listRootAddress;  // head of the list of elements of
                                           // the hash table

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(HashTableAnchor,
                                  TypeTraitUsesBslmaAllocator,
                                  TypeTraitBitwiseCopyable);

    // CREATORS
    explicit HashTableAnchor();
        // Create a 'bslalg::HashTableAnchor' object having the (default)
        // attribute values:
        //..
        //  bucketArrayAddress() == 0
        //  arraySize()   == 0
        //  listRootAddress()    == 0
        //..

    HashTableAnchor(HashTableBucket   *bucketArrayAddress,
                    std::size_t        arraySize,
                    BidirectionalLink *listRootAddress);
        // Create a 'bslalg::HashTableAnchor' object having the specified
        // 'bucketArrayAddress', 'arraySize', and 'listRootAddress' attribute.  TBD
        // wording for NULL pointers.
    
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

    void setBucketArrayAddress(HashTableBucket *value);
        // Set the 'bucketArrayAddress' attribute of this object to the
        // specified 'value'.

    void setArraySize(std::size_t value);
        // Set the 'arraySize' attribute of this object to the specified
        // 'value'.  The behavior is undefined unless the specified 'value'
        // represent the number of elements contained in the sequence of
        // elements starting at 'bucketArrayAddress'.
    
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

    std::size_t arraySize() const;
        // Return the value of the 'arraySize' attribute of this object.

    BidirectionalLink *listRootAddress() const;
        // Return the value 'listRootAddress' attribute of this object.


                                  // Aspects

    // bsl::ostream& print(bsl::ostream& stream,
    //                  int           level = 0,
    //                  int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const HashTableAnchor& lhs,
                const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor' objects
    // have the same value if all of the corresponding values of their
    // 'bucketArrayAddress', 'arraySize', and 'listRootAddress' attributes
    // are the same.

bool operator!=(const HashTableAnchor& lhs,
                const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor'
    // objects do not have the same value if any of the corresponding values of
    // their 'bucketArrayAddress', 'arraySize', or 'listRootAddress'
    // attributes are not the same.

//bsl::ostream& operator<<(bsl::ostream&                     stream,
//                         const bslalg::HashTableAnchor& object);
//    // Write the value of the specified 'object' to the specified
//    // output 'stream' in a single-line format, and return a reference
//    // providing modifiable access to 'stream'.  If 'stream' is not valid on
//    // entry, this operation has no effect.  Note that this human-readable
//    // format is not fully specified and can change without notice.  Also note
//
//    // that this method has the same behavior as 'object.print(stream, 0, -1)',
//    // but with the attribute names elided.

// FREE FUNCTIONS
void swap(HashTableAnchor& a, HashTableAnchor& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------------
                        // class bslalg::HashTableAnchor
                        // --------------------------------

// CREATORS
inline
HashTableAnchor::HashTableAnchor()
: d_bucketArrayAddress(static_cast<HashTableBucket *>(0))
, d_arraySize(0)
, d_listRootAddress(static_cast<BidirectionalLink *>(0))
{
}

inline
HashTableAnchor::HashTableAnchor(HashTableBucket   *bucketArrayAddress,
                                 std::size_t        arraySize,
                                 BidirectionalLink *listRootAddress)
: d_bucketArrayAddress(bucketArrayAddress)
, d_arraySize(arraySize)
, d_listRootAddress(listRootAddress)
{
    // TBD how do we check undefined behavior?
}

inline
HashTableAnchor::HashTableAnchor(const HashTableAnchor& original)
: d_bucketArrayAddress(original.d_bucketArrayAddress)
, d_arraySize(original.d_arraySize)
, d_listRootAddress(original.d_listRootAddress)
{
}

// MANIPULATORS
inline
HashTableAnchor& HashTableAnchor::operator=(const HashTableAnchor& rhs)
{
    d_bucketArrayAddress = rhs.d_bucketArrayAddress;
    d_arraySize          = rhs.d_arraySize;
    d_listRootAddress   = rhs.d_listRootAddress; 
    return *this;
}

inline
void HashTableAnchor::setListRootAddress(BidirectionalLink *value)
{
    d_listRootAddress = value;
}

inline
void HashTableAnchor::setArraySize(std::size_t value)
{
    d_arraySize = value;
}

inline
void HashTableAnchor::setBucketArrayAddress(HashTableBucket *value)
{
    d_bucketArrayAddress = value;
}

                                  // Aspects

inline
void HashTableAnchor::swap(HashTableAnchor& other)
{
    SwapUtil::swap(&d_arraySize,          &other.d_arraySize);
    SwapUtil::swap(&d_bucketArrayAddress, &other.d_bucketArrayAddress);
    SwapUtil::swap(&d_listRootAddress,    &other.d_listRootAddress);
}

// ACCESSORS
inline
BidirectionalLink *HashTableAnchor::listRootAddress() const
{
    return d_listRootAddress;
}

inline
std::size_t HashTableAnchor::arraySize() const
{
    return d_arraySize;
}

inline
HashTableBucket *HashTableAnchor::bucketArrayAddress() const
{
    return d_bucketArrayAddress;
}

} // end namespace bslalg

// FREE OPERATORS
inline
bool bslalg::operator==(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() == rhs.bucketArrayAddress()
        && lhs.arraySize()          == rhs.arraySize()
        && lhs.listRootAddress()    == rhs.listRootAddress();
}

inline
bool bslalg::operator!=(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() != rhs.bucketArrayAddress()
        || lhs.arraySize()          != rhs.arraySize()
        || lhs.listRootAddress()    != rhs.listRootAddress();
}

// FREE FUNCTIONS
inline
void bslalg::swap(bslalg::HashTableAnchor& a, bslalg::HashTableAnchor& b)
{
    a.swap(b);
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
