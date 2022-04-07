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
#include <bsls_log.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace bdlcc {

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

namespace bdlcc {

                         // ==========================
                         // class SkipList_PoolManager
                         // ==========================

class SkipList_PoolManager {
    // Operate a set of 'k_MAX_POOLS' memory pools, each of which allocates
    // memory chunks of a certain size.

    // PRIVATE TYPES
    enum {
        k_MAX_POOLS                       =  32,

        k_INITIAL_NUM_OBJECTS_TO_ALLOCATE =   1,

        k_GROWTH_FACTOR                   =   2
    };

    struct Node {
        // PUBLIC DATA
        bsls::AtomicInt  d_refCount;
        int              d_level;
        Node            *d_next_p;
    };

    struct Pool {
        // PUBLIC DATA
        bslmt::Mutex  d_poolMutex;
        Node         *d_freeList;
#if defined(BSLS_REVIEW_IS_ACTIVE)
        int           d_numNodes;    // Number of nodes allocated and not freed
#endif
        int           d_objectSize;
        int           d_numObjectsToAllocate;
        int           d_level;
    };

    // DATA
    bdlma::InfrequentDeleteBlockList   d_blockList;  // supplies free memory
    bslmt::Mutex                       d_blockMutex; // protects the block list

    Pool                               d_pools[k_MAX_POOLS];

  private:
    // NOT IMPLEMENTED
    SkipList_PoolManager(const SkipList_PoolManager&);
    SkipList_PoolManager& operator=(const SkipList_PoolManager&);

  public:
    // CLASS METHOD
    template <class TYPE>
    static Node *toNode(TYPE *p);
        // Cast the specified pointer 'p' to 'Node *'.

    // CREATORS
    explicit SkipList_PoolManager(int              *objectSizes,
                                  int               numPools,
                                  bslma::Allocator *basicAllocator);
        // Create a pool manager having the specified 'numPools' pools, with
        // the specified 'objectSizes' being an array of 'numPools' object
        // sizes for the respective pools.  Use the specified 'basicAllocator'
        // for memory allocation.  The behavior is undefined if
        // 'numPools > k_MAX_POOLS'.

    ~SkipList_PoolManager();
        // d'tor -- if 'BSLS_REVIEW_IS_ACTIVE' is defined, fail a review
        // if any nodes are leaked.

  public:
    // MANIPULATORS

                                // pool manipulators

    void *allocate(Pool *pool);
        // Allocate a standard chunk of memory from the specified 'pool'.
        // Return a pointer to the allocated node.

    void deallocate(Pool *pool, void *node);
        // Free the specified 'node' and return it to the list of the specified
        // 'pool'.

    void initPool(Pool *pool, int level, int objectSize);
        // Initialize the specified 'pool' with the specified 'level' and
        // 'objectSize'.  Note that we don't want to make this a constructor
        // because the 'pool' objects are created in an array and we want to
        // pass 'level' and 'objectSize' at initialization.

    void replenish(Pool *pool);
        // Allocate a new block for the specified 'pool'.

                            // pool manager manipulators

    void *allocate(int level);
        // Allocate and return a node of the size appropriate for the specified
        // 'level'.

