// bcec_atomicringbuffer.h                                            -*-C++-*-
#ifndef INCLUDED_BCEC_ATOMICRINGBUFFER
#define INCLUDED_BCEC_ATOMICRINGBUFFER

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
// communication between threads in a producer-consumer model.
//
// The queue provides 'pushBack' and 'popFront' methods for pushing data into
// the queue and popping it from the queue.  In case of overflow (queue full
// when pushing), or underflow (queue empty when popping), the methods block
// until data or free space in the queue appears.  Non-blocking methods
// 'tryPushBack' and 'tryPushFront' are also provided, which fail immediately
// returning a non-zero value in case of overflow or underflow.
//
// The queue may be placed into a "disabled" state using the 'disable' method.
// When disabled, 'pushBack' and 'tryPushBack' fail immediately (they do not
// block).  The queue may be restored to normal operation with the 'enable'
// method.
//
// Unlike 'bcec_Queue', a fixed queue is not double-ended, there is no timed
// API like 'timedPushBack' and 'timedPopFront', and no 'forcePush' methods, as
// the queue capacity is fixed.  Also, this component is not based on
// 'bdec_Queue', so there is no API for direct access to the underlying queue.
// These limitations are a trade-off for significant gain in performance
// compared to 'bcec_Queue'.
//
///Template Requirements
///---------------------
// 'bcec_AtomicRingBuffer' is a template that is parameterized on the type of
// element contained within the queue.  The supplied template argument, 'TYPE',
// must provide both a default constructor and a copy constructors as well as
// an assignment operator.  If the default constructor accepts a
// 'bslma::Allocator*', 'TYPE' must declare the uses 'bslma::Allocator' trait
// (see 'bslma_usesbslmaallocator') so that the allocator of the queue is
// propagated to the elements contained in the queue.
//
///Exception safety
///----------------
// A 'bcec_AtomicRingBuffer' is exception neutral, and all of the methods of
// 'bcec_AtomicRingBuffer' provide the strong exception safety guarantee
// except for 'pushBack' and 'tryPushBack', which provide the basic exception
// guarantee (see 'bsldoc_glossary').
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Thread Pool
///- - - - - - - - - - - - - - - -
// In the following example a 'bcec_AtomicRingBuffer' is used to communicate
// between a single "producer" thread and multiple "consumer" threads.  The
// "producer" will push work requests onto the queue, and each "consumer" will
// iteratively take a work request from the queue and service the request.
// This example shows a partial, simplified implementation of the
// 'bcep_FixedThreadPool' class.  See component 'bcep_fixedthreadpool' for more
// information.
//
// First, we define a utility classes that handles a simple "work item":
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
// void myDoWork(my_WorkData& )
// {
//     // do some stuff...
// }
//..
// Then, we define a 'myConsumer' function that will pop elements off the
// queue and process them.  Note that the call to 'queue->popFront()' will
// block until there is an element available on the queue.  This function will
// be executed in multiple threads, so that each thread waits in
// 'queue->popFront()', and 'bcec_AtomicRingBuffer' guarantees that each thread
// gets a unique element from the queue:
//..
// void myConsumer(bcec_AtomicRingBuffer<my_WorkRequest> *queue)
// {
//     while (1) {
//         // 'popFront()' will wait for a 'my_WorkRequest' until available.
//
//         my_WorkRequest item = queue->popFront();
//         if (item.d_type == my_WorkRequest::STOP) { break; }
//         myDoWork(item.d_data);
//     }
// }
//..
// Finally, we define a 'myProducer' function that serves multiple roles: it
// creates the 'bcec_AtomicRingBuffer', starts the consumer threads, and then
// produces and enqueues work items.  When work requests are exhausted, this
// function enqueues one 'STOP' item for each consumer queue.  This 'STOP'
// item indicates to the consumer thread to terminate its thread-handling
// function.
//
// Note that, although the producer cannot control which thread 'pop's a
// particular work item, it can rely on the knowledge that each consumer thread
// will read a single 'STOP' item and then terminate.
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

                        // ===========================
                        // class bcec_AtomicRingBuffer
                        // ===========================

