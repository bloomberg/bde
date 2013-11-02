// bcec_atomicringbufferindexmanager.cpp                            -*-C++-*-
#include <bcec_atomicringbufferindexmanager.h>

#include <bcemt_threadutil.h>

#include <bslalg_scalarprimitives.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_atomicringbufferindexmanager_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CREATORS
bcec_AtomicRingBufferIndexManager::bcec_AtomicRingBufferIndexManager(
                                          bsl::size_t       capacity,
                                          bslma::Allocator *basicAllocator)
: d_pushIndex(0)
, d_pushIndexPad()
, d_popIndex(0) 
, d_popIndexPad()
, d_capacity(capacity)
, d_maxGeneration(e_MAX_OP_INDEX / capacity)
, d_rolloverIndex((e_MAX_OP_INDEX + 1) % capacity)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(capacity > 0 && e_MAX_OP_INDEX > capacity);

    d_states = static_cast<bsls::AtomicInt*>(
                d_allocator_p->allocate(sizeof(bsls::AtomicInt) * capacity));

    // Can't invoke ArrayPrimitives::emplace here; AtomicInt doesn't have
    // a copy constructor. What we really want to do is just memset the whole
    // region to 0, but that assumes an AtomicInt implementation. We have to
    // loop over the array and initialize it ourselves.
    for (bsls::AtomicInt *state = d_states; 
         state < d_states + capacity; ++state) {
        bslalg::ScalarPrimitives::construct(
                                         state, 
                                         e_INDEX_STATE_EMPTY,
                                         d_allocator_p); // allocator unused
    }
}

bcec_AtomicRingBufferIndexManager::~bcec_AtomicRingBufferIndexManager()
{
    d_allocator_p->deallocate(d_states);
}

// MANIPULATORS
void bcec_AtomicRingBufferIndexManager::enable() {
    for(;;) {
        bsl::size_t pushIndex = d_pushIndex;
        
        if (0 == (pushIndex & e_DISABLED_STATE_MASK)) {
            return; // already enabled.
        }
        
        if (pushIndex == d_pushIndex.testAndSwap(pushIndex, 
                                                 pushIndex & e_MAX_OP_INDEX)) {
            return; 
        }
    }
}

void bcec_AtomicRingBufferIndexManager::disable() {
    for (;;) {
        bsl::size_t pushIndex = d_pushIndex;
        
        if (0 != (pushIndex & e_DISABLED_STATE_MASK)) {
            return; // already disabled.
        }
        
        if (pushIndex == 
            d_pushIndex.testAndSwap(pushIndex, 
                                    pushIndex | e_DISABLED_STATE_MASK)) {
            break; 
        }
    }
}

void
bcec_AtomicRingBufferIndexManager::releaseElement(bsl::size_t currGeneration, 
                                                  bsl::size_t index)
{
    bsl::size_t generation = currGeneration + 1;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxGeneration == 
                                              generation)) {
        if (index >= d_rolloverIndex) {
            generation = 0;
        }
    }
    else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxGeneration == 
                                                   currGeneration)) {
        generation = 1;
    }
    d_states[index] = e_INDEX_STATE_EMPTY | 
        (generation << e_INDEX_STATE_SHIFT);
}

