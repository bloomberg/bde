// bcec_skiplist.cpp             -*-C++-*-
#include <bcec_skiplist.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_skiplist_cpp,"$Id$ $CSID$")

#include <bdeu_random.h>
#include <bslma_allocator.h>
#include <bdema_infrequentdeleteblocklist.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace {
    enum {
          LEVEL_NUM_BITS = 5
        , LEVEL_MASK = (1 << LEVEL_NUM_BITS) - 1

        , REF_COUNT_NUM_BITS =
                             bcec_SkipList_ControlWord::BCEC_NUM_REFERENCE_BITS
        , REF_COUNT_OFFSET = LEVEL_NUM_BITS
        , REF_COUNT_INC = 1 << REF_COUNT_OFFSET
        , MAX_REF_COUNT = (1 << REF_COUNT_NUM_BITS) - 1
        , REF_COUNT_MASK = MAX_REF_COUNT << REF_COUNT_OFFSET

        , RELEASE_FLAG_NUM_BITS = 1
        , RELEASE_FLAG_OFFSET = REF_COUNT_OFFSET + REF_COUNT_NUM_BITS
        , RELEASE_FLAG_MASK = 1 << RELEASE_FLAG_OFFSET

        , ACQUIRE_COUNT_NUM_BITS = 7
        , ACQUIRE_COUNT_OFFSET = RELEASE_FLAG_OFFSET + RELEASE_FLAG_NUM_BITS
        , ACQUIRE_COUNT_INC = 1 << ACQUIRE_COUNT_OFFSET
        , MAX_ACQUIRE_COUNT = (1 << REF_COUNT_NUM_BITS) - 1
        , ACQUIRE_COUNT_MASK = MAX_ACQUIRE_COUNT << ACQUIRE_COUNT_OFFSET

        , RESERVED_NUM_BITS = 32 - ACQUIRE_COUNT_NUM_BITS -
                                   RELEASE_FLAG_NUM_BITS -
                                   REF_COUNT_NUM_BITS -
                                   LEVEL_NUM_BITS
    };
}

                             // ===============================
                             // class bcec_SkipList_ControlWord
                             // ===============================

void bcec_SkipList_ControlWord::init(int level)
{
    new (&d_cw) bces_AtomicInt(level);
}

int bcec_SkipList_ControlWord::level() const
{
    return d_cw & LEVEL_MASK;
}

int bcec_SkipList_ControlWord::incrementRefCount()
{
    int oldBits = d_cw;
    BSLS_ASSERT((oldBits & REF_COUNT_MASK) != REF_COUNT_MASK);
    int newBits = oldBits + REF_COUNT_INC;
    int result;

    while (oldBits != (result = d_cw.testAndSwap(oldBits, newBits))) {
        oldBits = result;
        BSLS_ASSERT((oldBits & REF_COUNT_MASK) != REF_COUNT_MASK);
        newBits = oldBits + REF_COUNT_INC;
    }

    return (newBits & REF_COUNT_MASK) >> REF_COUNT_OFFSET;
}

int bcec_SkipList_ControlWord::decrementRefCount()
{
    int oldBits = d_cw;
    BSLS_ASSERT(oldBits & REF_COUNT_MASK);
    int newBits = oldBits - REF_COUNT_INC;
    int result;

    while (oldBits != (result = d_cw.testAndSwap(oldBits, newBits))) {
        oldBits = result;
        BSLS_ASSERT(oldBits & REF_COUNT_MASK);
        newBits = oldBits - REF_COUNT_INC;
    }

    return (newBits & REF_COUNT_MASK) >> REF_COUNT_OFFSET;
}

                     // ========================================
                     // class bcec_SkipList_RandomLevelGenerator
                     // ========================================
bcec_SkipList_RandomLevelGenerator::bcec_SkipList_RandomLevelGenerator()
: d_seed(BCEC_SEED), d_randomBits(1)
{
}

