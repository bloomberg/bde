// bdlcc_multipriorityqueue.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_MULTIPRIORITYQUEUE
#define INCLUDED_BDLCC_MULTIPRIORITYQUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled parameterized multi-priority queue.
//
//@CLASSES:
//  bdlcc::MultipriorityQueue: thread-enabled, multi-priority queue
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a thread-enabled mechanism,
// 'bdlcc::MultipriorityQueue', implementing a special-purpose priority queue
// container of items of parameterized 'TYPE'.  Each item has a priority which,
// for efficiency of implementation, is limited to a relatively small number
// 'N' of contiguous integers '[ 0 .. N - 1 ]', with 'N' indicated at
// construction, and 0 being the most urgent priority.  This queue also takes
// an optional allocator, supplied at construction.  Once configured, these
// instance parameters remain unchanged for the life of each multi-priority
// queue.
//
///Thread-Enabled Idioms in the 'bdlcc::MultipriorityQueue' Interface
///------------------------------------------------------------------
// The thread-enabled 'bdlcc::MultipriorityQueue' is, in many regards, similar
// to a value-semantic type in that there is an obvious abstract notion of
// "value" that can be described in terms of salient attributes, which for this
// type is a sequence of priority/element pairs, constrained to be in
// increasing order of priority.  There are, however, several differences in
// method behavior and signature that arise due to the thread-enabled nature of
// the queue and its anticipated usage pattern.
//
// For example, if a queue object is empty, 'popFront' will block indefinitely
// until an element is added to the queue.  Also, since dynamic instance
// information, such as the number of elements currently in a queue, can be
// out-of-date by the time it is returned, some manipulators (e.g.,
// 'tryPopFront') are deliberately combined with an accessor operation (e.g.,
// 'isEmpty') in order to guarantee proper behavior.
//
// Finally, note that although the parameterized 'TYPE' is expected to at least
// support copy construction and assignment, the
// 'bdec::MultipriorityQueue<TYPE>' type currently does not support any
// value-semantic operations, since different queues could have different
// numbers of priorities, making comparison, assignment and copy construction
// awkward.
//
///Possible Future Enhancements
///----------------------------
// In addition to 'popFront' and 'tryPopFront', a 'bdlcc::MultipriorityQueue'
// may some day also provide a 'timedPopFront' method.  This method would block
// until it is able to complete successfully or until the specified time limit
// expires.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simple Thread Pool
///- - - - - - - - - - - - - - -
// This example demonstrates how we might use a 'bdlcc::MultipriorityQueue' to
// communicate between a single "producer" thread and multiple "consumer"
// threads.  The "producer" pushes work requests of varying priority onto the
// queue, and each "consumer" iteratively takes the highest priority work
// request from the queue and services it.
//
// We begin our example with some utility classes that define a simple "work
// item":
//..
//  enum {
//      k_MAX_CONSUMER_THREADS = 10
//  };
//
//  struct MyWorkData {
//      int d_i;        // input to work to be done
//
//      // Work data...
//  };
//
//  struct MyWorkRequest {
//      enum RequestType {
//          e_WORK = 1,
//          e_STOP = 2
//      };
//
//      RequestType d_type;
//      MyWorkData  d_data;
//
//      // Work data...
//  };
//..
// Next, we provide a simple function to service an individual work item, and a
// function to get a work item.  The details are unimportant for this example:
//..
//  void myDoWork(MyWorkData& data)
//  {
//      // Do work...
//      (void)data;
//  }
//
//  int getWorkData(MyWorkData *result)
//  {
//      static int count = 0;
//      result->d_i = rand();   // Only one thread runs this routine, so it
//                              // does not matter that 'rand()' is not
//                              // thread-safe, or that 'count' is 'static'.
//
//      return ++count >= 100;
//  }
//..
// The 'myConsumer' function (below) will pop elements off the queue in
// priority order and process them.  As discussed above, note that the call to
// 'queue->popFront(&item)' will block until there is an element available on
// the queue.  This function will be executed in multiple threads, so that each
// thread waits in 'queue->popFront()'; 'bdlcc::MultipriorityQueue' guarantees
// that each thread gets a unique element from the queue:
//..
//  void myConsumer(bdlcc::MultipriorityQueue<MyWorkRequest> *queue)
//  {
//      MyWorkRequest item;
//      while (1) {
//
//          // The 'popFront' function will wait for a 'MyWorkRequest' until
//          // one is available.
//
//          queue->popFront(&item);
//
//          if (MyWorkRequest::e_STOP == item.d_type) {
//              break;
//          }
//
//          myDoWork(item.d_data);
//      }
//  }
//..
// The 'myConsumerThread' function below is a callback for 'bslmt::ThreadUtil',
// which requires a "C" signature.  'bslmt::ThreadUtil::create()' expects a
// pointer to this function, and provides that function pointer to the
// newly-created thread.  The new thread then executes this function.
//
// Since 'bslmt::ThreadUtil::create()' uses the familiar "C" convention of
// passing a 'void' pointer, our function simply casts that pointer to our
// required type ('bdlcc::MultipriorityQueue<MyWorkRequest> *'), and then
// delegates to the queue-specific function 'myConsumer' (above):
//..
//  extern "C" void *myConsumerThread(void *queuePtr)
//  {
//      myConsumer ((bdlcc::MultipriorityQueue<MyWorkRequest>*) queuePtr);
//      return queuePtr;
//  }
//..
// In this simple example, the 'myProducer' function (below) serves multiple
// roles: it creates the 'bdlcc::MultipriorityQueue', starts the consumer
// threads, and then produces and queues work items.  When work requests are
// exhausted, this function queues one 'e_STOP' item for each consumer thread.
//
// When each consumer thread reads a 'e_STOP', it terminates its
// thread-handling function.  Note that, although the producer cannot control
// which thread pops a particular work item, it can rely on the knowledge that
// each consumer thread will read a single 'e_STOP' item and then terminate.
//
// Finally, the 'myProducer' function "joins" each consumer thread, which
// ensures that the thread itself will terminate correctly (see the
// 'bslmt_threadutil' component-level documentation for details):
//..
//  void myProducer()
//  {
//      enum {
//          k_NUM_PRIORITIES = 8,
//          k_NUM_THREADS    = 8
//      };
//
//      MyWorkRequest item;
//      MyWorkData    workData;
//
//      // Create multi-priority queue with specified number of priorities.
//
//      bdlcc::MultipriorityQueue<MyWorkRequest> queue(k_NUM_PRIORITIES);
//
//      // Start the specified number of threads.
//
//      assert(0 < k_NUM_THREADS
//          && k_NUM_THREADS <= static_cast<int>(k_MAX_CONSUMER_THREADS));
//      bslmt::ThreadUtil::Handle consumerHandles[k_MAX_CONSUMER_THREADS];
//
//      for (int i = 0; i < k_NUM_THREADS; ++i) {
//          bslmt::ThreadUtil::create(&consumerHandles[i],
//                                   myConsumerThread,
//                                   &queue);
//      }
//
//      // Load work data into work requests and push them onto the queue with
//      // varying priority until all work data has been exhausted.
//
//      int count = 0;                          // used to generate priorities
//
//      while (!getWorkData(&workData)) {       // see declaration (above)
//          item.d_type = MyWorkRequest::e_WORK;
//          item.d_data = workData;
//          queue.pushBack(item, count % k_NUM_PRIORITIES);  // mixed
//                                                           // priorities
//          ++count;
//      }
//
//      // Load as many stop requests as there are active consumer threads.
//
//      for (int i = 0; i < k_NUM_THREADS; ++i) {
//          item.d_type = MyWorkRequest::e_STOP;
//          queue.pushBack(item, k_NUM_PRIORITIES - 1);  // lowest priority
//      }
//
//      // Join all of the consumer threads back with the main thread.
//
//      for (int i = 0; i < k_NUM_THREADS; ++i) {
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
// end of execution, when the producer "joins" the various consumer threads.
//
// The simple strategy used in the first example works well for tasks that
// share no state, and are completely independent of one another.  For
// instance, a web server might use a similar strategy to distribute 'http'
// requests across multiple worker threads.
//
// In more complicated examples, it is often necessary or desirable to
// synchronize the separate tasks during execution.  The second example below
// shows a single "Observer" mechanism that receives event notification from
// the various worker threads.
//
// We first create a simple 'MyEvent' data type.  Worker threads will use this
// type to report information about their work.  In our example, we will report
// the "worker Id", the event number, and some arbitrary text.
//
// As with the previous example, class 'MyEvent' also contains an 'EventType',
// an enumeration that indicates whether the worker has completed all work.
// The "Observer" will use this enumerated value to note when a worker thread
// has completed its work:
//..
//  enum {
//      k_MAX_CONSUMER_THREADS = 10,
//      k_MAX_EVENT_TEXT       = 80
//  };
//
//  struct MyEvent {
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
// 'bdlcc::MultipriorityQueue<MyWorkRequest>'.
//
// In this example, however, we want to pass an additional data item.  Each
// worker thread is initialized with a unique integer value ("worker Id"),
// which identifies that thread.  We therefore create a simple 'struct' that
// contains both of these values:
//..
//  struct MyWorkerData {
//      int                               d_workerId;
//      bdlcc::MultipriorityQueue<MyEvent> *d_queue;
//  };
//..
// Function 'myWorker' (below) simulates a working thread by enqueuing multiple
// 'MyEvent' events during execution.  In a realistic application, each
// 'MyEvent' structure would likely contain different textual information.  For
// the sake of simplicity, however, our loop uses a constant value for the text
// field.  Note that various priorities are generated to illustrate the
// multi-priority aspect of this particular queue:
//..
//  void myWorker(int workerId, bdlcc::MultipriorityQueue<MyEvent> *queue)
//  {
//      const int N = queue->numPriorities();
//      const int NUM_EVENTS = 5;
//      int eventNumber;    // used also to generate mixed priorities
//
//      // First push 'NUM_EVENTS' events onto 'queue' with mixed priorities.
//
//      for (eventNumber = 0; eventNumber < NUM_EVENTS; ++eventNumber) {
//          MyEvent ev = {
//              MyEvent::e_IN_PROGRESS,
//              workerId,
//              eventNumber,
//              "In-Progress Event"         // constant (for simplicity)
//          };
//          queue->pushBack(ev, eventNumber % N);       // mixed priorities
//      }
//
//      // Now push an event to end this task.
//
//      MyEvent ev = {
//          MyEvent::e_TASK_COMPLETE,
//          workerId,
//          eventNumber,
//          "Task Complete"
//      };
//      queue->pushBack(ev, N - 1);                     // lowest priority
//  }
//..
// The callback function 'myWorkerThread' (below) invoked by
// 'bslmt::ThreadUtil::create' takes the traditional 'void' pointer.  The
// expected data is the composite structure 'MyWorkerData'.  The callback
// function casts the 'void' pointer to the application-specific data type and
// then uses the referenced object to construct a call to the 'myWorker'
// function:
//..
//  extern "C" void *myWorkerThread(void *vWorkerPtr)
//  {
//      MyWorkerData *workerPtr = (MyWorkerData *)vWorkerPtr;
//      myWorker(workerPtr->d_workerId, workerPtr->d_queue);
//      return vWorkerPtr;
//  }
//..
// For the sake of simplicity, we will implement the Observer behavior (below)
// in the main thread.  The 'void' function 'myObserver' starts multiple
// threads running the 'myWorker' function, reads 'MyEvent' values from the
// queue, and logs all messages in the order of arrival.
//
// As each 'myWorker' thread terminates, it sends a 'e_TASK_COMPLETE' event.
// Upon receiving this event, the 'myObserver' function uses the 'd_workerId'
// to find the relevant thread, and then "joins" that thread.
//
// The 'myObserver' function determines when all tasks have completed simply by
// counting the number of 'e_TASK_COMPLETE' messages received:
//..
//  void myObserver()
//  {
//      const int k_NUM_THREADS    = 10;
//      const int k_NUM_PRIORITIES = 4;
//
//      bdlcc::MultipriorityQueue<MyEvent> queue(k_NUM_PRIORITIES);
//
//      assert(0 < k_NUM_THREADS
//          && k_NUM_THREADS <= static_cast<int>(k_MAX_CONSUMER_THREADS));
//      bslmt::ThreadUtil::Handle workerHandles[k_MAX_CONSUMER_THREADS];
//
//      // Create 'k_NUM_THREADS' threads, each having a unique "worker id".
//
//      MyWorkerData workerData[k_NUM_THREADS];
//      for (int i = 0; i < k_NUM_THREADS; ++i) {
//          workerData[i].d_queue = &queue;
//          workerData[i].d_workerId = i;
//          bslmt::ThreadUtil::create(&workerHandles[i],
//                                   myWorkerThread,
//                                   &workerData[i]);
//      }
//
//      // Now print out each of the 'MyEvent' values as the threads complete.
//      // This function ends after a total of 'k_NUM_THREADS'
//      // 'MyEvent::e_TASK_COMPLETE' events have been printed.
//
//      int nStop = 0;
//      while (nStop < k_NUM_THREADS) {
//          MyEvent ev;
//          queue.popFront(&ev);
//          bsl::cout << "[" << ev.d_workerId << "] "
//                    << ev.d_eventNumber << ". "
//                    << ev.d_eventText << bsl::endl;
//          if (MyEvent::e_TASK_COMPLETE == ev.d_type) {
//              ++nStop;
//              bslmt::ThreadUtil::join(workerHandles[ev.d_workerId]);
//          }
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOL
#include <bdlma_concurrentpool.h>
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

