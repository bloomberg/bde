// bdlcc_queue.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_QUEUE
#define INCLUDED_BDLCC_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled queue of items of parameterized 'TYPE'.
//
//@CLASSES:
//   bdlcc::Queue: thread-enabled 'bdlc::Queue' wrapper
//
//@SEE_ALSO: bdlc_queue
//
//@DESCRIPTION: This component provides a thread-enabled implementation of an
// efficient, in-place, indexable, double-ended queue of parameterized 'TYPE'
// values, namely the 'bdlcc::Queue<TYPE>' container.  'bdlcc::Queue' is
// effectively a thread-enabled handle for 'bdlc::Queue', whose interface is
// also made available through 'bdlcc::Queue'.
//
///Thread-Enabled Idioms in the 'bdlcc::Queue' Interface
///-----------------------------------------------------
// The thread-enabled 'bdlcc::Queue' is similar to 'bdlc::Queue' in many
// regards, but there are several differences in method behavior and signature
// that arise due to the thread-enabled nature of the queue and its anticipated
// usage pattern.  Most notably, the 'popFront' and 'popBack' methods return a
// 'TYPE' object *by* *value*, rather than returning 'void', as 'bdlc::Queue'
// does.  Moreover, if a queue object is empty, 'popFront' and 'popBack' will
// block indefinitely until an item is added to the queue.
//
// As a corollary to this behavior choice, 'bdlcc::Queue' also provides
// 'timedPopFront' and 'timedPopBack' methods.  These methods wait until a
// specified timeout expires if the queue is empty, returning an item if one
// becomes available before the specified timeout; otherwise, they return a
// non-zero value to indicate that the specified timeout expired before an item
// was available.  Note that *all* timeouts are expressed as values of type
// 'bsls::TimeInterval' that represent !ABSOLUTE! times from 00:00:00 UTC,
// January 1, 1970.
//
// The behavior of the 'push' methods differs in a similar manner.
// 'bdlcc::Queue' supports the notion of a suggested maximum queue size, called
// the "high-water mark", a value supplied at construction.  The 'pushFront'
// and 'pushBack' methods will block indefinitely if the queue contains (at
// least) the high-water mark number of items, until the number of items falls
// below the high-water mark.  The 'timedPushFront' and 'timedPushBack' are
// provided to limit the duration of blocking; note, however, that these
// methods can fail to add an item to the queue.  For this reason,
// 'bdlcc::Queue' also provides a 'forcePushFront' method that will override
// the high-water mark, if needed, in order to succeed without blocking.  Note
// that this design decision makes the high-water mark concept a suggestion and
// not an invariant.
//
///Use of the 'bdlc::Queue' Interface
///----------------------------------
// Class 'bdlcc::Queue' provides access to an underlying 'bdlc::Queue', so
// clients of 'bdlcc::Queue' have full access to the interface behavior of
// 'bdlc::Queue' to inspect and modify the 'bdlcc::Queue'.
//
// Member function 'bdlcc::Queue::queue()' provides *direct* modifiable access
// to the 'bdlc::Queue' object used in the implementation.  Member functions
// 'bdlcc::Queue::mutex()', 'bdlcc::Queue::notEmptyCondition()', and
// 'bdlcc::Queue::notFullCondition()' correspondingly provide *direct*
// modifiable access to the underlying 'bslmt::Mutex' and 'bslmt::Condition'
// objects respectively.  These underlying objects are used within
// 'bdlcc::Queue' to manage concurrent access to the queue.  Clients may use
// these member variables together if needed.
//
// Whenever accessing the 'bdec' queue directly, clients must be sure to lock
// and unlock the mutex or to signal or broadcast on the condition variable as
// appropriate.  For example, a client might use the underlying queue and mutex
// as follows:
//..
//     bdlcc::Queue<myData>  myWorkQueue;
//     bdlc::Queue<myData>& rawQueue = myWorkQueue.queue();
//     bslmt::Mutex&        queueMutex = myWorkQueue.mutex();
//         // other code omitted...
//
//     myData  data1;
//     myData  data2;
//     bool pairFoundFlag = 0;
//     // Take two items from the queue atomically, if available.
//
//     queueMutex.lock();
//     if (rawQueue.length() >= 2) {
//         data1 = rawQueue.front();
//         rawQueue.popFront();
//         data2 = rawQueue.front();
//         rawQueue.popFront();
//         pairFound = 1;
//     }
//     queueMutex.unlock();
//
//     if (pairFoundFlag) {
//         // Process the pair
//     }
//..
// Note that a future version of this component will provide access to a
// thread-safe "smart pointer" that will manage the 'bdlc::Queue' with respect
// to locking and signaling.  At that time, direct access to the 'bdlc::Queue'
// will be deprecated.  In the meanwhile, the user should be careful to use the
// 'bdlc::Queue' and the synchronization objects properly.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simple Thread Pool
///- - - - - - - - - - - - - - -
// The following example demonstrates a typical usage of a 'bdlcc::Queue'.
//
// This 'bdlcc::Queue' is used to communicate between a single "producer"
// thread and multiple "consumer" threads.  The "producer" will push work
// requests onto the queue, and each "consumer" will iteratively take a work
// request from the queue and service the request.  This example shows a
// partial, simplified implementation of the 'bdlmt::ThreadPool' class.  See
// component 'bdlmt_threadpool' for more information.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
//  enum {
//      k_MAX_CONSUMER_THREADS = 10
//  };
//
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
// details are unimportant for this example.
//..
//  void myDoWork(my_WorkData& data)
//  {
//      // do some stuff...
//      (void)data;
//  }
//..
// The 'myConsumer' function will pop items off the queue and process them.  As
// discussed above, note that the call to 'queue->popFront()' will block until
// there is an item available on the queue.  This function will be executed in
// multiple threads, so that each thread waits in 'queue->popFront()', and
// 'bdlcc::Queue' guarantees that each thread gets a unique item from the
// queue.
//..
//  void myConsumer(bdlcc::Queue<my_WorkRequest> *queue)
//  {
//      while (1) {
//          // 'popFront()' will wait for a 'my_WorkRequest' until available.
//
//          my_WorkRequest item = queue->popFront();
//          if (item.d_type == my_WorkRequest::e_STOP) break;
//          myDoWork(item.d_data);
//      }
//  }
//..
// The function below is a callback for 'bslmt::ThreadUtil', which requires a
// "C" signature.  'bslmt::ThreadUtil::create()' expects a pointer to this
// function, and provides that function pointer to the newly created thread.
// The new thread then executes this function.
//
// Since 'bslmt::ThreadUtil::create()' uses the familiar "C" convention of
// passing a 'void' pointer, our function simply casts that pointer to our
// required type ('bdlcc::Queue<my_WorkRequest*> *'), and then delegates to the
// queue-specific function 'myConsumer', above.
//..
//  extern "C" void *myConsumerThread(void *queuePtr)
//  {
//      myConsumer ((bdlcc::Queue<my_WorkRequest> *)queuePtr);
//      return queuePtr;
//  }
//..
// In this simple example, the 'myProducer' function serves multiple roles: it
// creates the 'bdlcc::Queue', starts out the consumer threads, and then
// produces and queues work items.  When work requests are exhausted, this
// function queues one 'STOP' item for each consumer queue.
//
// When each Consumer thread reads a 'STOP', it terminates its thread-handling
// function.  Note that, although the producer cannot control which thread
// 'pop's a particular work item, it can rely on the knowledge that each
// Consumer thread will read a single 'STOP' item and then terminate.
//
// Finally, the 'myProducer' function "joins" each Consumer thread, which
// ensures that the thread itself will terminate correctly; see the
// 'bslmt_threadutil' component for details.
//..
//  void myProducer(int numThreads)
//  {
//      my_WorkRequest item;
//      my_WorkData    workData;
//
//      bdlcc::Queue<my_WorkRequest> queue;
//
//      assert(0 < numThreads && numThreads <= k_MAX_CONSUMER_THREADS);
//      bslmt::ThreadUtil::Handle consumerHandles[k_MAX_CONSUMER_THREADS];
//
//      for (int i = 0; i < numThreads; ++i) {
//          bslmt::ThreadUtil::create(&consumerHandles[i],
//                                    myConsumerThread,
//                                    &queue);
//      }
//
//      while (!getWorkData(&workData)) {
//          item.d_type = my_WorkRequest::e_WORK;
//          item.d_data = workData;
//          queue.pushBack(item);
//      }
//
//      for (int i = 0; i < numThreads; ++i) {
//          item.d_type = my_WorkRequest::e_STOP;
//          queue.pushBack(item);
//      }
//
//      for (int i = 0; i < numThreads; ++i) {
//          bslmt::ThreadUtil::join(consumerHandles[i]);
//      }
//  }
//..
//
///Example 2: Multi-Threaded Observer
/// - - - - - - - - - - - - - - - - -
// The previous example shows a simple mechanism for distributing work requests
// over multiple threads.  This approach works well for large tasks that can be
// decomposed into discrete, independent tasks that can benefit from parallel
// execution.  Note also that the various threads are synchronized only at the
// end of execution, when the Producer "joins" the various consumer threads.
//
// The simple strategy used in the first example works well for tasks that
// share no state, and are completely independent of one another.  For
// instance, a web server might use a similar strategy to distribute http
// requests across multiple worker threads.
//
// In more complicated examples, it is often necessary or desirable to
// synchronize the separate tasks during execution.  The second example below
// shows a single "Observer" mechanism that receives event notification from
// the various worker threads.
//
// We first create a simple 'my_Event' data type.  Worker threads will use this
// data type to report information about their work.  In our example, we will
// report the "worker Id", the event number, and some arbitrary text.
//
// As with the previous example, class 'my_Event' also contains an 'EventType',
// which is an enumeration which that indicates whether the worker has
// completed all work.  The "Observer" will use this enumerated value to note
// when a Worker thread has completed its work.
//..
//  enum {
//      k_MAX_CONSUMER_THREADS = 10,
//      k_MAX_EVENT_TEXT       = 80
//  };
//
//  struct my_Event {
//      enum EventType {
//          e_IN_PROGRESS   = 1,
//          e_TASK_COMPLETE = 2
//      };
//
//      EventType d_type;
//      int       d_workerId;
//      int       d_eventNumber;
//      char      d_eventText[k_MAX_EVENT_TEXT];
//  };
//..
// As noted in the previous example, 'bslmt::ThreadUtil::create()' spawns a new
// thread, which invokes a simple "C" function taking a 'void' pointer.  In the
// previous example, we simply converted that 'void' pointer into a pointer to
// the parameterized 'bdlcc::Queue<TYPE>' object.
//
// In this example, we want to pass an additional data item.  Each worker
// thread is initialized with a unique integer value ("worker Id") that
// identifies that thread.  We create a simple data structure that contains
// both of these values:
//..
//  struct my_WorkerData {
//      int                     d_workerId;
//      bdlcc::Queue<my_Event> *d_queue_p;
//  };
//..
// Function 'myWorker' simulates a working thread by enqueuing multiple
// 'my_Event' events during execution.  In a normal application, each
// 'my_Event' structure would likely contain different textual information; for
// the sake of simplicity, our loop uses a constant value for the text field.
//..
//  void myWorker(int workerId, bdlcc::Queue<my_Event> *queue)
//  {
//      const int NEVENTS = 5;
//      int evnum;
//
//      for (evnum = 0; evnum < NEVENTS; ++evnum) {
//          my_Event ev = {
//              my_Event::e_IN_PROGRESS,
//              workerId,
//              evnum,
//              "In-Progress Event"
//          };
//          queue->pushBack(ev);
//      }
//
//      my_Event ev = {
//          my_Event::e_TASK_COMPLETE,
//          workerId,
//          evnum,
//          "Task Complete"
//      };
//      queue->pushBack(ev);
//  }
//..
// The callback function invoked by 'bslmt::ThreadUtil::create()' takes the
// traditional 'void' pointer.  The expected data is the composite structure
// 'my_WorkerData'.  The callback function casts the 'void' pointer to the
// application-specific data type and then uses the referenced object to
// construct a call to the 'myWorker' function.
//..
//  extern "C" void *myWorkerThread(void *v_worker_p)
//  {
//      my_WorkerData *worker_p = (my_WorkerData *) v_worker_p;
//      myWorker(worker_p->d_workerId, worker_p->d_queue_p);
//      return v_worker_p;
//  }
//..
// For the sake of simplicity, we will implement the Observer behavior in the
// main thread.  The 'void' function 'myObserver' starts out multiple threads
// running the 'myWorker' function, reads 'my_Event's from the queue, and logs
// all messages in the order of arrival.
//
// As each 'myWorker' thread terminates, it sends a 'e_TASK_COMPLETE' event.
// Upon receiving this event, the 'myObserver' function uses the 'd_workerId'
// to find the relevant thread, and then "joins" that thread.
//
// The 'myObserver' function determines when all tasks have completed simply by
// counting the number of 'e_TASK_COMPLETE' messages received.
//..
//  void myObserver()
//  {
//      const int NTHREADS = 10;
//      bdlcc::Queue<my_Event> queue;
//
//      assert(NTHREADS > 0 && NTHREADS <= k_MAX_CONSUMER_THREADS);
//      bslmt::ThreadUtil::Handle workerHandles[k_MAX_CONSUMER_THREADS];
//
//      my_WorkerData workerData;
//      workerData.d_queue_p = &queue;
//      for (int i = 0; i < NTHREADS; ++i) {
//          workerData.d_workerId = i;
//          bslmt::ThreadUtil::create(&workerHandles[i],
//                                    myWorkerThread,
//                                    &workerData);
//      }
//      int nStop = 0;
//      while (nStop < NTHREADS) {
//          my_Event ev = queue.popFront();
//          bsl::cout << "[" << ev.d_workerId    << "] "
//                           << ev.d_eventNumber << ". "
//                           << ev.d_eventText   << bsl::endl;
//          if (my_Event::e_TASK_COMPLETE == ev.d_type) {
//              ++nStop;
//              bslmt::ThreadUtil::join(workerHandles[ev.d_workerId]);
//          }
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BDLC_QUEUE
#include <bdlc_queue.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                                // ===========
                                // class Queue
                                // ===========

