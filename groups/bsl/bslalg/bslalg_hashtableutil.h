// bslalg_hashtableutil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEUTIL
#define INCLUDED_BSLALG_HASHTABLEUTIL

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

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
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
using std::size_t;
                          // ===================
                          // class HashTableUtil
                          // ===================

struct HashTableUtil {
  private:
    static
    HashTableBucket *bucketForHashCode(HashTableBucket *buckets,
                                       int              nBuckets,
                                       size_t           hashCode);

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
    size_t bucketNumberForHashCode(size_t hashCode, size_t numBuckets);

    static
    void insertAtFrontOfBucket(BidirectionalLink  *newNode,
                               size_t              hashCode,
                               BidirectionalLink **listRoot,
                               HashTableBucket    *buckets,
                               int                 nBuckets);

    static
    void insertDuplicateAtPosition(BidirectionalLink  *newNode,
                                   size_t              hashCode,
                                   BidirectionalLink  *location,
                                   BidirectionalLink **listRoot,
                                   HashTableBucket    *buckets,
                                   int                 nBuckets);

    static
    void removeNodeFromTable(BidirectionalLink  *position,
                             size_t              hashCode,
                             BidirectionalLink **listRoot,
                             HashTableBucket    *buckets,
                             int                 nBuckets);

    // lookup
    template <class VALUE_POLICY, class KEY_EQUAL, class KEY_TYPE>
    static
    BidirectionalLink *find(const KEY_EQUAL&       keyComparator,
                            const HashTableBucket *buckets,
                            int                    nBuckets,
                            const KEY_TYPE&        key,
                            size_t                 hashCode);

    template <class VALUE_POLICY, class HASHER>
    static
    BidirectionalLink *rehash(HashTableBucket   *newBuckets,
                              int                newBucketCount,
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
                        // class HashTableUtil
                        //--------------------

inline
HashTableBucket *HashTableUtil::bucketForHashCode(HashTableBucket *buckets,
                                                  int              nBuckets,
                                                  size_t           hashCode)
{
    BSLS_ASSERT_SAFE(buckets);
    BSLS_ASSERT_SAFE(nBuckets);
    return
          &buckets[HashTableUtil::bucketNumberForHashCode(hashCode, nBuckets)];
}

inline
size_t HashTableUtil::bucketNumberForHashCode(size_t hashCode,
                                              size_t numBuckets)
{
    BSLS_ASSERT_SAFE(0 != numBuckets);
    return hashCode % numBuckets;
}

    // lookup
template <class VALUE_POLICY, class KEY_EQUAL, class KEY_TYPE>
BidirectionalLink *HashTableUtil::find(const KEY_EQUAL&       keyComparator,
                                       const HashTableBucket *buckets,
                                       int                    nBuckets,
                                       const KEY_TYPE&        key,
                                       size_t                 hashCode)
{
    BSLS_ASSERT_SAFE(0 != buckets);
    BSLS_ASSERT_SAFE(0 != nBuckets);

    //const size_t hashCode = hashPolicy.hashValue(key);
    const size_t bucketId = bucketNumberForHashCode(hashCode, nBuckets);
    const HashTableBucket& bucket = buckets[bucketId];

    // Odd loop structure as we must test on both first/last before terminating
    // the loop as not-found.
    if (BidirectionalLink *cursor = bucket.first()) {
        for ( ; ; cursor = cursor->next() ) {
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
BidirectionalLink *HashTableUtil::rehash(HashTableBucket   *newBuckets,
                                         int                newBucketCount,
                                         BidirectionalLink *oldRoot,
                                         const HASHER&      hash)
{
    BSLS_ASSERT_SAFE(newBuckets);
    BSLS_ASSERT_SAFE(newBucketCount);
    BSLS_ASSERT_SAFE(oldRoot);           // empty lists do not need a rehash
    BSLS_ASSERT_SAFE(!oldRoot->prev());  // otherwise, not a 'root'

    BidirectionalLink   *newRoot = 0;

    do {
        BidirectionalLink   *cursor  = oldRoot;

        HashTableBucket *bucket  = bucketForHashCode(
                                       newBuckets,
                                       newBucketCount,
                                       hash(VALUE_POLICY::extractKey(cursor)));

        BidirectionalLink *nextCursor  = cursor;
        // Walk list of nodes that will rehash to the same bucket
        // This will advance the list extraction point *before* we splice
        while ((oldRoot  = oldRoot->next()) &&
                bucket == bucketForHashCode(
                                    newBuckets,
                                    newBucketCount,
                                    hash(VALUE_POLICY::extractKey(oldRoot)))) {
             nextCursor = oldRoot;
        }

        spliceSegmentIntoBucket(cursor, nextCursor, bucket, &newRoot);
    }
    while (oldRoot);

BSLS_ASSERT_SAFE(!newRoot->prev());  // otherwise, not a 'root' - postcondition assertion during dev.
    return newRoot;
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
