// bdlcc_boundedqueue.h                                               -*-C++-*-

#ifndef INCLUDED_BDLCC_BOUNDEDQUEUE
#define INCLUDED_BDLCC_BOUNDEDQUEUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-aware bounded queue of values.
//
//@CLASSES:
//  bdlcc::BoundedQueue: thread-aware bounded queue of 'TYPE'
//
//@SEE_ALSO: bdlcc_fixedqueue
//
//@DESCRIPTION: This component defines a type, 'bdlcc::BoundedQueue', that
// provides an efficient, thread-aware bounded (capacity fixed at construction)
// queue of values.  This class is ideal for synchronization and communication
// between threads in a producer-consumer model when a bounded queue is
// appropriate.   Under most cicrumstances developers should prefer
// this component to the older {bdlcc_fixedqueue} (see {Comparison to
// FixedQueue}).
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
// 'disablePopFront' method.  When dequeue disabled, 'popFront' and
// 'tryPopFront' fail immediately and return an error code.  Any threads
// blocked in 'popFront' when the queue is dequeue disabled return from
// 'popFront' immediately and return an error code.  The queue may be restored
// to normal operation with the 'enablePopFront' method.
//
///Comparison To FixedQueue
///------------------------
// Both 'bdlcc::FixedQueue' and 'bdlcc::BoundedQueue' provide thread-aware
// bounded queues.  Under most circumstances developers should prefer
// {bdlcc_boundedqueue}: it is newer, has additional features, and provides
// better performance under most circumstances.  'bdlcc::BoundedQueue' is not
// quite a drop in replacement for 'bdlcc::FixedQueue' so both types are
// currently maintained.  There is additional information about
// performance of various queues in the article Concurrent Queue Evaluation
// (https://tinyurl.com/mr2un9f7).
//
///Template Requirements
///---------------------
// 'bdlcc::BoundedQueue' is a template that is parameterized on the type of
// element contained within the queue.  The supplied template argument, 'TYPE',
// must provide both a default constructor and a copy constructor, as well as
// an assignment operator.  If the default constructor accepts a
// 'bslma::Allocator *', 'TYPE' must declare the uses 'bslma::Allocator' trait
// (see 'bslma_usesbslmaallocator') so that the allocator of the queue is
// propagated to the elements contained in the queue.
//
///Exception Safety
///----------------
// A 'bdlcc::BoundedQueue' is exception neutral, and all of the methods of
// 'bdlcc::BoundedQueue' provide the basic exception safety guarantee (see
// 'bsldoc_glossary').  If an exception occurs while writing to an element, the
// element is marked unusable until after a read attempt from the element (at
// which point the element is "reclaimed").  This failure to write does not
// increment the result returned by 'numElements'.  Hence,
// 'numElements() == capacity()' is not a valid replacement for 'isFull()'.
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlcc::BoundedQueue' on C++11 platforms
// only (where 'BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES' is defined), and are
// not supported on C++03 platforms.  Unfortunately, in C++03, there are user
// types where a 'bslmf::MovableRef' will not safely degrade to a lvalue
// reference when a move constructor is not available (types providing a
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
// In the following example a 'bdlcc::BoundedQueue' is used to communicate
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
//  void myDoWork(const my_WorkData& data)
//      // Do some work based upon the specified 'data'.
//  {
//      // do some stuff...
//      (void)data;
//  }
//..
// Then, we define a 'myConsumer' function that will pop elements off the queue
// and process them.  Note that the call to 'queue->popFront()' will block
// until there is an element available on the queue.  This function will be
// executed in multiple threads, so that each thread waits in
// 'queue->popFront()', and 'bdlcc::BoundedQueue' guarantees that each thread
// gets a unique element from the queue:
//..
//  void myConsumer(bdlcc::BoundedQueue<my_WorkRequest> *queue)
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
// creates the 'bdlcc::BoundedQueue', starts the consumer threads, and then
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
//      // Create a queue, start the specified 'numThreads' consumer threads,
//      // produce and enqueue work.
//  {
//      enum {
//          k_MAX_QUEUE_LENGTH = 100,
//          k_NUM_WORK_ITEMS   = 1000
//      };
//
//      bdlcc::BoundedQueue<my_WorkRequest> queue(k_MAX_QUEUE_LENGTH);
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

#include <bdlb_bitutil.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_usesbslmaallocator.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_condition.h>
#include <bslmt_fastpostsemaphore.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace bdlcc {

                   // ===================================
                   // class BoundedQueue_PopCompleteGuard
                   // ===================================

template <class TYPE, class NODE>
class BoundedQueue_PopCompleteGuard {
    // This class implements a guard that invokes 'TYPE::popComplete' on a
    // 'NODE' upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue owning the managed node
    NODE *d_node_p;   // managed node

    // NOT IMPLEMENTED
    BoundedQueue_PopCompleteGuard();
    BoundedQueue_PopCompleteGuard(const BoundedQueue_PopCompleteGuard&);
    BoundedQueue_PopCompleteGuard& operator=(
                                         const BoundedQueue_PopCompleteGuard&);

