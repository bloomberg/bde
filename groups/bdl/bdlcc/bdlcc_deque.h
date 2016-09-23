// bdlcc_deque.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLCC_DEQUE
#define INCLUDED_BDLCC_DEQUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled deque of items of parameterized 'TYPE'.
//
//@CLASSES:
//   bdlcc_Deque: thread-safe 'bsl::deque' wrapper
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: bsl::deque
//
//@DESCRIPTION: This component provides 'bdlcc::Deque<TYPE>', a thread-safe
// implementation of an efficient, in-place, indexable, double-ended queue of
// parameterized 'TYPE' values.  'bdlcc::Deque' is effectively a thread-safe
// wrapper for 'bsl::deque', whose interface is also made available through
// proctor types that are nested classes.
//
///Tips For Porting From 'bcec_Queue':
///----------------------------------
//: o 'InitialCapacity' has been eliminated.  Instead, construct your
//:   'bdlcc::Deque' object and then use proctor access to call 'reserve' on
//:   the contained 'bsl::deque' to reserve the desired initial capacity.
//:   (Note that 'deque::reserve' is not part of the C++ standard, though
//:   'bsl::deque' does implement it).
//: o The mutex and condition variables are no longer directly exposed, in
//:   favor of the new proctor access, which gives direct access to the
//:   underlying 'bsl::deque', automatically locking the mutex and updating the
//:   condition variables as necessary.
//
///Throw Guarantees:
///----------------
// Assuming the following throw guarantees of the parametrized 'TYPE':
//: 1 The destructor doesn't throw.
//: 2 If copy-assignment throws, it leaves the state of the destination
//:   unchanged.
// We have the following guarantees:
//: 1 If a single-object push or pop operation throws, both the state of the
//:   container and the state of the 'TYPE' object being assigned to is
//:   unchanged.
//: 2 If a 'tryPop*' operation to a vector, or a 'removeAll' to a vector
//:   throws:
//:   o The state of the 'bdlcc::Deque<TYPE>' container is unchanged.
//:   o The previous contents of the vector will remain at the front of the
//:     vector.
//:   o The rest of the vector will be in a valid but unknown state -- the
//:     capacity may have grown, and it may contain copies of some of the
//:     elements that were in the 'bdlcc::Deque<TYPE>' container.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Queue of Work Requests
///- - - - - - - - - - - - - - - - - -
// First, declarer the struct 'my_WordData'.  Imagine it contains some data one
// wants to process:
//..
//  struct my_WorkData{
//      // worrk data...
//  };
//..
// Then, create the function that will produce a 'mY_WorkData' object:
//..
//  bool my_getWorkData(my_WorkData *)
//      // Dummy implementation of 'getWorkData' function required by the usage
//      // example.
//  {
//      static int i = 1;
//      return ++i < 1000;
//  }
//..
// Next, declare 'my_WorkRequest', the type of object that will be stored in
// the container:
//..
//  struct my_WorkRequest {
//      // PUBLIC TYPES
//      enum RequestType {
//          e_WORK = 1,
//          e_STOP = 2
//      };
//
//      // PUBLIC DATA
//      RequestType d_type;
//      my_WorkData d_data;
//  };
//..
// Then, create the funciton that will do work on a 'my_WorkRequest' object:
//..
//  void my_doWork(my_WorkData *workData)
//      // Function that pretends to do work on the specified 'workData'.
//  {
//      // do some stuff with '*workData' ...
//
//      (void) workData;
//  }
//..
// Next, create the functor that will be run in the consumer threads:
//..
//  struct my_ConsumerFunctor {
//      // DATA
//      bdlcc::Deque<my_WorkRequest> *d_deque_p;
//
//      // CREATORS
//      explicit
//      my_ConsumerFunctor(bdlcc::Deque<my_WorkRequest> *container)
//          // Create a ''my_ConsumerFunctor' object that will consumer work
//          // requests from the specified 'container'.
//      : d_deque_p(container)
//      {}
//
//      // MANIPULATORS
//      void operator()()
//          // Pop work requests off the deque and process them until an
//          // 'e_STOP' request is encountered.
//      {
//          my_WorkRequest item;
//
//          do {
//              item = d_deque_p->popFront();
//              if (my_WorkRequest::e_WORK == item.d_type) {
//                  my_doWork(&item.d_data);
//              }
//          } while (my_WorkRequest::e_STOP != item.d_type);
//      }
//  };
//..
// Then, create the functor that will be run in the producer threads:
//..
//  struct my_ProducerFunctor {
//      // DATA
//      bdlcc::Deque<my_WorkRequest> *d_deque_p;
//
//      // CREATORS
//      explicit
//      my_ProducerFunctor(bdlcc::Deque<my_WorkRequest> *container)
//          // Create a 'my_ProducerFunctor' object that will enqueue work
//          // requests into the specified 'container'.
//      : d_deque_p(container)
//      {}
//
//      // MANIPULATORS
//      void operator()()
//          // Enqueue work requests to the container until 'my_getWorkData'
//          // returns 'false', then enqueue an 'e_STOP' request.
//      {
//          my_WorkRequest item;
//          my_WorkData    workData;
//
//          while (!my_getWorkData(&workData)) {
//              item.d_type = my_WorkRequest::e_WORK;
//              item.d_data = workData;
//              d_deque_p->pushBack(item);
//          }
//
//          item.d_type = my_WorkRequest::e_STOP;
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
//  bdlcc::Deque<my_WorkRequest> deque;
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
//                                     my_ConsumerFunctor(&deque));
//      assert(0 == rc);
//  }
//  while (ti < k_NUM_CONSUMER_THREADS + k_NUM_PRODUCER_THREADS) {
//      rc = bslmt::ThreadUtil::create(&handles[ti++],
//                                     my_ProducerFunctor(&deque));
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
// First, we declare the 'my_Event' type, that will be contained in our
// 'bdlcc::Deque' object.
//
//  struct my_Event {
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
// Next, we declare our 'my_WorkerFunctor' type, that will push 'k_NUM_TO_PUSH'
// events into the deque.
//
//  struct my_WorkerFunctor {
//      int                     d_workerId;
//      bdlcc::Deque<my_Event> *d_deque_p;
//      bslmt::Barrier         *d_barrier_p;
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
//              my_Event ev = {
//                  my_Event::e_IN_PROGRESS,
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
//          my_Event ev = {
//              my_Event::e_TASK_COMPLETE,
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
//  bdlcc::Deque<my_Event>    myDeque;
//  bslmt::ThreadUtil::Handle handles[k_NUM_THREADS];
//  bslmt::Barrier            barrier(k_NUM_THREADS + 1);
//..
// Next, spawn the worker threads:
//..
//  for (int ti = 0; ti < k_NUM_THREADS; ++ti) {
//      my_WorkerFunctor functor = { ti, &myDeque, &barrier };
//
//      bslmt::ThreadUtil::create(&handles[ti], functor);
//  }
//..
// Then, wait on the barrier, that will set all the subthreads running:
//..
//  barrier.wait();
//..
// Now, loop to pop the events off the deque, and keep track of how many
// 'e_COMPLETE' events have been popped, when this equals the number of
// subthreads, we are done.
//..
//  int numCompleted = 0, numEvents = 0;
//  while (numCompleted < k_NUM_THREADS) {
//      my_Event ev = myDeque.popFront();
//      ++numEvents;
//      if (verbose) {
//          cout << "[" << ev.d_workerId << "] "
//               << ev.d_eventNumber << ". "
//               << ev.d_eventText_p << endl;
//      }
//      if (my_Event::e_TASK_COMPLETE == ev.d_type) {
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_DEQUE
#include <bsl_deque.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDIO
#include <bsl_cstdio.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                                 // ===========
                                 // class Deque
                                 // ===========

