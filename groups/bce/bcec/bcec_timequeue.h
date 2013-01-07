// bcec_timequeue.h   -*-C++-*-
#ifndef INCLUDED_BCEC_TIMEQUEUE
#define INCLUDED_BCEC_TIMEQUEUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an efficient queue for time events.
//
//@CLASSES:
//     bcec_TimeQueue: Templatized time event queue
// bcec_TimeQueueItem: ('struct') Templatized item in the time event queue
//
//@SEE_ALSO:
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a thread-safe and efficient templatized
// time queue.  The queue stores an ordered list of time values and associated
// 'DATA'.  Each item added to the queue is assigned a unique identifier that
// can be used to efficiently remove the item making this queue suitable for
// conditions where time items are added and removed very frequently.
//
// Class 'bcec_TimeQueue<DATA>' provides a public interface which is similar in
// structure and intent to 'bcec_queue<DATA>', with the exception that each
// item stored in the 'bcec_TimeQueue' is of type 'bcec_TimeQueueItem<DATA>'.
// This structure contains a single 'bdet_TimeInterval' value along with the
// 'DATA' value.
//
// Idiomatic usage of 'bcec_TimeQueue' includes the member function 'popLE',
// which finds all items on the queue whose 'bdet_TimeInterval' are less than a
// specified value, and transfers those items to a provided vector of items.
// Through the use of this member function, clients can retrieve and process
// multiple elements that have expired, that is, whose 'bdet_TimeInterval'
// values are in the past.
//
// 'bcec_TimeQueue' also makes use of an opaque data type
// 'bcec_TimeQueue::Handle' which serves to identify an individual element on
// the Time Queue.  A value of type 'Handle' is returned from the 'add'
// member function, and can then be used to remove or modify the corresponding
// element on the queue.  In this way, the 'update' member function can update
// the time value for a specific 'bcec_TimeQueueItem' without removing it from
// the queue.
//
///'poolTimerMemory' flag
///----------------------
// Clients can provide a memory hint to 'bcec_TimeQueue' asking it to pool the
// memory that it uses for the timers in it's internal data structures.  Note
// that it is inefficient to provide a generic pool allocator as
// 'bcec_TimeQueue' contains multiple data structures having differing memory
// requirements.  The performance benefit from pooling the timer memory varies
// across platforms but in the normal use case where many timers are registered
// and fired multiple times the processing time of adding and removing timers
// improves by 2 - 10 %.  The total number of memory allocations also drop
// significantly if 'poolTimerMemory' is 'true', however, it may result in
// higher peak memory depending on the usage pattern of the queue.
//
///Thread Safety
///-------------
// It is safe to access or modify two distinct 'bcec_TimeQueue' objects
// simultaneously, each from a separate thread.  It is safe to access or modify
// a single 'bcec_TimeQueue' object simultaneously from two or more separate
// threads.
//
// It is safe to enqueue objects in a 'bcec_TimeQueue' object whose destructor
// may access or even modify the same 'bcec_TimeQueue' object.  However, there
// is no guarantee regarding the safety of enqueuing objects whose copy
// constructors or assignment operators may modify or even merely access the
// same 'bcec_TimeQueue' object (except 'length').  Such attempts generally
// lead to a deadlock.
//
///Usage
///-----
// The following shows a typical usage of the 'bcec_TimeQueue' class,
// implementing a simple threaded server 'my_Server' that manages individual
// Connections ('my_Connection') on behalf of multiple Sessions ('my_Session').
// Each Connection is timed, such that input requests on that Connection will
// "time out" after a user-specified time interval.  When a specific Connection
// times out, that Connection is removed from the 'bcec_TimeQueue' and the
// corresponding 'my_Session' is informed.
//
// In this simplified example, class 'my_Session' will terminate when its
// Connection times out.  A more sophisticated implementation of 'my_Session'
// would attempt recovery, perhaps by closing and reopening the physical
// Connection.
//
///'numIndexBits'
///-------------
// The 'bcec_TimeQueue' component is constructed using a user-supplied
// 'numIndexBits'.  The number of bits used to store the index controls the
// range of available indices.
//
///Forward declarations:
///- - - - - - - - - - -
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
///struct 'my_Connection'
///- - - - - - - - - - -
// The 'my_Connection' structure is used by 'my_Server' to manage a single
// physical connection on behalf of a 'my_Session'.
//..
//  class my_Session;
//  struct my_Connection {
//      bcec_TimeQueue<my_Connection*>::Handle d_timerId;
//      my_Session *d_session_p;
//  };
//..
//
///Protocol classes
///- - - - - - - -
// Protocol class 'my_Session' provides a pure abstract protocol to manage a
// single "session" to be associated with a specific connection on a server.
//..
//  class my_Session {
//      // Pure protocol class to process a data buffer of arbitrary size.
//      // Concrete implementations in the "real world" would typically
//      // manage an external connection like a socket.
//
//    public:
//      inline my_Session();
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
//    private:
//      bsl::vector<my_Connection*>     d_connections;
//      bcec_TimeQueue<my_Connection*>  d_timeQueue;
//      int                             d_ioTimeout;
//      bcemt_Mutex                     d_timerMonitorMutex;
//      bcemt_Condition                 d_timerChangedCond;
//      bcemt_ThreadUtil::Handle        d_timerThreadHandle;
//      bcemt_ThreadUtil::Handle        d_connectionThreadHandle;
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
//          // Behavior is undefined if 'connection' has already been added
//          // to any 'my_Server' and has not been removed via member
//          // function 'closeConnection'.
//
//      void removeConnection(my_Connection *connection);
//          // Remove the specified 'connection' from the current 'my_Server',
//          // so that it will no longer be monitored for available data.
//
//      virtual void closeConnection(my_Connection *connection)=0;
//          // Provide a mechanism for a concrete implementation to close a
//          // connection.
//
//      void dataAvailable(my_Connection *connection,
//                         void          *buffer_p,
//                         int           length);
//          // Receive in 'buffer_p' a pointer to a data buffer of 'length'
//          // bytes, and pass this to 'connection' to be processed.  Behavior
//          // is undefined if 'connection' is not currently added to this
//          // 'my_Server' object, or if 'length' <= 0.
//
//    protected:
//      virtual void monitorConnections()=0;
//          // Monitor all connections in the current 'my_Server'.  When data
//          // becomes available for a given connection, pass the data to
//          // that connection for processing.
//
//      void monitorTimers();
//          // Monitor all timers in the current 'my_Server', and handle each
//          // timer as it expires.
//
//      friend void *my_connectionMonitorThreadEntry(void *server);
//      friend void *my_timerMonitorThreadEntry(void *server);
//
//    public:
//      // CREATORS
//      my_Server(int ioTimeout, bslma_Allocator *basicAllocator=0);
//          // Construct a 'my_Server' object with a timeout value of
//          // 'ioTimeout' seconds.  Use the specified 'basicAllocator' for all
//          // memory allocation for data members of 'my_Server'.
//
//      virtual ~my_Server();
//
//      // MANIPULATORS
//      int start();
//          // Begin monitoring timers and connections.
//  };
//..
// The constructor is simple: it initializes the internal 'bcec_TimeQueue' and
// sets the I/O timeout value.  The virtual destructor does nothing.
//..
//  my_Server::my_Server(int ioTimeout, bslma_Allocator *basicAllocator)
//  : d_timeQueue(basicAllocator)
//  , d_ioTimeout(ioTimeout)
//  {
//  }
//
//  my_Server::~my_Server()
//  {
//  }
//..
// Member function 'newConnection' adds the 'connection' to the current
// set of connections to be monitored.  This is done in two steps.
// First, the 'connection' is added to the internal array, and then a
// timer is set for the 'connection' by creating a corresponding entry
// in the internal 'bcec_TimeQueue'.
//..
//  void my_Server::newConnection(my_Connection *connection)
//  {
//      d_connections.push_back(connection);
//      int isNewTop = 0;
//      connection->d_timerId = d_timeQueue.add(bdetu_SystemTime::now() +
//                                                   d_ioTimeout,
//                                              connection,
//                                              &isNewTop);
//      if (isNewTop) {
//          bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
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
// internal 'bcec_TimeQueue', and then the 'connection' is removed from the
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
//                                void *data,
//                                int length)
//  {
//      if (connection->d_timerId) {
//          if (d_timeQueue.remove(connection->d_timerId))  return;
//          connection->d_timerId = 0;
//      }
//      connection->d_session_p->processData(data, length);
//
//      int isNewTop = 0;
//
//      connection->d_timerId = d_timeQueue.add(bdetu_SystemTime::now() +
//                                                  d_ioTimeout,
//                                              connection,
//                                              &isNewTop);
//      if (isNewTop) {
//          bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
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
//      while(1) {
//          bsl::vector<bcec_TimeQueueItem<my_Connection*> > expiredTimers;
//          {
//              bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
//              bdet_TimeInterval minTime;
//              int newLength;
//
//              d_timeQueue.popLE(bdetu_SystemTime::now(),
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
//          int length = expiredTimers.size();
//          if (length) {
//              bcec_TimeQueueItem<my_Connection*> *data =
//                                                   &expiredTimers.front();
//              for( int i=0; i < length; ++i) {
//                  closeConnection(data[i].data());
//              }
//          }
//      }
//  }
//..
// Function 'start' spawns two separate threads.  The first thread will monitor
// connections and handle any data received on them.  The second monitors the
// internal timer queue and removes connections that have timed out.  Function
// 'start' calls 'bcemt_ThreadUtil::create, which expects a function pointer to
// a function with the standard "C" callback signature 'void *fn(void *data)'.
// This non-member function will call back into the 'my_Server' object
// immediately.
//..
//  int my_Server::start()
//  {
//      bcemt_Attribute attr;
//
//      if (bcemt_ThreadUtil::create(&d_connectionThreadHandle, attr,
//                                   &my_connectionMonitorThreadEntry,
//                                   this)) {
//          return -1;
//      }
//
//      if (bcemt_ThreadUtil::create(&d_timerThreadHandle, attr,
//                                   &my_timerMonitorThreadEntry,
//                                   this)) {
//          return -1;
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
// In order to test our server, we provide two concrete implementations of
// a test session and of a test server as follows.
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
//    public:
//      // CREATORS
//      my_TestSession() : my_Session() { }
//
//      // MANIPULATORS
//      virtual int handleTimeout(my_Connection *connection) {
//          // Do something to handle timeout.
//          bsl::cout << bdetu_Time::currentTime() << ": ";
//          bsl::cout << "Connection " << connection << "timed out.\n";
//          return 0;
//      }
//
//      virtual int processData(void *data, int length) {
//          // Do something with the data...
//          bsl::cout << bdetu_Time::currentTime() << ": ";
//          bsl::cout << "Processing data at address " << data
//                    << " and length " << length << ".\n";
//          return 0;
//      }
//  };
//
//  // myTestSession.h             -*-C++-*-
//
//  class my_TestServer :  public my_Server {
//      // Concrete implementation of my_Server, providing connection logic.
//
//    protected:
//      virtual void closeConnection(my_Connection *connection);
//          // Close external connection and call 'removeConnection' when
//          // done.
//
//      virtual void monitorConnections();
//          // Monitor all connections in the current 'my_Server'.  When data
//          // becomes available for a given connection, pass the data to
//          // that connection for processing.
//
//    public:
//      // CREATORS
//      my_TestServer(int ioTimeout, bslma_Allocator *basicAllocator=0)
//      : my_Server(ioTimeout,basicAllocator)
//      {
//      };
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
//      bsl::cout << bdetu_Time::currentTime() << ": ";
//      bsl::cout << "Closing connection " << connection << bsl::endl;
//      delete connection;
//  }
//
//  void my_TestServer::monitorConnections()
//  {
//      my_Session *session = new my_TestSession();
//
//      // Simulate connection monitor logic...
//      my_Connection *connection1 = new my_Connection;
//      connection1->d_session_p = session;
//      newConnection(connection1);
//      bsl::cout << bdetu_Time::currentTime() << ": ";
//      bsl::cout << "Opening connection " << connection1 << endl;
//
//      my_Connection *connection2 = new my_Connection;
//      connection2->d_session_p = session;
//      newConnection(connection2);
//      bsl::cout << bdetu_Time::currentTime() << ": ";
//      bsl::cout << "Opening connection " << connection2 << endl;
//
//      bcemt_ThreadUtil::sleep(bdet_TimeInterval(2)); // 2s
//
//      // Simulate transmission...
//      const int  length = 1024;
//      const char*buffer[length];
//      bsl::cout << bdetu_Time::currentTime() << ": ";
//      bsl::cout << "Connection " << connection1
//                << " receives " << length << " bytes " << endl;
//      dataAvailable(connection1, buffer, length);
//
//      // Wait for timeout to occur, otherwise session get destroyed from
//      // stack too early.
//
//      bcemt_ThreadUtil::sleep(bdet_TimeInterval(8)); // 8s
//  }
//..
// The program that would exercise this test server would simply consist of:
//..
//  int main()
//  {
//      my_TestServer mX(5); // timeout for connections: 5s
//      mX.start();
//
//      // Wait sufficiently long to observe all events.
//      bcemt_ThreadUtil::sleep(bdet_TimeInterval(10)); // 10s
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

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCEMT_LOCKGUARD
#include <bcemt_lockguard.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR
#include <bdema_managedptr.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

