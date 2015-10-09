// bdlcc_skiplist.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlcc_skiplist.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_skiplist_cpp,"$Id$ $CSID$")

#include <bdlma_infrequentdeleteblocklist.h>
#include <bdlb_random.h>

#include <bslma_allocator.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

namespace BloombergLP {

enum {
      k_REF_COUNT_NUM_BITS = bdlcc::SkipList_Control::k_NUM_REFERENCE_BITS
    , k_REF_COUNT_INC = 1
    , k_MAX_REF_COUNT = (1 << k_REF_COUNT_NUM_BITS) - 1
    , k_REF_COUNT_MASK = k_MAX_REF_COUNT

    , k_RELEASE_FLAG_NUM_BITS = 1
    , k_RELEASE_FLAG_OFFSET = k_REF_COUNT_NUM_BITS
    , k_RELEASE_FLAG_MASK = 1 << k_RELEASE_FLAG_OFFSET

    , k_ACQUIRE_COUNT_NUM_BITS = 7
    , k_ACQUIRE_COUNT_OFFSET = k_RELEASE_FLAG_OFFSET + k_RELEASE_FLAG_NUM_BITS
    , k_ACQUIRE_COUNT_INC = 1 << k_ACQUIRE_COUNT_OFFSET
    , k_MAX_k_ACQUIRE_COUNT = (1 << k_ACQUIRE_COUNT_NUM_BITS) - 1
    , k_ACQUIRE_COUNT_MASK = k_MAX_k_ACQUIRE_COUNT << k_ACQUIRE_COUNT_OFFSET

      // The value of this is asserted below as a sanity-check.  If the number
      // of bits is changed, the assertion will need to change.
    , RESERVED_NUM_BITS = 32 - k_ACQUIRE_COUNT_NUM_BITS -
                               k_RELEASE_FLAG_NUM_BITS -
                               k_REF_COUNT_NUM_BITS
};

namespace bdlcc {

                           // ======================
                           // class SkipList_Control
                           // ======================

// MANIPULATORS
void SkipList_Control::init(int level)
{
    BSLS_ASSERT(static_cast<unsigned>(level) <= 31);  // k_MAX_LEVEL

    d_level = static_cast<unsigned char>(level);
    d_cw    = 0;
}

int SkipList_Control::incrementRefCount()
{
    int oldBits = d_cw;
    BSLS_ASSERT((oldBits & k_REF_COUNT_MASK) != k_REF_COUNT_MASK);

    int newBits = oldBits + k_REF_COUNT_INC;
    int result;

    while (oldBits != (result = d_cw.testAndSwap(oldBits, newBits))) {
        oldBits = result;
        BSLS_ASSERT((oldBits & k_REF_COUNT_MASK) != k_REF_COUNT_MASK);

        newBits = oldBits + k_REF_COUNT_INC;
    }

    return newBits & k_REF_COUNT_MASK;
}

int SkipList_Control::decrementRefCount()
{
    int oldBits = d_cw;
    BSLS_ASSERT(oldBits & k_REF_COUNT_MASK);

    int newBits = oldBits - k_REF_COUNT_INC;
    int result;

    while (oldBits != (result = d_cw.testAndSwap(oldBits, newBits))) {
        oldBits = result;
        BSLS_ASSERT(oldBits & k_REF_COUNT_MASK);

        newBits = oldBits - k_REF_COUNT_INC;
    }

    return newBits & k_REF_COUNT_MASK;
}

// ACCESSORS
int SkipList_Control::level() const
{
    return d_level;
}

                    // ===================================
                    // class SkipList_RandomLevelGenerator
                    // ===================================

SkipList_RandomLevelGenerator::SkipList_RandomLevelGenerator()
: d_seed(k_SEED), d_randomBits(1)
{
}

int SkipList_RandomLevelGenerator::randomLevel()
{
    // This routine is "thread-safe enough".

    int randomBits = d_randomBits.loadRelaxed();

    int level = 0;
    int b;

    do {
        if (1 == randomBits) {
            // Only the sentinel bit left.  Regenerate.

            int seed = d_seed.loadRelaxed();
            randomBits = bdlb::Random::generate15(&seed);
            d_seed.storeRelaxed(seed);
            BSLS_ASSERT((randomBits >> 15) == 0);

            randomBits |= (1 << 14); // Set the sentinel bit.
        }

        b = randomBits&3;
        level += !b;
        randomBits >>= 2;

    } while (!b);

    d_randomBits.storeRelaxed(randomBits);

    return level > k_MAX_LEVEL ? k_MAX_LEVEL : level;
}

}  // close package namespace

