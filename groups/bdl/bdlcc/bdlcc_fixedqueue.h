// bdlcc_fixedqueue.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_FIXEDQUEUE
#define INCLUDED_BDLCC_FIXEDQUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled fixed-size queue of values.
//
//@CLASSES:
//  bdlcc::FixedQueue: thread-enabled fixed-size queue of 'TYPE' values
//
//@DESCRIPTION: This component defines a type, 'bdlcc::FixedQueue', that
// provides an efficient, thread-enabled fixed-size queue of values.  This
// class is ideal for synchronization and communication between threads in a
// producer-consumer model.
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
// block and any blocked invocations will fail immediately).  The queue may be
// restored to normal operation with the 'enable' method.
//
// Unlike 'bdlcc::Queue', a fixed queue is not double-ended, there is no timed
// API like 'timedPushBack' and 'timedPopFront', and no 'forcePush' methods, as
// the queue capacity is fixed.  Also, this component is not based on
// 'bdlc::Queue', so there is no API for direct access to the underlying queue.
// These limitations are a trade-off for significant gain in performance
// compared to 'bdlcc::Queue'.
//
///Template Requirements
///---------------------
// 'bdlcc::FixedQueue' is a template that is parameterized on the type of
// element contained within the queue.  The supplied template argument, 'TYPE',
// must provide both a default constructor and a copy constructors as well as
// an assignment operator.  If the default constructor accepts a
// 'bslma::Allocator*', 'TYPE' must declare the uses 'bslma::Allocator' trait
// (see 'bslma_usesbslmaallocator') so that the allocator of the queue is
// propagated to the elements contained in the queue.
//
///Exception safety
///----------------
// A 'bdlcc::FixedQueue' is exception neutral, and all of the methods of
// 'bdlcc::FixedQueue' provide the strong exception safety guarantee except for
// 'pushBack' and 'tryPushBack', which provide the basic exception guarantee
// (see 'bsldoc_glossary').
//
///Memory Usage
///------------
// 'bdlcc::FixedQueue' is most efficient when dealing with small objects or
// fundamental types (as a thread-safe container, its methods pass objects *by*
// *value*).  We recommend:
//: o Large objects be stored as shared-pointers (or possibly raw pointers).
//: o Clients take care in specifying the queue capacity (specified in a number
//:   of objects, *not* a number of bytes).
//
// Note that the implementation of 'bdlcc::FixedQueue' currently creates a
// fixed size array of the contained object type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Thread Pool
///- - - - - - - - - - - - - - - -
// In the following example a 'bdlcc::FixedQueue' is used to communicate
// between a single "producer" thread and multiple "consumer" threads.  The
// "producer" will push work requests onto the queue, and each "consumer" will
// iteratively take a work request from the queue and service the request.
// This example shows a partial, simplified implementation of the
// 'bdlmt::FixedThreadPool' class.  See component 'bdlmt_fixedthreadpool' for
// more information.
//
// First, we define a utility classes that handles a simple "work item":
//..
//  struct my_WorkData {
//      // Work data...
//  };
//
//  struct my_WorkRequest {
//      enum RequestType {
//          e_WORK = 1,
//          e_STOP = 2
//      };
//
//      RequestType d_type;
//      my_WorkData d_data;
//      // Work data...
//  };
//..
// Next, we provide a simple function to service an individual work item.  The
// details are unimportant for this example:
//..
//  void myDoWork(my_WorkData& data)
//  {
//      // do some stuff...
//      (void)data;
//  }
//..
// Then, we define a 'myConsumer' function that will pop elements off the queue
// and process them.  Note that the call to 'queue->popFront()' will block
// until there is an element available on the queue.  This function will be
// executed in multiple threads, so that each thread waits in
// 'queue->popFront()', and 'bdlcc::FixedQueue' guarantees that each thread
// gets a unique element from the queue:
//..
//  void myConsumer(bdlcc::FixedQueue<my_WorkRequest> *queue)
//  {
//      while (1) {
//          // 'popFront()' will wait for a 'my_WorkRequest' until available.
//
//          my_WorkRequest item = queue->popFront();
//          if (item.d_type == my_WorkRequest::e_STOP) { break; }
//          myDoWork(item.d_data);
//      }
//  }
//..
// Finally, we define a 'myProducer' function that serves multiple roles: it
// creates the 'bdlcc::FixedQueue', starts the consumer threads, and then
// produces and enqueues work items.  When work requests are exhausted, this
// function enqueues one 'e_STOP' item for each consumer queue.  This 'e_STOP'
// item indicates to the consumer thread to terminate its thread-handling
// function.
//
// Note that, although the producer cannot control which thread 'pop's a
// particular work item, it can rely on the knowledge that each consumer thread
// will read a single 'e_STOP' item and then terminate.
//..
//  void myProducer(int numThreads)
//  {
//      enum {
//          k_MAX_QUEUE_LENGTH = 100,
//          k_NUM_WORK_ITEMS   = 1000
//      };
//
//      bdlcc::FixedQueue<my_WorkRequest> queue(k_MAX_QUEUE_LENGTH);
//
//      bslmt::ThreadGroup consumerThreads;
//      consumerThreads.addThreads(bdlf::BindUtil::bind(&myConsumer, &queue),
//                                 numThreads);
//
//      for (int i = 0; i < k_NUM_WORK_ITEMS; ++i) {
//          my_WorkRequest item;
//          item.d_type = my_WorkRequest::e_WORK;
//          item.d_data = my_WorkData(); // some stuff to do
//          queue.pushBack(item);
//      }
//
//      for (int i = 0; i < numThreads; ++i) {
//          my_WorkRequest item;
//          item.d_type = my_WorkRequest::e_STOP;
//          queue.pushBack(item);
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLCC_FIXEDQUEUEINDEXMANAGER
#include <bdlcc_fixedqueueindexmanager.h>
#endif