#ifndef INCLUDED_BDLB_BITUTIL
#include <bdlb_bitutil.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                 // =========================================
                 // local class MultipriorityQueue_Node<TYPE>
                 // =========================================

template <class TYPE>
class MultipriorityQueue_Node {
    // This class handles storage of one item of parameterized 'TYPE' as a node
    // in a linked list of items stored in a multipriority queue for a given
    // priority.  This class is not to be used from outside this component.

    // DATA
    bslalg::ConstructorProxy<TYPE>      d_item;    // object stored in node
    MultipriorityQueue_Node<TYPE> *d_next_p;  // next node on linked list

    // NOT IMPLEMENTED
    MultipriorityQueue_Node(const MultipriorityQueue_Node&);
    MultipriorityQueue_Node& operator=(const MultipriorityQueue_Node&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MultipriorityQueue_Node,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    MultipriorityQueue_Node(const TYPE&              item,
                            MultipriorityQueue_Node *next,
                            bslma::Allocator        *basicAllocator);
        // Create a node containing a copy of the specified 'item' and having
        // the specified 'next' pointer.  Use the specified 'basicAllocator' to
        // supply memory.  The behavior is undefined unless 'basicAllocator' is
        // non-null.  Note that 'item' must be copyable and assignable.

    ~MultipriorityQueue_Node();
        // Destroy this node and free all memory that was allocated on its
        // behalf, if any.

    // MANIPULATORS
    MultipriorityQueue_Node*& nextPtr();
        // Return a reference to the modifiable pointer to the node following
        // this node on the linked list.

    // ACCESSORS
    const MultipriorityQueue_Node *nextPtr() const;
        // Return a pointer to the non-modifiable node following this node on
        // the linked list, or 0 if this node has no successor.

    const TYPE& item() const;
        // Return a reference to the non-modifiable item stored in this node.
};

