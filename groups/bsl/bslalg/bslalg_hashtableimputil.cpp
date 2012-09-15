// bslalg_hashtableimputil.cpp                                        -*-C++-*-
#include <bslalg_hashtableimputil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_hashtableanchor.h>

#include <bsls_types.h>

#include <stdio.h>

namespace BloombergLP
{

namespace bslalg
{

// PRIVATE CLASS METHODS
bool HashTableImpUtil::isReachable(const BidirectionalLink *dst,
                                   const BidirectionalLink *src)
{
    BSLS_ASSERT_SAFE(dst);
    BSLS_ASSERT_SAFE(src);

    while(src) {
        if(src == dst) {
            return true;                                              // RETURN
        }
        src = src->nextLink();
    }
    return false;
}

// CLASS METHODS
void HashTableImpUtil::remove(HashTableAnchor    *anchor,
                              BidirectionalLink  *link,
                              native_std::size_t  hashCode)
{
    BSLS_ASSERT_SAFE(link);
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(
            link->previousLink() || anchor->listRootAddress() == link);

#ifdef BDE_BUILD_TARGET_SAFE2
    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    BSLS_ASSERT_SAFE2(bucketContainsLink(bucket, link));
#endif

    // Note that we must update the bucket *before* we unlink from the list,
    // as otherwise we will lose our nextLink()/prev pointers.

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    if (bucket->first() == link) {
        if (bucket->last() == link) {
            bucket->reset();
        }
        else {
            bucket->setFirst(link->nextLink());
        }
    }
    else if (bucket->last() == link) {
        bucket->setLast(link->previousLink());
    }

    BidirectionalLink *next = link->nextLink();
    BidirectionalLinkListUtil::unlink(link);

    if (link == anchor->listRootAddress()) {
        anchor->setListRootAddress(next);
    }
}

void HashTableImpUtil::insertAtPosition(HashTableAnchor    *anchor,
                                        BidirectionalLink  *link,
                                        native_std::size_t  hashCode,
                                        BidirectionalLink  *position)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(link);
    BSLS_ASSERT_SAFE(position);

#ifdef BDE_BUILD_TARGET_SAFE2
    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    BSLS_ASSERT_SAFE2(bucketContainsLink(bucket, position));
#endif

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);

    BidirectionalLinkListUtil::insertLinkBeforeTarget(link, position);

    if (position == bucket->first()) {
        bucket->setFirst(link);
    }
    if (position == anchor->listRootAddress()) {
        anchor->setListRootAddress(link);
    }
}

void HashTableImpUtil::insertAtFrontOfBucket(HashTableAnchor    *anchor,
                                             BidirectionalLink  *newNode,
                                             native_std::size_t  hashCode)
{
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(anchor);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    BSLS_ASSERT_SAFE(bucket);

    if (bucket->first()) {
        BidirectionalLinkListUtil::insertLinkBeforeTarget(newNode,
                                                          bucket->first());
        if (anchor->listRootAddress() == bucket->first()) {
            anchor->setListRootAddress(newNode);
        }
        bucket->setFirst(newNode);
    }
    else {
        // New bucket is required.

        BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                    newNode,
                                                    anchor->listRootAddress());

        // New buckets are inserted in front of the list.

        anchor->setListRootAddress(newNode);
        bucket->setFirstAndLast(newNode, newNode);
    }
}

void HashTableImpUtil::spliceSegmentIntoBucket(BidirectionalLink  *cursor,
                                               BidirectionalLink  *nextCursor,
                                               HashTableBucket    *bucket,
                                               BidirectionalLink **newRoot)
{
    BSLS_ASSERT_SAFE(cursor);
    BSLS_ASSERT_SAFE(nextCursor);
    BSLS_ASSERT_SAFE(bucket);
    BSLS_ASSERT_SAFE(newRoot);

    if (!bucket->first()) {
        bucket->setFirstAndLast(cursor, nextCursor);
        BidirectionalLinkListUtil::spliceListBeforeTarget(cursor,
                                                          nextCursor,
                                                          *newRoot);
        *newRoot = cursor;
    }
    else {
        BidirectionalLinkListUtil::spliceListBeforeTarget(cursor,
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
