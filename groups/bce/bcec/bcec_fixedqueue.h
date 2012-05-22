// bcec_fixedqueue.h                                                  -*-C++-*-
#ifndef INCLUDED_BCEC_FIXEDQUEUE
#define INCLUDED_BCEC_FIXEDQUEUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled blocking fixed-size queue of values.
//
//@CLASSES:
//  bcec_FixedQueue: thread-enabled fixed-size queue of 'TYPE' values
//
//@AUTHOR: Ilougino Rocha (irocha), Vlad Kliatchko (vkliatch)
//
//@DESCRIPTION: This component implements a very efficient, thread-enabled
// fixed-size queue of values.  Its semantics are identical to 'bcec_Queue'
// with a high-water mark set, though the API is limited.  This class is
// ideal for synchronization and communication between threads in a
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
///TEMPLATE REQUIREMENTS
///---------------------
// 'bcec_FixedQueue' is a template which stores items of a parameterized
// 'TYPE'.
//
// If 'TYPE' is a pointer type, 'bcec_FixedQueue' is specialized to store
// pointers by value.  In this case, the underlying objects are never copied;
// only the pointer values are stored.  Also, storing arbitrary pointers to
// memory not owned by the client (or null pointers) is *not* supported.  Only
// addresses allocated to the client may be stored in 'bcec_FixedQueue'.
//
// If 'TYPE' is a non-pointer type, it must supply a copy constructor.  Also,
// if the copy constructor accepts a 'bslma_Allocator*', 'TYPE' must declare
// the Uses Allocator trait (see 'bslalg_TypeTraits') so that the allocator of
// the queue is properly propagated.
//
///Usage
///-----
///Example: Simple Thread Pool
///- - - - - - - - - - - - - -
// The following example demonstrates a typical usage of a 'bcec_FixedQueue'.
//
// This 'bcec_FixedQueue' is used to communicate between a single "producer"
// thread and multiple "consumer" threads.  The "producer" will push work
// requests onto the queue, and each "consumer" will iteratively take a work
// request from the queue and service the request.  This example shows a
// partial, simplified implementation of the 'bcep_ThreadPool' class.  See
// component 'bcep_threadpool' for more information.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
// enum {
//     MAX_CONSUMER_THREADS = 10
// };
//
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
// 'queue->popFront()', and 'bcec_FixedQueue' guarantees that each thread
// gets a unique element from the queue:
//..
// void myConsumer(bcec_FixedQueue<my_WorkRequest> *queue)
// {
//     while (1) {
//         // 'popFront()' will wait for a 'my_WorkRequest' until available.
//         my_WorkRequest item = queue->popFront();
//         if (item.d_type == my_WorkRequest::STOP) { break; }
//         myDoWork(item.d_data);
//     }
// }
//..
// The function below is a callback for 'bcemt_ThreadUtil', which requires a
// "C" signature.  'bcemt_ThreadUtil::create()' expects a pointer to this
// function, and provides that function pointer to the newly created thread.
// The new thread then executes this function.
//
// Since 'bcemt_ThreadUtil::create()' uses the familiar "C" convention of
// passing a 'void' pointer, our function simply casts that pointer to our
// required type ('bcec_FixedQueue<my_WorkRequest*> *'), and then delegates to
// the queue-specific function 'myConsumer()', above:
//..
// extern "C" void *myConsumerThread(void *queuePtr)
// {
//     myConsumer ((bcec_FixedQueue<my_WorkRequest *>*) queuePtr);
//     return queuePtr;
// }
//..
// In this simple example, the 'myProducer' function serves multiple roles: it
// creates the 'bcec_FixedQueue', starts the consumer threads, and then
// produces and enqueues work items.  When work requests are exhausted, this
// function enqueues one 'STOP' item for each consumer queue.
//
// When each consumer thread reads a 'STOP', it terminates its thread-handling
// function.  Note that, although the producer cannot control which thread
// 'pop's a particular work item, it can rely on the knowledge that each
// consumer thread will read a single 'STOP' item and then terminate.
//
// Finally, the 'myProducer' function "joins" each consumer thread, which
// ensures that the thread itself will terminate correctly; see the
// 'bcemt_thread' component for details:
//..
// void myProducer(int numThreads)
// {
//     myWorkRequest item;
//     my_WorkData workData;
//
//     bcec_FixedQueue<my_WorkRequest> queue;
//
//     assert(0 < numThreads && numThreads <= MAX_CONSUMER_THREADS);
//     bcemt_ThreadUtil::Handle consumerHandles[MAX_CONSUMER_THREADS];
//
//     for (int i = 0; i < numThreads; ++i) {
//         bcemt_ThreadUtil::create(&consumerHandles[i],
//                                  myConsumerThread,
//                                  &queue);
//     }
//
//     while (!getWorkData(&workData)) {
//         item.d_type = my_WorkRequest::WORK;
//         item.d_data = workData;
//         queue.pushBack(item);
//     }
//
//     for (int i = 0; i < numThreads; ++i) {
//         item.d_type = my_WorkRequest::STOP;
//         queue.pushBack(item);
//     }
//
//     for (int i = 0; i < numThreads; ++i) {
//         bcemt_ThreadUtil::join(consumerHandles[i]);
//     }
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_FIXEDPOOL
#include <bcema_fixedpool.h>
#endif