#ifndef INCLUDED_BSLMT_SEMAPHORE
#include <bslmt_semaphore.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
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

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                              // ================
                              // class FixedQueue
                              // ================

template <class TYPE>
class FixedQueue {
    // This class provides a thread-enabled, lock-free, fixed-size queue of
    // values.

  private:

    // PRIVATE CONSTANTS
    enum {
        k_TYPE_PADDING = bslmt::Platform::e_CACHE_LINE_SIZE - sizeof(TYPE *),
        k_SEMA_PADDING = bslmt::Platform::e_CACHE_LINE_SIZE -
                                                       sizeof(bslmt::Semaphore)
    };

    // DATA
    TYPE             *d_elements;          // array of elements that comprise
                                           // the fixed queue (array elements
                                           // are manually constructed and
                                           // destroyed, and empty elements
                                           // hold uninitialized memory)

    const char        d_elementsPad[k_TYPE_PADDING];
                                           // padding to prevent false sharing
    FixedQueueIndexManager
                      d_impl;              // index manager for managing the
                                           // state of 'd_elements'

    bsls::AtomicInt   d_numWaitingPoppers; // number of threads waiting on
                                           // 'd_popControlSema' to pop an
                                           // element

    bslmt::Semaphore   d_popControlSema;    // semaphore on which threads
                                           // waiting to pop 'wait'

    const char        d_popControlSemaPad[k_SEMA_PADDING];
                                           // padding to prevent false sharing

    bsls::AtomicInt   d_numWaitingPushers; // number of threads waiting on
                                           // 'd_pushControlSema' to push an
                                           // element

    bslmt::Semaphore   d_pushControlSema;   // semaphore on which threads
                                           // waiting to push 'wait'

    const char        d_pushControlSemaPad[k_SEMA_PADDING];
                                           // padding to prevent false sharing

    bslma::Allocator *d_allocator_p;       // allocator, held not owned