                        // ============================
                        // class bcec_SkipList_PoolNode
                        // ============================

struct bcec_SkipList_PoolNode {
    typedef bdlcc::SkipList_Control  Control;
    typedef bcec_SkipList_PoolNode Node;

    Control         d_control; // must be first!
    Node *volatile  d_next_p;
};

                          // ========================
                          // class bcec_SkipList_Pool
                          // ========================

struct bcec_SkipList_Pool {
    typedef bcec_SkipList_PoolNode Node;

    bsls::AtomicPointer<Node> d_freeList;
    int                      d_objectSize;
    int                      d_numObjects;
    int                      d_level;
};

namespace bdlcc {

                         // ==========================
                         // class SkipList_PoolManager
                         // ==========================

class SkipList_PoolManager {
    enum {
        k_MAX_POOLS           =  32,

        k_INITIAL_NUM_OBJECTS =  -1,  // default 'numObjects' value

        k_GROW_FACTOR         =   2,  // multiplicative factor to grow pool
                                      // capacity

        k_MAX_NUM_OBJECTS     = -32   // minimum 'd_numObjects' value beyond
                                      // which 'd_numObjects' becomes positive
    };

    typedef bcec_SkipList_PoolNode  Node;
    typedef bcec_SkipList_Pool      Pool;

    bdlma::InfrequentDeleteBlockList   d_blockList;  // supplies free memory
    bslmt::Mutex                       d_mutex;      // protects the block list

    Pool                              d_pools[k_MAX_POOLS];

    void initPool(Pool *pool, int level, int objectSize);
    void replenish(Pool *pool);
    void *allocate(Pool *pool);
    void deallocate(Pool *pool, void *node);

  private:
    // Not implemented:
    SkipList_PoolManager(const SkipList_PoolManager&);

  public:
    explicit SkipList_PoolManager(int              *objectSizes,
                                  int               numPools,
                                  bslma::Allocator *basicAllocator);
    ~SkipList_PoolManager();

    void *allocate(int level);
    void deallocate(void *node);
};

void SkipList_PoolManager::replenish(Pool *pool)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    int objectSize = pool->d_objectSize;
    int numObjects = (pool->d_numObjects >= 0 ? pool->d_numObjects :
                                                -pool->d_numObjects);

    BSLS_ASSERT(0 < objectSize);
    BSLS_ASSERT(0 < numObjects);

    char *start = (char *) d_blockList.allocate(numObjects * objectSize);

    Node *last = (Node*)(void*)(start + (numObjects - 1) * objectSize);
    for (char *p = start; p < (char*)last; p += objectSize) {
        ((Node*)(void*)p)->d_control.init(pool->d_level);
        ((Node*)(void*)p)->d_next_p = (Node*)(void*)(p + objectSize);

    }
    last->d_control.init(pool->d_level);

    Node *old;
    do {
        old = pool->d_freeList;
        last->d_next_p = old;
    } while (old != pool->d_freeList.testAndSwap(old,(Node*)(void*)start));

    // Grow pool capacity only if 'd_numObjects' is negative and greater than
    // 'k_MAX_NUM_OBJECTS' (i.e., |newNumObjects| < |k_MAX_NUM_OBJECTS|).

