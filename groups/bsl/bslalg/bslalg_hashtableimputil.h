// bslalg_hashtableimputil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#define INCLUDED_BSLALG_HASHTABLEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash table data structure for unordered containers
//
//@CLASSES:
//         bslimp::HashTable: hash-table that manages externally allocated nodes
// bslimp::TableNodeIterator: iterators that walk the hash-table data structure
//
//@SEE_ALSO: bsl_unorderedmap5
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a common foundation to implement the
// four 'unorderd' containers specified by the C++11 standard.  It supplies
// a hash table, which stores values in buckets determined by the supplied
// hashing policy, and iterators to walk the table returning access to said
// values.
//
// This implementation will use a single, bidirectional list, indexed by
// a dynamic array of buckets, each of which contains pointers to the first
// and last nodes of that bucket in the linked list.
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
    static
    HashTableBucket *findBucketForHashCode(const HashTableAnchor& anchor,
                                           native_std::size_t     hashCode);

    static
    void spliceSegmentIntoBucket(BidirectionalLink  *cursor,
                                 BidirectionalLink  *nextCursor,
                                 HashTableBucket    *bucket,
                                 BidirectionalLink **newRoot);
        // Consider moving this 'private' method into an implemention-private
        // utility class with an '_' in the name, in order to be accissible for
        // testing in the test driver.

  public:
    static
    native_std::size_t computeBucketIndex(native_std::size_t hashCode,
                                          native_std::size_t numBuckets);

    static
    void insertAtFrontOfBucket(HashTableAnchor    *anchor,
                               BidirectionalLink  *newNode,
                               native_std::size_t  hashCode);

    static
    void insertDuplicateAtPosition(HashTableAnchor    *anchor,
                                   BidirectionalLink  *newNode,
                                   native_std::size_t  hashCode,
                                   BidirectionalLink  *location);

    static
    void removeNode(HashTableAnchor    *anchor,
                    BidirectionalLink  *position,
                    native_std::size_t  hashCode);

    template <class VALUE_POLICY, class KEY_EQUAL, class KEY_TYPE>
    static
    BidirectionalLink *find(const HashTableAnchor& anchor,
                            const KEY_EQUAL&       keyComparator,
                            const KEY_TYPE&        key,
                            native_std::size_t     hashCode);

    template <class VALUE_POLICY, class HASHER>
    static
    void rehash(HashTableAnchor   *newAnchor,
                BidirectionalLink *oldRoot,
                const HASHER&      hash);
        // Note that this function is not exception safe in the presence of a
        // throwing 'hash' functor.  This issue is easily alleviated if the
        // hash code is cached in each hashtable node, allowing this method
        // to be written as a non-generic function that does not need a hash
        // policy or functor.  The trade-off in memory for this truly rare
        // use case is deemed to be poor.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

// BSLS_ASSERT macros pulled all the way to the left margin are asserting that
// the implementation itself is bug-free, rather than enforcing public
// contracts.  They would not be present in any final release, as the
// comprehensive component test driver serves the same purpose.

                        //--------------------
                        // class HashTableImpUtil
                        //--------------------

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

    // lookup
template <class VALUE_POLICY, class KEY_EQUAL, class KEY_TYPE>
BidirectionalLink *HashTableImpUtil::find(const HashTableAnchor& anchor,
                                          const KEY_EQUAL&       keyComparator,
                                          const KEY_TYPE&        key,
                                          native_std::size_t     hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    const native_std::size_t bucketId = computeBucketIndex(hashCode,
                                                     anchor.bucketArraySize());
    const HashTableBucket& bucket = anchor.bucketArrayAddress()[bucketId];

    // Odd loop structure as we must test on both first/last before terminating
    // the loop as not-found.

    if (BidirectionalLink *cursor = bucket.first()) {
        for ( ; ; cursor = cursor->nextLink() ) {
            if (keyComparator(key, VALUE_POLICY::extractKey(cursor))) {
                return cursor;
            }
            if (cursor == bucket.last()) {
                break;
            }
        }
    }

    return 0;
}

template <class VALUE_POLICY, class HASHER>
void HashTableImpUtil::rehash(HashTableAnchor    *newAnchor,
                              BidirectionalLink  *oldRoot,
                              const HASHER&       hash)
{
    BSLS_ASSERT_SAFE(newAnchor);
    BSLS_ASSERT_SAFE(oldRoot);           // empty lists do not need a rehash
    BSLS_ASSERT_SAFE(!oldRoot->previousLink());  // otherwise, not a 'root'

    BidirectionalLink *newRoot = 0;

    do {
        BidirectionalLink *cursor = oldRoot;
        HashTableBucket   *bucket = findBucketForHashCode(
                                       *newAnchor,
                                       hash(VALUE_POLICY::extractKey(cursor)));

        BidirectionalLink *nextCursor  = cursor;
        // Walk list of nodes that will rehash to the same bucket
        // This will advance the list extraction point *before* we splice
        while ((oldRoot = oldRoot->nextLink()) &&
                bucket == findBucketForHashCode(
                                    *newAnchor,
                                    hash(VALUE_POLICY::extractKey(oldRoot)))) {
             nextCursor = oldRoot;
        }

        spliceSegmentIntoBucket(cursor, nextCursor, bucket, &newRoot);
    }
    while (oldRoot);

BSLS_ASSERT_SAFE(!newRoot->previousLink());  // otherwise, not a 'root' - postcondition assertion during dev.
    newAnchor->setListRootAddress(newRoot);
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