                       // ==============================
                       // class MultipriorityQueue<TYPE>
                       // ==============================

template <class TYPE>
class MultipriorityQueue {
    // This class implements a thread-enabled multipriority queue whose
    // priorities are restricted to a (small) set of contiguous 'N' integer
    // values, '[ 0 .. N - 1 ]', with 0 being the most urgent.
    //
    // This class does have a notion of value, namely the sequence of
    // priority/element pairs, constrained to be in decreasing order of urgency
    // (i.e., monotonically increasing priority values).  However, no
    // value-semantic operations are implemented.  Note that elements having
    // the same priority are maintained in First-In-First-Out (FIFO) order.
    // Note that the current implementation supports up to a maximum of
    // 'sizeof(int) * CHAR_BIT' priorities.
    //
    // This class is implemented as a set of linked lists, one for each
    // priority.  Two vectors are used to maintain head and tail pointers for
    // the lists.

    // PRIVATE CONSTANTS
    enum {
        k_BITS_PER_INT           = sizeof(int) * CHAR_BIT,
        k_DEFAULT_NUM_PRIORITIES = k_BITS_PER_INT,
        k_MAX_NUM_PRIORITIES     = k_BITS_PER_INT
    };

    // PRIVATE TYPES
    typedef MultipriorityQueue_Node<TYPE> Node;
        // The type of the elements on the linked lists of items that are
        // maintained for the 'N' priorities handled by this multipriority
        // queue.

