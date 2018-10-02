// bdlcc_singleproducerqueueimpl.h                                    -*-C++-*-

#ifndef INCLUDED_BDLCC_SINGLEPRODUCERQUEUEIMPL
#define INCLUDED_BDLCC_SINGLEPRODUCERQUEUEIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a testable thread-aware single producer queue of values.
//
//@CLASSES:
//  bdlcc::SingleProducerQueueImpl: thread-aware single producer 'TYPE' queue
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn4)
//
//@DESCRIPTION: This component defines a type,
// 'bdlcc::SingleProducerQueueImpl', that provides an efficient, thread-aware
// queue of values assuming a single producer (the use of 'pushBack' and
// 'tryPushBack' is done by one thread or a group of threads using external
// synchronization).  The behavior of the methods 'pushBack' and 'tryPushBack'
// is undefined unless the use is by a single producer.  This class is ideal
// for synchronization and communication between threads in a producer-consumer
// model when there is only one producer thread.
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
// The queue may be placed into a "enqueue disabled" state using the 'disable'
// method.  When disabled, 'pushBack' and 'tryPushBack' fail immediately and
// return an error code.  The queue may be restored to normal operation with
// the 'enablePushBack' method.
//
// The queue may be placed into a "dequeue disabled" state using the
// 'disablePopFront' method.  When dequeue disabled, 'popFront' and
// 'tryPopFront' fail immediately and return an error code.  Any threads
// blocked in 'popFront' when the queue is dequeue disabled return from
// 'popFront' immediately and return an error code.
//
///Exception safety
///----------------
// A 'bdlcc::SingleProducerQueueImpl' is exception neutral, and all of the
// methods of 'bdlcc::SingleProducerQueueImpl' provide the basic exception
// safety guarantee (see 'bsldoc_glossary').
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlcc::SingleProducerQueueImpl' on C++11
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
// 'bdlcc::SingleProducerQueueImpl' is most efficient when dealing with small
// objects or fundamental types (as a thread-safe container, its methods pass
// objects *by* *value*).  We recommend large objects be stored as
// shared-pointers (or possibly raw pointers).
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

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlcc {

            // ==================================================
            // class SingleProducerQueueImpl_ReleaseAllRawProctor
            // ==================================================

template <class TYPE>
class SingleProducerQueueImpl_ReleaseAllRawProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'releaseAllRaw' on a
    // 'TYPE' upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue

    // NOT IMPLEMENTED
    SingleProducerQueueImpl_ReleaseAllRawProctor();
    SingleProducerQueueImpl_ReleaseAllRawProctor(
                          const SingleProducerQueueImpl_ReleaseAllRawProctor&);
    SingleProducerQueueImpl_ReleaseAllRawProctor& operator=(
                          const SingleProducerQueueImpl_ReleaseAllRawProctor&);

  public:
    // CREATORS
    SingleProducerQueueImpl_ReleaseAllRawProctor(TYPE *queue);
        // Create a 'removeAll' proctor that conditionally manages the
        // specified 'queue' (if non-zero).

    ~SingleProducerQueueImpl_ReleaseAllRawProctor();
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed queue's 'releaseAllRaw' method.

    // MANIPULATORS
    void release();
        // Release from management the queue currently managed by this proctor.
        // If no queue, this method has no effect.
};

              // ==============================================
              // class SingleProducerQueueImpl_PopCompleteGuard
              // ==============================================

template <class TYPE, class NODE>
class SingleProducerQueueImpl_PopCompleteGuard {
    // This class implements a guard automatically invokes 'popComplete' on a
    // 'NODE' upon destruction.

    // DATA
    TYPE *d_queue_p;  // managed queue owning the managed node
    NODE *d_node_p;   // managed node
    bool  d_isEmpty;  // if true, the empty condition will be signalled

    // NOT IMPLEMENTED
    SingleProducerQueueImpl_PopCompleteGuard();
    SingleProducerQueueImpl_PopCompleteGuard(
                              const SingleProducerQueueImpl_PopCompleteGuard&);
    SingleProducerQueueImpl_PopCompleteGuard& operator=(
                              const SingleProducerQueueImpl_PopCompleteGuard&);

  public:
    // CREATORS
    SingleProducerQueueImpl_PopCompleteGuard(TYPE *queue,
                                             NODE *node,
                                             bool  isEmpty);
        // Create a 'popComplete' guard managing the specified 'queue' and
        // 'node' that will cause the empty condition to be signalled if the
        // specified 'isEmpty' is 'true'.