template <class TYPE>
class Queue {
    // This class provides a thread-enabled implementation of an efficient,
    // in-place, indexable, double-ended queue of parameterized 'TYPE' values.
    // Very efficient access to the underlying 'bdlc::Queue' object is
    // provided, as well as to a 'bslmt::Mutex' and a 'bslmt::Condition'
    // variable, to facilitate thread-safe use of the 'bdlc::Queue'.  Note that
    // 'Queue' is not a value-semantic type, but the underlying 'bdlc::Queue'
    // is.  In this regard, 'Queue' is a thread-enabled handle for a
    // 'bdlc::Queue'.

    // PRIVATE TYPES
    typedef typename bdlc::Queue<TYPE>::InitialCapacity QueueCapacity;
                                          // We need this typedef to work
                                          // around a bug in Sun WorkShop 6
                                          // update 1: if the typedef is
                                          // replaced by its actual definition
                                          // in the two constructors
                                          // initialization list, the compiler
                                          // erroneously reports a syntax error
                                          // ("Expected an expression").

    // DATA
    mutable
    bslmt::Mutex      d_mutex;             // mutex object used to synchronize
                                          // access to this queue

    bslmt::Condition  d_notEmptyCondition; // condition variable used to signal
                                          // that new data is available in the
                                          // queue