    typedef bsl::vector<Node *>                NodePtrVector;
        // The type of the vectors of list head and tail pointers.

    // DATA
    mutable bslmt::Mutex d_mutex;          // used to synchronize access
                                          // (including 'const' access)

    bslmt::Condition     d_notEmptyCondition;
                                          // signaled on each push

    NodePtrVector       d_heads;          // pointers to heads of linked lists
                                          // -- one for each priority

    NodePtrVector       d_tails;          // pointers to tails of linked lists
                                          // -- one for each priority

    volatile int        d_notEmptyFlags;  // bit mask indicating priorities for
                                          // which there is data, where bit 0
                                          // is the lowest order bit,
                                          // representing most urgent priority

    bdlma::ConcurrentPool          d_pool;           // memory pool used for
                                                     // node storage

    volatile int        d_length;         // total number of items in this
                                          // multipriority queue

    bool                d_enabledFlag;    // enabled/disabled state of pushes
                                          // to the multipriority queue (does
                                          // not affect pops)

    bslma::Allocator   *d_allocator_p;    // memory allocator (held)

    // NOT IMPLEMENTED
    MultipriorityQueue(const MultipriorityQueue&);
    MultipriorityQueue& operator=(const MultipriorityQueue&);

  private:
    // PRIVATE MANIPULATORS
    int tryPopFrontImpl(TYPE *item, int *itemPriority, bool blockFlag);
        // Attempt to remove (immediately) the least-recently added item having
        // the most urgent priority (lowest value) from this multipriority
        // queue.  If the specified 'blockFlag' is 'true', this method blocks
        // the calling thread until an item becomes available.  On success,
        // load the value of the popped item into the specified 'item'; if the
        // specified 'itemPriority' is non-null, load the priority of the
        // popped item into 'itemPriority'; and return 0.  Otherwise, leave
        // 'item' and 'itemPriority' unmodified, and return a non-zero value
        // indicating that this multipriority queue was empty.  The behavior is
        // undefined unless 'item' is non-null.  Note that a non-zero value can
        // be returned only if 'blockFlag' is 'false'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(MultipriorityQueue,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit MultipriorityQueue(bslma::Allocator *basicAllocator = 0);
    explicit MultipriorityQueue(int               numPriorities,
                                bslma::Allocator *basicAllocator = 0);
        // Create a multipriority queue.  Optionally specify 'numPriorities',
        // the number of distinct priorities supported by the multipriority
        // queue.  If 'numPriorities' is not specified, the
        // (implementation-imposed maximum) number 32 is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '1 <= numPriorities <= 32'
        // (if specified).

