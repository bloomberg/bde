// bdlcc_deque.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLCC_DEQUE
#define INCLUDED_BDLCC_DEQUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a fully thread-safe deque container.
//
//@CLASSES:
//  bdlcc::Deque: thread-safe 'bsl::deque' wrapper
//
//@SEE_ALSO: bsl::deque
//
//@DESCRIPTION: This component provides 'bdlcc::Deque<TYPE>', a fully
// thread-safe implementation of an efficient, double-ended queue of
// (template parameter) 'TYPE' values.  'bdlcc::Deque' is effectively a
// thread-safe wrapper for 'bsl::deque', whose interface is also made available
// through proctor types that are nested classes.
//
///Thread Safety
///-------------
// 'bdlcc::Deque' is fully *thread-safe*, meaning that all non-creator
// operations on an object can be safely invoked simultaneously from multiple
// threads.
//
///Exception Safety
///----------------
// Provided the template parameter 'TYPE' provides the following exception
// safety guarantees:
//: 1 The destructor provides the no-throw guarantee.
//: 2 Copy construction and assignment provide the strong guarantee and do not
//:   modify the source.
//: 3 Move construction and assignment where the allocators of the source and
//:   destination match, or if the type is non-allocating, provide the no-throw
//:   guarantee.
//: 4 Move construction and assignment where the allocators of source and
//:   destination do not match behave like non-moving copy construction and
//:   assignment.
// All operations on 'bdlcc::Deque' provide the strong exception guarantee,
// both for the 'bdlcc::Deque's own salient state and the salient state of the
// 'bsl::vector', if any, passed to manipulators.  However, the non-salient
// 'capacity' of the underlying 'bsl::deque' and of the passed 'bsl::vector'
// may be modified.
//
///Design Rationale for 'bdlcc::Deque'
///-----------------------------------
// The fully thread-safe 'bdlcc::Deque' is similar to 'bsl::deque' in many
// regards, but there are several differences in method behavior and signature
// that arise due to the thread-aware nature of the container and its
// anticipated usage pattern.
//
// A user of 'bsl::deque' is expected to consult the 'size' or 'empty'
// accessors before reading or popping to determine whether elements are
// available in the container to be read or popped.  This won't work in a
// multithreaded context since reading the accessor is a separate operation
// than the read or pop, and another thread may have altered the state of the
// container in between.
//
// So we have eliminated the 'front', 'back' and random-access methods.
// Reading is done from the ends of the container via the 'popFront' and
// 'popBack' methods, which return a 'TYPE' object *by* *value*, rather than
// returning 'void', as 'pop_front' and 'pop_back' in 'bsl::deque' do.
// Moreover, if a 'bdlcc::Deque' object is empty, 'popFront' and 'popBack' will
// block indefinitely until an item is added to the container.
//
///'High-Water Mark' Feature
///-------------------------
// The behaviors of the 'push' methods differ from those of 'bsl::deque' in
// that they can block under certain circumstances.  'bdlcc::Deque' supports
// the notion of a *suggested* maximum capacity known as the *high-water*
// *mark*.  The high-water mark value is supplied at construction, and affects
// some of the various forms of 'push*' methods.  The container is considered
// to be *full* if it contains (at least) the high-water mark number of items,
// and the container has *space* *available* if it is not full.  The high-water
// mark is set at construction and cannot be changed afterward.  If no
// high-water mark is specified, the high-water mark of the container is
// effectively inifinite.  Some of the variants of push operations (described
// below) may fail, and the return status of those operations indicates whether
// the operation succeeded, failed, or partially succeeded (which may happen,
// for example, when pushing a range of values).
//
// 'bdlcc::Deque' supports four variants of the two 'push' methods, whose
// behaviors differ when the container is *full* (i.e. when the push would
// raise the length of the container above the high-water mark).
//
//: 1 *blocking*: ('pushBack', 'pushFront'): If the container is full, block
//:   until space is available, then push, otherwise push immediately.
//:
//: 2 *try* ('tryPushBack', 'tryPushFront'): If the container is full, fail
//:   immediately.  If space is available, succeed immediately.  Note that
//:   partial success is possible in the case of a range try push.
//:
//: 3 *timed* *blocking*: ('timedPushBack', 'timedPushFront'): If the container
//:   is full, block until either space is available or the specified timeout
//:   has been reached.  If space was, or became, available, push and succeed,
//:   otherwise fail.
//:
//: 4 *force*: ('forcePushBack', 'forcePushFront'): If the container is full,
//:   push anyway, increasing the container's size above its high-water mark,
//:   always succeeding immediately.
//
// Note that the availability of force pushes means that the high-water mark is
// a suggestion and not an invariant.
//
// The purpose of a high-water mark is to enable the client to use the
// container as a fixed-length container, where pushes that will grow it above
// a certain size will block.  The purpose of the force pushes is to allow
// high-priority items to be pushed regardless of whether the container is
// full.
//
///Proctor Access
///--------------
// There are public nested classes 'bdlcc::Deque::Proctor' and
// 'bdlcc::Deque::ConstProctor' through which the client can directly access
// the underlying 'bsl::deque' contained in the 'bdlcc::Deque'.  When a proctor
// object is created, it acquires the container's mutex, and allows the client
// to use the overloaded '->' and '*' operators on the proctor object to access
// the underlying 'bsl::deque'.  'operator[]' is also provided for direct
// random access to that deque.  Because the mutex is locked, manipulators of
// 'bdlcc::Deque' called by other threads will block, thus allowing safe access
// to the underlying thread-unsafe container.  When the proctor is destroyed
// (or released via the 'release' method), the proctor signals the thread-aware
// container's condition variables to inform manipulators in other threads of
// new items being available for pops or new space becoming available for
// pushes.
//
///Supported Clock-Types
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which the 'timedPush*' and 'timedPop*' methods should be
// based.  If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', time should be expressed as an absolute
// offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch used in
// 'bdlt::CurrentTime::now(bsls::SystemClockType::e_REALTIME)'.  If the clock
// type indicated at construction is 'bsls::SystemClockType::e_MONOTONIC', time
// should be expressed as an absolute offset since the epoch of this clock
// (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///WARNING: Synchronization Required on Destruction
///------------------------------------------------
// The behavior for the destructor is undefined unless all access or
// modification of the object is completed prior to its destruction.  Some form
// of synchronization, external to the component, is required to ensure this
// precondition on the destructor is met.  For example, if two (or more)
// threads are manipulating a queue, it is *not* safe to anticipate the number
// of elements added to the queue, and destroy that queue immediately after the
// last element is popped (without additional synchronization) because one of
// the corresponding push functions may not have completed (push may, for
// instance, signal waiting threads after the element is considered added to
// the queue).
//
///Tips For Migrating From 'bcec_Queue'
///------------------------------------
//: o 'InitialCapacity' has been eliminated.  Instead, construct your
//:   'bdlcc::Deque' object and then use proctor access to call 'reserve' on
//:   the contained 'bsl::deque' to reserve the desired initial capacity.
//:   (Note that 'deque::reserve' is not part of the C++ standard, though
//:   'bsl::deque' does implement it).
//:
//: o The mutex and condition variables are no longer directly exposed, in
//:   favor of the new proctor access, which gives direct access to the
//:   underlying 'bsl::deque', automatically locking the mutex and updating the
//:   condition variables as necessary.
//:
//: o A new, thread-safe 'length' accessor is provided, eliminating the need to
//:   access the underlying thread-unsafe container to obtain its length.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Queue of Work Requests
///- - - - - - - - - - - - - - - - - -
// First, declarer the struct 'WordData'.  Imagine it contains some data one
// wants to process:
//..
//  struct WorkData {
//      // work data...
//  };
//..
// Then, create the function that will produce a 'WorkData' object:
//..
//  bool getWorkData(WorkData *)
//      // Dummy implementation of 'getWorkData' function required by the usage
//      // example.
//  {
//      static bsls::AtomicInt i(1);
//      return ++i < 1000;
//  }
//..
// Next, declare 'WorkRequest', the type of object that will be stored in
// the container:
//..
//  struct WorkRequest {
//      // PUBLIC TYPES
//      enum RequestType {
//          e_WORK = 1,
//          e_STOP = 2
//      };
//
//      // PUBLIC DATA
//      RequestType d_type;
//      WorkData d_data;
//  };
//..
// Then, create the function that will do work on a 'WorkRequest' object:
//..
//  void doWork(WorkData *workData)
//      // Function that pretends to do work on the specified 'workData'.
//  {
//      // do some stuff with '*workData' ...
//
//      (void) workData;
//  }
//..
// Next, create the functor that will be run in the consumer threads:
//..
//  struct ConsumerFunctor {
//      // DATA
//      bdlcc::Deque<WorkRequest> *d_deque_p;
//
//      // CREATORS
//      explicit
//      ConsumerFunctor(bdlcc::Deque<WorkRequest> *container)
//          // Create a ''ConsumerFunctor' object that will consumer work
//          // requests from the specified 'container'.
//      : d_deque_p(container)
//      {}
//
//      // MANIPULATORS
//      void operator()()
//          // Pop work requests off the deque and process them until an
//          // 'e_STOP' request is encountered.
//      {
//          WorkRequest item;
//
//          do {
//              item = d_deque_p->popFront();
//              if (WorkRequest::e_WORK == item.d_type) {
//                  doWork(&item.d_data);
//              }
//          } while (WorkRequest::e_STOP != item.d_type);
//      }
//  };
//..
// Then, create the functor that will be run in the producer threads:
//..
//  struct ProducerFunctor {
//      // DATA
//      bdlcc::Deque<WorkRequest> *d_deque_p;
//
//      // CREATORS
//      explicit
//      ProducerFunctor(bdlcc::Deque<WorkRequest> *container)
//          // Create a 'ProducerFunctor' object that will enqueue work
//          // requests into the specified 'container'.
//      : d_deque_p(container)
//      {}
//
//      // MANIPULATORS
//      void operator()()
//          // Enqueue work requests to the container until 'getWorkData'
//          // returns 'false', then enqueue an 'e_STOP' request.
//      {
//          WorkRequest item;
//          WorkData    workData;
//
//          while (!getWorkData(&workData)) {
//              item.d_type = WorkRequest::e_WORK;
//              item.d_data = workData;
//              d_deque_p->pushBack(item);
//          }
//
//          item.d_type = WorkRequest::e_STOP;
//          d_deque_p->pushBack(item);
//      }
//  };
//..
// Next, in 'main', define the number of consumer and producer threads (these
// numbers must be equal).
//..
//  enum { k_NUM_CONSUMER_THREADS = 10,
//         k_NUM_PRODUCER_THREADS = k_NUM_CONSUMER_THREADS };
//..
// Then, create our container:
//..
//  bdlcc::Deque<WorkRequest> deque;
//..
// Next, create the array of thread handles for the threads we will spawn:
//..
//  bslmt::ThreadUtil::Handle handles[k_NUM_CONSUMER_THREADS +
//                                    k_NUM_PRODUCER_THREADS];
//..
// Now, spawn all the consumers and producers:
//..
//  int ti = 0, rc;
//  while (ti < k_NUM_CONSUMER_THREADS) {
//      rc = bslmt::ThreadUtil::create(&handles[ti++],
//                                     ConsumerFunctor(&deque));
//      assert(0 == rc);
//  }
//  while (ti < k_NUM_CONSUMER_THREADS + k_NUM_PRODUCER_THREADS) {
//      rc = bslmt::ThreadUtil::create(&handles[ti++],
//                                     ProducerFunctor(&deque));
//      assert(0 == rc);
//  }
//..
// Finally, join all the threads after they finish and confirm the container is
// empty afterward:
//..
//  while (ti > 0) {
//      rc = bslmt::ThreadUtil::join(handles[--ti]);
//      assert(0 == rc);
//  }
//  assert(0 == deque.length());
//..
//
///Example 2: A Queue of Events
/// - - - - - - - - - - - - - -
// First, we declare the 'Event' type, that will be contained in our
// 'bdlcc::Deque' object.
//..
//  struct Event {
//      enum EventType {
//          e_IN_PROGRESS   = 1,
//          e_TASK_COMPLETE = 2 };
//
//      EventType   d_type;
//      int         d_workerId;
//      int         d_eventNumber;
//      const char *d_eventText_p;
//  };
//
// Then, we define the number of events each thread will push:
//
//  enum { k_NUM_TO_PUSH = 5 };
//
// Next, we declare our 'WorkerFunctor' type, that will push 'k_NUM_TO_PUSH'
// events into the deque.
//
//  struct WorkerFunctor {
//      int                  d_workerId;
//      bdlcc::Deque<Event> *d_deque_p;
//      bslmt::Barrier      *d_barrier_p;
//
//      void operator()()
//          // All the threads will block on the same barrier so they all start
//          // at once to maximize concurrency.
//      {
//          d_barrier_p->wait();
//
//          // Loop to push 'k_NUM_TO_PUSH - 1' events onto the deque.
//
//          int evnum = 1;
//          while (evnum < k_NUM_TO_PUSH) {
//              // Yield every loop to maximize concurrency.
//
//              bslmt::ThreadUtil::yield();
//
//              // Create the event object.
//
//              Event ev = {
//                  Event::e_IN_PROGRESS,
//                  d_workerId,
//                  evnum++,
//                  "In-Progress Event"
//              };
//
//              // Push the event object.
//
//              d_deque_p->pushBack(ev);
//          }
//
//          // Create the completing event object.
//
//          Event ev = {
//              Event::e_TASK_COMPLETE,
//              d_workerId,
//              evnum,
//              "Task Complete"
//          };
//
//          // Push the completing event object.
//
//          d_deque_p->pushBack(ev);
//      }
//  };
//..
// Next, in 'main', define the number of threads:
//..
//  enum { k_NUM_THREADS = 10 };
//..
// Then, declare out 'bdlcc::Deque' object, the set of handles of the
// subthreads, and our barrier object:
//..
//  bdlcc::Deque<Event>       myDeque;
//  bslmt::ThreadUtil::Handle handles[k_NUM_THREADS];
//  bslmt::Barrier            barrier(k_NUM_THREADS + 1);
//..
// Next, spawn the worker threads:
//..
//  for (int ti = 0; ti < k_NUM_THREADS; ++ti) {
//      WorkerFunctor functor = { ti, &myDeque, &barrier };
//
//      bslmt::ThreadUtil::create(&handles[ti], functor);
//  }
//..
// Then, wait on the barrier, that will set all the subthreads running:
//..
//  barrier.wait();
//..
// Now, loop to pop the events off the deque, and keep track of how many
// 'e_COMPLETE' events have been popped.  When this equals the number of
// subthreads, we are done.
//..
//  int numCompleted = 0, numEvents = 0;
//  while (numCompleted < k_NUM_THREADS) {
//      Event ev = myDeque.popFront();
//      ++numEvents;
//      if (verbose) {
//          cout << "[" << ev.d_workerId << "] "
//               << ev.d_eventNumber << ". "
//               << ev.d_eventText_p << endl;
//      }
//      if (Event::e_TASK_COMPLETE == ev.d_type) {
//          ++numCompleted;
//          int rc = bslmt::ThreadUtil::join(handles[ev.d_workerId]);
//          assert(!rc);
//      }
//  }
//..
// Finally, perform some sanity checks:
//..
//  assert(k_NUM_THREADS * k_NUM_TO_PUSH == numEvents);
//  assert(0 == myDeque.length());
//..

