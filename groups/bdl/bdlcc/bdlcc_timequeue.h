// bdlcc_timequeue.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_TIMEQUEUE
#define INCLUDED_BDLCC_TIMEQUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient queue for time events.
//
//@CLASSES:
//     bdlcc::TimeQueue: Templatized time event queue
// bdlcc::TimeQueueItem: ('struct') Templatized item in the time event queue
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a thread-safe and efficient templatized
// time queue.  The queue stores an ordered list of time values and associated
// 'DATA'.  Each item added to the queue is assigned a unique identifier that
// can be used to efficiently remove the item making this queue suitable for
// conditions where time items are added and removed very frequently.
//
// Class 'bdlcc::TimeQueue<DATA>' provides a public interface which is similar
// in structure and intent to 'bdlcc::Queue<DATA>', with the exception that
// each item stored in the 'bdlcc::TimeQueue' is of type
// 'bdlcc::TimeQueueItem<DATA>'.  This structure contains a single
// 'bsls::TimeInterval' value along with the 'DATA' value.
//
// Idiomatic usage of 'bdlcc::TimeQueue' includes the member function 'popLE',
// which finds all items on the queue whose 'bsls::TimeInterval' are less than
// a specified value, and transfers those items to a provided vector of items.
// Through the use of this member function, clients can retrieve and process
// multiple elements that have expired, that is, whose 'bsls::TimeInterval'
// values are in the past.
//
// 'bdlcc::TimeQueue' also makes use of an opaque data type
// 'bdlcc::TimeQueue::Handle' which serves to identify an individual element on
// the Time Queue.  A value of type 'Handle' is returned from the 'add' member
// function, and can then be used to remove or modify the corresponding element
// on the queue.  In this way, the 'update' member function can update the time
// value for a specific 'bdlcc::TimeQueueItem' without removing it from the
// queue.
//
///'bdlcc::TimeQueue::Handle' Uniqueness, Reuse and 'numIndexBits'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdlcc::TimeQueue::Handle' is an alias for a 32-bit 'int' type.  A handle
// consists of two parts, the "index section" and the "iteration section".  The
// index section, which is the low-order 'numIndexBits' (which defaults to
// 'numIndexBits == 17'), uniquely identifies the node.  Once a node is added,
// it never ceases to exist - it may be freed, but it will be kept on a free
// list to be eventually recycled, and the same index section will always
// identify that node.  The iteration section, the high-order
// '32 - numIndexBits', is changed every time a node is freed, so that an
// out-of-date handle can be identified as out-of-date.  But since the
// iteration section has only a finite number of bits, if a node is freed and
// re-added enough times, old handle values will eventually be reused.
//
// Up to '2 ** numIndexBits - 1' nodes can exist in a given time queue.  A
// given handle won't be reused for a node until that node has been freed and
// reused '2 ** (32 - numIndexBits) - 1' times.
//
// 'numIndexBits' is an optional parameter to the time queue constructors.  If
// unspecified, it has a value of 17.  The behavior is undefined unless the
// specified 'numIndexBits' is in the range '8 <= numIndexBits <= 24'.
//
///Thread Safety
///- - - - - - -
// It is safe to access or modify two distinct 'bdlcc::TimeQueue' objects
// simultaneously, each from a separate thread.  It is safe to access or modify
// a single 'bdlcc::TimeQueue' object simultaneously from two or more separate
// threads.
//
// It is safe to enqueue objects in a 'bdlcc::TimeQueue' object whose
// destructor may access or even modify the same 'bdlcc::TimeQueue' object.
// However, there is no guarantee regarding the safety of enqueuing objects
// whose copy constructors or assignment operators may modify or even merely
// access the same 'bdlcc::TimeQueue' object (except 'length').  Such attempts
// generally lead to a deadlock.
//
///Usage
///-----
// The following shows a typical usage of the 'bdlcc::TimeQueue' class,
// implementing a simple threaded server 'my_Server' that manages individual
// Connections ('my_Connection') on behalf of multiple Sessions ('my_Session').
// Each Connection is timed, such that input requests on that Connection will
// "time out" after a user-specified time interval.  When a specific Connection
// times out, that Connection is removed from the 'bdlcc::TimeQueue' and the
// corresponding 'my_Session' is informed.
//
// In this simplified example, class 'my_Session' will terminate when its
// Connection times out.  A more sophisticated implementation of 'my_Session'
// would attempt recovery, perhaps by closing and reopening the physical
// Connection.
//
///Forward Declarations
/// - - - - - - - - - -
// Class 'my_Server' will spawn two service threads to monitor connections for
// available data and to manage time-outs, respectively.  Two forward-declared
// "C" functions are invoked as the threads are spawned.  The signature of each
// function follows the "C" standard "'void *'" interface for spawning threads.
// Each function will be called on a new thread when the 'start' method is
// invoked for a given 'my_Server' object.  Each function then delegates
// processing for the thread back to the 'my_Server' object that spawned it.
//..
//  extern "C" {
//
//  void *my_connectionMonitorThreadEntry(void *server);
//
//  void *my_timerMonitorThreadEntry(void *server);
//
//  }
//..
//
///'struct my_Connection'
/// - - - - - - - - - - -
// The 'my_Connection' structure is used by 'my_Server' to manage a single
// physical connection on behalf of a 'my_Session'.
//..
//  class my_Session;
//  struct my_Connection {
//      int         d_timerId;
//      my_Session *d_session_p;
//  };
//..
//
///Protocol Classes
/// - - - - - - - -
// Protocol class 'my_Session' provides a pure abstract protocol to manage a
// single "session" to be associated with a specific connection on a server.
//..
//  class my_Session {
//      // Pure protocol class to process a data buffer of arbitrary size.
//      // Concrete implementations in the "real world" would typically manage
//      // an external connection like a socket.
//
//    public:
//      my_Session();
//      virtual int processData(void *data, int length) = 0;
//      virtual int handleTimeout(my_Connection *connection) = 0;
//      virtual ~my_Session();
//  };
//..
// The constructor and destructor do nothing:
//..
//  my_Session::my_Session()
//  {
//  }
//
//  my_Session::~my_Session()
//  {
//  }
//..
// Protocol class 'my_Server' provides a partial implementation of a simple
// server that supports and monitors an arbitrary number of connections and
// handles incoming data for those connections.  Clients must provide a
// concrete implementation that binds connections to concrete 'my_Session'
// objects and monitors all open connections for incoming requests.  The
// concrete implementation calls 'my_Server::newConnection()' when a new
// connections is required, and implements the virtual function
// 'monitorConnections' to monitor all open connections.
//..
//  class my_Server {
//      // Simple server supporting multiple Connections.
//
//      bsl::vector<my_Connection*>      d_connections;
//      bdlcc::TimeQueue<my_Connection*> d_timeQueue;
//      int                              d_ioTimeout;
//      bslmt::Mutex                     d_timerMonitorMutex;
//      bslmt::Condition                 d_timerChangedCond;
//      bslmt::ThreadUtil::Handle        d_connectionThreadHandle;
//      bslmt::ThreadUtil::Handle        d_timerThreadHandle;
//      volatile bool                    d_done;
//
//    protected:
//      void newConnection(my_Connection *connection);
//          // Add the specified 'connection' to the current 'my_Server',
//          // setting the new timeout value to the current time plus the
//          // timeout value provided at construction of this 'my_Server'
//          // instance.  If the added connection is the new "top" of the
//          // queue, signal that the minimum time on the queue has changed.
//          // Upon seeing this signal, the TimerMonitor thread will wake up
//          // and look for expired timers.
//          //
//          // Behavior is undefined if 'connection' has already been added to
//          // any 'my_Server' and has not been removed via member function
//          // 'closeConnection'.
//
//      void removeConnection(my_Connection *connection);
//          // Remove the specified 'connection' from the current 'my_Server',
//          // so that it will no longer be monitored for available data.
//
//      virtual void closeConnection(my_Connection *connection)=0;
//          // Provide a mechanism for a concrete implementation to close a
//          // specified 'connection'.
//
//      void dataAvailable(my_Connection *connection,
//                         void          *buffer_p,
//                         int            length);
//          // Receive in the specified 'buffer_p' a pointer to a data buffer
//          // of the specified 'length' bytes, and pass this to the specified
//          // 'connection' to be processed.  Behavior is undefined if
//          // 'connection' is not currently added to this 'my_Server' object,
//          // or if 'length' <= 0.
//
//    protected:
//      virtual void monitorConnections()=0;
//          // Monitor all connections in the current 'my_Server'.  When data
//          // becomes available for a given connection, pass the data to that
//          // connection for processing.
//
//      void monitorTimers();
//          // Monitor all timers in the current 'my_Server', and handle each
//          // timer as it expires.
//
//      friend void *my_connectionMonitorThreadEntry(void *server);
//      friend void *my_timerMonitorThreadEntry(void *server);
//
//    private:
//      // Not implemented:
//      my_Server(const my_Server&);
//
//    public:
//      // CREATORS
//      explicit
//      my_Server(int ioTimeout, bslma::Allocator *basicAllocator = 0);
//          // Construct a 'my_Server' object with a timeout value of the
//          // specified 'ioTimeout' seconds.  Use the optionally specified
//          // 'basicAllocator' for all memory allocation for data members of
//          // 'my_Server'.
//
//      virtual ~my_Server();
//
//      // MANIPULATORS
//      int start();
//          // Begin monitoring timers and connections.
//  };
//..
// The constructor is simple: it initializes the internal 'bdlcc::TimeQueue'
// and sets the I/O timeout value.  The virtual destructor does nothing.
//..
//  my_Server::my_Server(int ioTimeout, bslma::Allocator *basicAllocator)
//  : d_timeQueue(basicAllocator)
//  , d_ioTimeout(ioTimeout)
//  , d_connectionThreadHandle(bslmt::ThreadUtil::invalidHandle())
//  , d_timerThreadHandle(bslmt::ThreadUtil::invalidHandle())
//  {
//  }
//
//  my_Server::~my_Server()
//  {
//      d_done = true;
//      d_timerChangedCond.broadcast();
//      if (bslmt::ThreadUtil::invalidHandle() != d_connectionThreadHandle) {
//          bslmt::ThreadUtil::join(d_connectionThreadHandle);
//      }
//      if (bslmt::ThreadUtil::invalidHandle()!= d_timerThreadHandle) {
//          bslmt::ThreadUtil::join(d_timerThreadHandle);
//      }
//  }
//..
// Member function 'newConnection' adds the 'connection' to the current set of
// connections to be monitored.  This is done in two steps.  First, the
// 'connection' is added to the internal array, and then a timer is set for the
// 'connection' by creating a corresponding entry in the internal
// 'bdlcc::TimeQueue'.
//..
//  void my_Server::newConnection(my_Connection *connection)
//  {
//      d_connections.push_back(connection);
//      int isNewTop = 0;
//      connection->d_timerId = d_timeQueue.add(bdlt::CurrentTime::now() +
//                                                                 d_ioTimeout,
//                                              connection,
//                                              &isNewTop);
//      if (isNewTop) {
//          bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
//          d_timerChangedCond.signal();
//      }
//  }
//..
// Member function 'monitorConnections', provided by the concrete
// implementation class, can use the internal array to determine the set of
// connections to be monitored.
//
// Member function 'removeConnection' removes the 'connection' from the current
// set of connections to be monitored.  This is done in two steps, in reversed
// order from 'newConnection'.  First, the 'connection' is removed from the
// internal 'bdlcc::TimeQueue', and then the 'connection' is removed from the
// internal array.
//
// The concrete implementation class must provide an implementation of virtual
// function 'closeConnection'; this implementation must call 'removeConnection'
// when the actual connection is to be removed from the 'my_Server' object.
//
// Function 'closeConnection' is in turn called by function 'monitorTimers',
// which manages the overall timer monitor thread.  Because 'monitorTimers'
// takes responsibility for notifying other threads when the queue status
// changes, function 'removeConnection' does not address these concerns.
//..
//  void my_Server::removeConnection(my_Connection *connection)
//  {
//      // Remove from d_timeQueue
//      d_timeQueue.remove(connection->d_timerId);
//      // Remove from d_connections
//      bsl::vector<my_Connection*>::iterator begin = d_connections.begin(),
//          end = d_connections.end(),
//          it = begin;
//      for (; it != end; ++it) {
//          if (connection == *it) {
//              d_connections.erase(it);
//          }
//      }
//  }
//..
// The 'dataAvailable' function will be called when data becomes available for
// a specific connection.  It removes the connection from the timer queue while
// the connection is busy, processes the available data, and returns the
// connection to the queue with a new time value.
//..
//  void my_Server::dataAvailable(my_Connection *connection,
//                                void          *buffer_p,
//                                int            length)
//  {
//      if (connection->d_timerId) {
//          if (d_timeQueue.remove(connection->d_timerId))  return;   // RETURN
//          connection->d_timerId = 0;
//      }
//      connection->d_session_p->processData(buffer_p, length);
//
//      int isNewTop = 0;
//
//      connection->d_timerId = d_timeQueue.add(bdlt::CurrentTime::now() +
//                                                                 d_ioTimeout,
//                                              connection,
//                                              &isNewTop);
//      if (isNewTop) {
//          bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
//          d_timerChangedCond.signal();
//      }
//  }
//..
// Function 'monitorTimers' manages the timer monitor thread; it is called when
// the thread is spawned, and checks repeatedly for expired timers; after each
// check, it does a timed wait based upon the minimum time value seen in the
// queue after all expired timers have been removed.
//..
//  void my_Server::monitorTimers()
//  {
//      while (!d_done) {
//          bsl::vector<bdlcc::TimeQueueItem<my_Connection*> > expiredTimers;
//          {
//              bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
//              bsls::TimeInterval minTime;
//              int newLength;
//
//              d_timeQueue.popLE(bdlt::CurrentTime::now(),
//                                &expiredTimers,
//                                &newLength,
//                                &minTime );
//
//              if (!expiredTimers.size()) {
//                  if (newLength) {
//                      // no expired timers, but unexpired timers remain.
//                      d_timerChangedCond.timedWait(&d_timerMonitorMutex,
//                                                   minTime);
//                  }
//                  else {
//                      // no expired timers, and timer queue is empty.
//                      d_timerChangedCond.wait(&d_timerMonitorMutex);
//                  }
//                  continue;
//              }
//          }
//
//          int length = static_cast<int>(expiredTimers.size());
//          if (length) {
//              bdlcc::TimeQueueItem<my_Connection*> *data =
//                                                      &expiredTimers.front();
//              for (int i = 0; i < length; ++i) {
//                  closeConnection(data[i].data());
//              }
//          }
//      }
//  }
//..
// Function 'start' spawns two separate threads.  The first thread will monitor
// connections and handle any data received on them.  The second monitors the
// internal timer queue and removes connections that have timed out.  Function
// 'start' calls 'bslmt::ThreadUtil::create', which expects a function pointer
// to a function with the standard "C" callback signature
// 'void *fn(void *data)'.  This non-member function will call back into the
// 'my_Server' object immediately.
//..
//  int my_Server::start()
//  {
//      bslmt::ThreadAttributes attr;
//
//      if (bslmt::ThreadUtil::create(&d_connectionThreadHandle, attr,
//                                   &my_connectionMonitorThreadEntry,
//                                   this)) {
//          return -1;                                                // RETURN
//      }
//
//      if (bslmt::ThreadUtil::create(&d_timerThreadHandle, attr,
//                                   &my_timerMonitorThreadEntry,
//                                   this)) {
//          return -1;                                                // RETURN
//      }
//      return 0;
//  }
//..
// Finally, we are now in a position to implement the two thread dispatchers:
//..
//  extern "C" {
//
//  void *my_connectionMonitorThreadEntry(void *server)
//  {
//      ((my_Server*)server)->monitorConnections();
//      return server;
//  }
//
//  void *my_timerMonitorThreadEntry(void *server)
//  {
//      ((my_Server*)server)->monitorTimers();
//      return server;
//  }
//
//  }
//..
// In order to test our server, we provide two concrete implementations of a
// test session and of a test server as follows.
//..
//  // myTestSession.h             -*-C++-*-
//
//  class my_TestSession : public my_Session {
//      // Concrete implementation of my_Session, providing simple test
//      // semantics In particular, implement the virtual function
//      // processData() to record all incoming data for the controlling
//      // connection, and virtual function handleTimeout() for handling
//      // timeouts.
//
//      int d_verbose;
//
//    public:
//      // CREATORS
//      explicit
//      my_TestSession(int verbose) : my_Session(), d_verbose(verbose) { }
//
//      // MANIPULATORS
//      virtual int handleTimeout(my_Connection *connection)
//      {
//          // Do something to handle timeout.
//          if (d_verbose) {
//              bsl::cout << bdlt::CurrentTime::utc() << ": ";
//              bsl::cout << "Connection " << connection << "timed out.\n";
//          }
//          return 0;
//      }
//
//      virtual int processData(void *data, int length)
//      {
//          // Do something with the data...
//          if (d_verbose) {
//              bsl::cout << bdlt::CurrentTime::utc() << ": ";
//              bsl::cout << "Processing data at address " << data
//                        << " and length " << length << ".\n";
//          }
//          return 0;
//      }
//  };
//
//  // myTestSession.h             -*-C++-*-
//
//  class my_TestServer :  public my_Server {
//      // Concrete implementation of my_Server, providing connection logic.
//
//      int d_verbose;
//
//    protected:
//      virtual void closeConnection(my_Connection *connection);
//          // Close the specified external 'connection' and call
//          // 'removeConnection' when done.
//
//      virtual void monitorConnections();
//          // Monitor all connections in the current 'my_Server'.  When data
//          // becomes available for a given connection, pass the data to that
//          // connection for processing.
//
//    private:
//      // Not implemented:
//      my_TestServer(const my_TestServer&);
//
//    public:
//      // CREATORS
//      explicit
//      my_TestServer(int               ioTimeout,
//                    int               verbose = 0,
//                    bslma::Allocator *basicAllocator = 0)
//      : my_Server(ioTimeout, basicAllocator)
//      , d_verbose(verbose)
//      {
//      }
//
//      virtual ~my_TestServer();
//  };
//
//  // myTestSession.cpp             -*-C++-*-
//
//  my_TestServer::~my_TestServer()
//  {
//  }
//
//  void my_TestServer::closeConnection(my_Connection *connection)
//  {
//      if (d_verbose) {
//          bsl::cout << bdlt::CurrentTime::utc() << ": ";
//          bsl::cout << "Closing connection " << connection << bsl::endl;
//      }
//      delete connection;
//  }
//
//  void my_TestServer::monitorConnections()
//  {
//      my_Session *session = new my_TestSession(d_verbose);
//
//      // Simulate connection monitor logic...
//      my_Connection *connection1 = new my_Connection;
//      connection1->d_session_p = session;
//      newConnection(connection1);
//      if (d_verbose) {
//          bsl::cout << bdlt::CurrentTime::utc() << ": ";
//          bsl::cout << "Opening connection " << connection1 << endl;
//      }
//
//      my_Connection *connection2 = new my_Connection;
//      connection2->d_session_p = session;
//      newConnection(connection2);
//      if (d_verbose) {
//          bsl::cout << bdlt::CurrentTime::utc() << ": ";
//          bsl::cout << "Opening connection " << connection2 << endl;
//      }
//
//      bslmt::ThreadUtil::sleep(bsls::TimeInterval(2)); // 2s
//
//      // Simulate transmission...
//      const int  length = 1024;
//      const char*buffer[length];
//      if (d_verbose) {
//          bsl::cout << bdlt::CurrentTime::utc() << ": ";
//          bsl::cout << "Connection " << connection1
//                    << " receives " << length << " bytes " << endl;
//      }
//      dataAvailable(connection1, buffer, length);
//
//      // Wait for timeout to occur, otherwise session gets destroyed from
//      // stack too early.
//
//      bslmt::ThreadUtil::sleep(bsls::TimeInterval(8)); // 8s
//  }
//..
// The program that would exercise this test server would simply consist of:
//..
//  int usageExample(int verbose)
//  {
//      my_TestServer mX(5, verbose); // timeout for connections: 5s
//      mX.start();
//
//      // Wait sufficiently long to observe all events.
//      bslmt::ThreadUtil::sleep(bsls::TimeInterval(10)); // 10s
//
//      return 0;
//  }
//..
// The output of this program would look something as follows:
//..
//  17:10:35.000: Opening connection 0x00161880
//  17:10:35.000: Opening connection 0x001618b0
//  17:10:37.000: Connection 0x00161880 receives 1024 bytes
//  17:10:37.000: Processing data at address 0xfeefaf04 and length 1024.
//  17:10:40.000: Closing connection 0x001618b0
//  17:10:42.000: Closing connection 0x00161880
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLMA_POOL
#include <bdlma_pool.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlcc {template <class DATA>