    ~MultipriorityQueue();
        // Destroy this object.

    // MANIPULATORS
    int pushBack(const TYPE& item, int itemPriority);
        // Insert the value of the specified 'item' with the specified
        // 'itemPriority' into this multipriority queue before any queued items
        // having a less urgent priority (higher value) than 'itemPriority',
        // and after any items having the same or more urgent priority (lower
        // value) than 'itemPriority'.  If the multipriority queue is enabled,
        // the push succeeds and '0' is returned, otherwise the push fails, the
        // queue remains unchanged, and a nonzero value is returned.  The
        // behavior is undefined unless '0 <= itemPriority < numPriorities()'.

    void pushFrontMultipleRaw(const TYPE& item,
                              int         itemPriority,
                              int         numItems);
        // Insert the value of the specified 'item' with the specified
        // 'itemPriority' into the front of this multipriority queue the
        // specified 'numItems' times, before any queued items having the same
        // or less urgent priority (higher value) than 'itemPriority', and
        // after any items having more urgent priority (lower value) than
        // 'itemPriority'.  All 'numItems' items are pushed as a single atomic
        // action, unless the copy constructor throws while creating one of
        // them, in which case a possibly empty subset of the pushes will have
        // completed and no memory will be leaked.  'Raw' means that the push
        // will succeed even if the multipriority queue is disabled.  The
        // behavior is undefined unless '0 <= itemPriority < numPriorities()'.
        // Note that this method is targeted at specific uses by the class
        // 'bdlmt::MultipriorityThreadPool'.

