// bcec_timequeue.t.cpp             -*-C++-*-

#include <bcec_timequeue.h>

#include <bcema_testallocator.h>
#include <bcemt_lockguard.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>

#include <bdef_bind.h>
#include <bslma_defaultallocatorguard.h>
#include <bdetu_systemtime.h>
#include <bdetu_time.h>
#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_set.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The class under testing is a parameterized queuing mechanism where the key
// is a time value and the data a 'DATA' template parameter.
// Items are order by time value within the key.  Items that have the same time
// are stored in a circular linked list.  Thus it is particularly important to
// test with items having the same time value and try and splitting the
// circular lists at various places.  We test all functions in isolation.
//
// Note that this component is not tested, indeed not designed, with respect to
// exception safety.  (The constructor of the 'DATA' could throw.)  The
// component-level documentation should also be updated to explicitly state the
// guarantees.  Memory allocation, however, is well documented and tested in
// case 8.  Concurrent access (a concern due to the reuse of nodes in the free
// list) is also a concern and tested in case 11.
//-----------------------------------------------------------------------------
// [3 ] bcec_TimeQueue(bslma_Allocator *allocator=0);
// [3 ] ~bcec_TimeQueue();
// [5 ] int popFront(bcec_TimeQueueItem<DATA> *buffer);
// [6 ] int popFront(bcec_TimeQueueItem<DATA> *buffer,...
// [7 ] void popLE(const bdet_TimeInterval& time,...
// [4 ] int remove(Handle timeId, bdet_TimeInterval *newMinTime=0,...
// [5 ] void removeAll(bsl::vector<bcec_TimeQueueItem<DATA> > *buffer=0);
// [3 ] int add(const bdet_TimeInterval& time, const DATA& data, ...
// [3 ] int add(const bcec_TimeQueueItem<DATA> &item, int *isNewTop=0...
// [8 ] int update(int handle, const bdet_TimeInterval &newTime,...
// [3 ] int length() const;
// [3 ] bool isRegisteredHandle(int handle) const;
// [3 ] int minTime(bdet_TimeInterval *buffer);
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
// [2 ] CLASS 'bcec_TimeQueueItem'
// [9 ] CONCERN: Respect the 'bdema' allocator model
// [10] CONCERN: Callbacks and 'DATA' destructors invoked while holding lock
// [11] CONCURRENCY TEST
// [14] Usage example
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define NL() cout << endl;                    // End of line
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define Q_(X) cout << "<| " #X " |>, ";       // Q(X) without '\n'
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define L_ __LINE__                           // current Line number

bcemt_Mutex coutMutex;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bcec_TimeQueue<const char*> Obj;
typedef bcec_TimeQueueItem<const char*> Item;

                        // ================
                        // class TestString
                        // ================

class TestString {
    // This class is a string with allocation, except that a
    // default-constructed 'TestString' is guaranteed *not* to trigger an
    // allocation.  This class is used in test cases 9 and 11.

    bslma_Allocator *d_allocator_p;  // held, not owned
    bsl::string     *d_string_p;     // owned

    static bsl::string s_emptyString;
  public:
    // TYPES
    BSLALG_DECLARE_NESTED_TRAITS(TestString,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit TestString(bslma_Allocator *allocator = 0);
    explicit TestString(const char *s, bslma_Allocator *allocator = 0);
    explicit TestString(const bsl::string& s, bslma_Allocator *allocator = 0);
        // Create a string, optionally initialized with 's', using 'allocator'
        // to supply memory.  If allocator is null, the currently-installed
        // default allocator is used.

    ~TestString();
        // Destroy this string object.

    // MANIPULATORS
    TestString& operator=(const TestString& s);
    TestString& operator=(const bsl::string& s);
        // Assign the string 's' to this instance.

    // ACCESSORS
    operator const bsl::string& () const;
        // Return the string value of this object.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& os, const TestString& string);
    // Output the specified 'string' to the specified 'stream'.

bool operator==(const TestString& s1, const TestString& s2);
    // Return 0 whether the specified strings 's1' and 's2' hold the same C++
    // string and 1 if not.

bool operator!=(const TestString& s1, const TestString& s2);
    // Return 0 whether the specified strings 's1' and 's2' do not hold the
    // same C++ string and 1 if they do.

                        // ----------------
                        // class TestString
                        // ----------------

// CLASS MEMBERS
bsl::string TestString::s_emptyString; // default-initialized

// CREATORS
TestString::TestString(bslma_Allocator *alloc)
: d_allocator_p(bslma_Default::allocator(alloc))
, d_string_p(0)
{
}

TestString::TestString(const char *s, bslma_Allocator *alloc)
: d_allocator_p(bslma_Default::allocator(alloc))
{
    d_string_p = new(*d_allocator_p) bsl::string(s, d_allocator_p);
}

TestString::TestString(const bsl::string& s, bslma_Allocator *alloc)
: d_allocator_p(bslma_Default::allocator(alloc))
{
    d_string_p = new(*d_allocator_p) bsl::string(s, d_allocator_p);
}

TestString::~TestString()
{
    if (d_string_p) {
        d_allocator_p->deleteObjectRaw(d_string_p);
        d_string_p = 0;
    }
}

// MANIPULATORS
TestString& TestString::operator=(const TestString& s)
{
    if (d_string_p) {
        d_allocator_p->deleteObjectRaw(d_string_p);
        d_string_p = 0;
    }
    if (s.d_string_p) {
        d_string_p = new(*d_allocator_p) bsl::string(*s.d_string_p,
                                                     d_allocator_p);
    }
    return *this;
}

TestString& TestString::operator=(const bsl::string& s)
{
    if (d_string_p) {
        d_allocator_p->deleteObjectRaw(d_string_p);
        d_string_p = 0;
    }
    d_string_p = new(*d_allocator_p) bsl::string(s, d_allocator_p);
    return *this;
}

// ACCESSORS
TestString::operator const bsl::string&() const
{
    if (d_string_p) {
        return *d_string_p;
    }
    return s_emptyString;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& os, const TestString& s)
{
    return os << (const bsl::string&)s;
}

bool operator==(const TestString& s1, const TestString& s2)
{
    return (const bsl::string&)s1 == (const bsl::string&)s2;
}

bool operator!=(const TestString& s1, const TestString& s2)
{
    return !(s1 == s2);
}

//=============================================================================
//                          CASE 11 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BCEC_TIMEQUEUE_TEST_CASE_11 {

enum {
    NUM_THREADS    = 10,
    NUM_ITERATIONS = 1000,               // per thread
    NUM_REMOVE_ALL = NUM_ITERATIONS / 2  // between two 'removeAll'
};

typedef bsl::string DATA;
typedef bcec_TimeQueue<DATA> TimeQueue;
typedef bcec_TimeQueueItem<DATA> TimeQueueItem;

bcema_TestAllocator ta(veryVeryVerbose);
TimeQueue timequeue(&ta);

bcemt_Barrier barrier(NUM_THREADS + 1);

struct case11ThreadInfo {
    int                         d_id;
    bsl::vector<TimeQueueItem> *d_items_p;
};

extern "C" {

void *testAddUpdatePopRemoveAll(void *arg)
    // Invoke 'add', 'update', 'popFront', 'popLE', and/or 'removeAll' in a
    // loop.
{
    case11ThreadInfo *info = (case11ThreadInfo*)arg;
    const int THREAD_ID = info->d_id;
    bsl::vector<TimeQueueItem> *vPtr = info->d_items_p;

    // We stagger the removeAll steps among the threads.
    const int STEP_REMOVE_ALL = THREAD_ID * NUM_REMOVE_ALL / NUM_THREADS;

    bsl::ostringstream oss;
    oss << THREAD_ID;
    DATA V(oss.str());
    if (verbose) {
        coutMutex.lock();
        T_(); P_(THREAD_ID); Q_(CREATION); P(V);
        coutMutex.unlock(); }

    barrier.wait();

    int newLen;
   bdet_TimeInterval newMinTime;
    TimeQueueItem item;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        if (veryVerbose) {
            coutMutex.lock();
            T_(); P_(THREAD_ID); Q_(ITERATION); P(i);
            coutMutex.unlock();
        }
        const bdet_TimeInterval TIME((i * (i + 3)) % NUM_ITERATIONS);
        int h = timequeue.add(TIME, V);
        timequeue.update(h, TIME);
        if (0 == timequeue.popFront(&item, &newLen, &newMinTime)) {
            vPtr->push_back(item);
        }
        h = timequeue.add(newMinTime, V);
        timequeue.popLE(newMinTime, vPtr);
        if (0 == timequeue.remove(h, &newLen, &newMinTime, &item)) {
            vPtr->push_back(item);
        }
        if (i % NUM_REMOVE_ALL == STEP_REMOVE_ALL) {
            timequeue.removeAll(vPtr);
            if (veryVerbose) {
                coutMutex.lock();
                T_(); P_(THREAD_ID); Q_(REMOVE_ALL); P(i);
                coutMutex.unlock();
            }
        }
    }
    return NULL;
}

void *testLength(void *arg)
    // Invoke 'length' in a loop.
{
    barrier.wait();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int len = timequeue.length();
        LOOP2_ASSERT(i, len, len >= 0);
        LOOP2_ASSERT(i, len, len <= NUM_THREADS);
    }
    return NULL;
}

} // extern "C"

} // close namespace BCEC_TIMEQUEUE_TEST_CASE_11
//=============================================================================
//                      CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace BCEC_TIMEQUEUE_TEST_CASE_10 {

                        // ====================
                        // class TestLockObject
                        // ====================

class TestLockObject {
    // This small test object holds a time queue reference, and attempts to
    // access one of its blocking functions upon destruction.

    // PRIVATE DATA MEMBERS
    const bcec_TimeQueue<TestLockObject> *d_timeQueue_p; // held, not owned
    int                                  *d_numDestructions_p;
    int                                   d_verbose;

  public:
    // CREATORS
    TestLockObject(const bcec_TimeQueue<TestLockObject> *queue = 0,
                   int                                  *numDestructions = 0,
                   int                                   verbose = 0);
        // Create a test object that holds a reference to the specified
        // 'queue'.

    ~TestLockObject();
        // Destroy this test object, in the process trying to access the held
        // queue's 'minTime' function.

