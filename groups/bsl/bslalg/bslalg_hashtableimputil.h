// bslalg_hashtableimputil.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#define INCLUDED_BSLALG_HASHTABLEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide algorithms for implementing a hash table.
//
//@CLASSES:
//  bslalg::HashTableImpUtil: functions used to implement a hash table
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableanchor,
//           bslstl_hashtable
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a namespace for utility functions used
// to implement a hash table container.  Almost all the functions provided by
// this component operate on a 'HashTableAnchor', a type encapsulating the key
// data members of a hash table.
//
///Hash Table Structure
///--------------------
// The utilities provided by this component are used to create and manipulate
// a hash table that resolves collisions using a linked-list of elements
// (i.e., chaining).  Many of the operations provided by 'HashTableImpUtil'
// operate on a 'HashTableAnchor', which encapsulates the key data members of a
// hash table.  A 'HashTableAnchor' has the address of a single, doubly linked
// list holding all the elements in the hash table, as well as the address of
// an array of buckets.  Each bucket holds a reference to the first and last
// element in the linked-list whose *adjusted* *hash* *value* is equal to the
// index of the bucket.  Further, the functions in this component ensure (and
// require) that all elements that fall within a bucket form a contiguous
// sequence in the linked list, as can be seen in the
// diagram below:
//..
//  FIG 1: a hash table holding 5 elements
//
//  Hash Function:  h(n) -> n  [identity function]
//  F: First Element
//  L: Last Element
//
//                     0       1       2       3       4
//                 +-------+-------+-------+-------+-------+--
//  bucket array   |  F L  |  F L  |  F L  |  F L  |  F L  |  ...
//                 +--+-+--+-------+-------+--+-+--+-------+--
//                    | \___         _________/ /
//                     \    \       /          |
//                     V     V     V           V
//                    ,-.   ,-.   ,-.   ,-.   ,-.
//  doubly        |---|0|---|0|---|3|---|3|---|3|--|
//  linked-list       `-'   `-'   `-'   `-'   `-'
//..
//
///Hash Function and the Adjusted Hash Value
///-----------------------------------------
// The C++11 standard defines a hash function as a function 'h(k)' returning
// (integral) values of type 'size_t', such that, for two different values of
// 'k1' and 'k2', the probability that 'h(k1) == h(k2)' is true should approach
// '1.0 / numeric_limits<size_t>::max()' (see 17.6.3.4 [hash.requirements]).
// Such a function 'h(k)' may return values within the entire range of values
// that can be described using 'size_t', [0 ..  numeric_limits<size_t>::max()],
// however the array of buckets maintained by a hash table is typically
// significantly smaller than 'number_limits<size_t>::max()', therefore a
// hash-table implementation must adjust the returned hash function so that it
// falls in the valid range of bucket indices (typically either using an
// integer division or modulo operation) -- we refer to this as the *adjusted*
// *hash* *value*.  Note that currently 'HashTableImpUtil' adjusts the value
// returned by a supplied hash function using 'operator%' (modulo), which
// is more resilient to pathological behaviors when used in conjunction with a
// hash function that may produce contiguous hash values (with the 'div' method
// lower order bits do not participate to the final adjusted value); however,
// the means of adjustment may change in the future.
//
///
///Well-Formed 'HashTableAnchor' Objects
///--------------------------------------
// Many of the algorithms defined in this component operate on
// 'HashTableAnchor' objects, which describe the attributes of a hash table.
// The 'HashTableAnchor' objects supplied to 'HashTableImpUtil' are required
// to meet a series of constraints that are not enforced by the
// 'HashTableAnchor' type itself.  A 'HashTableAnchor' object meeting these
// requirements is said to be "well-formed" and the method
// 'HashTableImpUtil::isWellFormed' returns 'true' for such an object.  A
// 'HastTableAnchor' is considered well-formed for a particular key policy,
// 'KEY_CONFIG', and hash functor, 'HASHER', if all of the following are true:
//
//: 1 The list refers to a well-formed doubly linked list (see
//:   'bslalg_bidirectionallinklistutil').
//:
//: 2 Each link in the list is an object of type
//:   'BidirectionalNode<KEY_CONFIG::ValueType>'
//:
//: 3 Links in the doubly linked list having the same adjusted hash value are
//:   contiguous, where the adjusted hash value is the value returned by
//:   'HashTableImpUtil::computeBucketIndex', for
//:   'HashTableImpUtil::extractKey<KEY_CONFIG>(link)' and the size of the
//:   bucket array.
//:
//: 4 The first and last links in each bucket in the bucket array refer to a
//:   the first and last element in the doubly linked list having an adjusted
//:   hash value equal to that buckets index.  If no values in the doubly
//:   linked list have an adjust hash value equal to a bucket's index, then
//:   the addresses of the first and last links for that bucket are 0.
//
///'KEY_CONFIG' Template Parameter
///-------------------------------
// Several of the operations provided by 'HashTableImpUtil' are template
// functions parametrized on the typename 'KEY_CONFIG'.
//
///'KEY_CONFIG'
/// - - - - - -
// The 'KEY_CONFIG' template parameter must provide the the following type
// aliases and functions:
//..
//  typedef <VALUE_TYPE> ValueType;
//     // Alias for the type of the values stored by the 'BidirectionalNode'
//     // elements in the hash table.
//
//  typedef <KEY_TYPE>   KeyType;
//     // Alias for the type of the key value extracted from the 'ValueType'
//     // stored in the 'BidirectionalNode' elements of a hash table.
//
//  static const KeyType& extractKey(const ValueType& obj);
//      // Return the 'KeyType' information associated with the specified
//      // 'object'.
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTUTIL
#include <bslalg_bidirectionallinklistutil.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#include <bslalg_hashtableanchor.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
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

                          // ======================
                          // class HashTableImpUtil
                          // ======================

