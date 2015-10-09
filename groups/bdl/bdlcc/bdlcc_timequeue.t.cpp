// bdlcc_timequeue.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_timequeue.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_lockguard.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>

#include <bdlf_bind.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_ctime.h>
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
// is a time value and the data a 'DATA' template parameter.  Items are order
// by time value within the key.  Items that have the same time are stored in a
// circular linked list.  Thus it is particularly important to test with items
// having the same time value and try and splitting the circular lists at
// various places.  We test all functions in isolation.
//
// Note that this component is not tested, indeed not designed, with respect to
// exception safety.  (The constructor of the 'DATA' could throw.)  The
// component-level documentation should also be updated to explicitly state the
// guarantees.  Memory allocation, however, is well documented and tested in
// case 8.  Concurrent access (a concern due to the reuse of nodes in the free
// list) is also a concern and tested in case 11.
//-----------------------------------------------------------------------------
// [3 ] bdlcc::TimeQueue(bslma::Allocator *allocator=0);
// [3 ] ~bdlcc::TimeQueue();
// [5 ] int popFront(bdlcc::TimeQueueItem<DATA> *buffer);
// [6 ] int popFront(bdlcc::TimeQueueItem<DATA> *buffer,...
// [7 ] void popLE(const bsls::TimeInterval& time,...
// [4 ] int remove(Handle timeId, bsls::TimeInterval *newMinTime=0,...
// [5 ] void removeAll(bsl::vector<bdlcc::TimeQueueItem<DATA> > *buf=0);
// [3 ] int add(const bsls::TimeInterval& time, const DATA& data, ...
// [3 ] int add(const bdlcc::TimeQueueItem<DATA> &item, int *isNewTop=0...
// [8 ] int update(int handle, const bsls::TimeInterval &newTime,...
// [3 ] int length() const;
// [3 ] bool isRegisteredHandle(int handle) const;
// [3 ] int minTime(bsls::TimeInterval *buffer);
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
// [2 ] CLASS 'bdlcc::TimeQueueItem'
// [9 ] CONCERN: Respect the 'bdema' allocator model
// [10] CONCERN: Callbacks and 'DATA' destructors invoked while holding lock
// [11] CONCURRENCY TEST
// [12] CONCERN: The queue can be used after a call to 'drain'.
// [13] CONCERN: Memory Pooling
// [14] Usage example

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

bslmt::Mutex coutMutex;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bdlcc::TimeQueue<const char*> Obj;
typedef bdlcc::TimeQueueItem<const char*> Item;

                              // ================
                              // class TestString
                              // ================

class TestString {
    // This class is a string with allocation, except that a
    // default-constructed 'TestString' is guaranteed *not* to trigger an
    // allocation.  This class is used in test cases 9 and 11.

    bslma::Allocator *d_allocator_p;  // held, not owned
    bsl::string      *d_string_p;     // owned

  private:
    // Not implemented:
    TestString(const TestString&);