    // MANIPULATORS
    void reset();
        // Reset the held queue reference to 0.
};

                        // --------------------
                        // class TestLockObject
                        // --------------------

// CREATORS
TestLockObject::TestLockObject(
                         const bcec_TimeQueue<TestLockObject> *queue,
                         int                                  *numDestructions,
                         int                                   verbose)
: d_timeQueue_p(queue)
, d_numDestructions_p(numDestructions)
, d_verbose(verbose)
{
}

TestLockObject::~TestLockObject()
{
    if (d_timeQueue_p) {
        bdet_TimeInterval buffer;
        int isEmptyFlag = d_timeQueue_p->minTime(&buffer);  // may block
        ++(*d_numDestructions_p);
        if (d_verbose) {
            bsl::cout << "Destroying TestLockObject... ";
            if (isEmptyFlag) {
                bsl::cout << "Time queue is empty." << bsl::endl;
            }
            else {
                bsl::cout << "Time queue begins at: " << buffer << bsl::endl;
            }
        }
    }
}

// MANIPULATORS
void TestLockObject::reset()
{
    d_timeQueue_p = 0;
}

} // close namespace BCEC_TIMEQUEUE_TEST_CASE_10

//=============================================================================
//                      CASE -100 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_TIMEQUEUE_TEST_CASE_MINUS_100 {

static bces_AtomicInt currentTime = 0;

enum {
    NUM_THREADS    = 4,
    NUM_ITERATIONS = 16,
    SEND_COUNT = 1000,
    RCV_COUNT = 800,
    DELAY = 500,
    BITS_PER_HANDLE = 22
};

void threadFunc(bcec_TimeQueue<int> *timeQueue,
                int                  numIterations,
                int                  sendCount,
                int                  rcvCount,
                int                  delay)
{
    bsl::vector<int> timers;
    timers.resize(sendCount);

    bsl::vector<bcec_TimeQueueItem<int> > resubmit;
    bsls_Stopwatch sw;

    for(int i=0; i<numIterations; i++) {
        if( verbose ) {
            sw.start();
        }

        // "send" messages
        for(int snd=0; snd<sendCount; snd++) {
            currentTime++;
            bdet_TimeInterval t(currentTime + delay, 0);
            timers[snd] = timeQueue->add(t, delay);
        }

        // "receive" replies
        for(int rcv=0; rcv<rcvCount; rcv++) {
            timeQueue->remove(timers[rcv]);
        }

        // "resend" replies
        bdet_TimeInterval now(currentTime, 0);
        timeQueue->popLE(now, &resubmit);
        int numResubmitted = resubmit.size();
        for(int retry=0; retry<numResubmitted; retry++) {
            int newdelay = resubmit[retry].data() * 2;
            bdet_TimeInterval t(currentTime + newdelay, 0);
            timeQueue->add(t, newdelay);
        }

        if( verbose ) {
            sw.stop();

            int iteration = i;
            int queueLength = timeQueue->length();
            double elapsed = sw.elapsedTime();
            P(iteration);
            P(queueLength);
            P(numResubmitted);
            P(elapsed);
        }
    }
}

void run()
{
    if (verbose) cout << endl
                      << "The router simulation (kind of) test" << endl
                      << "====================================" << endl;

    bcec_TimeQueue<int> timeQueue(BITS_PER_HANDLE);

    bcemt_ThreadUtil::Handle threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::create(&threads[i],
                                 bdef_BindUtil::bind(&threadFunc,
                                                     &timeQueue,
                                                     (int)NUM_ITERATIONS,
                                                     (int)SEND_COUNT,
                                                     (int)RCV_COUNT,
                                                     (int)DELAY));
    }


    for (int i = 0; i < NUM_THREADS; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

}

} // close namespace BCEC_TIMEQUEUE_TEST_CASE_MINUS_100

//=============================================================================
//          USAGE EXAMPLE from header (with assert replaced with ASSERT)
//-----------------------------------------------------------------------------
namespace BCEC_TIMEQUEUE_USAGE_EXAMPLE {

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
    extern "C" {

    void* my_connectionMonitorThreadEntry(void *server);

    void* my_timerMonitorThreadEntry(void *server);

    }
//..
///struct 'my_Connection'
///- - - - - - - - - - -
// The 'my_Connection' structure is used by 'my_Server' to manage a single
// physical connection on behalf of a 'my_Session'.
//..
    class my_Session;
    struct my_Connection {
        int         d_timerId;
        my_Session *d_session_p;
    };
//..
///Protocol classes
///- - - - - - - -
// Protocol class 'my_Session' provides a pure abstract protocol to manage a
// single "session" to be associated with a specific connection on a server.
//..
    class my_Session {
        // Pure protocol class to process a data buffer of arbitrary size.
        // Concrete implementations in the "real world" would typically
        // manage an external connection like a socket.

      public:
        inline my_Session();
        virtual int processData(void *data, int length) = 0;
        virtual int handleTimeout(my_Connection *connection) = 0;
        virtual ~my_Session();
    };
//..
// The constructor and destructor do nothing:
//..
    my_Session::my_Session()
    {
    }

    my_Session::~my_Session()
    {
    }
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
    class my_Server {
        // Simple server supporting multiple Connections.

      private:
        bsl::vector<my_Connection*>     d_connections;
        bcec_TimeQueue<my_Connection*>  d_timeQueue;
        int                             d_ioTimeout;
        bcemt_Mutex                     d_timerMonitorMutex;
        bcemt_Condition                 d_timerChangedCond;
        bcemt_ThreadUtil::Handle        d_timerThreadHandle;
        bcemt_ThreadUtil::Handle        d_connectionThreadHandle;
        volatile bool                   d_done;

      protected:
        void newConnection(my_Connection *connection);
            // Add the specified 'connection' to the current 'my_Server',
            // setting the new timeout value to the current time plus the
            // timeout value provided at construction of this 'my_Server'
            // instance.  If the added connection is the new "top" of the
            // queue, signal that the minimum time on the queue has changed.
            // Upon seeing this signal, the TimerMonitor thread will wake up
            // and look for expired timers.
            //
            // Behavior is undefined if 'connection' has already been added
            // to any 'my_Server' and has not been removed via member
            // function 'closeConnection'.

        void removeConnection(my_Connection *connection);
            // Remove the specified 'connection' from the current 'my_Server',
            // so that it will no longer be monitored for available data.

        virtual void closeConnection(my_Connection *connection)=0;
            // Provide a mechanism for a concrete implementation to close a
            // connection.

        void dataAvailable(my_Connection *connection,
                           void          *buffer_p,
                           int           length);
            // Receive in 'buffer_p' a pointer to a data buffer of 'length'
            // bytes, and pass this to 'connection' to be processed.  Behavior
            // is undefined if 'connection' is not currently added to this
            // 'my_Server' object, or if 'length' <= 0.

      protected:
        virtual void monitorConnections()=0;
            // Monitor all connections in the current 'my_Server'.  When data
            // becomes available for a given connection, pass the data to
            // that connection for processing.

        void monitorTimers();
            // Monitor all timers in the current 'my_Server', and handle each
            // timer as it expires.

        friend void* my_connectionMonitorThreadEntry(void *server);
        friend void* my_timerMonitorThreadEntry(void *server);

      public:
        // CREATORS
        my_Server(int ioTimeout, bslma_Allocator *allocator=0);
            // Construct a 'my_Server' object with a timeout value of
            // 'ioTimeout' seconds.  Use the specified 'allocator' for all
            // memory allocation for data members of 'my_Server'.

        virtual ~my_Server();

        // MANIPULATORS
        int start();
            // Begin monitoring timers and connections.
    };
//..
// The constructor is simple: it initializes the internal 'bcec_TimeQueue' and
// sets the I/O timeout value.  The virtual destructor does nothing.
//..
    my_Server::my_Server(int ioTimeout, bslma_Allocator *allocator)
    : d_timeQueue(allocator)
    , d_ioTimeout(ioTimeout)
    , d_connectionThreadHandle(bcemt_ThreadUtil::invalidHandle())
    , d_timerThreadHandle(bcemt_ThreadUtil::invalidHandle())
    {
    }