  public:
    // CREATORS
    BoundedQueue_PopCompleteGuard(TYPE *queue, NODE *node);
        // Create a 'popComplete' guard managing the specified 'queue' and
        // 'node'.

    ~BoundedQueue_PopCompleteGuard();
        // Destroy this object and invoke the 'TYPE::popComplete' method with
        // the managed 'node'.
};

             // ===============================================
             // class BoundedQueue_PushExceptionCompleteProctor
             // ===============================================

template <class TYPE>
class BoundedQueue_PushExceptionCompleteProctor {
    // This class implements a proctor that invokes
    // 'TYPE::pushExceptionComplete' upon destruction unless 'release' has been
    // called.

    // DATA
    TYPE *d_queue_p;  // managed queue

    // NOT IMPLEMENTED
    BoundedQueue_PushExceptionCompleteProctor();
    BoundedQueue_PushExceptionCompleteProctor(
                             const BoundedQueue_PushExceptionCompleteProctor&);
    BoundedQueue_PushExceptionCompleteProctor& operator=(
                             const BoundedQueue_PushExceptionCompleteProctor&);

  public:
    // CREATORS
    explicit
    BoundedQueue_PushExceptionCompleteProctor(TYPE *queue);
        // Create a 'pushExceptionComplete' proctor that conditionally manages
        // the specified 'queue' (if non-zero).

    ~BoundedQueue_PushExceptionCompleteProctor();
        // Destroy this object and, if 'release' has not been invoked', invoke
        // the managed queue's 'pushExceptionComplete' method.

    // MANIPULATORS
    void release();
        // Release from management the queue currently managed by this proctor.
        // If no queue is currently managed, this method has no effect.
};

                         // ========================
                         // struct BoundedQueue_Node
                         // ========================

template <class TYPE, bool RECLAIMABLE>
struct BoundedQueue_Node;
    // This class implements the queue's node.  A node stores an instance of
    // the specified (template parameter) 'TYPE', and provides an accessor
    // 'isUnconstructed' that indicates whether the value of the node was
    // correctly constructed.  If 'isUnconstructed' is 'false', then the value
    // ('d_value') refers to a valid object.  If 'isUnconstructed' is 'true'
    // then 'd_value' does not refer to a valid object, it does not represent a
    // value in this queue, and the destructor of 'd_value' should not be
    // called.  The specified (template parameter) type 'RECLAIMABLE' is used
    // to provide a compile time optimization for the footprint of this
    // template when the value of 'isUnconstructed' is known at compile-time.
    // If 'RECLAIMABLE' is 'false' then it can be determined at compile time
    // that the construction of 'TYPE' will uncoditionally succeed (e.g., it
    // 'is_trivially_copyable'), and the 'isUnconstructed' property does not
    // require a data member to be accessed at run-time.

template <class TYPE>
struct BoundedQueue_Node<TYPE, true> {
  private:
    // DATA
    bool                     d_isUnconstructedFlag;  // node suffered exception

  public:
    // PUBLIC DATA
    bsls::ObjectBuffer<TYPE> d_value;                // stored value

    // MANIPULATORS
    void setIsUnconstructed(bool isUnconstructedFlag);
        // If the specified 'isUnconstructedFlag' is 'false', then 'd_value'
        // refers to a valid object of (the template parameter) 'TYPE',
        // otherwise (if 'isUnconstrucedFlag' is 'true') 'd_value' does not
        // refer to a valid object (because the attempt to construct 'TYPE'
        // resulted in an expection).  Note that this method is normally
        // invoked after each write to 'd_value'.

    // ACCESSORS
    bool isUnconstructed() const;
        // Return whether an exception occurred when last writing to 'd_value'.
};

template <class TYPE>
struct BoundedQueue_Node<TYPE, false> {
    // PUBLIC DATA
    bsls::ObjectBuffer<TYPE> d_value;    // stored value

    // MANIPULATORS
    void setIsUnconstructed(bool /* value */);
        // Do nothing.

    // ACCESSORS
    bool isUnconstructed() const;
        // Return 'false'.
};

                            // ==================
                            // class BoundedQueue
                            // ==================

template <class TYPE>
class BoundedQueue {
    // This class provides a thread-safe bounded queue of values.

    // PRIVATE CONSTANTS

    // The following constants are used to maintain the queue's 'd_popCount'
    // and 'd_pushCount' values.  See *Implementation* *Note* for details.

    static const bsls::Types::Uint64 k_STARTED_MASK   = 0x00000000ffffffffLL;
    static const bsls::Types::Uint64 k_STARTED_INC    = 0x0000000000000001LL;
    static const bsls::Types::Uint64 k_STARTED_DEC    = 0xffffffffffffffffLL;
    static const bsls::Types::Uint64 k_FINISHED_INC   = 0x0000000100000000LL;
    static const unsigned int        k_FINISHED_SHIFT = 32;