int bcec_SkipList_RandomLevelGenerator::randomLevel()
{
    // This routine is "thread-safe enough".

    int randomBits = d_randomBits.relaxedLoad();

    int level = 0;
    int b;

    do {
        if (1 == randomBits) {
            // Only the sentinel bit left.  Regenerate.

            int seed = d_seed.relaxedLoad();
            randomBits = bdeu_Random::generate15(&seed);
            d_seed.relaxedStore(seed);
            BSLS_ASSERT((randomBits >> 15) == 0);
            randomBits |= (1 << 14); // Set the sentinel bit.
        }

        b = randomBits&3;
        level += !b;
        randomBits >>= 2;

    } while (!b);

    d_randomBits.relaxedStore(randomBits);

    return level > BCEC_MAX_LEVEL ? BCEC_MAX_LEVEL : level;
}

                             // ============================
                             // class bcec_SkipList_PoolNode
                             // ============================
struct bcec_SkipList_PoolNode
{
    typedef bcec_SkipList_ControlWord     Control;
    typedef bcec_SkipList_PoolNode        Node;

    Control         d_control; // must be first!
    Node *volatile  d_next_p;
};

                             // ========================
                             // class bcec_SkipList_Pool
                             // ========================
struct bcec_SkipList_Pool
{
    typedef bcec_SkipList_PoolNode Node;

    bces_AtomicPointer<Node>  d_freeList;
    int                       d_objectSize;
    int                       d_numObjects;
    int                       d_level;
};

                             // ===============================
                             // class bcec_SkipList_PoolManager
                             // ===============================

class bcec_SkipList_PoolManager
{
    enum {
        MAX_POOLS = 32
      , INITIAL_NUM_OBJECTS = -1  // default 'numObjects' value
      , GROW_FACTOR         =  2  // multiplicative factor to grow
                                  // pool capacity
      , MAX_NUM_OBJECTS     = -32 // minimum 'd_numObjects' value beyond
                                  // which 'd_numObjects' becomes positive
    };

    typedef bcec_SkipList_PoolNode  Node;
    typedef bcec_SkipList_Pool      Pool;

    bdema_InfrequentDeleteBlockList   d_blockList;  // supplies free memory
    bcemt_Mutex                       d_mutex;      // protects the block list

    Pool                              d_pools[MAX_POOLS];

    void initPool(Pool *pool, int level, int objectSize);
    void replenish(Pool *pool);
    void *allocate(Pool *pool);
    void deallocate(Pool *pool, void *node);

  public:
    explicit bcec_SkipList_PoolManager(int             *objectSizes,
                                       int              numPools,
                                       bslma_Allocator *basicAllocator);
    ~bcec_SkipList_PoolManager();

    void *allocate(int level);
    void deallocate(void *node);
};

void bcec_SkipList_PoolManager::replenish(Pool *pool)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

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
    // 'MAX_NUM_OBJECTS' (i.e., |newNumObjects| < |MAX_NUM_OBJECTS|).
    if (numObjects < 0) {
        if (numObjects > MAX_NUM_OBJECTS) {
            pool->d_numObjects *= GROW_FACTOR;
        }
        else {
            pool->d_numObjects = -numObjects;
        }
    }
}