template <typename DATA> class bcec_TimeQueueItem;

                             // =====================
                             // class bcec_TimeQueue
                             // =====================
template <typename DATA>
class bcec_TimeQueue {
    // This parameterized class provides a public interface which is
    // similar in structure and intent to 'bcec_queue<DATA>', with the
    // exception that each item stored in the 'bcec_TimeQueue' has an
    // associated time value.  Items are retrieved or exchanged by proxy of a
    // 'bcec_TimeQueueItem<DATA>', and are referred to by an opaque data type
    // 'bcec_TimeQueue::Handle' which serves to identify an individual element
    // on the Time Queue.  Idiomatic usage of 'bcec_TimeQueue' includes the
    // member function 'popLE', which finds all items on the queue whose
    // 'bdet_TimeInterval' are less than a specified value and transfers those
    // items to a provided vector of items, and the member function 'update',
    // which can update the time value for a specific 'bcec_TimeQueueItem'
    // without removing it from the queue.

    // TYPES
    enum {
        BCEC_NUM_INDEX_BITS_MIN     = 8,
        BCEC_NUM_INDEX_BITS_MAX     = 24,
        BCEC_NUM_INDEX_BITS_DEFAULT = 17
    };

  public:
    // TYPES
    typedef int Handle;
        // Provide an opaque "handle" for a specific item in the
        // 'bcec_TimeQueue', as a return value when the item is inserted into
        // the queue.  Clients use this value to gain direct access to the
        // corresponding element; see member functions 'add', 'update', and
        // 'remove'.