class TimeQueueItem;

                              // ===============
                              // class TimeQueue
                              // ===============
template <class DATA>
class TimeQueue {
    // This parameterized class provides a public interface which is similar in
    // structure and intent to 'Queue<DATA>', with the exception that each item
    // stored in the 'TimeQueue' has an associated time value.  Items are
    // retrieved or exchanged by proxy of a 'TimeQueueItem<DATA>', and are
    // referred to by an opaque data type 'TimeQueue::Handle' which serves to
    // identify an individual element on the Time Queue.  Idiomatic usage of
    // 'TimeQueue' includes the member function 'popLE', which finds all items
    // on the queue whose 'bsls::TimeInterval' are less than a specified value
    // and transfers those items to a provided vector of items, and the member
    // function 'update', which can update the time value for a specific
    // 'TimeQueueItem' without removing it from the queue.

    // TYPES
    enum {
        k_NUM_INDEX_BITS_MIN     = 8,
        k_NUM_INDEX_BITS_MAX     = 24,
        k_NUM_INDEX_BITS_DEFAULT = 17
    };

  public:
    // TYPES
    typedef int Handle;
        // 'Handle' defines an alias for uniquely identifying a valid node in
        // the time queue.  Handles are returned when nodes are added to the
        // time queue, and must be supplied to the 'update' and 'remove'
        // methods to identify existing nodes.  When a node is removed, the
        // handle value becomes invalid, though invalidated handle values are
        // eventually reused.  See the component-level documentation for more
        // details.