    ~SingleProducerQueueImpl_PopCompleteGuard();
        // Destroy this object and invoke the static 'TYPE::popComplete' method
        // with the managed 'node'.
};

                      // =============================
                      // class SingleProducerQueueImpl
                      // =============================

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
class SingleProducerQueueImpl {
    // This class provides a thread-safe unbounded queue of values that assumes
    // a single producer thread.
    //
    // The types 'ATOMIC_OP', 'MUTEX', and 'CONDITION' are exposed for testing.
    // Typical usage is with 'bsls::AtomicOperations' for 'ATOMIC_OP',
    // 'bslmt::Mutex' for 'MUTEX', and 'bslmt::Condition' for 'CONDITION'.

    // PRIVATE CONSTANTS
    enum {
        // These value are used as values for 'd_state' in 'Node'.  A node is
        // writable at creation and after a read completes (when the single
        // producer can write to the node).  A node is readable after it is
        // written (when the node can be read by a consumer).  The states
        // in-between these two states (e.g., writing) are not needed by this
        // implementation of the queue.

        e_READABLE,  // node can be read
        e_WRITABLE   // node can be written
    };

    // PRIVATE TYPES
    typedef typename ATOMIC_OP::AtomicTypes::Int     AtomicInt;
    typedef typename ATOMIC_OP::AtomicTypes::Pointer AtomicPointer;

    template <class T>
    struct QueueNode {
        // PUBLIC DATA
        bsls::ObjectBuffer<T> d_value;  // stored value
        AtomicInt             d_state;  // 'e_READABLE' or 'e_WRITABLE'
        AtomicPointer         d_next;   // pointer to next node
    };

    typedef QueueNode<TYPE> Node;

    // DATA
    AtomicPointer     d_lastWritten;       // pointer to last written to node,
                                           // the next pushed node will be
                                           // after the currently pointed to
                                           // node

    // TBD consider padding between values for cache line efficiency

    AtomicPointer     d_nextRead;          // pointer to next read from node

    MUTEX             d_readMutex;         // blocking point for popping
                                           // threads, protects 'd_readRelease'

    int               d_readRelease;       // number of blocked popping threads
                                           // that can now read

    CONDITION         d_readCondition;     // condition variable for popping
                                           // threads

    mutable MUTEX     d_emptyMutex;        // blocking point for producer
                                           // during 'waitUntilEmpty'

    mutable CONDITION d_emptyCondition;    // condition variable for producer
                                           // during 'waitUntilEmpty'

    AtomicInt         d_numElements;       // if non-negative, number of
                                           // elements available; otherwise,
                                           // negative of number of blocked
                                           // threads

    AtomicInt         d_pushBackDisabled;  // is queue disabled

    AtomicInt         d_popFrontDisabled;  // is queue paused

    bslma::Allocator *d_allocator_p;       // allocator, held not owned

    // FRIENDS
    friend class SingleProducerQueueImpl_ReleaseAllRawProctor<
                                          SingleProducerQueueImpl<TYPE,
                                                                  ATOMIC_OP,
                                                                  MUTEX,
                                                                  CONDITION> >;

    friend class SingleProducerQueueImpl_PopCompleteGuard<
                           SingleProducerQueueImpl<TYPE,
                                                   ATOMIC_OP,
                                                   MUTEX,
                                                   CONDITION>,
                           typename SingleProducerQueueImpl<TYPE,
                                                            ATOMIC_OP,
                                                            MUTEX,
                                                            CONDITION>::Node >;

    // PRIVATE MANIPULATORS
    void popComplete(Node *node, bool isEmpty);
        // Destruct the value stored in the specified 'node', mark the 'node'
        // writable, and if the specified 'isEmpty' is 'true' then signal the
        // queue empty condition.  This method is used within 'popFrontRaw' by
        // a guard to complete the reclamation of a node in the presence of an
        // exception.

    void popFrontRaw(TYPE* value, bool isEmpty);
        // Remove the element, without verifying the availability of the
        // element, from the front of this queue, load that element into the
        // specified 'value', and if the specified 'isEmpty' is 'true' then
        // signal the queue empty condition.

    void releaseAllRaw();
        // Return all memory to the allocator.  This method is intended to be
        // used by the destructor and to avoid a memory leak when there is an
        // exception during construction.