template <typename TYPE>
class bcec_AtomicRingBuffer {
    // This class provides a thread-enabled, lock-free, fixed-size queue of
    // values.

  private:

    // PRIVATE CONSTANTS
    enum {
        e_PADDING = bces_Platform::e_CACHE_LINE_SIZE - sizeof(bsls::AtomicInt)
    };


    // DATA
    TYPE             *d_elements;          // array of elements that comprise
                                           // the fixed queue (array elements
                                           // are manually constructed and 
                                           // destroyed, and empty elements
                                           // hold uninitialized memory)

    const char        d_elementsPad[e_PADDING];
                                           // padding to prevent false sharing
    bcec_AtomicRingBufferIndexManager
                      d_impl;              // index manager for managing the
                                           // state of 'd_elements'

    bsls::AtomicInt   d_numWaitingPoppers; // number of threads waiting on
                                           // 'd_popControlSema' to pop an
                                           // element

    bcemt_Semaphore   d_popControlSema;    // semaphore on which threads
                                           // waiting to pop 'wait'

    const char        d_popControlSemaPad[e_PADDING];
                                           // padding to prevent false sharing

    bsls::AtomicInt   d_numWaitingPushers; // number of threads waiting on
                                           // 'd_pushControlSema' to push an
                                           // element

    bcemt_Semaphore   d_pushControlSema;   // semaphore on which threads
                                           // waiting to push 'wait'

    const char        d_pushControlSemaPad[e_PADDING];
                                           // padding to prevent false sharing

    bslma::Allocator *d_allocator_p;       // allocator, held not owned

  private:
    // NOT IMPLEMENTED
    bcec_AtomicRingBuffer(const bcec_AtomicRingBuffer&);
    bcec_AtomicRingBuffer& operator=(const bcec_AtomicRingBuffer&);


    // FRIENDS
    template <class VAL> friend class bcec_AtomicRingBuffer_PushProctor;
    template <class VAL> friend class bcec_AtomicRingBuffer_PopGuard;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(bcec_AtomicRingBuffer,
                                   bslma::UsesBslmaAllocator);
    // CREATORS
    explicit
    bcec_AtomicRingBuffer(bsl::size_t       capacity,
                          bslma::Allocator *basicAllocator = 0);
        // Create a thread-enabled lock-free queue having the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bcec_AtomicRingBuffer();
        // Destroy this object.

    // MANIPULATORS
    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue, blocking
        // until space is available if necessary.  Return 0 on success, and a
        // nonzero value if the queue is disabled.

    int tryPushBack(const TYPE &value);
        // Attempt to append the specified 'value' to the back of this
        // queue without blocking.  Return 0 on success, and a non-zero value
        // if the queue is full or disabled.

    void popFront(TYPE* value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.

    TYPE popFront();
        // Remove the element from the front of this queue and return it's
        // value.  If the queue is empty, block until it is not empty.

    int tryPopFront(TYPE *value);
        // Attempt to remove the element from the front of this queue without
        // blocking, and, if successful, load the specified 'value' with the
        // removed element.  Return 0 on success, and a non-zero value if queue
        // was empty.  On failure, 'value' is not changed.

    void removeAll();
        // Remove all items from this queue.  Note that this operation is not
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

    int numElements() const;
        // Returns the number of elements currently in this queue.

    int length() const;
        // [!DEPRECATED!] Invoke 'numElements'.

    int size() const;
        // [!DEPRECATED!] Invoke 'capacity'.

};

                        // ====================================
                        // class bcec_AtomicRingBuffer_PopGuard
                        // ====================================

template <class VALUE>
class bcec_AtomicRingBuffer_PopGuard {
    // This class provides a guard that, upon its destruction, will remove
    // (pop) the indicated element from the 'bcec_AtomicRingBuffer' object
    // supplied at construction.  Note that this guard is used to provide
    // exception safety when popping an element from a 'bcec_AtomicRingBuffer'
    // object.

