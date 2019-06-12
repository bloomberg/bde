// bdlcc_singleproducersingleconsumerboundedqueue.h                   -*-C++-*-

#ifndef INCLUDED_BDLCC_SINGLEPRODUCERSINGLECONSUMERBOUNDEDQUEUE
#define INCLUDED_BDLCC_SINGLEPRODUCERSINGLECONSUMERBOUNDEDQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-aware SPSC bounded queue of values.
//
//@CLASSES:
//  bdlcc::SingleProducerSingleConsumerBoundedQueue: SPSCB concurrent queue
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component defines a type,
// 'bdlcc::SingleProducerSingleConsumerBoundedQueue', that provides an
// efficient, thread-aware bounded (capacity fixed at construction) queue of
// values assuming a single producer and a single consumer.  The behavior of
// the methods 'pushBack' and 'tryPushBack' is undefined unless the use is by a
// single producer (one thread or a group of threads using external
// synchronization).  Also, the behavior of the methods 'popFront',
// 'tryPopFront', and 'removeAll' is undefined unless the use is by a single
// consumer.  This class is ideal for synchronization and communication between
// threads in a producer-consumer model when a bounded queue is appropriate and
// there is only one producer thread and one consumer thread.
//
// The queue provides 'pushBack' and 'popFront' methods for pushing data into
// the queue and popping data from the queue.  When the queue is full, the
// 'pushBack' methods block until data is removed from the queue.  When the
// queue is empty, the 'popFront' methods block until data appears in the
// queue.  Non-blocking methods 'tryPushBack' and 'tryPopFront' are also
// provided.  The 'tryPushBack' method fails immediately, returning a non-zero
// value, if the queue is full.  The 'tryPopFront' method fails immediately,
// returning a non-zero value, if the queue is empty.
//
// The queue may be placed into a "enqueue disabled" state using the
// 'disablePushBack' method.  When disabled, 'pushBack' and 'tryPushBack' fail
// immediately and return an error code.  Any threads blocked in 'pushBack'
// when the queue is enqueue disabled return from 'pushBack' immediately and
// return an error code.  The queue may be restored to normal operation with
// the 'enablePushBack' method.
//
// The queue may be placed into a "dequeue disabled" state using the
// 'disablePopFront' method.  When dequeue disabled, 'popFront', 'tryPopFront',
// and 'waitUntilEmpty' fail immediately and return an error code.  Any threads
// blocked in 'popFront' and 'waitUntilEmpty' when the queue is dequeue
// disabled return immediately and return an error code.  The queue may be
// restored to normal operation with the 'enablePopFront' method.
//
///Template Requirements
///---------------------
// 'bdlcc::SingleProducerSingleConsumerBoundedQueue' is a template that is
// parameterized on the type of element contained within the queue.  The
// supplied template argument, 'TYPE', must provide both a default constructor
// and a copy constructor, as well as an assignment operator.  If the default
// constructor accepts a 'bslma::Allocator *', 'TYPE' must declare the uses
// 'bslma::Allocator' trait (see 'bslma_usesbslmaallocator') so that the
// allocator of the queue is propagated to the elements contained in the queue.
//
///Exception safety
///----------------
// A 'bdlcc::SingleProducerSingleConsumerBoundedQueue' is exception neutral,
// and all of the methods of 'bdlcc::SingleProducerSingleConsumerBoundedQueue'
// provide the strong exception safety guarantee (see 'bsldoc_glossary').
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by
// 'bdlcc::SingleProducerSingleConsumerBoundedQueue' on C++11 platforms only
// (where 'BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES' is defined), and are not
// supported on C++03 platforms.  Unfortunately, in C++03, there are user types
// where a 'bslmf::MovableRef' will not safely degrade to a lvalue reference
// when a move constructor is not available (types providing a constructor
// template taking any type), so 'bslmf::MovableRefUtil::move' cannot be used
// directly on a user supplied template type.  See internal bug report 99039150
// for more information.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Thread Pool
///- - - - - - - - - - - - - - - -
// In the following example a 'bdlcc::SingleProducerSingleConsumerBoundedQueue'
// is used to communicate between a single "producer" thread and a single
// "consumer" thread.  The "producer" will push work requests onto the queue,
// and the "consumer" will iteratively take a work request from the queue and
// service the request.  This example shows a partial, simplified
// implementation of the 'bdlmt::FixedThreadPool' class.  See component
// 'bdlmt_fixedthreadpool' for more information.
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
//  void myDoWork(const my_WorkData& data)
//      // Do some work based upon the specified 'data'.
//  {
//      // do some stuff...
//      (void)data;
//  }
//..
// Then, we define a 'myConsumer' function that will pop elements off the queue
// and process them.  Note that the call to 'queue->popFront()' will block
// until there is an element available on the queue:
//..
//  void myConsumer(
//      bdlcc::SingleProducerSingleConsumerBoundedQueue<my_WorkRequest> *queue)
//      // Pop elements from the specified 'queue'.
//  {
//      while (1) {
//          // 'popFront()' will wait for a 'my_WorkRequest' until available.
//
//          my_WorkRequest item;
//          item.d_type = my_WorkRequest::e_WORK;
//
//          assert(0 == queue->popFront(&item));
//
//          if (item.d_type == my_WorkRequest::e_STOP) { break; }
//          myDoWork(item.d_data);
//      }
//  }
//..
// Finally, we define a 'myProducer' function that serves multiple roles: it
// creates the 'bdlcc::SingleProducerSingleConsumerBoundedQueue', starts the
// consumer thread, and then produces and enqueues work items.  When work
// requests are exhausted, this function enqueues one 'e_STOP' item for the
// consumer queue.  This 'e_STOP' item indicates to the consumer thread to
// terminate its thread-handling function.
//..
//  void myProducer()
//      // Create a queue, start consumer thread, produce and enqueue work.
//  {
//      enum {
//          k_MAX_QUEUE_LENGTH = 100,
//          k_NUM_WORK_ITEMS   = 1000
//      };
//
//      bdlcc::SingleProducerSingleConsumerBoundedQueue<my_WorkRequest>
//                                                   queue(k_MAX_QUEUE_LENGTH);
//
//      bslmt::ThreadGroup consumerThreads;
//      consumerThreads.addThreads(bdlf::BindUtil::bind(&myConsumer, &queue),
//                                 1);
//
//      for (int i = 0; i < k_NUM_WORK_ITEMS; ++i) {
//          my_WorkRequest item;
//          item.d_type = my_WorkRequest::e_WORK;
//          item.d_data = my_WorkData(); // some stuff to do
//          queue.pushBack(item);
//      }
//
//      {
//          my_WorkRequest item;
//          item.d_type = my_WorkRequest::e_STOP;
//          queue.pushBack(item);
//      }
//
//      consumerThreads.joinAll();
//  }
//..

