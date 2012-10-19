// bslalg_hashtableimputil.cpp                                        -*-C++-*-
#include <bslalg_hashtableimputil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_hashtableanchor.h>

#include <bsls_types.h>

namespace BloombergLP
{

namespace bslalg
{

                    //-------------------------------------
                    // class HashTableImpUtil::UniqueTester
                    //-------------------------------------

HashTableImpUtil::UniqueTester::UniqueTester(bslma::Allocator *allocator)
: d_hashNodesForBucket(0)
, d_freeHashNodes(0)
, d_allocator_p(allocator)
{
    BSLS_ASSERT_SAFE(allocator);
}

HashTableImpUtil::UniqueTester::~UniqueTester()
{
    for (HashNode *node = d_hashNodesForBucket; node; ) {
        HashNode *condemned = node;
        node = node->d_next;

        d_allocator_p->deallocate(condemned);
    }
    for (HashNode *node = d_freeHashNodes;      node; ) {
        HashNode *condemned = node;
        node = node->d_next;

        d_allocator_p->deallocate(condemned);
    }
}

void HashTableImpUtil::UniqueTester::clear()
{
    // Move all the nodes in the 'd_hashNodesForBucket' list to the
    // free list.

    while (d_hashNodesForBucket) {
        HashNode *movingNode = d_hashNodesForBucket;
        d_hashNodesForBucket = movingNode->d_next;

        movingNode->d_next = d_freeHashNodes;
        d_freeHashNodes = movingNode;
    }
}

bool HashTableImpUtil::UniqueTester::registerUniqueHashValue(
                                                       native_std::size_t hash)
{
    for (const HashNode *finder = d_hashNodesForBucket; finder;
                                                     finder = finder->d_next) {
        if (finder->d_hash == hash) {
            return false;                                             // RETURN
        }
    }

    // 'value' is unique.  Add it to the list.

    // Get a new node to store it in.

    HashNode *newNode;
    if (d_freeHashNodes) {
        newNode = d_freeHashNodes;
        d_freeHashNodes = newNode->d_next;
    }
    else {
        newNode = (HashNode *) d_allocator_p->allocate(sizeof(HashNode));
    }

    // Enter the new hash node into the list

    newNode->d_hash = hash;
    newNode->d_next = d_hashNodesForBucket;
    d_hashNodesForBucket = newNode;

    return true;
}

                        //-----------------------
                        // class HashTableImpUtil
                        //-----------------------

// CLASS METHODS
void HashTableImpUtil::remove(HashTableAnchor    *anchor,
                              BidirectionalLink  *link,
                              native_std::size_t  hashCode)
{
    BSLS_ASSERT(link);
    BSLS_ASSERT(anchor);
    BSLS_ASSERT(link->previousLink() || anchor->listRootAddress() == link);

    // Note that we must update the bucket *before* we unlink from the list,
    // as otherwise we will lose our nextLink()/prev pointers.

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT(bucket);
    BSLS_ASSERT_SAFE(bucketContainsLink(*bucket, link));
#endif

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
    BSLS_ASSERT(anchor);
    BSLS_ASSERT(link);
    BSLS_ASSERT(position);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT(bucket);
    BSLS_ASSERT_SAFE(bucketContainsLink(*bucket, position));
#endif

    BidirectionalLinkListUtil::insertLinkBeforeTarget(link, position);

    if (position == bucket->first()) {
        bucket->setFirst(link);
    }
    if (position == anchor->listRootAddress()) {
        anchor->setListRootAddress(link);
    }
}

void HashTableImpUtil::insertAtFrontOfBucket(HashTableAnchor    *anchor,
                                             BidirectionalLink  *link,
                                             native_std::size_t  hashCode)
{
    BSLS_ASSERT(anchor);
    BSLS_ASSERT(link);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    BSLS_ASSERT_SAFE(bucket);

    if (bucket->first()) {
        BidirectionalLinkListUtil::insertLinkBeforeTarget(link,
                                                          bucket->first());
        if (anchor->listRootAddress() == bucket->first()) {
            anchor->setListRootAddress(link);
        }
        bucket->setFirst(link);
    }
    else {
        // New bucket is required.

        BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                    link,
                                                    anchor->listRootAddress());

        // New buckets are inserted in front of the list.

        anchor->setListRootAddress(link);
        bucket->setFirstAndLast(link, link);
    }
}

void HashTableImpUtil::insertAtBackOfBucket(HashTableAnchor    *anchor,
                                            BidirectionalLink  *link,
                                            native_std::size_t  hashCode)
{
    BSLS_ASSERT(anchor);
    BSLS_ASSERT(link);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);
    BSLS_ASSERT_SAFE(bucket);

    if (bucket->last()) {
        BidirectionalLinkListUtil::insertLinkAfterTarget(link,
                                                         bucket->last());
        bucket->setLast(link);
    }
    else {
        // New bucket is required.

        BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                    link,
                                                    anchor->listRootAddress());

        // New buckets are inserted in front of the list.

        anchor->setListRootAddress(link);
        bucket->setFirstAndLast(link, link);
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

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);

#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT_SAFE(bucket->first());
    BSLS_ASSERT_SAFE(bucketContainsLink(*bucket, position));
#endif

    BidirectionalLinkListUtil::insertLinkBeforeTarget(link, position);

    if (position == bucket->first()) {
        bucket->setFirst(link);
    }
    if (position == anchor->listRootAddress()) {
        anchor->setListRootAddress(link);
    }
}

}  // close namespace BloombergLP::bslalg

}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