    // DATA
    bcec_AtomicRingBuffer<VALUE> *d_parent_p;
                                     // object from which an element will be
                                     // popped

    unsigned int                  d_generation;
                                     // generation count of cell being popped

    unsigned int                  d_index;
                                     // index of cell being popped

  private:
    // NOT IMPLEMENTED
    bcec_AtomicRingBuffer_PopGuard(const bcec_AtomicRingBuffer_PopGuard&);
    bcec_AtomicRingBuffer_PopGuard& operator=(
                                        const bcec_AtomicRingBuffer_PopGuard&);
  public:

    // CREATORS
    bcec_AtomicRingBuffer_PopGuard(bcec_AtomicRingBuffer<VALUE> *queue,
                                   unsigned int                  generation,
                                   unsigned int                  index);
        // Create a guard that, upon its destruction, will update the state
        // of the specified 'queue' to remove (pop) the element at the
        // specified 'index' having the specified 'generation', and destroy
        // that popped object.  The behavior is undefined unless 'index' and
        // 'generation' refer to a valid element in 'queue' that the current
        // thread has acquired a reservation to pop (using
        // 'bcec_AtomicRingBufferIndexManager::reservePopIndex').

    ~bcec_AtomicRingBuffer_PopGuard();
        // Update the state of the 'bcec_AtomicRingBuffer' object supplied at
        // construction to remove (pop) the indicated element, and destroy the
        // popped object.
};

                        // =======================================
                        // class bcec_AtomicRingBuffer_PushProctor
                        // =======================================

template <class VALUE>
class bcec_AtomicRingBuffer_PushProctor {
    // This class provides a proctor that, unless the 'release' method has
    // been previously invoked, will remove and destroy all the elements from a
    // 'bcec_AtomicRingBuffer' object supplied at construction (putting that
    // ring-buffer into a valid empty state) upon the proctor's destruction.
    // Note that this guard is used to provide exception safety when pushing an
    // element into a 'bcec_AtomicRingBuffer'.

    // DATA
    bcec_AtomicRingBuffer<VALUE> *d_parent_p;
                                     // object in which an element was pushed

    unsigned int                  d_generation;
                                     // generation of cell being pushed when an
                                     // exception was thrown


    unsigned int                  d_index;
                                     // index of cell being pushed when an
                                     // exception was thrown

  private:
    // NOT IMPLEMENTED
    bcec_AtomicRingBuffer_PushProctor(
                                     const bcec_AtomicRingBuffer_PushProctor&);
    bcec_AtomicRingBuffer_PushProctor& operator=(
                                     const bcec_AtomicRingBuffer_PushProctor&);

  public:

    // CREATORS
    bcec_AtomicRingBuffer_PushProctor(bcec_AtomicRingBuffer<VALUE> *queue,
                                      unsigned int                  generation,
                                      unsigned int                  index);
        // Create a proctor that manages the specified 'queue' and, unless
        // 'release' is called, will remove and destroy all the elements from
        // 'queue' starting at the specified 'index' in the specified
        // 'generation'.  The behavior is undefined unless 'index' and
        // 'generation' refers to a valid element in 'queue'.

    ~bcec_AtomicRingBuffer_PushProctor();
        // Destroy this proctor and, if 'release' was not called on this
        // object, remove and destroy all the elements from the
        // 'bcec_AtomicRingBuffer' object supplied at construction.

    // MANIPULATORS
    void release();
        // Release from management the 'bcec_AtomicRingBuffer' object supplied
        // at construction.

};


// =====================================================================
//                        INLINE FUNCTION DEFINITIONS
// =====================================================================

// See the .cpp for an implementatin note.

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
    unsigned int generation;
    unsigned int index;

