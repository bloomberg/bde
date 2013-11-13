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
// largely identical to that of 'bcec_FixedQueue'.  In comparison with that
// type, it has a less strict exception safety guarantee, but performs faster
// in benchmarks.  
//
// A ring buffer is a a fixed size buffer that logically wraps around itself.
// It is the ideal container for a fixed sized queue, since this structure
// imposes a strict upper bound on it’s internal capacity.
//
// Here is an illustration representing a ring buffer that can hold at most
// twenty items at any instant.
//
// +---------------------------------------------------------------------+
// | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9| 10| 11| 12| 13| 14| 15| 16| 17| 18| 19|
// +---------------------------------------------------------------------+
//                                 |
//                                 |
//                                 V
//                            +-------+
//                        +---|  9| 10|---+
//                    +---|  8|---+---| 11|---+
//                    |  7|---+       +---| 12|
//                   +----                +------+
//                   |  6|                   | 13|
//                  +----+                   +----+
//                  |  5|                     | 14|
//                  +---+                     +---+
//                  |  4|                     | 15|
//                  +----+                    ----+
//                   |  3|                   | 16|
//	                 +-----               +------+ 
//                    |  2|---+       +---| 17|
//                    +---|  1|---+---| 18|---+
//                        +---|  0| 19|---+
//                            +-------+
//
// ****************************************************************************
// The Atomic Ring Buffer (ARB) is an implementation of a ring buffer that
// allows concurrent access from multiple reader and writer threads.  The
// component was designed to minimize contention between threads.
//
// Conceptually, the ARB could be thought of as two concentric ring buffers.
// Cells of the outer ring hold an atomic integer which facilitates a state 
// machine (sn) whose purpose is to protect access to a value (vn) contained
// at the homogeneous inner cell.
//
//                            +-------+ 
//                        +---| s9|s10|---+   
//                     ---| s8+-------+s11|---
//                  ------|---| v9|v10|---|-------
//                +-------| v8|--- ---|v11|-------+
//                | s7| v7|---+       +---|v12|s12|
//              +---------+               +---------+
//              | s6| v6|                   |v13|s13|
//             +--------+                   +--------+
//             | s5| s5|                     |v14|s14|
//             +-------+                     +-------+
//             | s4| v4|                     |v15|s15|
//             +--------+                    +-------+
//              | s3| v3|                   |v16|s16|
//              +---------                +---------+ 
//                | s2| v2|---        +---|v17|s17|
//                +--- ---| v1|---+---|v18|-------+
//                  ------|---| v0|v19|---|------
//                     ---| s1|-------|s18|---
//                        +---| s0|s19|---+
//                            +-------+
//
// The outer ring implemented in the class bcec_AtomicRingBufferIndexManager.
// The inner ring is implemented in the class bcec_AtomicRingBuffer. Objects
// of this class have an instance of bcec_AtomicRinBufferIndexManager.
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
//
///Usage
///-----
///Example: Simple Thread Pool
///- - - - - - - - - - - - - -
// The following example demonstrates a typical usage of a 
// 'bcec_AtomicRingBuffer'.
//
// This 'bcec_AtomicRingBuffer' is used to communicate between a single
// "producer" thread and multiple "consumer" threads.  The "producer" will
// push work requests onto the queue, and each "consumer" will iteratively 
// take a work request from the queue and service the request.  This example
// shows a partial, simplified implementation of the 'bcep_FixedThreadPool' 
// class.  See component 'bcep_fixedthreadpool' for more information.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
// struct my_WorkData {
//     // Work data...
// };
//
// struct my_WorkRequest {
//     enum RequestType {
//           WORK = 1
//         , STOP = 2
//     };
//
//     RequestType d_type;
//     my_WorkData d_data;
//     // Work data...
// };
//..
// Next, we provide a simple function to service an individual work item.
// The details are unimportant for this example:
//..
// void myDoWork(my_WorkData& data)
// {
//     // do some stuff...
// }
//..
// The 'myConsumer' function will pop elements off the queue and process them.
// As discussed above, note that the call to 'queue->popFront()' will block
// until there is an element available on the queue.  This function will be
// executed in multiple threads, so that each thread waits in
// 'queue->popFront()', and 'bcec_AtomicRingBuffer' guarantees that each
// thread gets a unique element from the queue:
//..
// void myConsumer(bcec_AtomicRingBuffer<my_WorkRequest> *queue)
// {
//     while (1) {
//         // 'popFront()' will wait for a 'my_WorkRequest' until available.
//         my_WorkRequest item = queue->popFront();
//         if (item.d_type == my_WorkRequest::STOP) { break; }
//         myDoWork(item.d_data);
//     }
// }
//..
// In this simple example, the 'myProducer' function serves multiple roles: it
// creates the 'bcec_AtoimcRingBuffer', starts the consumer threads, and then
// produces and enqueues work items.  When work requests are exhausted, this
// function enqueues one 'STOP' item for each consumer queue.
//
// When each consumer thread reads a 'STOP', it terminates its thread-handling
// function.  Note that, although the producer cannot control which thread
// 'pop's a particular work item, it can rely on the knowledge that each
// consumer thread will read a single 'STOP' item and then terminate.
//
//..
// void myProducer(int numThreads)
// {
//     enum {
//       MAX_QUEUE_LENGTH = 100,
//       NUM_WORK_ITEMS = 1000
//     };
//
//     bcec_AtomicRingBuffer<my_WorkRequest> queue(MAX_QUEUE_LENGTH);
//
//     bcemt_ThreadGroup consumerThreads;
//     consumerThreads.addThreads(bdef_BindUtil::bind(&myConsumer, &queue),
//                                numThreads);
//
//     enum {
//        NUM_WORK_ITEMS = 1000
//     }
//  
//     for (int i = 0; i < NUM_WORK_ITEMS; ++i) {
//         my_WorkRequest item;
//         item.d_type = my_WorkRequest::WORK;
//         item.d_data = my_WorkData(); // some stuff to do
//         queue.pushBack(item);
//     }
//
//     for (int i = 0; i < numThreads; ++i) {
//         my_WorkRequest item;
//         item.d_type = my_WorkRequest::STOP;
//         queue.pushBack(item);
//     }
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEC_ATOMICRINGBUFFERINDEXMANAGER
#include <bcec_atomicringbufferindexmanager.h>
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
    