#include <bdlscm_version.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_platform.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlcc {

      // ===============================================================
      // class SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard
      // ===============================================================

template <class TYPE, class NODE>
class SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard {
    // This class implements a guard that invokes 'TYPE::popComplete' on a
    // 'NODE' upon destruction.

    // PRIVATE TYPES
    typedef typename bsls::Types::Uint64 Uint64;

    // DATA
    TYPE   *d_queue_p;  // managed queue owning the managed node
    NODE   *d_node_p;   // managed node
    Uint64  d_index;    // value of 'd_queue_p->d_popIndex'

    // NOT IMPLEMENTED
    SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard();
    SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard(
             const SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard&);
    SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard& operator=(
             const SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard&);

  public:
    // CREATORS
    SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard(TYPE   *queue,
                                                              NODE   *node,
                                                              Uint64  index);
        // Create a guard managing the specified 'queue' and will invoke
        // 'popComplete' with the specified 'node' and 'index'.

    ~SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard();
        // Destroy this object and invoke the 'TYPE::popComplete'.
};

              // ==============================================
              // class SingleProducerSingleConsumerBoundedQueue
              // ==============================================

template <class TYPE>
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIGNAS)
class alignas(bslmt::Platform::e_CACHE_LINE_SIZE)
                                     SingleProducerSingleConsumerBoundedQueue {
#else
class SingleProducerSingleConsumerBoundedQueue {
#endif
    // This class provides a thread-safe bounded queue of values.

    // PRIVATE TYPES
    typedef          unsigned int                                Uint;
    typedef typename bsls::Types::Uint64                         Uint64;
    typedef typename bsls::AtomicOperations::AtomicTypes::Uint   AtomicUint;
    typedef typename bsls::AtomicOperations::AtomicTypes::Uint64 AtomicUint64;
    typedef typename bsls::AtomicOperations                      AtomicOp;

    // PRIVATE CONSTANTS
    enum {
        // These value are used as values for 'd_state' in 'Node'.  A node is
        // writable at creation and after a read completes (when the single
        // producer can write to the node).  A node is readable after it is
        // written (when the node can be read by the single consumer).  The
        // states in-between these two states (e.g., writing) are not needed by
        // this implementation of the queue.

        e_READABLE,              // node can be read
        e_READABLE_AND_BLOCKED,  // node can be read and has blocked writer
        e_WRITABLE,              // node can be written
        e_WRITABLE_AND_BLOCKED   // node can be written and has blocked reader
    };

    // PRIVATE TYPES
    template <class DATA>
    struct QueueNode {
        // PUBLIC DATA
        bsls::ObjectBuffer<DATA> d_value;  // stored value
        AtomicUint               d_state;  // 'e_READABLE', 'e_WRITABLE', etc.
    };

    typedef QueueNode<TYPE> Node;

    // DATA
    AtomicUint64              d_popIndex;        // index of next element to
                                                 // pop

    Node                     *d_popElement_p;    // array of elements that
                                                 // comprise the bounded queue;
                                                 // identical to
                                                 // 'd_pushElement_p'

    const bsl::size_t         d_popCapacity;     // the capacity of the queue;
                                                 // identical to
                                                 // 'd_pushCapacity'

    AtomicUint                d_popDisabledGeneration;
                                                 // generation count of pop
                                                 // disablements

    mutable AtomicUint        d_emptyCount;      // count of threads in
                                                 // 'waitUntilEmpty'

    AtomicUint                d_emptyGeneration; // generation count of a
                                                 // method causing the queue to
                                                 // be empty

    const char                d_popPad[  bslmt::Platform::e_CACHE_LINE_SIZE
                                       - sizeof(AtomicUint64)
                                       - sizeof(Node *)
                                       - sizeof(bsl::size_t)
                                       - sizeof(AtomicUint)
                                       - sizeof(AtomicUint)
                                       - sizeof(AtomicUint)];
                                                 // padding to prevent
                                                 // subsequent data from being
                                                 // in the same cache line as
                                                 // the prior data

    AtomicUint64              d_pushIndex;       // index of next target
                                                 // element for a push

    Node                     *d_pushElement_p;   // array of elements that
                                                 // comprise the bounded queue;
                                                 // identical to
                                                 // 'd_popElement_p'

    const bsl::size_t         d_pushCapacity;    // the capacity of the queue;
                                                 // identical to
                                                 // 'd_popCapacity'

    AtomicUint                d_pushDisabledGeneration;
                                                 // generation count of push
                                                 // disablements

    const char                d_pushPad[  bslmt::Platform::e_CACHE_LINE_SIZE
                                        - sizeof(AtomicUint64)
                                        - sizeof(Node *)
                                        - sizeof(bsl::size_t)
                                        - sizeof(AtomicUint)];
                                                 // padding to prevent
                                                 // subsequent data from being
                                                 // in the same cache line as
                                                 // the prior data

    bslmt::Mutex              d_popMutex;        // used with 'd_popCondition'
                                                 // to block the consumer when
                                                 // the queue is empty

    bslmt::Condition          d_popCondition;    // condition for blocking the
                                                 // consumer when the queue is
                                                 // empty

    bslmt::Mutex              d_pushMutex;       // used with 'd_pushCondition'
                                                 // to block the producer when
                                                 // the queue is full

    bslmt::Condition          d_pushCondition;   // condition for blocking the
                                                 // producer when the queue is
                                                 // full

    mutable bslmt::Mutex      d_emptyMutex;      // blocking point for
                                                 // 'waitUntilEmpty'

    mutable bslmt::Condition  d_emptyCondition;  // condition variable for
                                                 // 'waitUntilEmpty'

    bslma::Allocator         *d_allocator_p;     // allocator, held not owned

    // FRIENDS
    friend class SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard<
                SingleProducerSingleConsumerBoundedQueue<TYPE>,
                typename SingleProducerSingleConsumerBoundedQueue<TYPE>::Node>;

    // PRIVATE CLASS METHODS
    static void incrementUntil(AtomicUint *value, unsigned int bitValue);
        // If the specified 'value' does not have its lowest-order bit set to
        // the value of the specified 'bitValue', increment 'value' until it
        // does.  Note that this method is used to modify the generation counts
        // stored in 'd_popDisabledGeneration' and 'd_pushDisabledGeneration'.

    // PRIVATE MANIPULATORS
    void popComplete(Node *node, Uint64 index);
        // Destruct the value stored in the specified 'node', use the specified
        // 'index' in calculations to mark the 'node' writable, unblock any
        // blocked "push" threads, and if the queue is empty update the empty
        // generation and signal the queue empty condition.  This method is
        // used within 'popFrontImp' by a guard to complete the reclamation of
        // a node in the presence of an exception.

    int popFrontImp(TYPE *value, bool isTry);
        // If the specified 'isTry' is 'false', remove the element from the
        // front of this queue and load that element into the specified
        // 'value'; otherwise, attempt to remove the element from the front of
        // this queue without blocking, and, if successful, load the 'value'
        // with the removed element.  If 'false == isTry' and the queue is
        // empty, block until it is not empty.  Return 0 on success, and a
        // non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if 'isPopFrontDisabled()', 'e_EMPTY' if
        // 'true == isTry', '!isPopFrontDisabled()', and the queue is empty,
        // and 'e_FAILED' if an underlying mechanism returns an error.  On
        // failure, 'value' is not changed.  Threads blocked due to the queue
        // being empty will return 'e_DISABLED' if 'disablePopFront' is
        // invoked.

    int pushBackImp(const TYPE& value, bool isTry);
        // If the specified 'isTry' is 'false', append the specified 'value' to
        // the back of this queue; otherwise, attempt to append the 'value' to
        // the back of this queue without blocking.  Return 0 on success, and a
        // non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if 'isPushBackDisabled()', 'e_FULL' if
        // 'true == isTry', '!isPushBackDisabled()', and the queue is full, and
        // 'e_FAILED' if an underlying mechanism returns an error.  Threads
        // blocked due to the queue being full will return 'e_DISABLED' if
        // 'disablePushFront' is invoked.

    int pushBackImp(bslmf::MovableRef<TYPE> value, bool isTry);
        // If the specified 'isTry' is 'false', append the specified
        // move-insertable 'value' to the back of this queue; otherwise,
        // attempt to append the 'value' to the back of this queue without
        // blocking.  'value' is left in a valid but unspecified state.  Return
        // 0 on success, and a non-zero value otherwise.  Specifically, return
        // 'e_DISABLED' if 'isPushBackDisabled()', 'e_FULL' if 'true == isTry',
        // '!isPushBackDisabled()', and the queue is full, and 'e_FAILED' if an
        // underlying mechanism returns an error.  On failure, 'value' is not
        // changed.  Threads blocked due to the queue being full will return
        // 'e_DISABLED' if 'disablePushFront' is invoked.

    void pushComplete(Node *node, Uint64 index);
        // Mark the specified 'node' readable, signal 'd_popCondition' if
        // necessary, and update 'd_popIndex' to be the index value of the
        // location to be used after specified 'index' location.  This method
        // is invoked from 'pushBackImp'.

    // NOT IMPLEMENTED
    SingleProducerSingleConsumerBoundedQueue(
                              const SingleProducerSingleConsumerBoundedQueue&);
    SingleProducerSingleConsumerBoundedQueue& operator=(
                              const SingleProducerSingleConsumerBoundedQueue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SingleProducerSingleConsumerBoundedQueue,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS  =  0,
        e_EMPTY    = -1,
        e_FULL     = -2,
        e_DISABLED = -3,
        e_FAILED   = -4
    };

    // CREATORS
    explicit
    SingleProducerSingleConsumerBoundedQueue(
                                         bsl::size_t       capacity,
                                         bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with, at least, the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~SingleProducerSingleConsumerBoundedQueue();
        // Destroy this object.

    // MANIPULATORS
    int popFront(TYPE *value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_SUCCESS' on success,
        // 'e_DISABLED' if 'isPopFrontDisabled()' and 'e_FAILED' if an
        // underlying mechanism returns an error.  On failure, 'value' is not
        // changed.  Threads blocked due to the queue being empty will return
        // 'e_DISABLED' if 'disablePopFront' is invoked.  The behavior is
        // undefined unless the invoker of this method is the single consumer.

    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()' and
        // 'e_FAILED' if an underlying mechanism returns an error.  Threads
        // blocked due to the queue being full will return 'e_DISABLED' if
        // 'disablePushFront' is invoked.  The behavior is undefined unless the
        // invoker of this method is the single producer.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()' and
        // 'e_FAILED' if an underlying mechanism returns an error.  On failure,
        // 'value' is not changed.  Threads blocked due to the queue being full
        // will return 'e_DISABLED' if 'disablePushFront' is invoked.  The
        // behavior is undefined unless the invoker of this method is the
        // single producer.

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
        // otherwise.  Specifically, return 'e_SUCCESS' on success,
        // 'e_DISABLED' if 'isPopFrontDisabled()', and 'e_EMPTY' if
        // '!isPopFrontDisabled()' and the queue was empty.  On failure,
        // 'value' is not changed.  The behavior is undefined unless the
        // invoker of this method is the single consumer.

    int tryPushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()', and
        // 'e_FULL' if '!isPushBackDisabled()' and the queue was full.  The
        // behavior is undefined unless the invoker of this method is the
        // single producer.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()', and
        // 'e_FULL' if '!isPushBackDisabled()' and the queue was full.  On
        // failure, 'value' is not changed.  The behavior is undefined unless
        // the invoker of this method is the single producer.

                       // Enqueue/Dequeue State

    void disablePopFront();
        // Disable dequeueing from this queue.  All subsequent invocations of
        // 'popFront' or 'tryPopFront' will fail immediately.  If the single
        // consumer is blocked in 'popFront', the invocation of 'popFront' will
        // fail immediately.  Any blocked invocations of 'waitUntilEmpty' will
        // fail immediately.  If the queue is already dequeue disabled, this
        // method has no effect.

    void disablePushBack();
        // Disable enqueueing into this queue.  All subsequent invocations of
        // 'pushBack' or 'tryPushBack' will fail immediately.  If the single
        // producer is blocked in 'pushBack', the invocation of 'pushBack' will
        // fail immediately.  If the queue is already enqueue disabled, this
        // method has no effect.

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
        // 'false' otherwise.

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
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPopFrontDisabled()' and
        // 'e_FAILED' if an underlying mechanism returns an error.  A blocked
        // thread waiting for the queue to empty will return a non-zero value
        // if 'disablePopFront' is invoked.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

     // ---------------------------------------------------------------
     // class SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard
     // ---------------------------------------------------------------

// CREATORS
template <class TYPE, class NODE>
inline
SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard<TYPE, NODE>
                   ::SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard(
                                                                 TYPE   *queue,
                                                                 NODE   *node,
                                                                 Uint64  index)
: d_queue_p(queue)
, d_node_p(node)
, d_index(index)
{
}

template <class TYPE, class NODE>
inline
SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard<TYPE, NODE>
                 ::~SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard()
{
    d_queue_p->popComplete(d_node_p, d_index);
}

              // ----------------------------------------------
              // class SingleProducerSingleConsumerBoundedQueue
              // ----------------------------------------------

// PRIVATE CLASS METHODS
template <class TYPE>
void SingleProducerSingleConsumerBoundedQueue<TYPE>
                     ::incrementUntil(AtomicUint *value, unsigned int bitValue)
{
    unsigned int state = AtomicOp::getUintAcquire(value);
    if (bitValue != (state & 1)) {
        unsigned int expState;
        do {
            expState = state;
            state = AtomicOp::testAndSwapUintAcqRel(value,
                                                     state,
                                                     state + 1);
        } while (state != expState && (bitValue == (state & 1)));
    }
}

// PRIVATE MANIPULATORS
template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::popComplete(Node   *node,
                                                                 Uint64  index)
{
    ++index;
    if (index == d_popCapacity) {
        index = 0;
    }
    AtomicOp::setUint64Release(&d_popIndex, index);

    node->d_value.object().~TYPE();

    Uint nodeState = AtomicOp::swapUintAcqRel(&node->d_state, e_WRITABLE);
    if (e_READABLE_AND_BLOCKED == nodeState) {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_pushMutex);
        }
        d_pushCondition.signal();
    }

    if (e_WRITABLE ==
                    AtomicOp::getUintAcquire(&d_popElement_p[index].d_state)) {
        AtomicOp::addUintAcqRel(&d_emptyGeneration, 1);
        if (0 < AtomicOp::getUintAcquire(&d_emptyCount)) {
            {
                bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
            }
            d_emptyCondition.broadcast();
        }
    }
}

template <class TYPE>
int SingleProducerSingleConsumerBoundedQueue<TYPE>::popFrontImp(TYPE *value,
                                                                bool  isTry)
{
    Uint64     index = AtomicOp::getUint64Acquire(&d_popIndex);
    const Uint disabledGen =
                            AtomicOp::getUintAcquire(&d_popDisabledGeneration);

    if (disabledGen & 1) {
        return e_DISABLED;                                            // RETURN
    }

    Node& node = d_popElement_p[index];

    Uint nodeState = AtomicOp::getUintAcquire(&node.d_state);

    // If the node is not available for reading:
    //   * if this is a "try" invocation, return
    //   * otherwise, yield and check again, then block
    // Note that 'e_WRITABLE_AND_BLOCKED != nodeState' since this is the one
    // consumer.

    if (e_WRITABLE == nodeState) {
        if (isTry) {
            return e_EMPTY;                                           // RETURN
        }

        bslmt::ThreadUtil::yield();
        nodeState = AtomicOp::getUintAcquire(&node.d_state);
        if (e_WRITABLE == nodeState) {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_popMutex);

            nodeState = AtomicOp::testAndSwapUintAcqRel(
                                                       &node.d_state,
                                                       e_WRITABLE,
                                                       e_WRITABLE_AND_BLOCKED);

            while ((   e_WRITABLE             == nodeState
                    || e_WRITABLE_AND_BLOCKED == nodeState)
                   && disabledGen ==
                          AtomicOp::getUintAcquire(&d_popDisabledGeneration)) {
                int rv = d_popCondition.wait(&d_popMutex);
                if (rv) {
                    AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                    e_WRITABLE_AND_BLOCKED,
                                                    e_WRITABLE);
                    return e_FAILED;                                  // RETURN
                }
                nodeState = AtomicOp::getUint(&node.d_state);
            }

            // The following checks for disablement being the cause of exiting
            // the 'while' loop.

            if (   e_WRITABLE             == nodeState
                || e_WRITABLE_AND_BLOCKED == nodeState) {
                AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                e_WRITABLE_AND_BLOCKED,
                                                e_WRITABLE);
                return e_DISABLED;                                    // RETURN
            }
        }
    }

    SingleProducerSingleConsumerBoundedQueue_PopCompleteGuard<
                          SingleProducerSingleConsumerBoundedQueue<TYPE>, Node>
                                                     guard(this, &node, index);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    *value = bslmf::MovableRefUtil::move(node.d_value.object());