    // SYNCHRONIZATION POINT 1
    // The following call to 'reservePushIndex' writes
    // 'bcec_AtomicRingBufferIndexManaged::d_pushIndex' with full sequential
    // consistency, which guarantees the subsequent (relaxed) read from
    // 'd_numWaitingPoppers' sees any waiting pointers from SYNCHRONIZATION
    // POINT 1-Prime.

    int retval = d_impl.reservePushIndex(&generation, &index);

    if (0 != retval) {
        return retval;                                                // RETURN
    }

    // Copy the element into the cell.  If an exception is thrown by the copy
    // constructor, PushProctor will pop and discard items until reaching this
    // cell, then mark this cell empty (without regard to its current state,
    // which is WRITING (i.e., reserved).  That will leave the queue in a valid
    // empty state.

    bcec_AtomicRingBuffer_PushProctor<TYPE> guard(this, generation, index);
    bslalg::ScalarPrimitives::copyConstruct(&d_elements[index],
                                            data,
                                            d_allocator_p);
    guard.release();
    d_impl.commitPushIndex(generation, index);


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

    // SYNCHRONIZATION POINT 2
    // The following call to 'reservePopIndex' writes
    // 'bcec_AtomicRingBufferIndexManaged::d_popIndex' with full sequential
    // consistency, which guarantees the subsequent (relaxed) read from
    // 'd_numWaitingPoppers' sees any waiting pointers from SYNCHRONIZATION
    // POINT 2-Prime.

    int retval = d_impl.reservePopIndex(&generation, &index);

    if (0 != retval) {
        return retval;                                                // RETURN
    }

    // Copy the element.  'bcec_AtomicRingBuffer_PopGuard' will destroy
    // original object, update the queue, and release a waiting pusher, even if
    // the assignment operator throws.

    bcec_AtomicRingBuffer_PopGuard<TYPE> guard(this, generation, index);
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
            // The queue is disabled.

            return retval;                                            // RETURN
        }

        d_numWaitingPushers.addRelaxed(1);

        // SYNCHRONIZATION POINT 1-Prime
        // The following call to 'isFull' loads
        // 'bcec_AtomicRingBufferIndexManager::d_pushIndex' with full
        // sequential consistency, which is required to ensure the visibility
        // of the preceding change to 'd_numWaitingPushers' to SYNCHRONIZATION
        // POINT 2.

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
    while (0 != tryPopFront(data)) {
        d_numWaitingPoppers.addRelaxed(1);

        // SYNCHRONIZATION POINT 2-Prime
        // The following call to 'isEmpty' loads
        // 'bcec_AtomicRingBufferIndexManager::d_pushIndex' with full
        // sequential consistency, which is required to ensure the visibility
        // of the preceding change to 'd_numWaitingPushers' to SYNCHRONIZATION
        // POINT 2.

        if (isEmpty()) {
            d_popControlSema.wait();
        }

        d_numWaitingPoppers.addRelaxed(-1);
    }
}

template <typename TYPE>
TYPE bcec_AtomicRingBuffer<TYPE>::popFront()
{
    unsigned int generation;
    unsigned int index;

    while (0 != d_impl.reservePopIndex(&generation, &index)) {
        d_numWaitingPoppers.addRelaxed(1);

        if (isEmpty()) {
            d_popControlSema.wait();
        }

        d_numWaitingPoppers.addRelaxed(-1);
    }

    // Copy the element.  'bcec_AtomicRingBuffer_PopGuard' will destroy
    // original object, update the queue, and release a waiting pusher, even if
    // the copy constructor throws.

    bcec_AtomicRingBuffer_PopGuard<TYPE> guard(this, generation, index);
    return TYPE(d_elements[index]);
}