    my_Server::~my_Server()
    {
        d_done = true;
        d_timerChangedCond.broadcast();
        if (bcemt_ThreadUtil::invalidHandle() != d_connectionThreadHandle) {
            bcemt_ThreadUtil::join(d_connectionThreadHandle);
        }
        if (bcemt_ThreadUtil::invalidHandle()!= d_timerThreadHandle) {
            bcemt_ThreadUtil::join(d_timerThreadHandle);
        }
    }
//..
// Member function 'newConnection' adds the 'connection' to the current
// set of connections to be monitored.  This is done in two steps.
// First, the 'connection' is added to the internal array, and then a
// timer is set for the 'connection' by creating a corresponding entry
// in the internal 'bcec_TimeQueue'.
//..
    void my_Server::newConnection(my_Connection *connection)
    {
        d_connections.push_back(connection);
        int isNewTop=0;
        connection->d_timerId = d_timeQueue.add(bdetu_SystemTime::now() +
                                                     d_ioTimeout,
                                                 connection,
                                                 &isNewTop);
        if (isNewTop) {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
            d_timerChangedCond.signal();
        }
    }
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
    void my_Server::removeConnection(my_Connection *connection)
    {
        // Remove from d_timeQueue
        d_timeQueue.remove(connection->d_timerId);
        // Remove from d_connections
        bsl::vector<my_Connection*>::iterator begin = d_connections.begin(),
            end = d_connections.end(),
            it = begin;
        for (; it != end; ++it) {
            if (connection == *it) {
                d_connections.erase(it);
            }
        }
    }
//..
// The 'dataAvailable' function will be called when data becomes available for
// a specific connection.  It removes the connection from the timer queue while
// the connection is busy, processes the available data, and returns the
// connection to the queue with a new time value.
//..
    void my_Server::dataAvailable(my_Connection *connection,
                                  void *data,
                                  int length)
    {
        if (connection->d_timerId) {
            if (d_timeQueue.remove(connection->d_timerId))  return;
            connection->d_timerId = 0;
        }
        connection->d_session_p->processData(data, length);

        int isNewTop=0;

        connection->d_timerId = d_timeQueue.add(bdetu_SystemTime::now() +
                                                    d_ioTimeout,
                                                connection,
                                                &isNewTop);
        if (isNewTop) {
            bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
            d_timerChangedCond.signal();
        }
    }
//..
// Function 'monitorTimers' manages the timer monitor thread; it is called when
// the thread is spawned, and checks repeatedly for expired timers; after each
// check, it does a timed wait based upon the minimum time value seen in the
// queue after all expired timers have been removed.
//..
    void my_Server::monitorTimers()
    {
        while(!d_done) {
            bsl::vector<bcec_TimeQueueItem<my_Connection*> > expiredTimers;
            {
                bcemt_LockGuard<bcemt_Mutex> lock(&d_timerMonitorMutex);
                bdet_TimeInterval minTime;
                int newLength;

                d_timeQueue.popLE(bdetu_SystemTime::now(),
                                  &expiredTimers,
                                  &newLength,
                                  &minTime );

                if (!expiredTimers.size()) {
                    if (newLength) {
                        // no expired timers, but unexpired timers remain.
                        d_timerChangedCond.timedWait(&d_timerMonitorMutex,
                                                     minTime);
                    }
                    else {
                        // no expired timers, and timer queue is empty.
                        d_timerChangedCond.wait(&d_timerMonitorMutex);
                    }
                    continue;
                }
            }

            int length = expiredTimers.size();
            if (length) {
                bcec_TimeQueueItem<my_Connection*> *data =
                                                        &expiredTimers.front();
                for( int i=0; i < length; ++i) {
                    closeConnection(data[i].data());
                }
            }
        }
    }
//..
// Function 'start' spawns two separate threads.  The first thread will monitor
// connections and handle any data received on them.  The second monitors the
// internal timer queue and removes connections that have timed out.  Function
// 'start' calls 'bcemt_ThreadUtil::create, which expects a function pointer to
// a function with the standard "C" callback signature 'void *fn(void* data)'.
// This non-member function will call back into the 'my_Server' object
// immediately.
//..
    int my_Server::start()
    {
        bcemt_Attribute attr;

        if (bcemt_ThreadUtil::create(&d_connectionThreadHandle, attr,
                                     &my_connectionMonitorThreadEntry,
                                     this)) {
            return -1;
        }

        if (bcemt_ThreadUtil::create(&d_timerThreadHandle, attr,
                                     &my_timerMonitorThreadEntry,
                                     this)) {
            return -1;
        }
        return 0;
    }
//..
// Finally, we are now in a position to implement the two thread dispatchers:
//..
    extern "C" {

    void *my_connectionMonitorThreadEntry(void *server)
    {
        ((my_Server*)server)->monitorConnections();
        return server;
    }

    void *my_timerMonitorThreadEntry(void *server)
    {
        ((my_Server*)server)->monitorTimers();
        return server;
    }

    }
//..
// In order to test our server, we provide two concrete implementations of
// a test session and of a test server as follows.
//..
    // myTestSession.h             -*-C++-*-

    class my_TestSession : public my_Session {
        // Concrete implementation of my_Session, providing simple test
        // semantics In particular, implement the virtual function
        // processData() to record all incoming data for the controlling
        // connection, and virtual function handleTimeout() for handling
        // timeouts.

        int d_verbose;
      public:
        // CREATORS
        my_TestSession(int verbose) : my_Session(), d_verbose(verbose) { }

        // MANIPULATORS
        virtual int handleTimeout(my_Connection *connection) {
            // Do something to handle timeout.
            if (d_verbose) {
                bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
                bsl::cout << "Connection " << connection << "timed out.\n";
            }
            return 0;
        }

        virtual int processData(void *data, int length) {
            // Do something with the data...
            if (d_verbose) {
                bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
                bsl::cout << "Processing data at address " << data
                          << " and length " << length << ".\n";
            }
            return 0;
        }
    };

    // myTestSession.h             -*-C++-*-

    class my_TestServer :  public my_Server {
        // Concrete implementation of my_Server, providing connection logic.

        int d_verbose;

      protected:
        virtual void closeConnection(my_Connection *connection);
            // Close external connection and call 'removeConnection' when
            // done.

        virtual void monitorConnections();
            // Monitor all connections in the current 'my_Server'.  When data
            // becomes available for a given connection, pass the data to
            // that connection for processing.

      public:
        // CREATORS
        my_TestServer(int              ioTimeout,
                      int              verbose = 0,
                      bslma_Allocator *allocator = 0)
        : my_Server(ioTimeout,allocator)
        , d_verbose(verbose)
        {
        };

        virtual ~my_TestServer();
    };

    // myTestSession.cpp             -*-C++-*-

    my_TestServer::~my_TestServer()
    {
    }

    void my_TestServer::closeConnection(my_Connection *connection)
    {
        if (d_verbose) {
            bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
            bsl::cout << "Closing connection " << connection << bsl::endl;
        }
        delete connection;
    }

    void my_TestServer::monitorConnections()
    {
        my_Session *session = new my_TestSession(d_verbose);

        // Simulate connection monitor logic...
        my_Connection *connection1 = new my_Connection;
        connection1->d_session_p = session;
        newConnection(connection1);
        if (d_verbose) {
            bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
            bsl::cout << "Opening connection " << connection1 << endl;
        }

        my_Connection *connection2 = new my_Connection;
        connection2->d_session_p = session;
        newConnection(connection2);
        if (d_verbose) {
            bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
            bsl::cout << "Opening connection " << connection2 << endl;
        }

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(2)); // 2s

        // Simulate transmission...
        const int  length = 1024;
        const char*buffer[length];
        if (d_verbose) {
            bsl::cout << bdetu_SystemTime::nowAsDatetimeGMT() << ": ";
            bsl::cout << "Connection " << connection1
                      << " receives " << length << " bytes " << endl;
        }
        dataAvailable(connection1, buffer, length);

        // Wait for timeout to occur, otherwise session get destroyed from
        // stack too early.

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(8)); // 8s
    }
//..
// The program that would exercise this test server would simply consist of:
//..
    int usageExample(int verbose)
    {
        my_TestServer mX(5, verbose); // timeout for connections: 5s
        mX.start();

        // Wait sufficiently long to observe all events.
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10)); // 10s

        return 0;
    }
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

} // close namespace BCEC_TIMEQUEUE_USAGE_EXAMPLE