    void deallocate(void *node);
        // Free the specified 'node' and return it to its appropriate pool.
};
                            // --------------------
                            // SkipList_PoolManager
                            // --------------------

// CLASS METHOD
template <class TYPE>
SkipList_PoolManager::Node *SkipList_PoolManager::toNode(TYPE *p)
{
    return static_cast<Node *>(static_cast<void *>(p));
}

// CREATORS
SkipList_PoolManager::SkipList_PoolManager(int              *objectSizes,
                                           int               numPools,
                                           bslma::Allocator *basicAllocator)
: d_blockList(basicAllocator)
{
    BSLS_ASSERT(numPools > 0);
    BSLS_ASSERT(numPools <= k_MAX_POOLS);

    // sanity-check

    for (int i = 0; i < numPools; ++i) {
        initPool(&d_pools[i], i, objectSizes[i]);
    }
}

inline
SkipList_PoolManager::~SkipList_PoolManager()
{
#if defined(BSLS_REVIEW_IS_ACTIVE)
    int numLeakedNodes = 0;
    for (int ii = 0; ii < k_MAX_POOLS; ++ii) {
        const int numNodes = d_pools[ii].d_numNodes;

        BSLS_ASSERT_OPT(0 <= numNodes);
        numLeakedNodes += numNodes;
    }

    // If the following review check fails, it probably means that you have
    // been referring to nodes via 'bdlcc::SkipList::Pair *', which is an
    // interface that is very prone to accidentally leaking nodes.

    // Switch to using the 'bdlcc::SkipList::PairHandle' interface.
    // 'PairHandle' has a destructor and properly cleans up after itself,
    // provided all the 'PairHandle' objects have been destroyed or released
    // before the container is destroyed.

    // If you encounter this using 'EventScheduler', the same goes for
    // 'bdlmt::EventScheduler::Event *' and
    // 'bdlmt::EvenScheduler::RecurringEvent *' which, which are really just
    // cast to 'bdlcc::Skiplist::Pair *'.  Switch to using
    // 'bdlmt::EventScheduler::EventHandle' or
    // 'bdlmt::EventScheduler::RecurringEventHandle'.

    // If you were using 'bcec_SkipList', that is just an alias to
    // 'bdlcc::SkipList'.

    // If you were using 'bcep_EventScheduler', note that that is just an alias
    // to 'bdlmt::EventScheduler'.

    if (0 != numLeakedNodes) {
        BSLS_LOG_ERROR(
             "bdlcc::SkipList:~SkipList: %d node(s) leaked\n", numLeakedNodes);

        BSLS_REVIEW(0 == numLeakedNodes && "node(s) leaked");
    }
#endif
}

// MANIPULATORS

                                // pool manipulators

void *SkipList_PoolManager::allocate(Pool *pool)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&pool->d_poolMutex);

    if (!pool->d_freeList) {
        replenish(pool);
    }

    Node *p = pool->d_freeList;
    BSLS_ASSERT(p);

    pool->d_freeList = p->d_next_p;

#if defined(BSLS_REVIEW_IS_ACTIVE)
    ++pool->d_numNodes;
#endif

    return p;
}

void SkipList_PoolManager::deallocate(Pool *pool, void *node)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&pool->d_poolMutex);

    Node *p = static_cast<Node *>(node);
    p->d_next_p = pool->d_freeList;
    pool->d_freeList = p;

#if defined(BSLS_REVIEW_IS_ACTIVE)
    --pool->d_numNodes;
#endif
}

inline
void SkipList_PoolManager::initPool(Pool *pool, int level, int objectSize)
{
    pool->d_freeList = 0;
#if defined(BSLS_REVIEW_IS_ACTIVE)
    pool->d_numNodes = 0;
#endif
    pool->d_objectSize = objectSize;
    pool->d_numObjectsToAllocate = k_INITIAL_NUM_OBJECTS_TO_ALLOCATE;
    pool->d_level = level;
}

void SkipList_PoolManager::replenish(Pool *pool)
{
    BSLMT_MUTEXASSERT_IS_LOCKED(&pool->d_poolMutex);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_blockMutex);

    BSLS_ASSERT(0 == pool->d_freeList);

    int objectSize = pool->d_objectSize;
    int numObjects = pool->d_numObjectsToAllocate;

    BSLS_ASSERT(0 < objectSize);
    BSLS_ASSERT(0 < numObjects);

    char *start = static_cast<char *>(d_blockList.allocate(
                                                     numObjects * objectSize));

    char *end = start + (numObjects - 1) * objectSize;
    Node *last = toNode(end);
    for (char *p = start; p < end; p += objectSize) {
        Node *n = toNode(p);
        n->d_refCount = 0;
        n->d_level    = pool->d_level;
        n->d_next_p   = toNode(p + objectSize);
    }
    last->d_refCount = 0;
    last->d_level    = pool->d_level;
    last->d_next_p   = pool->d_freeList;

    pool->d_freeList = toNode(start);

    pool->d_numObjectsToAllocate *= k_GROWTH_FACTOR;
}

                        // pool manager manipulators

inline
void *SkipList_PoolManager::allocate(int level)
{
    return allocate(&d_pools[level]);
}

inline
void SkipList_PoolManager::deallocate(void *node)
{
    int level = reinterpret_cast<Node *>(node)->d_level;
    deallocate(&d_pools[level], node);
}

                          // =======================
                          // class SkipList_PoolUtil
                          // =======================

void *SkipList_PoolUtil::allocate(PoolManager *poolManager, int level)
{
    return poolManager->allocate(level);
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

void SkipList_PoolUtil::deallocate(PoolManager *poolManager, void *address)
{
    poolManager->deallocate(address);
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