    class Key {
        // This type is a wrapper around a void pointer that will be supplied
        // and used by clients to uniquely identify an item in the queue.

        // PRIVATE DATA MEMBERS
        const void *d_key;

      public:
        // CREATORS
        explicit Key(const void *key)
        : d_key(key) { }

        explicit Key(int key)
        : d_key(reinterpret_cast<const void*>(key)) { }

        ~Key() { }

        // ACCESSORS
        bool operator==(const Key& rhs) const
        {
            return d_key == rhs.d_key;
        }

        bool operator!=(const Key& rhs) const
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

        int                      d_index;
        bdet_TimeInterval        d_time;
        Key                      d_key;
        Node                    *d_prev_p;
        Node                    *d_next_p;
        bsls_ObjectBuffer<DATA>  d_data;

        Node(const bdet_TimeInterval& time)
        : d_index(0)
        , d_time(time)
        , d_key(0)
        , d_prev_p(0)
        , d_next_p(0)
        {
        }

        Node()
        : d_index(0)
        , d_key(0)
        , d_prev_p(0)
        , d_next_p(0)
        {
        }
    };

    typedef bsl::map<bdet_TimeInterval, Node*> NodeMap;
        // Internal typedef for the time index map.

    typedef typename NodeMap::iterator         MapIter;
        // Internal typedefs for the iterator used to navigate the time index.

