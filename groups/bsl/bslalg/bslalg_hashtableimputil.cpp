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
// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