  private:
    // NOT IMPLEMENTED
    FixedQueue(const FixedQueue&);
    FixedQueue& operator=(const FixedQueue&);

    // FRIENDS
    template <class VAL> friend class FixedQueue_PushProctor;
    template <class VAL> friend class FixedQueue_PopGuard;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FixedQueue, bslma::UsesBslmaAllocator);
    // CREATORS
    explicit
    FixedQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator = 0);
        // Create a thread-enabled lock-free queue having the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~FixedQueue();
        // Destroy this object.

    // MANIPULATORS
    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue, blocking
        // until either space is available - if necessary - or the queue is
        // disabled.  Return 0 on success, and a nonzero value if the queue is
        // disabled.

    int tryPushBack(const TYPE& value);
        // Attempt to append the specified 'value' to the back of this queue
        // without blocking.  Return 0 on success, and a non-zero value if the
        // queue is full or disabled.

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
        // 'tryPushBack' will fail immediately.  All blocked invocations of
        // 'pushBack' will fail immediately.  If the queue is already disabled,
        // this method has no effect.

    void enable();
        // Enable queuing.  If the queue is not disabled, this call has no
        // effect.

    // ACCESSORS
    int capacity() const;
        // Return the maximum number of elements that may be stored in this
        // queue.

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

                         // =========================
                         // class FixedQueue_PopGuard
                         // =========================

template <class VALUE>
class FixedQueue_PopGuard {
    // This class provides a guard that, upon its destruction, will remove
    // (pop) the indicated element from the 'FixedQueue' object supplied at
    // construction.  Note that this guard is used to provide exception safety
    // when popping an element from a 'FixedQueue' object.

    // DATA
    FixedQueue<VALUE> *d_parent_p;
                                     // object from which an element will be
                                     // popped

    unsigned int                  d_generation;
                                     // generation count of cell being popped

    unsigned int                  d_index;
                                     // index of cell being popped

  private:
    // NOT IMPLEMENTED
    FixedQueue_PopGuard(const FixedQueue_PopGuard&);
    FixedQueue_PopGuard& operator=(const FixedQueue_PopGuard&);
  public:

    // CREATORS
    FixedQueue_PopGuard(FixedQueue<VALUE> *queue,
                        unsigned int       generation,
                        unsigned int       index);
        // Create a guard that, upon its destruction, will update the state of
        // the specified 'queue' to remove (pop) the element at the specified
        // 'index' having the specified 'generation', and destroy that popped
        // object.  The behavior is undefined unless 'index' and 'generation'
        // refer to a valid element in 'queue' that the current thread has
        // acquired a reservation to pop (using
        // 'FixedQueueIndexManager::reservePopIndex').

    ~FixedQueue_PopGuard();
        // Update the state of the 'FixedQueue' object supplied at construction
        // to remove (pop) the indicated element, and destroy the popped
        // object.
};

                        // ============================
                        // class FixedQueue_PushProctor
                        // ============================

template <class VALUE>
class FixedQueue_PushProctor {
    // This class provides a proctor that, unless the 'release' method has been
    // previously invoked, will remove and destroy all the elements from a
    // 'FixedQueue' object supplied at construction (putting that ring-buffer
    // into a valid empty state) upon the proctor's destruction.  Note that
    // this guard is used to provide exception safety when pushing an element
    // into a 'FixedQueue'.

    // DATA
    FixedQueue<VALUE> *d_parent_p;
                                     // object in which an element was pushed

    unsigned int                  d_generation;
                                     // generation of cell being pushed when an
                                     // exception was thrown

    unsigned int                  d_index;
                                     // index of cell being pushed when an
                                     // exception was thrown

  private:
    // NOT IMPLEMENTED
    FixedQueue_PushProctor(const FixedQueue_PushProctor&);
    FixedQueue_PushProctor& operator=(const FixedQueue_PushProctor&);

  public:

    // CREATORS
    FixedQueue_PushProctor(FixedQueue<VALUE> *queue,
                           unsigned int       generation,
                           unsigned int       index);
        // Create a proctor that manages the specified 'queue' and, unless
        // 'release' is called, will remove and destroy all the elements from
        // 'queue' starting at the specified 'index' in the specified
        // 'generation'.  The behavior is undefined unless 'index' and
        // 'generation' refers to a valid element in 'queue'.

    ~FixedQueue_PushProctor();
        // Destroy this proctor and, if 'release' was not called on this
        // object, remove and destroy all the elements from the 'FixedQueue'
        // object supplied at construction.

    // MANIPULATORS
    void release();
        // Release from management the 'FixedQueue' object supplied at
        // construction.

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// See the .cpp for an implementation note.

                           // ---------------------
                           // class FixedQueue
                           // ---------------------
// CREATORS
template <class TYPE>
FixedQueue<TYPE>::FixedQueue(bsl::size_t       capacity,
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
    d_elements = static_cast<TYPE *>(
                            d_allocator_p->allocate(capacity * sizeof(TYPE)));
}

template <class TYPE>
FixedQueue<TYPE>::~FixedQueue()
{
    removeAll();
    d_allocator_p->deallocate(d_elements);
}

template <class TYPE>
int FixedQueue<TYPE>::tryPushBack(const TYPE& value)
{
    unsigned int generation;
    unsigned int index;

    // SYNCHRONIZATION POINT 1
    //
    // The following call to 'reservePushIndex' writes
    // 'FixedQueueIndexManaged::d_pushIndex' with full sequential consistency,
    // which guarantees the subsequent (relaxed) read from
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

    FixedQueue_PushProctor<TYPE> guard(this, generation, index);
    bslalg::ScalarPrimitives::copyConstruct(&d_elements[index],
                                            value,
                                            d_allocator_p);
    guard.release();
    d_impl.commitPushIndex(generation, index);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPoppers)) {
        d_popControlSema.post();
    }

    return 0;
}

template <class TYPE>
int FixedQueue<TYPE>::tryPopFront(TYPE *value)
{
    unsigned int generation;
    unsigned int index;

    // SYNCHRONIZATION POINT 2
    //
    // The following call to 'reservePopIndex' writes
    // 'FixedQueueIndexManaged::d_popIndex' with full sequential consistency,
    // which guarantees the subsequent (relaxed) read from
    // 'd_numWaitingPoppers' sees any waiting pointers from SYNCHRONIZATION
    // POINT 2-Prime.

    int retval = d_impl.reservePopIndex(&generation, &index);

    if (0 != retval) {
        return retval;                                                // RETURN
    }

    // Copy the element.  'FixedQueue_PopGuard' will destroy original object,
    // update the queue, and release a waiting pusher, even if the assignment
    // operator throws.

    FixedQueue_PopGuard<TYPE> guard(this, generation, index);
    *value = d_elements[index];
    return 0;
}

// MANIPULATORS
template <class TYPE>
int FixedQueue<TYPE>::pushBack(const TYPE& value)
{
    int retval;
    while (0 != (retval = tryPushBack(value))) {
        if (retval < 0) {
            // The queue is disabled.

            return retval;                                            // RETURN
        }

        d_numWaitingPushers.addRelaxed(1);

        // SYNCHRONIZATION POINT 1-Prime
        //
        // The following call to 'isFull' loads
        // 'FixedQueueIndexManager::d_pushIndex' with full sequential
        // consistency, which is required to ensure the visibility of the
        // preceding change to 'd_numWaitingPushers' to SYNCHRONIZATION POINT
        // 2.

        if (isFull() && isEnabled()) {
            d_pushControlSema.wait();
        }

        d_numWaitingPushers.addRelaxed(-1);
    }

    return 0;
}