    void pushBackMultipleRaw(const TYPE& item, int itemPriority, int numItems);
        // Insert the value of the specified 'item' with the specified
        // 'itemPriority' onto the back of this multipriority queue before any
        // queued items having a less urgent priority (higher value) than
        // 'itemPriority', and after any items having the same or more urgent
        // priority (lower value) than 'itemPriority'.  All of the specified
        // 'numItems' items are pushed as a single atomic action, unless the
        // copy constructor for one of them throws an exception, in which case
        // a possibly empty subset of the pushes will have completed and no
        // memory will be leaked.  'Raw' means that the push will succeed even
        // if the multipriority queue is disabled.  Note that this method is
        // targeted for specific use by the class
        // 'bdlmt::MultipriorityThreadPool'.  The behavior is undefined unless
        // '0 <= itemPriority < numPriorities()'.

    void popFront(TYPE *item, int *itemPriority = 0);
        // Remove the least-recently added item having the most urgent priority
        // (lowest value) from this multi-priority queue and load its value
        // into the specified 'item'.  If this queue is empty, this method
        // blocks the calling thread until an item becomes available.  If the
        // optionally specified 'itemPriority' is non-null, load the priority
        // of the popped item into 'itemPriority'.  The behavior is undefined
        // unless 'item' is non-null.  Note this is unaffected by the enabled /
        // disabled state of the queue.

    int tryPopFront(TYPE *item, int *itemPriority = 0);
        // Attempt to remove (immediately) the least-recently added item having
        // the most urgent priority (lowest value) from this multi-priority
        // queue.  On success, load the value of the popped item into the
        // specified 'item'; if the optionally specified 'itemPriority' is
        // non-null, load the priority of the popped item into 'itemPriority';
        // and return 0.  Otherwise, leave 'item' and 'itemPriority'
        // unmodified, and return a non-zero value indicating that this queue
        // was empty.  The behavior is undefined unless 'item' is non-null.
        // Note this is unaffected by the enabled / disabled state of the
        // queue.

    void removeAll();
        // Remove and destroy all items from this multi-priority queue.

    void enable();
        // Enable pushes to this multipriority queue.  This method has no
        // effect unless the queue was disabled.

    void disable();
        // Disable pushes to this multipriority queue.  This method has no
        // effect unless the queue was enabled.

    // ACCESSORS
    int numPriorities() const;
        // Return the number of distinct priorities (indicated at construction)
        // that are supported by this multi-priority queue.

    int length() const;
        // Return the total number of items in this multi-priority queue.

    bool isEmpty() const;
        // Return 'true' if there are no items in this multi-priority queue,
        // and 'false' otherwise.

    bool isEnabled() const;
        // Return 'true' if this multipriority queue is enable and 'false'
        // otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                 // -----------------------------------------
                 // local class MultipriorityQueue_Node<TYPE>
                 // -----------------------------------------

// CREATORS
template <class TYPE>
inline
MultipriorityQueue_Node<TYPE>::MultipriorityQueue_Node(
                                       const TYPE&              item,
                                       MultipriorityQueue_Node *next,
                                       bslma::Allocator        *basicAllocator)
: d_item(item, basicAllocator)
, d_next_p(next)
{
}

template <class TYPE>
inline
MultipriorityQueue_Node<TYPE>::~MultipriorityQueue_Node()
{
}

// MANIPULATORS
template <class TYPE>
inline
MultipriorityQueue_Node<TYPE>*& MultipriorityQueue_Node<TYPE>::nextPtr()
{
    return d_next_p;
}

// ACCESSORS
template <class TYPE>
inline
const MultipriorityQueue_Node<TYPE> *
                            MultipriorityQueue_Node<TYPE>::nextPtr() const
{
    return d_next_p;
}

template <class TYPE>
inline
const TYPE& MultipriorityQueue_Node<TYPE>::item() const
{
    return d_item.object();
}

