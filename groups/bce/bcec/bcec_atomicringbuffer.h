// bcec_atomicringbuffer.h                                          -*-C++-*-
#ifndef INCLUDED_ATOMICRINGBUFFER
#define INCLUDED_ATOMICRINGBUFFER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled fixed-size queue of values.
//
//@CLASSES:
//  bcec_AtomicRingBuffer: thread-enabled fixed-size queue of 'TYPE' values
//
//@AUTHOR: Eric Winseman (ewinseman), Dave Schumann (dschuman)
//
//@DESCRIPTION: This component implements an efficient, thread-enabled 
// fixed-size queue of values.  This class is ideal for synchronization and
// communication between threads in a producer-consumer model.  Its API is
// largely identical to 'bcec_FixedQueue'.  In comparison with that type, it
// may occupy somewhat more space in memory, but performs faster in
// benchmarks.  
//
///Template Requirements
///---------------------
// 'bcec_AtomicRingBuffer' is a template which stores items of a parameterized
// 'TYPE'.  'TYPE' must supply default and copy constructors and the assignment
//  operator; if the default constructors accept a 'bslma::Allocator*', 
// 'TYPE' must declare the Uses Allocator trait (see 'bslalg_TypeTraits') so
//  that the allocator of the queue is propagated to the elements.
//
///Exception safety
///----------------
// AtomicRingBuffer provides a basic exception safety guarantee. It is 
// generally exception neutral; however, if an exception is thrown by the 
// copy constructor of the contained type during the invocation of 'pushBack'
// or 'tryPushBack', the queue is left in a valid but unspecified state. 

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_SEMAPHORE
#include <bcemt_semaphore.h>
#endif

#ifndef INCLUDED_BCEMT_THREADUTIL
#include <bcemt_threadutil.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDES_BITUTIL
#include <bdes_bitutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

namespace BloombergLP {

class bcec_AtomicRingBuffer_Impl {
    // This component-private class holds type-independent data and methods
    // used by bcec_AtomicRingBuffer. 
    enum {
        INDEX_STATE_OLD      = 0,
        INDEX_STATE_WRITING  = 1,
        INDEX_STATE_NEW      = 2,
        INDEX_STATE_READING  = 3,
        INDEX_STATE_MASK     = 0x3,
                               // roundUpToBinaryPower(<max state value>) - 1
        INDEX_STATE_SHIFT    = 0x2, 
                               // base2Log(INDEX_STATE_MASK)
        MAX_OP_INDEX = (1 << (8 * sizeof(int) - 2)) - 1,
        DISABLED_STATE_MASK   = MAX_OP_INDEX + 1,

        // We'll pad certain data members so they're stored on different
        // cache lines to prevent false sharing.   See bcec_fixedqueue
        // for a discussion of these constants.  This value affects 
        // performance only and not correctness. 
#ifdef BSLS_PLATFORM__CPU_POWERPC
        BCEC_PAD = 124
#else
        BCEC_PAD = 60
#endif
    };

    bces_AtomicInt                    d_pushIndex;
    const char                        d_pushIndexPad[BCEC_PAD]; 
    bces_AtomicInt                    d_popIndex;
    const char                        d_popIndexPad[BCEC_PAD];

    const unsigned int                d_capacity;
    const unsigned int                d_maxGeneration;
    const unsigned int                d_alignmentMod;
    bsl::vector<bces_AtomicInt>       d_states;    // index states

    // CREATORS
    bcec_AtomicRingBuffer_Impl(bsl::size_t       capacity,
                               bslma::Allocator *basicAllocator);
       // Create a new type-independent representation of a thread-safe queue
       // with the specified maximum 'capacity' using the specified
       // 'basicAllocator' to supply memory. 

    // CLASS METHODS
    static unsigned int incrementIndex(unsigned int opCount, 
                                       unsigned int currentIndex);
        // If the specified 'opCount' is less than the maximum allowed value,
        // return 'opCount + 1'; otherwise, i.e., the opCount has rolled over
        // the maximum value, return 'currentIndex + 1'. 

    // MANIPULATORS
    void releaseElement(unsigned int currGeneration, 
                        unsigned int index);
       // Mark the specified 'index' as available in the generation following
       // the specified 'currGeneration'.  
    