template <typename TYPE>
void bcec_AtomicRingBuffer<TYPE>::removeAll()
{
    const int numItems = numElements();
    int poppedItems = 0;
    while (poppedItems++ < numItems) {
        unsigned int index;
        unsigned int generation;

        if (0 != d_impl.reservePopIndex(&generation, &index)) {
            break;
        }

        bslalg::ScalarDestructionPrimitives::destroy(d_elements + index);
        d_impl.commitPopIndex(generation, index);
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
void bcec_AtomicRingBuffer<TYPE>::enable()
{
    d_impl.enable();
}

// ACCESSORS
template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::size() const
{
    return capacity();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::capacity() const
{
    return d_impl.capacity();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::length() const
{
    return numElements();
}

template <typename TYPE>
inline
int bcec_AtomicRingBuffer<TYPE>::numElements() const
{
    return d_impl.length();
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isFull() const
{
    return (capacity() <= numElements());
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEmpty() const
{
    return (0 >= numElements());
}

template <typename TYPE>
inline
bool bcec_AtomicRingBuffer<TYPE>::isEnabled() const
{
    return d_impl.isEnabled();
}

                        // ------------------------------------
                        // class bcec_AtomicRingBuffer_PopGuard
                        // ------------------------------------


// CREATORS
template <class VALUE>
inline
bcec_AtomicRingBuffer_PopGuard<VALUE>::bcec_AtomicRingBuffer_PopGuard(
                                     bcec_AtomicRingBuffer<VALUE> *parent,
                                     unsigned int                  generation,
                                     unsigned int                  index)
: d_parent_p(parent)
, d_generation(generation)
, d_index(index)
{
}

template <class VALUE>
inline
bcec_AtomicRingBuffer_PopGuard<VALUE>::~bcec_AtomicRingBuffer_PopGuard()
{
    // This popping thread currently has the cell at 'd_index' (in
    // 'd_generation') reserved for popping.  Destroy the element at that
    // position and then release the reservation.  Wake up to 1 waiting pusher
    // thread.

    bslalg::ScalarDestructionPrimitives::destroy(
                                             d_parent_p->d_elements + d_index);

    d_parent_p->d_impl.commitPopIndex(d_generation, d_index);

    // Notify pusher of available element.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
            d_parent_p->d_numWaitingPushers)) {
        d_parent_p->d_pushControlSema.post();
    }
}

                        // ---------------------------------------
                        // class bcec_AtomicRingBuffer_PushProctor
                        // ---------------------------------------

// CREATORS
template <class VALUE>
inline
bcec_AtomicRingBuffer_PushProctor<VALUE>::bcec_AtomicRingBuffer_PushProctor(
                                      bcec_AtomicRingBuffer<VALUE> *parent,
                                      unsigned int                  generation,
                                      unsigned int                  index)
: d_parent_p(parent)
, d_generation(generation)
, d_index(index)
{
}

template <class VALUE>
inline
bcec_AtomicRingBuffer_PushProctor<VALUE>::~bcec_AtomicRingBuffer_PushProctor()
{
    if (d_parent_p) {
        // This pushing thread currently has the cell at 'd_index' reserved as
        // 'e_WRITING'.  Dispose of all the elements up to 'd_index'.

        unsigned int generation, index;

        // We will always have at least 1 popped item for the cell reserved
        // for writing by the current thread.

        int poppedItems = 1;
        while (0 == d_parent_p->d_impl.reservePopIndexForClear(&generation,
                                                               &index,
                                                               d_generation,
                                                               d_index)) {
            bslalg::ScalarDestructionPrimitives::destroy(
                                              d_parent_p->d_elements + index);
            ++poppedItems;

            d_parent_p->d_impl.commitPopIndex(generation, index);
        }

        // Release the currently held pop index.

        d_parent_p->d_impl.abortPushIndexReservation(d_generation, d_index);

        while (poppedItems--) {
            // Wake up waiting pushers.

            d_parent_p->d_pushControlSema.post();
        }
    }
}

// MANIPULATORS
template <class VALUE>
inline
void bcec_AtomicRingBuffer_PushProctor<VALUE>::release()
{
    d_parent_p = 0;
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