#else
    *value = node.d_value.object();
#endif

    return e_SUCCESS;
}

template <class TYPE>
int SingleProducerSingleConsumerBoundedQueue<TYPE>::pushBackImp(
                                                             const TYPE& value,
                                                             bool        isTry)
{
    Uint64     index = AtomicOp::getUint64Acquire(&d_pushIndex);
    const Uint disabledGen =
                           AtomicOp::getUintAcquire(&d_pushDisabledGeneration);

    if (disabledGen & 1) {
        return e_DISABLED;                                            // RETURN
    }

    Node& node = d_pushElement_p[index];

    Uint nodeState = AtomicOp::getUintAcquire(&node.d_state);

    // If the node is not available for writing:
    //   * if this is a "try" invocation, return
    //   * otherwise, yield and check again, then block
    // Note that 'e_READABLE_AND_BLOCKED != nodeState' since this is the one
    // producer.

    if (e_READABLE == nodeState) {
        if (isTry) {
            return e_FULL;                                            // RETURN
        }

        bslmt::ThreadUtil::yield();
        nodeState = AtomicOp::getUintAcquire(&node.d_state);
        if (e_READABLE == nodeState) {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_pushMutex);

            nodeState = AtomicOp::testAndSwapUintAcqRel(
                                                       &node.d_state,
                                                       e_READABLE,
                                                       e_READABLE_AND_BLOCKED);

            while ((   e_READABLE             == nodeState
                    || e_READABLE_AND_BLOCKED == nodeState)
                   && disabledGen ==
                         AtomicOp::getUintAcquire(&d_pushDisabledGeneration)) {
                int rv = d_pushCondition.wait(&d_pushMutex);
                if (rv) {
                    AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                    e_READABLE_AND_BLOCKED,
                                                    e_READABLE);
                    return e_FAILED;                                  // RETURN
                }
                nodeState = AtomicOp::getUint(&node.d_state);
            }

            // The following checks for disablement being the cause of exiting
            // the 'while' loop.

            if (   e_READABLE             == nodeState
                || e_READABLE_AND_BLOCKED == nodeState) {
                AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                e_READABLE_AND_BLOCKED,
                                                e_READABLE);
                return e_DISABLED;                                    // RETURN
            }
        }
    }

    bslalg::ScalarPrimitives::copyConstruct(node.d_value.address(),
                                            value,
                                            d_allocator_p);

    pushComplete(&node, index);

    return e_SUCCESS;
}