    class Key {
        // This type is a wrapper around a void pointer that will be supplied
        // and used by clients to uniquely identify an item in the queue.

        // PRIVATE DATA MEMBERS
        const void *d_key;

      public:
        // CREATORS
        explicit Key(const void *key)
        : d_key(key)
            // Create a 'Key' object having the specified 'key' value.
        {}

        explicit Key(int key)
        : d_key(reinterpret_cast<const void*>(key))
            // Create a 'Key' object having the specified 'key' value cast to a
            // 'void *'.
        {}

        ~Key()
            // Destroy this 'Key' object.
        {}

        // ACCESSORS
        bool operator==(const Key& rhs) const
            // Return 'true' if this object has the same value as the specified
            // 'rhs' object, and 'false' otherwise.
        {
            return d_key == rhs.d_key;
        }

        bool operator!=(const Key& rhs) const
            // Return 'true' if this object does not have the same value as the
            // specified 'rhs' object, and 'false' otherwise.
        {
            return d_key != rhs.d_key;
        }
    };

  private:

    // PRIVATE TYPES
    struct Node {
        // This queue is implemented internally as a map of time values, each
        // entry in the map storing a doubly-linked circular list of items
        // having the same time value.  This struct provides the node in the
        // list.

        int                       d_index;
        bsls::TimeInterval         d_time;
        Key                       d_key;
        Node                     *d_prev_p;
        Node                     *d_next_p;
        bsls::ObjectBuffer<DATA>  d_data;

