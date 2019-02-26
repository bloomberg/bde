// bdlcc_singleproducerqueue.h                                        -*-C++-*-

#ifndef INCLUDED_BDLCC_SINGLEPRODUCERQUEUE
#define INCLUDED_BDLCC_SINGLEPRODUCERQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-aware single producer queue of values.
//
//@CLASSES:
//  bdlcc::SingleProducerQueue: thread-aware single producer queue of 'TYPE'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component defines a type, 'bdlcc::SingleProducerQueue',
// that provides an efficient, thread-aware queue of values assuming a single
// producer (the use of 'pushBack' and 'tryPushBack' is done by one thread or a
// group of threads using external synchronization).  The behavior of the
// methods 'pushBack' and 'tryPushBack' is undefined unless the use is by a
// single producer.  This class is ideal for synchronization and communication
// between threads in a producer-consumer model when there is only one producer
// thread.
//
// The queue provides 'pushBack' and 'popFront' methods for pushing data into
// the queue and popping data from the queue.  The queue will allocate memory
// as necessary to accommodate 'pushBack' invocations ('pushBack' will never
// block and is provided for consistency with other containers).  When the
// queue is empty, the 'popFront' methods block until data appears in the
// queue.  Non-blocking methods 'tryPushBack' and 'tryPopFront' are also
// provided.  The 'tryPopFront' method fails immediately, returning a non-zero
// value, if the queue is empty.
//
// The queue may be placed into a "enqueue disabled" state using the
// 'disablePushBack' method.  When disabled, 'pushBack' and 'tryPushBack' fail
// immediately and return an error code.  The queue may be restored to normal
// operation with the 'enablePushBack' method.
//
// The queue may be placed into a "dequeue disabled" state using the
// 'disablePopFront' method.  When dequeue disabled, 'popFront' and
// 'tryPopFront' fail immediately and return an error code.  Any threads
// blocked in 'popFront' when the queue is dequeue disabled return from
// 'popFront' immediately and return an error code.
//
///Template Requirements
///---------------------
// 'bdlcc::SingleProducerQueue' is a template that is parameterized on the type
// of element contained within the queue.  The supplied template argument,
// 'TYPE', must provide both a default constructor and a copy constructor, as
// well as an assignment operator.  If the default constructor accepts a
// 'bslma::Allocator *', 'TYPE' must declare the uses 'bslma::Allocator' trait
// (see 'bslma_usesbslmaallocator') so that the allocator of the queue is
// propagated to the elements contained in the queue.
//
///Exception safety
///----------------
// A 'bdlcc::SingleProducerQueue' is exception neutral, and all of the methods
// of 'bdlcc::SingleProducerQueue' provide the basic exception safety guarantee
// (see 'bsldoc_glossary').
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlcc::SingleProducerQueue' on C++11
// platforms only (where 'BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES' is defined),
// and are not supported on C++03 platforms.  Unfortunately, in C++03, there
// are user types where a 'bslmf::MovableRef' will not safely degrade to a
// lvalue reference when a move constructor is not available (types providing a
// constructor template taking any type), so 'bslmf::MovableRefUtil::move'
// cannot be used directly on a user supplied template type.  See internal bug
// report 99039150 for more information.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Thread Pool
///- - - - - - - - - - - - - - - -
// In the following example a 'bdlcc::SingleProducerQueue' is used to
// communicate between a single "producer" thread and multiple "consumer"
// threads.  The "producer" will push work requests onto the queue, and each
// "consumer" will iteratively take a work request from the queue and service
// the request.  This example shows a partial, simplified implementation of the
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
// and process them.  Note that the call to 'queue->popFront(&item)' will block
// until there is an element available on the queue.  This function will be
// executed in multiple threads, so that each thread waits in
// 'queue->popFront(&item)', and 'bdlcc::SingleProducerQueue' guarantees that
// each thread gets a unique element from the queue:
//..
//  void myConsumer(bdlcc::SingleProducerQueue<my_WorkRequest> *queue)
//  {
//      while (1) {
//          // 'popFront()' will wait for a 'my_WorkRequest' until available.
//
//          my_WorkRequest item;
//          queue->popFront(&item);
//          if (item.d_type == my_WorkRequest::e_STOP) { break; }
//          myDoWork(item.d_data);
//      }
//  }
//..
// Finally, we define a 'myProducer' function that serves multiple roles: it
// creates the 'bdlcc::SingleProducerQueue', starts the consumer threads, and
// then produces and enqueues work items.  When work requests are exhausted,
// this function enqueues one 'e_STOP' item for each consumer queue.  This
// 'e_STOP' item indicates to the consumer thread to terminate its
// thread-handling function.
//
// Note that, although the producer cannot control which thread 'pop's a
// particular work item, it can rely on the knowledge that each consumer thread
// will read a single 'e_STOP' item and then terminate.
//..
//  void myProducer(int numThreads)
//  {
//      enum {
//          k_NUM_WORK_ITEMS = 1000
//      };
//
//      bdlcc::SingleProducerQueue<my_WorkRequest> queue;
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
//
//      consumerThreads.joinAll();
//  }
//..

