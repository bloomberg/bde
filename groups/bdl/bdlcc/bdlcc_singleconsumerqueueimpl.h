// bdlcc_singleconsumerqueueimpl.h                                    -*-C++-*-

#ifndef INCLUDED_BDLCC_SINGLECONSUMERQUEUEIMPL
#define INCLUDED_BDLCC_SINGLECONSUMERQUEUEIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a testable thread-aware single consumer queue of values.
//
//@CLASSES:
//  bdlcc::SingleConsumerQueueImpl: thread-aware single consumer 'TYPE' queue
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component defines a type,
// 'bdlcc::SingleConsumerQueueImpl', that provides an efficient, thread-aware
// queue of values assuming a single consumer (the use of 'popFront',
// 'tryPopFront', and 'removeAll' is done by one thread or a group of threads
// using external synchronization).  The behavior of the methods 'popFront',
// 'tryPopFront', and 'removeAll' is undefined unless the use is by a single
// consumer.  This class is ideal for synchronization and communication between
// threads in a producer-consumer model when there is only one consumer thread.
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
///Exception safety
///----------------
// A 'bdlcc::SingleConsumerQueueImpl' is exception neutral, and all of the
// methods of 'bdlcc::SingleConsumerQueueImpl' provide the basic exception
// safety guarantee (see 'bsldoc_glossary').
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlcc::SingleConsumerQueueImpl' on C++11
// platforms only (where 'BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES' is defined),
// and are not supported on C++03 platforms.  Unfortunately, in C++03, there
// are user types where a 'bslmf::MovableRef' will not safely degrade to a
// lvalue reference when a move constructor is not available (types providing a
// constructor template taking any type), so 'bslmf::MovableRefUtil::move'
// cannot be used directly on a user supplied template type.  See internal bug
// report 99039150 for more information.
//
///Memory Usage
///------------
// 'bdlcc::SingleConsumerQueueImpl' is most efficient when dealing with small
// objects or fundamental types (as a thread-safe container, its methods pass
// objects *by* *value*).  We recommend large objects be stored as
// shared-pointers (or possibly raw pointers).
//
///WARNING: Late Signaling Optimization
///------------------------------------
// It is the responsibility of the client to ensure that ALL actions on a
// 'bdlcc::SingleConsumerQueueImpl' are COMPLETED before destroying it.  Note
// that a 'push' has not necessarily completed as soon as the mutex is unlocked
// and the pushed item is available for other threads to access -- we employ
// "late signaling", where the pushing thread will take action to inform other
// threads of the state change *AFTER* the action is completed and the mutex is
// unlocked, as a performance optimization.  If a caller anticipates the number
// of items that will be pushed and destroys a container IMMEDIATELY after they
// have all been popped, it may result in undefined behavior when a pusher
// tries to signal the destoryed 'bdlcc::SingleConsumerQueueImpl'.  It is best
// to wait until all subthreads are joined, or until after they have all
// arrived at a barrier, before destroying a shared
// 'bdlcc::SingleConsumerQueueImpl'.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bdlscm_version.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlcc {

            // ==================================================
            // class SingleConsumerQueueImpl_ReleaseAllRawProctor
            // ==================================================

template <class TYPE>
class SingleConsumerQueueImpl_ReleaseAllRawProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'releaseAllRaw' on a
    // 'TYPE' upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue

    // NOT IMPLEMENTED
    SingleConsumerQueueImpl_ReleaseAllRawProctor();
    SingleConsumerQueueImpl_ReleaseAllRawProctor(
                          const SingleConsumerQueueImpl_ReleaseAllRawProctor&);
    SingleConsumerQueueImpl_ReleaseAllRawProctor& operator=(
                          const SingleConsumerQueueImpl_ReleaseAllRawProctor&);

  public:
    // CREATORS
    explicit
    SingleConsumerQueueImpl_ReleaseAllRawProctor(TYPE *queue);
        // Create a 'removeAll' proctor that conditionally manages the
        // specified 'queue' (if non-zero).

    ~SingleConsumerQueueImpl_ReleaseAllRawProctor();
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed queue's 'releaseAllRaw' method.

    // MANIPULATORS
    void release();
        // Release from management the queue currently managed by this proctor.
        // If no queue, this method has no effect.
};

             // ================================================
             // class SingleConsumerQueueImpl_MarkReclaimProctor
             // ================================================