    // PRIVATE DATA MEMBERS
    const int                d_numIndexBits;
    const int                d_indexMask;
    const int                d_indexIterationMask;
    const int                d_indexIterationInc;

    mutable bcemt_Mutex      d_mutex;          // used for synchronizing access
                                               // to this queue

    bsl::vector<Node*>       d_nodeArray;      // array of nodes in this queue

    bces_AtomicPointer<Node> d_nextFreeNode_p; // pointer to the next free node
                                               // in this queue (the free list
                                               // is singly linked only, using
                                               // d_next_p)

    // Due to the initialization dependency between 'd_mapAllocatorPtr'
    // and 'd_map' their declaration order should always be as follows.

    bdema_ManagedPtr<bcema_PoolAllocator>
                             d_mapAllocatorPtr;// pointer to the pool
                                               // allocator supplied to
                                               // 'd_map' if the client
                                               // specifies that timers stored
                                               // in 'd_map' be pooled

    NodeMap                  d_map;            // list of time values in
                                               // increasing time order

    bces_AtomicInt           d_length;         // number of items currently in
                                               // this queue (not necessarily
                                               // equal to d_map.size())

    bslma_Allocator         *d_allocator_p;    // allocator (held, not owned)

    // PRIVATE MANIPULATORS
    void freeNode(Node *node);
        // Prepare this node for being reused on the free list by incrementing
        // the iteration count.  Set 'd_prev_p' field to 0.

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

    // NOT IMPLEMENTED
    bcec_TimeQueue(const bcec_TimeQueue&);
    bcec_TimeQueue& operator=(const bcec_TimeQueue&);

  public:
    // CREATORS
    explicit bcec_TimeQueue(bslma_Allocator *basicAllocator = 0);
    explicit bcec_TimeQueue(bool             poolTimerMemory,
                            bslma_Allocator *basicAllocator = 0);
    explicit bcec_TimeQueue(int              numIndexBits,
                            bslma_Allocator *basicAllocator = 0);
    bcec_TimeQueue(int              numIndexBits,
                   bool             poolTimerMemory,
                   bslma_Allocator *basicAllocator = 0);
        // Construct an empty time queue.  Optionally specify 'numIndexBits' to
        // configure the number of index bits used by this object.  If
        // 'numIndexBits' is not specified a default value of 17 is used.
        // Optionally specify 'poolTimerMemory' to indicate whether to pool the
        // memory used for timers.  The behavior is undefined unless
        // 'numIndexBits' is in the valid range, i.e., between 8 and 24.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  See component-level documentation for more information
        // regarding 'numIndexBits'.

    ~bcec_TimeQueue();
        // Destroy this time queue.

    // MANIPULATORS
    Handle add(const bdet_TimeInterval&  time,
               const DATA&               data,
               int                      *isNewTop  = 0,
               int                      *newLength = 0);
    Handle add(const bdet_TimeInterval&  time,
               const DATA&               data,
               const Key&                key,
               int                      *isNewTop  = 0,
               int                      *newLength = 0);
        // Add a new item to this queue having the specified 'time' value, and
        // associated 'data'.  Optionally use the specified 'key' to uniquely
        // identify the item in subsequent calls to 'remove' and 'update'.
        // Optionally load into the specified 'isNewTop' a non-zero value if
        // the item is now the lowest item in this queue, and a 0 value
        // otherwise.  If specified, load into 'newLength', the new number of
        // items in this queue.  Return a value that may be used to identify
        // the newly added item in future calls to time queue on success, and
        // -1 if the maximum queue length has been reached.

    Handle add(const bcec_TimeQueueItem<DATA>&  item,
               int                             *isNewTop  = 0,
               int                             *newLength = 0);
        // Add a new item to this queue having the specified 'time' value, and
        // associated 'data'.  Optionally load into the specified 'isNewTop'
        // a non-zero value if the replaces is now the lowest item in this
        // queue, and a 0 value otherwise.  If specified, load into
        // 'newLength', the new number of items in this queue.  Return a value
        // that may be used to identify the newly added item in future calls
        // to time queue.

    int popFront(bcec_TimeQueueItem<DATA> *buffer     = 0,
                 int                      *newLength  = 0,
                 bdet_TimeInterval        *newMinTime = 0);
        // Atomically remove the top item from this queue, and optionally load
        // into the specified 'buffer' the time and associated data of the item
        // removed.  Optionally load into the specified 'newLength', the number
        // of items remaining in the queue.  Optionally load into the specified
        // 'newMinTime' the new lowest time in this queue.  Return 0 on
        // success, and a non-zero value if there are no items in the queue.
        // Note that if 'DATA' follows the 'bdema' allocator model, the
        // allocator of the 'buffer' is used to supply memory.

