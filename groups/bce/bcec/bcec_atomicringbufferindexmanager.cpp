// bcec_atomicringbufferindexmanager.cpp                            -*-C++-*-
#include <bcec_atomicringbufferindexmanager.h>

#include <bcemt_threadutil.h>

#include <bslalg_scalarprimitives.h>
#include <bsls_assert.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_atomicringbufferindexmanager_cpp,"$Id$ $CSID$")

namespace BloombergLP {

enum {
    ///State Values
    /// - - - - - -
    // The following constants define the possible index states held in
    // array of state values 'd_states'.  Note that the usage of different
    // bits of the 'd_states' values are described in an implementation note
    // in the .cpp file.
    
    e_INDEX_STATE_EMPTY    = 0,   // cell is empty and available for writing

    e_INDEX_STATE_WRITING  = 1,   // cell is reserved for writing
    
    e_INDEX_STATE_FULL     = 2,   // cell has a value in it
    
    e_INDEX_STATE_READING  = 3,   // cell is reserved for reading
    

    ///State Constants
    ///- - - - - - - -
    // The following constants are used to manipulate the bits of elements
    // in the 'd_states' array.  Note that the usage of different bits of
    // the 'd_states' values are described in an implementation note in
    ///the .cpp file. 
    
    e_INDEX_STATE_MASK     = 0x3, // bitmask used to determine the
                                  // 'e_INDEX_STATE_*' value from an index
                                  // state element

    e_INDEX_STATE_SHIFT    = 0x2, // number of bits to left-shift to make
                                  // room for state value.  
                                  // must be base2Log(INDEX_STATE_MASK)

    ///PushIndex Constants
    ///- - - - - - - - - -
    // The following constants are used to manipulate and modify the bits of
    // 'd_pushIndex'.  The bits of 'd_pushIndex' encodes the index of the
    // of the next element to be pushed, as well as the current generation
    // count and a flag indicating whether the queue is disabled.  Note
    // that the encoding of bits in 'd_pushIndex' is described in an
    // implementation note in the .cpp file.

    e_MAX_OP_INDEX = (1 << (8 * sizeof(int) - 1)) - 1,


    e_DISABLED_STATE_MASK  = e_MAX_OP_INDEX + 1,
                                      // bitmask for the disabled bit in
                                      // 'd_pushIndex' 
};


// CLASS METHODS
inline 
bsl::size_t incrementIndex(bsl::size_t opCount, 
                           bsl::size_t currentIndex)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(e_MAX_OP_INDEX == opCount)) {
        return currentIndex + 1;
    }
    return opCount + 1;
}


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
    // region to 0, but that assumes a certain AtomicInt implementation. We 
    // have to loop over the array and initialize it ourselves.
    for (bsls::AtomicInt *state = d_states; 
         state < d_states + capacity; ++state) {
        bslalg::ScalarPrimitives::construct(
                                         state, 
                                         static_cast<int>(e_INDEX_STATE_EMPTY),
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
bcec_AtomicRingBufferIndexManager::releasePopIndex(
                                                  bsl::size_t currGeneration, 
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
            return -1;
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
                    return 1;
                }
            }
        }
    
        bsl::size_t next = incrementIndex(opIndex, currIndex) & e_MAX_OP_INDEX;
        
        pushIndex = d_pushIndex.testAndSwap(opIndex, next);
        
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pushIndex & 
                                                  e_DISABLED_STATE_MASK)) {
            
            return -1;
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
        
            return -1;
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
            return 1; 
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
                return 1;
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

void bcec_AtomicRingBufferIndexManager::incrementPopIndexFrom(
                                                       bsl::size_t index) {
    bsl::size_t opIndex = d_popIndex.loadRelaxed();
    bsl::size_t generation = opIndex / capacity();
    bsl::size_t currentIndex = opIndex - capacity() * generation;
    if (currentIndex == index) {
        bsl::size_t nextIndex = incrementIndex(opIndex, currentIndex);
        d_popIndex.testAndSwap(opIndex, nextIndex);
    }
}

void bcec_AtomicRingBufferIndexManager::releasePushIndex(
                                                    bsl::size_t generation,
                                                    bsl::size_t index) {
    d_states[index] = e_INDEX_STATE_FULL | 
        (generation << e_INDEX_STATE_SHIFT);
}


// ACCESSORS
bsl::size_t bcec_AtomicRingBufferIndexManager::length() const {
    const bsl::size_t w = d_pushIndex.load() & e_MAX_OP_INDEX;
    const bsl::size_t r = d_popIndex;
    return (w > r) 
        ? w - r 
        : 0;
}

bool bcec_AtomicRingBufferIndexManager::isEnabled() const {
    return (0 == (d_pushIndex.load() & e_DISABLED_STATE_MASK));
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