                       // ------------------------------
                       // class MultipriorityQueue<TYPE>
                       // ------------------------------

// PRIVATE MANIPULATORS
template <class TYPE>
int MultipriorityQueue<TYPE>::tryPopFrontImpl(TYPE *item,
                                              int  *itemPriority,
                                              bool  blockFlag)
{
    enum { e_SUCCESS = 0, e_FAILURE = -1 };

    Node *condemned;
    int priority;

    BSLS_ASSERT(item);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (0 == d_length) {
            // Note that if we get a spurious signal, we will check the
            // 'blockFlag' unnecessarily, but that will typically be a rare
            // occurrence.  This arrangement minimizes the time taken in the
            // case where '0 != d_length', which will typically be a frequent
            // occurrence.

            if (blockFlag) {
                d_notEmptyCondition.wait(&d_mutex);
            }
            else {
                return e_FAILURE;                                     // RETURN
            }
        }

        priority = bdlb::BitUtil::numTrailingUnsetBits(
                                               (bsl::uint32_t)d_notEmptyFlags);
        BSLS_ASSERT(priority < k_MAX_NUM_PRIORITIES);
            // verifies there is at least one priority bit set.  Note that
            // 'numTrailingUnsetBits' cannot return a negative value.

        Node *& head = d_heads[priority];
        condemned = head;

        *item = condemned->item();  // might throw

        head = head->nextPtr();
        if (0 == head) {
            // The last item with this priority was just popped.

            BSLS_ASSERT(d_tails[priority] == condemned);
            d_notEmptyFlags &= ~(1 << priority);
        }

        --d_length;
    }  // release mutex

    if (itemPriority) {
        *itemPriority = priority;
    }

    condemned->~Node();
    d_pool.deallocate(condemned);

    return e_SUCCESS;
}

// CREATORS
template <class TYPE>
MultipriorityQueue<TYPE>::MultipriorityQueue(bslma::Allocator *basicAllocator)
: d_heads((typename NodePtrVector::size_type)k_DEFAULT_NUM_PRIORITIES, 0,
          basicAllocator)
, d_tails((typename NodePtrVector::size_type)k_DEFAULT_NUM_PRIORITIES, 0,
          basicAllocator)
, d_notEmptyFlags(0)
, d_pool(sizeof(Node), bslma::Default::allocator(basicAllocator))
, d_length(0)
, d_enabledFlag(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
MultipriorityQueue<TYPE>::MultipriorityQueue(int               numPriorities,
                                             bslma::Allocator *basicAllocator)
: d_heads((typename NodePtrVector::size_type)numPriorities, 0, basicAllocator)
, d_tails((typename NodePtrVector::size_type)numPriorities, 0, basicAllocator)
, d_notEmptyFlags(0)
, d_pool(sizeof(Node), bslma::Default::allocator(basicAllocator))
, d_length(0)
, d_enabledFlag(true)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1                       <= numPriorities);
    BSLS_ASSERT(k_MAX_NUM_PRIORITIES >= numPriorities);
}

template <class TYPE>
MultipriorityQueue<TYPE>::~MultipriorityQueue()
{
    removeAll();

    typename NodePtrVector::iterator it;
    typename NodePtrVector::iterator endIt;

    for (it = d_heads.begin(), endIt = d_heads.end(); endIt != it; ++it) {
        BSLS_ASSERT(!*it);
    }

    // Tails do not get set to null by 'removeAll', so are indeterminate.

    BSLS_ASSERT(isEmpty());
    BSLS_ASSERT(0 == d_notEmptyFlags);
}

// MANIPULATORS
template <class TYPE>
int MultipriorityQueue<TYPE>::pushBack(const TYPE& item, int itemPriority)
{
    enum { e_SUCCESS = 0, e_FAILURE = -1 };

    BSLS_ASSERT((unsigned)itemPriority < d_heads.size());

    // Allocate and copy construct.  Note we are doing this work outside the
    // mutex, which is advantageous in that no one is waiting on us, but it has
    // the disadvantage that we haven't checked whether this multipriority
    // queue is disabled, in which case we'll throw the new node away.
    //     Note the queue being disabled is not the usual case.  Note a race
    // condition occurs if we check d_enabledFlag outside the mutex.
    Node *newNode = (Node *)d_pool.allocate();
    bslma::DeallocatorProctor<bdlma::ConcurrentPool> deleter(newNode, &d_pool);

    bslalg::ScalarPrimitives::construct(newNode,
                                        item,
                                        (Node *)0,
                                        d_allocator_p);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (!d_enabledFlag) {
            return e_FAILURE;                                         // RETURN
        }

        deleter.release();

        const int mask = 1 << itemPriority;
        if (d_notEmptyFlags & mask) {
            d_tails[itemPriority]->nextPtr() = newNode;
        }
        else {
            d_heads[itemPriority] = newNode;
            d_notEmptyFlags |= mask;
        }
        d_tails[itemPriority] = newNode;

        ++d_length;
    }  // release mutex

    d_notEmptyCondition.signal();

    return e_SUCCESS;
}