#ifndef INCLUDED_BCEMT_SEMAPHORE
#include <bcemt_semaphore.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTORPROCTOR
#include <bslma_destructorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_RAWDELETERGUARD
#include <bslma_rawdeleterguard.h>
#endif

#ifndef INCLUDED_BSLMA_RAWDELETERPROCTOR
#include <bslma_rawdeleterproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                   // ======================================
                   // local class bcec_FixedQueue_IndexQueue
                   // ======================================

class bcec_FixedQueue_IndexQueue {
    // This private class provides a lock-free fixed-size queue of integers in
    // the range from zero to a configurable upper limit.  It provides the
    // underlying implementation for 'bcec_FixedQueue'.

    // PUBLIC CONSTANTS
  public:
    enum {
        // We'll pad certain data members so they're stored on different
        // cache lines to prevent false sharing.
        //
        // For POWER cpus:
        // http://www.ibm.com/developerworks/power/library/pa-memory/index.html
        // has a simple program to determine the cache line size of the CPU.
        // Current Power cpus have 128-byte cache lines.
        //
        // On Solaris, to determine the cache line size on the local cpu, run:
        // ..
        //   prtconf -pv | grep -i l1-dcache-line-size | sort -u
        // ..
        // Older sparc cpus have 32-byte cache lines, newer 64-byte cache
        // lines.  We'll assume 64-byte cache lines here.
        //
        // On Linux with 'sysfs' support,
        //..
        //  cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
        //..
        // or
        //..
        //  cat /proc/cpuinfo | grep cache
        //..
        // Post SSE2 cpus have the clflush instruction which can be used to
        // write a program similar to the one mentioned above for POWER cpus.
        // Current x86/x86_64 have 64-byte cache lines.
        //
        // It is obviously suboptimal to determine this at compile time.  We
        // might want to do this at runtime, but this would add at least one
        // level of indirection.

#ifdef BSLS_PLATFORM__CPU_POWERPC
        BCEC_PAD = 128 - sizeof(bces_AtomicInt)
#else
        BCEC_PAD = 64 - sizeof(bces_AtomicInt)
#endif
    };

  private:
    // DATA
    const unsigned d_indexBits;      // Mask of the bits containing the
                                     // index stored in 'd_front' and
                                     // 'd_back'.

    const unsigned d_numIndexBits;   // Number of bits set in 'd_indexBits'.

    const unsigned d_dataBits;       // Mask of the bits containing the
                                     // integer identifier passed to
                                     // 'tryPushBack' stored in 'd_data'.

    const unsigned d_genCountOffset; // Number of bits set in 'd_dataBits'
                                     // plus 1.  Implicitly representing
                                     // the number of bits set in
                                     // 'd_dataBits | d_sentinel'.

    const unsigned d_sentinel;       // Disable sentinel value.  It used
                                     // the rightmost unused bit in
                                     // 'd_dataBits'.

    int            d_backoffLevel;

    bsl::vector<bces_AtomicInt>
                   d_data;