template <class TYPE>
int SingleProducerSingleConsumerBoundedQueue<TYPE>::pushBackImp(
                                                 bslmf::MovableRef<TYPE> value,
                                                 bool                    isTry)
{
    Uint64     index = AtomicOp::getUint64Acquire(&d_pushIndex);
    const Uint disabledGen =
                           AtomicOp::getUintAcquire(&d_pushDisabledGeneration);

    if (disabledGen & 1) {
        return e_DISABLED;                                            // RETURN
    }

    Node& node = d_pushElement_p[index];

    Uint nodeState = AtomicOp::getUintAcquire(&node.d_state);

    if (e_READABLE == nodeState) {
        if (isTry) {
            return e_FULL;                                            // RETURN
        }

        bslmt::ThreadUtil::yield();
        nodeState = AtomicOp::getUintAcquire(&node.d_state);
        if (e_READABLE == nodeState) {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_pushMutex);

            nodeState = AtomicOp::testAndSwapUintAcqRel(
                                                       &node.d_state,
                                                       e_READABLE,
                                                       e_READABLE_AND_BLOCKED);

            while ((   e_READABLE             == nodeState
                    || e_READABLE_AND_BLOCKED == nodeState)
                   && disabledGen ==
                         AtomicOp::getUintAcquire(&d_pushDisabledGeneration)) {
                int rv = d_pushCondition.wait(&d_pushMutex);
                if (rv) {
                    AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                    e_READABLE_AND_BLOCKED,
                                                    e_READABLE);
                    return e_FAILED;                                  // RETURN
                }
                nodeState = AtomicOp::getUint(&node.d_state);
            }

            if (   e_READABLE             == nodeState
                || e_READABLE_AND_BLOCKED == nodeState) {
                AtomicOp::testAndSwapUintAcqRel(&node.d_state,
                                                e_READABLE_AND_BLOCKED,
                                                e_READABLE);
                return e_DISABLED;                                    // RETURN
            }
        }
    }

    TYPE& dummy = value;
    bslalg::ScalarPrimitives::moveConstruct(node.d_value.address(),
                                            dummy,
                                            d_allocator_p);

    pushComplete(&node, index);

    return e_SUCCESS;
}