void *bcec_SkipList_PoolManager::allocate(Pool *pool)
{
    Node *p = 0;
    for (;;) {
        p = pool->d_freeList;
        if (!p) {
            replenish(pool);
            continue;
        }

        int controlBits = p->d_control.d_cw.add(ACQUIRE_COUNT_INC);
        if (ACQUIRE_COUNT_INC != (ACQUIRE_COUNT_MASK & controlBits)) {
            for (int i=0; i < 3; ++i) {
                // To avoid unnecessary contention, assume that if we did
                // not get the first reference, then the other thread is
                // about to complete the pop.  Wait for a few cycles until
                // he does.  If he does not complete then go on and try to
                // acquire it ourselves.
                if (pool->d_freeList != p) {
                    break;
                }
            }
        }

        // Make sure that freeList has not changed since we acquired a
        // reference, since then it's unsafe to dereference 'p' (note
        // that therefore *both* of these 'testAndSwap's are required).

        if (pool->d_freeList.testAndSwap(0,0) == p
         && pool->d_freeList.testAndSwap(p,p->d_next_p) == p) {
            return p;
        }
        else {
            int controlBits;
            for (;;) {
                controlBits = p->d_control.d_cw;
                if ( controlBits & RELEASE_FLAG_MASK ) {
                    if (controlBits == p->d_control.d_cw.testAndSwap(
                                controlBits,
                                controlBits^RELEASE_FLAG_MASK)) {
                        // The node is now free but not on the free list.
                        // Take it.
                        return p;
                    }
                }
                else if (controlBits == p->d_control.d_cw.testAndSwap(
                                controlBits,
                                controlBits - ACQUIRE_COUNT_INC)) {
                    break;
                }
            }
        }
    }

    return p;
}

void bcec_SkipList_PoolManager::deallocate(Pool *pool, void *node)
{
    Node *old;
    Node *p = reinterpret_cast<Node *>(node);

    int controlBits;
    for (;;) {
        controlBits = p->d_control.d_cw;

        if ((ACQUIRE_COUNT_INC == (ACQUIRE_COUNT_MASK & controlBits))) {
            if (controlBits == p->d_control.d_cw.testAndSwap(
                                            controlBits,
                                            controlBits - ACQUIRE_COUNT_INC)) {
                break;
            }
        }
        else if (controlBits == p->d_control.d_cw.testAndSwap(
                    controlBits,
                    (controlBits - ACQUIRE_COUNT_INC) | RELEASE_FLAG_MASK)) {
            // Someone else is still trying to pop this item.  Just let
            // them have it.
            return;
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
void bcec_SkipList_PoolManager::initPool(
    Pool *pool, int level, int objectSize)
{
    pool->d_freeList = 0;
    pool->d_objectSize = objectSize;
    pool->d_numObjects = INITIAL_NUM_OBJECTS;
    pool->d_level = level;
}

bcec_SkipList_PoolManager::bcec_SkipList_PoolManager(
    int             *objectSizes,
    int              numPools,
    bslma_Allocator *basicAllocator)
: d_blockList(basicAllocator)
{
    BSLS_ASSERT(numPools > 0);
    BSLS_ASSERT(numPools <= MAX_POOLS);

    for (int i=0; i<numPools; ++i) {
        initPool(&d_pools[i], i, objectSizes[i]);
    }

}

inline
bcec_SkipList_PoolManager::~bcec_SkipList_PoolManager()
{
}

inline
void *bcec_SkipList_PoolManager::allocate(int level)
{
    return allocate(&d_pools[level]);
}

inline
void bcec_SkipList_PoolManager::deallocate(void *node)
{
    int level = reinterpret_cast<Node *>(node)->d_control.level();
    deallocate(&d_pools[level], node);
}

                             // ============================
                             // class bcec_SkipList_PoolUtil
                             // ============================

void *bcec_SkipList_PoolUtil::allocate(PoolManager *poolManager, int level)
{
    return poolManager->allocate(level);
}

void bcec_SkipList_PoolUtil::deallocate(PoolManager *poolManager, void *node)
{
    poolManager->deallocate(node);
}

bcec_SkipList_PoolManager *bcec_SkipList_PoolUtil::createPoolManager(
    int             *objectSizes,
    int              numPools,
    bslma_Allocator *basicAllocator)
{
    return new (*basicAllocator) PoolManager(objectSizes,
                                             numPools,
                                             basicAllocator);
}

void bcec_SkipList_PoolUtil::deletePoolManager(
    bslma_Allocator *basicAllocator,
    PoolManager     *poolManager)
{
    basicAllocator->deleteObject(poolManager);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