    const char     d_dataPad[BCEC_PAD];  // keep 'd_data'  and 'd_back' on
                                         // separate cache lines
                                         // (performance only)

    bces_AtomicInt d_back;               // index of the back of the queue
                                         // ORed with a generation count

    const char     d_indexPad[BCEC_PAD]; // keep 'd_back'  and 'd_front' on
                                         // separate cache lines
                                         // (performance only)

    bces_AtomicInt d_front;              // index of the front of the queue
                                         // in 'd_data' ORed with a generation
                                         // count

  private:
    // NOT IMPLEMENTED
    bcec_FixedQueue_IndexQueue(const bcec_FixedQueue_IndexQueue&);
    bcec_FixedQueue_IndexQueue& operator=(const bcec_FixedQueue_IndexQueue&);

  public:
    // CREATORS
    bcec_FixedQueue_IndexQueue(int              capacity,
                               int              range,
                               bslma_Allocator *basicAllocator = 0);
        // Create an index queue with the specified 'capacity' for indices in
        // the range from 0 up to but not including 'range'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bcec_FixedQueue_IndexQueue();
        // Destroy this queue.

    // MANIPULATORS
   int tryPushBack(int data);
        // Append the specified 'data' to the back of this queue.  If the
        // number of elements in this queue is equal to the effective queue
        // capacity (the effective queue capacity is greater or equal to
        // the 'capacity' that was supplied at construction), or if the queue
        // is disabled, fail immediately.
        // Return 0 on success, and a non-zero value otherwise.  The behavior
        // is undefined unless 'data' is greater or equal to 0 and less then
        // the 'range' supplied at construction.  This method does not throw
        // exceptions.

    int tryPopFront(int *data);
        // Remove the element from the front of this queue and load that
        // element into the specified 'data'.  If this queue is empty, fail
        // immediately.  Return 0 on success, and a non-zero value
        // otherwise.  This method does not throw exceptions.

    void disable();
        // Disable this queue.  All subsequent invocations of 'tryPushBack'
        // will fail immediately without blocking.  If the queue is already
        // disabled, this method has no effect.  This method does not throw
        // exceptions.

    void enable();
        // Enable queuing.  If the queue is not disabled, this call has no
        // effect.  This method does not throw exceptions.

    void setBackoffLevel(int backoffLevel);
        // Configure this queue with a non-negative 'backoffLevel' that
        // controls the amount of spinning that happens when calls to this
        // queue encounter contention.  Setting 'backoffLevel' to 0 disables
        // spinning.
        // Greater values of 'backoffLevel' correspond to greater amounts of
        // spinning.  Both contention detection and spinning strategy are
        // implementation defined.

    // ACCESSORS
    int length() const;
        // Return the number of indices currently in this queue.  Note that the
        // current implementation provide a release barrier before accessing
        // any data.  This implementation detail is used by
        // 'bcec_FixedQueue_IndexQueue::pusherWait'.

    bool isEmpty() const;
        // Return 'true' if this queue is empty (has no elements), or 'false'
        // otherwise.  Note that the current implementation provide a release
        // barrier before accessing any data.  This implementation detail
        // is used by 'bcec_FixedQueue_IndexQueue::popperWait'.

    bool isEnabled() const;
        // Return 'true' if this queue is enabled, and 'false' otherwise.  Note
        // that the queue is created in the "enabled" state.

    int backoffLevel() const;
        // Return a non-negative 'backoffLevel' that controls the amount of
        // spinning that happens when calls to this queue encounter contention.

};

                            // =====================
                            // class bcec_FixedQueue
                            // =====================

template <typename TYPE>
class bcec_FixedQueue {
    // This class provides a thread-enabled implementation of a very efficient
    // fixed-size queue of templatized 'TYPE' values.

    // PRIVATE TYPES
    typedef bcec_FixedQueue_IndexQueue IndexQ;

    // DATA
    IndexQ           d_queue;

    const char       d_semaPad[IndexQ::BCEC_PAD];  // keep 'd_queue' separate
                                                   // from semaphores' cache
                                                   // line (performance only)