template <typename TYPE>
class bcec_AtomicRingBuffer {
    // This class provides a thread-enabled, lock-free, fixed-size queue of
    // values.      
private:

    // PRIVATE TYPES
    class PopGuard {
        bcec_AtomicRingBuffer *d_parent_p;
        int                    d_generation; 
                                 // of cell being popped when exception thrown
        int                    d_index;
                                 // of cell being popped when exception thrown
       
    public:
        PopGuard(bcec_AtomicRingBuffer *parent, int generation, int index) 
        : d_parent_p(parent), d_generation(generation), d_index(index)
        {}

        ~PopGuard() {
            // This popping thread currently has the cell at 'd_index'
            // (in 'd_generation') reserved for popping.  Destroy the
            // element at that position and then release the reservation.
            // Wake up to 1 waiting pusher thread.
            bslalg::ScalarDestructionPrimitives::destroy(
                                            d_parent_p->d_elements + d_index);
            d_parent_p->d_impl.releasePopReservation(d_generation, d_index);
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
                                 // of cell being popped when exception thrown

        
    public:
        PushGuard(bcec_AtomicRingBuffer *parent, int index) 
        : d_parent_p(parent), d_index(index)
        {}

        void release() {
            d_parent_p = 0;
        }

        ~PushGuard() {
            if (d_parent_p) {
                // This pushing thread currently has the cell at 'd_index'
                // reserved as WRITING. Pop and discard as many elements
                // as we can until the pop index points at that cell, then
                // mark the cell as OLD; this will leave the queue empty.
                
                bsl::size_t generation, index;
                bsl::size_t stopIndex = d_index == 0 
                    ? d_parent_p->d_impl.capacity()
                    : d_index - 1;
                int poppedItems = 1; // always at least 1 for the current cell
                do {
                    if (0 != d_parent_p->d_impl.acquirePopIndex(&generation, 
                                                                &index)) {
                        break;
                    }
                    bslalg::ScalarDestructionPrimitives::destroy(
                                              d_parent_p->d_elements + index);
                    d_parent_p->d_impl.releasePopReservation(generation, 
                                                             index);
                    poppedItems++;
                } while (index != stopIndex);
                
                if (index == stopIndex) {
                    // We incremented the pop index up to d_index, the 
                    // cell that is currently in the WRITING state, but we
                    // cannot advance past it using acquirePopIndex due to 
                    // its state. 
                    d_parent_p->d_impl.incrementPopIndexFrom(d_index);
                }
                    
                d_parent_p->d_impl.releasePopReservation(generation, d_index);
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

    // DATA
    TYPE                             *d_elements;  // element storage
    const char                        d_elementsPad[
                                                  bces_Platform::e_BCEC_PAD]; 
    bcec_AtomicRingBufferIndexManager d_impl;      // state variables
    
    bsls::AtomicInt                   d_numWaitingPoppers;
    bcemt_Semaphore                   d_popControlSema;  
                                                   // pop threads wait on this
                                                   // when the queue is empty
    const char                        d_popControlSemaPad[
                                                  bces_Platform::e_BCEC_PAD];
    bsls::AtomicInt                   d_numWaitingPushers;
                  
    bcemt_Semaphore                   d_pushControlSema;
    const char                        d_pushControlSemaPad[
                                                  bces_Platform::e_BCEC_PAD];
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
       // queue the result of numElements() after this function returns is not 
       // guaranteed to be 0. 

    void disable();
       // Disable this queue.  All subsequent invocations of 'pushBack' or
       // 'tryPushBack' will fail immediately.  If the queue is already
       // disabled, this method has no effect.  

    void enable();
       // Enable queuing.  If the queue is not disabled, this call has no
       // effect.  

    // ACCESSORS
    int capacity() const;
       // Return the maximum number of elements that may be stored 
       // in this queue.

    bool isEmpty() const;
       // Return 'true' if this queue is empty (has no elements), or 'false' 
       // otherwise.  

    bool isEnabled() const;
        // Return 'true' if this queue is enabled, and 'false' otherwise.  Note
        // that the queue is created in the "enabled" state.

    bool isFull() const;
       // Return 'true' if this queue is full (when the number of elements 
       // currently in this queue equals its capacity), or 'false' otherwise.  

    int length() const;
       // *DEPRECATED*.  Invoke 'numElements'. 

    int numElements() const;
       // Returns the number of elements currently in this queue.

    int size() const;
       // *DEPRECATED*.  Invoke 'capacity'.  

};

// =====================================================================
//                        INLINE FUNCTION DEFINITIONS
// =====================================================================

                           // ----------------------
                           // class AtomicRingBuffer
                           // ----------------------
// CREATORS
template <typename TYPE>
bcec_AtomicRingBuffer<TYPE>::bcec_AtomicRingBuffer(
                                             bsl::size_t       capacity,
                                             bslma::Allocator *basicAllocator)
: d_elements()  
, d_elementsPad()
, d_impl(capacity, basicAllocator)
, d_numWaitingPoppers(0)
, d_popControlSema(0)
, d_popControlSemaPad()
, d_numWaitingPushers(0)
, d_pushControlSema(0)
, d_pushControlSemaPad()
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
    bsl::size_t generation;
    bsl::size_t index;
    
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
    d_impl.releasePushReservation(generation, index);
    
    // notify poppers of available data
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPoppers)) {
        d_popControlSema.post();
    }
    