template <class TYPE>
void MultipriorityQueue<TYPE>::pushFrontMultipleRaw(const TYPE& item,
                                                    int         itemPriority,
                                                    int         numItems)
{
    BSLS_ASSERT((unsigned)itemPriority < d_heads.size());

    const int mask = 1 << itemPriority;

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        for (int i = 0; i < numItems; ++i) {
            Node *newNode = (Node *)d_pool.allocate();
            bslma::DeallocatorProctor<bdlma::ConcurrentPool> deleter(newNode,
                                                                     &d_pool);

            bslalg::ScalarPrimitives::construct(newNode,
                                                item,
                                                (Node *)0,
                                                d_allocator_p);

            deleter.release();

            Node *& head = d_heads[itemPriority];
            if (!head) {
                d_tails[itemPriority] = newNode;
                d_notEmptyFlags |= mask;
            }
            newNode->nextPtr() = head;
            head = newNode;

            ++d_length;

            d_notEmptyCondition.signal();
        } // for numItems i
    }  // release mutex
}

template <class TYPE>
void MultipriorityQueue<TYPE>::pushBackMultipleRaw(const TYPE& item,
                                                   int         itemPriority,
                                                   int         numItems)
{
    BSLS_ASSERT((unsigned)itemPriority < d_heads.size());

    const int mask = 1 << itemPriority;

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        for (int i = 0; i < numItems; ++i) {
            Node *newNode = (Node *)d_pool.allocate();
            bslma::DeallocatorProctor<bdlma::ConcurrentPool> deleter(newNode,
                                                                     &d_pool);

            bslalg::ScalarPrimitives::construct(newNode,
                                                item,
                                                (Node *)0,
                                                d_allocator_p);

            deleter.release();

            if (d_notEmptyFlags & mask) {
                d_tails[itemPriority]->nextPtr() = newNode;
            }
            else {
                d_heads[itemPriority] = newNode;
                d_notEmptyFlags |= mask;
            }
            d_tails[itemPriority] = newNode;

            ++d_length;

            d_notEmptyCondition.signal();
        } // for numItems i
    }  // release mutex
}

template <class TYPE>
inline
int MultipriorityQueue<TYPE>::tryPopFront(TYPE *item, int *itemPriority)
{
    return tryPopFrontImpl(item, itemPriority, false);
}

template <class TYPE>
inline
void MultipriorityQueue<TYPE>::popFront(TYPE *item, int *itemPriority)
{
    tryPopFrontImpl(item, itemPriority, true);
}

template <class TYPE>
void MultipriorityQueue<TYPE>::removeAll()
{
    Node *condemnedList = 0;

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        while (d_notEmptyFlags) {
            const int priority = bdlb::BitUtil::numTrailingUnsetBits(
                                  static_cast<bsl::uint32_t>(d_notEmptyFlags));

            Node *& head = d_heads[priority];
            BSLS_ASSERT(head);

            d_tails[priority]->nextPtr() = condemnedList;
            condemnedList = head;

            head = 0;

            d_notEmptyFlags &= ~(1 << priority);
        }

        BSLS_ASSERT(0 == d_notEmptyFlags);

        d_length = 0;
    }  // release mutex

    Node *node = condemnedList;
    while (node) {
        Node *condemnedNode = node;
        node = node->nextPtr();

        condemnedNode->~Node();
        d_pool.deallocate(condemnedNode);
    }
}

template <class TYPE>
inline
void MultipriorityQueue<TYPE>::enable()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_enabledFlag = true;
}

template <class TYPE>
inline
void MultipriorityQueue<TYPE>::disable()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    d_enabledFlag = false;
}

// ACCESSORS
template <class TYPE>
inline
int MultipriorityQueue<TYPE>::numPriorities() const
{
    return static_cast<int>(d_heads.size());
}

template <class TYPE>
inline
int MultipriorityQueue<TYPE>::length() const
{
    return d_length;
}

template <class TYPE>
inline
bool MultipriorityQueue<TYPE>::isEmpty() const
{
    return 0 == d_length;
}

template <class TYPE>
inline
bool MultipriorityQueue<TYPE>::isEnabled() const
{
    return d_enabledFlag;
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