#include <bdlscm_version.h>

#include <bdlcc_singleproducerqueueimpl.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_condition.h>
#include <bslmt_mutex.h>

#include <bsls_atomicoperations.h>

namespace BloombergLP {
namespace bdlcc {

                        // =========================
                        // class SingleProducerQueue
                        // =========================

template <class TYPE>
class SingleProducerQueue {
    // This class provides a thread-safe unbounded queue of values that assumes
    // a single producer thread.

    // PRIVATE TYPES
    typedef SingleProducerQueueImpl<TYPE,
                                    bsls::AtomicOperations,
                                    bslmt::Mutex,
                                    bslmt::Condition> Impl;

    // DATA
    Impl d_impl;

    // NOT IMPLEMENTED
    SingleProducerQueue(const SingleProducerQueue&);
    SingleProducerQueue& operator=(const SingleProducerQueue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SingleProducerQueue,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS  = Impl::e_SUCCESS,  // must be 0
        e_EMPTY    = Impl::e_EMPTY,
        e_DISABLED = Impl::e_DISABLED
    };

    // CREATORS
    SingleProducerQueue(bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    SingleProducerQueue(bsl::size_t       capacity,
                        bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with, at least, the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~SingleProducerQueue();
        // Destroy this object.

    // MANIPULATORS
    int popFront(TYPE* value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_DISABLED' if
        // 'isPopFrontDisabled()'.  On failure, 'value' is not changed.
        // Threads blocked due to the queue being empty will return a non-zero
        // value if 'disablePopFront' is invoked.

    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  The behavior is undefined
        // unless the invoker of this method is the single producer.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  On failure, 'value' is not
        // changed.  The behavior is undefined unless the invoker of this
        // method is the single producer.

    void removeAll();
        // Remove all items currently in this queue.  Note that this operation
        // is not atomic; if other threads are concurrently pushing items into
        // the queue the result of 'numElements()' after this function returns
        // is not guaranteed to be 0.

    int tryPopFront(TYPE *value);
        // Attempt to remove the element from the front of this queue without
        // blocking, and, if successful, load the specified 'value' with the
        // removed element.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_EMPTY' if the queue was empty,
        // and 'e_DISABLED' if 'isPopFrontDisabled()'.  On failure, 'value' is
        // not changed.

    int tryPushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  The behavior is undefined
        // unless the invoker of this method is the single producer.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  On failure, 'value' is not
        // changed.  The behavior is undefined unless the invoker of this
        // method is the single producer.

                       // Enqueue/Dequeue State

    void disablePopFront();
        // Disable dequeueing from this queue.  All subsequent invocations of
        // 'popFront' or 'tryPopFront' will fail immediately.  All blocked
        // invocations of 'popFront' and 'waitUntilEmpty' will fail
        // immediately.  If the queue is already dequeue disabled, this method
        // has no effect.

    void disablePushBack();
        // Disable enqueueing into this queue.  All subsequent invocations of
        // 'pushBack' or 'tryPushBack' will fail immediately.  All blocked
        // invocations of 'pushBack' will fail immediately.  If the queue is
        // already enqueue disabled, this method has no effect.

    void enablePushBack();
        // Enable queuing.  If the queue is not enqueue disabled, this call has
        // no effect.

    void enablePopFront();
        // Enable dequeueing.  If the queue is not dequeue disabled, this call
        // has no effect.

    // ACCESSORS
    bool isEmpty() const;
        // Return 'true' if this queue is empty (has no elements), or 'false'
        // otherwise.

    bool isFull() const;
        // Return 'true' if this queue is full (has no available capacity), or
        // 'false' otherwise.  Note that for unbounded queues, this method
        // always returns 'false'.

    bool isPopFrontDisabled() const;
        // Return 'true' if this queue is dequeue disabled, and 'false'
        // otherwise.  Note that the queue is created in the "dequeue enabled"
        // state.

    bool isPushBackDisabled() const;
        // Return 'true' if this queue is enqueue disabled, and 'false'
        // otherwise.  Note that the queue is created in the "enqueue enabled"
        // state.

    bsl::size_t numElements() const;
        // Returns the number of elements currently in this queue.

    int waitUntilEmpty() const;
        // Block until all the elements in this queue are removed.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if '!isEmpty() && isPopFrontDisabled()'.  A blocked
        // thread waiting for the queue to empty will return 'e_DISABLED' if
        // 'disablePopFront' is invoked.  The behavior is undefined unless the
        // invoker of this method is the single producer.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class SingleProducerQueue
                        // -------------------------

// CREATORS
template <class TYPE>
SingleProducerQueue<TYPE>::SingleProducerQueue(
                                              bslma::Allocator *basicAllocator)
: d_impl(basicAllocator)
{
}

template <class TYPE>
SingleProducerQueue<TYPE>::SingleProducerQueue(
                                              bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, basicAllocator)
{
}

template <class TYPE>
SingleProducerQueue<TYPE>::~SingleProducerQueue()
{
}

// MANIPULATORS
template <class TYPE>
int SingleProducerQueue<TYPE>::popFront(TYPE *value)
{
    return d_impl.popFront(value);
}

template <class TYPE>
int SingleProducerQueue<TYPE>::pushBack(const TYPE& value)
{
    return d_impl.pushBack(value);
}

template <class TYPE>
int SingleProducerQueue<TYPE>::pushBack(bslmf::MovableRef<TYPE> value)
{
    return d_impl.pushBack(bslmf::MovableRefUtil::move(value));
}

template <class TYPE>
void SingleProducerQueue<TYPE>::removeAll()
{
    d_impl.removeAll();
}

template <class TYPE>
int SingleProducerQueue<TYPE>::tryPopFront(TYPE *value)
{
    return d_impl.tryPopFront(value);
}

template <class TYPE>
int SingleProducerQueue<TYPE>::tryPushBack(const TYPE& value)
{
    return d_impl.tryPushBack(value);
}

template <class TYPE>
int SingleProducerQueue<TYPE>::tryPushBack(bslmf::MovableRef<TYPE> value)
{
    return d_impl.tryPushBack(bslmf::MovableRefUtil::move(value));
}

                       // Enqueue/Dequeue State

template <class TYPE>
void SingleProducerQueue<TYPE>::disablePopFront()
{
    d_impl.disablePopFront();
}

template <class TYPE>
void SingleProducerQueue<TYPE>::disablePushBack()
{
    d_impl.disablePushBack();
}

template <class TYPE>
void SingleProducerQueue<TYPE>::enablePopFront()
{
    d_impl.enablePopFront();
}

template <class TYPE>
void SingleProducerQueue<TYPE>::enablePushBack()
{
    d_impl.enablePushBack();
}

// ACCESSORS
template <class TYPE>
bool SingleProducerQueue<TYPE>::isEmpty() const
{
    return d_impl.isEmpty();
}

template <class TYPE>
bool SingleProducerQueue<TYPE>::isFull() const
{
    return d_impl.isFull();
}

template <class TYPE>
bool SingleProducerQueue<TYPE>::isPopFrontDisabled() const
{
    return d_impl.isPopFrontDisabled();
}

template <class TYPE>
bool SingleProducerQueue<TYPE>::isPushBackDisabled() const
{
    return d_impl.isPushBackDisabled();
}

template <class TYPE>
bsl::size_t SingleProducerQueue<TYPE>::numElements() const
{
    return d_impl.numElements();
}

template <class TYPE>
int SingleProducerQueue<TYPE>::waitUntilEmpty() const
{
    return d_impl.waitUntilEmpty();
}

                                  // Aspects

template <class TYPE>
bslma::Allocator *SingleProducerQueue<TYPE>::allocator() const
{
    return d_impl.allocator();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