        // CREATORS
        Node()
        : d_index(0)
        , d_key(0)
        , d_prev_p(0)
        , d_next_p(0)
            // Create a 'Node' having a time value of 0.
        {
        }

        explicit
        Node(const bsls::TimeInterval& time)
        : d_index(0)
        , d_time(time)
        , d_key(0)
        , d_prev_p(0)
        , d_next_p(0)
            // Create a 'Node' having the specified 'time' value.
        {
        }
    };

    typedef bsl::map<bsls::TimeInterval, Node*> NodeMap;
        // Internal typedef for the time index map.

    typedef typename NodeMap::iterator         MapIter;
        // Internal typedefs for the iterator used to navigate the time index.

    // PRIVATE DATA MEMBERS
    const int                d_indexMask;
    const int                d_indexIterationMask;
    const int                d_indexIterationInc;

    mutable bslmt::Mutex      d_mutex;          // used for synchronizing
                                                // access
                                               // to this queue

    bsl::vector<Node*>       d_nodeArray;      // array of nodes in this queue

    bsls::AtomicPointer<Node> d_nextFreeNode_p; // pointer to the next free
                                                // node
                                               // in this queue (the free list
                                               // is singly linked only, using
                                               // d_next_p)

    NodeMap                  d_map;            // list of time values in
                                               // increasing time order

    bsls::AtomicInt           d_length;         // number of items currently in
                                               // this queue (not necessarily
                                               // equal to d_map.size())

    bslma::Allocator        *d_allocator_p;    // allocator (held, not owned)

    // PRIVATE MANIPULATORS
    void freeNode(Node *node);
        // Prepare the specified 'node' for being reused on the free list by
        // incrementing the iteration count.  Set 'd_prev_p' field to 0.

    void putFreeNode(Node *node);
        // Destroy the data located at the specified 'node' and reattach this
        // 'node' to the front of the free list starting at 'd_nextFreeNode_p',
        // making 'node' the new 'd_nextFreeNode_p'.  Note that the caller must
        // not have acquired the lock to this queue.

    void putFreeNodeList(Node *begin);
        // Destroy the 'DATA' of every node in the singly-linked list starting
        // at the specified 'begin' node and ending with a null pointer, and
        // reattach these nodes to the front of the free list starting at
        // 'd_nextFreeNode_p', making 'begin' the new 'd_nextFreeNode_p' and
        // calling 'freeNode'.  Note that the caller must not have acquired the
        // lock to this queue.

  private:
    // NOT IMPLEMENTED
    TimeQueue(const TimeQueue&);
    TimeQueue& operator=(const TimeQueue&);

  public:
    // CREATORS
    explicit TimeQueue(bslma::Allocator *basicAllocator = 0);
    explicit TimeQueue(int numIndexBits, bslma::Allocator *basicAllocator = 0);
        // Create an empty time queue.  Optionally specify 'numIndexBits' to
        // configure the number of index bits used by this object.  If
        // 'numIndexBits' is not specified a default value of 17 is used.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '8 <= numIndexBits <= 24'.
        // See the component-level documentation for more information regarding
        // 'numIndexBits'.

    explicit TimeQueue(bool              poolTimerMemory,
                       bslma::Allocator *basicAllocator = 0);
    TimeQueue(int               numIndexBits,
              bool              poolTimerMemory,
              bslma::Allocator *basicAllocator = 0);
        // [!DEPRECATED!] Use the other constructor overloads instead.  Note
        // that the specified 'poolTimerMemory' argument controlled whether
        // additional memory used by an internal 'bsl::map' was pooled.  When
        // 'bsl::map' was modified to pool its own nodes, this option became
        // irrelevant and is now ignored.