int bcec_AtomicRingBufferIndexManager::acquirePushIndex(
                                                   bsl::size_t *generation, 
                                                   bsl::size_t *index)
{
    bsl::size_t pushIndex = d_pushIndex.loadRelaxed();
    bsl::size_t savedPushIndex = -1;
    bsl::size_t opIndex, currIndex, currGeneration;  

    for(;;) {

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pushIndex & 
                                                  e_DISABLED_STATE_MASK)) {
            return -2;
        }
        
        opIndex = pushIndex & e_MAX_OP_INDEX;
        currGeneration = *generation = opIndex / d_capacity;
        currIndex = *index = opIndex - d_capacity * currGeneration;
        
        const int compare = e_INDEX_STATE_EMPTY     | 
            (currGeneration << e_INDEX_STATE_SHIFT);
        const int swap    = e_INDEX_STATE_WRITING | 
            (currGeneration << e_INDEX_STATE_SHIFT);
        const int was     = d_states[currIndex].testAndSwap(compare, swap);
   
        if (compare == was) {
            break; // acquired.
        }    
        
        const int markedGeneration = was >> e_INDEX_STATE_SHIFT;
        
        if (markedGeneration < currGeneration 
        && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  0 != currGeneration && 
                                  d_maxGeneration != markedGeneration)) {
            const int state = was & e_INDEX_STATE_MASK;
            
            switch (state) {
            case e_INDEX_STATE_READING:
                bcemt_ThreadUtil::yield();
                pushIndex = d_pushIndex.loadRelaxed();
                continue;
            default:
                if (savedPushIndex != pushIndex) {
                    bcemt_ThreadUtil::yield();
                    savedPushIndex = pushIndex;
                    pushIndex = d_pushIndex.loadRelaxed();          
                    continue;
                }
                else {
                    return -1;
                }
            }
        }
    
        bsl::size_t next = incrementIndex(opIndex, currIndex) & e_MAX_OP_INDEX;
        
        pushIndex = d_pushIndex.testAndSwap(opIndex, next);
        
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pushIndex & 
                                                  e_DISABLED_STATE_MASK)) {
            
            return -2;
        }
    }
    
    bsl::size_t next = incrementIndex(opIndex, currIndex);
    
    pushIndex = 
        d_pushIndex.testAndSwap(opIndex, 
                                incrementIndex(opIndex, currIndex) & 
                                e_MAX_OP_INDEX);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                      pushIndex & e_DISABLED_STATE_MASK)) {
        bsl::size_t disabled = (pushIndex & e_MAX_OP_INDEX);
        if(opIndex >= disabled) {
            
            d_states[currIndex] = 
                e_INDEX_STATE_EMPTY | (currGeneration << e_INDEX_STATE_SHIFT);
        
            return -2;
        }
    }
    
    return 0;
}
    
int bcec_AtomicRingBufferIndexManager::acquirePopIndex(
                                                bsl::size_t *generation, 
                                                bsl::size_t *index)
{
    bsl::size_t savedPopIndex = -1;
    
    bsl::size_t opIndex = d_popIndex.loadRelaxed();
    bsl::size_t currIndex; 
    
    for(;;) {
        bsl::size_t currGeneration = *generation = opIndex / d_capacity;
        currIndex = *index = opIndex - d_capacity * currGeneration;
        
        const int compare = e_INDEX_STATE_FULL     | 
            (currGeneration << e_INDEX_STATE_SHIFT);
        const int swap    = e_INDEX_STATE_READING | 
            (currGeneration << e_INDEX_STATE_SHIFT);
        const int was     = d_states[currIndex].testAndSwap(compare, swap);
        
        if (compare == was) {
            break;
        }
        
        const int markedGeneration = was >> e_INDEX_STATE_SHIFT;
        
        if (currGeneration != markedGeneration) {
            return -1; 
        }
        
        int state = was & e_INDEX_STATE_MASK;
        
        switch (state) {
        case e_INDEX_STATE_EMPTY:
            if (savedPopIndex != opIndex) {
                bcemt_ThreadUtil::yield();
                savedPopIndex = opIndex;
                opIndex = d_popIndex.loadRelaxed();
                continue;
            }
            else {
                return -1;
            }
        case e_INDEX_STATE_WRITING:
            bcemt_ThreadUtil::yield();
            opIndex = d_popIndex.loadRelaxed();
            continue;
        }
        
        bsl::size_t next = incrementIndex(opIndex, currIndex);
        bsl::size_t old  = d_popIndex.testAndSwap(opIndex, next);
        opIndex = (opIndex == old) 
            ? next 
            : old;
    }
    
    d_popIndex.testAndSwap(opIndex, incrementIndex(opIndex, currIndex));
    
    return 0;
}

// ACCESSORS
bsl::size_t bcec_AtomicRingBufferIndexManager::length() const {
    const bsl::size_t w = d_pushIndex.load() & e_MAX_OP_INDEX;
    const bsl::size_t r = d_popIndex;
    return (w > r) 
        ? w - r 
        : 0;
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

