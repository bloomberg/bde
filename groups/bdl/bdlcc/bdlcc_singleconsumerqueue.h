// bdlcc_singleconsumerqueue.h                                        -*-C++-*-

#ifndef INCLUDED_BDLCC_SINGLECONSUMERQUEUE
#define INCLUDED_BDLCC_SINGLECONSUMERQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-aware single consumer queue of values.
//
//@CLASSES:
//  bdlcc::SingleConsumerQueue: thread-aware single consumer queue of 'TYPE'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component defines a type, 'bdlcc::SingleConsumerQueue',
// that provides an efficient, thread-aware queue of values assuming a single
// consumer (the use of 'popFront', 'tryPopFront', and 'removeAll' is done by
// one thread or a group of threads using external synchronization).  The
// behavior of the methods 'popFront', 'tryPopFront', and 'removeAll' is
// undefined unless the use is by a single consumer.  This class is ideal for
// synchronization and communication between threads in a producer-consumer
// model when there is only one consumer thread.
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
// 'bdlcc::SingleConsumerQueue' is a template that is parameterized on the type
// of element contained within the queue.  The supplied template argument,
// 'TYPE', must provide both a default constructor and a copy constructor, as
// well as an assignment operator.  If the default constructor accepts a
// 'bslma::Allocator *', 'TYPE' must declare the uses 'bslma::Allocator' trait
// (see 'bslma_usesbslmaallocator') so that the allocator of the queue is
// propagated to the elements contained in the queue.
//
///Exception safety
///----------------
// A 'bdlcc::SingleConsumerQueue' is exception neutral, and all of the methods
// of 'bdlcc::SingleConsumerQueue' provide the basic exception safety guarantee
// (see 'bsldoc_glossary').
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlcc::SingleConsumerQueue' on C++11
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
// In the following example a 'bdlcc::SingleConsumerQueue' is used to
// communicate between multiple "producer" threads and a single "consumer"
// thread.  The "producers" will push work requests onto the queue, and the
// "consumer" will iteratively take a work request from the queue and service
// the request.
//
// First, we define a utility classes that handles a simple "work item":
//..
//  struct my_WorkData {
//      // Work data...
//  };
//..
// Next, we provide a simple function to service an individual work item.  The
// details are unimportant for this example:
//..
//  void myDoWork(const my_WorkData& data)
//  {
//      // do some stuff...
//      (void)data;
//  }
//..
// Then, we define a 'myProducer' function that will push elements onto the
// queue until the queue is disabled.  Note that the call to
// 'queue->pushFront(&item)' will never block:
//..
//  void myProducer(bdlcc::SingleConsumerQueue<my_WorkData> *queue)
//  {
//      while (1) {
//          my_WorkData item;
//          if (queue->pushBack(item)) {
//              return;                                               // RETURN
//          }
//      }
//  }
//..
// Finally, we define a 'myConsumer' function that serves multiple roles: it
// creates the 'bdlcc::SingleConsumerQueue', starts the producer threads, and
// then dequeues and processes work items.  After completing an amount of work
// items, the queue is disabled for enqueueing, the producer threads are joined
// and the consumer uses 'tryPopFront' until the queue is empty.
//..
//  void myConsumer(int numThreads)
//  {
//      enum {
//          k_NUM_WORK_ITEMS = 1000
//      };
//
//      bdlcc::SingleConsumerQueue<my_WorkData> queue;
//
//      bslmt::ThreadGroup producerThreads;
//      producerThreads.addThreads(bdlf::BindUtil::bind(&myProducer, &queue),
//                                 numThreads);
//
//      my_WorkData item;
//
//      for (int i = 0; i < k_NUM_WORK_ITEMS; ++i) {
//          queue.popFront(&item);
//          myDoWork(item);
//      }
//
//      queue.disablePushBack();
//
//      producerThreads.joinAll();
//
//      while (0 == queue.tryPopFront(&item)) {
//          myDoWork(item);
//      }
//
//      ASSERT(queue.isEmpty());
//  }
//..

#include <bdlscm_version.h>

#include <bdlcc_singleconsumerqueueimpl.h>

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
                        // class SingleConsumerQueue
                        // =========================

template <class TYPE>
class SingleConsumerQueue {
    // This class provides a thread-safe unbounded queue of values that assumes
    // a single consumer thread.

    // PRIVATE TYPES
    typedef SingleConsumerQueueImpl<TYPE,
                                    bsls::AtomicOperations,
                                    bslmt::Mutex,
                                    bslmt::Condition> Impl;

    // DATA
    Impl d_impl;