template <class TYPE>
class Deque {
    // This class provides a thread-enabled implementation of an efficient,
    // in-place, indexable, double-ended queue of parameterized 'TYPE' values.
    // Direct access to the underlying 'bsl::deque<TYPE>' object is provided
    // through the nested 'Proctor' and 'ConstProctor' classes.  While this
    // class is not value-semantic, the underlying 'bsl::deque<TYPE>' class is.

  public:
    // PUBLIC TYPES
    typedef bsl::deque<TYPE>              MonoDeque;
    typedef typename MonoDeque::size_type size_type;

    class Proctor;        // defined below
    class ConstProctor;   // defined below

  private:
    // DATA
    mutable
    bslmt::Mutex       d_mutex;              // mutex object used to
                                             // synchronize access to the
                                             // underlying 'deque'.

    bslmt::Condition   d_notEmptyCondition;  // condition variable used to
                                             // signal that new data is
                                             // available in the queue

    bslmt::Condition   d_notFullCondition;   // condition variable used to
                                             // signal when there is room
                                             // available to add new data to
                                             // the queue

    MonoDeque          d_monoDeque;          // the underlying deque.

    const size_type    d_highWaterMark;      // positive maximum number of
                                             // items that can be queued before
                                             // insertions will be blocked.

  private:
    // NOT IMPLEMENTED
    Deque(const Deque<TYPE>&);
    Deque<TYPE>& operator=(const Deque<TYPE>&);

