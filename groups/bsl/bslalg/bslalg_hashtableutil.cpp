// bslalg_hashtableutil.cpp                                           -*-C++-*-
#include <bslalg_hashtableutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallistutil.h>

#include <bsls_types.h>

#include <stdio.h>

namespace BloombergLP
{

namespace bslalg
{

void HashTableUtil::removeNodeFromTable(BidirectionalLink  *position,
                                        size_t              hashCode,
                                        BidirectionalLink **listRoot,
                                        HashTableBucket    *buckets,
                                        int                 nBuckets)
{
    BSLS_ASSERT_SAFE(position);
    BSLS_ASSERT_SAFE(listRoot);
    BSLS_ASSERT_SAFE(*listRoot);
    BSLS_ASSERT_SAFE(nBuckets);
    BSLS_ASSERT_SAFE(position->prev() || *listRoot == position);

    // Note that we must update the bucket *before* we unlink from the list,
    // as otherwise we will lose our next/prev pointers.
    HashTableBucket *bucket = bucketForHashCode(buckets, nBuckets, hashCode);
    if (bucket->first() == position) {
        if (bucket->last() == position) {
            bucket->clearBucket();
        }
        else {
            bucket->setFirst(position->next());
        }
    }
    else if (bucket->last() == position) {
        bucket->setLast(position->prev());
    }

    if (position == *listRoot) {
        *listRoot = position->next();
    }

    BidirectionalListUtil::unlink(position);
}

void HashTableUtil::insertDuplicateAtPosition(BidirectionalLink  *newNode,
                                              size_t              hashCode,
                                              BidirectionalLink  *location,
                                              BidirectionalLink **listRoot,
                                              HashTableBucket    *buckets,
                                              int                 nBuckets)
{
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(location);
    BSLS_ASSERT_SAFE(listRoot);
    BSLS_ASSERT_SAFE(*listRoot);
    BSLS_ASSERT_SAFE(buckets);
    BSLS_ASSERT_SAFE(nBuckets);

    // Why are these conditions are so important?
    // Stated assumptions about our specific implementation, but don't see a need for guarantee
    BSLS_ASSERT_SAFE(!newNode->next());
    BSLS_ASSERT_SAFE(!newNode->prev());

    BidirectionalListUtil::insertLinkBeforeTail(newNode, location);

    HashTableBucket *bucket = bucketForHashCode(buckets, nBuckets, hashCode);
    if (location == bucket->first()) {
        bucket->setFirst(newNode);
    }

    if (location == *listRoot) {
        *listRoot = newNode;
    }
}

void HashTableUtil::insertAtFrontOfBucket(BidirectionalLink  *newNode,
                                          size_t              hashCode,
                                          BidirectionalLink **listRoot,
                                          HashTableBucket    *buckets,
                                          int                 nBuckets)
{
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(listRoot);
    BSLS_ASSERT_SAFE(buckets);
    BSLS_ASSERT_SAFE(nBuckets);

    // Why are these conditions are so important?
    BSLS_ASSERT_SAFE(!newNode->next());
    BSLS_ASSERT_SAFE(!newNode->prev());

    HashTableBucket *bucket = bucketForHashCode(buckets, nBuckets, hashCode);
    BSLS_ASSERT_SAFE(bucket);

    if (bucket->first()) {
        BidirectionalListUtil::insertLinkBeforeTail(newNode, bucket->first());
        if (*listRoot ==  bucket->first()) {
            *listRoot = newNode;
        }
        bucket->setFirst(newNode);
    }
    else {
        // New bucket required
        BidirectionalListUtil::insertLinkBeforeTail(newNode, *listRoot);
        *listRoot = newNode;   // New buckets prepend to the front of the list
        bucket->fillNewBucket(newNode);
    }
}

void HashTableUtil::spliceSegmentIntoBucket(BidirectionalLink  *cursor,
                                            BidirectionalLink  *nextCursor,
                                            HashTableBucket    *bucket,
                                            BidirectionalLink **newRoot)
{
    BSLS_ASSERT_SAFE(cursor);
    BSLS_ASSERT_SAFE(nextCursor);
    BSLS_ASSERT_SAFE(bucket);
    BSLS_ASSERT_SAFE(newRoot);

    // splice the array segment
    if (!bucket->first()) {
        bucket->createBucket(cursor, nextCursor);
        BidirectionalListUtil::spliceListBeforeLink(cursor,
                                                    nextCursor,
                                                    *newRoot);
        *newRoot = cursor;
    }
    else {
        BidirectionalListUtil::spliceListBeforeLink(cursor,
                                                    nextCursor,
                                                    bucket->first());
        if (bucket->first() == *newRoot) { // Check before updating 'first'!
            *newRoot = cursor;
        }
        bucket->setFirst(cursor);
    }
}

}  // close namespace BloobmergLP::bslalg
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