template <class TYPE, class NODE>
class SingleConsumerQueueImpl_MarkReclaimProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'markReclaim' on a 'NODE'
    // upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue owning the managed node
    NODE *d_node_p;   // managed node

    // NOT IMPLEMENTED
    SingleConsumerQueueImpl_MarkReclaimProctor();
    SingleConsumerQueueImpl_MarkReclaimProctor(
                            const SingleConsumerQueueImpl_MarkReclaimProctor&);
    SingleConsumerQueueImpl_MarkReclaimProctor& operator=(
                            const SingleConsumerQueueImpl_MarkReclaimProctor&);

  public:
    // CREATORS
    SingleConsumerQueueImpl_MarkReclaimProctor(TYPE *queue, NODE *node);
        // Create a 'popComplete' proctor managing the specified 'node' of the
        // specified 'queue'.

    ~SingleConsumerQueueImpl_MarkReclaimProctor();
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed queue's 'markReclaim' method with the managed node.

    // MANIPULATORS
    void release();
        // Release from management the queue and node currently managed by this
        // proctor.  If no queue, this method has no effect.
};

              // ==============================================
              // class SingleConsumerQueueImpl_PopCompleteGuard
              // ==============================================

template <class TYPE>
class SingleConsumerQueueImpl_PopCompleteGuard {
    // This class implements a guard automatically invokes 'popComplete' on the
    // managed queue upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue owning the managed node

    // NOT IMPLEMENTED
    SingleConsumerQueueImpl_PopCompleteGuard();
    SingleConsumerQueueImpl_PopCompleteGuard(
                              const SingleConsumerQueueImpl_PopCompleteGuard&);
    SingleConsumerQueueImpl_PopCompleteGuard& operator=(
                              const SingleConsumerQueueImpl_PopCompleteGuard&);

  public:
    // CREATORS
    explicit
    SingleConsumerQueueImpl_PopCompleteGuard(TYPE *queue);
        // Create a 'popComplete' guard managing the specified 'queue'.

    ~SingleConsumerQueueImpl_PopCompleteGuard();
        // Destroy this object and invoke the 'popComplete' method on the
        // managed queue.
};

                      // =============================
                      // class SingleConsumerQueueImpl
                      // =============================

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
class SingleConsumerQueueImpl {
    // This class provides a thread-safe unbounded queue of values that assumes
    // a single consumer thread.
    //
    // The types 'ATOMIC_OP', 'MUTEX', and 'CONDITION' are exposed for testing.
    // Typical usage is with 'bsls::AtomicOperations' for 'ATOMIC_OP',
    // 'bslmt::Mutex' for 'MUTEX', and 'bslmt::Condition' for 'CONDITION'.

    // PRIVATE CONSTANTS
    enum {
        // These value are used as values for 'd_state' in 'Node'.  A node is
        // writable at creation and after a read completes (when the single
        // consumer can write to the node).  A node is readable after it is
        // written (when the node can be read by a consumer).  The states
        // in-between these two states (e.g., writing) are not needed by this
        // implementation of the queue.

        e_READABLE,              // node can be read
        e_WRITABLE,              // node can be written
        e_WRITABLE_AND_BLOCKED,  // node can be written and has blocked reader
        e_RECLAIM                // node suffered exception while reading
    };

    // The following constants are used to maintain the queue's 'd_state'
    // attribute values for:
    //   * number of threads allocating a new node,
    //   * number of threads attempting to use an existing node,
    //   * and number of nodes available for use.
    //
    // The 'k_*_MASK' constants define the layout of the attributes, the
    // 'k_*_INC' constants are used to modify the 'd_state' attributes, and the
    // 'k_*_SHIFT' constants allow recovery of the stored value.
    //
    // See *Implementation* *Note* for further details.

    static const bsls::Types::Int64 k_ALLOCATE_MASK     = 0x0000000000003fffLL;
    static const bsls::Types::Int64 k_ALLOCATE_INC      = 0x0000000000000001LL;
    static const bsls::Types::Int64 k_USE_MASK          = 0x000000000fffc000LL;
    static const bsls::Types::Int64 k_USE_INC           = 0x0000000000004000LL;
    static const bsls::Types::Int64 k_AVAILABLE_MASK    = 0xfffffffff0000000LL;
    static const bsls::Types::Int64 k_AVAILABLE_INC     = 0x0000000010000000LL;
    static const int                k_AVAILABLE_SHIFT   = 28;