template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::pushComplete(
                                                                 Node   *node,
                                                                 Uint64  index)
{
    Uint nodeState = AtomicOp::swapUintAcqRel(&node->d_state, e_READABLE);
    if (e_WRITABLE_AND_BLOCKED == nodeState) {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_popMutex);
        }
        d_popCondition.signal();
    }

    ++index;
    if (index == d_pushCapacity) {
        index = 0;
    }
    AtomicOp::setUint64Release(&d_pushIndex, index);
}

// CREATORS
template <class TYPE>
SingleProducerSingleConsumerBoundedQueue<TYPE>::
     SingleProducerSingleConsumerBoundedQueue(bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_popElement_p(0)
, d_popCapacity(capacity > 0 ? capacity : 1)
, d_popPad()
, d_pushCapacity(capacity > 0 ? capacity : 1)
, d_pushPad()
, d_popMutex()
, d_popCondition()
, d_pushMutex()
, d_pushCondition()
, d_emptyMutex()
, d_emptyCondition()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    AtomicOp::initUint64(&d_popIndex,  0);
    AtomicOp::initUint64(&d_pushIndex, 0);

    AtomicOp::initUint(&d_popDisabledGeneration,  0);
    AtomicOp::initUint(&d_emptyCount,             0);
    AtomicOp::initUint(&d_emptyGeneration,        0);
    AtomicOp::initUint(&d_pushDisabledGeneration, 0);

    d_popElement_p = static_cast<Node *>(
                        d_allocator_p->allocate(d_popCapacity * sizeof(Node)));

    d_pushElement_p = d_popElement_p;

    for (bsl::size_t i = 0; i < d_popCapacity; ++i) {
        AtomicOp::initUint(&d_popElement_p[i].d_state, e_WRITABLE);
    }
}

