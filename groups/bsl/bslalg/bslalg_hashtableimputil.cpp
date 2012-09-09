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

void HashTableImpUtil::removeNode(HashTableAnchor    *anchor,
                                  BidirectionalLink  *position,
                                  native_std::size_t  hashCode)
{
    BSLS_ASSERT_SAFE(position);
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(
            position->previousLink() || anchor->listRootAddress() == position);

    // Note that we must update the bucket *before* we unlink from the list,
    // as otherwise we will lose our nextLink()/prev pointers.
    
    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    if (bucket->first() == position) {
        if (bucket->last() == position) {
            bucket->reset();
        }
        else {
            bucket->setFirst(position->nextLink());
        }
    }
    else if (bucket->last() == position) {
        bucket->setLast(position->previousLink());
    }
        
    BidirectionalLink *next = position->nextLink();
    BidirectionalLinkListUtil::unlink(position);

    if (position == anchor->listRootAddress()) {
        anchor->setListRootAddress(next);
    }
}

void HashTableImpUtil::insertDuplicateAtPosition(HashTableAnchor    *anchor,
                                                 BidirectionalLink  *newNode,
                                                 native_std::size_t  hashCode,
                                                 BidirectionalLink  *location)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(location);

    BidirectionalLinkListUtil::insertLinkBeforeTarget(newNode, location);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    if (location == bucket->first()) {
        bucket->setFirst(newNode);
    }

    if (location == anchor->listRootAddress()) {
        anchor->setListRootAddress(newNode);
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

    // splice the array segment
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