    int  acquirePushIndex(unsigned int *generation, 
                          unsigned int *index);
       // Mark the next available index as "writing" and load that index 
       // into the specified 'index'.  Load the current generation count into
       // the specified 'generation'.  Return 0 on success, and a nonzero
       // value if the queue is disabled or full.  

    int  acquirePopIndex(unsigned int *generation, 
                         unsigned int *index);
       // Mark the next occupied index (one having the specified
       // 'currentState' in the current generation) as "reading" and load
       // that index  into the specified 'index'.  Load the current 
       // generation count into the specified 'generation'.  Return 0 on 
       // success, and a nonzero value if the queue is empty.  

    void disable();
       // Mark the queue as disabled.  Future attempts to push into the queue
       // will fail.
    void enable();
       // Mark the queue as enabled.

    // ACCESSORS
    bool isEnabled() const;        
       // Return 'true' if the queue is enabled, and 'false' if it is disabled.
    int length() const;
       // Return the number of items in the queue.  
    int capacity() const; 
       // Return the maximum number of items that may be stored in the queue.

    template <typename TYPE>
    friend class bcec_AtomicRingBuffer;
};
    
template <typename TYPE>
class bcec_AtomicRingBuffer {
    // This class provides a thread-enabled, lock-free, fixed-size queue of
    // values.      
private:

    // PRIVATE TYPES
    class PopGuard {
        bcec_AtomicRingBuffer *d_parent_p;
        int                    d_generation;
        int                    d_index;
        
    public:
        PopGuard(bcec_AtomicRingBuffer *parent, int generation, int index) 
        : d_parent_p(parent), d_generation(generation), d_index(index)
        {}

        ~PopGuard() {
            d_parent_p->d_elements[d_index].~TYPE();
            d_parent_p->d_impl.releaseElement(d_generation, d_index);
            //notify pusher of available element
            if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                          d_parent_p->d_numWaitingPushers)) {
                d_parent_p->d_pushControlSema.post();
            }
        }
    };

    class PushGuard {
        bcec_AtomicRingBuffer *d_parent_p;
        int                    d_index;
        
    public:
        PushGuard(bcec_AtomicRingBuffer *parent, int index) 
        : d_parent_p(parent), d_index(index)
        {}

        void release() {
            d_parent_p = 0;
        }

        ~PushGuard() {
            if (d_parent_p) {
                // This pushing thread currently has the cell at 'index'
                // reserved as WRITING. Pop and discard as many elements
                // as we can until the pop index points at that cell, then
                // mark the cell as OLD; this will leave the queue empty.
                
                unsigned generation, index;
                unsigned stopIndex = d_index == 0 
                    ? d_parent_p->d_impl.capacity()
                    : d_index - 1;
                int poppedItems = 1; // always at least 1 for the current cell
                do {
                    if (0 != d_parent_p->d_impl.acquirePopIndex(&generation, 
                                                                &index)) {
                        break;
                    }
                    d_parent_p->d_elements[index].~TYPE();
                    d_parent_p->d_impl.releaseElement(generation, index);
                    poppedItems++;
                } while (index != stopIndex);
                if (index == stopIndex) {
                    // we couldn't invoke acquirePopIndex to advance the
                    // index to this position, since the cell is reserved in
                    // the WRITING state, so we have to advance the index
                    // as if acquirePopIndex had been invoked.
                    unsigned n = d_parent_p->d_impl.d_popIndex.relaxedLoad();
                    generation = n / d_parent_p->d_impl.capacity();
                    unsigned nextIndex = n - 
                        d_parent_p->d_impl.capacity() * generation;
                    if (nextIndex == d_index) {
                        unsigned x = 
                            bcec_AtomicRingBuffer_Impl::incrementIndex(
                                                                n, nextIndex);
                        d_parent_p->d_impl.d_popIndex.testAndSwap(n, x);
                    }
                }
                    
                d_parent_p->d_impl.releaseElement(generation, d_index);
                int numWakeUps = bsl::min(
                                       poppedItems, 
                                       (int)d_parent_p->d_numWaitingPushers);
                while (numWakeUps--) {
                    // Wake up waiting pushers.
                    d_parent_p->d_pushControlSema.post();
                }
            }
        }
    };


    enum {
        INDEX_STATE_OLD      = bcec_AtomicRingBuffer_Impl::INDEX_STATE_OLD,
        INDEX_STATE_WRITING  = bcec_AtomicRingBuffer_Impl::INDEX_STATE_WRITING,
        INDEX_STATE_NEW      = bcec_AtomicRingBuffer_Impl::INDEX_STATE_NEW,
        INDEX_STATE_MASK     = bcec_AtomicRingBuffer_Impl::INDEX_STATE_MASK,
        INDEX_STATE_SHIFT    = bcec_AtomicRingBuffer_Impl::INDEX_STATE_SHIFT,
        BCEC_PAD             = bcec_AtomicRingBuffer_Impl::BCEC_PAD
    };

    // DATA
    TYPE                             *d_elements;   
    const char                        d_elementsPad[BCEC_PAD]; 
    bcec_AtomicRingBuffer_Impl        d_impl;
    
    bcemt_Semaphore                   d_popControlSema;
    const char                        d_popControlSemaPad[BCEC_PAD];
    bces_AtomicInt                    d_numWaitingPushers; 
    bcemt_Semaphore                   d_pushControlSema;
    bces_AtomicInt                    d_numWaitingPoppers;
    bslma::Allocator                 *d_allocator_p;

    // NOT IMPLEMENTED
    bcec_AtomicRingBuffer(const bcec_AtomicRingBuffer&);
    bcec_AtomicRingBuffer& operator=(const bcec_AtomicRingBuffer&);