template <class TYPE>
SingleProducerSingleConsumerBoundedQueue<TYPE>
                                  ::~SingleProducerSingleConsumerBoundedQueue()
{
    if (d_popElement_p) {
        removeAll();
        d_allocator_p->deallocate(d_popElement_p);
    }
}

// MANIPULATORS
template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::popFront(TYPE *value)
{
    return popFrontImp(value, false);
}

template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::pushBack(const TYPE& value)
{
    return pushBackImp(value, false);
}

template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::pushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    return pushBackImp(bslmf::MovableRefUtil::move(value), false);
}

template <class TYPE>
void SingleProducerSingleConsumerBoundedQueue<TYPE>::removeAll()
{
    Uint64 index     = AtomicOp::getUint64Acquire(&d_popIndex);
    Uint   nodeState = AtomicOp::getUintAcquire(
                                               &d_popElement_p[index].d_state);

    while (e_READABLE == nodeState || e_READABLE_AND_BLOCKED == nodeState) {
        d_popElement_p[index].d_value.object().~TYPE();

        AtomicOp::swapUintAcqRel(&d_popElement_p[index].d_state, e_WRITABLE);

        ++index;
        if (index == d_popCapacity) {
            index = 0;
        }

        nodeState = AtomicOp::getUintAcquire(&d_popElement_p[index].d_state);
    }

    AtomicOp::setUint64Release(&d_popIndex, index);

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_pushMutex);
    }
    d_pushCondition.signal();

    AtomicOp::addUintAcqRel(&d_emptyGeneration, 1);
    if (0 < AtomicOp::getUintAcquire(&d_emptyCount)) {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
        }
        d_emptyCondition.broadcast();
    }
}