    // NOT IMPLEMENTED
    SingleProducerQueueImpl(const SingleProducerQueueImpl&);
    SingleProducerQueueImpl& operator=(const SingleProducerQueueImpl&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SingleProducerQueueImpl,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    // PUBLIC CONSTANTS
    enum {
        e_SUCCESS = 0,
        e_EMPTY,
        e_DISABLED
    };

    // CREATORS
    SingleProducerQueueImpl(bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    SingleProducerQueueImpl(bsl::size_t       capacity,
                            bslma::Allocator *basicAllocator = 0);
        // Create a thread-aware queue with, at least, the specified
        // 'capacity'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~SingleProducerQueueImpl();
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
        // invoker of this method is the single producer.

    int pushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a nonzero value if 'isPushBackDisabled()'.  On
        // failure, 'value' is not changed.  The behavior is undefined unless
        // the invoker of this method is the single producer.

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
        // invoker of this method is the single producer.

    int tryPushBack(bslmf::MovableRef<TYPE> value);
        // Append the specified move-insertable 'value' to the back of this
        // queue.  'value' is left in a valid but unspecified state.  Return 0
        // on success, and a nonzero value if 'isPushBackDisabled()'.  On
        // failure, 'value' is not changed.  The behavior is undefined unless
        // the invoker of this method is the single producer.

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
            // class SingleProducerQueueImpl_ReleaseAllRawProctor
            // --------------------------------------------------

// CREATORS
template <class TYPE>
SingleProducerQueueImpl_ReleaseAllRawProctor<TYPE>::
                      SingleProducerQueueImpl_ReleaseAllRawProctor(TYPE *queue)
: d_queue_p(queue)
{
}

template <class TYPE>
SingleProducerQueueImpl_ReleaseAllRawProctor<TYPE>::
                                ~SingleProducerQueueImpl_ReleaseAllRawProctor()
{
    if (d_queue_p) {
        d_queue_p->releaseAllRaw();
    }
}

// MANIPULATORS
template <class TYPE>
void SingleProducerQueueImpl_ReleaseAllRawProctor<TYPE>::release()
{
    d_queue_p = 0;
}

              // ----------------------------------------------
              // class SingleProducerQueueImpl_PopCompleteGuard
              // ----------------------------------------------

// CREATORS
template <class TYPE, class NODE>
SingleProducerQueueImpl_PopCompleteGuard<TYPE, NODE>::
                        SingleProducerQueueImpl_PopCompleteGuard(TYPE *queue,
                                                                 NODE *node,
                                                                 bool  isEmpty)
: d_queue_p(queue)
, d_node_p(node)
, d_isEmpty(isEmpty)
{
}

template <class TYPE, class NODE>
SingleProducerQueueImpl_PopCompleteGuard<TYPE, NODE>::
                                    ~SingleProducerQueueImpl_PopCompleteGuard()
{
    d_queue_p->popComplete(d_node_p, d_isEmpty);
}

                      // -----------------------------
                      // class SingleProducerQueueImpl
                      // -----------------------------

// PRIVATE MANIPULATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                          popComplete(Node *node, bool isEmpty)
{
    node->d_value.object().~TYPE();

    ATOMIC_OP::setIntRelease(&node->d_state, e_WRITABLE);

    if (isEmpty) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
        }
        d_emptyCondition.broadcast();
    }

}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                          popFrontRaw(TYPE *value,
                                                      bool  isEmpty)
{
    Node *readFrom =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));

    Node *exp;
    do {
        Node *next =
              static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&readFrom->d_next));

        exp      = readFrom;
        readFrom = static_cast<Node *>(ATOMIC_OP::testAndSwapPtrAcqRel(
                                                                   &d_nextRead,
                                                                   readFrom,
                                                                   next));
    } while (readFrom != exp);

    SingleProducerQueueImpl_PopCompleteGuard<
                                          SingleProducerQueueImpl <TYPE,
                                                                   ATOMIC_OP,
                                                                   MUTEX,
                                                                   CONDITION>,
                                          Node> guard(this, readFrom, isEmpty);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    *value = bslmf::MovableRefUtil::move(readFrom->d_value.object());
#else
    *value = readFrom->d_value.object();