    bcemt_Semaphore  d_semaWaitingPushers;
    bces_AtomicInt   d_numWaitingPushers;
    bcemt_Semaphore  d_semaWaitingPoppers;
    bces_AtomicInt   d_numWaitingPoppers;

    bcema_FixedPool  d_pool;

    const int        d_size;
    bslma_Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED
    bcec_FixedQueue(const bcec_FixedQueue&);
    bcec_FixedQueue& operator=(const bcec_FixedQueue&);

    // PRIVATE MANIPULATORS
    void popperWait();
        // Wait until the queue is not empty.  This method should be invoked
        // by popping threads after a tryPopFront() call has failed.

    void pusherWait();
        // Wait until the queue is not full.  This method should be invoked
        // by pushing threads after a tryPushBack() call has failed.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcec_FixedQueue,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bcec_FixedQueue(int size, bslma_Allocator *basicAllocator = 0);
        // Create a queue having the specified 'size'.  Use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless '0 < size' and 'size <= 0x01FFFFFF'.

    ~bcec_FixedQueue();
        // Destroy this queue.

    // MANIPULATORS
    int pushBack(const TYPE& object);
        // Append the specified 'object' to the back of this queue.  If the
        // number of items in this queue is greater than or equal to the queue
        // size, then block until the number of items is less than the
        // capacity.  Return 0 on success, and a nonzero value if the queue
        // is disabled.  The behavior is undefined if 'TYPE' is a pointer
        // type and 'object' is not a valid pointer to memory owned by the
        // caller.

    int tryPushBack(const TYPE& object);
        // Append the specified 'object' to the back of this queue.  If the
        // number of items in this queue is greater than or equal to the queue
        // size, or if the queue is disabled, fail immediately.  Return 0
        // on success, and a non-zero value otherwise.  The behavior is
        // undefined if 'TYPE' is a pointer type and 'object' is not a valid
        // pointer to memory owned by the caller.

    void popFront(TYPE *buffer);
    TYPE popFront();
        // Remove the first item in this queue and return that value.  If the
        // queue is empty, block until an item becomes available.

    int tryPopFront(TYPE *buffer);
        // Remove the first item in this queue and load that item value into
        // the specified 'buffer'.  If the queue is empty, fail immediately.
        // Return 0 on success, and a non-zero value otherwise.

    void removeAll();
        // Remove all items from this queue.  Note that this operation is not
        // atomic; if other threads are concurrently pushing items into the
        // queue the result of 'length()' after this function returns is not
        // guaranteed to be 0.

    void disable();
        // Disable this queue.  All subsequent invocations of 'pushBack' or
        // 'tryPushBack' will fail immediately without blocking.  If the queue
        // is already disabled, this method has no effect.

    void enable();
        // Enable queuing.  If the queue is not disabled, this call has no
        // effect.

    void setBackoffLevel(int backoffLevel);
        // Configure this queue with a non-negative 'backoffLevel' that
        // controls the amount of spinning that happens when calls to this
        // queue encounter contention.  Setting 'backoffLevel' to 0 disables
        // spinning.
        // Greater values of 'backoffLevel' correspond to greater amounts of
        // spinning.  Both contention detection and spinning strategy are
        // implementation defined.

    // ACCESSORS
    int size() const;
        // Return the size of this queue.  Note that the size is the
        // 'size' that was supplied at construction.

    int length() const;
        // Return the number of elements currently in this queue.

    bool isFull() const;
        // Return 'true' when this queue is full (i.e., when the number of
        // elements currently in this queue equals its size), or 'false'
        // otherwise.

    bool isEmpty() const;
        // Return 'true' if this queue is empty (has no elements), or 'false'
        // otherwise.

    bool isEnabled() const;
        // Return 'true' if this queue is enabled, and 'false' otherwise.  Note
        // that the queue is created in the "enabled" state.

    int backoffLevel() const;
        // Return a non-negative 'backoffLevel' that controls the amount of
        // spinning that happens when calls to this queue encounter contention.
};

// =====================================================================
//                        INLINE FUNCTION DEFINITIONS
// =====================================================================

                    // --------------------------------------
                    // local class bcec_FixedQueue_IndexQueue
                    // --------------------------------------