    bslmt::Condition  d_notFullCondition;  // condition variable used to signal
                                          // when there is room available to
                                          // add new data to the queue

    bdlc::Queue<TYPE> d_queue;             // the queue, with allocator as last
                                          // data member

    int              d_highWaterMark;     // positive maximum number of items
                                          // that can be queued before
                                          // insertions will be blocked, or
                                          // -1 if unlimited

  private:
    // NOT IMPLEMENTED
    Queue(const Queue<TYPE>&);
    Queue<TYPE>& operator=(const Queue<TYPE>&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Queue, bslalg::TypeTraitUsesBslmaAllocator);

    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in items).  For example,
        //..
        //   const Queue<int>::InitialCapacity NUM_ITEMS(8));
        //   Queue<int> x(NUM_ITEMS);
        //..
        // defines an instance 'x' with an initial capacity of 8 items, but
        // with a logical length of 0 items.

        // DATA
        unsigned int d_i;

        // CREATORS
        explicit InitialCapacity(int i)
        : d_i(i)
            // Create an object with the specified value 'i'.
        {}
    };

    // CREATORS
    explicit
    Queue(bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit
    Queue(int highWaterMark, bslma::Allocator *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' having either the
        // specified 'highWaterMark' suggested maximum length if
        // 'highWaterMark' is positive, or no maximum length if 'highWaterMark'
        // is negative.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'highWaterMark != 0'.

    explicit
    Queue(const InitialCapacity&  numItems,
          bslma::Allocator       *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' with sufficient
        // initial capacity to accommodate up to the specified 'numItems'
        // values without subsequent reallocation.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    Queue(const InitialCapacity&  numItems,
          int                     highWaterMark,
          bslma::Allocator       *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' with sufficient
        // initial capacity to accommodate up to the specified 'numItems'
        // values without subsequent reallocation and having either the
        // specified 'highWaterMark' suggested maximum length if
        // 'highWaterMark' is positive, or no maximum length if 'highWaterMark'
        // is negative.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'highWaterMark != 0'.

    Queue(const bdlc::Queue<TYPE>&  srcQueue,
          bslma::Allocator         *basicAllocator = 0);        // IMPLICIT
        // Create a queue of objects of parameterized 'TYPE' containing the
        // sequence of 'TYPE' values from the specified 'srcQueue'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Queue(const bdlc::Queue<TYPE>&  srcQueue,
          int                       highWaterMark,
          bslma::Allocator         *basicAllocator = 0);
        // Create a queue of objects of parameterized 'TYPE' containing the
        // sequence of 'TYPE' values from the specified 'srcQueue' and having
        // either the specified 'highWaterMark' suggested maximum length if
        // 'highWaterMark' is positive, or no maximum length if 'highWaterMark'
        // is negative.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'highWaterMark != 0'.

    ~Queue();
        // Destroy this queue.

    // MANIPULATORS
    void popBack(TYPE *buffer);
        // Remove the last item in this queue and load that item into the
        // specified 'buffer'.  If this queue is empty, block until an item is
        // available.

    TYPE popBack();
        // Remove the last item in this queue and return that item value.  If
        // this queue is empty, block until an item is available.

    int timedPopBack(TYPE *buffer, const bsls::TimeInterval& timeout);
        // Remove the last item in this queue and load that item value into the
        // specified 'buffer'.  If this queue is empty, block until an item is
        // available or until the specified 'timeout' (expressed as the
        // !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.  Return
        // 0 on success, and a non-zero value if the call timed out before an
        // item was available.

    void popFront(TYPE *buffer);
        // Remove the first item in this queue and load that item into the
        // specified 'buffer'.  If the queue is empty, block until an item is
        // available.

    TYPE popFront();
        // Remove the first item in this queue and return that item value.  If
        // the queue is empty, block until an item is available.

    int timedPopFront(TYPE *buffer, const bsls::TimeInterval& timeout);
        // Remove the first item in this queue and load that item value into
        // the specified 'buffer'.  If this queue is empty, block until an item
        // is available or until the specified 'timeout' (expressed as the
        // !ABSOLUTE! time from 00:00:00 UTC, January 1, 1970) expires.  Return
        // 0 on success, and a non-zero value if the call timed out before an
        // item was available.

    void removeAll(bsl::vector<TYPE> *buffer = 0);
        // Remove all the items in this queue.  If the optionally specified
        // 'buffer' is not 0, load into 'buffer' a copy of the items removed in
        // front to back order of the queue prior to 'removeAll'.

    void pushBack(const TYPE& item);
        // Append the specified 'item' to the back of this queue.  If the
        // high-water mark is non-negative and the number of items in this
        // queue is greater than or equal to the high-water mark, then block
        // until the number of items in this queue is less than the high-water
        // mark.

    void pushFront(const TYPE& item);
        // Append the specified 'item' to the front of this queue.  If the
        // high-water mark is non-negative and the number of items in this
        // queue is greater than or equal to the high-water mark, then block
        // until the number of items in this queue is less than the high-water
        // mark.

    int timedPushBack(const TYPE& item, const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the back of this queue.  If the
        // high-water mark is non-negative and the number of items in this
        // queue is greater than or equal to the high-water mark, then block
        // until the number of items in this queue is less than the high-water
        // mark or until the specified 'timeout' (expressed as the !ABSOLUTE!
        // time from 00:00:00 UTC, January 1, 1970) expires.  Return 0 on
        // success, and a non-zero value if the call timed out before the
        // number of items in this queue fell below the high-water mark.

    int timedPushFront(const TYPE& item, const bsls::TimeInterval& timeout);
        // Append the specified 'item' to the front of this queue.  If the high
        // water mark is non-negative and the number of items in this queue is
        // greater than or equal to the high-water mark, then block until the
        // number of items in this queue is less than the high-water mark or
        // until the specified 'timeout' (expressed as the !ABSOLUTE! time from
        // 00:00:00 UTC, January 1, 1970) expires.  Return 0 on success, and a
        // non-zero value if the call timed out before the number of items in
        // this queue fell below the high-water mark.

    void forcePushFront(const TYPE& item);
        // Append the specified 'item' to the front of this queue without
        // regard for the high-water mark.  Note that this method is provided
        // to allow high priority items to be inserted when the queue is full
        // (i.e., has a number of items greater than or equal to its high-water
        // mark); 'pushFront' and 'pushBack' should be used for general use.

    int tryPopFront(TYPE *buffer);
        // If this queue is non-empty, remove the first item, load that item
        // into the specified 'buffer', and return 0 indicating success.  If
        // this queue is empty, return a non-zero value with no effect on
        // 'buffer' or the state of this queue.  This method never blocks.

    void tryPopFront(int maxNumItems, bsl::vector<TYPE> *buffer = 0);
        // Remove up to the specified 'maxNumItems' from the front of this
        // queue.  Optionally specify a 'buffer' into which the items removed
        // from the queue are loaded.  If 'buffer' is non-null, the removed
        // items are appended to it as if by repeated application of
        // 'buffer->push_back(popFront())' while the queue is not empty and
        // 'maxNumItems' have not yet been removed.  The behavior is undefined
        // unless 'maxNumItems >= 0'.  This method never blocks.

    int tryPopBack(TYPE *buffer);
        // If this queue is non-empty, remove the last item, load that item
        // into the specified 'buffer', and return 0 indicating success.  If
        // this queue is empty, return a non-zero value with no effect on
        // 'buffer' or the state of this queue.  This method never blocks.

    void tryPopBack(int maxNumItems, bsl::vector<TYPE> *buffer = 0);
        // Remove up to the specified 'maxNumItems' from the back of this
        // queue.  Optionally specify a 'buffer' into which the items removed
        // from the queue are loaded.  If 'buffer' is non-null, the removed
        // items are appended to it as if by repeated application of
        // 'buffer->push_back(popBack())' while the queue is not empty and
        // 'maxNumItems' have not yet been removed.  This method never blocks.
        // The behavior is undefined unless 'maxNumItems >= 0'.  Note that the
        // ordering of the items in '*buffer' after the call is the reverse of
        // the ordering they had in the queue.

    // *** Modifiable access to the mutex, condition variable, and queue ***

    bslmt::Condition& condition();
        // Return a reference to the modifiable condition variable used by this
        // queue to signal that the queue is not empty.
        //
        // *DEPRECATED* Use 'notEmptyCondition' instead.

    bslmt::Condition& insertCondition();
        // Return a reference to the modifiable condition variable used by this
        // queue to signal that the queue is not full (i.e., has fewer items
        // than its high-water mark).
        //
        // *DEPRECATED* Use 'notFullCondition' instead.

    bslmt::Mutex& mutex();
        // Return a reference to the modifiable mutex used by this queue to
        // synchronize access to its underlying 'bdlc::Queue' object.

    bslmt::Condition& notEmptyCondition();
        // Return the condition variable used by this queue to signal that the
        // queue is not empty.

    bslmt::Condition& notFullCondition();
        // Return the condition variable used by this queue to signal that the
        // queue is not full (i.e., has fewer items than its high-water mark).

    bdlc::Queue<TYPE>& queue();
        // Return a reference to the modifiable underlying 'bdlc::Queue' object
        // used by this queue.  Any access to the returned queue MUST first
        // lock the associated mutex object (see the 'mutex' method) in a
        // multi-threaded environment.  And when items are directly added to
        // the queue returned by this method, the associated condition variable
        // (see the 'condition' method) should be signaled to notify any
        // waiting threads of the availability of the new data.
        //
        // The (error-prone) usage of this method will be replaced by an
        // appropriate smart-pointer-like proctor object in the future.
        // Meanwhile, use this method with caution.

    // ACCESSORS
    int highWaterMark() const;
        // Return the high-water mark value for this queue.  Note that a
        // negative value indicates no suggested-maximum capacity, and is not
        // necessarily the same negative value that was passed to the
        // constructor.

    int length() const;
        // Return the number of elements in this queue.  Note that if other
        // threads are manipulating the queue, this information may be obsolete
        // by the time it is returned.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE>
inline
Queue<TYPE>::Queue(bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_highWaterMark(-1)
{
}

template <class TYPE>
inline
Queue<TYPE>:: Queue(const InitialCapacity&  numItems,
                    bslma::Allocator       *basicAllocator)
: d_queue(QueueCapacity(numItems.d_i), basicAllocator)
, d_highWaterMark(-1)
{
}

template <class TYPE>
inline
Queue<TYPE>::Queue(int highWaterMark, bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_highWaterMark(highWaterMark < 0 ? -1 : highWaterMark)
{
}

template <class TYPE>
inline
Queue<TYPE>::Queue(const InitialCapacity&  numItems,
                   int                     highWaterMark,
                   bslma::Allocator       *basicAllocator)
: d_queue(QueueCapacity(numItems.d_i), basicAllocator)
, d_highWaterMark(highWaterMark < 0 ? -1 : highWaterMark)
{
}

template <class TYPE>
inline
Queue<TYPE>::Queue(const bdlc::Queue<TYPE>&  srcQueue,
                   bslma::Allocator         *basicAllocator)
: d_queue(srcQueue, basicAllocator)
, d_highWaterMark(-1)
{
}

template <class TYPE>
inline
Queue<TYPE>::Queue(const bdlc::Queue<TYPE>&  srcQueue,
                   int                       highWaterMark,
                   bslma::Allocator         *basicAllocator)
: d_queue(srcQueue, basicAllocator)
, d_highWaterMark(highWaterMark < 0 ? -1 : highWaterMark)
{
}

template <class TYPE>
inline
Queue<TYPE>::~Queue()
{
}

// MANIPULATORS
template <class TYPE>
void Queue<TYPE>::popBack(TYPE *buffer)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    *buffer = d_queue.back();
    d_queue.popBack();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
}

template <class TYPE>
TYPE Queue<TYPE>::popBack()
{
    // Note that this method is not implemented in terms of 'popBack(TYPE*)'
    // because that would require TYPE to have a default constructor.

    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    TYPE back = d_queue.back();
    d_queue.popBack();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return back;
}

template <class TYPE>
int Queue<TYPE>::timedPopBack(TYPE *buffer, const bsls::TimeInterval& timeout)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    *buffer = d_queue.back();
    d_queue.popBack();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return 0;
}

template <class TYPE>
void Queue<TYPE>::popFront(TYPE *buffer)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    *buffer = d_queue.front();
    d_queue.popFront();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
}

template <class TYPE>
TYPE Queue<TYPE>::popFront()
{
    // Note that this method is not implemented in terms of 'popFront(TYPE*)'
    // because that would require TYPE to have a default constructor.

    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        d_notEmptyCondition.wait(&d_mutex);
    }
    TYPE front = d_queue.front();
    d_queue.popFront();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return front;
}

template <class TYPE>
int Queue<TYPE>::timedPopFront(TYPE *buffer, const bsls::TimeInterval& timeout)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    while (0 == (length = d_queue.length())) {
        if (d_notEmptyCondition.timedWait(&d_mutex, timeout)) {
            return 1;                                                 // RETURN
        }
    }
    *buffer = d_queue.front();
    d_queue.popFront();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return 0;
}