  public:
    // CLASS METHODS
    static
    size_type maxSizeT();
        // Return the maximum value that can be stored in a veriable of type
        // 'size_type'.  The high water mark defaults to having this value.

    // CREATORS
    explicit
    Deque(bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE', with no high
        // water mark.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    explicit
    Deque(bsl::size_t       highWaterMark,
          bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE', with the
        // specified 'highWaterMark'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless 'highWaterMark > 0'.

    template <class INPUT_ITER>
    Deque(INPUT_ITER        begin,
          INPUT_ITER        end,
          bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' containing the
        // sequence of elements in the specified range '[ begin, end )' and
        // having no high water mark.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    template <class INPUT_ITER>
    Deque(INPUT_ITER        begin,
          INPUT_ITER        end,
          bsl::size_t       highWaterMark,
          bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' containing the
        // sequence of 'TYPE' values in the specified range '[ begin, end )'
        // and having the specified 'highWaterMark'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'highWaterMark > 0'.  Note that if the number of
        // elements in the range '[begin, end)' exceeds 'highWaterMark', the
        // effect will be the same as if the extra elements were added by
        // forced pushes.

    ~Deque();
        // Destroy this queue.

    // MANIPULATORS
    void forcePushBack(const TYPE& item);
        // Append the specified 'item' to the back of this queue without regard
        // for the high-water mark.  Note that this method is provided to allow
        // high priority items to be inserted when the queue is full (i.e., has
        // a number of items greater than or equal to its high-water mark);
        // 'pushFront' and 'pushBack' should be used for general use.

    template <class INPUT_ITER>
    void forcePushBack(INPUT_ITER begin,
                       INPUT_ITER end);
        // Append the specified specified range '[begin, end)' of items to the
        // back of this queue without regard for the high-water mark.

    void forcePushFront(const TYPE& item);
        // Append the specified 'item' to the front of this queue without
        // regard for the high-water mark.  Note that this method is provided
        // to allow high priority items to be inserted when the queue is full
        // (i.e., has a number of items greater than or equal to its high-water
        // mark); 'pushFront' and 'pushBack' should be used for general use.

    template <class INPUT_ITER>
    void forcePushFront(INPUT_ITER begin,
                        INPUT_ITER end);
        // Append the specified specified range '[begin, end)' of items to the
        // front of this queue without regard for the high-water mark.  Note
        // that the items will be in the queue in the reverse of the order in
        // which they occur in the range.

    TYPE popBack();
        // Return the last item in this container and remove it.  If this queue
        // is empty, block until an item is available.

    void popBack(TYPE *item);
        // Remove the last item in this queue and load that item into the
        // specified '*item'.  If this queue is empty, block until an item is
        // available.

    TYPE popFront();
        // Return the first item in this container and remove it.  If this
        // queue is empty, block until an item is available.

    void popFront(TYPE *item);
        // Remove the first item in this queue and load that item into the
        // specified '*item'.  If the queue is empty, block until an item is
        // available.

    void pushBack(const TYPE& item);
        // Append the specified 'item' to the back of this queue.  If the
        // number of items in this queue is greater than or equal to the
        // high-water mark, then block until the number of items in this queue
        // is less than the high-water mark.

    void pushFront(const TYPE& item);
        // Append the specified 'item' to the front of this queue.  If the
        // number of items in this queue is greater than or equal to the
        // high-water mark, then block until the number of items in this queue
        // is less than the high-water mark.

    void removeAll(bsl::vector<TYPE> *buffer = 0);
        // If the optionally specified 'buffer' is non-zero, append all the
        // elements from this container to '*buffer', in the same order, then,
        // regardless of whether 'buffer' is zero, clear this container.  Note
        // that the previous contents of '*buffer' are not discarded -- the
        // removed items are appended to it.

    int timedPopBack(TYPE *item, const bsls::TimeInterval& timeout);
        // Remove the last item in this queue and load that item value into the
        // specified '*item'.  If this queue is empty, block until an item is
        // available or until the specified 'timeout' (expressed as the
        // !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.  Return
        // 0 on success, and a non-zero value if the call timed out before an
        // item was available.  Note that this method can block indefinitely if
        // another thread has the mutex locked, particularly by a proctor
        // object -- there is no guarantee that this method will return after
        // 'timeout'.

    int timedPopFront(TYPE *item, const bsls::TimeInterval& timeout);
        // Remove the first item in this queue and load that item value into
        // the specified '*item'.  If this queue is empty, block until an item
        // is available or until the specified 'timeout' (expressed as the
        // !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.  Return
        // 0 on success, and a non-zero value if the call timed out before an
        // item was available.  Note that this method can block indefinitely if
        // another thread has the mutex locked, particularly by a proctor
        // object -- there is no guarantee that this method will return after
        // 'timeout'.

    int timedPushBack(const TYPE& item, const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the back of this queue.  If the
        // number of items in this queue is greater than or equal to the
        // high-water mark, then block until the number of items in this queue
        // is less than the high-water mark or until the specified 'timeout'
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) expires.  Return 0 on success, and a non-zero value if the
        // call timed out before the number of items in this queue fell below
        // the high-water mark.  Note that this method can block indefinitely
        // if another thread has the mutex locked, particularly by a proctor
        // object -- there is no guarantee that this method will return after
        // 'timeout'.

    int timedPushFront(const TYPE& item,  const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the front of this queue.  If the
        // number of items in this queue is greater than or equal to the
        // high-water mark, then block until the number of items in this queue
        // is less than the high-water mark or until the specified 'timeout'
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) expires.  Return 0 on success, and a non-zero value if the
        // call timed out before the number of items in this queue fell below
        // the high-water mark.  Note that this method can block indefinitely
        // if another thread has the mutex locked, particularly by a proctor
        // object -- there is no guarantee that this method will return after
        // 'timeout'.

    int tryPopBack(TYPE *item);
        // If this queue is non-empty, remove the last item, load that item
        // into the specified '*item', and return 0 indicating success.  If
        // this queue is empty, return a non-zero value with no effect on
        // 'item' or the state of this container.

    void tryPopBack(size_type maxNumItems, bsl::vector<TYPE> *buffer = 0);
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
        // If this queue is non-empty, remove the first item, load that item
        // into the specified '*item', and return 0 indicating success.  If
        // this queue is empty, return a non-zero value with no effect on
        // '*item' or the state of this queue.

    void tryPopFront(size_type maxNumItems, bsl::vector<TYPE> *buffer = 0);
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

    int tryPushBack(const TYPE& item);
        // If the queue is not full, append the specified 'item' to the back of
        // the queue, otherwise leave the queue unchanged.  Return 0 if the
        // queue was updated and a non-zero value otherwise.

    template <class INPUT_ITER>
    size_type tryPushBack(INPUT_ITER begin,
                          INPUT_ITER end);
        // Push as many of the items in the specified range '[begin, end)' as
        // possible to the back of the container, stopping if the high water
        // mark is reached.  Return the number of items pushed.

    int tryPushFront(const TYPE& item);
        // If the queue is not full, append the specified 'item' to the front
        // of the queue, otherwise leave the queue unchanged.  Return 0 if the
        // queue was updated and a non-zero value otherwise.

    template <class INPUT_ITER>
    size_type tryPushFront(INPUT_ITER begin,
                           INPUT_ITER end);
        // Push as many of the items in the specified range '[begin, end)' as
        // possible to the back of the container, stopping if the high water
        // mark is reached.  Return the number of items pushed.

    // ACCESSORS
    size_type highWaterMark() const;
        // Return the high-water mark value for this queue.

    size_type length() const;
        // Return the number of elements contained in this container.  Note
        // that this method temporarily acquires the mutex, so that this method
        // must not be called while a proctor has this container locked, and
        // the value returned is potentially obsolete before it is returned if
        // any other threads are simultaneously modifying this container.  To
        // find the length while a proctor has the container locked, call
        // 'proctor->size()'.
};

                              // ====================
                              // class Deque::Proctor
                              // ====================

template <class TYPE>
class Deque<TYPE>::Proctor {
    // This class defines a proctor type that provides direct access to the
    // underlying 'bsl::deque' contained in a 'Deque'.  Creation of a 'Proctor'
    // object locks the mutex of the 'Deque', and destruction unlocks it.
    //
    // Note that the idiom 'Proctor(&myDeque)->function();' may be dangerous in
    // that the temporary 'Proctor' object may not be destroyed until the end
    // of the block, depending upon the compiler, which means that two such
    // expressions existing within the same block could lead to a deadlock.

    // PRIVATE TYPES
    typedef bsl::deque<TYPE>       MonoDeque;

    // DATA
    Deque<TYPE>                   *d_container_p;
    typename MonoDeque::size_type  d_startLength;  // If '!d_container_p', this
                                                   // field may be left
                                                   // uninitialized.

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
    //
    // Note that the idiom 'ConstProctor(&myDeque)->function();' may be
    // dangerous in that the temporary 'Proctor' object may not be destroyed
    // until the end of the block, depending upon the compiler, which means
    // that two such expressions existing within the same block could lead to a
    // deadlock.

    // PRIVATE TYPES
    typedef bsl::deque<TYPE>       MonoDeque;

    // DATA
    const Deque<TYPE>             *d_container_p;
    typename MonoDeque::size_type  d_startLength;  // If '!d_container_p', this
                                                   // field may be left
                                                   // uninitialized.

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

    bslmt::Mutex *mutex = &d_container_p->d_mutex;
    d_container_p = 0;
    mutex->unlock();
}

// ACCESSORS
template <class TYPE>
inline
bsl::deque<TYPE> *Deque<TYPE>::Proctor::operator->() const
{
    BSLS_ASSERT_SAFE(d_container_p);

    return &d_container_p->d_monoDeque;
}

template <class TYPE>
inline
bsl::deque<TYPE>& Deque<TYPE>::Proctor::operator*() const
{
    BSLS_ASSERT_SAFE(d_container_p);

    return d_container_p->d_monoDeque;
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
    BSLS_ASSERT_SAFE(d_container_p);

    return &d_container_p->d_monoDeque;
}

template <class TYPE>
inline
const bsl::deque<TYPE>& Deque<TYPE>::ConstProctor::operator*() const
{
    BSLS_ASSERT_SAFE(d_container_p);

    return d_container_p->d_monoDeque;
}

template <class TYPE>
inline
bool Deque<TYPE>::ConstProctor::isNull() const
{
    return 0 == d_container_p;
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
{
    BSLS_ASSERT(highWaterMark > 0);
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
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_monoDeque.push_back(item);

    d_notEmptyCondition.signal();
}

template <class TYPE>
template <class INPUT_ITER>
inline
void Deque<TYPE>::forcePushBack(INPUT_ITER begin,
                                INPUT_ITER end)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    const size_type initialSize = d_monoDeque.size();
    d_monoDeque.insert(d_monoDeque.end(), begin, end);
    size_type growth = d_monoDeque.size() - initialSize;

    for (; growth > 0; --growth) {
        d_notEmptyCondition.signal();
    }
}

template <class TYPE>
inline
void Deque<TYPE>::forcePushFront(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_monoDeque.push_front(item);

    d_notEmptyCondition.signal();
}

template <class TYPE>
template <class INPUT_ITER>
inline
void Deque<TYPE>::forcePushFront(INPUT_ITER begin,
                                 INPUT_ITER end)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    for (; end != begin; ++begin) {
        d_monoDeque.push_front(*begin);
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
    TYPE ret(d_monoDeque.back());
    d_monoDeque.pop_back();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }

    return ret;
}

template <class TYPE>
void Deque<TYPE>::popBack(TYPE *item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    *item = d_monoDeque.back();
    d_monoDeque.pop_back();

    if (d_monoDeque.size() < d_highWaterMark) {
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
    TYPE ret(d_monoDeque.front());
    d_monoDeque.pop_front();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }

    return ret;
}

template <class TYPE>
void Deque<TYPE>::popFront(TYPE *item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    *item = d_monoDeque.front();
    d_monoDeque.pop_front();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }
}

template <class TYPE>
void Deque<TYPE>::pushBack(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.size() >= d_highWaterMark) {
        d_notFullCondition.wait(&d_mutex);
    }
    d_monoDeque.push_back(item);

    d_notEmptyCondition.signal();
}

template <class TYPE>
void Deque<TYPE>::pushFront(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.size() >= d_highWaterMark) {
        d_notFullCondition.wait(&d_mutex);
    }
    d_monoDeque.push_front(item);