    // PRIVATE TYPES
    typedef typename ATOMIC_OP::AtomicTypes::Int     AtomicInt;
    typedef typename ATOMIC_OP::AtomicTypes::Int64   AtomicInt64;
    typedef typename ATOMIC_OP::AtomicTypes::Uint    AtomicUint;
    typedef typename ATOMIC_OP::AtomicTypes::Pointer AtomicPointer;

    template <class T>
    struct QueueNode {
        // PUBLIC DATA
        bsls::ObjectBuffer<T> d_value;  // stored value
        AtomicInt             d_state;  // 'e_READABLE', 'e_WRITABLE', etc.
        AtomicPointer         d_next;   // pointer to next node
    };

    typedef QueueNode<TYPE> Node;

    // DATA
    AtomicPointer       d_nextWrite;         // pointer to next write to node

    AtomicPointer       d_nextRead;          // pointer to next read from node

    MUTEX               d_readMutex;         // used with 'd_readCondition' to
                                             // block until an element is
                                             // available for popping

    CONDITION           d_readCondition;     // condition variable for popping
                                             // thread

    MUTEX               d_writeMutex;        // during allocation, used to
                                             // synchronize threads access to
                                             // 'd_nextWrite'

    AtomicInt64         d_capacity;          // capacity of this queue

    mutable MUTEX       d_emptyMutex;        // blocking point for consumer
                                             // during 'waitUntilEmpty'

    mutable CONDITION   d_emptyCondition;    // condition variable for consumer
                                             // during 'waitUntilEmpty'

    AtomicInt64         d_state;             // bit pattern representing the
                                             // state of the queue (see
                                             // implementation notes)

    AtomicUint          d_popFrontDisabled;  // is queue pop disabled and
                                             // generation count; see
                                             // *Implementation* *Note*

    AtomicUint          d_pushBackDisabled;  // is queue push disabled and
                                             // generation count; see
                                             // *Implementation* *Note*

    bslma::Allocator   *d_allocator_p;       // allocator, held not owned

    // FRIENDS
    friend class SingleConsumerQueueImpl_ReleaseAllRawProctor<
                                          SingleConsumerQueueImpl<TYPE,
                                                                  ATOMIC_OP,
                                                                  MUTEX,
                                                                  CONDITION> >;

    friend class SingleConsumerQueueImpl_MarkReclaimProctor<
                           SingleConsumerQueueImpl<TYPE,
                                                   ATOMIC_OP,
                                                   MUTEX,
                                                   CONDITION>,
                           typename SingleConsumerQueueImpl<TYPE,
                                                            ATOMIC_OP,
                                                            MUTEX,
                                                            CONDITION>::Node >;

    friend class SingleConsumerQueueImpl_PopCompleteGuard<
                                          SingleConsumerQueueImpl<TYPE,
                                                                  ATOMIC_OP,
                                                                  MUTEX,
                                                                  CONDITION> >;

    // PRIVATE CLASS METHODS
    static bsls::Types::Int64 available(bsls::Types::Int64 state);
        // Return the available attribute from the specified 'state'.

    // PRIVATE MANIPULATORS
    void markReclaim(Node *node);
        // Mark the specified 'node' as a node to be reclaimed.

    void popComplete(bool destruct);
        // If the specified 'destruct' is true, destruct the value stored in
        // 'd_nextRead'.  Mark 'd_nextRead' writable, and if the queue is empty
        // then signal the queue empty condition.  This method is used to
        // complete the reclamation of a node in the presence of an exception.

    Node *pushBackHelper();
        // Return a pointer to the node to assign the value being pushed into
        // this queue, or 0 if 'isPushBackDisabled()'.

    void incrementUntil(AtomicUint *value, unsigned int bitValue);
        // If the specified 'value' does not have its lowest-order bit set to
        // the value of the specified 'bitValue', increment 'value' until it
        // does.  Note that this method is used to modify the generation counts
        // stored in 'd_popFrontDisabled' and 'd_pushBackDisabled'.  See
        // *Implementation* *Note* for further details.

    void releaseAllRaw();
        // Return all memory to the allocator.  This method is intended to be
        // used by the destructor and to avoid a memory leak when there is an
        // exception during construction.