template <class TYPE>
int Queue<TYPE>::tryPopFront(TYPE *buffer)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (0 == (length = d_queue.length())) {
        return 1;                                                     // RETURN
    }
    *buffer = d_queue.front();
    d_queue.popFront();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return 0;
}

template <class TYPE>
void Queue<TYPE>::tryPopFront(int maxNumItems, bsl::vector<TYPE> *buffer)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int        length  = d_queue.length();
    const bool wasFull = d_highWaterMark > 0 && length >= d_highWaterMark;

    for (; d_queue.length() > 0 && maxNumItems > 0; --maxNumItems) {
        if (buffer) {
            buffer->push_back(d_queue.front());
        }
        d_queue.popFront();
        --length;
        if (wasFull && length < d_highWaterMark) {
            d_notFullCondition.signal();
        }
    }
}

template <class TYPE>
int Queue<TYPE>::tryPopBack(TYPE *buffer)
{
    unsigned int length;

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (0 == (length = d_queue.length())) {
        return 1;                                                     // RETURN
    }
    *buffer = d_queue.back();
    d_queue.popBack();
    --length;

    if (length < (unsigned) d_highWaterMark) {
        d_notFullCondition.signal();
    }
    return 0;
}

template <class TYPE>
void Queue<TYPE>::tryPopBack(int maxNumItems, bsl::vector<TYPE> *buffer)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    int        length  = d_queue.length();
    const bool wasFull = d_highWaterMark > 0 && length >= d_highWaterMark;

    for (; d_queue.length() > 0 && maxNumItems > 0; --maxNumItems) {
        if (buffer) {
            buffer->push_back(d_queue.back());
        }
        d_queue.popBack();
        --length;
        if (wasFull && length < d_highWaterMark) {
            d_notFullCondition.signal();
        }
    }
}