    return 0;
}

template <typename TYPE>
int bcec_AtomicRingBuffer<TYPE>::tryPopFront(TYPE *data)
{
    bsl::size_t generation;
    bsl::size_t index;
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
        if (retval < 0) {
            return retval; // disabled
        }
        
        d_numWaitingPushers.addRelaxed(1);
        
        if (isFull() && isEnabled()) {
            d_pushControlSema.wait();
        }
        
        d_numWaitingPushers.addRelaxed(-1);
    }
    
    return 0;
}
    
template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::popFront(TYPE *data)
{
    while(0 != tryPopFront(data)) {
        d_numWaitingPoppers.addRelaxed(1);

        if (isEmpty()) {
            d_popControlSema.wait();
        }

        d_numWaitingPoppers.addRelaxed(-1);
    }
}

template <typename TYPE>
TYPE bcec_AtomicRingBuffer<TYPE>::popFront()
{
    bsl::size_t generation;
    bsl::size_t index;
    
    while(0 != d_impl.acquirePopIndex(&generation, &index)) {
        d_numWaitingPoppers.addRelaxed(1);
        
        if (isEmpty()) {
            d_popControlSema.wait();
        }
        
        d_numWaitingPoppers.addRelaxed(-1);
    }
    
    // copy data. PopGuard will destroy original object, update the queue, and
    // release a waiting pusher, even if the copy constructor throws.
    PopGuard guard(this, generation, index);
    return TYPE(d_elements[index]);
}           
    
template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::removeAll()
{
    const int numItems = numElements();
    int poppedItems = 0;    
    while (poppedItems++ < numItems) {
        bsl::size_t index;
        bsl::size_t generation;

        if (0 != d_impl.acquirePopIndex(&generation, &index)) {
            break;
        }
        
        bslalg::ScalarDestructionPrimitives::destroy(d_elements + index);
        d_impl.releasePopReservation(generation, index);
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
    return capacity();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::capacity() const {
    return d_impl.capacity();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::length() const {
    return numElements();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::numElements() const {
    return d_impl.length();
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isFull() const {
    return (capacity() <= numElements());
}
    
template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEmpty() const {
    return (0 >= numElements());
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
