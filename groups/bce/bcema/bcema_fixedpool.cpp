// bcema_fixedpool.cpp    -*-C++-*-

#include <bcema_fixedpool.h>

#include <bcemt_lockguard.h>

#include <bdes_bitutil.h>

#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {

namespace {

enum {
    DEFAULT_BACKOFF_LEVEL = 4
};

// STATIC HELPER FUNCTIONS
void backoff(int *contentionCount, int backoffLevel)
{
    if (0 == backoffLevel) {
        return;
    }

    enum { MAX_SPIN_LEVEL = 10 };

    int count = ++(*contentionCount);
    if (count > MAX_SPIN_LEVEL) {
        *contentionCount = 0;
        bcemt_ThreadUtil::yield();  // exhaust time slice
    }
    else {
        int maxSpin = backoffLevel << count;
        for (volatile int spin = 0; spin < maxSpin; ++spin) {
            // spin
        }
    }
}

}  // close unnamed namespace

                        // ---------------------
                        // class bcema_FixedPool
                        // ---------------------

// PRIVATE MANIPULATORS
void *bcema_FixedPool::allocateNew()
{
    Node *node;
    int   numNodes;

    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_nodePoolMutex);

        numNodes = d_numNodes;
        if (numNodes == (int)d_nodes.size()) {
            return 0;
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
bcema_FixedPool::bcema_FixedPool(int              objectSize,
                                 int              poolSize,
                                 bslma_Allocator *basicAllocator)
: d_freeList(0)
, d_sizeMask(bdes_BitUtil::roundUpToBinaryPower(poolSize + 1) - 1)
, d_nodes(poolSize, bslma_Default::allocator(basicAllocator))
, d_dataOffset(bsl::max((int)sizeof(Node),
                   bsls_AlignmentUtil::calculateAlignmentFromSize(objectSize)))
, d_nodeSize(
          (objectSize + d_dataOffset + d_dataOffset - 1) & ~(d_dataOffset - 1))
, d_nodePool(d_nodeSize, bslma_Default::allocator(basicAllocator))
, d_numNodes(0)
, d_objectSize(objectSize)
, d_backoffLevel(DEFAULT_BACKOFF_LEVEL)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 < objectSize);
    BSLS_ASSERT(0 < poolSize);
    BSLS_ASSERT(0x1FFFFFF >= poolSize);

    BSLS_ASSERT(d_dataOffset >= (int)sizeof(Node));
    BSLS_ASSERT(d_dataOffset >=
                   bsls_AlignmentUtil::calculateAlignmentFromSize(objectSize));
    BSLS_ASSERT(d_nodeSize >= objectSize + d_dataOffset);
    BSLS_ASSERT(d_dataOffset <=
                   bsls_AlignmentUtil::calculateAlignmentFromSize(d_nodeSize));
}

bcema_FixedPool::~bcema_FixedPool()
{
}

// MANIPULATORS
void *bcema_FixedPool::allocate()
{
    int contentionCount = 0;

    int head;

    Node *node;

    while (1) {
        head = d_freeList.relaxedLoad();
        if (!head) {
            return allocateNew();
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

void bcema_FixedPool::deallocate(void *address)
{
    int contentionCount = 0;

    Node *node = (Node *)(void *)((char *)address - d_dataOffset);
    int index = node->d_next;  // 'd_next' contains the link index of this
                               // link node advanced by one generation.
    while (1) {
        int old = d_freeList.relaxedLoad();
        node->d_next = old;
        if (old == d_freeList.testAndSwap(old, index)) {
            break;
        }

        backoff(&contentionCount, d_backoffLevel);
    }
}

void bcema_FixedPool::release()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_nodePoolMutex);

    d_freeList = 0;
    d_numNodes = 0;

    d_nodePool.release();
}

int bcema_FixedPool::reserveCapacity(int numObjects)
{
    BSLS_ASSERT(0 <= numObjects);

    int contentionCount = 0;

    const int poolSize = d_nodes.size();  // size of this pool
    const int genCount = d_sizeMask + 1;  // generation count increment

    Node  reserved;              // head of the list of reserved nodes
    Node *lastNode = &reserved;  // last node in the reserved list

    // Reserve nodes using the free list.
    while (numObjects) {
        int head = d_freeList.relaxedLoad();
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
            bcemt_LockGuard<bcemt_Mutex> guard(&d_nodePoolMutex);

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
            int old = d_freeList.relaxedLoad();
            lastNode->d_next = old;
            if (old == d_freeList.testAndSwap(old, head)) {
                break;
            }
        }
    }

    return numObjects;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