template <class TYPE>
void Queue<TYPE>::removeAll(bsl::vector<TYPE> *buffer)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    bool wasFull = d_highWaterMark > 0 && d_queue.length() >= d_highWaterMark;
    d_queue.removeAll(buffer);
    if (wasFull) {
        for (int i = 0; d_highWaterMark > i; ++i) {
            d_notFullCondition.signal();
        }
    }
}

template <class TYPE>
void Queue<TYPE>::pushBack(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_highWaterMark >= 0) {
        while (d_queue.length() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
    }
    d_queue.pushBack(item);
    d_notEmptyCondition.signal();
}

template <class TYPE>
void Queue<TYPE>::pushFront(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_highWaterMark >= 0) {
        while (d_queue.length() >= d_highWaterMark) {
            d_notFullCondition.wait(&d_mutex);
        }
    }
    d_queue.pushFront(item);
    d_notEmptyCondition.signal();
}

template <class TYPE>
int Queue<TYPE>::timedPushBack(const TYPE&               item,
                               const bsls::TimeInterval& timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_highWaterMark >= 0) {
        while (d_queue.length() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
    }
    d_queue.pushBack(item);
    d_notEmptyCondition.signal();
    return 0;
}

template <class TYPE>
int Queue<TYPE>::timedPushFront(const TYPE&               item,
                                const bsls::TimeInterval& timeout)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    if (d_highWaterMark >= 0) {
        while (d_queue.length() >= d_highWaterMark) {
            if (d_notFullCondition.timedWait(&d_mutex, timeout)) {
                return 1;                                             // RETURN
            }
        }
    }
    d_queue.pushFront(item);
    d_notEmptyCondition.signal();
    return 0;
}