    ~TimeQueue();
        // Destroy this time queue.

    // MANIPULATORS
    Handle add(const bsls::TimeInterval&  time,
               const DATA&                data,
               int                       *isNewTop = 0,
               int                       *newLength = 0);
    Handle add(const bsls::TimeInterval&  time,
               const DATA&                data,
               const Key&                 key,
               int                       *isNewTop = 0,
               int                       *newLength = 0);
        // Add a new item to this queue having the specified 'time' value, and
        // associated 'data'.  Optionally use the specified 'key' to uniquely
        // identify the item in subsequent calls to 'remove' and 'update'.
        // Optionally load into the optionally specified 'isNewTop' a non-zero
        // value if the item is now the lowest item in this queue, and a 0
        // value otherwise.  If specified, load into the optionally specified
        // 'newLength', the new number of items in this queue.  Return a value
        // that may be used to identify the newly added item in future calls to
        // time queue on success, and
        // -1 if the maximum queue length has been reached.

    Handle add(const TimeQueueItem<DATA>&  item,
               int                        *isNewTop = 0,
               int                        *newLength = 0);
        // Add the value of the specified 'item' to this queue.  Optionally
        // load into the optionally specified 'isNewTop' a non-zero value if
        // the replaces is now the lowest element in this queue, and a 0 value
        // otherwise.  If specified, load into the optionally specified
        // 'newLength', the new number of elements in this queue.  Return a
        // value that may be used to identify the newly added element in future
        // calls to time queue.

    int popFront(TimeQueueItem<DATA> *buffer = 0,
                 int                 *newLength = 0,
                 bsls::TimeInterval  *newMinTime = 0);
        // Atomically remove the top item from this queue, and optionally load
        // into the optionally specified 'buffer' the time and associated data
        // of the item removed.  Optionally load into the optionally specified
        // 'newLength', the number of items remaining in the queue.  Optionally
        // load into the optionally specified 'newMinTime' the new lowest time
        // in this queue.  Return 0 on success, and a non-zero value if there
        // are no items in the queue.  Note that if 'DATA' follows the 'bdema'
        // allocator model, the allocator of the 'buffer' is used to supply
        // memory.

    void popLE(const bsls::TimeInterval&          time,
               bsl::vector<TimeQueueItem<DATA> > *buffer = 0,
               int                               *newLength = 0,
               bsls::TimeInterval                *newMinTime = 0);
        // Remove from this queue all the items that have a time value less
        // than or equal to the specified 'time', and optionally append into
        // the optionally specified 'buffer' a list of the removed items,
        // ordered by their corresponding time values (top item first).
        // Optionally load into the optionally specified 'newLength' the number
        // of items remaining in this queue, and into the optionally specified
        // 'newMinTime' the lowest remaining time value in this queue.  Note
        // that 'newMinTime' is only loaded if there are items remaining in the
        // time queue; therefore, 'newLength' should be specified and examined
        // to determine whether items remain, and 'newMinTime' used only when
        // 'newLength' > 0.  Also note that if 'DATA' follows the 'bdema'
        // allocator model, the allocator of the 'buffer' vector is used to
        // supply memory for the items appended to the 'buffer'.

    void popLE(const bsls::TimeInterval&          time,
               int                                maxTimers,
               bsl::vector<TimeQueueItem<DATA> > *buffer = 0,
               int                               *newLength = 0,
               bsls::TimeInterval                *newMinTime = 0);
        // Remove from this queue up to the specified 'maxTimers' number of
        // items that have a time value less than or equal to the specified
        // 'time', and optionally append into the optionally specified 'buffer'
        // a list of the removed items, ordered by their corresponding time
        // values (top item first).  Optionally load into the optionally
        // specified 'newLength' the number of items remaining in this queue,
        // and into the optionally specified 'newMinTime' the lowest remaining
        // time value in this queue.  The behavior is undefined unless
        // 'maxTimers' >= 0.  Note that 'newMinTime' is only loaded if there
        // are items remaining in the time queue; therefore, 'newLength' should
        // be specified and examined to determine whether items remain, and
        // 'newMinTime' used only when 'newLength' > 0.  Also note that if
        // 'DATA' follows the 'bdema' allocator model, the allocator of the
        // 'buffer' vector is used to supply memory.  Note finally that all the
        // items appended into 'buffer' have a time value less than or equal to
        // the elements remaining in this queue.

    int remove(Handle               handle,
               int                 *newLength = 0,
               bsls::TimeInterval  *newMinTime = 0,
               TimeQueueItem<DATA> *item = 0);
    int remove(Handle               handle,
               const Key&           key,
               int                 *newLength = 0,
               bsls::TimeInterval  *newMinTime = 0,
               TimeQueueItem<DATA> *item = 0);
        // Remove from this queue the item having the specified 'handle', and
        // optionally load into the optionally specified 'item' the time and
        // data values of the recently removed item.  Optionally use the
        // specified 'key' to uniquely identify the item.  If specified, load
        // into the optionally specified 'newMinTime', the resulting lowest
        // time value remaining in the queue.  Return 0 on success, and a
        // non-zero value if no item with the 'handle' exists in the queue.
        // Note that if 'DATA' follows the 'bdema' allocator model, the
        // allocator of the 'item' instance is used to supply memory.

    void removeAll(bsl::vector<TimeQueueItem<DATA> > *buffer = 0);
        // Optionally load all the items in this queue to the optionally
        // specified 'buffer', and remove all the items in this queue.  Note
        // that the allocator of the 'buffer' vector is used to supply memory.

    int update(Handle                     handle,
               const bsls::TimeInterval&  newTime,
               int                       *isNewTop = 0);
    int update(Handle                     handle,
               const Key&                 key,
               const bsls::TimeInterval&  newTime,
               int                       *isNewTop = 0);
        // Update the time value of the item having the specified 'handle' to
        // the specified 'newTime' and optionally load into the optionally
        // specified 'isNewTop' a non-zero value if the modified item is now
        // the lowest time value in the time queue or zero otherwise.  Return 0
        // on success, and a non-zero value if there is currently no item
        // having the 'handle' registered with this time queue.

    // ACCESSORS
    int length() const;
        // Return a "snapshot" of the current number of items in this queue.

    bool isRegisteredHandle(Handle handle) const;
    bool isRegisteredHandle(Handle handle, const Key& key) const;
        // Return 'true' if an item having specified 'handle' is currently
        // registered with this time queue and false otherwise.

    int minTime(bsls::TimeInterval *buffer) const;
        // Load into the specified 'buffer', the time value of the lowest time
        // in this queue.  Return 0 on success, and a non-zero value if this
        // queue is empty.
};

                            // ====================
                            // struct TimeQueueItem
                            // ====================