struct HashTableImpUtil {
    // This 'struct' provides a namespace for a suite of utility functions
    // for creating and manipulating a hash table.

  private:
    // PRIVATE CLASS METHODS
    static HashTableBucket *findBucketForHashCode(
                                              const HashTableAnchor& anchor,
                                              native_std::size_t     hashCode);
        // Return the address of the 'HashTableBucket' in the array of buckets
        // referred to by the specified hash-table 'anchor' whose index is the
        // adjusted value of the specified 'hashCode' (see
        // 'computeBucketIndex').  The behavior is undefined if 'anchor'
        // has 0 buckets.

    static void spliceListIntoBucket(HashTableAnchor    *anchor,
                                     native_std::size_t  bucketIndex,
                                     BidirectionalLink  *first,
                                     BidirectionalLink  *last);
         // Move into the bucket at the specified 'bucketIndex', in the
         // specified 'anchor', the list of elements in the closed range
         // '[first, last]'.  The behavior is undefined unless 'bucketIndex' is
         // the adjusted hash value of all the elements held by the links in
         // the closed range '[first, last]', and unless the linked list
         // identified by the closed range '[first, last]' is well formed (see
         // 'bslalg::BidirectionalLink::isWellFormed').

    static bool bucketContainsLink(const HashTableBucket&  bucket,
                                   BidirectionalLink      *linkAddress);
        // Return 'true' if the specified 'linkAddress' is the address of one
        // of the links in the list of elements in the closed range
        // '[bucket.first(), bucket.last()]'.