template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::tryPopFront(TYPE *value)
{
    return popFrontImp(value, true);
}

template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::tryPushBack(
                                                             const TYPE& value)
{
    return pushBackImp(value, true);
}

template <class TYPE>
inline
int SingleProducerSingleConsumerBoundedQueue<TYPE>::tryPushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    return pushBackImp(bslmf::MovableRefUtil::move(value), true);
}

                       // Enqueue/Dequeue State

template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::disablePopFront()
{
    incrementUntil(&d_popDisabledGeneration, 1);

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_popMutex);
    }
    d_popCondition.broadcast();

    if (0 < AtomicOp::getUintAcquire(&d_emptyCount)) {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
        }
        d_emptyCondition.broadcast();
    }
}

template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::disablePushBack()
{
    incrementUntil(&d_pushDisabledGeneration, 1);

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_pushMutex);
    }
    d_pushCondition.broadcast();
}

template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::enablePopFront()
{
    incrementUntil(&d_popDisabledGeneration, 0);
}

template <class TYPE>
inline
void SingleProducerSingleConsumerBoundedQueue<TYPE>::enablePushBack()
{
    incrementUntil(&d_pushDisabledGeneration, 0);
}

// ACCESSORS
template <class TYPE>
inline
bool SingleProducerSingleConsumerBoundedQueue<TYPE>::isEmpty() const
{
    Node& node      = d_popElement_p[AtomicOp::getUint64Acquire(&d_popIndex)];
    Uint  nodeState = AtomicOp::getUintAcquire(&node.d_state);

    return e_WRITABLE == nodeState || e_WRITABLE_AND_BLOCKED == nodeState;
}

