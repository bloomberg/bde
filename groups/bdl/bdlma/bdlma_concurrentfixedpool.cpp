// bdlma_concurrentfixedpool.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlma_concurrentfixedpool.h>

#include <bdlb_bitutil.h>

#include <bslma_default.h>

#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace {

enum {
    k_DEFAULT_BACKOFF_LEVEL = 4
};

// STATIC HELPER FUNCTIONS
void backoff(int *contentionCount, int backoffLevel)
{
    if (0 == backoffLevel) {
        return;                                                       // RETURN
    }

    enum { k_MAX_SPIN_LEVEL = 10 };

    int count = ++(*contentionCount);
    if (count > k_MAX_SPIN_LEVEL) {
        *contentionCount = 0;
        bslmt::ThreadUtil::yield();  // exhaust time slice
    }
    else {
        int maxSpin = backoffLevel << count;
        for (volatile int spin = 0; spin < maxSpin; ++spin) {
            // spin
        }
    }
}

}  // close unnamed namespace

namespace bdlma {

                        // -------------------------
                        // class ConcurrentFixedPool
                        // -------------------------

// PRIVATE MANIPULATORS
void *ConcurrentFixedPool::allocateNew()
{
    Node *node;
    int   numNodes;

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_nodePoolMutex);

        numNodes = d_numNodes;
        if (numNodes == (int)d_nodes.size()) {
            return 0;                                                 // RETURN
        }

        ++d_numNodes;
        node = (Node *)d_nodePool.allocate();
    }

    const int genCount = d_sizeMask + 1;  // initial generation count
    node->d_next = (unsigned)numNodes + 1 + genCount;
    d_nodes[numNodes] = node;
    return (char *)node + d_dataOffset;
}

// CREATORS
ConcurrentFixedPool::ConcurrentFixedPool(int               objectSize,
                                         int               poolSize,
                                         bslma::Allocator *basicAllocator)
: d_freeList(0)
, d_sizeMask(bdlb::BitUtil::roundUpToBinaryPower(
                                 static_cast<bsl::uint32_t>(poolSize + 1)) - 1)
, d_nodes(poolSize, bslma::Default::allocator(basicAllocator))
, d_dataOffset(bsl::max((int)sizeof(Node),
                  bsls::AlignmentUtil::calculateAlignmentFromSize(objectSize)))
, d_nodeSize(
          (objectSize + d_dataOffset + d_dataOffset - 1) & ~(d_dataOffset - 1))
, d_nodePool(d_nodeSize, bslma::Default::allocator(basicAllocator))
, d_numNodes(0)
, d_objectSize(objectSize)
, d_backoffLevel(k_DEFAULT_BACKOFF_LEVEL)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < objectSize);
    BSLS_ASSERT(0 < poolSize);
    BSLS_ASSERT(0x1FFFFFF >= poolSize);

    BSLS_ASSERT(d_dataOffset >= (int)sizeof(Node));
    BSLS_ASSERT(d_dataOffset >=
                  bsls::AlignmentUtil::calculateAlignmentFromSize(objectSize));
    BSLS_ASSERT(d_nodeSize >= objectSize + d_dataOffset);
    BSLS_ASSERT(d_dataOffset <=
                  bsls::AlignmentUtil::calculateAlignmentFromSize(d_nodeSize));
}

ConcurrentFixedPool::~ConcurrentFixedPool()
{
}

// MANIPULATORS
void *ConcurrentFixedPool::allocate()
{
    int contentionCount = 0;

    int head;

    Node *node;

    while (1) {
        head = d_freeList.loadRelaxed();
        if (!head) {
            return allocateNew();                                     // RETURN
        }

        node = d_nodes[((unsigned)head & d_sizeMask) - 1];
        if (head == d_freeList.testAndSwap(head, node->d_next)) {
            break;
        }

        backoff(&contentionCount, d_backoffLevel);
    }

    const int genCount = d_sizeMask + 1;  // generation count increment
    node->d_next = (unsigned)head + genCount;

    return (char *)node + d_dataOffset;
}

void ConcurrentFixedPool::deallocate(void *address)
{
    int contentionCount = 0;

    Node *node = (Node *)(void *)((char *)address - d_dataOffset);
    int index = node->d_next;  // 'd_next' contains the link index of this link
                               // node advanced by one generation.
    while (1) {
        int old = d_freeList.loadRelaxed();
        node->d_next = old;
        if (old == d_freeList.testAndSwap(old, index)) {
            break;
        }

        backoff(&contentionCount, d_backoffLevel);
    }
}

void ConcurrentFixedPool::release()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_nodePoolMutex);

    d_freeList = 0;
    d_numNodes = 0;

    d_nodePool.release();
}

int ConcurrentFixedPool::reserveCapacity(int numObjects)
{
    BSLS_ASSERT(0 <= numObjects);

    int contentionCount = 0;

    const int poolSize = static_cast<int>(d_nodes.size()); // size of this pool

    const int genCount = d_sizeMask + 1;  // generation count increment

    Node  reserved;              // head of the list of reserved nodes
    Node *lastNode = &reserved;  // last node in the reserved list

    // Reserve nodes using the free list.
    while (numObjects) {
        int head = d_freeList.loadRelaxed();
        if (!head) {
            break;
        }

        Node *node = d_nodes[((unsigned)head & d_sizeMask) - 1];
        if (head != d_freeList.testAndSwap(head, node->d_next)) {
            backoff(&contentionCount, d_backoffLevel);
            continue;
        }

        lastNode->d_next = (unsigned)head + genCount;
        lastNode = node;

        --numObjects;
    }

    // Reserve additional items from 'd_nodePool'.
    while (numObjects) {
        Node *node;
        int numNodes;

        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_nodePoolMutex);

            numNodes = d_numNodes;
            if (numNodes == poolSize) {
                break;
            }

            ++d_numNodes;
            node = (Node *)d_nodePool.allocate();
        }

        d_nodes[numNodes] = node;

        lastNode->d_next = (unsigned)numNodes + 1 + genCount;
        lastNode = node;

        --numObjects;
    }

    // Add the reserved nodes to the free list.
    if (lastNode != &reserved) {
        int head = reserved.d_next;
        while (1) {
            int old = d_freeList.loadRelaxed();
            lastNode->d_next = old;
            if (old == d_freeList.testAndSwap(old, head)) {
                break;
            }
        }
    }

    return numObjects;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