template <class TYPE>
void FixedQueue<TYPE>::popFront(TYPE *value)
{
    while (0 != tryPopFront(value)) {
        d_numWaitingPoppers.addRelaxed(1);

        // SYNCHRONIZATION POINT 2-Prime
        //
        // The following call to 'isEmpty' loads
        // 'FixedQueueIndexManager::d_pushIndex' with full sequential
        // consistency, which is required to ensure the visibility of the
        // preceding change to 'd_numWaitingPushers' to SYNCHRONIZATION POINT
        // 2.

        if (isEmpty()) {
            d_popControlSema.wait();
        }

        d_numWaitingPoppers.addRelaxed(-1);
    }
}

template <class TYPE>
TYPE FixedQueue<TYPE>::popFront()
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

    // Copy the element.  'FixedQueue_PopGuard' will destroy original object,
    // update the queue, and release a waiting pusher, even if the copy
    // constructor throws.

    FixedQueue_PopGuard<TYPE> guard(this, generation, index);
    return TYPE(d_elements[index]);
}

template <class TYPE>
void FixedQueue<TYPE>::removeAll()
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

    int numWakeUps = bsl::min(poppedItems,
                              static_cast<int>(d_numWaitingPushers));
    while (numWakeUps--) {
        // Wake up waiting pushers.

        d_pushControlSema.post();
    }
}

template <class TYPE>
void FixedQueue<TYPE>::disable()
{
    d_impl.disable();

    const int numWaitingPushers = d_numWaitingPushers;

    for (int i = 0; i < numWaitingPushers; ++i) {
        d_pushControlSema.post();
    }
}

template <class TYPE>
inline
void FixedQueue<TYPE>::enable()
{
    d_impl.enable();
}

// ACCESSORS
template <class TYPE>
inline
int FixedQueue<TYPE>::capacity() const
{
    return static_cast<int>(d_impl.capacity());
}

template <class TYPE>
inline
bool FixedQueue<TYPE>::isEmpty() const
{
    return (0 >= numElements());
}

template <class TYPE>
inline
bool FixedQueue<TYPE>::isEnabled() const
{
    return d_impl.isEnabled();
}

template <class TYPE>
inline
bool FixedQueue<TYPE>::isFull() const
{
    return (capacity() <= numElements());
}

template <class TYPE>
inline
int FixedQueue<TYPE>::length() const
{
    return numElements();
}

template <class TYPE>
inline
int FixedQueue<TYPE>::numElements() const
{
    return static_cast<int>(d_impl.length());
}

template <class TYPE>
inline
int FixedQueue<TYPE>::size() const
{
    return static_cast<int>(capacity());
}

                         // -------------------------
                         // class FixedQueue_PopGuard
                         // -------------------------

// CREATORS
template <class VALUE>
inline
FixedQueue_PopGuard<VALUE>::FixedQueue_PopGuard(FixedQueue<VALUE> *queue,
                                                unsigned int       generation,
                                                unsigned int       index)
: d_parent_p(queue)
, d_generation(generation)
, d_index(index)
{
}

template <class VALUE>
FixedQueue_PopGuard<VALUE>::~FixedQueue_PopGuard()
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

                        // ----------------------------
                        // class FixedQueue_PushProctor
                        // ----------------------------

// CREATORS
template <class VALUE>
inline
FixedQueue_PushProctor<VALUE>::FixedQueue_PushProctor(
                                                 FixedQueue<VALUE> *queue,
                                                 unsigned int       generation,
                                                 unsigned int       index)
: d_parent_p(queue)
, d_generation(generation)
, d_index(index)
{
}

template <class VALUE>
FixedQueue_PushProctor<VALUE>::~FixedQueue_PushProctor()
{
    if (d_parent_p) {
        // This pushing thread currently has the cell at 'd_index' reserved as
        // 'e_WRITING'.  Dispose of all the elements up to 'd_index'.

        unsigned int generation, index;

        // We will always have at least 1 popped item for the cell reserved for
        // writing by the current thread.

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
void FixedQueue_PushProctor<VALUE>::release()
{
    d_parent_p = 0;
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