#include <bdlscm_version.h>

#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bslma_allocator.h>

#include <bslmf_integralconstant.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_systemclocktype.h>
#include <bsls_timeinterval.h>

#include <bsl_algorithm.h>
#include <bsl_deque.h>
#include <bsl_vector.h>
#include <bsl_limits.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>

namespace BloombergLP {
namespace bdlcc {

// PRIVATE TYPES
template <class TYPE>
class Deque_DequeThrowGuard {
    // This private 'class' is used to manage a 'bsl::deque', during the course
    // of an operation by a 'bdlcc::Deque'.  Because it has a 'release' method,
    // it is actually a proctor, but we call it a 'guard' to avoid having
    // clients confuse it with this component's 'Proctor' and 'ConstProctor'
    // types.  A 'deque' that is being managed may only grow, and only on one
    // end or the other.  If a throw happens during the course of the operation
    // and this guard's destructor is called while still managing the object,
    // it will restore the managed object to its initial state via operations
    // that are guaranteed not to throw.

    // PRIVATE TYPES
    typedef bsl::deque<TYPE>                      MonoDeque;
    typedef typename MonoDeque::size_type         size_type;
    typedef typename MonoDeque::const_iterator    MDCIter;

    // DATA
    MonoDeque          *d_monoDeque_p;
    const MDCIter       d_mdBegin;
    const MDCIter       d_mdEnd;
    const bool          d_mdWasEmpty;