    static const unsigned int k_MAXIMUM_CIRCULAR_DIFFERENCE =
         static_cast<unsigned int>(1) << (sizeof(unsigned int) * CHAR_BIT - 1);

    // PRIVATE TYPES
    typedef unsigned int                                         Uint;
    typedef typename bsls::Types::Uint64                         Uint64;
    typedef typename bsls::AtomicOperations::AtomicTypes::Uint   AtomicUint;
    typedef typename bsls::AtomicOperations::AtomicTypes::Uint64 AtomicUint64;

    typedef typename bsls::AtomicOperations AtomicOp;

    typedef BoundedQueue_Node<TYPE,
                              !bsl::is_trivially_copyable<TYPE>::value> Node;

    // DATA
    bslmt::FastPostSemaphore  d_pushSemaphore;     // synchronization primitive
                                                   // restricting access to
                                                   // empty elements and
                                                   // providing
                                                   // enablement/disablement of
                                                   // "push" operations

    AtomicUint64              d_pushCount;         // count of "push"
                                                   // operations started and
                                                   // completed, used to detect
                                                   // a quiescent state for
                                                   // safely incrementing
                                                   // number of available
                                                   // elements

    AtomicUint64              d_pushIndex;         // index of next enqueue
                                                   // element location

    bslmt::FastPostSemaphore  d_popSemaphore;      // synchronization primitive
                                                   // restricting access to
                                                   // available elements and
                                                   // providing
                                                   // enablement/disablement of
                                                   // "pop" operations

    AtomicUint64              d_popCount;          // count of "pop" operations
                                                   // started and completed,
                                                   // used to detect a
                                                   // quiescent state for
                                                   // safely incrementing
                                                   // number of empty elements

    AtomicUint64              d_popIndex;          // index of next dequeue
                                                   // element location

    mutable AtomicUint        d_emptyWaiterCount;  // circular count of
                                                   // 'waitUntilEmpty'
                                                   // invocations

    AtomicUint                d_emptyCountSeen;    // maximum
                                                   // 'd_emptyWaiterCount' seen
                                                   // prior to the queue being
                                                   // observed as empty; used
                                                   // to detect most very short
                                                   // lived transitions to the
                                                   // queue being empty

    mutable bslmt::Mutex      d_emptyMutex;        // blocking point for
                                                   // 'waitUntilEmpty'

    mutable bslmt::Condition  d_emptyCondition;    // condition variable for
                                                   // 'waitUntilEmpty'

    Node                     *d_element_p;         // array of elements that
                                                   // comprise the bounded
                                                   // queue

    Uint64                    d_capacity;          // capacity of the queue

    bslma::Allocator         *d_allocator_p;       // allocator, held not owned

    // FRIENDS
    friend class BoundedQueue_PopCompleteGuard<
                                            BoundedQueue<TYPE>,
                                            typename BoundedQueue<TYPE>::Node>;

    friend class BoundedQueue_PushExceptionCompleteProctor<
                                                          BoundedQueue<TYPE> >;

    // PRIVATE CLASS METHODS
    static bool circularlyGreater(Uint lhs, Uint rhs);
        // Return 'true' if the specified 'lhs' is circularly greater than the
        // specified 'rhs', and 'false' otherwise.  'lhs' is cicularly greater
        // than 'rhs' if 'lhs' is equal to a value obtained by adding a value
        // in '[1 .. 2^31]' to 'rhs'.

    static bool isQuiescentState(bsls::Types::Uint64 count);
        // Return 'true' if the specified 'count' implies a quiescent state
        // (see *Implementation* *Note*), and 'false' otherwise.  A quiescent
        // state indicates there is a (possibly zero length) contiguous set of
        // elements that can safely be made available to the operation
        // complementary to the operation 'count' tracks (pop and push are
        // complementary operations).

    static Uint64 markFinishedOperation(AtomicUint64 *count);
    static Uint64 markFinishedOperation(AtomicUint64 *count, int num);
        // Update the specified 'count' to indicate that an operation (or the
        // optionally specified 'num' operations) has completed, and return the
        // updated 'count' value.  Marking an operation finished means that
        // 'isQuiscentState' *may* now be true.  The behavior is undefined
        // unless 'markStartedOperation' was previously called on this 'count',
        // and a corresponding 'markFinishedOperation' or
        // 'unmarkStartOperation' has not already been called.  Note that the
        // "operation" that has finished refers to either a push or pop
        // (depending on whether this is applied to 'd_pushCount' or
        // 'd_popCount').

    static void markReclaimed(AtomicUint64 *count);
        // Update the specified 'count' to indicate that a node that suffered
        // an exception has been reclaimed.  Marking a node reclaimed can *not*
        // alter the value of 'isQuiescentState', but does increase the size of
        // the contiguous set of elements that will eventially be made
        // available to the operation complementary to the operation 'count'
        // tracks (pop and push are complementary operations).  Note that this
        // method does not require a previous call to 'markStartedOperation'
        // and does not meet the requirements for 'markFinishedOperation' or
        // 'unmarkStartOperation' to be invoked.