  public:
    // CLASS METHODS
    template<class KEY_CONFIG>
    static const typename KEY_CONFIG::KeyType& extractKey(
                                                const BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // key (of type 'KEY_CONFIG::KeyType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_CONFIG::ValueType>'.  'KEY_CONFIG'
        // shall be a namespace providing the type names 'KeyType' and
        // 'ValueType', as well as a function that can be called as if it had
        // the following signature:
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..

    template <class KEY_CONFIG>
    static typename KEY_CONFIG::ValueType& extractValue(
                                                      BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // value (of type 'KEY_CONFIG::ValueType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_CONFIG::ValueType>'.  'KEY_CONFIG'
        // shall be a namespace providing the type name 'ValueType'.

    template <class KEY_CONFIG, class HASHER>
    static bool isWellFormed(const HashTableAnchor& anchor);
        // Return 'true' if the specified 'anchor' is well-formed.  For a
        // 'HashTableAnchor' to be considered well-formed for a particular key
        // policy, 'KEY_CONFIG', and hash functor, 'HASHER', all of the
        // following must be true:
        //
        //: 1 The 'anchor.listRootAddress()' is the address of a
        //:   well-formed doubly linked list (see
        //:   'bslalg_bidirectionallinklistutil').
        //:
        //: 2 Each link in the list is an object of type
        //:   'BidirectionalNode<KEY_CONFIG::ValueType>'
        //:
        //: 3 Links in the doubly linked list having the same adjusted hash
        //:   value are contiguous, where the adjusted hash value is the value
        //:   returned by 'computeBucketIndex', for
        //:   'extractKey<KEY_CONFIG>(link)' and 'anchor.bucketArraySize()'.
        //:
        //: 4 The first and last links in each bucket (in the bucket array,
        //:   anchor.bucketArrayAddress()') refer to a the first and last
        //:   element in the doubly linked list having an adjusted hash value
        //:   equal to that bucket's array index.  If no values in the doubly
        //:   linked list have an adjusted hash value equal to a bucket's
        //:   index, then the addresses of the first and last links for that
        //:   bucket are 0.

    static native_std::size_t computeBucketIndex(
                                                native_std::size_t hashCode,
                                                native_std::size_t numBuckets);
        // Return the index of the bucket referring to the elements whose
        // adjusted hash codes are the same as the adjusted value of the
        // specified 'hashCode', where 'hashCode' (and the
        // hash-codes of the elements) are adjusted for the specified
        // 'numBuckets'.  The behavior is undefined if 'numBuckets' is 0.

    static void insertAtFrontOfBucket(HashTableAnchor    *anchor,
                                      BidirectionalLink  *link,
                                      native_std::size_t  hashCode);
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode',  into the the specified 'anchor', at the front of the
        // bucket with index
        // 'computeBucketIndex(hashCode, anchor->bucketArraySize()'.  The
        // behavior is undefined unless 'anchor' is well-formed (see
        // 'isWellFormed') for some combination of 'KEY_CONFIG' and
        // 'HASHER' such that 'link' refers to a node of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    static void insertAtPosition(HashTableAnchor    *anchor,
                                 BidirectionalLink  *link,
                                 native_std::size_t  hashCode,
                                 BidirectionalLink  *position);
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode', into the specified 'anchor' immediately before the
        // specified 'position' in the bi-directional linked list of 'anchor'.
        // The behavior is undefined unless position is in the bucket having
        // index 'computeBucketIndex(hashCode, anchor->bucketArraySize())' and
        // anchor' is well-formed (see 'isWellFormed') for some combination of
        // 'KEY_CONFIG' and 'HASHER' such that 'link' refers to a node of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    static void remove(HashTableAnchor    *anchor,
                       BidirectionalLink  *link,
                       native_std::size_t  hashCode);
        // Remove the specified 'link', having the specified (non-adjusted)
        // 'hashCode', from the specified 'anchor'.  The behavior is undefined
        // unless 'anchor' is well-formed (see 'isWellFormed') for some
        // combination of 'KEY_CONFIG' and 'HASHER' such that 'link' refers to
        // a node of type 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    template <class KEY_CONFIG, class KEY_EQUAL>
    static BidirectionalLink *find(
                           const HashTableAnchor&              anchor,
                           const typename KEY_CONFIG::KeyType& key,
                           const KEY_EQUAL&                    equalityFunctor,
                           native_std::size_t                  hashCode);
        // Return the address of the first link in the list element of
        // the specified 'anchor', having a value matching (according to the
        // specified 'equalityFunctor') the specified 'key' in the bucket that
        // holds elements with the specified 'hashCode' if such a link exists,
        // and return 0 otherwise.  The behavior is undefined unless, for the
        // provided 'KEY_CONFIG' and some hash function, 'HASHER', 'anchor' is
        // well-formed (see 'isWellFormed') and 'HASHER(key)' returns
        // 'hashCode'.  'KEY_CONFIG' shall be a
        // namespace providing the type names 'KeyType' and 'ValueType', as
        // well as a function that can be called as if it had the following
        // signature:
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..
        // 'KEY_EQUAL' shall be a functor that can be called as if it had the
        // following signature:
        //..
        //  bool operator()(const KEY_CONFIG::KeyType& key1,
        //                  const KEY_CONFIG::KeyType& key2)
        //..