  private:
    // NOT IMPLEMENTED
    Deque_DequeThrowGuard(const Deque_DequeThrowGuard&);
    Deque_DequeThrowGuard& operator=(const Deque_DequeThrowGuard&);

  public:
    // CREATORS
    explicit
    Deque_DequeThrowGuard(MonoDeque *monoDeque_p);
        // Create a 'Deque_DequeThrowGuard' object that will manage the
        // specified '*monoDeque_p'.  The behavior is undefined if
        // '0 == monoDeque_p'.

    ~Deque_DequeThrowGuard();
        // If a 'MonoDeque' is being managed by this 'ThrowGuard', restore it
        // to the state it was in when this object was created.

    // MANIPULATOR
    void release();
        // Release the monitored 'MonoDeque' from management by this
        // 'Deque_DequeThrowGuard' object.
};

template <class TYPE>
class Deque_VectorThrowGuard {
    // This private 'class' is used to manage one object, either a 'MonoDeque'
    // or a 'vector', during the course of an operation by a 'bdlcc::Deque'.
    // Because it has a 'release' method, it is actually a proctor, but we call
    // it a 'guard' to avoid having clients confuse it with the 'Proctor' and
    // 'ConstProctor' types.  If a 'deque' is being managed, it may only grow,
    // and only on one end or the other.  If a throw happens during the course
    // of the operation and the guard's destructor is called while still
    // managing the object, it will restore the managed object to its initial
    // state via operations that are guaranteed not to throw.

    // PRIVATE TYPES
    typedef typename bsl::vector<TYPE>::size_type VSize;

    // DATA
    bsl::vector<TYPE>  *d_vector_p;
    const VSize         d_vSize;

  private:
    // NOT IMPLEMENTED
    Deque_VectorThrowGuard(const Deque_VectorThrowGuard&);
    Deque_VectorThrowGuard& operator=(const Deque_VectorThrowGuard&);

  public:
    // CREATORS
    explicit
    Deque_VectorThrowGuard(bsl::vector<TYPE> *vector_p);
        // Create a 'Deque_VectorThrowGuard' object that will manage the
        // specified '*vector_p'.  Note that the case where '0 == vector_p' is
        // explicitly permitted, in which case this object will not manage
        // anything.

    ~Deque_VectorThrowGuard();
        // If a 'vector' is being managed by this 'Deque_VectorThrowGuard',
        // restore it to the state it was in when this object was created.

    // MANIPULATOR
    void release();
        // Release the monitored 'vector' from management by this
        // 'Deque_VectorThrowGuard' object.
};

                                 // ===========
                                 // class Deque
                                 // ===========

template <class TYPE>
class Deque {
    // This class provides a fully thread-safe implementation of an efficient,
    // in-place, indexable, double-ended queue of (template parameter) 'TYPE'
    // values.  Direct access to the underlying 'bsl::deque<TYPE>' object is
    // provided through the nested 'Proctor' and 'ConstProctor' classes.  While
    // this class is not value-semantic, the underlying 'bsl::deque<TYPE>'
    // class is.

  public:
    // PUBLIC TYPES
    typedef bsl::deque<TYPE>               MonoDeque;
    typedef typename MonoDeque::size_type  size_type;

    class Proctor;            // defined after this 'class'
    class ConstProctor;       // defined after this 'class'

  private:
    // PRIVATE TYPES
    typedef Deque_DequeThrowGuard<TYPE>  DequeThrowGuard;
    typedef Deque_VectorThrowGuard<TYPE> VectorThrowGuard;

  private:
    // DATA
    mutable
    bslmt::Mutex       d_mutex;              // mutex object used to
                                             // synchronize access to the
                                             // underlying 'deque'.

    bslmt::Condition   d_notEmptyCondition;  // condition variable used to
                                             // signal that new data is
                                             // available in the container

    bslmt::Condition   d_notFullCondition;   // condition variable used to
                                             // signal when there is room
                                             // available to add new data to
                                             // the container

    MonoDeque          d_monoDeque;          // the underlying deque.

    const size_type    d_highWaterMark;      // positive maximum number of
                                             // items that can be contained
                                             // before insertions will be
                                             // blocked.

    bsls::SystemClockType::Enum
                       d_clockType;          // clock type used

  private:
    // NOT IMPLEMENTED
    Deque<TYPE>& operator=(const Deque<TYPE>&);

  public:
    // CLASS METHODS
    static
    size_type maxSizeT();
        // Return the maximum value that can be stored in a veriable of type
        // 'size_type'.  The high water mark defaults to having this value.

    // CREATORS
    explicit
    Deque(bslma::Allocator            *basicAllocator = 0);
    explicit
    Deque(bsls::SystemClockType::Enum  clockType,
          bslma::Allocator            *basicAllocator = 0);
        // Create a container of objects of (template parameter) 'TYPE', with
        // no high water mark, and use the specified 'clockType' to indicate
        // the epoch used for all time intervals (see {Supported Clock-Types}
        // in the component documentation).  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    explicit
    Deque(bsl::size_t                  highWaterMark,
          bslma::Allocator            *basicAllocator = 0);
    Deque(bsl::size_t                  highWaterMark,
          bsls::SystemClockType::Enum  clockType,
          bslma::Allocator            *basicAllocator = 0);
        // Create a container of objects of (template parameter) 'TYPE', with
        // the specified 'highWaterMark', and use the specified 'clockType' to
        // indicate the epoch used for all time intervals (see {Supported
        // Clock-Types} in the component documentation).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'highWaterMark > 0'.

    template <class INPUT_ITER>
    Deque(INPUT_ITER                   begin,
          INPUT_ITER                   end,
          bslma::Allocator            *basicAllocator = 0);
    template <class INPUT_ITER>
    Deque(INPUT_ITER                   begin,
          INPUT_ITER                   end,
          bsls::SystemClockType::Enum  clockType,
          bslma::Allocator            *basicAllocator = 0);
        // Create a container of objects of (template parameter) 'TYPE'
        // containing the sequence of elements in the specified range
        // '[begin .. end)', having no high water mark, and use the specified
        // 'clockType' to indicate the epoch used for all time intervals (see
        // {Supported Clock-Types} in the component documentation).  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that the items in the range are treated as 'const'
        // objects, copied without being modified.

    template <class INPUT_ITER>
    Deque(INPUT_ITER                   begin,
          INPUT_ITER                   end,
          bsl::size_t                  highWaterMark,
          bslma::Allocator            *basicAllocator = 0);
    template <class INPUT_ITER>
    Deque(INPUT_ITER                   begin,
          INPUT_ITER                   end,
          bsl::size_t                  highWaterMark,
          bsls::SystemClockType::Enum  clockType,
          bslma::Allocator            *basicAllocator = 0);
        // Create a container of objects of (template parameter) 'TYPE'
        // containing the sequence of 'TYPE' values in the specified range
        // '[begin .. end)' having the specified 'highWaterMark', and use the
        // specified 'clockType' to indicate the epoch used for all time
        // intervals (see {Supported Clock-Types} in the component
        // documentation).  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'highWaterMark > 0'.  Note that if the number of elements in the
        // range '[begin, end)' exceeds 'highWaterMark', the effect will be the
        // same as if the extra elements were added by forced pushes.  Also
        // note that the items in the range are treated as 'const' objects,
        // copied without being modified.