#endif
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                                releaseAllRaw()
{
    Node *end = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_lastWritten));

    if (end) {
        Node *at = static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&end->d_next));

        while (at != end) {
            Node *next =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&at->d_next));

            if (e_WRITABLE != ATOMIC_OP::getIntAcquire(&at->d_state)) {
                at->d_value.object().~TYPE();
            }

            d_allocator_p->deallocate(at);

            at = next;
        }

        if (e_WRITABLE != ATOMIC_OP::getIntAcquire(&at->d_state)) {
            at->d_value.object().~TYPE();
        }

        d_allocator_p->deallocate(at);
    }
}

// CREATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                      SingleProducerQueueImpl(bslma::Allocator *basicAllocator)
: d_readMutex()
, d_readRelease(0)
, d_readCondition()
, d_emptyMutex()
, d_emptyCondition()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ATOMIC_OP::initInt(&d_numElements,      0);
    ATOMIC_OP::initInt(&d_pushBackDisabled, 0);
    ATOMIC_OP::initInt(&d_popFrontDisabled, 0);

    ATOMIC_OP::initPointer(&d_lastWritten, 0);

    SingleProducerQueueImpl_ReleaseAllRawProctor<SingleProducerQueueImpl <
                                                    TYPE,
                                                    ATOMIC_OP,
                                                    MUTEX,
                                                    CONDITION> > proctor(this);

    Node *n1 = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n1->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n1->d_next, n1);

    ATOMIC_OP::setPtrRelease(&d_lastWritten, n1);

    Node *n2 = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n2->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n2->d_next, n1);
    ATOMIC_OP::setPtrRelease(&n1->d_next, n2);

    ATOMIC_OP::initPointer(&d_nextRead, n2);

    proctor.release();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                      SingleProducerQueueImpl(bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_readMutex()
, d_readRelease(0)
, d_readCondition()
, d_emptyMutex()
, d_emptyCondition()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ATOMIC_OP::initInt(&d_numElements,      0);
    ATOMIC_OP::initInt(&d_pushBackDisabled, 0);
    ATOMIC_OP::initInt(&d_popFrontDisabled, 0);

    ATOMIC_OP::initPointer(&d_lastWritten, 0);

    SingleProducerQueueImpl_ReleaseAllRawProctor<SingleProducerQueueImpl <
                                                    TYPE,
                                                    ATOMIC_OP,
                                                    MUTEX,
                                                    CONDITION> > proctor(this);

    Node *n1 = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n1->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n1->d_next, n1);

    ATOMIC_OP::setPtrRelease(&d_lastWritten, n1);

    Node *n2 = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
    ATOMIC_OP::initInt(&n2->d_state, e_WRITABLE);
    ATOMIC_OP::initPointer(&n2->d_next, n1);
    ATOMIC_OP::setPtrRelease(&n1->d_next, n2);

    ATOMIC_OP::initPointer(&d_nextRead, n2);

    capacity = (2 <= capacity ? capacity : 2);

    for (bsl::size_t i = 2; i < capacity; ++i) {
        Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));
        ATOMIC_OP::initInt(&n->d_state, e_WRITABLE);
        ATOMIC_OP::initPointer(&n->d_next,
                               ATOMIC_OP::getPtrAcquire(&n2->d_next));

        ATOMIC_OP::setPtrRelease(&n2->d_next, n);
    }

    proctor.release();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                     ~SingleProducerQueueImpl()
{
    releaseAllRaw();
}

// MANIPULATORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::popFront(
                                                                   TYPE *value)
{
    if (0 != ATOMIC_OP::getIntAcquire(&d_popFrontDisabled)) {
        return e_DISABLED;                                            // RETURN
    }

    int numElements = ATOMIC_OP::addIntNvAcqRel(&d_numElements, -1);

    if (0 > numElements) {
        bslmt::LockGuard<MUTEX> guard(&d_readMutex);
        while (0 == d_readRelease) {
            if (0 != ATOMIC_OP::getIntAcquire(&d_popFrontDisabled)) {
                ATOMIC_OP::addIntAcqRel(&d_numElements, 1);
                return e_DISABLED;                                    // RETURN
            }
            d_readCondition.wait(&d_readMutex);
        }
        --d_readRelease;
    }

    popFrontRaw(value, 0 == numElements);

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::pushBack(
                                                             const TYPE& value)
{
    if (0 != ATOMIC_OP::getIntAcquire(&d_pushBackDisabled)) {
        return -1;                                                    // RETURN
    }

    Node *lastWritten = static_cast<Node *>(
                                     ATOMIC_OP::getPtrAcquire(&d_lastWritten));
    Node *target      = static_cast<Node *>(
                               ATOMIC_OP::getPtrAcquire(&lastWritten->d_next));

    if (e_WRITABLE != ATOMIC_OP::getIntAcquire(&target->d_state)) {
        Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));

        bslma::DeallocatorProctor<bslma::Allocator> proctor(n, d_allocator_p);

        bslalg::ScalarPrimitives::copyConstruct(n->d_value.address(),
                                                value,
                                                d_allocator_p);

        proctor.release();

        ATOMIC_OP::initInt(&n->d_state, e_READABLE);
        ATOMIC_OP::initPointer(&n->d_next, target);

        ATOMIC_OP::setPtrRelease(&lastWritten->d_next, n);
        ATOMIC_OP::setPtrRelease(&d_lastWritten, n);

        // While allocating this node, it is possible the queue went from full
        // to empty.  If the queue is empty when this node is added,
        // 'd_nextRead' might not be pointing to this new node.  Spin wait for
        // 'e_WRITABLE == lastWritten->d_state', and then if
        // 'd_nextRead == target' set to 'd_nextRead' to 'n'.

        if (0 >= ATOMIC_OP::getIntAcquire(&d_numElements)) {
            while (e_WRITABLE !=
                             ATOMIC_OP::getIntAcquire(&lastWritten->d_state)) {
                bslmt::ThreadUtil::yield();
            }
            ATOMIC_OP::testAndSwapPtrAcqRel(&d_nextRead,
                                            target,
                                            n);
        }
    }
    else {
        bslalg::ScalarPrimitives::copyConstruct(target->d_value.address(),
                                                value,
                                                d_allocator_p);

        ATOMIC_OP::setIntRelease(&target->d_state, e_READABLE);

        ATOMIC_OP::setPtrRelease(&d_lastWritten, target);
    }

    if (0 >= ATOMIC_OP::addIntNvAcqRel(&d_numElements, 1)) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_readMutex);
            ++d_readRelease;
        }
        d_readCondition.signal();
    }

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::pushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    if (0 != ATOMIC_OP::getIntAcquire(&d_pushBackDisabled)) {
        return -1;                                                    // RETURN
    }

    Node *lastWritten = static_cast<Node *>(
                                     ATOMIC_OP::getPtrAcquire(&d_lastWritten));
    Node *target      = static_cast<Node *>(
                               ATOMIC_OP::getPtrAcquire(&lastWritten->d_next));

    if (e_WRITABLE != ATOMIC_OP::getIntAcquire(&target->d_state)) {
        Node *n = static_cast<Node *>(d_allocator_p->allocate(sizeof(Node)));

        bslma::DeallocatorProctor<bslma::Allocator> proctor(n, d_allocator_p);

        TYPE& dummy = value;
        bslalg::ScalarPrimitives::moveConstruct(n->d_value.address(),
                                                dummy,
                                                d_allocator_p);

        proctor.release();

        ATOMIC_OP::initInt(&n->d_state, e_READABLE);
        ATOMIC_OP::initPointer(&n->d_next, target);

        ATOMIC_OP::setPtrRelease(&lastWritten->d_next, n);
        ATOMIC_OP::setPtrRelease(&d_lastWritten, n);

        // While allocating this node, it is possible the queue went from full
        // to empty.  If the queue is empty when this node is added,
        // 'd_nextRead' might not be pointing to this new node.  Spin wait for
        // 'e_WRITABLE == lastWritten->d_state', and then if
        // 'd_nextRead == target' set to 'd_nextRead' to 'n'.

        if (0 >= ATOMIC_OP::getIntAcquire(&d_numElements)) {
            while (e_WRITABLE !=
                             ATOMIC_OP::getIntAcquire(&lastWritten->d_state)) {
                bslmt::ThreadUtil::yield();
            }
            ATOMIC_OP::testAndSwapPtrAcqRel(&d_nextRead,
                                            target,
                                            n);
        }
    }
    else {
        TYPE& dummy = value;
        bslalg::ScalarPrimitives::moveConstruct(target->d_value.address(),
                                                dummy,
                                                d_allocator_p);

        ATOMIC_OP::setIntRelease(&target->d_state, e_READABLE);

        ATOMIC_OP::setPtrRelease(&d_lastWritten, target);
    }

    if (0 >= ATOMIC_OP::addIntNvAcqRel(&d_numElements, 1)) {
        {
            bslmt::LockGuard<MUTEX> guard(&d_readMutex);
            ++d_readRelease;
        }
        d_readCondition.signal();
    }

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPopFront(
                                                                   TYPE *value)
{
    if (0 != ATOMIC_OP::getIntAcquire(&d_popFrontDisabled)) {
        return e_DISABLED;                                            // RETURN
    }

    int numElements = ATOMIC_OP::getIntAcquire(&d_numElements);
    int expNumElements;
    do {
        if (numElements <= 0) {
            return e_EMPTY;                                           // RETURN
        }

        expNumElements = numElements;
        numElements    = ATOMIC_OP::testAndSwapIntAcqRel(&d_numElements,
                                                         numElements,
                                                         numElements - 1);
    } while (numElements != expNumElements);

    popFrontRaw(value, 1 == numElements);

    return 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPushBack(
                                                             const TYPE& value)
{
    return pushBack(value);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::tryPushBack(
                                                 bslmf::MovableRef<TYPE> value)
{
    return pushBack(bslmf::MovableRefUtil::move(value));
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::removeAll()
{
    int numElements = ATOMIC_OP::getIntAcquire(&d_numElements);
    int expNumElements;
    do {
        if (numElements <= 0) {
            return;                                                   // RETURN
        }
        expNumElements = numElements;
        numElements    = ATOMIC_OP::testAndSwapIntAcqRel(&d_numElements,
                                                         numElements,
                                                         0);
    } while (numElements != expNumElements);

    for (int i = 0; i < numElements; ++i) {
        Node *readFrom =
                    static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&d_nextRead));
        Node *exp;
        do {
            Node *next =
              static_cast<Node *>(ATOMIC_OP::getPtrAcquire(&readFrom->d_next));

            exp      = readFrom;
            readFrom = static_cast<Node *>(ATOMIC_OP::testAndSwapPtrAcqRel(
                                                                   &d_nextRead,
                                                                   readFrom,
                                                                   next));
        } while (readFrom != exp);

        readFrom->d_value.object().~TYPE();

        ATOMIC_OP::setIntRelease(&readFrom->d_state, e_WRITABLE);
    }

    {
        bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
    }
    d_emptyCondition.signal();
}

                       // Enqueue/Dequeue State

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                            ::disablePopFront()
{
    ATOMIC_OP::setIntRelease(&d_popFrontDisabled, 1);

    {
        bslmt::LockGuard<MUTEX> guard(&d_readMutex);
    }
    d_readCondition.broadcast();

    {
        bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);
    }
    d_emptyCondition.broadcast();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                            ::disablePushBack()
{
    ATOMIC_OP::setIntRelease(&d_pushBackDisabled, 1);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                             ::enablePopFront()
{
    ATOMIC_OP::setIntRelease(&d_popFrontDisabled, 0);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
void SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>
                                                             ::enablePushBack()
{
    ATOMIC_OP::setIntRelease(&d_pushBackDisabled, 0);
}

// ACCESSORS
template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                                isEmpty() const
{
    return 0 >= numElements();
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                     isPopFrontDisabled() const
{
    return 0 != ATOMIC_OP::getIntAcquire(&d_popFrontDisabled);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bool SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                     isPushBackDisabled() const
{
    return 0 != ATOMIC_OP::getIntAcquire(&d_pushBackDisabled);
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bsl::size_t SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                            numElements() const
{
    int numElements = ATOMIC_OP::getIntAcquire(&d_numElements);

    return numElements >= 0 ? static_cast<bsl::size_t>(numElements) : 0;
}

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
int SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
                                                         waitUntilEmpty() const
{
    bslmt::LockGuard<MUTEX> guard(&d_emptyMutex);

    while (0 < ATOMIC_OP::getIntAcquire(&d_numElements)) {
        if (0 != ATOMIC_OP::getIntAcquire(&d_popFrontDisabled)) {
            return -1;                                                // RETURN
        }
        d_emptyCondition.wait(&d_emptyMutex);
    }

    return 0;
}

                                  // Aspects

template <class TYPE, class ATOMIC_OP, class MUTEX, class CONDITION>
bslma::Allocator *SingleProducerQueueImpl<TYPE, ATOMIC_OP, MUTEX, CONDITION>::
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