template <class DATA>
class TimeQueueItem {
    // This parameterized structure holds a time, data and associated handle.
    // This structure is used in the interface of 'TimeQueue<DATA>' to provide
    // thread-safe access to individual elements on the queue.  Note that
    // 'DATA' must be default-constructible.

  public:
    // PUBLIC TYPES
    typedef typename TimeQueue<DATA>::Handle Handle;
    typedef typename TimeQueue<DATA>::Key    Key;

    BSLALG_DECLARE_NESTED_TRAITS(TimeQueueItem,
                                 bslalg::TypeTraitUsesBslmaAllocator);

  private:
    bsls::TimeInterval             d_time;    // Time value
    DATA                          d_data;    // Associated data value
    Handle                        d_handle;  // Associated handle
    Key                           d_key;     // Associated key

  public:
    // CREATORS
    explicit
    TimeQueueItem(bslma::Allocator *basicAllocator = 0);
        // Create an empty time queue item.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // zero, then use the currently installed default allocator.

    TimeQueueItem(const bsls::TimeInterval&  time,
                  const DATA&                data,
                  Handle                     handle,
                  bslma::Allocator          *basicAllocator = 0);
        // Create time queue item holding a copy of the specified 'data', with
        // the specified associated 'time' and 'handle' information.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is zero, then use the currently installed default
        // allocator.

    TimeQueueItem(const bsls::TimeInterval&  time,
                  const DATA&                data,
                  Handle                     handle,
                  const Key&                 key,
                  bslma::Allocator          *basicAllocator = 0);
        // Create time queue item holding a copy of the specified 'data', with
        // the specified associated 'time', 'handle', and 'key' information.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is zero, then use the currently installed default
        // allocator.

    TimeQueueItem(const TimeQueueItem<DATA>&  original,
                  bslma::Allocator           *basicAllocator = 0);
        // Create a copy of the specified 'original' time queue item.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is zero, then use the currently installed default
        // allocator.

    // MANIPULATORS
    bsls::TimeInterval& time();
        // Return the modifiable time value associated with this item.

    DATA& data();
        // Return the modifiable data instance associated with this item.

    Handle& handle()
        // Return the modifiable handle value associated with this item.
    {
        // this definition was moved into the class declaration to work around
        // a Visual Studio .NET 2003 bug.

        return d_handle;
    }

    Key& key();
        // Return the modifiable key value associated with this item.

    // ACCESSORS
    const bsls::TimeInterval& time() const;
        // Return the non-modifiable time value associated with this item.

    const DATA& data() const;
        // Return the non-modifiable data associated with this item.

    Handle handle() const
        // Return the non-modifiable handle value associated with this item.
    {
        // this definition was moved into the class declaration to work around
        // a Visual Studio .NET 2003 bug.

        return d_handle;
    }