    void popLE(const bdet_TimeInterval&                time,
               bsl::vector<bcec_TimeQueueItem<DATA> > *buffer     = 0,
               int                                    *newLength  = 0,
               bdet_TimeInterval                      *newMinTime = 0);
        // Remove from this queue all the items that have a time value less
        // than or equal to the specified 'time', and optionally append into
        // the specified 'buffer' a list of the removed items, ordered by their
        // corresponding time values (top item first).  Optionally load into
        // the specified 'newLength' the number of items remaining in this
        // queue, and into the specified 'newMinTime' the lowest remaining time
        // value in this queue.  Note that 'newMinTime' is only loaded if there
        // are items remaining in the time queue; therefore, 'newLength' should
        // be specified and examined to determine whether items remain, and
        // 'newMinTime' used only when 'newLength' > 0.  Also note that if
        // 'DATA' follows the 'bdema' allocator model, the allocator of the
        // 'buffer' vector is used to supply memory for the items appended to
        // the 'buffer'.

    void popLE(const bdet_TimeInterval&                time,
               int                                     maxTimers,
               bsl::vector<bcec_TimeQueueItem<DATA> > *buffer     = 0,
               int                                    *newLength  = 0,
               bdet_TimeInterval                      *newMinTime = 0);
        // Remove from this queue up to the specified 'maxTimers' number of
        // items that have a time value less than or equal to the specified
        // 'time', and optionally append into the specified 'buffer' a list of
        // the removed items, ordered by their corresponding time values (top
        // item first).  Optionally load into the specified 'newLength' the
        // number of items remaining in this queue, and into the specified
        // 'newMinTime' the lowest remaining time value in this queue.  The
        // behavior is undefined unless 'maxTimers' >= 0.  Note that
        // 'newMinTime' is only loaded if there are items remaining in the time
        // queue; therefore, 'newLength' should be specified and examined to
        // determine whether items remain, and 'newMinTime' used only when
        // 'newLength' > 0.  Also note that if 'DATA' follows the 'bdema'
        // allocator model, the allocator of the 'buffer' vector is used to
        // supply memory.  Note finally that all the items appended into
        // 'buffer' have a time value less than or equal to the elements
        // remaining in this queue.

    int remove(Handle                    handle,
               int                      *newMinLength = 0,
               bdet_TimeInterval        *newMinTime   = 0,
               bcec_TimeQueueItem<DATA> *item         = 0);
    int remove(Handle                    handle,
               const Key&                key,
               int                      *newMinLength = 0,
               bdet_TimeInterval        *newMinTime   = 0,
               bcec_TimeQueueItem<DATA> *item         = 0);
        // Remove from this queue the item having the specified 'handle', and
        // optionally load into the specified 'item' the time and data values
        // of the recently removed item.  Optionally use the specified 'key' to
        // uniquely identify the item.  If specified, load into 'newMinTime',
        // the resulting lowest time value remaining in the queue.  Return 0 on
        // success, and a non-zero value if no item with the specified 'handle'
        // exists in the queue.  Note that if 'DATA' follows the 'bdema'
        // allocator model, the allocator of the 'item' instance is used to
        // supply memory.

    void removeAll(bsl::vector<bcec_TimeQueueItem<DATA> > *buffer = 0);
        // Optionally load all the items in this queue to the specified
        // 'buffer', and remove all the items in this queue.  Note that the
        // allocator of the 'buffer' vector is used to supply memory.

    int update(Handle                    handle,
               const bdet_TimeInterval&  newTime,
               int                      *isNewTop = 0);
    int update(Handle                    handle,
               const Key&                key,
               const bdet_TimeInterval&  newTime,
               int                      *isNewTop = 0);
        // Update the time value of the item having the specified 'handle' to
        // the specified 'newTime' and optionally load into the specified
        // 'isNewTop' a non-zero value if the modified item is now the lowest
        // time value in the time queue or zero otherwise.  Return 0 on
        // success, and a non-zero value if there is currently no having the
        // specified 'handle' registered with this time queue.

    // ACCESSORS
    int length() const;
        // Return a "snapshot" of the current number of items in this queue.

    bool isRegisteredHandle(Handle handle) const;
    bool isRegisteredHandle(Handle handle, const Key& key) const;
        // Return 'true' if an item having specified 'handle' is currently
        // registered with this time queue and false otherwise.

    int minTime(bdet_TimeInterval *buffer) const;
        // Load into the specified 'buffer', the time value of the lowest time
        // in this queue.  Return 0 on success, and a non-zero value if this
        // queue is empty.
};

                             // =========================
                             // struct bcec_TimeQueueItem
                             // =========================

template <typename DATA>
class bcec_TimeQueueItem {
    // This parameterized structure holds a time, data and associated handle.
    // This structure is used in the interface of 'bcec_TimeQueue<DATA>' to
    // provide thread-safe access to individual elements on the queue.  Note
    // that 'DATA' must be default-constructible.