template <class TYPE>
inline
void Queue<TYPE>::forcePushFront(const TYPE& item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_queue.pushFront(item);
    d_notEmptyCondition.signal();
}

// *** Modifiable access to the mutex, condition variable, and queue ***

template <class TYPE>
inline
bslmt::Condition& Queue<TYPE>::condition()
{
    return d_notEmptyCondition;
}

template <class TYPE>
inline
bslmt::Condition& Queue<TYPE>::insertCondition()
{
    return d_notFullCondition;
}

template <class TYPE>
inline
bslmt::Mutex& Queue<TYPE>::mutex()
{
    return d_mutex;
}

template <class TYPE>
inline
bslmt::Condition& Queue<TYPE>::notEmptyCondition()
{
    return d_notEmptyCondition;
}

template <class TYPE>
inline
bslmt::Condition& Queue<TYPE>::notFullCondition()
{
    return d_notFullCondition;
}

template <class TYPE>
inline
bdlc::Queue<TYPE>& Queue<TYPE>::queue()
{
    return d_queue;
}

// ACCESSORS
template <class TYPE>
inline
int Queue<TYPE>::highWaterMark() const
{
    return d_highWaterMark;
}

template <class TYPE>
inline
int Queue<TYPE>::length() const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    return d_queue.length();
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