    const Key& key() const;
        // Return the non-modifiable key value associated with this item.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                // ---------
                                // TimeQueue
                                // ---------

// PRIVATE MANIPULATORS
template <class DATA>
inline
void TimeQueue<DATA>::freeNode(Node *node)
{
    node->d_index = ((node->d_index + d_indexIterationInc) &
                         d_indexIterationMask) | (node->d_index & d_indexMask);

    if (!(node->d_index & d_indexIterationMask)) {
        node->d_index += d_indexIterationInc;
    }
    node->d_prev_p = 0;
}

template <class DATA>
void TimeQueue<DATA>::putFreeNode(Node *node)
{
    node->d_data.object().~DATA();

    Node *nextFreeNode = d_nextFreeNode_p;
    node->d_next_p = nextFreeNode;
    while (nextFreeNode != d_nextFreeNode_p.testAndSwap(nextFreeNode, node)) {
        nextFreeNode = d_nextFreeNode_p;
        node->d_next_p = nextFreeNode;
    }
}

template <class DATA>
void TimeQueue<DATA>::putFreeNodeList(Node *begin)
{
    if (begin) {
        begin->d_data.object().~DATA();

        Node *end = begin;
        while (end->d_next_p) {
            end = end->d_next_p;
            end->d_data.object().~DATA();
        }

        Node *nextFreeNode = d_nextFreeNode_p;
        end->d_next_p = nextFreeNode;

        while (nextFreeNode !=
                           d_nextFreeNode_p.testAndSwap(nextFreeNode, begin)) {
            nextFreeNode = d_nextFreeNode_p;
            end->d_next_p = nextFreeNode;
        }
    }
    return;
}

// CREATORS
template <class DATA>
TimeQueue<DATA>::TimeQueue(bslma::Allocator *basicAllocator)
: d_indexMask((1 << k_NUM_INDEX_BITS_DEFAULT) - 1)
, d_indexIterationMask(~(int)d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class DATA>
TimeQueue<DATA>::TimeQueue(bool              poolTimerMemory,
                           bslma::Allocator *basicAllocator)
: d_indexMask((1 << k_NUM_INDEX_BITS_DEFAULT) - 1)
, d_indexIterationMask(~(int)d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // The 'poolTimerMemory' option has been deprecated (see method
    // documentation).

    (void)poolTimerMemory;
}

template <class DATA>
TimeQueue<DATA>::TimeQueue(int numIndexBits, bslma::Allocator *basicAllocator)
: d_indexMask((1 << numIndexBits) - 1)
, d_indexIterationMask(~d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(k_NUM_INDEX_BITS_MIN <= numIndexBits
             && k_NUM_INDEX_BITS_MAX >= numIndexBits);
}

template <class DATA>
TimeQueue<DATA>::TimeQueue(int               numIndexBits,
                           bool              poolTimerMemory,
                           bslma::Allocator *basicAllocator)
: d_indexMask((1 << numIndexBits) - 1)
, d_indexIterationMask(~d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(k_NUM_INDEX_BITS_MIN <= numIndexBits
             && k_NUM_INDEX_BITS_MAX >= numIndexBits);

    // The 'poolTimerMemory' option has been deprecated (see method
    // documentation).

    (void)poolTimerMemory;

}

template <class DATA>
TimeQueue<DATA>::~TimeQueue()
{
    removeAll();
    if (!d_nodeArray.empty()) {
        Node **data = &d_nodeArray.front();
        const int numNodes = static_cast<int>(d_nodeArray.size());
        for (int i = 0; i < numNodes; ++i) {
            d_allocator_p->deleteObjectRaw(data[i]);
        }
    }
}

// MANIPULATORS
template <class DATA>
inline
typename TimeQueue<DATA>:: Handle TimeQueue<DATA>::add(
                                          const bsls::TimeInterval&  time,
                                          const DATA&                data,
                                          int                       *isNewTop,
                                          int                       *newLength)
{
    return add(time, data, Key(0), isNewTop, newLength);
}

template <class DATA>
typename TimeQueue<DATA>:: Handle TimeQueue<DATA>::add(
                                          const bsls::TimeInterval&  time,
                                          const DATA&                data,
                                          const Key&                 key,
                                          int                       *isNewTop,
                                          int                       *newLength)

{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    Node *node;
    if (d_nextFreeNode_p) {
        // All allocation of nodes goes through this routine, which is guarded
        // by the mutex.  So no other thread will remove anything from the free
        // list while this code is executing.  However, other threads may add
        // to the free list.

        node = d_nextFreeNode_p;
        Node *next = node->d_next_p;
        while (node != d_nextFreeNode_p.testAndSwap(node, next)) {
            node = d_nextFreeNode_p;
            next = node->d_next_p;
        }
    }
    else {
        // The number of nodes cannot grow to a size larger than the range of
        // available indices.

        if ((int)d_nodeArray.size() >= d_indexMask - 1) {
            return -1;                                                // RETURN
        }

        node = new (*d_allocator_p) Node;
        d_nodeArray.push_back(node);
        node->d_index =
                    static_cast<int>(d_nodeArray.size()) | d_indexIterationInc;
    }
    node->d_time = time;
    node->d_key  = key;
    bslalg::ScalarPrimitives::copyConstruct(&node->d_data.object(),
                                            data,
                                            d_allocator_p);

    {
        MapIter it = d_map.find(time);

        if (d_map.end() == it) {
            node->d_prev_p = node;
            node->d_next_p = node;
            d_map[time] = node;
        }
        else {
            node->d_prev_p = it->second->d_prev_p;
            it->second->d_prev_p->d_next_p = node;
            node->d_next_p = it->second;
            it->second->d_prev_p = node;
        }
    }

    ++d_length;
    if (isNewTop) {
        *isNewTop = d_map.begin()->second == node && node->d_prev_p == node;
    }

    if (newLength) {
        *newLength = d_length;
    }

    BSLS_ASSERT(-1 != node->d_index);
    return node->d_index;
}

template <class DATA>
inline
typename TimeQueue<DATA>::Handle TimeQueue<DATA>::add(
                                         const TimeQueueItem<DATA>&  item,
                                         int                        *isNewTop,
                                         int                        *newLength)
{
    return add(item.time(), item.data(), item.key(), isNewTop, newLength);
}

template <class DATA>
int TimeQueue<DATA>::popFront(TimeQueueItem<DATA> *buffer,
                              int                 *newLength,
                              bsls::TimeInterval  *newMinTime)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    MapIter it = d_map.begin();

    if (d_map.end() == it) {
        return 1;                                                     // RETURN
    }
    Node *node = it->second;

    if (buffer) {
        buffer->time()   = node->d_time;
        buffer->data()   = node->d_data.object();
        buffer->handle() = node->d_index;
        buffer->key()    = node->d_key;
    }
    if (node->d_next_p != node) {
        node->d_prev_p->d_next_p = node->d_next_p;
        node->d_next_p->d_prev_p = node->d_prev_p;
        if (it->second == node) {
            it->second = node->d_next_p;
        }
    }
    else {
        d_map.erase(it);
    }

    freeNode(node);
    --d_length;

    if (d_length && newMinTime && !d_map.empty()) {
        *newMinTime = d_map.begin()->first;
    }

    if (newLength) {
        *newLength = d_length;
    }

    lock.release()->unlock();

    putFreeNode(node);
    return 0;
}

template <class DATA>
void TimeQueue<DATA>::popLE(const bsls::TimeInterval&          time,
                            bsl::vector<TimeQueueItem<DATA> > *buffer,
                            int                               *newLength,
                            bsls::TimeInterval                *newMinTime)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it && it->first <= time) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node = first;

        do {
            if (buffer) {
                buffer->push_back(TimeQueueItem<DATA>(
                                                         it->first,
                                                         node->d_data.object(),
                                                         node->d_index,
                                                         node->d_key,
                                                         d_allocator_p));
            }
            freeNode(node);
            node = node->d_next_p;
            --d_length;
        } while (node != first);

        last->d_next_p = begin;
        begin = first;

        MapIter condemned = it;
        ++it;
        d_map.erase(condemned);
    }

    if (newLength) {
        *newLength = d_length;
    }
    if (d_map.end() != it && newMinTime) {
        *newMinTime = it->first;
    }

    lock.release()->unlock();
    putFreeNodeList(begin);
}

template <class DATA>
void TimeQueue<DATA>::popLE(const bsls::TimeInterval&          time,
                            int                                maxTimers,
                            bsl::vector<TimeQueueItem<DATA> > *buffer,
                            int                               *newLength,
                            bsls::TimeInterval                *newMinTime)
{
    BSLS_ASSERT(0 <= maxTimers);

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it && it->first <= time && 0 < maxTimers) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node  = first;
        Node *prevNode  = first->d_prev_p;

        do {
            if (buffer) {
                buffer->push_back(TimeQueueItem<DATA>(
                                                         it->first,
                                                         node->d_data.object(),
                                                         node->d_index,
                                                         node->d_key,
                                                         d_allocator_p));
            }
            freeNode(node);
            prevNode = node;
            node = node->d_next_p;
            --d_length;
            --maxTimers;
        } while (0 < maxTimers && node != first);

        prevNode->d_next_p = begin;
        begin = first;

        if (node == first) {
            MapIter condemned = it;
            ++it;
            d_map.erase(condemned);
        }
        else {
            node->d_prev_p = last;
            last->d_next_p = node;
            it->second = node;
            break;
        }
    }

    if (newLength) {
        *newLength = d_length;
    }
    if (d_map.end() != it && newMinTime) {
        *newMinTime = it->first;
    }

    lock.release()->unlock();
    putFreeNodeList(begin);
}

template <class DATA>
inline
int TimeQueue<DATA>::remove(typename TimeQueue<DATA>::Handle  handle,
                            int                              *newLength,
                            bsls::TimeInterval               *newMinTime,
                            TimeQueueItem<DATA>              *item)
{
    return remove(handle, Key(0), newLength, newMinTime, item);
}

template <class DATA>
int TimeQueue<DATA>::remove(typename TimeQueue<DATA>::Handle  handle,
                            const Key&                        key,
                            int                              *newLength,
                            bsls::TimeInterval               *newMinTime,
                            TimeQueueItem<DATA>              *item)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    int index = ((int)handle & d_indexMask) - 1;
    if (index < 0 || index >= (int)d_nodeArray.size()) {
        return 1;                                                     // RETURN
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle
     || node->d_key != key
     || 0 == node->d_prev_p) {
        return 1;                                                     // RETURN
    }

    if (item) {
        item->time()   = node->d_time;
        item->data()   = node->d_data.object();
        item->handle() = node->d_index;
        item->key()    = node->d_key;
    }

    if (node->d_next_p != node) {
        node->d_prev_p->d_next_p = node->d_next_p;
        node->d_next_p->d_prev_p = node->d_prev_p;

        MapIter it = d_map.find(node->d_time);
        if (it->second == node) {
            it->second = node->d_next_p;
        }
    }
    else {
        d_map.erase(node->d_time);
    }
    freeNode(node);
    --d_length;

    if (newLength) {
        *newLength = d_length;
    }

    if (d_length && newMinTime) {
        BSLS_ASSERT(! d_map.empty());

        *newMinTime = d_map.begin()->first;
    }

    lock.release()->unlock();

    putFreeNode(node);
    return 0;
}