  public:
    // PUBLIC TYPES
    typedef typename bcec_TimeQueue<DATA>::Handle Handle;
    typedef typename bcec_TimeQueue<DATA>::Key    Key;

    BSLALG_DECLARE_NESTED_TRAITS(bcec_TimeQueueItem,
                                 bslalg_TypeTraitUsesBslmaAllocator);

  private:
    bdet_TimeInterval             d_time;    // Time value
    DATA                          d_data;    // Associated data value
    Handle                        d_handle;  // Associated handle
    Key                           d_key;     // Associated key

  public:
    // CREATORS
    bcec_TimeQueueItem(bslma_Allocator                 *basicAllocator = 0);
    bcec_TimeQueueItem(const bdet_TimeInterval&         time,
                       const DATA&                      data,
                       Handle                           handle,
                       bslma_Allocator                 *basicAllocator = 0);
    bcec_TimeQueueItem(const bdet_TimeInterval&         time,
                       const DATA&                      data,
                       Handle                           handle,
                       const Key&                       key,
                       bslma_Allocator                 *basicAllocator = 0);
        // Create time queue item holding a copy of the 'data', optionally with
        // associated 'time', 'handle' and 'key' information.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is zero, then use the currently installed default
        // allocator.

    bcec_TimeQueueItem(bcec_TimeQueueItem<DATA> const&  original,
                       bslma_Allocator                 *basicAllocator = 0);
        // Create a copy of the 'original' time queue item.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is zero, then use the currently installed default
        // allocator.

    // MANIPULATORS
    bdet_TimeInterval& time();
        // Return the modifiable time value associated with this item.

    DATA& data();
        // Return the modifiable data instance associated with this item.

    Handle& handle()
        // Return the modifiable handle value associated with this item.
    {
        // this definition was moved into the class declaration
        // to work around a Visual Studio .NET 2003 bug.
        return d_handle;
    }

    Key& key();
        // Return the modifiable key value associated with this item.

    // ACCESSORS
    const bdet_TimeInterval& time() const;
        // Return the non-modifiable time value associated with this item.

    const DATA& data() const;
        // Return the non-modifiable data associated with this item.

    Handle handle() const
        // Return the non-modifiable handle value associated with this item.
    {
        // this definition was moved into the class declaration
        // to work around a Visual Studio .NET 2003 bug.
        return d_handle;
    }