template <class TYPE>
inline
bool SingleProducerSingleConsumerBoundedQueue<TYPE>::isFull() const
{
    Node& node      = d_pushElement_p[AtomicOp::getUint64Acquire(
                                                                &d_pushIndex)];
    Uint  nodeState = AtomicOp::getUintAcquire(&node.d_state);

    return e_READABLE == nodeState || e_READABLE_AND_BLOCKED == nodeState;
}

template <class TYPE>
inline
bool SingleProducerSingleConsumerBoundedQueue<TYPE>::isPopFrontDisabled() const
{
    return 1 == (AtomicOp::getUintAcquire(&d_popDisabledGeneration) & 1);
}

template <class TYPE>
inline
bool SingleProducerSingleConsumerBoundedQueue<TYPE>::isPushBackDisabled() const
{
    return 1 == (AtomicOp::getUintAcquire(&d_pushDisabledGeneration) & 1);
}

template <class TYPE>
inline
bsl::size_t SingleProducerSingleConsumerBoundedQueue<TYPE>::numElements() const
{
    Uint64 popIndex  = AtomicOp::getUint64Acquire(&d_popIndex);
    Uint64 pushIndex = AtomicOp::getUint64Acquire(&d_pushIndex);
    Node&  node      = d_pushElement_p[pushIndex];
    Uint   nodeState = AtomicOp::getUintAcquire(&node.d_state);

    if (e_READABLE == nodeState || e_READABLE_AND_BLOCKED == nodeState) {
        return d_popCapacity;                                         // RETURN
    }

    return pushIndex >= popIndex
         ? pushIndex - popIndex
         : pushIndex + d_popCapacity - popIndex;
}

template <class TYPE>
int SingleProducerSingleConsumerBoundedQueue<TYPE>::waitUntilEmpty() const
{
    AtomicOp::addUintAcqRel(&d_emptyCount, 1);

    const Uint initEmptyGen = AtomicOp::getUintAcquire(&d_emptyGeneration);

    const Uint disabledGen =
                            AtomicOp::getUintAcquire(&d_popDisabledGeneration);

    if (disabledGen & 1) {
        AtomicOp::addUintAcqRel(&d_emptyCount, -1);
        return e_DISABLED;                                            // RETURN
    }

    if (isEmpty()) {
        AtomicOp::addUintAcqRel(&d_emptyCount, -1);
        return e_SUCCESS;                                             // RETURN
    }

    bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);

    Uint emptyGen = AtomicOp::getUintAcquire(&d_emptyGeneration);

    while (   initEmptyGen == emptyGen
           && disabledGen  ==
                          AtomicOp::getUintAcquire(&d_popDisabledGeneration)) {
        int rv = d_emptyCondition.wait(&d_emptyMutex);
        if (rv) {
            AtomicOp::addUintAcqRel(&d_emptyCount, -1);
            return e_FAILED;                                          // RETURN
        }
        emptyGen = AtomicOp::getUintAcquire(&d_emptyGeneration);
    }

    AtomicOp::addUintAcqRel(&d_emptyCount, -1);

    if (initEmptyGen == emptyGen) {
        return e_DISABLED;                                            // RETURN
    }

    return e_SUCCESS;
}

                                  // Aspects

template <class TYPE>
inline
bslma::Allocator *SingleProducerSingleConsumerBoundedQueue<TYPE>::allocator()
                                                                          const
{
    return d_allocator_p;
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