    // NOT IMPLEMENTED
    SingleConsumerQueue(const SingleConsumerQueue&);
    SingleConsumerQueue& operator=(const SingleConsumerQueue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SingleConsumerQueue,
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
    explicit
    SingleConsumerQueue(bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    explicit
    SingleConsumerQueue(bsl::size_t       capacity,
                        bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with, at least, the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    // ~SingleConsumerQueue() = default;
        // Destroy this object.

    // MANIPULATORS
    int popFront(TYPE* value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_DISABLED' if
        // 'isPopFrontDisabled()'.  On failure, 'value' is not changed.
        // Threads blocked due to the queue being empty will return
        // 'e_DISABLED' if 'disablePopFront' is invoked.  The behavior is
        // undefined unless the invoker of this method is the single consumer.

    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  On failure, 'value' is not
        // changed.

    void removeAll();
        // Remove all items currently in this queue.  Note that this operation
        // is not atomic; if other threads are concurrently pushing items into
        // the queue the result of 'numElements()' after this function returns
        // is not guaranteed to be 0.  The behavior is undefined unless the
        // invoker of this method is the single consumer.

    int tryPopFront(TYPE *value);
        // Attempt to remove the element from the front of this queue without
        // blocking, and, if successful, load the specified 'value' with the
        // removed element.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_DISABLED' if
        // 'isPopFrontDisabled()', and 'e_EMPTY' if '!isPopFrontDisabled()' and
        // the queue was empty.  On failure, 'value' is not changed.  The
        // behavior is undefined unless the invoker of this method is the
        // single consumer.

    int tryPushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()'.  On failure, 'value' is not
        // changed.

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
        // 'disablePopFront' is invoked.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class SingleConsumerQueue
                        // -------------------------

// CREATORS
template <class TYPE>
SingleConsumerQueue<TYPE>::SingleConsumerQueue(
                                              bslma::Allocator *basicAllocator)
: d_impl(basicAllocator)
{
}

template <class TYPE>
SingleConsumerQueue<TYPE>::SingleConsumerQueue(
                                              bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, basicAllocator)
{
}

// MANIPULATORS
template <class TYPE>
int SingleConsumerQueue<TYPE>::popFront(TYPE *value)
{
    return d_impl.popFront(value);
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::pushBack(const TYPE& value)
{
    return d_impl.pushBack(value);
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::pushBack(bslmf::MovableRef<TYPE> value)
{
    return d_impl.pushBack(bslmf::MovableRefUtil::move(value));
}

template <class TYPE>
void SingleConsumerQueue<TYPE>::removeAll()
{
    d_impl.removeAll();
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::tryPopFront(TYPE *value)
{
    return d_impl.tryPopFront(value);
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::tryPushBack(const TYPE& value)
{
    return d_impl.tryPushBack(value);
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::tryPushBack(bslmf::MovableRef<TYPE> value)
{
    return d_impl.tryPushBack(bslmf::MovableRefUtil::move(value));
}

                       // Enqueue/Dequeue State

template <class TYPE>
void SingleConsumerQueue<TYPE>::disablePopFront()
{
    d_impl.disablePopFront();
}

template <class TYPE>
void SingleConsumerQueue<TYPE>::disablePushBack()
{
    d_impl.disablePushBack();
}

template <class TYPE>
void SingleConsumerQueue<TYPE>::enablePopFront()
{
    d_impl.enablePopFront();
}

template <class TYPE>
void SingleConsumerQueue<TYPE>::enablePushBack()
{
    d_impl.enablePushBack();
}

// ACCESSORS
template <class TYPE>
bool SingleConsumerQueue<TYPE>::isEmpty() const
{
    return d_impl.isEmpty();
}

template <class TYPE>
bool SingleConsumerQueue<TYPE>::isFull() const
{
    return d_impl.isFull();
}

template <class TYPE>
bool SingleConsumerQueue<TYPE>::isPopFrontDisabled() const
{
    return d_impl.isPopFrontDisabled();
}

template <class TYPE>
bool SingleConsumerQueue<TYPE>::isPushBackDisabled() const
{
    return d_impl.isPushBackDisabled();
}

template <class TYPE>
bsl::size_t SingleConsumerQueue<TYPE>::numElements() const
{
    return d_impl.numElements();
}

template <class TYPE>
int SingleConsumerQueue<TYPE>::waitUntilEmpty() const
{
    return d_impl.waitUntilEmpty();
}

                                  // Aspects

template <class TYPE>
bslma::Allocator *SingleConsumerQueue<TYPE>::allocator() const
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