public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_AtomicRingBuffer,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_AtomicRingBuffer(bsl::size_t       capacity,
                          bslma::Allocator *basicAllocator = 0);
       // Create a thread-enabled lock-free queue having the specified
       // 'capacity'. Optionally specify a 'basicAllocator' used to supply
       // memory.  If 'basicAllocator' is 0, the currently installed default
       // allocator is used.

    ~bcec_AtomicRingBuffer();
       // Destroy this object.  

    // MANIPULATORS
    int pushBack(const TYPE& object);
       // Append the specified 'object' to the back of this queue.  If 
       // the queue is full, block until the queue is not full or until
       // the queue is disabled.  Return 0 on success, and a nonzero value if
       // the queue is disabled.

    void popFront(TYPE* data);
    TYPE popFront();
       // Remove the element from the from of that queue and load that 
       // element into the specified 'data'.  If the queue is empty, block
       // until it is not empty.

    int tryPopFront(TYPE *data);
       // Remove the element from the front of this queue and load that
       // element into the specified 'data'. If this queue is empty, fail
       // immediately.  Return 0 on success, and a non-zero value
       // otherwise.  

    int tryPushBack(const TYPE &data);
       // Append the specified 'data' to the back of this queue. If the
       // queue is full, or if the queue is disabled, fail immediately.
       // Return 0 on success, and a non-zero value otherwise.
    
    void removeAll();
       // Remove all items from this queue. Note that this operation is not 
       // atomic; if other threads are concurrently pushing items into the 
       // queue the result of length() after this function returns is not 
       // guaranteed to be 0. 

    void disable();
       // Disable this queue.  All subsequent invocations of 'pushBack' or
       // 'tryPushBack' will fail immediately.  If the queue is already
       // disabled, this method has no effect.  

    void enable();
       // Enable queuing.  If the queue is not disabled, this call has no
       // effect.  

    // ACCESSORS
    int size() const;
       // Return the maximum number of elements that may be stored 
       // in this queue.  

    int length() const;
       // Return the number of elements currently in this queue.  

    bool isFull() const;
       // Return 'true' if this queue is full (when the number of elements 
       // currently in this queue equals its capacity), or 'false' otherwise.  

    bool isEmpty() const;
       // Return 'true' if this queue is empty (has no elements), or 'false' 
       // otherwise.  

    bool isEnabled() const;
        // Return 'true' if this queue is enabled, and 'false' otherwise.  Note
        // that the queue is created in the "enabled" state.
};

                        // ===========================
                        // class AtomicRingBuffer_Impl
                        // ===========================

// ACCESSORS
inline
int bcec_AtomicRingBuffer_Impl::capacity() const {
    return d_capacity;
}

                           // ======================
                           // class AtomicRingBuffer
                           // ======================
// CREATORS
template <typename TYPE>
bcec_AtomicRingBuffer<TYPE>::bcec_AtomicRingBuffer(
                                             bsl::size_t       capacity,
                                             bslma::Allocator *basicAllocator)