    template <class KEY_CONFIG, class HASHER>
    static void rehash(HashTableAnchor   *newAnchor,
                       BidirectionalLink *elementList,
                       const HASHER&      hasher);
        // Populate the specified 'newHashTable' with all the elements
        // in the specified 'elementList', using the specified 'hasher' to
        // determine the (non-adjusted) hash code for each element.  This
        // operation provides the strong exception guarantee unless the
        // supplied 'hasher' throws, in which case it provides no exception
        // safety guarantee.  The behavior is undefined unless, 'newHashTable'
        // holds no elements and has one or more (empty) buckets, and
        // 'elementList' is a well-formed bi-directional list (see
        // 'BidirectionalLinkListUtil::isWellFormed') whose nodes are each of
        // type 'BidirectionalNode<KEY_CONFIG::ValueType>', the previous
        // address of the first node and the next address of the last node are
        // 0.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //-----------------------
                        // class HashTableImpUtil
                        //-----------------------

// PRIVATE CLASS METHODS
inline
HashTableBucket *HashTableImpUtil::findBucketForHashCode(
                                               const HashTableAnchor& anchor,
                                               native_std::size_t     hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    native_std::size_t bucketId = HashTableImpUtil::computeBucketIndex(
                                                     hashCode,
                                                     anchor.bucketArraySize());
    return &(anchor.bucketArrayAddress()[bucketId]);
}

inline
native_std::size_t HashTableImpUtil::computeBucketIndex(
                                                 native_std::size_t hashCode,
                                                 native_std::size_t numBuckets)
{
    BSLS_ASSERT_SAFE(0 != numBuckets);

    return hashCode % numBuckets;
}

inline
bool HashTableImpUtil::bucketContainsLink(const HashTableBucket&   bucket,
                                          BidirectionalLink       *linkAddress)
    // Return true the specified 'link' is contained in the specified 'bucket'
    // and false otherwise.
{
    BSLS_ASSERT_SAFE(!bucket.first() == !bucket.last());

    if(!bucket.first()) {
        return false;                                                 // RETURN
    }

    const BidirectionalLink *cursor = bucket.first();
    while (bucket.last()->nextLink() != cursor) {
        if (cursor == linkAddress) {
            return true;                                              // RETURN
        }
        cursor = cursor->nextLink();
    }
    return false;
}

// CLASS METHODS
template<class KEY_CONFIG>
inline
typename KEY_CONFIG::ValueType& HashTableImpUtil::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_CONFIG::ValueType> BNode;
    return static_cast<BNode *>(link)->value();
}

template<class KEY_CONFIG>
inline
const typename KEY_CONFIG::KeyType& HashTableImpUtil::extractKey(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_CONFIG::ValueType> BNode;

    const BNode *node = static_cast<const BNode *>(link);
    return KEY_CONFIG::extractKey(node->value());
}