    static void markStartedOperation(AtomicUint64 *count);
    static void markStartedOperation(AtomicUint64 *count, int num);
        // Update the specified 'count' to indicate that an operation (or the
        // optionally specified 'num' operations) has started, and return the
        // updated 'count' value.  Marking an operation started means that
        // 'isQuiescentState' is not true and will not be true until
        // 'markFinishedOperation' or 'unmarkStartedOperation' is invoked.
        // Note that the "operation" that has started refers to either a push
        // or pop (depending on whether this is applied to 'd_pushCount' or
        // 'd_popCount').

    static Uint64 unmarkStartedOperation(AtomicUint64 *count);
        // Update the specified 'count' to indicate that an operation has
        // aborted without finishing, and return the updated 'count' value.
        // Marking a started operation as having aborted means that
        // 'isQuiscentState' *may* now be true.  The behavior is undefined
        // unless 'markStartedOperation' was previously called on this 'count',
        // and a corresponding 'markFinishedOperation' or
        // 'unmarkStartOperation' has not already been called.  Note that the
        // "operation" that has aborted refers to either a push or pop
        // (depending on whether this is applied to 'd_pushCount' or
        // 'd_popCount').

    // PRIVATE MANIPULATORS
    void popComplete(Node *node);
        // Destruct the value stored in the specified 'node', and mark the
        // 'node' writable.  This method is used within 'popFrontHelper' by a
        // guard to complete the reclamation of a node in the presence of an
        // exception.