// MANIPULATORS
inline
void bcec_FixedQueue_IndexQueue::setBackoffLevel(int backoffLevel)
{
    d_backoffLevel = backoffLevel;
}

// ACCESSORS
inline
int bcec_FixedQueue_IndexQueue::backoffLevel() const
{
    return d_backoffLevel;
}

inline
bool bcec_FixedQueue_IndexQueue::isEmpty() const
{
    // Note: 'bcec_FixedQueue::popperWait' relies on the fact that the
    // following atomic load has a release barrier.  If this were to change,
    // 'bcec_FixedQueue::popperWait' would need to be modified.

    const unsigned back = d_back;
    const unsigned front = d_front;
    const int length = back - front;
    return length <= 0;
}

inline
int bcec_FixedQueue_IndexQueue::length() const
{
    // Note: 'bcec_FixedQueue::pusherWait' relies on the fact that the
    // following atomic load has a release barrier.  If this were to change,
    // 'bcec_FixedQueue::pusherWait' would need to be modified.

    const unsigned back = d_back;
    const unsigned front = d_front;
    const int length = back - front;
    return bsl::max(length, 0);
}

                         // ---------------------------
                         // class bcec_FixedQueue<TYPE>
                         // ---------------------------

// CREATORS
template <typename TYPE>
bcec_FixedQueue<TYPE>::bcec_FixedQueue(int              size,
                                       bslma_Allocator *basicAllocator)
: d_queue(size, size, basicAllocator)
, d_semaPad()
, d_numWaitingPushers(0)
, d_numWaitingPoppers(0)
, d_pool(sizeof(TYPE), size, basicAllocator)
, d_size(size)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT_OPT(0 < size);
    BSLS_ASSERT_OPT(0x1FFFFFF >= size);
}

template <typename TYPE>
inline
bcec_FixedQueue<TYPE>::~bcec_FixedQueue()
{
    removeAll();
}

// PRIVATE MANIPULATORS
template <typename TYPE>
void bcec_FixedQueue<TYPE>::popperWait()
{
    // This can be relaxed because 'isEmpty()' emits a release barrier before
    // accessing any data.  If its implementation were to change, the addition
    // would need this barrier.

    d_numWaitingPoppers.relaxedAdd(1);

    if (isEmpty()) {
        d_semaWaitingPoppers.wait();
    }

    d_numWaitingPoppers.relaxedAdd(-1);
}

template <typename TYPE>
void bcec_FixedQueue<TYPE>::pusherWait()
{
    // This can be relaxed because 'isFull()' emits a release barrier before
    // accessing any data.  If its implementation were to change, the addition
    // would need this barrier.

    d_numWaitingPushers.relaxedAdd(1);

    if (isFull() && isEnabled()) {
        d_semaWaitingPushers.wait();
    }

    d_numWaitingPushers.relaxedAdd(-1);
}

// MANIPULATORS
template <typename TYPE>
inline
void bcec_FixedQueue<TYPE>::enable()
{
    d_queue.enable();
}

template <typename TYPE>
void bcec_FixedQueue<TYPE>::disable()
{
    d_queue.disable();
    const int numWaitingPushers = d_numWaitingPushers;

    for (int i = 0; i < numWaitingPushers; ++i) {
        // Wake up waiting pushers.

        d_semaWaitingPushers.post();
    }
}

template <typename TYPE>
inline
void bcec_FixedQueue<TYPE>::popFront(TYPE *buffer)
{
    while (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(tryPopFront(buffer))) {
        // The queue is empty.

        popperWait();
    }
}

template <typename TYPE>
TYPE bcec_FixedQueue<TYPE>::popFront()
{
    int index;
    while (d_queue.tryPopFront(&index)) {
        // The queue is empty.

        popperWait();
    }

    TYPE *const resultPtr = reinterpret_cast<TYPE *>(
                                               d_pool.addressFromIndex(index));
    BSLS_ASSERT(resultPtr);

    bslma_RawDeleterProctor<TYPE, bcema_FixedPool> deleterProctor(resultPtr,
                                                                  &d_pool);

    // We need to create a temporary and manually release the original object
    // back to the pool so that if we wake up waiting pushers, they have memory
    // to use.

    TYPE result(*resultPtr);  // may throw...deleterProctor protects us

    deleterProctor.release();
    resultPtr->~TYPE();
    d_pool.deallocate(resultPtr);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPushers)) {
        // Wake up waiting pushers.

        d_semaWaitingPushers.post();
    }

    return result;
}

