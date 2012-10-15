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

// CLASS METHODS
void HashTableImpUtil::remove(HashTableAnchor    *anchor,
                              BidirectionalLink  *link,
                              native_std::size_t  hashCode)
{
    BSLS_ASSERT_SAFE(link);
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(
            link->previousLink() || anchor->listRootAddress() == link);

    HashTableBucket *bucket = findBucketForHashCode(*anchor, hashCode);

#ifdef BDE_BUILD_TARGET_SAFE_2
    BSLS_ASSERT_SAFE(bucket->first());
    BSLS_ASSERT_SAFE(bucketContainsLink(bucket, link));
#endif

    // Note that we must update the bucket *before* we unlink from the list,
    // as otherwise we will lose our nextLink()/prev pointers.

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

void HashTableImpUtil::insertAtFrontOfBucket(HashTableAnchor    *anchor,
                                             BidirectionalLink  *link,
                                             native_std::size_t  hashCode)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(link);

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
    BSLS_ASSERT_SAFE(bucketContainsLink(bucket, position));
#endif

    BidirectionalLinkListUtil::insertLinkBeforeTarget(link, position);

    if (position == bucket->first()) {
        bucket->setFirst(link);
    }
    if (position == anchor->listRootAddress()) {
        anchor->setListRootAddress(link);
    }
}

void HashTableImpUtil::spliceListIntoBucket(HashTableAnchor    *anchor,
                                            native_std::size_t  bucketIndex,
                                            BidirectionalLink  *first,
                                            BidirectionalLink  *last)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(anchor->bucketArraySize() > bucketIndex);
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);

    BidirectionalLink *root   = anchor->listRootAddress();
    HashTableBucket   *bucket = &anchor->bucketArrayAddress()[bucketIndex];

    if (!bucket->first()) {
        bucket->setFirstAndLast(first, last);

        BidirectionalLinkListUtil::spliceListBeforeTarget(first, last, root);
        anchor->setListRootAddress(first);
    }
    else {
        BidirectionalLinkListUtil::spliceListBeforeTarget(first,
                                                          last,
                                                          bucket->first());
        if (bucket->first() == root) {
            anchor->setListRootAddress(first);
        }
        bucket->setFirst(first);
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