    Deque(const Deque<TYPE>& original, bslma::Allocator *basicAllocator = 0);
        // Create a container having the same value as the specified 'original'
        // object.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~Deque();
        // Destroy this container.  The behavior is undefined unless all access
        // or modification of the container has completed prior to the
        // destruction of this object.

    // MANIPULATORS
    void forcePushBack(const TYPE&             item);
        // Append the specified 'item' to the back of this container without
        // regard for the high-water mark.  Note that this method is provided
        // to allow high priority items to be inserted when the container is
        // full; 'pushFront' and 'pushBack' should be used for general use.

    void forcePushBack(bslmf::MovableRef<TYPE> item);
        // Append the specified move-insertable 'item' to the back of this
        // container without regard for the high-water mark.  'item' is left in
        // a valid but unspecified state.  Note that this method is provided to
        // allow high priority items to be inserted when the container is full;
        // 'pushFront' and 'pushBack' should be used for general use.

    template <class INPUT_ITER>
    void forcePushBack(INPUT_ITER begin,
                       INPUT_ITER end);
        // Append the specified specified range '[begin .. end)' of items to
        // the back of this container without regard for the high-water mark.
        // Note that the items in the range are treated as 'const' objects,
        // copied without being modified.

    void forcePushFront(const TYPE&             item);
        // Append the specified 'item' to the front of this container without
        // regard for the high-water mark.  Note that this method is provided
        // to allow high priority items to be inserted when the container is
        // full; 'pushFront' and 'pushBack' should be used for general use.

    void forcePushFront(bslmf::MovableRef<TYPE> item);
        // Append the specified move-insertable 'item' to the front of this
        // container without regard for the high-water mark.  'item' is left in
        // a valid but unspecified state.  Note that this method is provided to
        // allow high priority items to be inserted when the container is full;
        // 'pushFront' and 'pushBack' should be used for general use.

    template <class INPUT_ITER>
    void forcePushFront(INPUT_ITER begin,
                        INPUT_ITER end);
        // Append the specified specified range '[begin .. end)' of items to
        // the front of this container without regard for the high-water mark.
        // Note that pushed the items will be in the container in the reverse
        // of the order in which they occur in the range.  Also note that the
        // items in the range are treated as 'const' objects, copied without
        // being modified.

    TYPE popBack();
        // Return the last item in this container and remove it.  If this
        // container is empty, block until an item is available.

    void popBack(TYPE *item);
        // Remove the last item in this container and load that item into the
        // specified '*item'.  If this container is empty, block until an item
        // is available.

    TYPE popFront();
        // Return the first item in this container and remove it.  If this
        // container is empty, block until an item is available.

    void popFront(TYPE *item);
        // Remove the first item in this container and load that item into the
        // specified '*item'.  If the container is empty, block until an item
        // is available.

    void pushBack(const TYPE&             item);
        // Block until space in this container becomes available (see
        // {'High-Water Mark' Feature}), then append the specified 'item' to
        // the back of this container.

    void pushBack(bslmf::MovableRef<TYPE> item);
        // Block until space in this container becomes available (see
        // {'High-Water Mark' Feature}), then append the specified
        // move-insertable 'item' to the back of this container.  'item' is
        // left in a valid but unspecified state.

    void pushFront(const TYPE&             item);
        // Block until space in this container becomes available (see
        // {'High-Water Mark' Feature}), then append the specified 'item' to
        // the front of this container.

    void pushFront(bslmf::MovableRef<TYPE> item);
        // Block until space in this container becomes available (see
        // {'High-Water Mark' Feature}), then append the specified
        // move-insertable 'item' to the front of this container.  'item' is
        // left in a valid but unspecified state.

    void removeAll(bsl::vector<TYPE> *buffer = 0);
        // If the optionally specified 'buffer' is non-zero, append all the
        // elements from this container to '*buffer' in the same order, then,
        // regardless of whether 'buffer' is zero, clear this container.  Note
        // that the previous contents of '*buffer' are not discarded -- the
        // removed items are appended to it.

    int timedPopBack(TYPE *item, const bsls::TimeInterval& timeout);
        // Remove the last item in this container and load that item value into
        // the specified '*item'.  If this container is empty, block until an
        // item is available or until the specified 'timeout' (expressed as the
        // !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.  Return
        // 0 on success, and a non-zero value if the call timed out before an
        // item was available.  Note that this method can block indefinitely if
        // another thread has the mutex locked, particularly by a proctor
        // object -- there is no guarantee that this method will return after
        // 'timeout'.

    int timedPopFront(TYPE *item, const bsls::TimeInterval& timeout);
        // Remove the first item in this container and load that item value
        // into the specified '*item'.  If this container is empty, block until
        // an item is available or until the specified 'timeout' (expressed as
        // the !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.
        // Return 0 on success, and a non-zero value if the call timed out
        // before an item was available.  Note that this method can block
        // indefinitely if another thread has the mutex locked, particularly by
        // a proctor object -- there is no guarantee that this method will
        // return after 'timeout'.

    int timedPushBack(const TYPE&               item,
                      const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the back of this container if space
        // is available, otherwise (if the container is full) block waiting for
        // space to become available or until the specified 'timeout'
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) expires.  Return 0 if space was or became available and this
        // container was updated, and a non-zero value if the call timed out
        // before space became available and this container was left
        // unmodified.  Note that this method can block indefinitely if another
        // thread has the mutex locked, particularly by a proctor object --
        // there is no guarantee that this method will return after 'timeout'.

    int timedPushBack(bslmf::MovableRef<TYPE>   item,
                      const bsls::TimeInterval& timeout);
        // Append the specified move-insertable 'item' to the back of this
        // container if space is available, otherwise (if the container is
        // full) block waiting for space to become available or until the
        // specified 'timeout' (expressed as the !ABSOLUTE! time from 00:00:00
        // UTC, January 1, 1970) expires.  If the container is modified, 'item'
        // is left in a valid but unspecified state, otherwise 'item' is
        // unchanged.  Return 0 if space was or became available and this
        // container was updated, and a non-zero value if the call timed out
        // before space became available and this container was left
        // unmodified.  Note that this method can block indefinitely if another
        // thread has the mutex locked, particularly by a proctor object --
        // there is no guarantee that this method will return after 'timeout'.

    int timedPushFront(const TYPE&               item,
                       const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the front of this container if space
        // is available, otherwise (if the container is full) block waiting for
        // space to become available or until the specified 'timeout'
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) expires.  Return 0 if space was or became available and this
        // container was updated, and a non-zero value if the call timed out
        // before space became available and this container was left
        // unmodified.  Note that this method can block indefinitely if another
        // thread has the mutex locked, particularly by a proctor object --
        // there is no guarantee that this method will return after 'timeout'.

    int timedPushFront(bslmf::MovableRef<TYPE>   item,
                       const bsls::TimeInterval& timeout);
        // Append the specified move-insertable 'item' to the front of this
        // container if space is available, otherwise (if the container is
        // full) block waiting for space to become available or until the
        // specified 'timeout' (expressed as the !ABSOLUTE! time from 00:00:00
        // UTC, January 1, 1970) expires.  If the container is modified,'item'
        // is left in a valid but unspecified state, otherwise 'item' is left
        // unchanged.  Return 0 if space was or became available and this
        // container was updated, and a non-zero value if the call timed out
        // before space became available and this container was left
        // unmodified.  Note that this method can block indefinitely if another
        // thread has the mutex locked, particularly by a proctor object --
        // there is no guarantee that this method will return after 'timeout'.

    int tryPopBack(TYPE *item);
        // If this container is non-empty, remove the last item, load that item
        // into the specified '*item', and return 0 indicating success.  If
        // this container is empty, return a non-zero value with no effect on
        // 'item' or the state of this container.

    void tryPopBack(size_type          maxNumItems,
                    bsl::vector<TYPE> *buffer = 0);
        // Remove up to the specified 'maxNumItems' from the back of this
        // container.  Optionally specify a 'buffer' into which the items
        // removed from the container are loaded.  If 'buffer' is non-null, the
        // removed items are appended to it as if by repeated application of
        // 'buffer->push_back(popBack())' while the container is not empty and
        // 'maxNumItems' have not yet been removed.  Note that the ordering of
        // the items in '*buffer' after the call is the reverse of the ordering
        // they had in the deque.  Also note that '*buffer' is not cleared --
        // the popped items are appended after any pre-existing contents.
        //
        // Also note that to transfer the entire contents of a 'Deque' 'd' to a
        // vector 'v', use 'd.removeAll(&v);'.