    // NOT IMPLEMENTED
    SingleConsumerQueueImpl(const SingleConsumerQueueImpl&);
    SingleConsumerQueueImpl& operator=(const SingleConsumerQueueImpl&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SingleConsumerQueueImpl,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS  =  0,
        e_EMPTY    = -1,
        e_DISABLED = -2
    };

    // CREATORS
    explicit
    SingleConsumerQueueImpl(bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    explicit
    SingleConsumerQueueImpl(bsl::size_t       capacity,
                            bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with, at least, the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~SingleConsumerQueueImpl();
        // Destroy this object.

    // MANIPULATORS
    int popFront(TYPE *value);
        // Remove the element from the front of this queue and load that
        // element into the specified 'value'.  If the queue is empty, block
        // until it is not empty.  Return 0 on success, and a non-zero value if
        // 'isPopFrontDisabled()'.  On failure, 'value' is not changed.
        // Threads blocked due to the queue being empty will return a non-zero
        // value if 'disablePopFront' is invoked.

    int pushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a nonzero value if 'isPushBackDisabled()'.  On failure,
        // 'value' is not changed.  The behavior is undefined unless the
        // invoker of this method is the single consumer.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a nonzero value if 'isPushBackDisabled()'.  On
        // failure, 'value' is not changed.  The behavior is undefined unless
        // the invoker of this method is the single consumer.

    void removeAll();
        // Remove all items currently in this queue.  Note that this operation
        // is not atomic; if other threads are concurrently pushing items into
        // the queue the result of 'numElements()' after this function returns
        // is not guaranteed to be 0.

    int tryPopFront(TYPE *value);
        // Attempt to remove the element from the front of this queue without
        // blocking, and, if successful, load the specified 'value' with the
        // removed element.  Return 0 on success, 'e_EMPTY' if the queue was
        // empty, and 'e_DISABLED' if 'isPopFrontDisabled()'.  On failure,
        // 'value' is not changed.

    int tryPushBack(const TYPE& value);
        // Append the specified 'value' to the back of this queue.  Return 0 on
        // success, and a nonzero value if 'isPushBackDisabled()'.  On failure,
        // 'value' is not changed.  The behavior is undefined unless the
        // invoker of this method is the single consumer.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a nonzero value if 'isPushBackDisabled()'.  On
        // failure, 'value' is not changed.  The behavior is undefined unless
        // the invoker of this method is the single consumer.

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
        // success, and a non-zero value if
        // '!isEmpty() && isPopFrontDisabled()'.  A blocked thread waiting for
        // the queue to empty will return a non-zero value if 'disablePopFront'
        // is invoked.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

            // --------------------------------------------------
            // class SingleConsumerQueueImpl_ReleaseAllRawProctor
            // --------------------------------------------------

// CREATORS
template <class TYPE>
SingleConsumerQueueImpl_ReleaseAllRawProctor<TYPE>::
                      SingleConsumerQueueImpl_ReleaseAllRawProctor(TYPE *queue)
: d_queue_p(queue)
{
}

template <class TYPE>
SingleConsumerQueueImpl_ReleaseAllRawProctor<TYPE>::
                                ~SingleConsumerQueueImpl_ReleaseAllRawProctor()
{
    if (d_queue_p) {
        d_queue_p->releaseAllRaw();
    }
}

// MANIPULATORS
template <class TYPE>
void SingleConsumerQueueImpl_ReleaseAllRawProctor<TYPE>::release()
{
    d_queue_p = 0;
}

             // ------------------------------------------------
             // class SingleConsumerQueueImpl_MarkReclaimProctor
             // ------------------------------------------------

// CREATORS
template <class TYPE, class NODE>
SingleConsumerQueueImpl_MarkReclaimProctor<TYPE, NODE>::
            SingleConsumerQueueImpl_MarkReclaimProctor(TYPE *queue, NODE *node)
: d_queue_p(queue)
, d_node_p(node)
{
}

template <class TYPE, class NODE>
SingleConsumerQueueImpl_MarkReclaimProctor<TYPE, NODE>::
                                  ~SingleConsumerQueueImpl_MarkReclaimProctor()
{
    if (d_queue_p) {
        d_queue_p->markReclaim(d_node_p);
    }
}

// MANIPULATORS
template <class TYPE, class NODE>
void SingleConsumerQueueImpl_MarkReclaimProctor<TYPE, NODE>::release()
{
    d_queue_p = 0;
}

              // ----------------------------------------------
              // class SingleConsumerQueueImpl_PopCompleteGuard
              // ----------------------------------------------

// CREATORS
template <class TYPE>
SingleConsumerQueueImpl_PopCompleteGuard<TYPE>::
                          SingleConsumerQueueImpl_PopCompleteGuard(TYPE *queue)
: d_queue_p(queue)
{
}

template <class TYPE>
SingleConsumerQueueImpl_PopCompleteGuard<TYPE>::
                                    ~SingleConsumerQueueImpl_PopCompleteGuard()
{
    d_queue_p->popComplete(true);
}

                      // -----------------------------
                      // class SingleConsumerQueueImpl
                      // -----------------------------

// PRIVATE CLASS METHODS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bsls::Types::Int64 SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                          ::available(bsls::Types::Int64 state)
{
    return state >> k_AVAILABLE_SHIFT;
}

// PRIVATE MANIPULATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                      ::markReclaim(Node *node)
{
    // A reclaimed node is used to denote an exception occurred and the node is
    // currently invalid.  A reclaimed node is considered removed and not
    // counted as part of the capacity of the queue (thus ensuring
    // 'numElements' is correct).

    ATOMIC_OP::addInt64AcqRel(&d_capacity, -1);

    int nodeState = ATOMIC_OP::swapIntAcqRel(&node->d_state, e_RECLAIM);
    if (e_WRITABLE_AND_BLOCKED == nodeState) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_readMutex);
        }
        d_readCondition.signal();
    }
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                   ::popComplete(bool destruct)
{
    Node *nextRead =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));

    if (destruct) {
        nextRead->d_value.object().~TYPE();
    }

    ATOMIC_OP::setIntRelease(&nextRead->d_state, e_WRITABLE);

    ATOMIC_OP::setPtrRelease(&d_nextRead,
                             ATOMIC_OP::getPtrAcquire(&nextRead->d_next));

    bsls::Types::Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state,
                                                           k_AVAILABLE_INC);

    if (ATOMIC_OP::getInt64Acquire(&d_capacity) == available(state)) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
        }
        d_emptyCondition.broadcast();
    }
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
typename SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::Node *
                     SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                             ::pushBackHelper()
{
    if (1 == (ATOMIC_OP::getUintAcquire(&d_pushBackDisabled) & 1)) {
        return 0;                                                     // RETURN
    }

    // Fast path requires an available node and needs to indicate a thread is
    // intending to use an existing node.

    bsls::Types::Int64 state = ATOMIC_OP::addInt64NvAcqRel(
                                                  &d_state,
                                                  k_USE_INC - k_AVAILABLE_INC);

    if (0 > state || 0 < (state & k_ALLOCATE_MASK)) {

        // The determination to use an existing node was premature, undo the
        // indication.

        state = ATOMIC_OP::addInt64NvAcqRel(&d_state, -k_USE_INC);

        bsls::Types::Int64 expState;
        do {
            if (0 > state && 0 == (state & k_USE_MASK)) {
                expState = state;
                state = ATOMIC_OP::testAndSwapInt64AcqRel(
                                                       &d_state,
                                                       state,
                                                       state + k_AVAILABLE_INC
                                                             + k_ALLOCATE_INC);
            }
            else if (0 <= state && 0 == (state & k_ALLOCATE_MASK)) {
                expState = state;
                state = ATOMIC_OP::testAndSwapInt64AcqRel(&d_state,
                                                          state,
                                                          state + k_USE_INC);
            }
            else {
                bslmt::ThreadUtil::yield();
                state = ATOMIC_OP::getInt64Acquire(&d_state);

                expState = ~state;  // cause the 'while' to fail and remain in
                                    // this loop
            }
        } while (state != expState);
    }

    Node *nextWrite;

    if (0 <= state) {
        // Note that there are no threads attempting to allocate new nodes.

        nextWrite = static_cast<Node *>(
                                       ATOMIC_OP::getPtrAcquire(&d_nextWrite));
        Node *expNextWrite;
        do {
            expNextWrite = nextWrite;
            Node *next = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(
                                                          &nextWrite->d_next));

            nextWrite = static_cast<Node *>(ATOMIC_OP::testAndSwapPtrAcqRel(
                                                                  &d_nextWrite,
                                                                  nextWrite,
                                                                  next));
        } while (nextWrite != expNextWrite);

        ATOMIC_OP::addInt64AcqRel(&d_state, -k_USE_INC);
    }
    else {
        // Note that there are no threads attempting to use available nodes.

        Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));

        ATOMIC_OP::initInt(&n->d_state, e_WRITABLE);

        {
            bslmt::LockGuard<MUTEX> guard(&d_writeMutex);

            // Note that this is the *only* thread accessing 'd_nextWrite'.

            nextWrite = static_cast<Node *>(
                                       ATOMIC_OP::getPtrAcquire(&d_nextWrite));

            ATOMIC_OP::setPtrRelease(&n->d_next,
                                     static_cast<Node *>(
                                                      ATOMIC_OP::getPtrAcquire(
                                                         &nextWrite->d_next)));
            ATOMIC_OP::setPtrRelease(&nextWrite->d_next, n);
            ATOMIC_OP::setPtrRelease(&d_nextWrite, n);

            ATOMIC_OP::addInt64AcqRel(&d_capacity, 1);
        }

        ATOMIC_OP::addInt64AcqRel(&d_state, -k_ALLOCATE_INC);
    }

    return nextWrite;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                     ::incrementUntil(AtomicUint *value, unsigned int bitValue)
{
    unsigned int state = ATOMIC_OP::getUintAcquire(value);
    if (bitValue != (state & 1)) {
        unsigned int expState;
        do {
            expState = state;
            state = ATOMIC_OP::testAndSwapUintAcqRel(value,
                                                     state,
                                                     state + 1);
        } while (state != expState && (bitValue == (state & 1)));
    }
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                              ::releaseAllRaw()
{
    Node *end = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextWrite));

    if (end) {
        Node *at = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&end->d_next));

        while (at != end) {
            Node *next =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&at->d_next));

            if (e_READABLE == ATOMIC_OP::getIntAcquire(&at->d_state)) {
                at->d_value.object().~TYPE();
            }

            d_allocator_p->deallocate(at);

            at = next;
        }

        if (e_READABLE == ATOMIC_OP::getIntAcquire(&at->d_state)) {
            at->d_value.object().~TYPE();
        }

        d_allocator_p->deallocate(at);
    }
}

// CREATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                      SingleConsumerQueueImpl(bslma::Allocator *basicAllocator)
: d_readMutex()
, d_readCondition()
, d_writeMutex()
, d_emptyMutex()
, d_emptyCondition()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ATOMIC_OP::initInt64(&d_capacity, 0);
    ATOMIC_OP::initInt64(&d_state,    0);

    ATOMIC_OP::initUint(&d_popFrontDisabled, 0);
    ATOMIC_OP::initUint(&d_pushBackDisabled, 0);

    ATOMIC_OP::initPointer(&d_nextWrite, 0);

    SingleConsumerQueueImpl_ReleaseAllRawProctor<SingleConsumerQueueImpl <
                                                    TYPE,
                                                    ATOMIC_OP,
                                                    MUTEX,
                                                    CONDITION> > proctor(this);

    Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n->d_next, n);

    ATOMIC_OP::setPtrRelease(&d_nextWrite, n);
    ATOMIC_OP::setPtrRelease(&d_nextRead,  n);

    proctor.release();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                      SingleConsumerQueueImpl(bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_readMutex()
, d_readCondition()
, d_writeMutex()
, d_emptyMutex()
, d_emptyCondition()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ATOMIC_OP::initInt64(&d_capacity, 0);
    ATOMIC_OP::initInt64(&d_state,    0);

    ATOMIC_OP::initUint(&d_popFrontDisabled, 0);
    ATOMIC_OP::initUint(&d_pushBackDisabled, 0);

    ATOMIC_OP::initPointer(&d_nextWrite, 0);

    SingleConsumerQueueImpl_ReleaseAllRawProctor<SingleConsumerQueueImpl <
                                                    TYPE,
                                                    ATOMIC_OP,
                                                    MUTEX,
                                                    CONDITION> > proctor(this);

    Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n->d_next, n);

    ATOMIC_OP::setPtrRelease(&d_nextWrite, n);
    ATOMIC_OP::setPtrRelease(&d_nextRead,  n);

    for (bsl::size_t i = 0; i < capacity; ++i) {
        Node *nn = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
        ATOMIC_OP::initInt(&nn->d_state, e_WRITABLE);
        ATOMIC_OP::initPointer(&nn->d_next,
                               static_cast<Node *>(ATOMIC_OP::getPtrAcquire(
                                                                 &n->d_next)));
        ATOMIC_OP::setPtrRelease(&n->d_next, nn);
    }

    ATOMIC_OP::addInt64AcqRel(&d_capacity, capacity);
    ATOMIC_OP::addInt64AcqRel(&d_state, k_AVAILABLE_INC * capacity);

    proctor.release();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                     ~SingleConsumerQueueImpl()
{
    releaseAllRaw();
}

// MANIPULATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::popFront(
                                                                   TYPE *value)
{
    unsigned int generation = ATOMIC_OP::getUintAcquire(&d_popFrontDisabled);
    if (1 == (generation & 1)) {
        return e_DISABLED;                                            // RETURN
    }

    Node *nextRead =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
    int nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
    do {
        // Note that 'e_WRITABLE_AND_BLOCKED != nodeState' since if the one
        // consumer sets this state, the one consumer waits until the node is
        // readable, and either the producer that signalled the consumer
        // changed the node state already, or the consumer will change the node
        // state in 'popComplete'.

        if (e_WRITABLE == nodeState) {
            bslmt::ThreadUtil::yield();
            nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
            if (e_WRITABLE == nodeState) {
                bslmt::LockGuard<MUTEX> guard(&d_readMutex);
                nodeState = ATOMIC_OP::swapIntAcqRel(&nextRead->d_state,
                                                     e_WRITABLE_AND_BLOCKED);
                while (e_READABLE != nodeState && e_RECLAIM != nodeState) {
                    if (generation !=
                              ATOMIC_OP::getUintAcquire(&d_popFrontDisabled)) {
                        return e_DISABLED;                            // RETURN
                    }
                    d_readCondition.wait(&d_readMutex);
                    nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
                }
            }
        }
        if (e_RECLAIM == nodeState) {
            ATOMIC_OP::addInt64AcqRel(&d_capacity, 1);
            popComplete(false);
            nextRead =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
            nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
        }
    } while (e_RECLAIM == nodeState);

    SingleConsumerQueueImpl_PopCompleteGuard<
                              SingleConsumerQueueImpl<TYPE,
                                                      ATOMIC_OP,
                                                      MUTEX,
                                                      CONDITION> > guard(this);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    *value = bslmf::MovableRefUtil::move(nextRead->d_value.object());
#else
    *value = nextRead->d_value.object();
#endif

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::pushBack(
                                                             const TYPE& value)
{
    Node *target = pushBackHelper();

    if (0 == target) {
        return e_DISABLED;                                            // RETURN
    }

    SingleConsumerQueueImpl_MarkReclaimProctor<
                                            SingleConsumerQueueImpl<TYPE,
                                                                    ATOMIC_OP,
                                                                    MUTEX,
                                                                    CONDITION>,
                                            Node> proctor(this, target);

    bslalg::ScalarPrimitives::copyConstruct(target->d_value.address(),
                                            value,
                                            d_allocator_p);

    proctor.release();

    int nodeState = ATOMIC_OP::swapIntAcqRel(&target->d_state, e_READABLE);
    if (e_WRITABLE_AND_BLOCKED == nodeState) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_readMutex);
        }
        d_readCondition.signal();
    }

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::pushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    Node *target = pushBackHelper();

    if (0 == target) {
        return e_DISABLED;                                            // RETURN
    }

    SingleConsumerQueueImpl_MarkReclaimProctor<
                                            SingleConsumerQueueImpl<TYPE,
                                                                    ATOMIC_OP,
                                                                    MUTEX,
                                                                    CONDITION>,
                                            Node> proctor(this, target);

    TYPE& dummy = value;
    bslalg::ScalarPrimitives::moveConstruct(target->d_value.address(),
                                            dummy,
                                            d_allocator_p);

    proctor.release();

    int nodeState = ATOMIC_OP::swapIntAcqRel(&target->d_state, e_READABLE);
    if (e_WRITABLE_AND_BLOCKED == nodeState) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_readMutex);
        }
        d_readCondition.signal();
    }

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::removeAll()
{
    int count = 0;

    bsls::Types::Int64 reclaim = 0;

    Node *nextRead =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
    int nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
    while (e_READABLE == nodeState || e_RECLAIM == nodeState) {
        if (e_READABLE == nodeState) {
            nextRead->d_value.object().~TYPE();
        }
        else {
            ++reclaim;
        }
        ATOMIC_OP::setIntRelease(&nextRead->d_state, e_WRITABLE);
        nextRead =
              static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&nextRead->d_next));
        ATOMIC_OP::setPtrRelease(&d_nextRead, nextRead);
        nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
        ++count;
    }

    ATOMIC_OP::addInt64AcqRel(&d_capacity, reclaim);
    ATOMIC_OP::addInt64AcqRel(&d_state, k_AVAILABLE_INC * count);

    {
        bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
    }
    d_emptyCondition.broadcast();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPopFront(
                                                                   TYPE *value)
{
    unsigned int generation = ATOMIC_OP::getUintAcquire(&d_popFrontDisabled);
    if (1 == (generation & 1)) {
        return e_DISABLED;                                            // RETURN
    }

    Node *nextRead =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
    int nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);

    while (e_RECLAIM == nodeState) {
        ATOMIC_OP::addInt64AcqRel(&d_capacity, 1);
        popComplete(false);
        nextRead = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
        nodeState = ATOMIC_OP::getIntAcquire(&nextRead->d_state);
    }

    if (e_READABLE != nodeState) {
        return e_EMPTY;                                               // RETURN
    }

    SingleConsumerQueueImpl_PopCompleteGuard<
                              SingleConsumerQueueImpl<TYPE,
                                                      ATOMIC_OP,
                                                      MUTEX,
                                                      CONDITION> > guard(this);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    *value = bslmf::MovableRefUtil::move(nextRead->d_value.object());
