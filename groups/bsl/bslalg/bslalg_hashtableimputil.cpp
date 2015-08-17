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

                        //-----------------------
                        // class HashTableImpUtil
                        //-----------------------

// CLASS METHODS
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

}  // close package namespace

}  // close enterprise namespace
// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