    d_notEmptyCondition.signal();
}

template <class TYPE>
inline
void Deque<TYPE>::removeAll(bsl::vector<TYPE> *buffer)
{
    Proctor proctor(this);

    if (buffer) {
        buffer->reserve(buffer->size() + d_monoDeque.size());
        buffer->insert(buffer->end(), d_monoDeque.begin(), d_monoDeque.end());
    }

    proctor->clear();
}

template <class TYPE>
int Deque<TYPE>::timedPopBack(TYPE                      *item,
                              const bsls::TimeInterval&  timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    *item = d_monoDeque.back();
    d_monoDeque.pop_back();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPopFront(TYPE                      *item,
                               const bsls::TimeInterval&  timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.empty()) {
        if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    *item = d_monoDeque.front();
    d_monoDeque.pop_front();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushBack(const TYPE&               item,
                               const bsls::TimeInterval& timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.size() >= d_highWaterMark) {
        if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    d_monoDeque.push_back(item);

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::timedPushFront(const TYPE&               item,
                                const bsls::TimeInterval &timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (d_monoDeque.size() >= d_highWaterMark) {
        if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    d_monoDeque.push_front(item);

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
int Deque<TYPE>::tryPopBack(TYPE *item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (d_monoDeque.empty()) {
        return 1;                                                     // RETURN
    }
    *item = d_monoDeque.back();
    d_monoDeque.pop_back();

    if (d_monoDeque.size() < d_highWaterMark) {
        d_notFullCondition.signal();
    }

    return 0;
}

template <class TYPE>
void Deque<TYPE>::tryPopBack(typename Deque<TYPE>::size_type  maxNumItems,
                             bsl::vector<TYPE>               *buffer)
{
    Proctor proctor(this);

    const size_type toMove = bsl::min(d_monoDeque.size(), maxNumItems);

    if (buffer) {
        buffer->reserve(buffer->size() + toMove);
        buffer->insert(buffer->end(),
                       d_monoDeque.rbegin(),
                       d_monoDeque.rbegin() + toMove);
    }
    d_monoDeque.erase(d_monoDeque.end() - toMove, d_monoDeque.end());

    // Signalling will happen automatically when proctor is destroyed.
}

template <class TYPE>
int Deque<TYPE>::tryPopFront(TYPE *item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (d_monoDeque.empty()) {
        return 1;                                                     // RETURN
    }
    *item = d_monoDeque.front();
    d_monoDeque.pop_front();

    if (d_monoDeque.size() < d_highWaterMark) {
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

    // First, calculate 'toMove' and 'toSignal', which drive how the rest of
    // the function behaves.

    const size_type toMove     = bsl::min(d_monoDeque.size(), maxNumItems);
    const Iterator  beginRange = d_monoDeque.begin();
    const Iterator  endRange   = beginRange + toMove;

    if (buffer) {
        buffer->reserve(buffer->size() + toMove);
        buffer->insert(buffer->end(), beginRange, endRange);
    }
    proctor->erase(beginRange, endRange);

    // Signalling will happen automatically when proctor is destroyed.
}

template <class TYPE>
int Deque<TYPE>::tryPushBack(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (d_monoDeque.size() >= d_highWaterMark) {
        return 1;                                                     // RETURN
    }

    d_monoDeque.push_back(item);

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
template <class INPUT_ITER>
typename Deque<TYPE>::size_type
Deque<TYPE>::tryPushBack(INPUT_ITER begin,
                         INPUT_ITER end)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    const size_type startLength = d_monoDeque.size();
    size_type       length = startLength;

    for (; length < d_highWaterMark && end != begin; ++length, ++begin) {
        d_monoDeque.push_back(*begin);
        d_notEmptyCondition.signal();
    }

    return length - startLength;
}

template <class TYPE>
int Deque<TYPE>::tryPushFront(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (d_monoDeque.size() >= d_highWaterMark) {
        return 1;                                                     // RETURN
    }

    d_monoDeque.push_front(item);

    d_notEmptyCondition.signal();

    return 0;
}

template <class TYPE>
template <class INPUT_ITER>
typename Deque<TYPE>::size_type
Deque<TYPE>::tryPushFront(INPUT_ITER begin,
                          INPUT_ITER end)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    const size_type startLength = d_monoDeque.size();
    size_type       length = startLength;

    for (; length < d_highWaterMark && end != begin; ++length, ++begin) {
        d_monoDeque.push_front(*begin);
        d_notEmptyCondition.signal();
    }

    return length - startLength;
}

// ACCESSORS
template <class TYPE>
inline
typename Deque<TYPE>::size_type Deque<TYPE>::highWaterMark() const
{
    // This mutex lock is unnecessary since we decided to make the high water
    // mark into a non-malleable property of this container.
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