#else
    *value = nextRead->d_value.object();
#endif

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPushBack(
                                                             const TYPE& value)
{
    return pushBack(value);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    return pushBack(bslmf::MovableRefUtil::move(value));
}

                       // Enqueue/Dequeue State

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                            ::disablePopFront()
{
    incrementUntil(&d_popFrontDisabled, 1);

    {
        bslmt::LockGuard<MUTEX> guard(&d_readMutex);
    }
    d_readCondition.signal();

    {
        bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
    }
    d_emptyCondition.broadcast();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                            ::disablePushBack()
{
    incrementUntil(&d_pushBackDisabled, 1);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                             ::enablePopFront()
{
    incrementUntil(&d_popFrontDisabled, 0);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                             ::enablePushBack()
{
    incrementUntil(&d_pushBackDisabled, 0);
}

// ACCESSORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                              ::isEmpty() const
{
    return ATOMIC_OP::getInt64Acquire(&d_capacity) ==
                               available(ATOMIC_OP::getInt64Acquire(&d_state));
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::isFull() const
{
    return false;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                   ::isPopFrontDisabled() const
{
    return 1 == (ATOMIC_OP::getUintAcquire(&d_popFrontDisabled) & 1);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                   ::isPushBackDisabled() const
{
    return 1 == (ATOMIC_OP::getUintAcquire(&d_pushBackDisabled) & 1);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bsl::size_t SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                          ::numElements() const
{
    bsls::Types::Int64 avail = available(ATOMIC_OP::getInt64Acquire(&d_state));
    return static_cast<bsl::size_t>(
                                avail > 0
                              ? ATOMIC_OP::getInt64Acquire(&d_capacity) - avail
                              : ATOMIC_OP::getInt64Acquire(&d_capacity));
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                       ::waitUntilEmpty() const
{
    unsigned int generation = ATOMIC_OP::getUintAcquire(&d_popFrontDisabled);
    if (1 == (generation & 1)) {
        return e_DISABLED;                                            // RETURN
    }

    bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);

    bsls::Types::Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);
    while (ATOMIC_OP::getInt64Acquire(&d_capacity) != available(state)) {
        if (generation != ATOMIC_OP::getUintAcquire(&d_popFrontDisabled)) {
            return e_DISABLED;                                        // RETURN
        }
        d_emptyCondition.wait(&d_emptyMutex);
        state = ATOMIC_OP::getInt64Acquire(&d_state);
    }

    return 0;
}

                                  // Aspects

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bslma::Allocator *SingleConsumerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                              allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