: d_elements()  
, d_elementsPad()
, d_impl(capacity, basicAllocator)
, d_popControlSema(0)
, d_popControlSemaPad()
, d_numWaitingPushers(0)
, d_pushControlSema(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_elements = (TYPE*)d_allocator_p->allocate(capacity * sizeof(TYPE));
}

template <typename TYPE>
bcec_AtomicRingBuffer<TYPE>::~bcec_AtomicRingBuffer()
{
    removeAll();
    d_allocator_p->deallocate(d_elements);
}

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPushBack(const TYPE& data)
{
    unsigned int generation;
    unsigned int index;
    
    int retval = d_impl.acquirePushIndex(&generation, &index);
    
    if (0 != retval) {
        return retval;
    } 
   
    // save data, mark as new
    // If an exception is thrown by the copy constructor, PushGuard will
    // pop and discard items until reaching this cell, then mark this cell
    // empty (without regard to its current state, which is WRITING (i.e., 
    // reserved). That will leave the queue in a valid empty state.
    PushGuard guard(this, index);
    bslalg::ScalarPrimitives::copyConstruct(&d_elements[index], 
                                            data, 
                                            d_allocator_p);
    guard.release();

    d_impl.d_states[index] = 
        INDEX_STATE_NEW | (generation << INDEX_STATE_SHIFT);
    
    // notify poppers of available data
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPoppers)) {
        d_popControlSema.post();
    }
    
    return 0;
}

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPopFront(TYPE *data)
{
    unsigned int generation;
    unsigned int index;
    int retval = d_impl.acquirePopIndex(&generation, &index);
    
    if (0 != retval) {
        return retval;
    }
    
    // copy data. PopGuard will destroy original object, update the queue, and
    // release a waiting pusher, even if the assignment operator throws.
    PopGuard guard(this, generation, index);
    *data = d_elements[index];
    return 0;
}
    
// MANIPULATORS
template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::pushBack(const TYPE &data)
{
    int retval;
    while (0 != (retval = tryPushBack(data))) {
        if (-2 == retval) {
            return -2;
        }
        
        d_numWaitingPushers.relaxedAdd(1);
        
        if (isFull() && isEnabled()) {
            d_pushControlSema.wait();
        }
        
        d_numWaitingPushers.relaxedAdd(-1);
    }
    
    return 0;
}
    
template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::popFront(TYPE *data)
{
    while(0 != tryPopFront(data)) {
        d_numWaitingPoppers.relaxedAdd(1);

        if (isEmpty()) {
            d_popControlSema.wait();
        }

        d_numWaitingPoppers.relaxedAdd(-1);
    }
}

template <typename TYPE>
TYPE bcec_AtomicRingBuffer<TYPE>::popFront()
{
    unsigned int generation;
    unsigned int index;
    
    while(0 != d_impl.acquirePopIndex(&generation, &index)) {
        d_numWaitingPoppers.relaxedAdd(1);
        
        if (isEmpty()) {
            d_popControlSema.wait();
        }
        
        d_numWaitingPoppers.relaxedAdd(-1);
    }
    
    // copy data. PopGuard will destroy original object, update the queue, and
    // release a waiting pusher, even if the copy constructor throws.
    PopGuard guard(this, generation, index);
    return TYPE(d_elements[index]);
}           
    
template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::removeAll()
{
    const int numItems = length();
    int poppedItems = 0;    
    while (poppedItems++ < numItems) {
        unsigned int index;
        unsigned int generation;
        unsigned int n;

        if (0 != d_impl.acquirePopIndex(&generation, &index)) {
            break;
        }
        d_elements[index].~TYPE();
        d_impl.releaseElement(generation, index);
    }
  
    int numWakeUps = bsl::min(poppedItems, (int) d_numWaitingPushers);
    while (numWakeUps--) {
        // Wake up waiting pushers.
        d_pushControlSema.post();
    }
}

template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::disable()
{
    d_impl.disable();
    
    const int numWaitingPushers = d_numWaitingPushers;
    
    for (int i = 0; i < numWaitingPushers; ++i) {
        d_pushControlSema.post();
    }
} 

template <typename TYPE>
inline
void bcec_AtomicRingBuffer<TYPE>::enable() {
    d_impl.enable();
}

// ACCESSORS    
template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::size() const {
    return d_impl.d_capacity;
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::length() const {
    return d_impl.length();
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isFull() const {
    return (size() <= length());
}
    
template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEmpty() const {
    return (0 >= length());
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEnabled() const {
    return d_impl.isEnabled();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