template <class KEY_CONFIG, class KEY_EQUAL>
inline
BidirectionalLink *HashTableImpUtil::find(
                           const HashTableAnchor&              anchor,
                           const typename KEY_CONFIG::KeyType& key,
                           const KEY_EQUAL&                    equalityFunctor,
                           native_std::size_t                  hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    const native_std::size_t bucketId = computeBucketIndex(
                                                     hashCode,
                                                     anchor.bucketArraySize());
    const HashTableBucket& bucket = anchor.bucketArrayAddress()[bucketId];

    // Odd loop structure as we must test on both first/last before terminating
    // the loop as not-found.

    if (BidirectionalLink *cursor = bucket.first()) {
        for ( ; ; cursor = cursor->nextLink() ) {
            if (equalityFunctor(key, extractKey<KEY_CONFIG>(cursor))) {
                return cursor;                                        // RETURN
            }
            if (cursor == bucket.last()) {
                break;
            }
        }
    }

    return 0;
}

template <class KEY_CONFIG, class HASHER>
inline
void HashTableImpUtil::rehash(HashTableAnchor   *newAnchor,
                              BidirectionalLink *elementList,
                              const HASHER&      hasher)
{
    BSLS_ASSERT_SAFE(newAnchor);
    BSLS_ASSERT_SAFE(0 != newAnchor->bucketArraySize());
    BSLS_ASSERT_SAFE(elementList);          // empty lists do not need a rehash
    BSLS_ASSERT_SAFE(!elementList->previousLink());  // otherwise, not a 'root'

    const native_std::size_t bucketArraySize = newAnchor->bucketArraySize();

    do {
        BidirectionalLink *first = elementList;
        native_std::size_t  bucketIndex= computeBucketIndex(
                                        hasher(extractKey<KEY_CONFIG>(first)),
                                        bucketArraySize);

        BidirectionalLink *last  = first;

        // Walk list of nodes that will rehash to the same bucket
        // This will advance the list extraction point *before* we splice

        while ((elementList = elementList->nextLink()) &&
                bucketIndex == computeBucketIndex(
                                   hasher(extractKey<KEY_CONFIG>(elementList)),
                                   bucketArraySize)) {
            last = elementList;
        }

        spliceListIntoBucket(newAnchor, bucketIndex, first, last);
    }
    while (elementList);
}

template <class KEY_CONFIG, class HASHER>
inline
bool HashTableImpUtil::isWellFormed(const HashTableAnchor& anchor)
{
    HashTableBucket    *array = anchor.bucketArrayAddress();
    native_std::size_t  size  = anchor.bucketArraySize();
    BidirectionalLink  *root  = anchor.listRootAddress();

    // Check that all nodes are in the correct bucket.

    if (size) {
        BidirectionalLink *cursor = anchor.listRootAddress();
        while (cursor) {
            size_t hash  = HASHER()(extractKey<KEY_CONFIG>(cursor));
            size_t index = computeBucketIndex(hash, size);
            if (!bucketContainsLink(array[index], cursor)) {
                return false;                                         // RETURN
            }
            cursor = cursor->nextLink();
        }
    }


    // Check that all the buckets have consistent 'first' and 'last', and that
    // 'first' and 'last', if not 0, are part of the list of elements.

    for (size_t i = 0; i < size; ++i) {

        HashTableBucket& bucket = array[i];

        if (!bucket.first() != !bucket.last()) {  // 'first' XOR 'second
            return false;                                             // RETURN
        }

        if (bucket.first()) {
            if (   !bslalg::BidirectionalLinkListUtil::isWellFormed(
                                                                root,
                                                                bucket.first())
                || !bslalg::BidirectionalLinkListUtil::isWellFormed(
                                                                root,
                                                                bucket.last()))
            {
                return false;                                         // RETURN
            }
            BidirectionalLink *cursor = bucket.first();
            while (cursor != bucket.last()->nextLink()) {
                size_t hash  = HASHER()(extractKey<KEY_CONFIG>(cursor));
                size_t index = computeBucketIndex(hash, size);
                if (!bucketContainsLink(array[index], cursor)) {
                    return false;                                     // RETURN
                }
                cursor = cursor->nextLink();
            }
        }
    }
    return true;
}

}  // close namespace BloombergLP::bslalg
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