    int tryPopFront(TYPE *item);
        // If this container is non-empty, remove the first item, load that
        // item into the specified '*item', and return 0 indicating success.
        // If this container is empty, return a non-zero value with no effect
        // on '*item' or the state of this container.

    void tryPopFront(size_type          maxNumItems,
                     bsl::vector<TYPE> *buffer = 0);
        // Remove up to the specified 'maxNumItems' from the front of this
        // container.  Optionally specify a 'buffer' into which the items
        // removed from the container are appended.  If 'buffer' is non-null,
        // the removed items are appended to it as if by repeated application
        // of 'buffer->push_back(popFront())' while the const is not empty and
        // 'maxNumItems' have not yet been removed.  Note that '*buffer' is not
        // cleared -- the popped items are appended after any pre-existing
        // contents.
        //
        // Also note that to transfer the entire contents of a 'Deque' 'd' to a
        // vector 'v', use 'd.removeAll(&v);'.

    int tryPushBack(const TYPE&             item);
        // If the container is not full (see {'High-Water Mark' Feature}),
        // append the specified 'item' to the back of the container, otherwise
        // leave the container unchanged.  Return 0 if the container was
        // updated and a non-zero value otherwise.

    int tryPushBack(bslmf::MovableRef<TYPE> item);
        // If the container is not full (see {'High-Water Mark' Feature}),
        // append the specified move-insertable 'item' to the back of the
        // container, otherwise leave the container unchanged.  If the
        // container is modified, 'item' is left in a valid but unspecified
        // state, otherwise 'item' is unchanged.  Return 0 if the container was
        // updated and a non-zero value otherwise.

    template <class INPUT_ITER>
    size_type tryPushBack(INPUT_ITER begin,
                          INPUT_ITER end);
        // Push as many of the items in the specified range '[begin .. end)' as
        // there is space available for (see {'High-Water Mark' Feature}) to
        // the back of the container, stopping if the container high-water mark
        // is reached.  Return the number of items pushed.  Note that the items
        // in the range are treated as 'const' objects, copied without being
        // modified.

    int tryPushFront(const TYPE&             item);
        // If the container is not full (see {'High-Water Mark' Feature}),
        // append the specified 'item' to the front of the container, otherwise
        // leave the container unchanged.  Return 0 if the container was
        // updated and a non-zero value otherwise.

    int tryPushFront(bslmf::MovableRef<TYPE> item);
        // If the container is not full (see {'High-Water Mark' Feature}),
        // append the specified move-insertable 'item' to the front of the
        // container, otherwise leave the container unchanged.  If the
        // container is modified, 'item' is left in a valid but unspecified
        // state, otherwise 'item' is unchanged.  Return 0 if the container was
        // updated and a non-zero value otherwise.

    template <class INPUT_ITER>
    size_type tryPushFront(INPUT_ITER begin,
                           INPUT_ITER end);
        // Push as many of the items in the specified range '[begin .. end)' as
        // there is space available for (see {'High-Water Mark' Feature}) to
        // the front of the container, stopping if the container high-water
        // mark is reached.  Return the number of items pushed.  Note that the
        // pushed items will be in the container in the reverse of the order in
        // which they occur in the range.  Also note that the items in the
        // range are treated as 'const' objects, copied without being modified.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this container for allocating memory.

    bsls::SystemClockType::Enum clockType() const;
        // Return the system clock type used for timing 'timed*' operations on
        // this object.

    size_type highWaterMark() const;
        // Return the high-water mark value for this container.

    size_type length() const;
        // Return the number of elements contained in this container.  Note
        // that this method temporarily acquires the mutex, so that this method
        // must not be called while a proctor in the same thread has this
        // container locked, and the value returned is potentially obsolete
        // before it is returned if any other threads are simultaneously
        // modifying this container.  To find the length while a proctor has
        // the container locked, call 'proctor->size()'.
};

                              // ====================
                              // class Deque::Proctor
                              // ====================

template <class TYPE>
class Deque<TYPE>::Proctor {
    // This class defines a proctor type that provides direct access to the
    // underlying 'bsl::deque' contained in a 'Deque'.  Creation of a 'Proctor'
    // object locks the mutex of the 'Deque', and destruction unlocks it.

    // PRIVATE TYPES
    typedef bsl::deque<TYPE>              MonoDeque;
    typedef typename MonoDeque::size_type size_type;

    // DATA
    Deque<TYPE>    *d_container_p;
    size_type       d_startLength;      // If '!d_container_p', this field may
                                        // be left uninitialized.

  private:
    // NOT IMPLEMENTED
    Proctor(const Proctor&);
    Proctor& operator=(const Proctor&);

  public:
    // CREATORS
    explicit
    Proctor(Deque<TYPE> *container = 0);
        // Create a 'Proctor' object to provide access to the underlying
        // 'bsl::deque' contained in the optionally specified '*container',
        // locking 'container's mutex.  If no 'container' is specified, this
        // object will be null.

    ~Proctor();
        // Release the lock on the mutex of the 'Deque' that was provided at
        // contstuction and destroy this 'Proctor' object.  Signal the
        // conditions on the 'Deque' that was supplied to this object at
        // construction to reflect any changes that have been made to its
        // contents since construction.

    // MANIPULATORS
    void load(Deque<TYPE> *container);
        // In the case where this 'Proctor' has been released, attach this
        // object to the specified 'container'.  If this object is already
        // attached, release the previous object first.  The behavior is
        // undefined if '0 == container'.

    void release();
        // Release this proctor without destroying it.  Afterward the
        // destructor will have no effect.  This may be called multiple times;
        // only the first call has any effect.

    // ACCESSORS
    MonoDeque *operator->() const;
        // Return a pointer to the 'bsl::deque' contained in the 'Deque'
        // managed by this 'Proctor' object'.  The behavior is undefined if
        // this 'Proctor' has been released.

    MonoDeque& operator*() const;
        // Return a reference to the 'bsl::deque' managed by this 'Proctor'
        // object.  The behavior is undefined if this 'Proctor' has been
        // released.

    TYPE& operator[](typename MonoDeque::size_type position) const;
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in the 'bsl::deque' held by this proctor.  The
        // behavior is undefined unless 'position < size' where 'size' is the
        // the number of elements in that deque.

    bool isNull() const;
        // Return 'true' if this object is not associated with a 'Deque'
        // object.
};

                            // =========================
                            // class Deque::ConstProctor
                            // =========================

template <class TYPE>
class Deque<TYPE>::ConstProctor {
    // This class defines a proctor type that provides direct const access to
    // the underlying 'bsl::deque' contained in a 'Deque'.

    // PRIVATE TYPES
    typedef bsl::deque<TYPE>              MonoDeque;
    typedef typename MonoDeque::size_type size_type;

    // DATA
    const Deque<TYPE>  *d_container_p;
    size_type           d_startLength;  // If '!d_container_p', this field may
                                        // be left uninitialized.

  private:
    // NOT IMPLEMENTED
    ConstProctor(const ConstProctor&);
    ConstProctor& operator=(const ConstProctor&);

  public:
    // CREATORS
    explicit
    ConstProctor(const Deque<TYPE> *container = 0);
        // Create a 'ConstProctor' object to provide const access to the
        // underlying 'bsl::deque' contained in the optionally specified
        // '*container', locking 'container's mutex.  If no 'container' is
        // specified, this object will be null.

    ~ConstProctor();
        // Release the lock on the mutex of the 'Deque' that was provided at
        // contstuction and destroy this 'Proctor' object.  The behavior is
        // undefined if the 'Deque' has been modified since the construction of
        // this object.

    // MANIPULATORS
    void load(const Deque<TYPE> *container);
        // In the case where this 'Proctor' has been released, attach this
        // object to the specified 'container'.  If this object is already
        // attached, release the previous object first.  The behavior is
        // undefined if '0 == container'.