template <class DATA>
void TimeQueue<DATA>::removeAll(bsl::vector<TimeQueueItem<DATA> > *buffer)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node = first;

        do {
            if (buffer) {
                buffer->push_back(TimeQueueItem<DATA>(
                                                         it->first,
                                                         node->d_data.object(),
                                                         node->d_index,
                                                         node->d_key,
                                                         d_allocator_p));
            }
            freeNode(node);
            node = node->d_next_p;
            --d_length;
        } while (node != first);

        last->d_next_p = begin;
        begin = first;

        MapIter condemned = it;
        ++it;
        d_map.erase(condemned);
    }

    lock.release()->unlock();
    putFreeNodeList(begin);
}

template <class DATA>
inline
int TimeQueue<DATA>::update(typename TimeQueue<DATA>::Handle  handle,
                            const bsls::TimeInterval&         newTime,
                            int                              *isNewTop)
{
    return update(handle, Key(0), newTime, isNewTop);
}

template <class DATA>
int TimeQueue<DATA>::update(typename TimeQueue<DATA>::Handle  handle,
                            const Key&                        key,
                            const bsls::TimeInterval&         newTime,
                            int                              *isNewTop)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    int index = ((int)handle & d_indexMask) - 1;

    if (index < 0 || (unsigned) index >= d_nodeArray.size()) {
        return 1;                                                     // RETURN
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle || node->d_key != key) {
        return 1;                                                     // RETURN
    }

    if (node->d_prev_p != node) {
        node->d_prev_p->d_next_p = node->d_next_p;
        node->d_next_p->d_prev_p = node->d_prev_p;

        MapIter it = d_map.find(node->d_time);
        if (it->second == node) {
            it->second = node->d_next_p;
        }
    }
    else {
        d_map.erase(node->d_time);
    }
    node->d_time = newTime;

    MapIter it = d_map.find(newTime);

    if (d_map.end() == it) {
        node->d_prev_p = node;
        node->d_next_p = node;
        d_map[newTime] = node;
    }
    else {
        node->d_prev_p = it->second->d_prev_p;
        it->second->d_prev_p->d_next_p = node;
        node->d_next_p = it->second;
        it->second->d_prev_p = node;
    }

    if (isNewTop) {
        *isNewTop = d_map.begin()->second == node && node->d_prev_p == node;
    }
    return 0;
}

// ACCESSORS
template <class DATA>
inline
int TimeQueue<DATA>::length() const
{
    return d_length;
}

template <class DATA>
inline
bool TimeQueue<DATA>::isRegisteredHandle(
                                 typename TimeQueue<DATA>::Handle handle) const
{
    return isRegisteredHandle(handle, Key(0));
}

template <class DATA>
inline
bool TimeQueue<DATA>::isRegisteredHandle(
                                    typename TimeQueue<DATA>::Handle handle,
                                    const Key&                       key) const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    int index = (handle & d_indexMask) - 1;

    if ( 0 > index || index >= (int)d_nodeArray.size()) {
        return false;                                                 // RETURN
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle || node->d_key != key) {
        return false;                                                 // RETURN
    }

    return true;
}

template <class DATA>
inline
int TimeQueue<DATA>::minTime(bsls::TimeInterval *buffer) const
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (d_map.empty()) {
        return 1;                                                     // RETURN
    }

    *buffer = d_map.begin()->first;
    return 0;
}

                            // --------------------
                            // struct TimeQueueItem
                            // --------------------

// CREATORS
template <class DATA>
TimeQueueItem<DATA>::TimeQueueItem(bslma::Allocator *basicAllocator)
: d_key(0)
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_data);
    bslalg::ScalarPrimitives::defaultConstruct(&d_data, basicAllocator);
}

template <class DATA>
TimeQueueItem<DATA>::
TimeQueueItem(TimeQueueItem<DATA> const&  original,
              bslma::Allocator           *basicAllocator)
: d_time(original.d_time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(original.d_handle)
, d_key(original.d_key)
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_data);
    bslalg::ScalarPrimitives::copyConstruct(&d_data,
                                            original.d_data,
                                            basicAllocator);
}

template <class DATA>
TimeQueueItem<DATA>::
TimeQueueItem(const bsls::TimeInterval&  time,
              const DATA&                data,
              Handle                     handle,
              bslma::Allocator          *basicAllocator)
: d_time(time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(handle)
, d_key(0)
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_data);
    bslalg::ScalarPrimitives::copyConstruct(&d_data,
                                            data,
                                            basicAllocator);
}

template <class DATA>
TimeQueueItem<DATA>::
TimeQueueItem(const bsls::TimeInterval&  time,
              const DATA&                data,
              Handle                     handle,
              const Key&                 key,
              bslma::Allocator          *basicAllocator)
: d_time(time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(handle)
, d_key(key)
{
    bslalg::ScalarDestructionPrimitives::destroy(&d_data);
    bslalg::ScalarPrimitives::copyConstruct(&d_data,
                                            data,
                                            basicAllocator);
}

// MANIPULATORS
template <class DATA>
inline
bsls::TimeInterval& TimeQueueItem<DATA>::time()
{
    return d_time;
}

template <class DATA>
inline
DATA& TimeQueueItem<DATA>::data()
{
    return d_data;
}
}  // close package namespace

#if 0

namespace bdlcc {// this definition was moved into the class declaration

// to work around a Visual Studio .NET 2003 bug.
template <typename DATA>
inline
typename TimeQueueItem<DATA>::Handle&
TimeQueueItem<DATA>::handle()
{
    return d_handle;
}
}  // close package namespace
#endif

namespace bdlcc {

template <class DATA>
inline
typename TimeQueueItem<DATA>::Key&
TimeQueueItem<DATA>::key()
{
    return d_key;
}

// ACCESSORS
template <class DATA>
inline
const bsls::TimeInterval& TimeQueueItem<DATA>::time() const
{
    return d_time;
}

template <class DATA>
inline
const DATA& TimeQueueItem<DATA>::data() const
{
    return d_data;
}
}  // close package namespace

#if 0

namespace bdlcc {// this definition was moved into the class declaration

// to work around a Visual Studio .NET 2003 bug.
template <typename DATA>
inline
typename TimeQueueItem<DATA>::Handle
TimeQueueItem<DATA>::handle() const
{
    return d_handle;
}
}  // close package namespace
#endif

namespace bdlcc {

template <class DATA>
inline
const typename TimeQueueItem<DATA>::Key&
TimeQueueItem<DATA>::key() const
{
    return d_key;
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