    void popFrontHelper(TYPE *value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  This method is invoked by
        // 'popFront' and 'tryPopFront' once an element is available.

    void pushComplete();
        // Mark a "push" operation as complete, and 'post' to the
        // 'd_popSemaphore' if appropriate.

    void pushExceptionComplete();
        // Remove the indicator for a started push operation, and 'post' to the
        // 'd_popSemaphore' if appropriate.  This method is used within
        // 'pushFront' by a proctor to complete the marking of a node to
        // reclaim in the presence of an exception.

    bool updateEmptyCountSeen(Uint emptyCount);
        // If the specified 'emptyCount' is (circularly) greater than
        // 'd_emptyCountSeen', assign 'd_emptyCountSeen' the value of
        // 'emptyCount' and return 'true'.  Otherwise, return 'false'.  A
        // return value of 'true' indicates this thread *must* signal any
        // waiting threads.  Note that a return value of 'false' indicates
        // another thread has (or will) signal the queue is empty and this
        // thread does not need to signal.

    // NOT IMPLEMENTED
    BoundedQueue(const BoundedQueue&);
    BoundedQueue& operator=(const BoundedQueue&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BoundedQueue, bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS  =  0,  // must be 0
        e_EMPTY    = -1,
        e_FULL     = -2,
        e_DISABLED = -3,
        e_FAILED   = -4
    };

    // CREATORS
    explicit
    BoundedQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with at least the specified 'capacity'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~BoundedQueue();
        // Destroy this object.

    // MANIPULATORS
    int popFront(TYPE *value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_SUCCESS' on success,
        // 'e_DISABLED' if 'isPopFrontDisabled()' and 'e_FAILED' if an error
        // occurs.  On failure, 'value' is not changed.  Threads blocked due to
        // the queue being empty will return 'e_DISABLED' if 'disablePopFront'
        // is invoked.

    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  If the
        // queue is full, block until it is not full.  Return 0 on success, and
        // a non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if 'isPushBackDisabled()' and 'e_FAILED' if an
        // error occurs.  Threads blocked due to the queue being full will
        // return 'e_DISABLED' if 'disablePushBack' is invoked.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  If the queue is full, block until it is not full.  'value'
        // is left in a valid but unspecified state.  Return 0 on success, and
        // a non-zero value otherwise.  Specifically, return 'e_SUCCESS' on
        // success, 'e_DISABLED' if 'isPushBackDisabled()' and 'e_FAILED' if an
        // error occurs.  On failure, 'value' is not changed.  Threads blocked
        // due to the queue being full will return 'e_DISABLED' if
        // 'disablePushBack' is invoked.

    void removeAll();
        // Remove all items currently in this queue.  Note that this operation
        // is not atomic; if other threads are concurrently pushing items into
        // the queue the result of 'numElements()' after this function returns
        // is not guaranteed to be 0.

    int tryPopFront(TYPE *value);
        // Attempt to remove the element from the front of this queue without
        // blocking, and, if successful, load the specified 'value' with the
        // removed element.  Return 0 on success, and a non-zero value
        // otherwise.  Specifically, return 'e_SUCCESS' on success,
        // 'e_DISABLED' if 'isPopFrontDisabled()', 'e_EMPTY' if
        // '!isPopFrontDisabled()' and the queue was empty, and 'e_FAILED' if
        // an error occurs.  On failure, 'value' is not changed.

    int tryPushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()',
        // 'e_FULL' if '!isPushBackDisabled()' and the queue was full, and
        // 'e_FAILED' if an error occurs.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if 'isPushBackDisabled()',
        // 'e_FULL' if '!isPushBackDisabled()' and the queue was full, and
        // 'e_FAILED' if an error occurs.  On failure, 'value' is not changed.

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

    void enablePopFront();
        // Enable dequeueing.  If the queue is not dequeue disabled, this call
        // has no effect.

    void enablePushBack();
        // Enable queuing.  If the queue is not enqueue disabled, this call has
        // no effect.

    // ACCESSORS
    bsl::size_t capacity() const;
        // Return the maximum number of elements that may be stored in this
        // queue.  Note that the value returned may be greater than that
        // supplied at construction.

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
        // Returns the number of elements currently in this queue.  Note that
        // 'numElements() == capacity()' is not a valid replacement for
        // 'isFull' (see {Exception Safety} for details).

    int waitUntilEmpty() const;
        // Block until all the elements in this queue are removed.  Return 0 on
        // success, and a non-zero value otherwise.  Specifically, return
        // 'e_SUCCESS' on success, 'e_DISABLED' if
        // '!isEmpty() && isPopFrontDisabled()'.  A blocked thread waiting for
        // the queue to empty will return 'e_DISABLED' if 'disablePopFront' is
        // invoked.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                   // -----------------------------------
                   // class BoundedQueue_PopCompleteGuard
                   // -----------------------------------

// CREATORS
template <class TYPE, class NODE>
inline
BoundedQueue_PopCompleteGuard<TYPE, NODE>::
                         BoundedQueue_PopCompleteGuard(TYPE *queue, NODE *node)
: d_queue_p(queue)
, d_node_p(node)
{
}

template <class TYPE, class NODE>
inline
BoundedQueue_PopCompleteGuard<TYPE, NODE>::~BoundedQueue_PopCompleteGuard()
{
    d_queue_p->popComplete(d_node_p);
}

             // -----------------------------------------------
             // class BoundedQueue_PushExceptionCompleteProctor
             // -----------------------------------------------

// CREATORS
template <class TYPE>
inline
BoundedQueue_PushExceptionCompleteProctor<TYPE>::
                         BoundedQueue_PushExceptionCompleteProctor(TYPE *queue)
: d_queue_p(queue)
{
}

template <class TYPE>
inline
BoundedQueue_PushExceptionCompleteProctor<TYPE>::
                                   ~BoundedQueue_PushExceptionCompleteProctor()
{
    if (d_queue_p) {
        d_queue_p->pushExceptionComplete();
    }
}

// MANIPULATORS
template <class TYPE>
inline
void BoundedQueue_PushExceptionCompleteProctor<TYPE>::release()
{
    d_queue_p = 0;
}

                         // ------------------------
                         // struct BoundedQueue_Node
                         // ------------------------

// MANIPULATORS
template <class TYPE>
inline
void BoundedQueue_Node<TYPE, true>::setIsUnconstructed(
                                                      bool isUnconstructedFlag)
{
    d_isUnconstructedFlag = isUnconstructedFlag;
}

template <class TYPE>
inline
void BoundedQueue_Node<TYPE, false>::setIsUnconstructed(
                                                 bool /* isUnconstrutedFlag */)
{
}

// ACCESSORS
template <class TYPE>
inline
bool BoundedQueue_Node<TYPE, true>::isUnconstructed() const
{
    return d_isUnconstructedFlag;
}

template <class TYPE>
inline
bool BoundedQueue_Node<TYPE, false>::isUnconstructed() const
{
    return false;
}

                            // ------------------
                            // class BoundedQueue
                            // ------------------

// PRIVATE CLASS METHODS
template <class TYPE>
inline
bool BoundedQueue<TYPE>::circularlyGreater(Uint lhs, Uint rhs)
{
    return lhs > rhs ? (lhs - rhs) <= k_MAXIMUM_CIRCULAR_DIFFERENCE
                     : (rhs - lhs) >  k_MAXIMUM_CIRCULAR_DIFFERENCE;
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::isQuiescentState(bsls::Types::Uint64 count)
{
    return (count >> k_FINISHED_SHIFT) == (count & k_STARTED_MASK);
}

template <class TYPE>
inline
bsls::Types::Uint64 BoundedQueue<TYPE>::markFinishedOperation(
                                                           AtomicUint64 *count)
{
    return AtomicOp::addUint64NvAcqRel(count, k_FINISHED_INC);
}

template <class TYPE>
inline
bsls::Types::Uint64 BoundedQueue<TYPE>::markFinishedOperation(
                                                           AtomicUint64 *count,
                                                           int           num)
{
    return AtomicOp::addUint64NvAcqRel(count, num * k_FINISHED_INC);
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::markReclaimed(AtomicUint64 *count)
{
    AtomicOp::addUint64AcqRel(count, k_STARTED_INC + k_FINISHED_INC);
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::markStartedOperation(AtomicUint64 *count)
{
    AtomicOp::addUint64AcqRel(count, k_STARTED_INC);
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::markStartedOperation(AtomicUint64 *count, int num)
{
    AtomicOp::addUint64AcqRel(count, num * k_STARTED_INC);
}

template <class TYPE>
inline
bsls::Types::Uint64 BoundedQueue<TYPE>::unmarkStartedOperation(
                                                           AtomicUint64 *count)
{
    return AtomicOp::addUint64NvAcqRel(count, k_STARTED_DEC);
}

// PRIVATE MANIPULATORS
template <class TYPE>
inline
void BoundedQueue<TYPE>::popComplete(Node *node)
{
    node->d_value.object().~TYPE();

    Uint64 count = markFinishedOperation(&d_popCount);
    if (isQuiescentState(count)) {

        // The total number of popped elements is 'count & k_STARTED_MASK'.
        // Attempt, once, to zero the count and, if successful, post to the
        // push semaphore.

        if (AtomicOp::testAndSwapUint64AcqRel(&d_popCount,
                                              count,
                                              0) == count) {
            d_pushSemaphore.post(static_cast<int>(count & k_STARTED_MASK));

            Uint emptyCount = AtomicOp::getUintAcquire(&d_emptyWaiterCount);

            if (isEmpty() && updateEmptyCountSeen(emptyCount)) {
                {
                    bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
                }
                d_emptyCondition.broadcast();
            }
        }
    }
}

template <class TYPE>
void BoundedQueue<TYPE>::popFrontHelper(TYPE *value)
{
    markStartedOperation(&d_popCount);

    // 'd_popIndex' stores the next location to use (want the original value)

    Uint64  index = (AtomicOp::addUint64NvAcqRel(&d_popIndex, 1) - 1)
                                                                  % d_capacity;
    Node   *node  = &d_element_p[index];

    // Nodes marked for reclamation are not counted in 'd_popSemaphore' and are
    // to be skipped; 'd_isUnconstructed' does not need to be modified here
    // since it will be updated in a "push" operation.  However, the node does
    // need to be counted in the 'd_pushSemaphore' as an empty node.  This is
    // accomplished by incrementing the started and finished attributes in
    // 'd_popCount'.

    while (node->isUnconstructed()) {
        markReclaimed(&d_popCount);

        index = (AtomicOp::addUint64NvAcqRel(&d_popIndex, 1) - 1) % d_capacity;
        node  = &d_element_p[index];
    }

    BoundedQueue_PopCompleteGuard<BoundedQueue<TYPE>, Node> guard(this, node);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    *value = bslmf::MovableRefUtil::move(node->d_value.object());
#else
    *value = node->d_value.object();
#endif
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::pushComplete()
{
    Uint64 count = markFinishedOperation(&d_pushCount);
    if (isQuiescentState(count)) {

        // The total number of pushed elements is 'count & k_STARTED_MASK'.
        // Attempt, once, to zero the count and, if successful, post to the pop
        // semaphore.

        if (AtomicOp::testAndSwapUint64AcqRel(&d_pushCount,
                                               count,
                                               0) == count) {
            d_popSemaphore.post(static_cast<int>(count & k_STARTED_MASK));
        }
    }
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::pushExceptionComplete()
{
    Uint64 count = unmarkStartedOperation(&d_pushCount);

    int numToPost = static_cast<int>(count & k_STARTED_MASK);

    if (0 != numToPost && isQuiescentState(count)) {

        // The total number of pushed elements is 'count & k_STARTED_MASK'.
        // Attempt, once, to zero the count and, if successful, post to the pop
        // semaphore.

        if (AtomicOp::testAndSwapUint64AcqRel(&d_pushCount,
                                               count,
                                               0) == count) {
            d_popSemaphore.post(numToPost);
        }
    }
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::updateEmptyCountSeen(Uint emptyCount)
{
    Uint emptyCountSeen = AtomicOp::getUintAcquire(&d_emptyCountSeen);
    while (circularlyGreater(emptyCount, emptyCountSeen)) {
        const Uint origEmptyCountSeen = emptyCountSeen;

        emptyCountSeen = AtomicOp::testAndSwapUintAcqRel(&d_emptyCountSeen,
                                                         emptyCountSeen,
                                                         emptyCount);

        if (origEmptyCountSeen == emptyCountSeen) {
            return true;                                              // RETURN
        }
    }
    return false;
}

// CREATORS
template <class TYPE>
BoundedQueue<TYPE>::BoundedQueue(bsl::size_t       capacity,
                                 bslma::Allocator *basicAllocator)
: d_pushSemaphore()
, d_popSemaphore()
, d_emptyMutex()
, d_emptyCondition()
, d_element_p(0)
, d_capacity(capacity > 2 ? capacity : 2)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    AtomicOp::initUint64(&d_pushCount, 0);
    AtomicOp::initUint64(&d_pushIndex, 0);
    AtomicOp::initUint64(&d_popCount,  0);
    AtomicOp::initUint64(&d_popIndex,  0);

    AtomicOp::initUint(&d_emptyWaiterCount, 0);
    AtomicOp::initUint(&d_emptyCountSeen,   0);

    d_element_p = static_cast<Node *>(
                              d_allocator_p->allocate(static_cast<bsl::size_t>(
                                                  d_capacity * sizeof(Node))));

    for (bsl::size_t i = 0; i < d_capacity; ++i) {
        d_element_p[i].setIsUnconstructed(false);
    }

    d_pushSemaphore.post(static_cast<int>(d_capacity));
}

template <class TYPE>
BoundedQueue<TYPE>::~BoundedQueue()
{
    if (d_element_p) {
        removeAll();
        d_allocator_p->deallocate(d_element_p);
    }
}

// MANIPULATORS
template <class TYPE>
inline
int BoundedQueue<TYPE>::popFront(TYPE *value)
{
    int rv = d_popSemaphore.wait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    popFrontHelper(value);

    return e_SUCCESS;
}

template <class TYPE>
int BoundedQueue<TYPE>::pushBack(const TYPE& value)
{
    int rv = d_pushSemaphore.wait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    markStartedOperation(&d_pushCount);

    // 'd_pushIndex' stores the next location to use (want the original value)

    Uint64 index = (AtomicOp::addUint64NvAcqRel(&d_pushIndex, 1) - 1)
                                                                  % d_capacity;
    Node&  node  = d_element_p[index];

    node.setIsUnconstructed(true);

    BoundedQueue_PushExceptionCompleteProctor<BoundedQueue<TYPE> > guard(this);

    bslalg::ScalarPrimitives::copyConstruct(node.d_value.address(),
                                            value,
                                            d_allocator_p);

    guard.release();

    node.setIsUnconstructed(false);

    pushComplete();

    return e_SUCCESS;
}

template <class TYPE>
int BoundedQueue<TYPE>::pushBack(bslmf::MovableRef<TYPE> value)
{
    int rv = d_pushSemaphore.wait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    markStartedOperation(&d_pushCount);

    // 'd_pushIndex' stores the next location to use (want the original value)

    Uint64 index = (AtomicOp::addUint64NvAcqRel(&d_pushIndex, 1) - 1)
                                                                  % d_capacity;
    Node&  node  = d_element_p[index];

    node.setIsUnconstructed(true);

    BoundedQueue_PushExceptionCompleteProctor<BoundedQueue<TYPE> > guard(this);

    TYPE& dummy = value;
    bslalg::ScalarPrimitives::moveConstruct(node.d_value.address(),
                                            dummy,
                                            d_allocator_p);

    guard.release();

    node.setIsUnconstructed(false);

    pushComplete();

    return e_SUCCESS;
}

template <class TYPE>
void BoundedQueue<TYPE>::removeAll()
{
    int reclaim = d_popSemaphore.takeAll();

    if (reclaim) {
        while (reclaim) {
            int count = reclaim;

            reclaim = 0;

            // For quiescent state detection (see *Implementation* *Note*) and
            // eventual 'post' to the 'd_pushSemaphore' to indicate node
            // availability, indicate 'count' remove operations have begin.

            markStartedOperation(&d_popCount, count);

            // 'd_popIndex' stores the next location to use (want the original
            // value)

            Uint64 index = AtomicOp::addUint64NvAcqRel(&d_popIndex, count)
                                                                       - count;

            for (int i = 0; i < count; ++i, ++index) {
                Node& node = d_element_p[index % d_capacity];

                if (!node.isUnconstructed()) {
                    node.d_value.object().~TYPE();
                }
                else {
                    ++reclaim;
                }
            }

            // For quiescent state detection (see *Implementation* *Note*) and
            // eventual 'post' to the 'd_pushSemaphore' to indicate node
            // availability, indicate 'count' remove operations have finished.

            Uint64 popCount = markFinishedOperation(&d_popCount, count);

            if (isQuiescentState(popCount)) {
                // The total number of popped elements is
                // 'popCount & k_STARTED_MASK'.  Attempt, once, to zero the
                // count and, if successful, post to the push semaphore.

                if (AtomicOp::testAndSwapUint64AcqRel(&d_popCount,
                                                      popCount,
                                                      0) == popCount) {
                    d_pushSemaphore.post(static_cast<int>(
                                                   popCount & k_STARTED_MASK));
                }

                Uint emptyCount = AtomicOp::getUintAcquire(
                                                          &d_emptyWaiterCount);

                if (isEmpty() && updateEmptyCountSeen(emptyCount)) {
                    {
                        bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
                    }
                    d_emptyCondition.broadcast();
                }
            }
        }
    }
}

template <class TYPE>
inline
int BoundedQueue<TYPE>::tryPopFront(TYPE *value)
{
    int rv = d_popSemaphore.tryWait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        if (bslmt::FastPostSemaphore::e_WOULD_BLOCK == rv) {
            return e_EMPTY;                                           // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    popFrontHelper(value);

    return e_SUCCESS;
}

template <class TYPE>
int BoundedQueue<TYPE>::tryPushBack(const TYPE& value)
{
    int rv = d_pushSemaphore.tryWait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        if (bslmt::FastPostSemaphore::e_WOULD_BLOCK == rv) {
            return e_FULL;                                            // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    markStartedOperation(&d_pushCount);

    // 'd_pushIndex' stores the next location to use (want the original value)

    Uint64 index = (AtomicOp::addUint64NvAcqRel(&d_pushIndex, 1) - 1)
                                                                  % d_capacity;
    Node&  node  = d_element_p[index];

    node.setIsUnconstructed(true);

    BoundedQueue_PushExceptionCompleteProctor<BoundedQueue<TYPE> > guard(this);

    bslalg::ScalarPrimitives::copyConstruct(node.d_value.address(),
                                            value,
                                            d_allocator_p);

    guard.release();

    node.setIsUnconstructed(false);

    pushComplete();

    return e_SUCCESS;
}

template <class TYPE>
int BoundedQueue<TYPE>::tryPushBack(bslmf::MovableRef<TYPE> value)
{
    int rv = d_pushSemaphore.tryWait();
    if (rv) {
        if (bslmt::FastPostSemaphore::e_DISABLED == rv) {
            return e_DISABLED;                                        // RETURN
        }
        if (bslmt::FastPostSemaphore::e_WOULD_BLOCK == rv) {
            return e_FULL;                                            // RETURN
        }
        return e_FAILED;                                              // RETURN
    }

    markStartedOperation(&d_pushCount);

    // 'd_pushIndex' stores the next location to use (want the original value)

    Uint64 index = (AtomicOp::addUint64NvAcqRel(&d_pushIndex, 1) - 1)
                                                                  % d_capacity;
    Node&  node  = d_element_p[index];

    node.setIsUnconstructed(true);

    BoundedQueue_PushExceptionCompleteProctor<BoundedQueue<TYPE> > guard(this);

    TYPE& dummy = value;
    bslalg::ScalarPrimitives::moveConstruct(node.d_value.address(),
                                            dummy,
                                            d_allocator_p);

    guard.release();

    node.setIsUnconstructed(false);

    pushComplete();

    return e_SUCCESS;
}

                       // Enqueue/Dequeue State

template <class TYPE>
inline
void BoundedQueue<TYPE>::disablePopFront()
{
    d_popSemaphore.disable();

    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
    }
    d_emptyCondition.broadcast();
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::disablePushBack()
{
    d_pushSemaphore.disable();
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::enablePopFront()
{
    d_popSemaphore.enable();
}

template <class TYPE>
inline
void BoundedQueue<TYPE>::enablePushBack()
{
    d_pushSemaphore.enable();
}

// ACCESSORS
template <class TYPE>
inline
bsl::size_t BoundedQueue<TYPE>::capacity() const
{
    return static_cast<bsl::size_t>(d_capacity);
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::isEmpty() const
{
    return d_capacity == static_cast<Uint64>(d_pushSemaphore.getValue());
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::isFull() const
{
    return 0 == d_pushSemaphore.getValue();
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::isPopFrontDisabled() const
{
    return d_popSemaphore.isDisabled();
}

template <class TYPE>
inline
bool BoundedQueue<TYPE>::isPushBackDisabled() const
{
    return d_pushSemaphore.isDisabled();
}

template <class TYPE>
inline
bsl::size_t BoundedQueue<TYPE>::numElements() const
{
    return d_popSemaphore.getValue();
}

template <class TYPE>
int BoundedQueue<TYPE>::waitUntilEmpty() const
{
    Uint emptyCount = AtomicOp::addUintNvAcqRel(&d_emptyWaiterCount, 1) - 1;

    int state = d_popSemaphore.getDisabledState();
    if (1 == (state & 1)) {
        return e_DISABLED;                                            // RETURN
    }

    if (isEmpty()) {
        return e_SUCCESS;                                             // RETURN
    }

    bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);

    // Return successfully when this queue is empty ('isEmpty()') or this queue
    // was empty at some point since this method was invoked (the condition
    // tested by 'circularlyGreater' in the below).

    bool empty = isEmpty()
              || circularlyGreater(AtomicOp::getUintAcquire(&d_emptyCountSeen),
                                   emptyCount);

    while (!empty && state == d_popSemaphore.getDisabledState()) {
        int rv = d_emptyCondition.wait(&d_emptyMutex);
        if (rv) {
            return e_FAILED;                                          // RETURN
        }
        empty = isEmpty()
             || circularlyGreater(AtomicOp::getUintAcquire(&d_emptyCountSeen),
                                  emptyCount);
    }

    if (!empty) {
        return e_DISABLED;                                            // RETURN
    }

    return e_SUCCESS;
}

                                  // Aspects

template <class TYPE>
inline
bslma::Allocator *BoundedQueue<TYPE>::allocator() const
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