    void release();
        // Release this proctor without destroying it.  Afterward the
        // destructor will have no effect.  This may be called multiple times;
        // only the first call has any effect;

    // ACCESSORS
    const MonoDeque *operator->() const;
        // Return a pointer to the 'bsl::deque' contained in the 'Deque'
        // managed by this object.  The behavior is undefined if this
        // 'ConstProctor' has been released.

    const MonoDeque& operator*() const;
        // Return a reference to the 'bsl::deque' managed by this 'Proctor'
        // object.  The behavior is undefined if this 'ConstProctor' has been
        // released.

    const TYPE& operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in the 'bsl::deque' held by this proctor.
        // The behavior is undefined unless 'position < size' where 'size' is
        // the number of elements in that deque.

    bool isNull() const;
        // Return 'true' if this object is not associated with a 'Deque'
        // object.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ---------------------
                            // bdlcc::Deque::Proctor
                            // ---------------------

// CREATORS
template <class TYPE>
inline
Deque<TYPE>::Proctor::Proctor(Deque<TYPE> *container)
: d_container_p(0)
{
    if (container) {
        this->load(container);
    }

    // If '0 == container', leave 'd_startLength' uninitialized.
}

template <class TYPE>
inline
Deque<TYPE>::Proctor::~Proctor()
{
    this->release();
}

// MANIPULATORS
template <class TYPE>
inline
void Deque<TYPE>::Proctor::load(Deque<TYPE> *container)
{
    BSLS_ASSERT(0 != container);

    if (0 != d_container_p) {
        this->release();
    }

    container->d_mutex.lock();
    d_container_p = container;
    d_startLength = d_container_p->d_monoDeque.size();
}

template <class TYPE>
void Deque<TYPE>::Proctor::release()
{
    if (0 == d_container_p) {
        return;                                                       // RETURN
    }

    const size_type sz = d_container_p->d_monoDeque.size();
    size_type       ii = d_startLength;

    d_container_p->d_mutex.unlock();

    if (ii < sz) {
        do {
            d_container_p->d_notEmptyCondition.signal();
        } while (++ii < sz);
    }
    else {
        if (d_container_p->d_highWaterMark < ii) {
            ii = d_container_p->d_highWaterMark;
        }
        for (; ii > sz; --ii) {
            d_container_p->d_notFullCondition.signal();
        }
    }

    d_container_p = 0;
}

// ACCESSORS
template <class TYPE>
inline
bsl::deque<TYPE> *Deque<TYPE>::Proctor::operator->() const
{
    BSLS_ASSERT(d_container_p);

    return &d_container_p->d_monoDeque;
}

template <class TYPE>
inline
bsl::deque<TYPE>& Deque<TYPE>::Proctor::operator*() const
{
    BSLS_ASSERT(d_container_p);

    return d_container_p->d_monoDeque;
}

template <class TYPE>
inline
TYPE& Deque<TYPE>::Proctor::operator[](size_type position) const
{
    BSLS_ASSERT(position < d_container_p->d_monoDeque.size());

    return d_container_p->d_monoDeque[position];
}

template <class TYPE>
inline
bool Deque<TYPE>::Proctor::isNull() const
{
    return 0 == d_container_p;
}

                        // --------------------------
                        // bdlcc::Deque::ConstProctor
                        // --------------------------


// CREATORS
template <class TYPE>
inline
Deque<TYPE>::ConstProctor::ConstProctor(const Deque<TYPE> *container)
: d_container_p(0)
{
    if (container) {
        this->load(container);
    }

    // If '0 == container', leave 'd_startLength' uninitialized.
}

template <class TYPE>
inline
Deque<TYPE>::ConstProctor::~ConstProctor()
{
    this->release();
}

// MANIPULATORS
template <class TYPE>
inline
void Deque<TYPE>::ConstProctor::load(const Deque<TYPE> *container)
{
    BSLS_ASSERT(0 != container);

    this->release();

    container->d_mutex.lock();
    d_container_p = container;
    d_startLength = d_container_p->d_monoDeque.size();
}

template <class TYPE>
inline
void Deque<TYPE>::ConstProctor::release()
{
    // It is important that nobody did a const_cast and modified the underlying
    // 'bsls:deque' since this destructor won't signal the appropriate
    // condtions in the 'Deque' in that case.  If they wanted to modify the
    // 'bsl::deque' they should have used a 'Proctor' instead of a
    // 'ConstProctor'.

    if (0 == d_container_p) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT_OPT(d_container_p->d_monoDeque.size() == d_startLength &&
                     "Underlying 'bsl::deque' modified through ConstProcter.");

    bslmt::Mutex *mutex = &d_container_p->d_mutex;
    d_container_p = 0;
    mutex->unlock();
}

// ACCESSORS
template <class TYPE>
inline
const bsl::deque<TYPE> *Deque<TYPE>::ConstProctor::operator->() const
{
    BSLS_ASSERT(d_container_p);

    return &d_container_p->d_monoDeque;
}

template <class TYPE>
inline
const bsl::deque<TYPE>& Deque<TYPE>::ConstProctor::operator*() const
{
    BSLS_ASSERT(d_container_p);

    return d_container_p->d_monoDeque;
}

template <class TYPE>
inline
const TYPE& Deque<TYPE>::ConstProctor::operator[](size_type position) const
{
    BSLS_ASSERT(position < d_container_p->d_monoDeque.size());

    return d_container_p->d_monoDeque[position];
}

template <class TYPE>
inline
bool Deque<TYPE>::ConstProctor::isNull() const
{
    return 0 == d_container_p;
}

                          // ----------------------------
                          // bdlcc::Deque_DequeThrowGuard
                          // ----------------------------

// CREATORS
template <class TYPE>
inline
Deque_DequeThrowGuard<TYPE>::Deque_DequeThrowGuard(
                                                 bsl::deque<TYPE> *monoDeque_p)
: d_monoDeque_p(monoDeque_p)
, d_mdBegin(   monoDeque_p->cbegin())
, d_mdEnd(     monoDeque_p->cend())
, d_mdWasEmpty(monoDeque_p->empty())
{
    BSLS_ASSERT(0 != monoDeque_p);
}

template <class TYPE>
inline
Deque_DequeThrowGuard<TYPE>::~Deque_DequeThrowGuard()
{
    if (d_monoDeque_p)  {
        if (d_mdWasEmpty) {
            // In the case where the mono deque started out empty, pushing to
            // it can invalidate the iterators that were copied to 'd_mdBegin'
            // and 'd_mdEnd' when it was empty, so comparisons between
            // 'newBegin' & 'newEnd' and the old iterators will yield undefined
            // results.  So we kept a separate boolean, 'd_mdWasEmpty', to
            // track that case, which we handle specially here.

            d_monoDeque_p->clear();

            return;                                                   // RETURN
        }

        const MDCIter newBegin = d_monoDeque_p->cbegin();
        const MDCIter newEnd   = d_monoDeque_p->cend();

        // While range-based 'erase' of 'bsl::deque' does not always provide
        // the no-throw guarantee, all the erasing here is done at the ends of
        // the 'bsl::deque', so no items have to be copied around, so no
        // throwing should occur.

        // The 'MonoDeque' may have been pushed to, but only on one end or the
        // other.  It should never have been deleted from.

        if (newBegin < d_mdBegin) {
            BSLS_ASSERT(d_mdEnd == newEnd);

            d_monoDeque_p->erase(newBegin, d_mdBegin);
        }
        else {
            BSLS_ASSERT(newBegin == d_mdBegin);

            if (d_mdEnd < newEnd) {
                d_monoDeque_p->erase(d_mdEnd, newEnd);
            }
            else {
                BSLS_ASSERT(d_mdEnd == newEnd);
            }
        }
    }
}

// MANIPULATOR
template <class TYPE>
inline
void Deque_DequeThrowGuard<TYPE>::release()
{
    d_monoDeque_p = 0;
}