bdet_TimeInterval makeTimeInterval()
{
    static bces_AtomicInt counter(0);
    return bdet_TimeInterval((double)counter++);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bcema_TestAllocator ta(veryVeryVerbose);
    bcema_TestAllocator defaultAlloc(veryVeryVerbose);
    bslma_DefaultAllocatorGuard defaultAllocGuard(&defaultAlloc);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //   The usage example from the header has been incorporated into this
        //   test driver.  All references to 'assert' have been replaced with
        //   'ASSERT'.  Call the test example function and assert that it works
        //   as expected.
        //
        // Plan:
        //   Create a test server with a timeout of five seconds, start it.
        //   The monitor connection will create one session and initiate
        //   several connections on that session, and send data to some of
        //   these connections.  This thread will simply wait long enough so
        //   that the session times out.  Since this is a usage and not a test,
        //   we let the server output events, and the user can visually inspect
        //   that events happen as expected.
        //
        // Testing:
        //   USAGE example
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing USAGE example" << endl
                          << "=====================" << endl;

        using namespace BCEC_TIMEQUEUE_USAGE_EXAMPLE;
        {
            usageExample(verbose);
        }

      } break;
    case 13: {
        // --------------------------------------------------------------------
        // 'poolTimerMemory' PERFORMANCE TESTING
        //
        // Concerns:
        //: 1 Using the 'poolTimerMemory' flag while constructing a time
        //:   queue provides a performance benefit.
        //
        // Plan:
        //: 1 Specify two constant values, 'NUM_INNER_ITERATIONS' and
        //:   'NUM_OUTER_TRANSITIONS' specifying the number of timers to be
        //:   registered in a batch and how many such batches should be
        //:   registered respectively.
        //:
        //: 2 Starting from 'now' construct a set of random timers ('timers')
        //:   that will be registered with the time queue.
        //:
        //: 3 Identify intermediate times ('popTimes') when timers will be
        //:   popped from the time queue.
        //:
        //: 4 Create a time queue, 'X', without specifying the
        //:   'poolTimerMemory' flag, and specifying a test allocator.  For
        //:   each value from [0..NUM_OUTER_ITERATIONS]:
        //:
        //:   1 Start a stop watch to record the time taken.
        //:
        //:   2 Add 'NUM_INNER_ITERATIONS' timers from the 'timers' vector.
        //:
        //:   3 Sleep for a certain interval to let the timers expire.
        //:
        //:   4 Pop all timers that have expired.
        //:
        //:   5 Stop the stop watch and record elapsed time.
        //:
        //: 5 Repeat Plan-4.1-5 for a time queue created by specifying the
        //:   'poolTimerMemory' flag.
        //:
        //: 6 In verbose mode print out the time taken by both time queues and
        //:   also the memory allocation characteristics as specified by their
        //:   respective test allocators.
        //
        // Testing:
        //   'poolTimerMemory' flag performance testing
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Performance test using 'poolTimerMemory' flag" << endl
                 << "=============================================" << endl;

        const char VA[] = "A";

#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_SOLARIS)
        // Specifying larger number of iterations causes these platforms to
        // take a really long time to complete the test.

        const int NUM_OUTER_ITERATIONS = 1000;
        const int NUM_INNER_ITERATIONS = 50;
#else
        const int NUM_OUTER_ITERATIONS = 10000;
        const int NUM_INNER_ITERATIONS = 100;
#endif
        bslma_TestAllocator na1, na2;

        const int NUM_TOTAL_ITERATIONS =
                                   NUM_OUTER_ITERATIONS * NUM_INNER_ITERATIONS;

        bdet_TimeInterval         TIME = bdetu_SystemTime::now();
        int                       isNewTop;
        bdet_TimeInterval         newMinTime;
        int                       newLength;
        vector<int>               handles(NUM_TOTAL_ITERATIONS);
        vector<Item>              items(NUM_TOTAL_ITERATIONS);
        vector<bdet_TimeInterval> timers(NUM_TOTAL_ITERATIONS, TIME);
        vector<bdet_TimeInterval> popTimes(NUM_OUTER_ITERATIONS, TIME);

        srand(time(0));

        for (int i = 0, k = 0; i < NUM_OUTER_ITERATIONS; ++i) {
            for (int j = 0; j < NUM_INNER_ITERATIONS; ++j, ++k) {
                timers[k].addMilliseconds(rand() % NUM_TOTAL_ITERATIONS);
            }
            popTimes[i].addMilliseconds(i * NUM_INNER_ITERATIONS);
        }

        bsls_Stopwatch s;
        s.start();
        {
            Obj mX(&na1); const Obj& X = mX;
            for (int j = 0, t = 0; j < NUM_OUTER_ITERATIONS; ++j) {
                for (int k = 0; k < NUM_INNER_ITERATIONS; ++k, ++t) {
                    handles[k] = mX.add(timers[k], VA, &isNewTop, &newLength);
                }

                bcemt_ThreadUtil::microSleep(NUM_INNER_ITERATIONS);

                mX.popLE(popTimes[j], &items, &newLength, &newMinTime);
            }
        }
        s.stop();

        if (veryVerbose) {
            P(s.elapsedTime())
        }

        s.reset();
        s.start();
        {
            Obj mX(true, &na2); const Obj& X = mX;
            for (int j = 0, t = 0; j < NUM_OUTER_ITERATIONS; ++j) {
                for (int k = 0; k < NUM_INNER_ITERATIONS; ++k, ++t) {
                    handles[k] = mX.add(timers[k], VA, &isNewTop, &newLength);
                }

                bcemt_ThreadUtil::microSleep(NUM_INNER_ITERATIONS);

                mX.popLE(popTimes[j], &items, &newLength, &newMinTime);
            }
        }
        s.stop();

        if (veryVerbose) {
            P(s.elapsedTime())
        }

        if (veryVerbose) {
            P(na1.numBlocksTotal());
            P(na1.numBytesTotal());
            P(na1.numBlocksMax());
            P(na1.numBytesMax());
            P(na2.numBlocksTotal());
            P(na2.numBytesTotal());
            P(na2.numBlocksMax());
            P(na2.numBytesMax());
        }
      } break;
      case 12: {
          // ------------------------------------------------------------------
          // TESTING CONCERN: After draining the queue new elements may be
          // added to the queue.
          //
          // Plan:
          //   For each of the following functions that remove elements from
          //   the queue ('removeAll', 'popFront', 'popLE', 'remove'):
          //
          //   1. Add items to an initially empty queue until more items can be
          //      added.
          //   2. Ensure that the queue size is non-zero.
          //   3. Remove all the items from the queue using the currently
          //      tested function.
          //   4. Ensure that the queue is empty.
          //   5. Ensure that new items may be added to the queue.
          //
          // Testing:
          //   Draining the queue and adding new elements.
          // ------------------------------------------------------------------

          if (verbose) cout << endl
                          << "Testing \"Remove All\" followed by \"Add\""
                          << endl
                          << "======================================"
                          << endl;

          {
              if (veryVerbose) bsl::cout << "a) removeAll" << bsl::endl;

              bcec_TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(0 < q.length());

              q.removeAll();

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(makeTimeInterval(), 0));
          }

          {
              if (veryVerbose) bsl::cout << "b) popFront" << bsl::endl;

              bcec_TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bcec_TimeQueueItem<int> item;
              while (0 == q.popFront(&item))
                  ;

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(makeTimeInterval(), 0));
          }

          {
              if (veryVerbose) bsl::cout << "c) popLE" << bsl::endl;

              bcec_TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bsl::vector<bcec_TimeQueueItem<int> > items;
              q.popLE(makeTimeInterval(), &items);

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(bdet_TimeInterval(0), 0));
          }

          {
              if (veryVerbose) bsl::cout << "d) remove" << bsl::endl;

              bcec_TimeQueue<int> q;
              bsl::set<int> handles;

              while (true) {
                  int handle = q.add(makeTimeInterval(), 0);

                  if (-1 == handle) {
                      break;
                  }

                  handles.insert(handle);
              }

              ASSERT(q.length() > 0);

              for (bsl::set<int>::const_iterator it =
                                    handles.begin(); it != handles.end(); ++it)
              {
                  ASSERT(0 == q.remove(*it));
              }

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(makeTimeInterval(), 0));
          }


          {
              if (veryVerbose) bsl::cout << "e) popFront (once)" << bsl::endl;

              bcec_TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bcec_TimeQueueItem<int> item;
              ASSERT(0 == q.popFront(&item));
              ASSERT(-1 != q.add(makeTimeInterval(), 0));
          }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //   Verify the concurrent access to the time queue
        //
        // Concern:
        //   When a node is placed back on the free list from a call to
        //   'popFront', 'popLE' or 'removeAll', it must be ready for
        //   being reused right away even before that call terminates and
        //   without races.
        //
        // Plan:
        //   Create a time queue.  Create 'NUM_THREADS' threads and let each
        //   thread invoke 'add', 'find', 'update', 'popFront', and 'popLE' in
        //   a loop.  Create a thread, let it invoke 'length' in a loop and
        //   verify that there are at least 0 and no more than 'NUM_THREADS'
        //   elements at any given time.  At periodic intervals, let another
        //   thread invoke 'removeAll'.  Let all threads run concurrently.
        //   This test is mostly to verify that races don't happen, we are only
        //   going to do a mild error checking.  Nevertheless, let each thread
        //   gather all the items it removes in its own container, and check
        //   that the total size of those containers is the number of elements
        //   added.
        //
        // Testing:
        //   CONCERN: CONCURRENCY TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Concurrency test" << endl
                          << "================" << endl;

        using namespace BCEC_TIMEQUEUE_TEST_CASE_11;

        case11ThreadInfo info[NUM_THREADS];
        bcemt_ThreadUtil::Handle threads[NUM_THREADS + 1];
        bsl::vector<bcec_TimeQueueItem<DATA> > items[NUM_THREADS];

        for (int i = 0; i < NUM_THREADS; ++i) {
            info[i].d_id = i;
            info[i].d_items_p = &items[i];
            bcemt_ThreadUtil::create(&threads[i],
                                     testAddUpdatePopRemoveAll,
                                     (void *)&info[i]);
        }

        bcemt_ThreadUtil::create(&threads[NUM_THREADS], testLength, NULL);

        int size = 0;
        for (int i = 0; i < NUM_THREADS; ++i) {
            bcemt_ThreadUtil::join(threads[i]);
            size += items[i].size();
        }
        bcemt_ThreadUtil::join(threads[NUM_THREADS]);

        LOOP_ASSERT(timequeue.length(), 0 == timequeue.length());
        LOOP_ASSERT(size, NUM_ITERATIONS * NUM_THREADS * 2 == size);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST CONCERN: 'DATA' destructors invoked while holding lock
        //
        // Concern:
        //   Whenever an object belonging to the queue is destroyed (either
        //   while 'pop*', 'remove*', or the queue destructor, it could invoke
        //   an action that attempts to lock this time queue (e.g.,  access the
        //   'minTime'.  Thus the 'DATA' destructor should never be invoked
        //   while holding the queue's internal lock.
        //
        // Plan:
        //   Create a time queue holding elements whose destructor purposely
        //   tries to access the 'minTime'.  Then remove these elements in a
        //   variety of fashions.
        //
        // Testing:
        //   CONCERN: 'DATA' DESTRUCTORS INVOKED WHILE HOLDING LOCK
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Concern: Callback deadlock" << endl
                          << "==================================" << endl;

        using namespace BCEC_TIMEQUEUE_TEST_CASE_10;

        int numDestructions = 0;
        {
            typedef bcec_TimeQueue<TestLockObject> Obj;

            Obj mX(&ta);
            const Obj& X = mX;

            TestLockObject mL(&X, &numDestructions, veryVerbose);
            const TestLockObject& L = mL;

            const bdet_TimeInterval NOW = bdetu_SystemTime::now();
            const bdet_TimeInterval T1  = NOW + bdet_TimeInterval(1.0);
            const bdet_TimeInterval T2  = NOW + bdet_TimeInterval(2.0);
            const bdet_TimeInterval T3  = NOW + bdet_TimeInterval(3.0);
            const bdet_TimeInterval T4  = NOW + bdet_TimeInterval(4.0);

            /* Obj::Handle f = */ mX.add(NOW, L);
            /* Obj::Handle g = */ mX.add(T1, L);
            /* Obj::Handle h = */ mX.add(T2, L);
            int i = mX.add(T3, L);
            /* Obj::Handle j = */ mX.add(T4, L);
            ASSERT(5 == mX.length());
            ASSERT(0 == numDestructions);

            if (verbose) cout << "\tduring popFront...\n";
            mX.popFront();
            ASSERT(4 == mX.length());
            ASSERT(1 == numDestructions);

            if (verbose) cout << "\tduring popLE...\n";
            mX.popLE(T1);
            ASSERT(3 == mX.length());
            ASSERT(2 == numDestructions);
            mX.popLE(T2, 1);
            ASSERT(2 == mX.length());
            ASSERT(3 == numDestructions);

            if (verbose) cout << "\tduring update...\n";
            mX.update(i, NOW);
            ASSERT(2 == mX.length());
            ASSERT(3 == numDestructions);

            if (verbose) cout << "\tduring remove...\n";
            mX.remove(i);
            ASSERT(1 == mX.length());
            ASSERT(4 == numDestructions);

            if (verbose) cout << "\tduring removeAll...\n";
            mX.removeAll();
            ASSERT(0 == mX.length());
            ASSERT(5 == numDestructions);

            if (verbose) cout << "\tat destruction...\n";
            /* h  = */ mX.add(NOW, L);
            mL.reset();  // avoid complications with order of
                         // destruction of L and mX
            ASSERT(1 == mX.length());
        }
        ASSERT(6 == numDestructions);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST CONCERN: Respecting the 'bdema' allocator model
        //
        // Plan:
        //   Redo case 6, but using 'TestString' instead of 'const char*'.
        //   The only default allocation should be for the temporary
        //   empty constructs in the 'bcec_TimeQueueItem' constructors, and the
        //   test class 'TestString' guarantees that a default-constructed
        //   instance does not trigger an allocation.
        //
        // Testing:
        //   CONCERN: RESPECTING THE 'bdema' ALLOCATOR MODEL
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Concern: 'bdema' allocator model" << endl
                 << "========================================" << endl;

        bcema_TestAllocator ta2(veryVeryVerbose);
        const TestString VA("A", &ta2);
        const TestString VB("B", &ta2);
        const TestString VC("C", &ta2);
        const TestString VD("D", &ta2);
        const TestString VE("E", &ta2);
        const TestString VF("F", &ta2);
        const TestString VG("G", &ta2);
        const TestString VH("H", &ta2);
        ASSERT(0 == defaultAlloc.numAllocations());

        static const struct {
            int               d_lineNum;     // Source line number
            int               d_secs;
            int               d_nsecs;
            const TestString *d_value;
        } VALUES[] = {
            //line secs  nsecs    value
            //---- ----- -------- --------
            { L_,   2  , 1000000, &VE    },  // \x0
            { L_,   2  , 1000000, &VE    },  // \x1
            { L_,   2  , 1000000, &VE    },  // \x2
            { L_,   2  , 1000001, &VF    },  // \x3
            { L_,   1  , 9999998, &VC    },  // \x4
            { L_,   1  , 9999999, &VD    },  // \x5
            { L_,   0  , 0000000, &VA    },  // \x6
            { L_,   3  , 1000000, &VG    },  // \x7
            { L_,   3  , 1000000, &VG    },  // \x8
            { L_,   2  , 1500000, &VF    },  // \x9
            { L_,   4  , 1000001, &VH    },  // \xa
            { L_,   1  , 9999998, &VC    },  // \xb
            { L_,   1  , 9999999, &VD    },  // \xc
            { L_,   0  , 0000001, &VB    }   // \xd
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const char *SORTED_VALUES =
                                 "\x6\xd\x4\xb\x5\xc\x0\x1\x2\x3\x9\x7\x8\xa";

        typedef bcec_TimeQueue<TestString> Obj;

        if (verbose) cout << "\t'popLE' without maxTimers." << endl;
        {
            static const struct {
                int          d_line;
                int          d_secs;
                int          d_nsecs;
                int          d_expNumItems;
                const char  *d_expItems;
            } POP_DATA[] = {
                // line secs  nsecs     expNumItems expItems
                // ---- ----- --------- ----------- ----------------
                {  L_ , 1    , 0       , 2         , "\x6\xd"          },
                {  L_ , 1    , 0       , 0         , ""                },
            //  {  L_ , 1    , 9999998 , 2         , "\x4\xb"          },
            //  {  L_ , 1    , 9999999 , 2         , "\x5\xc"          },
                {  L_ , 1    , 9999999 , 4         , "\x4\xb\x5\xc"    },
                {  L_ , 2    , 4000000 , 5         , "\x0\x1\x2\x3\x9" },
                {  L_ , 5    , 0       , 3         , "\x7\x8\xa   "    },
                {  L_ , 100  , 9999999 , 0         , ""                }
            };
            const int NUM_POPS   = sizeof POP_DATA / sizeof *POP_DATA;

            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int         LINE   = VALUES[i].d_lineNum;
                const TestString& VAL    = *VALUES[i].d_value;
                const int         SECS   = VALUES[i].d_secs;
                const int         NSECS  = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL); P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();

                bsl::vector<bcec_TimeQueueItem<TestString> > buffer(&ta);

                int newLength = 0;
                bdet_TimeInterval newMinTime;

                mX.popLE(TIME, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    NL();
                    T_(); P_(LINE); P_(EXPNUMITEMS);P(TIME);
                    T_(); P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bdet_TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                        T_(); P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bdet_TimeInterval() == newMinTime);
                }

                if (buffer.size() && EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int         I      = EXPITEMS[j];
                        const TestString& EXPVAL = *VALUES[I].d_value;
                        const bdet_TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_(); T_(); P_(I); P_(EXPVAL); P(EXPTIME);
                            T_(); T_(); P_(buffer[j].time());
                            P(buffer[j].data());
                        }

                        LOOP_ASSERT(j, EXPTIME == buffer[j].time());
                        LOOP_ASSERT(j, EXPVAL  == buffer[j].data());
                        LOOP_ASSERT(j,
                                    true != X.isRegisteredHandle(handles[I]));
                    }
                }
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == defaultAlloc.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); cout << defaultAlloc; }

        if (verbose) cout << "\t'popLE' with maxTimers." << endl;
        {
            static const struct {
                int          d_line;
                int          d_secs;
                int          d_nsecs;
                int          d_maxNumItems;
                int          d_expNumItems;
                const char  *d_expItems;
            } POP_DATA[] = {
                // line secs nsecs     maxNum expNum expItems
                // ---- ---- --------- ------ ------ ----------------
                {  L_ , 1   , 0       , 0   , 0    , ""                },
                {  L_ , 1   , 0       , 1   , 1    , "\x6"             },
                {  L_ , 1   , 0       , 1   , 1    , "\xd"             },
                {  L_ , 1   , 0       , 1   , 0    , ""                },
                {  L_ , 1   , 0       , 20  , 0    , ""                },
                {  L_ , 1   , 9999998 , 1   , 1    , "\x4"             },
                {  L_ , 1   , 9999998 , 20  , 1    , "\xb"             },
                {  L_ , 1   , 9999998 , 1   , 0    , ""                },
                {  L_ , 1   , 9999999 , 20  , 2    , "\x5\xc"          },
                {  L_ , 2   , 1500000 , 5   , 5    , "\x0\x1\x2\x3\x9" },
                {  L_ , 2   , 4000000 , 0   , 0    , ""                },
                {  L_ , 5   , 0       , 1   , 1    , "\x7"             },
                {  L_ , 5   , 0       , 1   , 1    , "\x8"             },
                {  L_ , 5   , 0       , 1   , 1    , "\xa"             },
                {  L_ , 100 , 9999999 , 20  , 0    , ""                },
                {  L_ , 100 , 9999999 , 0   , 0    , ""                }
            };
            const int NUM_POPS   = sizeof POP_DATA / sizeof *POP_DATA;

            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int         LINE   = VALUES[i].d_lineNum;
                const int         SECS   = VALUES[i].d_secs;
                const int         NSECS  = VALUES[i].d_nsecs;
                const TestString& VAL    = *VALUES[i].d_value;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            bsl::vector<bcec_TimeQueueItem<TestString> > buffer(&ta);
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const int   MAXNUMITEMS  = POP_DATA[i].d_maxNumItems;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const bdet_TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();
                const int   OLDSIZE      = buffer.size();

                int newLength = 0;
                bdet_TimeInterval newMinTime;

                mX.popLE(TIME, MAXNUMITEMS, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    NL();
                    T_(); P_(LINE); P_(MAXNUMITEMS); P_(EXPNUMITEMS); P(TIME);
                    T_(); P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, OLDSIZE + EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bdet_TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                       T_(); P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bdet_TimeInterval() == newMinTime);
                }

                if (OLDSIZE + EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int         I      = EXPITEMS[j];
                        const TestString& EXPVAL = *VALUES[I].d_value;
                        const bdet_TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_(); T_(); P_(I); P_(EXPVAL);P(EXPTIME);
                            T_(); T_(); P_(buffer[OLDSIZE + j].time());
                            P(buffer[j].data());
                        }

                        LOOP_ASSERT(j, EXPTIME == buffer[OLDSIZE + j].time());
                        LOOP_ASSERT(j, EXPVAL  == buffer[OLDSIZE + j].data());
                        LOOP_ASSERT(j,
                                    true != X.isRegisteredHandle(handles[I]));
                    }
                }
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == defaultAlloc.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); cout << defaultAlloc; }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TEST 'update' MANIPULATOR
        //
        // Plan:
        //
        // Testing:
        //   int update(int handle, const bdet_TimeInterval &newTime,...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'update' manipulator" << endl
                          << "============================" << endl;

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
                int         d_updsecs;   // Seconds to update to
                int         d_updnsecs;  // Nanoseconds to update to
                int         d_isNewTop;  // Should item be new top after update
            } VALUES[] = {
                //line secs  nsecs    value updsecs updnsecs isNewTop
                //---- ----- -------- ----- ------- -------- --------
                { L_,   2  , 1000000, VA   ,  0    , 1000000, 0 },
                { L_,   2  , 1000000, VB   ,  3    , 1000000, 0 },
                { L_,   2  , 1000000, VC   ,  0    ,    4000, 0 },
                { L_,   2  , 1000001, VB   ,  0    ,    3999, 1 },
                { L_,   1  , 9999998, VC   ,  4    , 9999998, 0 },
                { L_,   1  , 9999999, VD   ,  0    ,       0, 1 },
                { L_,   0  ,    4000, VE   ,  10   ,    4000, 0 }
            };

            static const int POP_ORDER[]={5,3,2,0,1,4,6};

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            {
                Item item(&ta);
                ASSERT(0 != mX.popFront(&item));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE     = VALUES[i].d_lineNum;
                const int   UPDSECS  = VALUES[i].d_updsecs;
                const int   EXPNEWTOP= VALUES[i].d_isNewTop;
                const int   UPDNSECS = VALUES[i].d_updnsecs;
                const bdet_TimeInterval UPDTIME(UPDSECS,UPDNSECS);

                int isNewTop;

                Item item(&ta);
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
                LOOP_ASSERT(LINE, 0 == mX.update(handles[i], UPDTIME,
                                                &isNewTop));
                LOOP_ASSERT(LINE, EXPNEWTOP == isNewTop);
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }


            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE     = VALUES[i].d_lineNum;
                const int   I        = POP_ORDER[i];
                const char *EXPVAL   = VALUES[I].d_value;
                const int   EXPSECS  = VALUES[I].d_updsecs;
                const int   EXPNSECS = VALUES[I].d_updnsecs;
                const bdet_TimeInterval EXPTIME(EXPSECS,EXPNSECS);

                Item item(&ta);
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[I]));
                LOOP_ASSERT(LINE, 0 == mX.popFront(&item));
                LOOP_ASSERT(LINE, EXPTIME == item.time());
                LOOP_ASSERT(LINE, EXPVAL == item.data());
                LOOP_ASSERT(LINE, true != X.isRegisteredHandle(handles[I]));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST 'popLE' MANIPULATOR
        //
        // Plan:
        //
        // Testing:
        //   void popLE(const bdet_TimeInterval& time,...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'popLE' manipulator" << endl
                          << "===========================" << endl;

        const char VA[] = "A";
        const char VB[] = "B";
        const char VC[] = "C";
        const char VD[] = "D";
        const char VE[] = "E";
        const char VF[] = "F";
        const char VG[] = "G";
        const char VH[] = "H";

        static const struct {
            int         d_lineNum;     // Source line number
            int         d_secs;
            int         d_nsecs;
            const char* d_value;
        } VALUES[] = {
            //line secs  nsecs    value
            //---- ----- -------- --------
            { L_,   2  , 1000000, VE     },  // \x0
            { L_,   2  , 1000000, VE     },  // \x1
            { L_,   2  , 1000000, VE     },  // \x2
            { L_,   2  , 1000001, VF     },  // \x3
            { L_,   1  , 9999998, VC     },  // \x4
            { L_,   1  , 9999999, VD     },  // \x5
            { L_,   0  , 0000000, VA     },  // \x6
            { L_,   3  , 1000000, VG     },  // \x7
            { L_,   3  , 1000000, VG     },  // \x8
            { L_,   2  , 1500000, VF     },  // \x9
            { L_,   4  , 1000001, VH     },  // \xa
            { L_,   1  , 9999998, VC     },  // \xb
            { L_,   1  , 9999999, VD     },  // \xc
            { L_,   0  , 0000001, VB     }   // \xd
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        const char *SORTED_VALUES =
                                 "\x6\xd\x4\xb\x5\xc\x0\x1\x2\x3\x9\x7\x8\xa";

        if (verbose) cout << "\t'popLE' without maxTimers." << endl;
        {
            static const struct {
                int          d_line;
                int          d_secs;
                int          d_nsecs;
                int          d_expNumItems;
                const char  *d_expItems;
            } POP_DATA[] = {
                // line secs  nsecs     expNumItems expItems
                // ---- ----- --------- ----------- ----------------
                {  L_ , 1    , 0       , 2         , "\x6\xd"          },
                {  L_ , 1    , 0       , 0         , ""                },
            //  {  L_ , 1    , 9999998 , 2         , "\x4\xb"          },
            //  {  L_ , 1    , 9999999 , 2         , "\x5\xc"          },
                {  L_ , 1    , 9999999 , 4         , "\x4\xb\x5\xc"    },
                {  L_ , 2    , 4000000 , 5         , "\x0\x1\x2\x3\x9" },
                {  L_ , 5    , 0       , 3         , "\x7\x8\xa   "    },
                {  L_ , 100  , 9999999 , 0         , ""                }
            };
            const int NUM_POPS   = sizeof POP_DATA / sizeof *POP_DATA;

            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL); P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();

                bsl::vector<bcec_TimeQueueItem<const char*> > buffer(&ta);

                int newLength = 0;
                bdet_TimeInterval newMinTime;

                mX.popLE(TIME, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    NL();
                    T_(); P_(LINE); P_(EXPNUMITEMS);P(TIME);
                    T_(); P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bdet_TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                        T_(); P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bdet_TimeInterval() == newMinTime);
                }

                if (buffer.size() && EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int   I      = EXPITEMS[j];
                        const char *EXPVAL = VALUES[I].d_value;
                        const bdet_TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_(); T_(); P_(I); P_(EXPVAL);P(EXPTIME);
                            T_(); T_(); P_(buffer[j].time());
                            P(buffer[j].data());
                        }

                        LOOP_ASSERT(j, EXPTIME == buffer[j].time());
                        LOOP_ASSERT(j, EXPVAL  == buffer[j].data());
                        LOOP_ASSERT(j,
                                    true != X.isRegisteredHandle(handles[I]));
                    }
                }
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

        if (verbose) cout << "\t'popLE' with maxTimers." << endl;
        {
            static const struct {
                int          d_line;
                int          d_secs;
                int          d_nsecs;
                int          d_maxNumItems;
                int          d_expNumItems;
                const char  *d_expItems;
            } POP_DATA[] = {
                // line secs nsecs     maxNum expNum expItems
                // ---- ---- --------- ------ ------ ----------------
                {  L_ , 1   , 0       , 0   , 0    , ""                },
                {  L_ , 1   , 0       , 1   , 1    , "\x6"             },
                {  L_ , 1   , 0       , 1   , 1    , "\xd"             },
                {  L_ , 1   , 0       , 1   , 0    , ""                },
                {  L_ , 1   , 0       , 20  , 0    , ""                },
                {  L_ , 1   , 9999998 , 1   , 1    , "\x4"             },
                {  L_ , 1   , 9999998 , 20  , 1    , "\xb"             },
                {  L_ , 1   , 9999998 , 1   , 0    , ""                },
                {  L_ , 1   , 9999999 , 20  , 2    , "\x5\xc"          },
                {  L_ , 2   , 1500000 , 5   , 5    , "\x0\x1\x2\x3\x9" },
                {  L_ , 2   , 4000000 , 0   , 0    , ""                },
                {  L_ , 5   , 0       , 1   , 1    , "\x7"             },
                {  L_ , 5   , 0       , 1   , 1    , "\x8"             },
                {  L_ , 5   , 0       , 1   , 1    , "\xa"             },
                {  L_ , 100 , 9999999 , 20  , 0    , ""                },
                {  L_ , 100 , 9999999 , 0   , 0    , ""                }
            };
            const int NUM_POPS   = sizeof POP_DATA / sizeof *POP_DATA;

            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const char *VAL         = VALUES[i].d_value;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            bsl::vector<bcec_TimeQueueItem<const char*> > buffer(&ta);
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const int   MAXNUMITEMS  = POP_DATA[i].d_maxNumItems;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const bdet_TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();
                const int   OLDSIZE      = buffer.size();

                int newLength = 0;
                bdet_TimeInterval newMinTime;

                mX.popLE(TIME, MAXNUMITEMS, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    NL();
                    T_(); P_(LINE); P_(MAXNUMITEMS); P_(EXPNUMITEMS); P(TIME);
                    T_(); P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, OLDSIZE + EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bdet_TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                       T_(); P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bdet_TimeInterval() == newMinTime);
                }

                if (OLDSIZE + EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int   I      = EXPITEMS[j];
                        const char *EXPVAL = VALUES[I].d_value;
                        const bdet_TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_(); T_(); P_(I); P_(EXPVAL);P(EXPTIME);
                            T_(); T_(); P_(buffer[OLDSIZE + j].time());
                            P(buffer[j].data());
                        }

                        LOOP_ASSERT(j, EXPTIME == buffer[OLDSIZE + j].time());
                        LOOP_ASSERT(j, EXPVAL  == buffer[OLDSIZE + j].data());
                        LOOP_ASSERT(j,
                                    true != X.isRegisteredHandle(handles[I]));
                    }
                }
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TEST 'popFront' MANIPULATORS
        //
        // Plan:
        //
        // Testing:
        //   int popFront(bcec_TimeQueueItem<DATA> *buffer);
        //   int popFront(bcec_TimeQueueItem<DATA> *buffer,...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'popFront' manipulator" << endl
                          << "==============================" << endl;

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   0  , 0000000, VE     }
            };

            static const int POP_ORDER[]={6,4,5,0,1,2,3};

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            {
                Item item(&ta);
                ASSERT(0 != mX.popFront(&item));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   I        = POP_ORDER[i];
                const char *EXPVAL   = VALUES[I].d_value;
                const int   EXPSECS  = VALUES[I].d_secs;
                const int   EXPNSECS = VALUES[I].d_nsecs;
                const bdet_TimeInterval EXPTIME(EXPSECS,EXPNSECS);

                Item item(&ta);
                LOOP_ASSERT(i, true == X.isRegisteredHandle(handles[I]));
                LOOP_ASSERT(i, 0 == mX.popFront(&item));
                LOOP_ASSERT(i, EXPTIME == item.time());
                LOOP_ASSERT(i, EXPVAL == item.data());
                LOOP_ASSERT(i, handles[I] == item.handle());
                LOOP_ASSERT(i, (NUM_VALUES - 1 - i) == X.length());
                LOOP_ASSERT(i, true != X.isRegisteredHandle(handles[I]));
            }

            {
                Item item(&ta);
                ASSERT(0 != mX.popFront(&item));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   0  , 0000000, VE     }
            };

            static const struct {
                int          d_expItem;
                int          d_expMinSecs;
                int          d_expMinNsecs;
            } POP_DATA[] = {
                // expItem expMinSecs expMinNsecs
                // ------- ---------- -----------
                {  6     , 1        , 9999998    },
                {  4     , 1        , 9999999    },
                {  5     , 2        , 1000000    },
                {  0     , 2        , 1000000    },
                {  1     , 2        , 1000000    },
                {  2     , 2        , 1000001    },
                {  3     , 0        , 0          },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            {
                Item item(&ta);
                int newLength;
                bdet_TimeInterval newMinTime;

                ASSERT(0 != mX.popFront(&item, &newLength, &newMinTime));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   I           = POP_DATA[i].d_expItem;
                const char *EXPVAL      = VALUES[I].d_value;
                const int   EXPSECS     = VALUES[I].d_secs;
                const int   EXPNSECS    = VALUES[I].d_nsecs;
                const int   EXPMINSECS  = POP_DATA[i].d_expMinSecs;
                const int   EXPMINNSECS = POP_DATA[i].d_expMinNsecs;

                const bdet_TimeInterval EXPTIME(EXPSECS,EXPNSECS);
                const bdet_TimeInterval EXPMINTIME(EXPMINSECS,EXPMINNSECS);

                int newLength;
                bdet_TimeInterval newMinTime;

                Item item(&ta);
                LOOP_ASSERT(i, true == X.isRegisteredHandle(handles[I]));
                LOOP_ASSERT(i, 0 == mX.popFront(&item, &newLength,
                                               &newMinTime));
                LOOP_ASSERT(i, EXPTIME == item.time());
                LOOP_ASSERT(i, EXPVAL == item.data());
                LOOP_ASSERT(i, handles[I] == item.handle());
                LOOP_ASSERT(i, (NUM_VALUES - 1 - i) == X.length());
                LOOP_ASSERT(i, (NUM_VALUES - 1 - i) == newLength);
                LOOP_ASSERT(i, EXPMINTIME == newMinTime);
                LOOP_ASSERT(i, true != X.isRegisteredHandle(handles[I]));
            }

            {
                Item item(&ta);
                int newLength;
                bdet_TimeInterval newMinTime;

                ASSERT(0 != mX.popFront(&item, &newLength, &newMinTime));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST REMOVEALL MANIPULATOR
        //
        // Plan:
        //   Create and populate a time queue, invoke 'removeAll',
        //   and assert that all the handles are indeed deregistered and that
        //   the time queue is empty.  For the version that gets a copy of the
        //   removed items into a local buffer, assert that the items are as
        //   expected.
        //
        // Testing:
        //   void removeAll(bsl::vector<bcec_TimeQueueItem<DATA> > *buffer=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'removeAll' manipulator" << endl
                          << "===============================" << endl;

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   0  , 0000000, VE     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            ASSERT(NUM_VALUES == X.length());
            mX.removeAll();
            ASSERT(0 == X.length());

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;
                LOOP_ASSERT(LINE, false == X.isRegisteredHandle(handles[i]));
            }
        }

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   0  , 0000000, VE     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            bsl::vector<bcec_TimeQueueItem<const char*> > buffer(&ta);

            ASSERT(NUM_VALUES == X.length());
            mX.removeAll(&buffer);
            ASSERT(0 == X.length());
            ASSERT(NUM_VALUES == (int)buffer.size());

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE   = VALUES[i].d_lineNum;
                const char *VAL    = VALUES[i].d_value;
                const int   SECS   = VALUES[i].d_secs;
                const int   NSECS  = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                LOOP_ASSERT(LINE, TIME == buffer[i].time());
                LOOP_ASSERT(LINE, VAL == buffer[i].data());
                LOOP_ASSERT(LINE, handles[i] == buffer[i].handle());
                LOOP_ASSERT(LINE, true != X.isRegisteredHandle(handles[i]));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST REMOVE MANIPULATORS
        //
        // Plan:
        //
        // Testing:
        //   int remove(int timeId, int *newLength=0,...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'remove' manipulator" << endl
                          << "============================" << endl;

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   0  , 0000000, VE     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                LOOP_ASSERT(LINE, 0 == mX.remove(handles[i]));
                LOOP_ASSERT(LINE, NUM_VALUES-i-1 == X.length());
                LOOP_ASSERT(LINE, false == X.isRegisteredHandle(handles[i]));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
            } VALUES[] = {
                //line secs  nsecs    value
                //---- ----- -------- --------
                { L_,   0  , 0000000, VE     },
                { L_,   1  , 9999998, VC     },
                { L_,   1  , 9999999, VD     },
                { L_,   2  , 1000000, VA     },
                { L_,   2  , 1000000, VB     },
                { L_,   2  , 1000000, VC     },
                { L_,   2  , 1000001, VB     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int handles[NUM_VALUES];

            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bdet_TimeInterval TIME(SECS,NSECS);

                int newLength;
                bcec_TimeQueueItem<const char*> buffer(&ta);
                bdet_TimeInterval newMinTime;

                LOOP_ASSERT(LINE, 0 == mX.remove(handles[i], &newLength,
                                                &newMinTime, &buffer));
                LOOP_ASSERT(LINE, NUM_VALUES-i-1 == newLength);
                LOOP_ASSERT(LINE, NUM_VALUES-i-1 == X.length());
                LOOP_ASSERT(LINE, TIME == buffer.time());
                LOOP_ASSERT(LINE, VAL == buffer.data());
                LOOP_ASSERT(LINE, handles[i] == buffer.handle());
                LOOP_ASSERT(LINE, true != X.isRegisteredHandle(handles[i]));
                if (i < NUM_VALUES-1) {
                    const bdet_TimeInterval NEWMINTIME(VALUES[i+1].d_secs,
                                                       VALUES[i+1].d_nsecs);
                    LOOP_ASSERT(LINE, NEWMINTIME == newMinTime);
                }

            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST PRIMARY MANIPULATORS/ACCESSORS
        //
        // Plan:
        //
        // Testing:
        //   bcec_TimeQueue(bslma_Allocator *allocator=0);
        //   ~bcec_TimeQueue();
        //   void* add(const bdet_TimeInterval& time, const DATA& data, ...
        //   int length() const;
        //   bool isRegisteredHandle(int handle) const;
        //   int minTime(bdet_TimeInterval *buffer);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing primary manipulators and accessors" << endl
                 << "==========================================" << endl;

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
                int         d_isNewTop;
            } VALUES[] = {
                //line secs  nsecs    value    isNewTop
                //---- ----- -------- -------- --------
                { L_,   2  , 1000000, VA     , 1       },
                { L_,   2  , 1000000, VB     , 0       },
                { L_,   2  , 1000000, VC     , 0       },
                { L_,   2  , 1000001, VB     , 0       },
                { L_,   1  , 9999998, VC     , 1       },
                { L_,   1  , 9999999, VD     , 0       },
                { L_,   1  , 9999999, VE     , 0       },
                { L_,   1  , 9999999, VC     , 0       },
                { L_,   0  , 0000000, VE     , 1       }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            Obj mX(&ta); const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const int   ISNEWTOP    = VALUES[i].d_isNewTop;
                const bdet_TimeInterval TIME(SECS,NSECS);

                int isNewTop;
                bdet_TimeInterval newMinTime;
                int newLength;
                int  handle;
                handle = mX.add(TIME, VAL, &isNewTop, &newLength);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(ISNEWTOP);
                    T_();  P_(isNewTop); P(newMinTime); P_(newLength);
                    P(X.length());
                }
                LOOP_ASSERT(LINE, ISNEWTOP == isNewTop);
                LOOP_ASSERT(LINE, (i+1) == newLength);
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handle));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

        {
            const char VA[] = "A";
            const char VB[] = "B";
            const char VC[] = "C";
            const char VD[] = "D";
            const char VE[] = "E";

            static const struct {
                int         d_lineNum;     // Source line number
                int         d_secs;
                int         d_nsecs;
                const char* d_value;
                int         d_isNewTop;
            } VALUES[] = {
                //line secs  nsecs    value    isNewTop
                //---- ----- -------- -------- --------
                { L_,   2  , 1000000, VA     , 1       },
                { L_,   2  , 1000001, VB     , 0       },
                { L_,   1  , 9999998, VC     , 1       },
                { L_,   1  , 9999999, VD     , 0       },
                { L_,   0  , 0000000, VE     , 1       }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            Obj mX(&ta);  const Obj& X = mX;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const int   ISNEWTOP    = VALUES[i].d_isNewTop;
                const bdet_TimeInterval TIME(SECS,NSECS);

                int isNewTop;
                int newLength;

                int handle;
                handle = mX.add(TIME, VAL, &isNewTop, &newLength);
                if (veryVerbose) {
                    T_(); P_(LINE); P_(VAL);P_(TIME); P(ISNEWTOP);
                    T_();  P_(isNewTop); P_(newLength); P(X.length());
                }
                LOOP_ASSERT(LINE, ISNEWTOP == isNewTop);
                LOOP_ASSERT(LINE, (i+1) == newLength);
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handle));
            }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST CLASS 'bcec_TimeQueueItem'
        //
        // Plan:
        //   Create various time queue items using the different constructors.
        //   Exercise the manipulators and accessors in isolation.  Verify that
        //   the allocator is correctly passed to the data argument.
        //
        // Testing:
        //   bcec_TimeQueueItem(...
        //   bcec_TimeQueueItem::time();
        //   bcec_TimeQueueItem::data();
        //   bcec_TimeQueueItem::handle();
        //   bcec_TimeQueueItem::key();
        //   bcec_TimeQueueItem::time() const;
        //   bcec_TimeQueueItem::data() const;
        //   bcec_TimeQueueItem::handle() const;
        //   bcec_TimeQueueItem::key() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CLASS 'bcec_TimeQueueItem'" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\tWithout allocators.\n";
        {
            typedef bcec_TimeQueueItem<const char*> Obj;
            const bdet_TimeInterval  TIME(1);
            const char              *STRDATA = "Unused";
            const int                HANDLE = 0xffff;
            const Obj::Key           KEY((void*)&HANDLE);

            // CREATORS, ACCESSORS
            if (verbose) cout << "\t\tDefault constructor.\n";
            Obj mX;  const Obj& X = mX;
            ASSERT(bdet_TimeInterval() == X.time());
            ASSERT(NULL                == X.data());
            // ASSERT(0                   == X.handle());
            ASSERT(Obj::Key(0)         == X.key());

            if (verbose) cout << "\t\tConstructor without key.\n";
            Obj mY(TIME, STRDATA, HANDLE);  const Obj& Y = mY;
            ASSERT(TIME                == Y.time());
            ASSERT(STRDATA             == Y.data());
            ASSERT(HANDLE              == Y.handle());
            ASSERT(Obj::Key(0)         == Y.key());

            if (verbose) cout << "\t\tConstructor with key.\n";
            Obj mZ(TIME, STRDATA, HANDLE, KEY);  const Obj& Z = mZ;
            ASSERT(TIME                == Z.time());
            ASSERT(STRDATA             == Z.data());
            ASSERT(HANDLE              == Z.handle());
            ASSERT(KEY                 == Z.key());

            if (verbose) cout << "\t\tCopy constructor.\n";
            Obj mU(Z);  const Obj& U = mU;
            ASSERT(TIME                == U.time());
            ASSERT(STRDATA             == U.data());
            ASSERT(HANDLE              == U.handle());
            ASSERT(KEY                 == U.key());

            if (verbose) cout << "\t\tCopy assignment.\n";
            Obj mV;  const Obj& V = mV;
            mV = U;
            ASSERT(TIME                == V.time());
            ASSERT(STRDATA             == V.data());
            ASSERT(HANDLE              == V.handle());
            ASSERT(KEY                 == V.key());

            // MANIPULATORS
            mX.time() = TIME;
            ASSERT(TIME == X.time());

            mX.data() = STRDATA;
            ASSERT(STRDATA == X.data());

            mX.handle() = HANDLE;
            ASSERT(HANDLE == X.handle());

            mX.key() = KEY;
            ASSERT(KEY == X.key());
        }

        if (verbose) cout << "\tWith allocators.\n";
        {
            bcema_TestAllocator ta2(veryVeryVerbose);

            typedef bcec_TimeQueueItem<TestString> Obj;
            const bdet_TimeInterval  TIME(1);
            const TestString         STRDATA("Unused", &ta2);
            const TestString         STRDATA2("Another unused", &ta2);
            const int                HANDLE = 0xffff;
            const Obj::Key           KEY((void*)&HANDLE);

            const int NUM_ALLOC2 = ta2.numAllocations();

            // CREATORS, ACCESSORS
            if (verbose) cout << "\t\tDefault constructor.\n";
            Obj mX(&ta);  const Obj& X = mX;
            {
                const int NUM_ALLOC = ta.numAllocations();
                ASSERT(bdet_TimeInterval() == X.time());
                ASSERT(TestString()        == X.data());
                // ASSERT(0                   == X.handle());
                ASSERT(Obj::Key(0)         == X.key());
                ASSERT(NUM_ALLOC           == ta.numAllocations());
                ASSERT(NUM_ALLOC2          == ta2.numAllocations());
            }

            if (verbose) cout << "\t\tConstructor without key.\n";
            Obj mY(TIME, STRDATA, HANDLE, &ta);  const Obj& Y = mY;
            {
                const int NUM_ALLOC = ta.numAllocations();
                ASSERT(TIME                == Y.time());
                ASSERT(STRDATA             == Y.data());
                ASSERT(HANDLE              == Y.handle());
                ASSERT(Obj::Key(0)         == Y.key());
                ASSERT(NUM_ALLOC           == ta.numAllocations());
                ASSERT(NUM_ALLOC2          == ta2.numAllocations());
            }

            if (verbose) cout << "\t\tConstructor with key.\n";
            Obj mZ(TIME, STRDATA, HANDLE, KEY, &ta);  const Obj& Z = mZ;
            {
                const int NUM_ALLOC = ta.numAllocations();
                ASSERT(TIME                == Z.time());
                ASSERT(STRDATA             == Z.data());
                ASSERT(HANDLE              == Z.handle());
                ASSERT(KEY                 == Z.key());
                ASSERT(NUM_ALLOC           == ta.numAllocations());
                ASSERT(NUM_ALLOC2          == ta2.numAllocations());
            }

            bcema_TestAllocator ta3(veryVeryVerbose);
            const int NUM_ALLOC = ta.numAllocations();

            if (verbose) cout << "\t\tCopy constructor.\n";
            Obj mU(Z, &ta3);  const Obj& U = mU;
            ASSERT(TIME                    == U.time());
            ASSERT(STRDATA                 == U.data());
            ASSERT(HANDLE                  == U.handle());
            ASSERT(KEY                     == U.key());
            ASSERT(NUM_ALLOC               == ta.numAllocations());
            ASSERT(NUM_ALLOC2              == ta2.numAllocations());

            if (verbose) cout << "\t\tCopy assignment.\n";
            Obj mV(&ta3);  const Obj& V = mV;
            mV = U;
            ASSERT(TIME                    == V.time());
            ASSERT(STRDATA                 == V.data());
            ASSERT(HANDLE                  == V.handle());
            ASSERT(KEY                     == V.key());
            ASSERT(NUM_ALLOC               == ta.numAllocations());
            ASSERT(NUM_ALLOC2              == ta2.numAllocations());
            if (veryVeryVerbose) { P(ta3); }

            // MANIPULATORS
            mX.time() = TIME;
            ASSERT(TIME == X.time());

            const int NUM_ALLOC3 = ta3.numAllocations();
            mU.data() = STRDATA2;
            ASSERT(STRDATA2 == U.data());
            ASSERT(NUM_ALLOC  == ta.numAllocations());
            ASSERT(NUM_ALLOC2 == ta2.numAllocations());
            ASSERT(NUM_ALLOC3 <  ta3.numAllocations());

            mX.handle() = HANDLE;
            ASSERT(HANDLE == X.handle());

            mX.key() = KEY;
            ASSERT(KEY == X.key());

            ASSERT(NUM_ALLOC2 == ta2.numAllocations());
            if (veryVeryVerbose) { P(ta2); }
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            Obj x1(&ta);
            const bdet_TimeInterval TA = bdetu_SystemTime::now();
            const bdet_TimeInterval TB = TA + 1;
            const bdet_TimeInterval TC = TB + 1;
            const bdet_TimeInterval TD = TC + 1;
            const bdet_TimeInterval TE = TD + 1;

            const char* VA= "hello";
            const char* VB= "world,";
            const char* VC= "how";
            const char* VD= "are";
            const char* VE= "you";


            int HA = x1.add(TA, VA);
            int HB = x1.add(TB, VB);
            int HC = x1.add(TC, VC);
            int HD = x1.add(TD, VD);
            int HE = x1.add(TE, VE);

            bcec_TimeQueueItem<const char*> tItem(&ta);
            bdet_TimeInterval newMinTime;
            int newLength;

            ASSERT(0  == x1.popFront(&tItem));
            if (verbose) cout << tItem.time() << " " << tItem.data() << endl;
            ASSERT(VA == tItem.data());
            ASSERT(TA == tItem.time());
            ASSERT(HA == tItem.handle());


            ASSERT(0  == x1.popFront(&tItem, &newLength, &newMinTime));
            ASSERT(3  == newLength);
            ASSERT(TC == newMinTime);
            ASSERT(TB == tItem.time());
            ASSERT(VB == tItem.data());
            ASSERT(HB == tItem.handle());

            bsl::vector<bcec_TimeQueueItem<const char*> > a1(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A1 = a1;

            x1.popLE(TD, &a1, &newLength, &newMinTime);
            ASSERT(2  == A1.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);
            ASSERT(TC == A1[0].time());
            ASSERT(VC == A1[0].data());
            ASSERT(HC == A1[0].handle());
            ASSERT(TD == A1[1].time());
            ASSERT(VD == A1[1].data());
            ASSERT(HD == A1[1].handle());

            bsl::vector<bcec_TimeQueueItem<const char*> > a2(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A2 = a2;

            x1.popLE(TD, &a2, &newLength, &newMinTime);
            ASSERT(0  == A2.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);

            bsl::vector<bcec_TimeQueueItem<const char*> > a3(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A3 = a3;

            x1.popLE(TE, &a3, &newLength, &newMinTime);
            ASSERT(1  == A3.size());
            ASSERT(0  == newLength);
            ASSERT(TE == A3[0].time());
            ASSERT(VE == A3[0].data());
            ASSERT(HE == A3[0].handle());
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

        {
            Obj x1(&ta);
            const bdet_TimeInterval TA = bdetu_SystemTime::now();
            const bdet_TimeInterval TB = TA + 1;
            const bdet_TimeInterval TC = TB + 1;
            const bdet_TimeInterval TD = TC + 1;
            const bdet_TimeInterval TE = TD + 1;

            const char* VA= "hello";
            const char* VB= "world,";
            const char* VC= "how";
            const char* VD= "are";
            const char* VE= "you";

            typedef Obj::Key Key;

            const Key KA = Key(&TA);
            const Key KB = Key(&TB);
            const Key KC = Key(382);
            const Key KD = Key(123);
            const Key KE = Key(&VE);

            int HA = x1.add(TA, VA, KA);
            int HB = x1.add(TB, VB, KB);
            int HC = x1.add(TC, VC, KC);
            int HD = x1.add(TD, VD, KD);
            int HE = x1.add(TE, VE, KE);

            ASSERT(0 != x1.remove(HA, KB));
            ASSERT(x1.isRegisteredHandle(HA, KA));
            ASSERT(0 != x1.update(HC, KD, TE));

            bcec_TimeQueueItem<const char*> tItem(&ta);
            bdet_TimeInterval newMinTime;
            int newLength;

            ASSERT(0  == x1.popFront(&tItem));
            if (verbose) cout << tItem.time() << " " << tItem.data() << endl;
            ASSERT(VA == tItem.data());
            ASSERT(TA == tItem.time());
            ASSERT(HA == tItem.handle());
            ASSERT(KA == tItem.key());


            ASSERT(0  == x1.popFront(&tItem, &newLength, &newMinTime));
            ASSERT(3  == newLength);
            ASSERT(TC == newMinTime);
            ASSERT(TB == tItem.time());
            ASSERT(VB == tItem.data());
            ASSERT(HB == tItem.handle());
            ASSERT(KB == tItem.key());

            bsl::vector<bcec_TimeQueueItem<const char*> > a1(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A1 = a1;

            x1.popLE(TD, &a1, &newLength, &newMinTime);
            ASSERT(2  == A1.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);
            ASSERT(TC == A1[0].time());
            ASSERT(VC == A1[0].data());
            ASSERT(HC == A1[0].handle());
            ASSERT(KC == A1[0].key());
            ASSERT(TD == A1[1].time());
            ASSERT(VD == A1[1].data());
            ASSERT(HD == A1[1].handle());
            ASSERT(KD == A1[1].key());

            bsl::vector<bcec_TimeQueueItem<const char*> > a2(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A2 = a2;

            x1.popLE(TD, &a2, &newLength, &newMinTime);
            ASSERT(0  == A2.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);

            bsl::vector<bcec_TimeQueueItem<const char*> > a3(&ta);
            const bsl::vector<bcec_TimeQueueItem<const char*> > &A3 = a3;

            x1.popLE(TE, &a3, &newLength, &newMinTime);
            ASSERT(1  == A3.size());
            ASSERT(0  == newLength);
            ASSERT(TE == A3[0].time());
            ASSERT(VE == A3[0].data());
            ASSERT(HE == A3[0].handle());
            ASSERT(KE == A3[0].key());
        }
        ASSERT(0 == defaultAlloc.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        BCEC_TIMEQUEUE_TEST_CASE_MINUS_100::run();
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
