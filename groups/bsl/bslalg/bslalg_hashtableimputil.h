// bslalg_hashtableimputil.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#define INCLUDED_BSLALG_HASHTABLEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash table data structure for unordered containers
//
//@CLASSES:
//         bslalg::HashTableImpUtil: functions used to implement a hash table
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableanchor,
//           bslstl_hashtable
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico(spacifico1)
//
//@DESCRIPTION: This component provides a namespace for utility functions used
// to implement a hash table container.  Almost all the functions provided by
// this component operate on a hash-table anchor, the minimal data organization
// representing a hash table (see 'bslalg_hashtableanchor').
///Hash Table
///----------
// The model of hash table that is intended to be implmenented with this
// 'HashTableImpUtil' component is a a chained hash-table with one single chain
// of elements.  The model of hash table in question includes the concept of
// *array* *of* *buckets*.  The array of buckets is an array that provides
// access, at each index of the array, to all the elements contained in the
// table whose *adjusted* *hash* *value* is equal to the index.  Before
// continuing we need to explain some details to confusion on the
// definitons.
//
//..
// FIG. 1 Hash table anchor with an array of 4 buckets, two of which non empty.
//
//             0<--+-+<--+-+<--+-+<--+-+<--+-+
//                 |1|   |7|   |3|   |0|   |9|
//                 +-+-->+-+-->+-+-->+-+-->+-+-->0
//                   ^    ^     ^            ^
//                   |    |      |_________   |
//                    |  /                  |  |
//                    | |                    |  |
//                 +--+-+--+--x-x--+--x-x--+--+-+--+--+-+--+
//                 |  * *  |  * *  |  * *  |  * *  |  * *  |
//                 +-------+-------+-------+-------+-------+
//                     0       1       2       3       4
//..
//
//
///Hash Function And Adjsuted Hash Value
///-------------------------------------
// The C++11 standard defines a hash function as the application of a 'HASHER'
// functor type to a 'KEY' type, returning a value between 0 and
// 'numeric_limits<std::size_t>::max'.  At the same time, in literature, the
// hash function (the functor of type 'HASHER' before) is often defined as that
// function *h(x)* that applied to an element *x* (corresponding to an element
// of type 'KEY' in the standard definition) returns a value between 0 and
// *N-1*, where *N* is the number of buckets of a hash table.  In order to
// distinguish between the two, we adopt the same definition for hash function
// as the C++11 standard, and we define *extended* *hash* *value* the value
// obtained by composing the standard hash function with another function that
// return values between 0 and N-1.
//
///Well Formed Anchor
///------------------
// A 'HashTableAnchor' object holds references to an array of 'HashTableBucket'
// objects with its size,  and to a null-terminated doubly linked list of
// 'BidirectionalLink' nodes, instances of template class 'BidirectionalNode',
// parametrized on the type of value held by the node.
// A 'HashTableAnchor' value is *well* *formed*, with respect to a given
// hashing function 'H', if 1) the doubly linked list is well formed, 2) each
// bucket in the array of buckets points to first and last elements of
// non-overlappling ranges in the list referenced by the anchor, and 3) every
// element in a bucket is such that the its extended hash value (see previous
// section) is equal to the index of the bucket.
//
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propogate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#include <bslalg_hashtableanchor.h>
#endif

 #ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t', prefer bsls::Types::SizeType, but that is signed
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP
{

namespace bslalg
{

                          // ======================
                          // class HashTableImpUtil
                          // ======================

struct HashTableImpUtil {
  private:
    // PRIVATE CLASS METHODS
    static
    HashTableBucket *findBucketForHashCode(const HashTableAnchor& anchor,
                                           native_std::size_t     hashCode);
        // Return the address of the 'HashTableBucket', in the specified
        // hash-table 'anchor', having index such that the specified 'hashCode'
        // maps to that index (see 'computeBucketIndex').

    static
    void spliceSegmentIntoBucket(BidirectionalLink  *cursor,
                                 BidirectionalLink  *nextCursor,
                                 HashTableBucket    *bucket,
                                 BidirectionalLink **newRoot);
        // TBD (for Alisdair)

    static
    bool bucketContainsLink(const HashTableBucket&   bucket,
                            const BidirectionalLink *link);
        // Return true the specified 'link' is contained in the specified
        // 'bucket' and false otherwise.

    static
    bool isReachable(const BidirectionalLink *dst,
                     const BidirectionalLink *src);
        // Return true if the specified 'dst' is found by walking forward in
        // the the null-terminated doubly linked list from the specified 'src.
        // 'src .  The behavior is undefined unless 'src' is not part of
        // a circular linked list and the value of the 'previousLink' attribute
        // of the first node of the list is 0.

  public:
    // CLASS METHODS
    template<class KEY_POLICY>
    static
    const typename KEY_POLICY::KeyType& extractKey(
                                                const BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // parametrized 'typename KEY_POLICY::KeyType' type property of
        // the value held by the node referenced by the specified 'link'.  The
        // behavior is undefined unless 'link' refereces a node of type
        // 'BidirectionalNode<KEY_POLICY::ValueType>.

    template <class KEY_POLICY>
    static
    typename KEY_POLICY::ValueType& extractValue(BidirectionalLink *link);
        // Return a reference providing modifiable access to the parametrized
        // 'typename KEY_POLICY::ValueType' value held by the node referenced
        // by the specified 'link'.  The behavior is undefined unless 'link'
        // refereces a node of type 'BidirectionalNode<KEY_POLICY::ValueType>.

    template <class KEY_POLICY, class HASHER>
    static
    bool isWellFormedAnchor(const HashTableAnchor& anchor);
        // Return true if, for the specified 'anchor', all the following
        // conditions are true:
        //
        //: 1 All the nodes accessible from 'anchor->listRootAddress()' are
        //:   are instances of 'BidirectionalNode<KEY_POLICY::ValueType' for
        //:   specified parametrized type 'KEY_POLICY'.
        //:
        //: 2 For each 'link' accessible from 'anchor->listRootAddress()', the
        //:   bucket index for 'link' recomputed using the hash genearated by
        //:   the specified parametrized 'KEY_POLICY' and 'HASHER' types is the
        //:   same as the actual bucket index for 'link'.
        //
        // Note that the recomputed bucket index for a 'link' in terms of the
        // parametrized types 'KEY_POLICY' and 'HASHER' has the same value as
        // the one returned by:
        // ..
        //    'computeBucketIndex(HASHER()(extractKey<KEY_POLICY>(link),
        //                        anchor->bucketArraySize());
        // ..

    static
    native_std::size_t computeBucketIndex(native_std::size_t hashCode,
                                          native_std::size_t numBuckets);
        // Return the index of the bucket storing the values that hash,
        // according to a given hash function, to the specified 'hashCode' in a
        // hash table having the specified 'numBuckets'.  Bucket indexes are
        // mapped to hash values following and implementation-specific
        // strategy.

    static
    void insertAtFrontOfBucket(HashTableAnchor    *anchor,
                               BidirectionalLink  *link,
                               native_std::size_t  hashCode);
        // Insert, into the array of buckets referenced by the specified
        // 'anchor', the specified 'link' at the front of the bucket
        // associated to the specified 'hashCode'.  The behavior is undefined
        // unless 'link' references a node of type 'BidirectionalNode'
        // parametrized on the same type as the other 'BidirectionalNode'
        // values referenced by all the links accessible from
        // 'anchor->listRootAddress()', and unless 'hashCode' was computed
        // using a 'KEY_POLICY' and 'HASHER' types such that:
        // ..
        //   true == isWellFormedAnchor<KEY_POLICY, HASHER>(anchor);
        // ..

    static
    void insertAtPosition(HashTableAnchor    *anchor,
                          BidirectionalLink  *link,
                          native_std::size_t  hashCode,
                          BidirectionalLink  *position);
        // Insert, in the specified hash-table 'anchor', the specified 'link',
        // into the bucket corresponding to the specified 'hashCode', before
        // the specified before the specified 'poistion'.  The behavior is
        // undefined unless 'position' belongs to
        // the bucket corresponding to 'hashCode' in 'anchor', and 'hashCode'
        // was obtained
        // from 'link' using a 'KEY_POLICY' and 'HASHER' types
        // such that:
        // ..
        //   true == isWellFormedAnchor<KEY_POLICY, HASHER>(anchor);
        // ..

    static
    void remove(HashTableAnchor    *anchor,
                BidirectionalLink  *link,
                native_std::size_t  hashCode);
        // Remove, in the specified hash-table 'anchor', the specified 'link',
         // from the bucket corresponding to the specified 'hashCode'.  The
        // behavior is undefined unless 'link' belongs to the bucket
        // corresponding to 'hashCode' in 'anchor', and 'hashCode' was obtained
        // from 'link' using a 'KEY_POLICY' and 'HASHER' types such
        // that:
        // ..
        //   true == isWellFormedAnchor<KEY_POLICY, HASHER>(anchor);
        // ..


    template <class KEY_POLICY, class KEY_EQUAL>
    static
    BidirectionalLink *find(
                           const HashTableAnchor&              anchor,
                           const typename KEY_POLICY::KeyType& key,
                           const KEY_EQUAL&                    equalityFunctor,
                           native_std::size_t                  hashCode);
        // Return the address, if found, of the first link in the specified
        // hash-table 'anchor' that holds a value matching the specified 'key'
        // according the specified parametrized type 'KEY_POLICY' and the
        // specified 'equalityFunctor', in the bucket storing elements having
        // the specified 'hashCode'.  Return 0, otherwise.  The behavior is
        // undefined unless 'link' belongs to the bucket corresponding to
        // 'hashCode' in 'anchor', and 'hashCode' was obtained from 'link'
        // using a 'KEY_POLICY' and 'HASHER' types such that:
        // ..
        //   true == isWellFormedAnchor<KEY_POLICY, HASHER>(anchor);
        // ..

    template <class KEY_POLICY, class HASHER>
    static
    void rehash(HashTableAnchor   *newAnchor,
                BidirectionalLink *oldRoot,
                const HASHER&      hasher);
        // Redistribute, into the specified 'newAnchor', all the links starting
        // from the specified 'oldRoot', recomputing for each node the new
        // bucket index using the specified 'hasher' functor on the keys
        // extracted by the specified parametrized 'KEY_POLICY' type.  Note
        // that this function is not exception safe in the presence of a
        // throwing 'hash' functor.
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
                                          const BidirectionalLink *link)
    // Return true the specified 'link' is contained in the specified 'bucket'
    // and false otherwise.
{
    BSLS_ASSERT_SAFE(!bucket.first() == !bucket.last());

    if(!bucket.first()) {
        return false;
    }

    const BidirectionalLink *cursor = bucket.first();
    while (bucket.last()->nextLink() != cursor) {
        if (cursor == link) {
            return true;
        }
        cursor = cursor->nextLink();
    }
    return false;
}

// PRIVATE CLASS METHODS
template<class KEY_POLICY>
inline
typename KEY_POLICY::ValueType& HashTableImpUtil::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_POLICY::ValueType> BNode;
    return static_cast<BNode *>(link)->value();
}

template<class KEY_POLICY>
inline
const typename KEY_POLICY::KeyType& HashTableImpUtil::extractKey(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);
    typedef BidirectionalNode<typename KEY_POLICY::ValueType> BNode;
    const BNode *node = static_cast<const BNode *>(link);
    return KEY_POLICY::extractKey(node->value());
}

    // lookup
template <class KEY_POLICY, class KEY_EQUAL>
inline
BidirectionalLink *HashTableImpUtil::find(
                           const HashTableAnchor&              anchor,
                           const typename KEY_POLICY::KeyType& key,
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
            if (equalityFunctor(key, extractKey<KEY_POLICY>(cursor))) {
                return cursor;
            }
            if (cursor == bucket.last()) {
                break;
            }
        }
    }

    return 0;
}