  public:
    // TYPES
    BSLALG_DECLARE_NESTED_TRAITS(TestString,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit TestString(bslma::Allocator *allocator = 0);
    explicit TestString(const char *s, bslma::Allocator *allocator = 0);
    explicit TestString(const bsl::string& s, bslma::Allocator *allocator = 0);
        // Create a string, optionally initialized with the optionally
        // specified 's', using the optionally specified 'allocator' to supply
        // memory.  If allocator is null, the currently-installed default
        // allocator is used.

    ~TestString();
        // Destroy this string object.

    // MANIPULATORS
    TestString& operator=(const TestString& rhs);
    TestString& operator=(const bsl::string& rhs);
        // Assign the string 's' to this instance.

    // ACCESSORS
    operator const bsl::string& () const;
        // Return the string value of this object.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& os, const TestString& string);
    // Output the specified 'string' to the specified 'stream'.

bool operator==(const TestString& lhs, const TestString& rhs);
    // Return 0 whether the specified strings 's1' and 's2' hold the same C++
    // string and 1 if not.

bool operator!=(const TestString& lhs, const TestString& rhs);
    // Return 0 whether the specified strings 's1' and 's2' do not hold the
    // same C++ string and 1 if they do.

                              // ----------------
                              // class TestString
                              // ----------------

// CREATORS
TestString::TestString(bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_string_p(0)
{
}

TestString::TestString(const char *s, bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
{
    d_string_p = new(*d_allocator_p) bsl::string(s, d_allocator_p);
}

TestString::TestString(const bsl::string& s, bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
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
TestString& TestString::operator=(const TestString& rhs)
{
    if (d_string_p) {
        d_allocator_p->deleteObjectRaw(d_string_p);
        d_string_p = 0;
    }
    if (rhs.d_string_p) {
        d_string_p = new(*d_allocator_p) bsl::string(*rhs.d_string_p,
                                                     d_allocator_p);
    }
    return *this;
}

TestString& TestString::operator=(const bsl::string& rhs)
{
    if (d_string_p) {
        d_allocator_p->deleteObjectRaw(d_string_p);
        d_string_p = 0;
    }
    d_string_p = new(*d_allocator_p) bsl::string(rhs, d_allocator_p);
    return *this;
}

// ACCESSORS
TestString::operator const bsl::string&() const
{
    if (d_string_p) {
        return *d_string_p;                                           // RETURN
    }
    static bsl::string s_emptyString(d_allocator_p);
    return s_emptyString;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& os, const TestString& string)
{
    return os << (const bsl::string&)string;
}

bool operator==(const TestString& lhs, const TestString& rhs)
{
    return (const bsl::string&) lhs == (const bsl::string&) rhs;
}

bool operator!=(const TestString& lhs, const TestString& rhs)
{
    return !(lhs == rhs);
}

namespace BloombergLP {
namespace bslma {

bsl::ostream& operator<<(bsl::ostream& out, const TestAllocator& ta)
{
    ta.print();
    return out;
}
}  // close namespace bslma
}  // close enterprise namespace

// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMEQUEUE_TEST_CASE_11 {

enum {
    k_NUM_THREADS    = 10,
    k_NUM_ITERATIONS = 1000,                 // per thread
    k_NUM_REMOVE_ALL = k_NUM_ITERATIONS / 2  // between two 'removeAll'
};

typedef bsl::string DATA;
typedef bdlcc::TimeQueue<DATA> TimeQueue;
typedef bdlcc::TimeQueueItem<DATA> TimeQueueItem;

bslma::TestAllocator ta(veryVeryVerbose);
TimeQueue timequeue(&ta);

bslmt::Barrier barrier(k_NUM_THREADS + 1);

struct Case11ThreadInfo {
    int                         d_id;
    bsl::vector<TimeQueueItem> *d_items_p;
};

extern "C" {

void *testAddUpdatePopRemoveAll(void *arg)
    // Invoke 'add', 'update', 'popFront', 'popLE', and/or 'removeAll' in a
    // loop.
{
    Case11ThreadInfo *info = (Case11ThreadInfo*)arg;
    const int THREAD_ID = info->d_id;
    bsl::vector<TimeQueueItem> *vPtr = info->d_items_p;

    // We stagger the removeAll steps among the threads.
    const int STEP_REMOVE_ALL = THREAD_ID * k_NUM_REMOVE_ALL / k_NUM_THREADS;

    bsl::ostringstream oss;
    oss << THREAD_ID;
    DATA V(oss.str());
    if (verbose) {
        coutMutex.lock();
        T_; P_(THREAD_ID); Q(CREATION); P(V);
        coutMutex.unlock(); }

    barrier.wait();

    int newLen;
   bsls::TimeInterval newMinTime;
    TimeQueueItem item;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        if (veryVerbose) {
            coutMutex.lock();
            T_; P_(THREAD_ID); Q(ITERATION); P(i);
            coutMutex.unlock();
        }
        const bsls::TimeInterval TIME((i * (i + 3)) % k_NUM_ITERATIONS);
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
        if (i % k_NUM_REMOVE_ALL == STEP_REMOVE_ALL) {
            timequeue.removeAll(vPtr);
            if (veryVerbose) {
                coutMutex.lock();
                T_; P_(THREAD_ID); Q(REMOVE_ALL); P(i);
                coutMutex.unlock();
            }
        }
    }
    return NULL;
}

void *testLength(void *)
    // Invoke 'length' in a loop.
{
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        int len = timequeue.length();
        LOOP2_ASSERT(i, len, len >= 0);
        LOOP2_ASSERT(i, len, len <= k_NUM_THREADS);
    }
    return NULL;
}

} // extern "C"

}  // close namespace TIMEQUEUE_TEST_CASE_11
// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMEQUEUE_TEST_CASE_10 {

                            // ====================
                            // class TestLockObject
                            // ====================

class TestLockObject {
    // This small test object holds a time queue reference, and attempts to
    // access one of its blocking functions upon destruction.

    // PRIVATE DATA MEMBERS
    const bdlcc::TimeQueue<TestLockObject> *d_timeQueue_p; // held, not owned
    int                                  *d_numDestructions_p;
    int                                   d_verbose;

  public:
    // CREATORS
    explicit
    TestLockObject(const bdlcc::TimeQueue<TestLockObject> *queue = 0,
                   int                                    *numDestructions = 0,
                   int                                     verbose = 0);
        // Create a test object that holds a reference to the optionally
        // specified 'queue'.

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
                       const bdlcc::TimeQueue<TestLockObject> *queue,
                       int                                    *numDestructions,
                       int                                     verbose)
: d_timeQueue_p(queue)
, d_numDestructions_p(numDestructions)
, d_verbose(verbose)
{
}

TestLockObject::~TestLockObject()
{
    if (d_timeQueue_p) {
        bsls::TimeInterval buffer;
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

}  // close namespace TIMEQUEUE_TEST_CASE_10

// ============================================================================
//                        CASE -100 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMEQUEUE_TEST_CASE_MINUS_100 {

static bsls::AtomicInt currentTime(0);

enum {
    k_NUM_THREADS    = 4,
    k_NUM_ITERATIONS = 16,
    k_SEND_COUNT = 1000,
    k_RCV_COUNT = 800,
    k_DELAY = 500,
    k_BITS_PER_HANDLE = 22
};

void threadFunc(bdlcc::TimeQueue<int> *timeQueue,
                int                    numIterations,
                int                    sendCount,
                int                    receiveCount,
                int                    delay)
{
    bsl::vector<int> timers;
    timers.resize(sendCount);

    bsl::vector<bdlcc::TimeQueueItem<int> > resubmit;
    bsls::Stopwatch sw;

    for (int i=0; i<numIterations; i++) {
        if ( verbose ) {
            sw.start();
        }

        // "send" messages
        for (int snd=0; snd<sendCount; snd++) {
            currentTime++;
            bsls::TimeInterval t(currentTime + delay, 0);
            timers[snd] = timeQueue->add(t, delay);
        }

        // "receive" replies
        for (int rcv=0; rcv<receiveCount; rcv++) {
            timeQueue->remove(timers[rcv]);
        }

        // "resend" replies
        bsls::TimeInterval now(currentTime, 0);
        timeQueue->popLE(now, &resubmit);
        int numResubmitted = static_cast<int>(resubmit.size());
        for (int retry=0; retry<numResubmitted; retry++) {
            int newdelay = resubmit[retry].data() * 2;
            bsls::TimeInterval t(currentTime + newdelay, 0);
            timeQueue->add(t, newdelay);
        }

        if (verbose) {
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

    bdlcc::TimeQueue<int> timeQueue(k_BITS_PER_HANDLE);

    bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                 bdlf::BindUtil::bind(&threadFunc,
                                                     &timeQueue,
                                                     (int)k_NUM_ITERATIONS,
                                                     (int)k_SEND_COUNT,
                                                     (int)k_RCV_COUNT,
                                                     (int)k_DELAY));
    }

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

}

}  // close namespace TIMEQUEUE_TEST_CASE_MINUS_100

// ============================================================================
//       USAGE EXAMPLE from header (with assert replaced with ASSERT)
// ----------------------------------------------------------------------------

namespace TIMEQUEUE_USAGE_EXAMPLE {

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

    void *my_connectionMonitorThreadEntry(void *server);

    void *my_timerMonitorThreadEntry(void *server);

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
        // Concrete implementations in the "real world" would typically manage
        // an external connection like a socket.

      public:
        my_Session();
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

        bsl::vector<my_Connection*>      d_connections;
        bdlcc::TimeQueue<my_Connection*> d_timeQueue;
        int                              d_ioTimeout;
        bslmt::Mutex                     d_timerMonitorMutex;
        bslmt::Condition                 d_timerChangedCond;
        bslmt::ThreadUtil::Handle        d_connectionThreadHandle;
        bslmt::ThreadUtil::Handle        d_timerThreadHandle;
        volatile bool                    d_done;

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
            // Behavior is undefined if 'connection' has already been added to
            // any 'my_Server' and has not been removed via member function
            // 'closeConnection'.

        void removeConnection(my_Connection *connection);
            // Remove the specified 'connection' from the current 'my_Server',
            // so that it will no longer be monitored for available data.

        virtual void closeConnection(my_Connection *connection)=0;
            // Provide a mechanism for a concrete implementation to close a
            // specified 'connection'.

        void dataAvailable(my_Connection *connection,
                           void          *buffer_p,
                           int            length);
            // Receive in the specified 'buffer_p' a pointer to a data buffer
            // of the specified 'length' bytes, and pass this to the specified
            // 'connection' to be processed.  Behavior is undefined if
            // 'connection' is not currently added to this 'my_Server' object,
            // or if 'length' <= 0.

      protected:
        virtual void monitorConnections()=0;
            // Monitor all connections in the current 'my_Server'.  When data
            // becomes available for a given connection, pass the data to that
            // connection for processing.

        void monitorTimers();
            // Monitor all timers in the current 'my_Server', and handle each
            // timer as it expires.

        friend void *my_connectionMonitorThreadEntry(void *server);
        friend void *my_timerMonitorThreadEntry(void *server);

      private:
        // Not implemented:
        my_Server(const my_Server&);

      public:
        // CREATORS
        explicit
        my_Server(int ioTimeout, bslma::Allocator *basicAllocator = 0);
            // Construct a 'my_Server' object with a timeout value of the
            // specified 'ioTimeout' seconds.  Use the optionally specified
            // 'basicAllocator' for all memory allocation for data members of
            // 'my_Server'.

        virtual ~my_Server();

        // MANIPULATORS
        int start();
            // Begin monitoring timers and connections.
    };
//..
// The constructor is simple: it initializes the internal 'bdlcc::TimeQueue'
// and sets the I/O timeout value.  The virtual destructor does nothing.
//..
    my_Server::my_Server(int ioTimeout, bslma::Allocator *basicAllocator)
    : d_timeQueue(basicAllocator)
    , d_ioTimeout(ioTimeout)
    , d_connectionThreadHandle(bslmt::ThreadUtil::invalidHandle())
    , d_timerThreadHandle(bslmt::ThreadUtil::invalidHandle())
    {
    }

    my_Server::~my_Server()
    {
        d_done = true;
        d_timerChangedCond.broadcast();
        if (bslmt::ThreadUtil::invalidHandle() != d_connectionThreadHandle) {
            bslmt::ThreadUtil::join(d_connectionThreadHandle);
        }
        if (bslmt::ThreadUtil::invalidHandle()!= d_timerThreadHandle) {
            bslmt::ThreadUtil::join(d_timerThreadHandle);
        }
    }
//..
// Member function 'newConnection' adds the 'connection' to the current set of
// connections to be monitored.  This is done in two steps.  First, the
// 'connection' is added to the internal array, and then a timer is set for the
// 'connection' by creating a corresponding entry in the internal
// 'bdlcc::TimeQueue'.
//..
    void my_Server::newConnection(my_Connection *connection)
    {
        d_connections.push_back(connection);
        int isNewTop = 0;
        connection->d_timerId = d_timeQueue.add(bdlt::CurrentTime::now() +
                                                                   d_ioTimeout,
                                                connection,
                                                &isNewTop);
        if (isNewTop) {
            bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
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
                                  void          *buffer_p,
                                  int            length)
    {
        if (connection->d_timerId) {
            if (d_timeQueue.remove(connection->d_timerId))  return;   // RETURN
            connection->d_timerId = 0;
        }
        connection->d_session_p->processData(buffer_p, length);

        int isNewTop = 0;

        connection->d_timerId = d_timeQueue.add(bdlt::CurrentTime::now() +
                                                                   d_ioTimeout,
                                                connection,
                                                &isNewTop);
        if (isNewTop) {
            bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
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
        while (!d_done) {
            bsl::vector<bdlcc::TimeQueueItem<my_Connection*> > expiredTimers;
            {
                bslmt::LockGuard<bslmt::Mutex> lock(&d_timerMonitorMutex);
                bsls::TimeInterval minTime;
                int newLength;

                d_timeQueue.popLE(bdlt::CurrentTime::now(),
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

            int length = static_cast<int>(expiredTimers.size());
            if (length) {
                bdlcc::TimeQueueItem<my_Connection*> *data =
                                                        &expiredTimers.front();
                for (int i = 0; i < length; ++i) {
                    closeConnection(data[i].data());
                }
            }
        }
    }
//..
// Function 'start' spawns two separate threads.  The first thread will monitor
// connections and handle any data received on them.  The second monitors the
// internal timer queue and removes connections that have timed out.  Function
// 'start' calls 'bslmt::ThreadUtil::create', which expects a function pointer
// to a function with the standard "C" callback signature
// 'void *fn(void *data)'.  This non-member function will call back into the
// 'my_Server' object immediately.
//..
    int my_Server::start()
    {
        bslmt::ThreadAttributes attr;

        if (bslmt::ThreadUtil::create(&d_connectionThreadHandle, attr,
                                     &my_connectionMonitorThreadEntry,
                                     this)) {
            return -1;                                                // RETURN
        }

        if (bslmt::ThreadUtil::create(&d_timerThreadHandle, attr,
                                     &my_timerMonitorThreadEntry,
                                     this)) {
            return -1;                                                // RETURN
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
// In order to test our server, we provide two concrete implementations of a
// test session and of a test server as follows.
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
        explicit
        my_TestSession(int verbose) : my_Session(), d_verbose(verbose) { }

        // MANIPULATORS
        virtual int handleTimeout(my_Connection *connection)
        {
            // Do something to handle timeout.
            if (d_verbose) {
                bsl::cout << bdlt::CurrentTime::utc() << ": ";
                bsl::cout << "Connection " << connection << "timed out.\n";
            }
            return 0;
        }

        virtual int processData(void *data, int length)
        {
            // Do something with the data...
            if (d_verbose) {
                bsl::cout << bdlt::CurrentTime::utc() << ": ";
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
            // Close the specified external 'connection' and call
            // 'removeConnection' when done.

        virtual void monitorConnections();
            // Monitor all connections in the current 'my_Server'.  When data
            // becomes available for a given connection, pass the data to that
            // connection for processing.

      private:
        // Not implemented:
        my_TestServer(const my_TestServer&);

      public:
        // CREATORS
        explicit
        my_TestServer(int               ioTimeout,
                      int               verbose = 0,
                      bslma::Allocator *basicAllocator = 0)
        : my_Server(ioTimeout, basicAllocator)
        , d_verbose(verbose)
        {
        }

        virtual ~my_TestServer();
    };

    // myTestSession.cpp             -*-C++-*-

    my_TestServer::~my_TestServer()
    {
    }

    void my_TestServer::closeConnection(my_Connection *connection)
    {
        if (d_verbose) {
            bsl::cout << bdlt::CurrentTime::utc() << ": ";
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
            bsl::cout << bdlt::CurrentTime::utc() << ": ";
            bsl::cout << "Opening connection " << connection1 << endl;
        }

        my_Connection *connection2 = new my_Connection;
        connection2->d_session_p = session;
        newConnection(connection2);
        if (d_verbose) {
            bsl::cout << bdlt::CurrentTime::utc() << ": ";
            bsl::cout << "Opening connection " << connection2 << endl;
        }

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(2)); // 2s

        // Simulate transmission...
        const int  length = 1024;
        const char*buffer[length];
        if (d_verbose) {
            bsl::cout << bdlt::CurrentTime::utc() << ": ";
            bsl::cout << "Connection " << connection1
                      << " receives " << length << " bytes " << endl;
        }
        dataAvailable(connection1, buffer, length);

        // Wait for timeout to occur, otherwise session gets destroyed from
        // stack too early.

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(8)); // 8s
    }
//..
// The program that would exercise this test server would simply consist of:
//..
    int usageExample(int verbose)
    {
        my_TestServer mX(5, verbose); // timeout for connections: 5s
        mX.start();

        // Wait sufficiently long to observe all events.
        bslmt::ThreadUtil::sleep(bsls::TimeInterval(10)); // 10s

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

}  // close namespace TIMEQUEUE_USAGE_EXAMPLE

bsls::TimeInterval makeTimeInterval()
{
    static bsls::AtomicInt counter(0);
    return bsls::TimeInterval((double)counter++);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator defaultAlloc(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&defaultAlloc);

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

        using namespace TIMEQUEUE_USAGE_EXAMPLE;
        {
            usageExample(verbose);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CONCERN: Memory Pooling.
        //
        // Note that this is a white-box test added when the 'poolTimerMemory'
        // flag was removed.  See drqs 35794413.
        //
        // Concerns:
        //  1. That memory using when adding an element to a queue, is
        //     returned to a pool when the element is remmoved, and re-used for
        //     later elements.
        //
        // Plan:
        //  1. Create a 'bdlcc_timequeue' with a test allocator, add elements
        //     and verify that memory is allocated.  Remove those elements
        //     and add the same number of new elements, and verify that no
        //     additional memory is allocated.
        //
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CONCERN: Memory Pooling" << endl
                 << "=======================" << endl;
        }

        bslma::TestAllocator ta;

        {
            if (verbose) {
                cout << "\tAdd, remove, and re-add itesm to the queue" << endl;
            }

            bsls::TimeInterval futureTime = bdlt::CurrentTime::now() +
                                           bsls::TimeInterval(600, 0);

            Obj x(&ta);

            {
                bslma::TestAllocatorMonitor tam(&ta);
                x.add(futureTime, "test data 1");
                x.add(futureTime, "test data 2");
                x.removeAll();

                ASSERT(tam.isInUseUp());
                ASSERT(tam.isTotalUp());
            }
            {
                bslma::TestAllocatorMonitor tam(&ta);
                x.add(futureTime, "test data 3");
                x.add(futureTime, "test data 4");

                ASSERT(tam.isInUseSame());
                ASSERT(tam.isTotalSame());
            }
            x.removeAll();

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

              bdlcc::TimeQueue<int> q;

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

              bdlcc::TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bdlcc::TimeQueueItem<int> item;
              while (0 == q.popFront(&item))
                  ;

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(makeTimeInterval(), 0));
          }

          {
              if (veryVerbose) bsl::cout << "c) popLE" << bsl::endl;

              bdlcc::TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bsl::vector<bdlcc::TimeQueueItem<int> > items;
              q.popLE(makeTimeInterval(), &items);

              ASSERT(0 == q.length());
              ASSERT(-1 != q.add(bsls::TimeInterval(0), 0));
          }

          {
              if (veryVerbose) bsl::cout << "d) remove" << bsl::endl;

              bdlcc::TimeQueue<int> q;
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

              bdlcc::TimeQueue<int> q;

              int handle;
              do {
                  handle = q.add(makeTimeInterval(), 0);
              } while (-1 != handle);

              ASSERT(q.length() > 0);

              bdlcc::TimeQueueItem<int> item;
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
        //   Create a time queue.  Create 'k_NUM_THREADS' threads and let each
        //   thread invoke 'add', 'find', 'update', 'popFront', and 'popLE' in
        //   a loop.  Create a thread, let it invoke 'length' in a loop and
        //   verify that there are at least 0 and no more than 'k_NUM_THREADS'
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

        using namespace TIMEQUEUE_TEST_CASE_11;

        Case11ThreadInfo info[k_NUM_THREADS];
        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS + 1];
        bsl::vector<bdlcc::TimeQueueItem<DATA> > items[k_NUM_THREADS];

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            info[i].d_id = i;
            info[i].d_items_p = &items[i];
            bslmt::ThreadUtil::create(&threads[i],
                                     testAddUpdatePopRemoveAll,
                                     (void *)&info[i]);
        }

        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS], testLength, NULL);

        int size = 0;
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::join(threads[i]);
            size += static_cast<int>(items[i].size());
        }
        bslmt::ThreadUtil::join(threads[k_NUM_THREADS]);

        LOOP_ASSERT(timequeue.length(), 0 == timequeue.length());
        LOOP_ASSERT(size, k_NUM_ITERATIONS * k_NUM_THREADS * 2 == size);

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

        using namespace TIMEQUEUE_TEST_CASE_10;

        int numDestructions = 0;
        {
            typedef bdlcc::TimeQueue<TestLockObject> Obj;

            Obj mX(&ta);
            const Obj& X = mX;

            TestLockObject mL(&X, &numDestructions, veryVerbose);
            const TestLockObject& L = mL;

            const bsls::TimeInterval NOW = bdlt::CurrentTime::now();
            const bsls::TimeInterval T1  = NOW + bsls::TimeInterval(1.0);
            const bsls::TimeInterval T2  = NOW + bsls::TimeInterval(2.0);
            const bsls::TimeInterval T3  = NOW + bsls::TimeInterval(3.0);
            const bsls::TimeInterval T4  = NOW + bsls::TimeInterval(4.0);

            (void) mX.add(NOW, L);
            (void) mX.add(T1, L);
            (void) mX.add(T2, L);
            int i = mX.add(T3, L);
            (void) mX.add(T4, L);
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
            (void) mX.add(NOW, L);
            mL.reset();  // avoid complications with order of destruction of L
                         // and mX
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
        //   empty constructs in the 'bdlcc::TimeQueueItem' constructors, and
        //   the test class 'TestString' guarantees that a default-constructed
        //   instance does not trigger an allocation.
        //
        // Testing:
        //   CONCERN: RESPECTING THE 'bdema' ALLOCATOR MODEL
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Concern: 'bdema' allocator model" << endl
                 << "========================================" << endl;

        bslma::TestAllocator ta2(veryVeryVerbose);
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

        typedef bdlcc::TimeQueue<TestString> Obj;

        if (verbose) cout << "\t'popLE' without maxTimers." << endl;
        {
            static const struct {
                int          d_line;
                int          d_secs;
                int          d_nsecs;
                int          d_expNumItems;
                const char  *d_expItems;
            } POP_DATA[] = {
                //line  secs    nsecs   expNumItems expItems
                //---- ------ --------- ----------- ----------------
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL); P_(TIME); P(X.length());
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
                const bsls::TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();

                bsl::vector<bdlcc::TimeQueueItem<TestString> > buffer(&ta);

                int newLength = 0;
                bsls::TimeInterval newMinTime;

                mX.popLE(TIME, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    T_; P_(LINE); P_(EXPNUMITEMS);P(TIME);
                    T_; P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bsls::TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                        T_; P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bsls::TimeInterval() == newMinTime);
                }

                if (buffer.size() && EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int         I      = EXPITEMS[j];
                        const TestString& EXPVAL = *VALUES[I].d_value;
                        const bsls::TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_; T_; P_(I); P_(EXPVAL); P(EXPTIME);
                            T_; T_; P_(buffer[j].time());
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
                //line secs   nsecs   maxNum expNum expItems
                //---- ---- --------- ------ ------ ----------------
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            bsl::vector<bdlcc::TimeQueueItem<TestString> > buffer(&ta);
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const int   MAXNUMITEMS  = POP_DATA[i].d_maxNumItems;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const bsls::TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();
                const int   OLDSIZE      = static_cast<int>(buffer.size());

                int newLength = 0;
                bsls::TimeInterval newMinTime;

                mX.popLE(TIME, MAXNUMITEMS, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    T_; P_(LINE); P_(MAXNUMITEMS); P_(EXPNUMITEMS); P(TIME);
                    T_; P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, OLDSIZE + EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bsls::TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                       T_; P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bsls::TimeInterval() == newMinTime);
                }

                if (OLDSIZE + EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int         I      = EXPITEMS[j];
                        const TestString& EXPVAL = *VALUES[I].d_value;
                        const bsls::TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_; T_; P_(I); P_(EXPVAL);P(EXPTIME);
                            T_; T_; P_(buffer[OLDSIZE + j].time());
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
        //   int update(int handle, const bsls::TimeInterval &newTime,...
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
                //line secs  nsecs    value update secs update nsecs isNewTop
                //---- ----- -------- ----- ----------- ------------ --------
                { L_,   2  , 1000000, VA   ,      0    ,     1000000, 0 },
                { L_,   2  , 1000000, VB   ,      3    ,     1000000, 0 },
                { L_,   2  , 1000000, VC   ,      0    ,        4000, 0 },
                { L_,   2  , 1000001, VB   ,      0    ,        3999, 1 },
                { L_,   1  , 9999998, VC   ,      4    ,     9999998, 0 },
                { L_,   1  , 9999999, VD   ,      0    ,           0, 1 },
                { L_,   0  ,    4000, VE   ,     10    ,        4000, 0 }
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE     = VALUES[i].d_lineNum;
                const int   UPDSECS  = VALUES[i].d_updsecs;
                const int   EXPNEWTOP= VALUES[i].d_isNewTop;
                const int   UPDNSECS = VALUES[i].d_updnsecs;
                const bsls::TimeInterval UPDTIME(UPDSECS,UPDNSECS);

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
                const bsls::TimeInterval EXPTIME(EXPSECS,EXPNSECS);

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
        //   void popLE(const bsls::TimeInterval& time,...
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
                //line secs   nsecs    expNumItems expItems
                //---- ----- --------- ----------- ----------------
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL); P_(TIME); P(X.length());
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
                const bsls::TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();

                bsl::vector<bdlcc::TimeQueueItem<const char*> > buffer(&ta);

                int newLength = 0;
                bsls::TimeInterval newMinTime;

                mX.popLE(TIME, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    T_; P_(LINE); P_(EXPNUMITEMS);P(TIME);
                    T_; P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bsls::TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                        T_; P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bsls::TimeInterval() == newMinTime);
                }

                if (buffer.size() && EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int   I      = EXPITEMS[j];
                        const char *EXPVAL = VALUES[I].d_value;
                        const bsls::TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_; T_; P_(I); P_(EXPVAL);P(EXPTIME);
                            T_; T_; P_(buffer[j].time());
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
                //line secs nsecs     maxNum expNum expItems
                //---- ---- --------- ------ ------ ----------------
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            int cumulNumItems = 0;
            bsl::vector<bdlcc::TimeQueueItem<const char*> > buffer(&ta);
            for (int i = 0; i < NUM_POPS; ++i) {
                const int   LINE         = POP_DATA[i].d_line;
                const int   SECS         = POP_DATA[i].d_secs;
                const int   NSECS        = POP_DATA[i].d_nsecs;
                const int   MAXNUMITEMS  = POP_DATA[i].d_maxNumItems;
                const int   EXPNUMITEMS  = POP_DATA[i].d_expNumItems;
                const char *EXPITEMS     = POP_DATA[i].d_expItems;
                const bsls::TimeInterval TIME(SECS,NSECS);
                const int   OLDLENGTH    = X.length();
                const int   OLDSIZE      = static_cast<int>(buffer.size());

                int newLength = 0;
                bsls::TimeInterval newMinTime;

                mX.popLE(TIME, MAXNUMITEMS, &buffer, &newLength, &newMinTime);

                if (veryVerbose) {
                    T_; P_(LINE); P_(MAXNUMITEMS); P_(EXPNUMITEMS); P(TIME);
                    T_; P_(OLDLENGTH); P_(X.length()); P(buffer.size());
                }

                LOOP_ASSERT(LINE, OLDSIZE + EXPNUMITEMS == (int)buffer.size());
                LOOP_ASSERT(LINE, OLDLENGTH - EXPNUMITEMS == newLength);

                if (0 < newLength) {
                    cumulNumItems += EXPNUMITEMS;
                    const int SORTED_IDX = SORTED_VALUES[cumulNumItems];
                    const int NEWSECS    = VALUES[SORTED_IDX].d_secs;
                    const int NEWNSECS   = VALUES[SORTED_IDX].d_nsecs;
                    const bsls::TimeInterval EXPNEWMINTIME(NEWSECS,NEWNSECS);
                    LOOP_ASSERT(LINE, EXPNEWMINTIME == newMinTime);
                    if (veryVerbose) {
                       T_; P_(EXPNEWMINTIME); P(newMinTime);
                    }
                }
                else {
                    LOOP_ASSERT(LINE, bsls::TimeInterval() == newMinTime);
                }

                if (OLDSIZE + EXPNUMITEMS == (int)buffer.size()) {
                    for (int j=0; j < EXPNUMITEMS; ++j) {
                        const int   I      = EXPITEMS[j];
                        const char *EXPVAL = VALUES[I].d_value;
                        const bsls::TimeInterval EXPTIME(VALUES[I].d_secs,
                                                        VALUES[I].d_nsecs);

                        if (veryVerbose) {
                            T_; T_; P_(I); P_(EXPVAL);P(EXPTIME);
                            T_; T_; P_(buffer[OLDSIZE + j].time());
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
        //   int popFront(bdlcc::TimeQueueItem<DATA> *buffer);
        //   int popFront(bdlcc::TimeQueueItem<DATA> *buffer,...
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   I        = POP_ORDER[i];
                const char *EXPVAL   = VALUES[I].d_value;
                const int   EXPSECS  = VALUES[I].d_secs;
                const int   EXPNSECS = VALUES[I].d_nsecs;
                const bsls::TimeInterval EXPTIME(EXPSECS,EXPNSECS);

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
                //expItem expMinSecs expMinNsecs
                //------- ---------- -----------
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
                bsls::TimeInterval newMinTime;

                ASSERT(0 != mX.popFront(&item, &newLength, &newMinTime));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
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

                const bsls::TimeInterval EXPTIME(EXPSECS,EXPNSECS);
                const bsls::TimeInterval EXPMINTIME(EXPMINSECS,EXPMINNSECS);

                int newLength;
                bsls::TimeInterval newMinTime;

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
                bsls::TimeInterval newMinTime;

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
        //   and assert that all the handles are indeed de-registered and that
        //   the time queue is empty.  For the version that gets a copy of the
        //   removed items into a local buffer, assert that the items are as
        //   expected.
        //
        // Testing:
        //   void removeAll(bsl::vector<bdlcc::TimeQueueItem<DATA> > *buf=0);
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            bsl::vector<bdlcc::TimeQueueItem<const char*> > buffer(&ta);

            ASSERT(NUM_VALUES == X.length());
            mX.removeAll(&buffer);
            ASSERT(0 == X.length());
            ASSERT(NUM_VALUES == (int)buffer.size());

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE   = VALUES[i].d_lineNum;
                const char *VAL    = VALUES[i].d_value;
                const int   SECS   = VALUES[i].d_secs;
                const int   NSECS  = VALUES[i].d_nsecs;
                const bsls::TimeInterval TIME(SECS,NSECS);

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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
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
                const bsls::TimeInterval TIME(SECS,NSECS);

                handles[i] = mX.add(TIME,VAL);
                if (veryVerbose) {
                    T_; P_(LINE); P_(VAL);P_(TIME); P(X.length());
                }
                LOOP_ASSERT(LINE, (i+1) == X.length());
                LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handles[i]));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const bsls::TimeInterval TIME(SECS,NSECS);

                int newLength;
                bdlcc::TimeQueueItem<const char*> buffer(&ta);
                bsls::TimeInterval newMinTime;

                LOOP_ASSERT(LINE, 0 == mX.remove(handles[i], &newLength,
                                                &newMinTime, &buffer));
                LOOP_ASSERT(LINE, NUM_VALUES-i-1 == newLength);
                LOOP_ASSERT(LINE, NUM_VALUES-i-1 == X.length());
                LOOP_ASSERT(LINE, TIME == buffer.time());
                LOOP_ASSERT(LINE, VAL == buffer.data());
                LOOP_ASSERT(LINE, handles[i] == buffer.handle());
                LOOP_ASSERT(LINE, true != X.isRegisteredHandle(handles[i]));
                if (i < NUM_VALUES-1) {
                    const bsls::TimeInterval NEWMINTIME(VALUES[i+1].d_secs,
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
        //   bdlcc::TimeQueue(bslma::Allocator *allocator=0);
        //   bdlcc::TimeQueue(bool poolTimerMem, bslma::Allocator *alloc=0);
        //   ~bdlcc::TimeQueue();
        //   void* add(const bsls::TimeInterval& time, const DATA& data, ...
        //   int length() const;
        //   bool isRegisteredHandle(int handle) const;
        //   int minTime(bsls::TimeInterval *buffer);
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

            Obj mA;                 const Obj& A = mA;
            Obj mB(&ta);            const Obj& B = mB;
            Obj mC(true);           const Obj& C = mC;
            Obj mD(false, &ta);     const Obj& D = mD;
            Obj mE(8);              const Obj& E = mE;
            Obj mF(24, &ta);        const Obj& F = mF;
            Obj mG(10, true);       const Obj& G = mG;
            Obj mH(17, false, &ta); const Obj& H = mH;

            (void)A;  // Suppress unused variable warning
            (void)B;  // Suppress unused variable warning
            (void)C;  // Suppress unused variable warning
            (void)D;  // Suppress unused variable warning
            (void)E;  // Suppress unused variable warning
            (void)F;  // Suppress unused variable warning
            (void)G;  // Suppress unused variable warning
            (void)H;  // Suppress unused variable warning

            Obj *OBJS[] = { &mA, &mB, &mC, &mD, &mE, &mF, &mG, &mH };
            const int NUM_OBJS = sizeof OBJS / sizeof *OBJS;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const int   ISNEWTOP    = VALUES[i].d_isNewTop;
                const bsls::TimeInterval TIME(SECS,NSECS);

                for (int j = 0; j < NUM_OBJS; ++j) {
                    Obj& mX = *OBJS[j]; const Obj& X = mX;

                    int isNewTop;
                    bsls::TimeInterval newMinTime;
                    int newLength;
                    int  handle;
                    handle = mX.add(TIME, VAL, &isNewTop, &newLength);
                    if (veryVerbose) {
                        T_; P_(LINE); P_(VAL);P_(TIME); P(ISNEWTOP);
                        T_;  P_(isNewTop); P(newMinTime); P_(newLength);
                        P(X.length());
                    }
                    LOOP_ASSERT(LINE, ISNEWTOP == isNewTop);
                    LOOP_ASSERT(LINE, (i+1) == newLength);
                    LOOP_ASSERT(LINE, (i+1) == X.length());
                    LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handle));
                }
            }
        }
        ASSERT(0 == defaultAlloc.numBytesInUse());
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

            Obj mA;                 const Obj& A = mA;
            Obj mB(&ta);            const Obj& B = mB;
            Obj mC(true);           const Obj& C = mC;
            Obj mD(false, &ta);     const Obj& D = mD;
            Obj mE(8);              const Obj& E = mE;
            Obj mF(24, &ta);        const Obj& F = mF;
            Obj mG(10, true);       const Obj& G = mG;
            Obj mH(17, false, &ta); const Obj& H = mH;

            (void)A;  // Suppress unused variable warning
            (void)B;  // Suppress unused variable warning
            (void)C;  // Suppress unused variable warning
            (void)D;  // Suppress unused variable warning
            (void)E;  // Suppress unused variable warning
            (void)F;  // Suppress unused variable warning
            (void)G;  // Suppress unused variable warning
            (void)H;  // Suppress unused variable warning

            Obj *OBJS[] = { &mA, &mB, &mC, &mD, &mE, &mF, &mG, &mH };
            const int NUM_OBJS = sizeof OBJS / sizeof *OBJS;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_lineNum;
                const char *VAL         = VALUES[i].d_value;
                const int   SECS        = VALUES[i].d_secs;
                const int   NSECS       = VALUES[i].d_nsecs;
                const int   ISNEWTOP    = VALUES[i].d_isNewTop;
                const bsls::TimeInterval TIME(SECS,NSECS);

                for (int j = 0; j < NUM_OBJS; ++j) {
                    Obj& mX = *OBJS[j]; const Obj& X = mX;

                    int isNewTop;
                    int newLength;

                    int handle;
                    handle = mX.add(TIME, VAL, &isNewTop, &newLength);
                    if (veryVerbose) {
                        T_; P_(LINE); P_(VAL);P_(TIME); P(ISNEWTOP);
                        T_; P_(isNewTop); P_(newLength); P(X.length());
                    }
                    LOOP_ASSERT(LINE, ISNEWTOP == isNewTop);
                    LOOP_ASSERT(LINE, (i+1) == newLength);
                    LOOP_ASSERT(LINE, (i+1) == X.length());
                    LOOP_ASSERT(LINE, true == X.isRegisteredHandle(handle));
                }
            }
        }
        ASSERT(0 == defaultAlloc.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == ta.numMismatches());
        if (veryVeryVerbose) { P(ta); }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST CLASS 'bdlcc::TimeQueueItem'
        //
        // Plan:
        //   Create various time queue items using the different constructors.
        //   Exercise the manipulators and accessors in isolation.  Verify that
        //   the allocator is correctly passed to the data argument.
        //
        // Testing:
        //   bdlcc::TimeQueueItem(...
        //   bdlcc::TimeQueueItem::time();
        //   bdlcc::TimeQueueItem::data();
        //   bdlcc::TimeQueueItem::handle();
        //   bdlcc::TimeQueueItem::key();
        //   bdlcc::TimeQueueItem::time() const;
        //   bdlcc::TimeQueueItem::data() const;
        //   bdlcc::TimeQueueItem::handle() const;
        //   bdlcc::TimeQueueItem::key() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CLASS 'bdlcc::TimeQueueItem'" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\tWithout allocators.\n";
        {
            typedef bdlcc::TimeQueueItem<const char*> Obj;
            const bsls::TimeInterval  TIME(1);
            const char              *STRDATA = "Unused";
            const int                HANDLE = 0xffff;
            const Obj::Key           KEY((const void*)&HANDLE);

            // CREATORS, ACCESSORS
            if (verbose) cout << "\t\tDefault constructor.\n";
            Obj mX;  const Obj& X = mX;
            ASSERT(bsls::TimeInterval() == X.time());
            ASSERT(NULL                == X.data());
            // ASSERT(0 == X.handle());
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
            bslma::TestAllocator ta2(veryVeryVerbose);

            typedef bdlcc::TimeQueueItem<TestString> Obj;
            const bsls::TimeInterval  TIME(1);
            const TestString         STRDATA("Unused", &ta2);
            const TestString         STRDATA2("Another unused", &ta2);
            const int                HANDLE = 0xffff;
            const Obj::Key           KEY((const void*)&HANDLE);

            const bsls::Types::Int64 NUM_ALLOC2 = ta2.numAllocations();

            // CREATORS, ACCESSORS
            if (verbose) cout << "\t\tDefault constructor.\n";
            Obj mX(&ta);  const Obj& X = mX;
            {
                const bsls::Types::Int64 NUM_ALLOC = ta.numAllocations();
                ASSERT(bsls::TimeInterval() == X.time());
                const TestString dfltTestString;    // Appease bug in gcc-4.1.2
                ASSERT(dfltTestString       == X.data());
                // ASSERT(0 == X.handle());
                ASSERT(Obj::Key(0)          == X.key());
                ASSERT(NUM_ALLOC            == ta.numAllocations());
                ASSERT(NUM_ALLOC2           == ta2.numAllocations());
            }

            if (verbose) cout << "\t\tConstructor without key.\n";
            Obj mY(TIME, STRDATA, HANDLE, &ta);  const Obj& Y = mY;
            {
                const bsls::Types::Int64 NUM_ALLOC = ta.numAllocations();
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
                const bsls::Types::Int64 NUM_ALLOC = ta.numAllocations();
                ASSERT(TIME                == Z.time());
                ASSERT(STRDATA             == Z.data());
                ASSERT(HANDLE              == Z.handle());
                ASSERT(KEY                 == Z.key());
                ASSERT(NUM_ALLOC           == ta.numAllocations());
                ASSERT(NUM_ALLOC2          == ta2.numAllocations());
            }

            bslma::TestAllocator     ta3(veryVeryVerbose);
            const bsls::Types::Int64 NUM_ALLOC = ta.numAllocations();

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

            const bsls::Types::Int64 NUM_ALLOC3 = ta3.numAllocations();
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
            const bsls::TimeInterval TA = bdlt::CurrentTime::now();
            const bsls::TimeInterval TB = TA + 1;
            const bsls::TimeInterval TC = TB + 1;
            const bsls::TimeInterval TD = TC + 1;
            const bsls::TimeInterval TE = TD + 1;

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

            bdlcc::TimeQueueItem<const char*> tItem(&ta);
            bsls::TimeInterval newMinTime;
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

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a1(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A1 = a1;

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

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a2(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A2 = a2;

            x1.popLE(TD, &a2, &newLength, &newMinTime);
            ASSERT(0  == A2.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a3(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A3 = a3;

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
            const bsls::TimeInterval TA = bdlt::CurrentTime::now();
            const bsls::TimeInterval TB = TA + 1;
            const bsls::TimeInterval TC = TB + 1;
            const bsls::TimeInterval TD = TC + 1;
            const bsls::TimeInterval TE = TD + 1;

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

            bdlcc::TimeQueueItem<const char*> tItem(&ta);
            bsls::TimeInterval newMinTime;
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

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a1(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A1 = a1;

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

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a2(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A2 = a2;

            x1.popLE(TD, &a2, &newLength, &newMinTime);
            ASSERT(0  == A2.size());
            ASSERT(1  == newLength);
            ASSERT(TE == newMinTime);

            bsl::vector<bdlcc::TimeQueueItem<const char*> > a3(&ta);
            const bsl::vector<bdlcc::TimeQueueItem<const char*> > &A3 = a3;

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
    case -1: {
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

        // The 'poolTimerMemory' option is now deprecated.  However this test,
        // is not a "test" in that it has no asserts, and might still be
        // utilized to verify the performance of the 'bdlcc::TimeQueue'.

        if (verbose)
            cout << endl
                 << "Performance test using 'poolTimerMemory' flag" << endl
                 << "=============================================" << endl;

        const char VA[] = "A";

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_SOLARIS)
        // Specifying larger number of iterations causes these platforms to
        // take a really long time to complete the test.

        const int NUM_OUTER_ITERATIONS = 1000;
        const int NUM_INNER_ITERATIONS = 50;
#else
        const int NUM_OUTER_ITERATIONS = 10000;
        const int NUM_INNER_ITERATIONS = 100;
#endif
        bslma::TestAllocator na1, na2;

        const int NUM_TOTAL_ITERATIONS =
                                   NUM_OUTER_ITERATIONS * NUM_INNER_ITERATIONS;

        bsls::TimeInterval         TIME = bdlt::CurrentTime::now();
        int                       isNewTop;
        bsls::TimeInterval         newMinTime;
        int                       newLength;
        vector<int>               handles(NUM_TOTAL_ITERATIONS);
        vector<Item>              items(NUM_TOTAL_ITERATIONS);
        vector<bsls::TimeInterval> timers(NUM_TOTAL_ITERATIONS, TIME);
        vector<bsls::TimeInterval> popTimes(NUM_OUTER_ITERATIONS, TIME);

        srand(static_cast<unsigned int>(time(0)));

        for (int i = 0, k = 0; i < NUM_OUTER_ITERATIONS; ++i) {
            for (int j = 0; j < NUM_INNER_ITERATIONS; ++j, ++k) {
                timers[k].addMilliseconds(rand() % NUM_TOTAL_ITERATIONS);
            }
            popTimes[i].addMilliseconds(i * NUM_INNER_ITERATIONS);
        }

        bsls::Stopwatch s;
        s.start();
        {
            Obj mX(&na1);
            for (int j = 0, t = 0; j < NUM_OUTER_ITERATIONS; ++j) {
                for (int k = 0; k < NUM_INNER_ITERATIONS; ++k, ++t) {
                    handles[k] = mX.add(timers[k], VA, &isNewTop, &newLength);
                }

                bslmt::ThreadUtil::microSleep(NUM_INNER_ITERATIONS);

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
            Obj mX(true, &na2);
            for (int j = 0, t = 0; j < NUM_OUTER_ITERATIONS; ++j) {
                for (int k = 0; k < NUM_INNER_ITERATIONS; ++k, ++t) {
                    handles[k] = mX.add(timers[k], VA, &isNewTop, &newLength);
                }

                bslmt::ThreadUtil::microSleep(NUM_INNER_ITERATIONS);

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

      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        TIMEQUEUE_TEST_CASE_MINUS_100::run();
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