    const Key& key() const;
        // Return the non-modifiable key value associated with this item.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                              // --------------
                              // bcec_TimeQueue
                              // --------------

// PRIVATE MANIPULATORS
template <typename DATA>
inline void bcec_TimeQueue<DATA>::freeNode(Node *node)
{
    node->d_index = ((node->d_index + d_indexIterationInc) &
                    d_indexIterationMask) | (node->d_index & d_indexMask);
    if (!(node->d_index & d_indexIterationMask)) {
        node->d_index += d_indexIterationInc;
    }
    node->d_prev_p = 0;
}

template <typename DATA>
void bcec_TimeQueue<DATA>::putFreeNode(Node *node)
{
    node->d_data.object().~DATA();

    Node *nextFreeNode = d_nextFreeNode_p;
    node->d_next_p = nextFreeNode;
    while (nextFreeNode != d_nextFreeNode_p.testAndSwap(nextFreeNode, node)) {
        nextFreeNode = d_nextFreeNode_p;
        node->d_next_p = nextFreeNode;
    }
}

template <typename DATA>
void bcec_TimeQueue<DATA>::putFreeNodeList(Node *begin)
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
template <typename DATA>
bcec_TimeQueue<DATA>::bcec_TimeQueue(bslma_Allocator *basicAllocator)
: d_numIndexBits(BCEC_NUM_INDEX_BITS_DEFAULT)
, d_indexMask((1<<d_numIndexBits) - 1)
, d_indexIterationMask(~(int)d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_mapAllocatorPtr(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename DATA>
bcec_TimeQueue<DATA>::bcec_TimeQueue(bool             poolTimerMemory,
                                     bslma_Allocator *basicAllocator)
: d_numIndexBits(BCEC_NUM_INDEX_BITS_DEFAULT)
, d_indexMask((1<<d_numIndexBits) - 1)
, d_indexIterationMask(~(int)d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_mapAllocatorPtr(poolTimerMemory
                    ? new (*bslma_Default::allocator(basicAllocator))
                                            bcema_PoolAllocator(sizeof(Node),
                                                                basicAllocator)
                    : 0,
                    bslma_Default::allocator(basicAllocator))
, d_map(poolTimerMemory ? d_mapAllocatorPtr.ptr() : basicAllocator)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename DATA>
bcec_TimeQueue<DATA>::bcec_TimeQueue(int              numIndexBits,
                                     bslma_Allocator *basicAllocator)
: d_numIndexBits(numIndexBits)
, d_indexMask((1<<d_numIndexBits) - 1)
, d_indexIterationMask(~d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_mapAllocatorPtr(0)
, d_map(basicAllocator)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(BCEC_NUM_INDEX_BITS_MIN <= numIndexBits
             && BCEC_NUM_INDEX_BITS_MAX >= numIndexBits);
}

template <typename DATA>
bcec_TimeQueue<DATA>::bcec_TimeQueue(int              numIndexBits,
                                     bool             poolTimerMemory,
                                     bslma_Allocator *basicAllocator)
: d_numIndexBits(numIndexBits)
, d_indexMask((1<<d_numIndexBits) - 1)
, d_indexIterationMask(~d_indexMask)
, d_indexIterationInc(d_indexMask + 1)
, d_nodeArray(basicAllocator)
, d_nextFreeNode_p(0)
, d_mapAllocatorPtr(poolTimerMemory
                    ? new (*bslma_Default::allocator(basicAllocator))
                                            bcema_PoolAllocator(sizeof(Node),
                                                                basicAllocator)
                    : 0,
                    bslma_Default::allocator(basicAllocator))
, d_map(poolTimerMemory ? d_mapAllocatorPtr.ptr() : basicAllocator)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(BCEC_NUM_INDEX_BITS_MIN <= numIndexBits
             && BCEC_NUM_INDEX_BITS_MAX >= numIndexBits);
}

template <typename DATA>
bcec_TimeQueue<DATA>::~bcec_TimeQueue()
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
template <typename DATA>
inline typename bcec_TimeQueue<DATA>:: Handle bcec_TimeQueue<DATA>::add(
                                           const bdet_TimeInterval&  time,
                                           const DATA&               data,
                                           int                      *isNewTop,
                                           int                      *newLength)
{
    return add(time, data, Key(0), isNewTop, newLength);
}

template <typename DATA>
typename bcec_TimeQueue<DATA>:: Handle bcec_TimeQueue<DATA>::add(
                                           const bdet_TimeInterval&  time,
                                           const DATA&               data,
                                           const Key&                key,
                                           int                      *isNewTop,
                                           int                      *newLength)

{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    Node *node;
    if (d_nextFreeNode_p) {
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
            return -1;
        }

        node = new (*d_allocator_p) Node;
        d_nodeArray.push_back(node);
        node->d_index =
                    static_cast<int>(d_nodeArray.size()) | d_indexIterationInc;
    }
    node->d_time = time;
    node->d_key  = key;
    bslalg_ScalarPrimitives::copyConstruct(&node->d_data.object(),
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

template <typename DATA>
inline typename bcec_TimeQueue<DATA>::Handle bcec_TimeQueue<DATA>::add(
                                    const bcec_TimeQueueItem<DATA>&  item,
                                    int                             *isNewTop,
                                    int                             *newLength)
{
    return add(item.time(), item.data(), item.key(), isNewTop, newLength);
}

template <typename DATA>
int bcec_TimeQueue<DATA>::popFront(bcec_TimeQueueItem<DATA> *buffer,
                                   int                      *newLength,
                                   bdet_TimeInterval        *newMinTime)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    MapIter it = d_map.begin();

    if (d_map.end() == it) {
        return 1;
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

template <typename DATA>
void bcec_TimeQueue<DATA>::popLE(
        const bdet_TimeInterval&                time,
        bsl::vector<bcec_TimeQueueItem<DATA> > *buffer,
        int                                    *newLength,
        bdet_TimeInterval                      *newMinTime)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it && it->first <= time) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node = first;

        do {
            if (buffer) {
                buffer->push_back(bcec_TimeQueueItem<DATA>(
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

template <typename DATA>
void bcec_TimeQueue<DATA>::popLE(
        const bdet_TimeInterval&                time,
        int                                     maxTimers,
        bsl::vector<bcec_TimeQueueItem<DATA> > *buffer,
        int                                    *newLength,
        bdet_TimeInterval                      *newMinTime)
{
    BSLS_ASSERT(0 <= maxTimers);

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it && it->first <= time && 0 < maxTimers) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node  = first;
        Node *prevNode  = first->d_prev_p;

        do {
            if (buffer) {
                buffer->push_back(bcec_TimeQueueItem<DATA>(
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

template <typename DATA>
inline
int bcec_TimeQueue<DATA>::remove(
                             typename bcec_TimeQueue<DATA>::Handle  handle,
                             int                                   *newLength,
                             bdet_TimeInterval                     *newMinTime,
                             bcec_TimeQueueItem<DATA>              *item)
{
    return remove(handle, Key(0), newLength, newMinTime, item);
}

template <typename DATA>
int bcec_TimeQueue<DATA>::remove(
                             typename bcec_TimeQueue<DATA>::Handle  handle,
                             const Key&                             key,
                             int                                   *newLength,
                             bdet_TimeInterval                     *newMinTime,
                             bcec_TimeQueueItem<DATA>              *item)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    int index = ((int)handle & d_indexMask) - 1;
    if (index < 0 || index >= (int)d_nodeArray.size()) {
        return 1;
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle
     || node->d_key != key
     || 0 == node->d_prev_p) {
        return 1;
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

template <typename DATA>
void bcec_TimeQueue<DATA>::removeAll(
                                bsl::vector<bcec_TimeQueueItem<DATA> > *buffer)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    MapIter it = d_map.begin();

    Node *begin = 0;
    while (d_map.end() != it) {
        Node *const first = it->second;
        Node *const last  = first->d_prev_p;
        Node *node = first;

        do {
            if (buffer) {
                buffer->push_back(bcec_TimeQueueItem<DATA>(
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

template <typename DATA>
inline
int bcec_TimeQueue<DATA>::update(
                               typename bcec_TimeQueue<DATA>::Handle  handle,
                               const bdet_TimeInterval&               newTime,
                               int                                   *isNewTop)
{
    return update(handle, Key(0), newTime, isNewTop);
}

template <typename DATA>
int bcec_TimeQueue<DATA>::update(
                               typename bcec_TimeQueue<DATA>::Handle  handle,
                               const Key&                             key,
                               const bdet_TimeInterval&               newTime,
                               int                                   *isNewTop)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    int index = ((int)handle & d_indexMask) - 1;

    if (index < 0 || (unsigned) index >= d_nodeArray.size()) {
        return 1;
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle || node->d_key != key) {
        return 1;
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
template <typename DATA>
inline
int bcec_TimeQueue<DATA>::length() const
{
    return d_length;
}

template <typename DATA>
inline
bool bcec_TimeQueue<DATA>::isRegisteredHandle(
                            typename bcec_TimeQueue<DATA>::Handle handle) const
{
    return isRegisteredHandle(handle, Key(0));
}

template <typename DATA>
inline
bool bcec_TimeQueue<DATA>::isRegisteredHandle(
                               typename bcec_TimeQueue<DATA>::Handle handle,
                               const Key&                            key) const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    int index = (handle & d_indexMask) - 1;

    if ( 0 > index || index >= (int)d_nodeArray.size()) {
        return false;
    }
    Node *node = d_nodeArray[index];

    if (node->d_index != (int)handle || node->d_key != key) {
        return false;
    }

    return true;
}

template <typename DATA>
inline
int bcec_TimeQueue<DATA>::minTime(bdet_TimeInterval *buffer) const
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    if (d_map.empty()) {
        return 1;
    }

    *buffer = d_map.begin()->first;
    return 0;
}

                             // -------------------------
                             // struct bcec_TimeQueueItem
                             // -------------------------

// CREATORS
template <typename DATA>
bcec_TimeQueueItem<DATA>::bcec_TimeQueueItem(bslma_Allocator *basicAllocator)
: d_key(0)
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_data);
    bslalg_ScalarPrimitives::defaultConstruct(&d_data, basicAllocator);
}

template <typename DATA>
bcec_TimeQueueItem<DATA>::
bcec_TimeQueueItem(bcec_TimeQueueItem<DATA> const&  original,
                   bslma_Allocator                 *basicAllocator)
: d_time(original.d_time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(original.d_handle)
, d_key(original.d_key)
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_data);
    bslalg_ScalarPrimitives::copyConstruct(&d_data,
                                           original.d_data,
                                           basicAllocator);
}

template <typename DATA>
bcec_TimeQueueItem<DATA>::
bcec_TimeQueueItem(const bdet_TimeInterval&  time,
                   const DATA&               data,
                   Handle                    handle,
                   bslma_Allocator          *basicAllocator)
: d_time(time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(handle)
, d_key(0)
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_data);
    bslalg_ScalarPrimitives::copyConstruct(&d_data,
                                           data,
                                           basicAllocator);
}

template <typename DATA>
bcec_TimeQueueItem<DATA>::
bcec_TimeQueueItem(const bdet_TimeInterval&  time,
                   const DATA&               data,
                   Handle                    handle,
                   const Key&                key,
                   bslma_Allocator          *basicAllocator)
: d_time(time)
// require that 'd_data' be default-constructible, hopefully at no cost
, d_handle(handle)
, d_key(key)
{
    bslalg_ScalarDestructionPrimitives::destroy(&d_data);
    bslalg_ScalarPrimitives::copyConstruct(&d_data,
                                           data,
                                           basicAllocator);
}

// MANIPULATORS
template <typename DATA>
inline
bdet_TimeInterval& bcec_TimeQueueItem<DATA>::time()
{
    return d_time;
}

template <typename DATA>
inline
DATA& bcec_TimeQueueItem<DATA>::data()
{
    return d_data;
}

/*
// this definition was moved into the class declaration
// to work around a Visual Studio .NET 2003 bug.
template <typename DATA>
inline
typename bcec_TimeQueueItem<DATA>::Handle&
bcec_TimeQueueItem<DATA>::handle()
{
    return d_handle;
}
*/

template <typename DATA>
inline
typename bcec_TimeQueueItem<DATA>::Key&
bcec_TimeQueueItem<DATA>::key()
{
    return d_key;
}

// ACCESSORS
template <typename DATA>
inline
const bdet_TimeInterval& bcec_TimeQueueItem<DATA>::time() const
{
    return d_time;
}

template <typename DATA>
inline
const DATA& bcec_TimeQueueItem<DATA>::data() const
{
    return d_data;
}

/*
// this definition was moved into the class declaration
// to work around a Visual Studio .NET 2003 bug.
template <typename DATA>
inline
typename bcec_TimeQueueItem<DATA>::Handle
bcec_TimeQueueItem<DATA>::handle() const
{
    return d_handle;
}
*/

template <typename DATA>
inline
const typename bcec_TimeQueueItem<DATA>::Key&
bcec_TimeQueueItem<DATA>::key() const
{
    return d_key;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