template <class KEY_POLICY, class HASHER>
inline
void HashTableImpUtil::rehash(HashTableAnchor   *newAnchor,
                              BidirectionalLink *oldRoot,
                              const HASHER&      hasher)
{
    BSLS_ASSERT_SAFE(newAnchor);
    BSLS_ASSERT_SAFE(oldRoot);           // empty lists do not need a rehash
    BSLS_ASSERT_SAFE(!oldRoot->previousLink());  // otherwise, not a 'root'

    BidirectionalLink *newRoot = 0;

    do {
        BidirectionalLink *cursor = oldRoot;
        HashTableBucket   *bucket = findBucketForHashCode(
                                       *newAnchor,
                                       hasher(extractKey<KEY_POLICY>(cursor)));

        BidirectionalLink *nextCursor  = cursor;

        // Walk list of nodes that will rehash to the same bucket
        // This will advance the list extraction point *before* we splice

        while ((oldRoot = oldRoot->nextLink()) &&
                bucket == findBucketForHashCode(
                                    *newAnchor,
                                    hasher(extractKey<KEY_POLICY>(oldRoot)))) {
             nextCursor = oldRoot;
        }

        spliceSegmentIntoBucket(cursor, nextCursor, bucket, &newRoot);
    }
    while (oldRoot);

    newAnchor->setListRootAddress(newRoot);
}