    if (numObjects < 0) {
        if (numObjects > k_MAX_NUM_OBJECTS) {
            pool->d_numObjects *= k_GROW_FACTOR;
        }
        else {
            pool->d_numObjects = -numObjects;
        }
    }
}

void *SkipList_PoolManager::allocate(Pool *pool)
{
    Node *p = 0;
    for (;;) {
        p = pool->d_freeList;
        if (!p) {
            replenish(pool);
            continue;
        }

        int controlBits = p->d_control.d_cw.add(k_ACQUIRE_COUNT_INC);
        if (k_ACQUIRE_COUNT_INC != (k_ACQUIRE_COUNT_MASK & controlBits)) {
            for (int i=0; i < 3; ++i) {
                // To avoid unnecessary contention, assume that if we did not
                // get the first reference, then the other thread is about to
                // complete the pop.  Wait for a few cycles until he does.  If
                // he does not complete then go on and try to acquire it
                // ourselves.
                if (pool->d_freeList != p) {
                    break;
                }
            }
        }

        // Make sure that freeList has not changed since we acquired a
        // reference, since then it's unsafe to dereference 'p' (note that
        // therefore *both* of these 'testAndSwap's are required).

        if (pool->d_freeList.testAndSwap(0,0) == p
         && pool->d_freeList.testAndSwap(p,p->d_next_p) == p) {
            return p;                                                 // RETURN
        }
        else {
            int controlBits;
            for (;;) {
                controlBits = p->d_control.d_cw;
                if ( controlBits & k_RELEASE_FLAG_MASK ) {
                    if (controlBits == p->d_control.d_cw.testAndSwap(
                                controlBits,
                                controlBits^k_RELEASE_FLAG_MASK)) {
                        // The node is now free but not on the free list.  Take
                        // it.
                        return p;                                     // RETURN
                    }
                }
                else if (controlBits == p->d_control.d_cw.testAndSwap(
                                controlBits,
                                controlBits - k_ACQUIRE_COUNT_INC)) {
                    break;
                }
            }
        }
    }

    return p;
}

void SkipList_PoolManager::deallocate(Pool *pool, void *node)
{
    Node *old;
    Node *p = reinterpret_cast<Node *>(node);

    int controlBits;
    for (;;) {
        controlBits = p->d_control.d_cw;

        if ((k_ACQUIRE_COUNT_INC == (k_ACQUIRE_COUNT_MASK & controlBits))) {
            if (controlBits == p->d_control.d_cw.testAndSwap(
                                          controlBits,
                                          controlBits - k_ACQUIRE_COUNT_INC)) {
                break;
            }
        }
        else if (controlBits == p->d_control.d_cw.testAndSwap(
                  controlBits,
                  (controlBits - k_ACQUIRE_COUNT_INC) | k_RELEASE_FLAG_MASK)) {
            // Someone else is still trying to pop this item.  Just let them
            // have it.
            return;                                                   // RETURN
        }
    }

    for (;;) {
        old = pool->d_freeList;
        p->d_next_p = old;
        if (pool->d_freeList.testAndSwap(old, p) == old) {
            break;
        }
    }
}

inline
void SkipList_PoolManager::initPool(Pool *pool, int level, int objectSize)
{
    pool->d_freeList = 0;
    pool->d_objectSize = objectSize;
    pool->d_numObjects = k_INITIAL_NUM_OBJECTS;
    pool->d_level = level;
}

SkipList_PoolManager::SkipList_PoolManager(int              *objectSizes,
                                           int               numPools,
                                           bslma::Allocator *basicAllocator)
: d_blockList(basicAllocator)
{
    BSLS_ASSERT(numPools > 0);
    BSLS_ASSERT(numPools <= k_MAX_POOLS);

    // sanity-check
    BSLMF_ASSERT(4 == RESERVED_NUM_BITS);

    for (int i = 0; i < numPools; ++i) {
        initPool(&d_pools[i], i, objectSizes[i]);
    }
}

inline
SkipList_PoolManager::~SkipList_PoolManager()
{
}

inline
void *SkipList_PoolManager::allocate(int level)
{
    return allocate(&d_pools[level]);
}

inline
void SkipList_PoolManager::deallocate(void *node)
{
    int level = reinterpret_cast<Node *>(node)->d_control.level();
    deallocate(&d_pools[level], node);
}

                          // =======================
                          // class SkipList_PoolUtil
                          // =======================

void *SkipList_PoolUtil::allocate(PoolManager *poolManager, int level)
{
    return poolManager->allocate(level);
}

void SkipList_PoolUtil::deallocate(PoolManager *poolManager, void *address)
{
    poolManager->deallocate(address);
}

SkipList_PoolManager *SkipList_PoolUtil::createPoolManager(
                                              int              *objectSizes,
                                              int               numLevels,
                                              bslma::Allocator *basicAllocator)
{
    return new (*basicAllocator) PoolManager(objectSizes,
                                             numLevels,
                                             basicAllocator);
}

void SkipList_PoolUtil::deletePoolManager(bslma::Allocator *basicAllocator,
                                          PoolManager      *poolManager)
{
    basicAllocator->deleteObject(poolManager);
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