template <typename TYPE>
int bcec_FixedQueue<TYPE>::pushBack(const TYPE& object)
{
    while (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(tryPushBack(object))) {

        if (!d_queue.isEnabled()) {
            return -2;
        }

        // The queue is full.
        pusherWait();
    }

    return 0;
}

template <typename TYPE>
void bcec_FixedQueue<TYPE>::removeAll()
{
    const int numItems = length();
    int poppedItems = 0;

    while (poppedItems++ < numItems) {
        int index;
        if (d_queue.tryPopFront(&index)) {
            break;
        }

        TYPE * const s = reinterpret_cast<TYPE *>(
                                               d_pool.addressFromIndex(index));
        BSLS_ASSERT(s);
        s->~TYPE();
        d_pool.deallocate(s);
    }

    int numWakeUps = bsl::min(poppedItems, (int) d_numWaitingPushers);
    while (numWakeUps--) {
        // Wake up waiting pushers.

        d_semaWaitingPushers.post();
    }
}

template <typename TYPE>
inline
void bcec_FixedQueue<TYPE>::setBackoffLevel(int backoffLevel)
{
    d_pool.setBackoffLevel(backoffLevel);
    d_queue.setBackoffLevel(backoffLevel);
}

template <typename TYPE>
int bcec_FixedQueue<TYPE>::tryPopFront(TYPE *buffer)
{
    int index;
    const int ret = d_queue.tryPopFront(&index);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(ret)) {
        return ret;
    }

    // Successfully popped an item from the front of the queue.

    TYPE * const s = reinterpret_cast<TYPE *>(d_pool.addressFromIndex(index));
    BSLS_ASSERT(s);

    {
        bslma_RawDeleterGuard<TYPE, bcema_FixedPool> deleterGuard(s, &d_pool);
        *buffer = *s;  // may throw - deleterGuard protects us

        // memory released to the pool here, before waking pushers
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_numWaitingPushers)) {
        // Wake up waiting pushers.

        d_semaWaitingPushers.post();
    }

    return 0;
}

template <typename TYPE>
int bcec_FixedQueue<TYPE>::tryPushBack(const TYPE& object)
{
    TYPE * const s = reinterpret_cast<TYPE*>(d_pool.allocate());
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(s == 0)) {
        return -1;
    }
    bslma_DeallocatorProctor<bcema_FixedPool> deallocatorProctor(s, &d_pool);

    bslalg_ScalarPrimitives::copyConstruct(s, object, d_allocator_p);
    bslma_DestructorProctor<TYPE> destructorProctor(s);

    const int index = d_pool.indexFromAddress(s);
    const int ret = d_queue.tryPushBack(index);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 == ret)) {
        // Successfully pushed back the object.

        destructorProctor.release();
        deallocatorProctor.release();

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(d_numWaitingPoppers)) {
            // Wake up waiting poppers.

            d_semaWaitingPoppers.post();
         }
    }

    return ret;
}

// ACCESSORS
template <typename TYPE>
inline
int bcec_FixedQueue<TYPE>::backoffLevel() const
{
    return d_queue.backoffLevel();
}

template <typename TYPE>
inline
int bcec_FixedQueue<TYPE>::size() const
{
    return d_size;
}

template <typename TYPE>
inline
int bcec_FixedQueue<TYPE>::length() const
{
    return d_queue.length();
}

template <typename TYPE>
inline
bool bcec_FixedQueue<TYPE>::isFull() const
{
    return d_queue.length() >= d_size;
}

template <typename TYPE>
inline
bool bcec_FixedQueue<TYPE>::isEmpty() const
{
    return d_queue.isEmpty();
}

template <typename TYPE>
inline
bool bcec_FixedQueue<TYPE>::isEnabled() const
{
    return d_queue.isEnabled();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