template <class KEY_POLICY, class HASHER>
inline
bool HashTableImpUtil::isWellFormedAnchor(const HashTableAnchor& anchor)
{
    const HashTableBucket   *array = anchor.bucketArrayAddress();
    native_std::size_t       size  = anchor.bucketArraySize();
    const BidirectionalLink *root  = anchor.listRootAddress();

    // Check that all nodes are in the correct bucket.

    if (size) {
        const BidirectionalLink *cursor = anchor.listRootAddress();
        while (cursor) {
            size_t hash  = HASHER()(extractKey<KEY_POLICY>(cursor));
            size_t index = computeBucketIndex(hash, size);
            if (!bucketContainsLink(array[index], cursor)) {
                return false;                                          // RETURN
            }
            cursor = cursor->nextLink();
        }
    }


    // Check that all the buckets have consistent 'first' and 'last', and that
    // 'first' and 'last', if not 0, are part of the list of elements.

    for (size_t i = 0; i < size; ++i) {

        const HashTableBucket& bucket = array[i];

        if (!bucket.first() != !bucket.last()) {  // 'first' XOR 'second
            return false;
        }

        if (bucket.first()) {
            if (   !isReachable(bucket.first(), root)
                || !isReachable(bucket.last(),  root)) {
                return false;                                         // RETURN
            }
            const BidirectionalLink *cursor = bucket.first();
            while (cursor != bucket.last()->nextLink()) {
                size_t hash  = HASHER()(extractKey<KEY_POLICY>(cursor));
                size_t index = computeBucketIndex(hash, size);
                if (!bucketContainsLink(array[index], cursor)) {
                    return false;                                      // RETURN
                }
            cursor = cursor->nextLink();
            }
        }
    }
    return true;
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
