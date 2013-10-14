// bcec_atomicringbuffer.cpp     -*-C++-*-

#include <bcec_atomicringbuffer.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_atomicringbuffer_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CREATORS
bcec_AtomicRingBuffer_Impl::bcec_AtomicRingBuffer_Impl(
                                          bsl::size_t       capacity,
                                          bslma::Allocator *basicAllocator)
: d_pushIndex(0)
, d_pushIndexPad()
, d_popIndex(0) 
, d_popIndexPad()
, d_capacity(capacity)
, d_maxGeneration(MAX_OP_INDEX / capacity)
, d_alignmentMod((MAX_OP_INDEX + 1) % capacity)
, d_states(capacity, INDEX_STATE_OLD, basicAllocator)
{
    BSLS_ASSERT_OPT(capacity > 0 && MAX_OP_INDEX > capacity);
}

unsigned int 
bcec_AtomicRingBuffer_Impl::incrementIndex(unsigned int opCount, 
                                           unsigned int currIndex)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(MAX_OP_INDEX == opCount)) {
        return currIndex + 1;
    }

    return opCount + 1;
}

bool bcec_AtomicRingBuffer_Impl::isEnabled() const {
    return (0 == (int(d_pushIndex) & DISABLED_STATE_MASK));
}

int bcec_AtomicRingBuffer_Impl::length() const {
    const unsigned w = d_pushIndex & MAX_OP_INDEX;
    const unsigned r = d_popIndex;
    const int l = (w - r);
    return bsl::max(l, 0);
}

void bcec_AtomicRingBuffer_Impl::enable() {
    for(;;) {
        unsigned int pushIndex = d_pushIndex;
        
        if (0 == (pushIndex & DISABLED_STATE_MASK)) {
            return; // already enabled.
        }
        
        if (pushIndex == d_pushIndex.testAndSwap(pushIndex, 
                                                 pushIndex & MAX_OP_INDEX)) {
            return; 
        }
    }
}

void bcec_AtomicRingBuffer_Impl::disable() {
    for (;;) {
        unsigned int pushIndex = d_pushIndex;
        
        if (0 != (pushIndex & DISABLED_STATE_MASK)) {
            return; // already disabled.
        }
        
        if (pushIndex == 
            d_pushIndex.testAndSwap(pushIndex, 
                                    pushIndex | DISABLED_STATE_MASK)) {
            break; 
        }
    }
}

void
bcec_AtomicRingBuffer_Impl::releaseElement(unsigned int currGeneration, 
                                           unsigned int index)
{
    unsigned int generation = currGeneration + 1;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxGeneration == 
                                              generation)) {
        if (index >= d_alignmentMod) {
            generation = 0;
        }
    }
    else if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxGeneration == 
                                                   currGeneration)) {
        generation = 1;
    }
    d_states[index] = INDEX_STATE_OLD | (generation << INDEX_STATE_SHIFT);
}

int bcec_AtomicRingBuffer_Impl::acquirePushIndex(unsigned int *generation, 
                                                 unsigned int *index)
{
    unsigned int pushIndex = d_pushIndex.relaxedLoad();
    unsigned int savedPushIndex = -1;
    unsigned int n, currIndex, currGeneration;  

    for(;;) {

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(pushIndex & 
                                                  DISABLED_STATE_MASK)) {
            return -2;
        }
        
        n = (pushIndex & MAX_OP_INDEX);
        currGeneration = *generation = n / d_capacity;
        currIndex = *index = n - d_capacity * currGeneration;
        
        const int compare = INDEX_STATE_OLD     | 
            (currGeneration << INDEX_STATE_SHIFT);
        const int swap    = INDEX_STATE_WRITING | 
            (currGeneration << INDEX_STATE_SHIFT);
        const int was     = d_states[currIndex].testAndSwap(compare, swap);
   
        if (compare == was) {
            break; // acquired.
        }    
        
        const int markedGeneration = was >> INDEX_STATE_SHIFT;
        
        if (markedGeneration < currGeneration 
        && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                  0 != currGeneration && 
                                  d_maxGeneration != markedGeneration)) {
            const int state = was & INDEX_STATE_MASK;
            
            switch (state) {
            case INDEX_STATE_READING:
                bcemt_ThreadUtil::yield();
                pushIndex = d_pushIndex.relaxedLoad();
                continue;
            default:
                if (savedPushIndex != pushIndex) {
                    bcemt_ThreadUtil::yield();
                    savedPushIndex = pushIndex;
                    pushIndex = d_pushIndex.relaxedLoad();          
                    continue;
                }
                else {
                    return -1;
                }
            }
        }
    
        unsigned int next = incrementIndex(n, currIndex) & MAX_OP_INDEX;
        
        pushIndex = d_pushIndex.testAndSwap(n, next);
        
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                        pushIndex & DISABLED_STATE_MASK)) {
            
            return -2;
        }
    }
    
    unsigned int next = incrementIndex(n, currIndex);
    
    pushIndex = 
        d_pushIndex.testAndSwap(n, 
                                incrementIndex(n, currIndex) & MAX_OP_INDEX);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                      pushIndex & DISABLED_STATE_MASK)) {

        unsigned int disabled = (pushIndex & MAX_OP_INDEX);
        if(n >= disabled) {
            
            d_states[currIndex] = 
                INDEX_STATE_OLD | (currGeneration << INDEX_STATE_SHIFT);
        
            return -2;
        }
    }
    
    return 0;
}
    
int bcec_AtomicRingBuffer_Impl::acquirePopIndex(unsigned int *generation, 
                                                unsigned int *index)
{
    unsigned int savedPopIndex = -1;
    
    unsigned int n = d_popIndex.relaxedLoad();
    unsigned int currIndex; 
    
    for(;;) {
        unsigned int currGeneration = *generation = n / d_capacity;
        currIndex = *index = n - d_capacity * currGeneration;
        
        const int compare = INDEX_STATE_NEW     | 
            (currGeneration << INDEX_STATE_SHIFT);
        const int swap    = INDEX_STATE_READING | 
            (currGeneration << INDEX_STATE_SHIFT);
        const int was     = d_states[currIndex].testAndSwap(compare, swap);
        
        if (compare == was) {
            break;
        }
        
        const int markedGeneration = was >> INDEX_STATE_SHIFT;
        
        if (currGeneration != markedGeneration) {
            return -1; 
        }
        
        int state(was & INDEX_STATE_MASK);
        
        switch (state) {
        case INDEX_STATE_OLD:
            if (savedPopIndex != n) {
                bcemt_ThreadUtil::yield();
                savedPopIndex = n;
                n = d_popIndex.relaxedLoad();
                continue;
            }
            else {
                return -1;
            }
        case INDEX_STATE_WRITING:
            bcemt_ThreadUtil::yield();
            n = d_popIndex.relaxedLoad();
            continue;
        }
        
        unsigned int next = incrementIndex(n, currIndex);
        unsigned int old  = d_popIndex.testAndSwap(n, next);
        if(n == old) {
            n = next;
        }
        else {
            n = old;
        }
    }
    
    d_popIndex.testAndSwap(n, incrementIndex(n, currIndex));
    
    return 0;
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