                          // -----------------------------
                          // bdlcc::Deque_VectorThrowGuard
                          // -----------------------------

// CREATORS
template <class TYPE>
inline
Deque_VectorThrowGuard<TYPE>::Deque_VectorThrowGuard(
                                                   bsl::vector<TYPE> *vector_p)
: d_vector_p(vector_p)
, d_vSize(vector_p ? vector_p->size() : 0)
{
}

template <class TYPE>
inline
Deque_VectorThrowGuard<TYPE>::~Deque_VectorThrowGuard()
{
    if (d_vector_p) {
        const VSize newSize = d_vector_p->size();

        // While 'vector::resize' does not always provide the no-throw
        // guarantee, here we are always shrinking the vector, so it should not
        // throw.

        // The vector may have grown, it should never have been shrunk.

        if (d_vSize < newSize) {
            d_vector_p->resize(d_vSize);
        }
        else {
            BSLS_ASSERT(d_vSize == newSize);
        }
    }
}

// MANIPULATOR
template <class TYPE>
inline
void Deque_VectorThrowGuard<TYPE>::release()
{
    d_vector_p = 0;
}

                                  // ------------
                                  // bdlcc::Deque
                                  // ------------

// CLASS METHODS
template <class TYPE>
inline
typename Deque<TYPE>::size_type Deque<TYPE>::maxSizeT()
{
    return bsl::numeric_limits<size_type>::max();
}

// CREATORS
template <class TYPE>
inline
Deque<TYPE>::Deque(bslma::Allocator *basicAllocator)
: d_mutex()
, d_notEmptyCondition()
, d_notFullCondition()
, d_monoDeque(basicAllocator)
, d_highWaterMark(maxSizeT())
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

template <class TYPE>
inline
Deque<TYPE>::Deque(bsls::SystemClockType::Enum  clockType,
                   bslma::Allocator            *basicAllocator)
: d_mutex()
, d_notEmptyCondition(clockType)
, d_notFullCondition(clockType)
, d_monoDeque(basicAllocator)
, d_highWaterMark(maxSizeT())
, d_clockType(clockType)
{
}

template <class TYPE>
inline
Deque<TYPE>::Deque(bsl::size_t       highWaterMark,
                   bslma::Allocator *basicAllocator)
: d_mutex()
, d_notEmptyCondition()
, d_notFullCondition()
, d_monoDeque(basicAllocator)
, d_highWaterMark(highWaterMark)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
    BSLS_ASSERT(highWaterMark > 0);
}

template <class TYPE>
inline
Deque<TYPE>::Deque(bsl::size_t                  highWaterMark,
                   bsls::SystemClockType::Enum  clockType,
                   bslma::Allocator            *basicAllocator)
: d_mutex()
, d_notEmptyCondition(clockType)
, d_notFullCondition(clockType)
, d_monoDeque(basicAllocator)
, d_highWaterMark(highWaterMark)
, d_clockType(clockType)
{
    BSLS_ASSERT(highWaterMark > 0);
}

template <class TYPE>
template <class INPUT_ITER>
inline
Deque<TYPE>::Deque(INPUT_ITER        begin,
                   INPUT_ITER        end,
                   bslma::Allocator *basicAllocator)
: d_mutex()
, d_notEmptyCondition()
, d_notFullCondition()
, d_monoDeque(begin, end, basicAllocator)
, d_highWaterMark(maxSizeT())
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
}

template <class TYPE>
template <class INPUT_ITER>
inline
Deque<TYPE>::Deque(INPUT_ITER                   begin,
                   INPUT_ITER                   end,
                   bsls::SystemClockType::Enum  clockType,
                   bslma::Allocator            *basicAllocator)
: d_mutex()
, d_notEmptyCondition(clockType)
, d_notFullCondition(clockType)
, d_monoDeque(begin, end, basicAllocator)
, d_highWaterMark(maxSizeT())
, d_clockType(clockType)
{
}

template <class TYPE>
template <class INPUT_ITER>
inline
Deque<TYPE>::Deque(INPUT_ITER        begin,
                   INPUT_ITER        end,
                   bsl::size_t       highWaterMark,
                   bslma::Allocator *basicAllocator)
: d_mutex()
, d_notEmptyCondition()
, d_notFullCondition()
, d_monoDeque(begin, end, basicAllocator)
, d_highWaterMark(highWaterMark)
, d_clockType(bsls::SystemClockType::e_REALTIME)
{
    BSLS_ASSERT(highWaterMark > 0);
}

template <class TYPE>
template <class INPUT_ITER>
inline
Deque<TYPE>::Deque(INPUT_ITER                   begin,
                   INPUT_ITER                   end,
                   bsl::size_t                  highWaterMark,
                   bsls::SystemClockType::Enum  clockType,
                   bslma::Allocator            *basicAllocator)
: d_mutex()
, d_notEmptyCondition(clockType)
, d_notFullCondition(clockType)
, d_monoDeque(begin, end, basicAllocator)
, d_highWaterMark(highWaterMark)
, d_clockType(clockType)
{
    BSLS_ASSERT(highWaterMark > 0);
}

template <class TYPE>
inline
Deque<TYPE>::Deque(const Deque<TYPE>&  original,
                   bslma::Allocator   *basicAllocator)
: d_mutex()
, d_notEmptyCondition()
, d_notFullCondition()
, d_monoDeque(basicAllocator)
, d_highWaterMark(maxSizeT())
, d_clockType(original.d_clockType)
{
    ConstProctor proctor(&original);

    d_monoDeque.insert(d_monoDeque.end(), proctor->begin(), proctor->end());
}

template <class TYPE>
inline
Deque<TYPE>::~Deque()
{
}

// MANIPULATORS
template <class TYPE>
inline
void Deque<TYPE>::forcePushBack(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        d_monoDeque.push_back(item);
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
inline
void Deque<TYPE>::forcePushBack(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        d_monoDeque.push_back(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
template <class INPUT_ITER>
inline
void Deque<TYPE>::forcePushBack(INPUT_ITER begin,
                                INPUT_ITER end)
{
    size_type growth;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        DequeThrowGuard tg(&d_monoDeque);

        const size_type initialSize = d_monoDeque.size();
        d_monoDeque.insert(d_monoDeque.end(), begin, end);
        growth = d_monoDeque.size() - initialSize;

        tg.release();
    }

    for (; growth > 0; --growth) {
        d_notEmptyCondition.signal();
    }
}

template <class TYPE>
inline
void Deque<TYPE>::forcePushFront(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        d_monoDeque.push_front(item);
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
inline
void Deque<TYPE>::forcePushFront(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        d_monoDeque.push_front(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
template <class INPUT_ITER>
inline
void Deque<TYPE>::forcePushFront(INPUT_ITER begin,
                                 INPUT_ITER end)
{
    size_type growth;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        DequeThrowGuard tg(&d_monoDeque);

        const size_type initialSize = d_monoDeque.size();
        for (; end != begin; ++begin) {
            d_monoDeque.push_front(*begin);
        }
        growth = d_monoDeque.size() - initialSize;

        tg.release();
    }

    for (; growth > 0; --growth) {
        d_notEmptyCondition.signal();
    }
}

template <class TYPE>
TYPE Deque<TYPE>::popBack()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    TYPE ret(bslmf::MovableRefUtil::move(d_monoDeque.back()));
    d_monoDeque.pop_back();

    const bool shouldSignal = d_monoDeque.size() < d_highWaterMark;
    lock.release()->unlock();

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return ret;
}

template <class TYPE>
void Deque<TYPE>::popBack(TYPE *item)
{
    bool shouldSignal;

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.empty()) {
            d_notEmptyCondition.wait(&d_mutex);
        }

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.back());
#else
        *item = d_monoDeque.back();
#endif
        d_monoDeque.pop_back();
        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }
}

template <class TYPE>
TYPE Deque<TYPE>::popFront()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    TYPE ret(bslmf::MovableRefUtil::move(d_monoDeque.front()));
    d_monoDeque.pop_front();

    const bool shouldSignal = d_monoDeque.size() < d_highWaterMark;
    lock.release()->unlock();

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return ret;
}

template <class TYPE>
void Deque<TYPE>::popFront(TYPE *item)
{
    bool shouldSignal;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.empty()) {
            d_notEmptyCondition.wait(&d_mutex);
        }
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.front());
#else
        *item = d_monoDeque.front();
#endif
        d_monoDeque.pop_front();

        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }
}

template <class TYPE>
void Deque<TYPE>::pushBack(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
        d_monoDeque.push_back(item);
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
void Deque<TYPE>::pushBack(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
        d_monoDeque.push_back(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
void Deque<TYPE>::pushFront(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
        d_monoDeque.push_front(item);
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
void Deque<TYPE>::pushFront(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
        d_monoDeque.push_front(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();
}

template <class TYPE>
inline
void Deque<TYPE>::removeAll(bsl::vector<TYPE> *buffer)
{
    Proctor proctor(this);

    VectorThrowGuard tg(buffer);

    if (buffer) {
        const size_type size = d_monoDeque.size();
        buffer->reserve(buffer->size() + size);

        for (size_type ii = 0; ii < size; ++ii) {
            buffer->push_back(bslmf::MovableRefUtil::move(d_monoDeque[ii]));
        }
    }

    proctor->clear();

    tg.release();
}

template <class TYPE>
int Deque<TYPE>::timedPopBack(TYPE                      *item,
                              const bsls::TimeInterval&  timeout)
{
    bool shouldSignal;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.empty()) {
            if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.back());
#else
        *item = d_monoDeque.back();
#endif
        d_monoDeque.pop_back();

        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPopFront(TYPE                      *item,
                               const bsls::TimeInterval&  timeout)
{
    bool shouldSignal;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.empty()) {
            if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.front());
#else
        *item = d_monoDeque.front();
#endif
        d_monoDeque.pop_front();

        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushBack(const TYPE&               item,
                               const bsls::TimeInterval& timeout)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
        d_monoDeque.push_back(item);
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushBack(bslmf::MovableRef<TYPE>   item,
                               const bsls::TimeInterval& timeout)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
        d_monoDeque.push_back(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushFront(const TYPE&               item,
                                const bsls::TimeInterval &timeout)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
        d_monoDeque.push_front(item);
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushFront(bslmf::MovableRef<TYPE>   item,
                                const bsls::TimeInterval &timeout)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_monoDeque.size() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
        d_monoDeque.push_front(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::tryPopBack(TYPE *item)
{
    bool shouldSignal;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.empty()) {
            return 1;                                                 // RETURN
        }
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.back());
#else
        *item = d_monoDeque.back();
#endif
        d_monoDeque.pop_back();

        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
void Deque<TYPE>::tryPopBack(typename Deque<TYPE>::size_type  maxNumItems,
                             bsl::vector<TYPE>               *buffer)
{
    Proctor proctor(this);
    VectorThrowGuard tg(buffer);

    // First, calculate 'toMove', which drives how the rest of the function
    // behaves.

    const size_type size   = d_monoDeque.size();
    const size_type toMove = bsl::min(size, maxNumItems);

    if (buffer) {
        buffer->reserve(buffer->size() + toMove);

        const size_type lastMovedIdx = size - toMove;
        for (size_type ii = size; lastMovedIdx < ii--; ) {
            buffer->push_back(bslmf::MovableRefUtil::move(d_monoDeque[ii]));
        }
    }
    d_monoDeque.erase(d_monoDeque.end() - toMove, d_monoDeque.end());

    tg.release();

    // Signalling will happen automatically when proctor is destroyed.
}

template <class TYPE>
int Deque<TYPE>::tryPopFront(TYPE *item)
{
    bool shouldSignal;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.empty()) {
            return 1;                                                 // RETURN
        }
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        *item = bslmf::MovableRefUtil::move(d_monoDeque.front());
#else
        *item = d_monoDeque.front();
#endif
        d_monoDeque.pop_front();

        shouldSignal = d_monoDeque.size() < d_highWaterMark;
    }

    if (shouldSignal) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
void Deque<TYPE>::tryPopFront(typename Deque<TYPE>::size_type  maxNumItems,
                              bsl::vector<TYPE>               *buffer)
{
    typedef typename MonoDeque::iterator Iterator;

    Proctor proctor(this);
    VectorThrowGuard tg(buffer);

    // First, calculate 'toMove', which drives how the rest of the function
    // behaves.

    const size_type toMove     = bsl::min(d_monoDeque.size(), maxNumItems);
    const Iterator  beginRange = d_monoDeque.begin();
    const Iterator  endRange   = beginRange + toMove;

    if (buffer) {
        buffer->reserve(buffer->size() + toMove);

        for (size_type ii = 0; ii < toMove; ++ii) {
            buffer->push_back(bslmf::MovableRefUtil::move(d_monoDeque[ii]));
        }
    }
    proctor->erase(beginRange, endRange);

    tg.release();

    // Signalling will happen automatically when proctor is destroyed.
}

template <class TYPE>
int Deque<TYPE>::tryPushBack(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.size() >= d_highWaterMark) {
            return 1;                                                 // RETURN
        }

        d_monoDeque.push_back(item);
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::tryPushBack(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.size() >= d_highWaterMark) {
            return 1;                                                 // RETURN
        }

        d_monoDeque.push_back(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
template <class INPUT_ITER>
typename Deque<TYPE>::size_type
Deque<TYPE>::tryPushBack(INPUT_ITER begin,
                         INPUT_ITER end)
{
    size_type growth;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        DequeThrowGuard tg(&d_monoDeque);

        const size_type startLength = d_monoDeque.size();
        size_type       length      = startLength;

        for (; length < d_highWaterMark && end != begin; ++length, ++begin) {
            d_monoDeque.push_back(*begin);
        }

        tg.release();

        growth = length - startLength;
    }

    for (size_type ii = 0; ii < growth; ++ii) {
        d_notEmptyCondition.signal();
    }

    return growth;
}

template <class TYPE>
int Deque<TYPE>::tryPushFront(const TYPE& item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.size() >= d_highWaterMark) {
            return 1;                                                 // RETURN
        }

        d_monoDeque.push_front(item);
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::tryPushFront(bslmf::MovableRef<TYPE> item)
{
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (d_monoDeque.size() >= d_highWaterMark) {
            return 1;                                                 // RETURN
        }

        d_monoDeque.push_front(bslmf::MovableRefUtil::move(item));
    }

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
template <class INPUT_ITER>
typename Deque<TYPE>::size_type
Deque<TYPE>::tryPushFront(INPUT_ITER begin,
                          INPUT_ITER end)
{
    size_type growth;
    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        DequeThrowGuard tg(&d_monoDeque);

        const size_type startLength = d_monoDeque.size();
        size_type       length      = startLength;

        for (; length < d_highWaterMark && end != begin; ++length, ++begin) {
            d_monoDeque.push_front(*begin);
        }

        tg.release();

        growth = length - startLength;
    }

    for (size_type ii = 0; ii < growth; ++ii) {
        d_notEmptyCondition.signal();
    }

    return growth;
}

// ACCESSORS
template <class TYPE>
inline
bslma::Allocator *Deque<TYPE>::allocator() const
{
    return d_monoDeque.get_allocator().mechanism();
}

template <class TYPE>
inline
bsls::SystemClockType::Enum Deque<TYPE>::clockType() const
{
    return d_clockType;
}

template <class TYPE>
inline
typename Deque<TYPE>::size_type Deque<TYPE>::highWaterMark() const
{
    // A mutex lock is unnecessary since we decided to make the high water mark
    // into a non-malleable property of this container.
    //
    // bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    return d_highWaterMark;
}

template <class TYPE>
inline
typename Deque<TYPE>::size_type Deque<TYPE>::length() const
{
    // Note that it is VITAL to lock the mutex here.  'size' on a deque is a
    // potentially complex operation as the deque might be managing multiple
    // blocks of memory.  If 'd_monoDeque' were being modified while we perform
    // the 'size' operation, we could potentially dereference pointers to
    // freed memory.

    // The predessor to this component, 'bcec_queue', originally had no
    // 'length' accessor, and we found that users were very, very frequently
    // accessing the underlying thread-unsafe container just to obtain the
    // length.

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    return d_monoDeque.size();
}

}  // close package namespace

namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator<bdlcc::Deque<TYPE> > : bsl::true_type
{};

}  // close namespace bslma
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
