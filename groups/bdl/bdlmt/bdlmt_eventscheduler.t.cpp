// bdlmt_eventscheduler.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_eventscheduler.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bsls_atomic.h>
#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlf_memfn.h>
#include <bdlb_bitutil.h>
#include <bdlt_datetime.h>
#include <bdlt_currenttime.h>

#include <bslma_defaultallocatorguard.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_sys_time.h>
#include <unistd.h>
#endif

#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'bdlmt::EventScheduler' is divided into 2 parts (apart from
// breathing test and usage example).  The first part tests the functions in
// isolation, the second is more integrated in that it tests a particular
// situation in context, with a combination of functions.
//
// [2] Verify that callbacks are invoked as expected when multiple
// clocks and multiple events are scheduled.
//
// [3] Verify that 'cancelEvent' works correctly in various white box
// states.
//
// [4] Verify that 'cancelAllEvents' works correctly in various white
// box states.
//
// [7] Test 'scheduleEvent', 'cancelEvent', and 'cancelAllEvents' when they
// are invoked from dispatcher thread.
//
// [8] Test the scheduler with a user-defined dispatcher.
//
// [9] Verify that 'start' and 'stop' work correctly in various white
// box states.
//
// [10] Verify the concurrent scheduling and cancelling of clocks and
// events.
//
// [11] Verify the concurrent scheduling and cancelling-all of clocks and
// events.
//
//-----------------------------------------------------------------------------
// CREATORS
// [01] bdlmt::EventScheduler(allocator = 0);
// [19] bdlmt::EventScheduler(clockType, allocator = 0);
//
// [08] bdlmt::EventScheduler(dispatcher, allocator = 0);
// [20] bdlmt::EventScheduler(disp, clockType, alloc = 0);
//
// [01] ~bdlmt::EventScheduler();
//
// MANIPULATORS
// [04] void cancelAllEvents(bool wait=false);
//
// [03] int cancelEvent(Handle handle, bool wait=false);
//
// [12] int rescheduleEvent(handle, newTime);
//
// [02] Handle scheduleEvent(time, callback);
//
// [09] int start();
//
// [16] int start(const bslmt::ThreadAttributes& threadAttributes);
//
// [09] void stop();
//
// ACCESSORS
//-----------------------------------------------------------------------------
// [01] BREATHING TEST
// [07] TESTING METHODS INVOCATIONS FROM THE DISPATCHER THREAD
// [10] TESTING CONCURRENT SCHEDULING AND CANCELLING
// [11] TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL
// [21] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   THREAD-SAFE OUTPUT AND ASSERT MACROS
// ----------------------------------------------------------------------------
static bslmt::Mutex printMutex;  // mutex to protect output macros
#define PT(X) { printMutex.lock(); P(X); printMutex.unlock(); }
#define PT_(X) { printMutex.lock(); P_(X); printMutex.unlock(); }
#define ET(X) { printMutex.lock(); cout << X << endl; printMutex.unlock(); }
#define ET_(X) { printMutex.lock(); cout << X << " "; printMutex.unlock(); }
static bslmt::Mutex &assertMutex = printMutex;
                                              // mutex to protect assert macros

#define ASSERTT(X) { assertMutex.lock(); aSsErT(!(X), #X, __LINE__); \
                                             assertMutex.unlock();}

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << endl; \
       aSsErT(1, #X, __LINE__); assertMutex.unlock(); } }

#define LOOP2_ASSERTT(I,J,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << endl; aSsErT(1, #X, __LINE__); \
       assertMutex.unlock(); } }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
      << #J << ": " << J << "\t" << #K << ": " << K << endl; \
      aSsErT(1, #X, __LINE__); assertMutex.unlock(); } }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << endl; aSsErT(1, #X, __LINE__); assertMutex.unlock(); } }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << "\t" << #M << ": " << M << endl; \
       aSsErT(1, #X, __LINE__); assertMutex.unlock(); } }

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bdlmt::EventScheduler                Obj;
typedef Obj::EventHandle                   EventHandle;
typedef Obj::RecurringEvent                RecurringEvent;
typedef Obj::Event                         Event;
typedef Obj::RecurringEventHandle          RecurringEventHandle;

// TESTING NOTE: This component relies on timing and on
// 'bslmt::ThreadUtil::microsleep' which has no guarantee of sleeping the
// exact amount of time (it can oversleep, and frequently so when load is
// high).  For this purpose, we need to make sure that the tests are meaningful
// if microsleep oversleeps, so all test cases adopt a defensive style where we
// only assert when we *know* that the condition could not have happened (by
// measuring elapsed times).

static const float DECI_SEC = 1.0/10;    // 1 deci-second (a tenth of a second)

static const int   DECI_SEC_IN_MICRO_SEC = 100000;
                               // number of microseconds in a tenth of a second

// Tolerance for testing correct timing
static const bsls::TimeInterval UNACCEPTABLE_DIFFERENCE(0, 500000000); // 500ms
static const bsls::TimeInterval ALLOWABLE_DIFFERENCE   (0,  75000000); //  75ms
static const bsls::TimeInterval APPRECIABLE_DIFFERENCE (0,  20000000); //  20ms

static inline bool isUnacceptable(const bsls::TimeInterval& t1,
                                  const bsls::TimeInterval& t2)
    // Return true if the specified 't1' and 't2' are unacceptably not (the
    // definition of *unacceptable* is implementation-defined) equal, otherwise
    // return false.
{
    bsls::TimeInterval absDifference = (t1 > t2) ? (t1 - t2) : (t2 - t1);
    return (ALLOWABLE_DIFFERENCE > absDifference)? true : false;
}

static inline bool isApproxEqual(const bsls::TimeInterval& t1,
                                 const bsls::TimeInterval& t2)
    // Return true if the specified 't1' and 't2' are approximately (the
    // definition of *approximate* is implementation-defined) equal, otherwise
    // return false.
{
    bsls::TimeInterval absDifference = (t1 > t2) ? (t1 - t2) : (t2 - t1);
    return (ALLOWABLE_DIFFERENCE > absDifference)? true : false;
}

static inline bool isApproxGreaterThan(const bsls::TimeInterval& t1,
                                       const bsls::TimeInterval& t2)
    // Return true if the specified 't1' is approximately (the definition of
    // *approximate* is implementation-defined) greater than the specified
    // 't2', otherwise return false.
{
    return ((t1 - t2) > APPRECIABLE_DIFFERENCE)? true : false;
}

void microSleep(int microSeconds, int seconds)
    // Sleep for *at* *least* the specified 'seconds' and 'microseconds'.  This
    // function is used for testing only.  It uses the function
    // 'bslmt::ThreadUtil::microSleep' but interleaves calls to 'yield' to
    // give a chance to the event scheduler to process its dispatching thread.
    // Without this, there have been a large number of unpredictable
    // intermittent failures by this test driver, especially on AIX with
    // xlc-8.0, in the nightly builds (i.e., when the load is higher than
    // running the test driver by hand).  It was noticed that calls to 'yield'
    // helped, and this routine centralizes this as a mechanism.
    //
    // On 7/29/09 this was changed to do a single 'microSleep' method call, due
    // to the observation that on a heavily loaded machine a call to the
    // 'bslmt::ThreadUtil::microSleep' method can take over a second longer
    // than specified.  Otherwise we would have had to add at least 2 seconds
    // tolerance for every call to the 'microSleep' method.
{
    bslmt::ThreadUtil::yield();
    bslmt::ThreadUtil::microSleep(microSeconds, seconds);
    bslmt::ThreadUtil::yield();
}

template <class TESTCLASS>
void makeSureTestObjectIsExecuted(TESTCLASS& testObject,
                                  const int  microSeconds,
                                  int        numAttempts,
                                  int        numExecuted = 0)
    // Check that the specified 'testObject' has been executed one more time in
    // addition to the optionally specified 'numExecuted' times, for the
    // specified 'numAttempts' separated by the specified 'microSeconds', and
    // return as soon as that is true or when the 'numAttempts' all fail.
{
    for (int i = 0; i < numAttempts; ++i) {
        if (numExecuted + 1 <= testObject.numExecuted()) {
            return;                                                   // RETURN
        }
        bslmt::ThreadUtil::microSleep(microSeconds);
        bslmt::ThreadUtil::yield();
    }
}

                         // ==========================
                         // function executeInParallel
                         // ==========================

static void executeInParallel(int                               numThreads,
                              bslmt::ThreadUtil::ThreadFunction func)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
    // to i'th thread.  Finally join all the threads.
{
    bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

                              // ===============
                              // class TestClass
                              // ===============

class TestClass {
    // This class encapsulates the data associated with a clock or an event.

    bool                d_isClock;                  // true if this is a clock,
                                                    // false when this is an
                                                    // event

    bsls::TimeInterval  d_periodicInterval;         // periodic interval if
                                                    // this is a recurring
                                                    // event

    bsls::TimeInterval  d_expectedTimeAtExecution;  // expected time at which
                                                    // callback should run

    bsls::AtomicInt     d_numExecuted;              // number of times callback
                                                    // has been executed

    bsls::AtomicInt     d_executionTime;            // duration for which
                                                    // callback executes

    int                 d_line;                     // for error reporting

    bsls::AtomicInt     d_delayed;                  // will be set to true if
                                                    // any execution of the
                                                    // callback is delayed from
                                                    // its expected execution
                                                    // time

    bsls::TimeInterval  d_referenceTime;            // time from which
                                                    // execution time is
                                                    // referenced for debugging
                                                    // purpose

    bsls::TimeInterval *d_globalLastExecutionTime;  // last time *ANY* callback
                                                    // was executed

    bool                d_assertOnFailure;          // case 2 must not assert
                                                    // on failure unless it
                                                    // fails too many times

    bsls::AtomicInt     d_failures;                 // timing failures

    // FRIENDS
    friend bsl::ostream &operator<<(bsl::ostream &, const TestClass &);

  public:
    // CREATORS
    TestClass(int                 line,
              bsls::TimeInterval  expectedTimeAtExecution,
              bsls::TimeInterval *globalLastExecutionTime,
              int                 executionTime = 0,
              bool                assertOnFailure = true):
      d_isClock(false),
      d_periodicInterval(0),
      d_expectedTimeAtExecution(expectedTimeAtExecution),
      d_numExecuted(0),
      d_executionTime(executionTime),
      d_line(line),
      d_delayed(false),
      d_referenceTime(bdlt::CurrentTime::now()),
      d_globalLastExecutionTime(globalLastExecutionTime),
      d_assertOnFailure(assertOnFailure),
      d_failures(0)
    {
    }

    TestClass(int                 line,
              bsls::TimeInterval  expectedTimeAtExecution,
              bsls::TimeInterval  periodicInterval,
              bsls::TimeInterval *globalLastExecutionTime,
              int                 executionTime = 0,
              bool                assertOnFailure = true):
      d_isClock(true),
      d_periodicInterval(periodicInterval),
      d_expectedTimeAtExecution(expectedTimeAtExecution),
      d_numExecuted(0),
      d_executionTime(executionTime),
      d_line(line),
      d_delayed(false),
      d_referenceTime(bdlt::CurrentTime::now()),
      d_globalLastExecutionTime(globalLastExecutionTime),
      d_assertOnFailure(assertOnFailure),
      d_failures(0)
    {
    }

    TestClass(const TestClass& original):
      d_isClock(original.d_isClock),
      d_periodicInterval(original.d_periodicInterval),
      d_expectedTimeAtExecution(original.d_expectedTimeAtExecution),
      d_numExecuted(original.d_numExecuted.loadRelaxed()),
      d_executionTime(original.d_executionTime.loadRelaxed()),
      d_line(original.d_line),
      d_delayed(original.d_delayed.loadRelaxed()),
      d_referenceTime(original.d_referenceTime),
      d_globalLastExecutionTime(original.d_globalLastExecutionTime),
      d_assertOnFailure(original.d_assertOnFailure),
      d_failures(0)
    {
    }

    // MANIPULATORS
    void callback()
        // This function is the callback associated with this clock or event.
        // On execution, it print an error if it has not been executed at
        // expected time.  It also updates any relevant class data.
    {
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        if (veryVerbose) {
            printMutex.lock();
            cout << (d_isClock ? "CLOCK" : "EVENT") << " specified at line "
                 << d_line << " executed "
                 << (now - d_referenceTime).nanoseconds() / 1000000
                 << " milliseconds after it was scheduled" << endl;
            printMutex.unlock();
        }

        // if this execution has been delayed due to a long running callback
        // or due to high loads during parallel testing
        if (d_delayed || isApproxGreaterThan(*d_globalLastExecutionTime,
                                             d_expectedTimeAtExecution)) {
            d_delayed = true;
            d_expectedTimeAtExecution = now;
        }

        // assert that it runs on expected time
        if (d_assertOnFailure) {
            LOOP3_ASSERTT(d_line, now, d_expectedTimeAtExecution,
                          isApproxEqual(now, d_expectedTimeAtExecution));
        }

        // in any case, keep track of number of failures
        if (!isApproxEqual(now, d_expectedTimeAtExecution)) {
            if (isUnacceptable(now, d_expectedTimeAtExecution)) {
                d_failures += 100; // large number to trigger overall failure
            } else {
                ++d_failures; // small failure, might still be below threshold
            }
        }

        if (d_executionTime) {
            bslmt::ThreadUtil::microSleep(d_executionTime);
        }

        now = bdlt::CurrentTime::now();
        *d_globalLastExecutionTime = now;

        // if this is a clock, update the expected time for the *next*
        // execution
        if (d_isClock) {
            d_expectedTimeAtExecution = now + d_periodicInterval;
        }
        ++d_numExecuted;
    }

    // ACCESSORS
    bool delayed() const
    {
        return d_delayed;
    }

    int numExecuted() const
    {
        return d_numExecuted;
    }

    int numFailures() const
    {
        return d_failures;
    }
};

// FRIENDS
bsl::ostream& operator << (bsl::ostream& os, const TestClass& testObject)
{
    P(testObject.d_line);
    P(testObject.d_isClock);
    P(testObject.d_periodicInterval);
    P(testObject.d_expectedTimeAtExecution);
    P(testObject.d_numExecuted);
    P(testObject.d_executionTime);
    P(testObject.d_delayed);
    P(testObject.d_referenceTime);
    return os;
}

                              // ================
                              // class TestClass1
                              // ================

struct TestClass1 {
    // This class define a function 'callback' that is used as a callback for a
    // clock or an event.  The class keeps track of number of times the
    // callback has been executed.

    bsls::AtomicInt  d_numExecuted;
    int              d_executionTime;  // in microseconds

    // CREATORS
    TestClass1() :
    d_numExecuted(0),
    d_executionTime(0)
    {
    }

    explicit
    TestClass1(int executionTime) :
    d_numExecuted(0),
    d_executionTime(executionTime)
    {
    }

    // MANIPULATORS
    void callback()
    {
        if (veryVerbose) {
            ET_("TestClass1::callback"); PT_(this);
            PT(bdlt::CurrentTime::now());
        }
        if (d_executionTime) {
            bslmt::ThreadUtil::microSleep(d_executionTime, 0);
        }

        ++d_numExecuted;
    }

    // ACCESSORS
    int numExecuted()
    {
        return d_numExecuted;
    }
};

extern "C" void mustBeCancelledCallBack()
    // for testing 'cancel' methods -- this function will be scheduled, but
    // always canceled before it happens.

{
    ASSERT(0);
}

void cancelEventCallback(Obj         *scheduler,
                         EventHandle  handle,
                         int          wait,
                         int          expectedStatus,
                         int          line)
    // If the specified 'wait' is non-zero, invoke 'cancelEventAndWait' on the
    // specified 'scheduler' passing 'handle', otherwise invoke 'cancelEvent'.
    // Assert that the result is equal to the specified 'expectedStatus' and
    // report the error on the specified 'line' if not.
{
    if (veryVerbose) {
        ET_("cancelEventCallback"); PT(bdlt::CurrentTime::now());
    }

    int ret = -999;
    if (wait) {
        ret = scheduler->cancelEventAndWait(handle);
    }
    else {
        ret = scheduler->cancelEvent(handle);
    }

    LOOP3_ASSERTT(line, expectedStatus, ret, expectedStatus == ret);
}

void cancelEventHandleCallback(Obj         *scheduler,
                               EventHandle *handle,
                               int          wait,
                               int          expectedStatus,
                               int          line)
    // If the specified 'wait' is non-zero, invoke 'cancelEventAndWait' on the
    // specified 'scheduler' passing 'handle', otherwise invoke 'cancelEvent'.
    // Assert that the result is equal to the specified 'expectedStatus' and
    // report the error on the specified 'line' if not.
{
    if (veryVerbose) {
        ET_("cancelEventCallback"); PT(bdlt::CurrentTime::now());
    }

    int ret = -999;
    if (wait) {
        ret = scheduler->cancelEventAndWait(handle);
    }
    else {
        ret = scheduler->cancelEvent(handle);
    }

    LOOP3_ASSERTT(line, expectedStatus, ret, expectedStatus == ret);
    ASSERTT(0 == (const Event *) *handle);
}

void cancelEventRawCallback(Obj    *scheduler,
                            Event **handle,
                            int     wait,
                            int     expectedStatus)
    // If the specified 'wait' is non-zero, invoke 'cancelEventAndWait' on the
    // specified 'scheduler' passing '*handle', otherwise invoke 'cancelEvent'.
    // Assert that the result is equal to the specified 'expectedStatus'.
{
    int ret = -999;
    if (wait) {
        ret = scheduler->cancelEventAndWait(*handle);
    }
    else {
        ret = scheduler->cancelEvent(*handle);
    }
    LOOP_ASSERTT(ret, expectedStatus == ret);
}

void cancelEventCallbackWithState(Obj                         *scheduler,
                                  EventHandle                 *handle,
                                  const bsl::shared_ptr<int>&  state)
    // Invoke 'cancelEvent' on the specified 'scheduler' passing '*handle' and
    // assert that the specified 'state' is still valid.
{
    const int s = *state;
    int ret = scheduler->cancelEvent(*handle);
    LOOP_ASSERT(ret, s == *state);
}

static void cancelRecurringEventCallback(Obj                  *scheduler,
                                         RecurringEventHandle *handlePtr,
                                         int                   wait,
                                         int                   expectedStatus)
    // If the specified 'wait' is non-zero, invoke 'cancelEventAndWait' on the
    // specified 'scheduler' passing '*handlePtr', otherwise invoke
    // 'cancelEvent'.  Assert that the result is equal to the specified
    // 'expectedStatus'.
{
    int ret = -999;
    if (wait) {
        ret = scheduler->cancelEventAndWait(*handlePtr);
    }
    else {
        ret = scheduler->cancelEvent(*handlePtr);
    }

    LOOP_ASSERT(ret, expectedStatus == ret);
}

static void cancelAllEventsCallback(Obj *scheduler, int wait)
    // Invoke 'cancelAllEvents' on the specified 'scheduler' passing 'wait'.
{
    if (wait) {
        scheduler->cancelAllEventsAndWait();
    }
    else {
        scheduler->cancelAllEvents();
    }
}

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS FOR TESTING
// ============================================================================

// ============================================================================
//                      USAGE EXAMPLE RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_USAGE {

bsls::AtomicInt  g_data;  // Some global data we want to track
typedef pair<bdlt::Datetime, int> Value;

void saveData(vector<Value> *array)
{
    array->push_back(Value(bdlt::CurrentTime::utc(), g_data));
}

class my_Session{
    // This class encapsulates the data and state associated with a connection
    // and provides a method 'processData' to process the incoming data for the
    // connection.

  public:
    int processData(void *, int)
        // Process the data of length given by 'int' pointed at by 'void *'
    {
        // (undefined in usage example...no-op here)

        return 0;
    }
};

class my_Server {
    // This class implements a server maintaining several connections.  A
    // connection is closed if the data for it does not arrive before a timeout
    // (specified at the server creation time).

    struct Connection {
        bdlmt::EventSchedulerEventHandle  d_timerId;      // handle for timeout
                                                          // event

        my_Session                       *d_session_p;    // session for this
                                                          // connection
    };

    bsl::vector<Connection*> d_connections;  // maintained connections
    bdlmt::EventScheduler    d_scheduler;    // timeout event scheduler
    bsls::TimeInterval       d_ioTimeout;    // time out

    void newConnection(Connection *connection);
        // Add the specified 'connection' to this server and schedule the
        // timeout event that closes this connection if the data for this
        // connection does not arrive before the timeout.

    void closeConnection(Connection *connection);
        // Close the specified 'connection' and remove it from this server.

    void dataAvailable(Connection *connection, void *data, int length);
        // Return if the specified 'connection' has already timed-out.  If not,
        // cancel the existing timeout event for the 'connection', process the
        // specified 'data' of the specified 'length' and schedule a new
        // timeout event that closes the 'connection' if the data does not
        // arrive before the timeout.

    my_Server(const my_Server&);             // unimplemented
    my_Server& operator=(const my_Server&);  // unimplemented

  public:
    explicit
    my_Server(const bsls::TimeInterval&  ioTimeout,
              bslma::Allocator          *allocator = 0);
        // Construct a 'my_Server' object with a timeout value of the specified
        // 'ioTimeout' seconds.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is 0, the currently installed default
        // allocator is used.

    ~my_Server();
        // Perform the required clean-up and destroy this object.
};

my_Server::my_Server(const bsls::TimeInterval&  ioTimeout,
                     bslma::Allocator          *allocator)
: d_connections(allocator)
, d_scheduler(allocator)
, d_ioTimeout(ioTimeout)
{
     // logic to start monitoring the arriving connections or data

     d_scheduler.start();
}

my_Server::~my_Server()
{
    // logic to clean up

    d_scheduler.stop();
}

void my_Server::newConnection(my_Server::Connection *connection)
{
    // logic to add 'connection' to the 'd_connections'

    // setup the timeout for data arrival
    d_scheduler.scheduleEvent(
          &connection->d_timerId,
          bdlt::CurrentTime::now() + d_ioTimeout,
          bdlf::BindUtil::bind(&my_Server::closeConnection, this, connection));
}

void my_Server::closeConnection(my_Server::Connection *)
{
    // logic to close the 'Connection *' and remove it from 'd_ioTimeout'
}

void my_Server::dataAvailable(my_Server::Connection *connection,
                              void                  *data,
                              int                    length)
{
    // If connection has already timed out and closed, simply return.
    if (d_scheduler.cancelEvent(connection->d_timerId)) {
        return;                                                       // RETURN
    }

    // process the data
    connection->d_session_p->processData(data, length);

    // setup the timeout for data arrival
    d_scheduler.scheduleEvent(
          &connection->d_timerId,
          bdlt::CurrentTime::now() + d_ioTimeout,
          bdlf::BindUtil::bind(&my_Server::closeConnection, this, connection));
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_USAGE

// ============================================================================
//                         CASE 20 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_20 {

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply executes the specified
    // 'functor'.
{
    functor();
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_20

// ============================================================================
//                         CASE 19 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_19 {

}  // close namespace EVENTSCHEDULER_TEST_CASE_19

// ============================================================================
//                         CASE 17 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_17 {

enum { BUFSIZE = 40 * 1000 };

struct Recurser {
    int d_recurseDepth;
    char *d_topPtr;
    static bool s_finished;

    // pseudoUse() was added to prevent the recursion of 'deepRecurser()' from
    // being tail recursion, which the optimizer on windows was turning into a
    // loop, which was an infinite loop.

    void pseudoUse(volatile char *buffer)
    {
        buffer[0] = 0;
    }

    void deepRecurser()
    {
        volatile char buffer[BUFSIZE];

        int curDepth = abs((int)(buffer - d_topPtr));

        pseudoUse(buffer);

        if (veryVerbose) {
            cout << "Depth: " << curDepth << endl;
        }

        if (curDepth < d_recurseDepth) {
            // recurse

            this->deepRecurser();
        }

        pseudoUse(buffer);
    }

    void operator()()
    {
        char topRef;

        microSleep(600 * 1000, 0);

        d_topPtr = &topRef;
        this->deepRecurser();

        s_finished = true;
    }
};
bool Recurser::s_finished = false;

}  // close namespace EVENTSCHEDULER_TEST_CASE_17

// ============================================================================
//                       CASE 15 & 16 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_15 {

struct SlowFunctor {

    // TYPES
    typedef bsl::pair<double, bsls::Types::Int64> TimeElement;
    typedef bsl::list<TimeElement >               DateTimeList;

    enum {
        SLEEP_MICROSECONDS = 100 * 1000
    };

    // CLASS DATA
    static const double SLEEP_SECONDS;

    // DATA
    DateTimeList d_timeList;

    DateTimeList& timeList()
    {
        return d_timeList;
    }

    static TimeElement timeOfDay(bsls::Types::Int64 warnAfter)
    {
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        bsls::Types::Int64 interval = now.totalMicroseconds();
        if (0 < warnAfter && warnAfter < interval) {
            cout << "...SlowFunctor invoked at " << interval
                 << " ( " << interval - warnAfter << " after "
                 << warnAfter << ")" << endl;
        }
        return TimeElement(now.totalSecondsAsDouble(), interval);
    }

    void callback(bsls::Types::Int64 warnAfter)
    {
        d_timeList.push_back(timeOfDay(warnAfter));
        bslmt::ThreadUtil::microSleep(SLEEP_MICROSECONDS);
        d_timeList.push_back(timeOfDay(0));
    }

    double tolerance(int i)
    {
        return SlowFunctor::SLEEP_SECONDS * (0.2 * i + 2);
    }
};

const double SlowFunctor::SLEEP_SECONDS = SLEEP_MICROSECONDS * 1e-6;

struct FastFunctor {

    // TYPES
    typedef bsl::list<double>  DateTimeList;

    // CLASS DATA
    static const double TOLERANCE_AHEAD;
    static const double TOLERANCE_BEHIND;

    // DATA
    DateTimeList d_timeList;

    DateTimeList& timeList()
    {
        return d_timeList;
    }

    static double timeOfDay()
    {
        return bdlt::CurrentTime::now().totalSecondsAsDouble();
    }

    void callback(bool verbose)
    {
        if (verbose) {
            cout << "...FastFunctor invoked at "
                 << bdlt::CurrentTime::now().totalMicroseconds()
                 << endl;
        }
        d_timeList.push_back(timeOfDay());
    }
};
const double FastFunctor::TOLERANCE_AHEAD  = 0.015;
const double FastFunctor::TOLERANCE_BEHIND = 0.300;

}  // close namespace EVENTSCHEDULER_TEST_CASE_15

// ----------------------------------------------------------------------------
//                       CASE 13 & 14 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_13 {

void countInvoked(bsls::AtomicInt *numInvoked)
{
    if (numInvoked) {
        ++*numInvoked;
    }
}

void scheduleEvent(Obj             *scheduler,
                   int              numNeeded,
                   bsls::AtomicInt *numAdded,
                   bsls::AtomicInt *numInvoked,
                   bslmt::Barrier  *barrier)
{
    barrier->wait();

    while (*numAdded < numNeeded) {
        scheduler->scheduleEvent(
                              bdlt::CurrentTime::now(),
                              bdlf::BindUtil::bind(&countInvoked, numInvoked));
        ++*numAdded;
    }
}

void scheduleRecurringEvent(Obj             *scheduler,
                            int              numNeeded,
                            bsls::AtomicInt *numAdded,
                            bsls::AtomicInt *numInvoked,
                            bslmt::Barrier  *barrier)
{
    barrier->wait();

    while (*numAdded < numNeeded) {
        scheduler->scheduleRecurringEvent(
                              bsls::TimeInterval(1),
                              bdlf::BindUtil::bind(&countInvoked, numInvoked));
        ++*numAdded;
    }
}

// Calculate the number of bits required to store an index with the specified
// 'maxValue'.
int numBitsRequired(int maxValue)
{
    ASSERT(0 <= maxValue);

    return (sizeof(maxValue) * CHAR_BIT) - bdlb::BitUtil::numLeadingUnsetBits(
                                         static_cast<bsl::uint32_t>(maxValue));
}

// Calculate the largest integer identifiable using the specified 'numBits'.
int maxNodeIndex(int numBits)
{
    return (1 << numBits) - 2;
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_13

// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_12 {

}  // close namespace EVENTSCHEDULER_TEST_CASE_12

// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_11 {

enum {
    NUM_THREADS    = 8,    // number of threads
    NUM_ITERATIONS = 1000  // number of iterations
};

const bsls::TimeInterval T6(6 * DECI_SEC); // decrease chance of timing failure
bool  testTimingFailure = false;

bslmt::Barrier barrier(NUM_THREADS);
bslma::TestAllocator ta;
Obj x(&ta);

TestClass1 testObj[NUM_THREADS]; // one test object for each thread

extern "C" {
void *workerThread11(void *arg)
{
    int id = (int)(bsls::Types::IntPtr)arg;

    barrier.wait();
    switch(id % 2) {

      // even numbered threads run 'case 0:'
      case 0: {
          for (int i = 0; i< NUM_ITERATIONS; ++i) {
              bsls::TimeInterval now = bdlt::CurrentTime::now();
              x.scheduleEvent(now + T6,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj[id]));
              x.cancelAllEvents();
              bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
              if (!testTimingFailure) {
                  // This logic is such that if testTimingFailure is false,
                  // then we can *guarantee* that no job should have been able
                  // to execute.  The logic always errs in the favor of doubt,
                  // but the common case is that elapsed will always be < T4.

                  testTimingFailure = (elapsed >= T6);
              }
          }
      }
      break;

      // odd numbered threads run 'case 1:'
      case 1: {
          for (int i = 0; i< NUM_ITERATIONS; ++i) {
              bsls::TimeInterval now = bdlt::CurrentTime::now();
              x.scheduleRecurringEvent(
                  T6,
                  bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj[id]));
              x.cancelAllEvents();
              bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
              if (!testTimingFailure) {
                  // This logic is such that if testTimingFailure is false,
                  // then we can *guarantee* that no job should have been able
                  // to execute.  The logic always errs in the favor of doubt,
                  // but the common case is that elapsed will always be < T4.

                  testTimingFailure = (elapsed >= T6);
              }
          }
      }
      break;
    };

    barrier.wait();
    return NULL;
}
} // extern "C"

}  // close namespace EVENTSCHEDULER_TEST_CASE_11

// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_10 {

enum {
    NUM_THREADS    = 8,    // number of threads
    NUM_ITERATIONS = 1000  // number of iterations
};

const bsls::TimeInterval T6(6 * DECI_SEC); // decrease chance of timing failure
bool  testTimingFailure = false;

bslmt::Barrier barrier(NUM_THREADS);
bslma::TestAllocator ta;
Obj x(&ta);

TestClass1 testObj[NUM_THREADS]; // one test object for each thread

extern "C" {
void *workerThread10(void *arg)
{
    int id = (int)(bsls::Types::IntPtr)arg;

    barrier.wait();
    switch(id % 2) {

      // even numbered threads run 'case 0:'
      case 0: {
          if (veryVerbose) {
              printMutex.lock();
              cout << "\tStart event iterations" << endl;
              printMutex.unlock();
          }
          for (int i = 0; i< NUM_ITERATIONS; ++i) {
              bsls::TimeInterval now = bdlt::CurrentTime::now();
              EventHandle h;
              x.scheduleEvent(&h,
                              now + T6,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj[id]));
              if (veryVeryVerbose) {
                  printMutex.lock();
                  cout << "\t\tAdded event: "; P_(id); P_(i);
                  printMutex.unlock();
              }
              if (0 != x.cancelEvent(h) && !testTimingFailure) {
                  // We tried and the 'cancelEvent' failed, but we do not want
                  // to generate an error unless we can *guarantee* that the
                  // 'cancelEvent' should have succeeded.

                  bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                  LOOP2_ASSERTT(id, i, elapsed < T6);
                  testTimingFailure = (elapsed >= T6);
              }
          }
      }
      break;

      // odd numbered threads run 'case 1:'
      case 1: {
          if (veryVerbose) {
              printMutex.lock();
              cout << "\tStart clock iterations" << endl;
              printMutex.unlock();
          }
          for (int i = 0; i< NUM_ITERATIONS; ++i) {
              bsls::TimeInterval now = bdlt::CurrentTime::now();
              RecurringEventHandle h;
              x.scheduleRecurringEvent(
                  &h,
                  T6,
                  bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj[id]));
              if (veryVeryVerbose) {
                  printMutex.lock();
                  cout << "\t\tAdded clock: "; P_(id); P_(i);
                  printMutex.unlock();
              }
              if (0 != x.cancelEvent(h) && !testTimingFailure) {
                  // We tried and the 'cancelRecurringEvent' failed, but we do
                  // not want to generate an error unless we can *guarantee*
                  // that the 'cancelRecurringEvent' should have succeeded.

                  bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                  LOOP2_ASSERTT(id, i, elapsed < T6);
                  testTimingFailure = (elapsed >= T6);
              }
          }
      }
      break;
    };

    return NULL;
}
} // extern "C"

}  // close namespace EVENTSCHEDULER_TEST_CASE_10
// ============================================================================
//                          CASE 9 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_9 {

}  // close namespace EVENTSCHEDULER_TEST_CASE_9

// ============================================================================
//                          CASE 8 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_8 {

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply executes the specified
    // 'functor'.
{
    functor();
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_8

// ============================================================================
//                          CASE 7 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_7 {

void schedulingCallback(Obj        *scheduler,
                        TestClass1 *event,
                        TestClass1 *clock)
    // Schedule the specified 'event' and the specified 'clock' on the
    // specified 'scheduler'.
{
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T4(4 * DECI_SEC);

    scheduler->scheduleEvent(
                         bdlt::CurrentTime::now() + T,
                         bdlf::MemFnUtil::memFn(&TestClass1::callback, event));

    scheduler->scheduleRecurringEvent(
                         T4,
                         bdlf::MemFnUtil::memFn(&TestClass1::callback, clock));
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_7

// ============================================================================
//                          CASE 6 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_6 {

bslma::TestAllocator *pta;

struct Test6_0 {
    Test6_0() {}

    void operator()();
};

void Test6_0::operator()()
{
    // Schedule clocks starting at T3 and T5, invoke 'cancelAllClocks' at time
    // T and make sure that both are cancelled successfully.

    const int T = 1 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const bsls::TimeInterval T5(5 * DECI_SEC);
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    Obj x(pta); x.start();
    TestClass1 testObj1;
    TestClass1 testObj2;

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    x.scheduleRecurringEvent(
                     T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

    x.scheduleRecurringEvent(
                     T5,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

    bslmt::ThreadUtil::microSleep(T, 0);
    bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
    if (elapsed < T2) {
        // put a little margin between this and the first clock (T3).

        x.cancelAllEventsAndWait();
        microSleep(T6, 0);
        ASSERT( 0 == testObj1.numExecuted() );
        ASSERT( 0 == testObj2.numExecuted() );
    }
    x.stop();
}

struct Test6_1 {
    Test6_1() {}

    void operator()();
};

void Test6_1::operator()()
{
    // Schedule clocks c1 at T(which executes for T10 time), c2 at T2 and c3 at
    // T3.  Let all clocks be simultaneously put onto the pending list (this is
    // done by sleeping enough time before starting the scheduler).  Let c1's
    // first execution be started (by sleeping enough time), invoke
    // 'cancelAllClocks' without wait argument, verify that c1's first
    // execution has not yet completed and verify that c2 and c3 are cancelled
    // without any executions.

    bsls::TimeInterval  T(1 * DECI_SEC);
    bsls::TimeInterval  T2(2 * DECI_SEC);
    bsls::TimeInterval  T3(3 * DECI_SEC);
    bsls::TimeInterval  T20(20 * DECI_SEC);

    const int TM4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const int TM20 = 20 * DECI_SEC_IN_MICRO_SEC;

    Obj x(pta);

    TestClass1 testObj1(TM20);
    TestClass1 testObj2;
    TestClass1 testObj3;

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    x.scheduleRecurringEvent(
                      T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1),
                      now - T3);

    x.scheduleRecurringEvent(
                      T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2),
                      now - T2);

    x.scheduleRecurringEvent(
                      T3,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3),
                      now - T);

    x.start();
    microSleep(TM4, 0);  // let the callback of 'testObj1' be started
    x.cancelAllEvents();
    if ((bdlt::CurrentTime::now() - now) < T20) {
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );

        x.stop();
        LOOP_ASSERT(testObj1.numExecuted(), 1 == testObj1.numExecuted());
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );
    }
    else {
        if (verbose) ET("Test6_1: Timed out");
    }
    x.stop();
}

struct Test6_2 {
    Test6_2() {}

    void operator()();
};

void Test6_2::operator()()
{
    // Schedule clocks c1 at T(which executes for T10 time), c2 at.  T2 and c3
    // at T3.  Let all clocks be simultaneously put onto the pending list (this
    // is done by sleeping enough time before starting the scheduler).  Let
    // c1's first execution be started (by sleeping enough time), invoke
    // 'cancelAllClocks' with wait argument, verify that c1's first execution
    // has completed and verify that c2 and c3 are cancelled without any
    // executions.

    bsls::TimeInterval  T(1 * DECI_SEC);
    bsls::TimeInterval  T2(2 * DECI_SEC);
    bsls::TimeInterval  T3(3 * DECI_SEC);
    bsls::TimeInterval  T20(20 * DECI_SEC);

    const int TM4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const int TM20 = 20 * DECI_SEC_IN_MICRO_SEC;
    const int TM40 = 40 * DECI_SEC_IN_MICRO_SEC;

    if (veryVerbose) {
        P_(TM4); P_(TM20); P(TM40);
    }
    Obj x(pta);

    TestClass1 testObj1(TM20);
    TestClass1 testObj2;
    TestClass1 testObj3;

    bsls::TimeInterval now = bdlt::CurrentTime::now();
    x.scheduleRecurringEvent(
                      T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1),
                      now - T3);

    x.scheduleRecurringEvent(
                      T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2),
                      now - T2);

    x.scheduleRecurringEvent(
                      T3,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3),
                      now - T);

    x.start();
    microSleep(TM4, 0); // let the callback of 'testObj1' be started
    double elapsed = (bdlt::CurrentTime::now() - now).totalSecondsAsDouble();
    x.cancelAllEventsAndWait();
    if (elapsed < 1.0) {
        LOOP_ASSERT(testObj1.numExecuted(), 1 == testObj1.numExecuted());
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );

        x.stop();
        LOOP_ASSERT(testObj1.numExecuted(), 1 == testObj1.numExecuted());
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );
    }
    else {
        if (verbose) ET("Test6_2: timed out");
    }
    x.stop();
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_6

// ============================================================================
//                          CASE 5 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_5 {

bslma::TestAllocator *pta;

struct Test5_0 {
    Test5_0() {}

    void operator()()
    {
        // Schedule a clock starting at T2, cancel it at time T and verify that
        // it has been successfully cancelled.

        const int TM4 = 4 * DECI_SEC_IN_MICRO_SEC;
        const bsls::TimeInterval T10(10 * DECI_SEC);

        Obj x(pta); x.start();
        TestClass1 testObj;

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        RecurringEventHandle h;
        x.scheduleRecurringEvent(
                      &h,
                      T10,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        bslmt::ThreadUtil::microSleep(TM4, 0);
        ASSERT( 0 == x.cancelEvent(h) );
        bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
        if (elapsed < T10) {
            x.stop();
            ASSERT( 0 == testObj.numExecuted() );
        }
        else {
            if (verbose) ET("Test5_0: timed out");
        }
        x.stop();
    }
};

struct Test5_1 {
    Test5_1() {}

    void operator()()
    {
        // Schedule two clocks c1 and c2 starting at T and T2.  Let both be
        // simultaneously put onto the pending list (this is done by sleeping
        // enough time before starting the scheduler), cancel c2 before it's
        // callback is dispatched and verify the result.

        bsls::TimeInterval  T(1 * DECI_SEC);
        const bsls::TimeInterval T2(2 * DECI_SEC);
        const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
        const int T20 = 20 * DECI_SEC_IN_MICRO_SEC;
        Obj x(pta);
        TestClass1 testObj1(T20);
        TestClass1 testObj2;

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        x.scheduleRecurringEvent(
                      T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1),
                      now - T2);

        RecurringEventHandle h;
        x.scheduleRecurringEvent(
                      &h,
                      T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2),
                      now - T);
        x.start();
        microSleep(T3, 0);
        ASSERT( 0 == x.cancelEvent(h) );
        if ((bdlt::CurrentTime::now() - now) <
                                           bsls::TimeInterval(20 * DECI_SEC)) {
            x.stop();
            LOOP_ASSERT(testObj1.numExecuted(), 1 == testObj1.numExecuted());
            ASSERT(0 == testObj2.numExecuted());
        }
        else {
            if (verbose) ET("Test5_1: timed out");
        }
        x.stop();
    }
};

struct Test5_2 {
    Test5_2() {}

    void operator()()
    {
        // Schedule a clock (whose callback executes for T5 time) starting at
        // T.  Let its first execution be started (by sleeping for T2 time),
        // cancel it without wait argument, verify that its execution has not
        // yet completed, make sure that it is cancelled after this execution.

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        bsls::TimeInterval T(   1 * DECI_SEC);
        bsls::TimeInterval T4(  4 * DECI_SEC);
        bsls::TimeInterval T20(20 * DECI_SEC);
        bsls::TimeInterval T40(40 * DECI_SEC);
        const int TM5 = 5 * DECI_SEC_IN_MICRO_SEC;
        const int TM20 = 20 * DECI_SEC_IN_MICRO_SEC;
        Obj x(pta);
        x.start();

        TestClass1 testObj(TM20);

        RecurringEventHandle h;
        x.scheduleRecurringEvent(
                      &h,
                      T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        microSleep(TM5, 0);
        ASSERT( 0 == x.cancelEvent(h) );
        bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
        if (elapsed < T20) {
            ASSERT( 0 == testObj.numExecuted() );
        }
        makeSureTestObjectIsExecuted(testObj, mT, 400);
        elapsed = bdlt::CurrentTime::now() - now;
        if (elapsed < T40) {
            LOOP_ASSERT(testObj.numExecuted(), 1 == testObj.numExecuted());
        }
        else {
            if (verbose) ET("Test5_2: timed out");
        }
        x.stop();
    }
};

struct Test5_3 {
    Test5_3() {}

    void operator()()
    {
#if 0
        // Schedule a clock (whose callback executes for T5 time)
        // starting at T.  Let its first execution be started (by
        // sleeping for T2 time), cancel it with wait argument, verify
        // that its execution has completed, make sure that it is
        // cancelled after this execution.

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        bsls::TimeInterval  T(1 * DECI_SEC);
        const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
        const int T5 = 5 * DECI_SEC_IN_MICRO_SEC;
        bsls::TimeInterval  T10(10 * DECI_SEC);
        Obj x(pta);
        x.start();

        TestClass1 testObj(T5);

        RecurringEventHandle h;
        x.scheduleRecurringEvent(
                      &h,
                      T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        microSleep(T3, 0);
        ASSERT( 0 == x.cancelEventAndWait(h) );
        ASSERT( 1 == testObj.numExecuted() );
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        if (bdlt::CurrentTime::now() - now < T10) {
            ASSERT( 1 == testObj.numExecuted() );
        }
        x.stop();
#endif
    }
};

}  // close namespace EVENTSCHEDULER_TEST_CASE_5

// ============================================================================
//                          CASE 4 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_4 {

bslma::TestAllocator *pta;

struct Test4_0 {
    Test4_0() {}

    void operator()()
    {
        // Schedule events at T5, T10 and T15, invoke 'cancelAllEvents' before
        // any of them have a chance to get pending, and verify the result.

        const bsls::TimeInterval T5(  5 * DECI_SEC);
        const bsls::TimeInterval T10(10 * DECI_SEC);
        const bsls::TimeInterval T15(15 * DECI_SEC);

        Obj x(pta); x.start();
        TestClass1 testObj1;
        TestClass1 testObj2;
        TestClass1 testObj3;

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        EventHandle h1, h2, h3;
        x.scheduleEvent(
                     &h1,
                     now + T5,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

        x.scheduleEvent(
                     &h2,
                     now + T10,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

        x.scheduleEvent(
                     &h3,
                     now + T15,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));

        x.cancelAllEvents();
        ASSERT( 0 != x.cancelEvent(h1) );
        ASSERT( 0 != x.cancelEvent(h2) );
        ASSERT( 0 != x.cancelEvent(h3) );
        x.stop();
    }
};

struct Test4_1 {
    Test4_1() {}

    void operator()()
    {
        // Schedule events e1, e2 and e3 at T, T2 and T8 respectively.  Let the
        // scheduler to start executing e1 (this is done by sleeping enough
        // time before starting the scheduler).  Invoke 'cancelAllEvents' and
        // ensure that it cancels e2 and e3 and does not wait for e1 to
        // complete its execution.

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        bsls::TimeInterval  T(1 * DECI_SEC);
        const bsls::TimeInterval T2(2 * DECI_SEC);
        const bsls::TimeInterval T8(8 * DECI_SEC);
            // This will not be put onto the pending list.

        const int TM3  =  3 * DECI_SEC_IN_MICRO_SEC;
        const int TM20 = 20 * DECI_SEC_IN_MICRO_SEC;

        Obj x(pta);

        TestClass1 testObj1(TM20);
        TestClass1 testObj2;
        TestClass1 testObj3;
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        x.scheduleEvent(
                     now - T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

        x.scheduleEvent(
                     now,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

        x.scheduleEvent(
                     now + T8,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));

        x.start();
        microSleep(TM3, 0);     // give enough time to put on pending list
        x.cancelAllEvents();    // testObj1 is pending, 2 & 3 get killed
        if ((bdlt::CurrentTime::now() - now).totalSecondsAsDouble() < 2.0) {
            LOOP_ASSERT(testObj1.numExecuted(), 0 == testObj1.numExecuted());
            LOOP_ASSERT(testObj2.numExecuted(), 0 == testObj2.numExecuted());
            LOOP_ASSERT(testObj3.numExecuted(), 0 == testObj3.numExecuted());
        }
        else if (verbose) {
            ET("Test4_1: timed out");
        }

        makeSureTestObjectIsExecuted(testObj1, mT, 400);
        LOOP_ASSERT(testObj1.numExecuted(), 1 == testObj1.numExecuted());
        LOOP_ASSERT(testObj2.numExecuted(), 0 == testObj2.numExecuted());
        LOOP_ASSERT(testObj3.numExecuted(), 0 == testObj3.numExecuted());
        x.stop();
    }
};

struct Test4_2 {
    Test4_2() {}

    void operator()()
    {
        // Schedule events e1, e2 and e3 at T, T2 and T8 respectively.  Let the
        // scheduler to start executing e1 (this is done by sleeping enough
        // time before starting the scheduler).  Invoke 'cancelAllEvents' with
        // wait argument and ensure that it cancels e3 and e2 wait for e1 to
        // complete its execution.

        bsls::TimeInterval  T(1 * DECI_SEC);
        const bsls::TimeInterval T2(2 * DECI_SEC);
        const bsls::TimeInterval T5(5 * DECI_SEC);
        const bsls::TimeInterval T8(8 * DECI_SEC);

        const int T3  = 3 * DECI_SEC_IN_MICRO_SEC;
        const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

        Obj x(pta);

        TestClass1 testObj1(T10);
        TestClass1 testObj2;
        TestClass1 testObj3;
        bsls::TimeInterval now = bdlt::CurrentTime::now();
        x.scheduleEvent(
                     now - T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

        x.scheduleEvent(
                     now - T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

        x.scheduleEvent(
                     now + T5,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));

        x.start();
        microSleep(T3, 0); // give enough time to put on pending list
        bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
        x.cancelAllEventsAndWait();
        ASSERT( 1 == testObj1.numExecuted() );
        if (elapsed < T8) {
            ASSERT( 0 == testObj2.numExecuted() );
            ASSERT( 0 == testObj3.numExecuted() );
            x.stop();
            ASSERT( 1 == testObj1.numExecuted() );
            ASSERT( 0 == testObj2.numExecuted() );
            ASSERT( 0 == testObj3.numExecuted() );
        }
        else if (verbose) {
            ET("Test4_2: timed out");
        }
        x.stop();
    }
};

}  // close namespace EVENTSCHEDULER_TEST_CASE_4

// ============================================================================
//                          CASE 3 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_3 {

}  // close namespace EVENTSCHEDULER_TEST_CASE_3

// ============================================================================
//                          CASE 2 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_2 {

struct TestCase2Data {
    int               d_line;
    int               d_startTime;         // in 1/10th of a sec.
    bool              d_isClock;
    int               d_periodicInterval;  // in 1/10th of a sec.
    int               d_executionTime;     // in 1/10th of a sec.
    bool              d_delayed;
};

bool testCallbacks(int                  *failures,
                   const float           totalTime,
                   const TestCase2Data  *DATA,
                   const int             NUM_DATA,
                   TestClass           **testObjects)
{
    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    bsls::TimeInterval globalLastExecutionTime = bdlt::CurrentTime::now();

    bool assertOnFailure = (failures == 0);

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    // the thinking here is that this loop, scheduling the events, will take
    // insignificant time
    for (int i = 0; i < NUM_DATA; ++i) {
        const int  LINE             = DATA[i].d_line;
        const int  STARTTIME        = DATA[i].d_startTime;
        const bool ISCLOCK          = DATA[i].d_isClock;
        const int  PERIODICINTERVAL = DATA[i].d_periodicInterval;
        const int  EXECUTIONTIME    = DATA[i].d_executionTime;

        if (ISCLOCK) {
            testObjects[i] = new TestClass(
                    LINE,
                    now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                    bsls::TimeInterval(PERIODICINTERVAL * DECI_SEC),
                    &globalLastExecutionTime,
                    EXECUTIONTIME * DECI_SEC_IN_MICRO_SEC,
                    assertOnFailure);

            x.scheduleRecurringEvent(
                  bsls::TimeInterval(PERIODICINTERVAL * DECI_SEC),
                  bdlf::MemFnUtil::memFn(&TestClass::callback, testObjects[i]),
                  now + bsls::TimeInterval(STARTTIME * DECI_SEC));
        }
        else {
            testObjects[i] = new TestClass(
                    LINE,
                    now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                    &globalLastExecutionTime,
                    EXECUTIONTIME * DECI_SEC_IN_MICRO_SEC,
                    assertOnFailure);

            x.scheduleEvent(
                 now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                 bdlf::MemFnUtil::memFn(&TestClass::callback, testObjects[i]));
        }
    }

    x.start();
    float delta = .5;
    microSleep((totalTime + delta) * DECI_SEC_IN_MICRO_SEC, 0);
    x.stop();
    double finishTime = bdlt::CurrentTime::now().totalSecondsAsDouble();
    finishTime = (finishTime - now.totalSecondsAsDouble()) * 10 - delta;
    // if the 'microSleep' method slept for exactly how long we asked it to,
    // 'finishTime' should equal totalTime, but microSleep often lags.  By a
    // lot.

    bool result = true;
    for (int i = 0; i < NUM_DATA; ++i) {
        const int  LINE             = DATA[i].d_line;
        const int  STARTTIME        = DATA[i].d_startTime;
        const bool ISCLOCK          = DATA[i].d_isClock;
        const int  PERIODICINTERVAL = DATA[i].d_periodicInterval;
        const bool DELAYED          = DATA[i].d_delayed;

        if (veryVerbose) {
            cout << *testObjects[i] << endl;
        }
        if (testObjects[i]->numFailures()) {
            result = false;
            if (!assertOnFailure) {
                ++(*failures); // counts number of slightly delayed events
            }
        }
        if (assertOnFailure) {
            LOOP_ASSERTT(LINE, DELAYED == testObjects[i]->delayed());
        } else if (DELAYED != testObjects[i]->delayed()) {
            result = false;
            *failures += 10000; // large number to trigger overall failure
        }
        if (ISCLOCK) {
            if (!testObjects[i]->delayed()) {
                int n1 = (finishTime - STARTTIME) / PERIODICINTERVAL + 1;
                    // this formula is bogus in general but works for the data
                    // and relies on totalTime being a float
                int n2 = testObjects[i]->numExecuted();
                if (assertOnFailure) {
                    LOOP3_ASSERTT(LINE, n1, n2, n1 == n2);
                } else if (n1 != n2) {
                    result = false;
                    *failures += 10000; // idem
                }
            }
        }
        else {
            if (assertOnFailure) {
                LOOP_ASSERTT(LINE, 1 == testObjects[i]->numExecuted());
            } else if (1 != testObjects[i]->numExecuted()) {
                result = false;
            }
        }
        delete testObjects[i];
    }
    return result;
}

}  // close namespace EVENTSCHEDULER_TEST_CASE_2

// ============================================================================
//                          CASE 1 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_1 {

}  // close namespace EVENTSCHEDULER_TEST_CASE_1


// ============================================================================
//                         CASE -1 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace EVENTSCHEDULER_TEST_CASE_MINUS_1 {

                            // ====================
                            // class TestPrintClass
                            // ====================

struct TestPrintClass {
    // This class define a function 'callback' that prints a message.  This
    // class is intended for use to verify changes to the system clock do or do
    // not affect the behavior of the scheduler (see Test Case -1).

    bsls::AtomicInt d_numExecuted;

    // CREATORS
    TestPrintClass() :
    d_numExecuted(0)
    {
    }

    // MANIPULATORS
    void callback()
    {
        ++d_numExecuted;
        cout << "iteration: " << d_numExecuted << endl;
    }

    // ACCESSORS
    int numExecuted()
    {
        return d_numExecuted;
    }
};

}  // close namespace EVENTSCHEDULER_TEST_CASE_MINUS_1

// ============================================================================
//                        CASE -100 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TEST_CASE_MINUS_100 {

enum {
    NUM_THREADS    = 4,
    NUM_ITERATIONS = 100,
    SEND_COUNT = 1000,
    RCV_COUNT = 900,
    DELAY = 2
};

TestClass1 testObj;

void threadFunc(bdlmt::EventScheduler *scheduler,
                int                    numIterations,
                int                    sendCount,
                int                    rcvCount,
                int                    delay)
{
    bsl::vector<bdlmt::EventScheduler::Event*> timers;
    timers.resize(sendCount);

    bsls::Stopwatch sw;

    for (int i=0; i<numIterations; ++i) {
        if ( verbose ) {
            sw.start();
        }

        bsls::TimeInterval n = bdlt::CurrentTime::now();

        // "send" messages
        for (int snd=0; snd<sendCount; ++snd) {
            bsls::TimeInterval d(delay, snd);
            scheduler->scheduleEventRaw(
                      &timers[snd],
                      n + d,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
        }

        // "receive" replies
        for (int rcv=0; rcv<rcvCount; ++rcv) {
            scheduler->cancelEvent(timers[rcv]);
        }

        // release handles
        for (int j=0; j<sendCount; ++j) {
            scheduler->releaseEventRaw(timers[j]);
        }

        if (verbose) {
            sw.stop();
            int iteration = i;
            double elapsed = sw.elapsedTime();
            P(iteration);
            P(elapsed);
        }
    }
}

void run()
{
    if (verbose) cout << endl
                      << "The router simulation (kind of) test" << endl
                      << "==================================" << endl;

    bdlmt::EventScheduler scheduler;

    scheduler.start();

    bslmt::ThreadUtil::Handle threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                  bdlf::BindUtil::bind(&threadFunc,
                                                       &scheduler,
                                                       (int)NUM_ITERATIONS,
                                                       (int)SEND_COUNT,
                                                       (int)RCV_COUNT,
                                                       (int)DELAY));
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    scheduler.stop();
}

}  // close namespace TEST_CASE_MINUS_100

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    int nExec;

    bslma::TestAllocator ta;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 21: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLES:
        //
        // Concerns:
        //   The usage examples provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage examples from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE 1" << endl
                          << "=======================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        using namespace EVENTSCHEDULER_TEST_CASE_USAGE;
        bslma::DefaultAllocatorGuard defaultAllocGuard(&ta);
        {
            bdlmt::EventScheduler scheduler;
            vector<Value> values;

            scheduler.scheduleRecurringEvent(
                                     bsls::TimeInterval(1.5),
                                     bdlf::BindUtil::bind(&saveData, &values));
            scheduler.start();
            bdlt::Datetime start = bdlt::CurrentTime::utc();
            while ((bdlt::CurrentTime::utc() -
                                               start).totalSeconds() < 7) {
                ++g_data;
            }
            scheduler.stop();
            ASSERT(values.size() >= 4);
            if (verbose) {
                for (int i = 0; i < (int) values.size(); ++i) {
                    cout << "At " << values[i].first << " g_data was "
                         << values[i].second << endl;
                }
            }

            if (verbose) cout << endl
                              << "TESTING USAGE EXAMPLE 2" << endl
                              << "=======================" << endl;
            // Note: this is just a compile test
            my_Server server(bsls::TimeInterval(10), &ta);
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'bdlmt::EventScheduler(disp, clockType, alloc = 0)':
        //   Verify this 'bdlmt::EventScheduler' creator, which uses the
        //   specified dispatcher and specified clock, correctly schedules
        //   events.
        //
        // Concerns:
        //: 1 The creator with a user-defined dispatcher and clock correctly
        //:   initializes the object.
        //
        // Plan:
        //: 1 Define a dispatcher that simply executes the specified
        //    functor.  Create a scheduler using this dispatcher and the
        //    monotonic clock, schedule an event and make sure that it is
        //    executed by the specified dispatcher.
        //
        // Testing:
        //   bdlmt::EventScheduler(disp, clockType, alloc = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::EventScheduler"
                          << "(disp, clockType, alloc = 0)'" << endl
                          << "============================"
                          << "=============================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_20;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::EventScheduler::Dispatcher dispatcher =
                                 bdlf::BindUtil::bind(&dispatcherFunction, _1);

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(dispatcher, bsls::SystemClockType::e_MONOTONIC, &ta); x.start();

        TestClass1 testObj;

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        microSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );
        x.stop();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'bdlmt::EventScheduler(clockType, allocator = 0)': Verify
        // this 'bdlmt::EventScheduler' creator, which uses the default
        // dispatcher and specified clock, correctly schedules events.
        //
        // Concerns:
        //: 1 The creator with the default dispatcher and specified clock
        //:   correctly initializes the object.
        //
        // Plan:
        //: 1 Create a scheduler using the default dispatcher and the monotonic
        //    clock, schedule an event and make sure that it is executed by the
        //    default dispatcher.
        //
        // Testing:
        //   bdlmt::EventScheduler(clockType, allocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::EventScheduler"
                          << "(clockType, alloc = 0)'" << endl
                          << "============================"
                          << "=======================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_19;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(bsls::SystemClockType::e_MONOTONIC, &ta); x.start();

        TestClass1 testObj;

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        microSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );
        x.stop();

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING REDUNDANT CANCEL{RECURRING}EVENT{ANDWAIT} ON HANDLES
        //
        // Concerns:
        //   That 'cancelEvent{AndWait}' and 'cancelRecurringEvent{AndWait}'
        //   work properly with regard to releasing handles and not segfaulting
        //   or aborting when called on handles that are already released.
        //
        // Plan:
        //   Create a scheduler and schedule an event on it, far in the future
        //   so that the test will be over before the event occurs.  When
        //   creating the event, bind an handle to it.  Cancel the event twice.
        //   Verify the state of the queue throughout this process by
        //   monitoring 'scheduler.numEvents()' or
        //   'scheduler.numRecurrringEvents()', verify that the return codes
        //   from the cancel calls are what they should be, and verify that the
        //   handle is in the correct state by casting it to a pointer (a
        // released handle will cast to a null pointer, a bound handle will
        // cast to some other value).
        //
        //   The same test is repeated 4 times, for every combination of handle
        //   type and cancel method:
        //
        //   Handle Type                Cancel Method
        //   -----------                -------------
        //   EventHandle                cancelEvent
        //   EventHandle                cancelEventAndWait
        //   RecurringEventHandle       cancelEvent
        //   RecurringEventHandle       cancelEventAndWait
        // --------------------------------------------------------------------

        if (verbose) cout <<
              "TESTING REDUNDANT CANCEL{RECURRING}EVENT{ANDWAIT} ON HANDLES\n"
              "============================================================\n";

        bslma::TestAllocator ta;
        Obj scheduler(&ta);

        const bsls::TimeInterval farFuture = bdlt::CurrentTime::now() + 10.0;
        const bsls::TimeInterval oneSec(1.0);

        Obj::EventHandle eh;
        scheduler.scheduleEvent(&eh, farFuture, &mustBeCancelledCallBack);
        ASSERT(1 == scheduler.numEvents());
        ASSERT(0 != (const Obj::Event *) eh);

        int rc;
        rc = scheduler.cancelEvent(&eh);
        ASSERT(0 == rc);
        ASSERT(0 == scheduler.numEvents());
        ASSERT(0 == (const Obj::Event *) eh);

        rc = scheduler.cancelEvent(&eh);
        ASSERT((bdlcc::SkipList<int, int>::e_INVALID == rc));
        ASSERT(0 == scheduler.numEvents());
        ASSERT(0 == (const Obj::Event *) eh);

        scheduler.scheduleEvent(&eh, farFuture, &mustBeCancelledCallBack);
        ASSERT(1 == scheduler.numEvents());
        ASSERT(0 != (const Obj::Event *) eh);

        rc = scheduler.cancelEventAndWait(&eh);
        ASSERT(0 == rc);
        ASSERT(0 == scheduler.numEvents());
        ASSERT(0 == (const Obj::Event *) eh);

        rc = scheduler.cancelEventAndWait(&eh);
        ASSERT((bdlcc::SkipList<int, int>::e_INVALID == rc));
        ASSERT(0 == scheduler.numEvents());
        ASSERT(0 == (const Obj::Event *) eh);

        Obj::RecurringEventHandle reh;
        scheduler.scheduleRecurringEvent(&reh,
                                         oneSec,
                                         &mustBeCancelledCallBack,
                                         farFuture);
        ASSERT(1 == scheduler.numRecurringEvents());
        ASSERT(0 != (const Obj::RecurringEvent *) reh);

        rc = scheduler.cancelEvent(&reh);
        ASSERT(0 == rc);
        ASSERT(0 == scheduler.numRecurringEvents());
        ASSERT(0 == (const Obj::RecurringEvent *) reh);

        rc = scheduler.cancelEvent(&reh);
        ASSERT((bdlcc::SkipList<int, int>::e_INVALID == rc));
        ASSERT(0 == scheduler.numRecurringEvents());
        ASSERT(0 == (const Obj::RecurringEvent *) reh);

        scheduler.scheduleRecurringEvent(&reh,
                                         oneSec,
                                         &mustBeCancelledCallBack,
                                         farFuture);
        ASSERT(1 == scheduler.numRecurringEvents());
        ASSERT(0 != (const Obj::RecurringEvent *) reh);

        rc = scheduler.cancelEventAndWait(&reh);
        ASSERT(0 == rc);
        ASSERT(0 == scheduler.numRecurringEvents());
        ASSERT(0 == (const Obj::RecurringEvent *) reh);

        rc = scheduler.cancelEventAndWait(&reh);
        ASSERT((bdlcc::SkipList<int, int>::e_INVALID == rc));
        ASSERT(0 == scheduler.numRecurringEvents());
        ASSERT(0 == (const Obj::RecurringEvent *) reh);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING PASSING ATTRIBUTES TO START:
        //
        // Concerns:
        //   That we are able to specify attributes of dispatcher thread by
        //   passing them to 'start()'.
        //
        // Plan:
        //   Configure the stack size to be much bigger than the default,
        //   then verify that it really is bigger.  Note: veryVeryVerbose will
        //   deliberately make this test crash to find out how big the default
        //   stack size is (just displaying attr.stackSize() just shows 0 if
        //   you let stackSize default on Solaris).
        //
        //   Also verify that DETACHED attribute is ignored by setting the
        //   attributes passed to be DETACHED and then doing a 'stop()', which
        //   will join with the dispatcher thread.
        //
        // Testing:
        //   int start(const bslmt::ThreadAttributes& threadAttributes);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Attributes test\n";
        }

        using namespace EVENTSCHEDULER_TEST_CASE_17;

        bslmt::ThreadAttributes attr;

        if (!veryVeryVerbose) {
            attr.setStackSize(80 * 1000 * 1000);
        }
        attr.setDetachedState(bslmt::ThreadAttributes::e_CREATE_DETACHED);

        if (verbose) {
            cout << "StackSize: " << attr.stackSize() << endl;
        }

        // As of 10/16/08, the default stack size was about 1MB on Solaris,
        // 0.25MB on AIX, and 10MB on Linux.
        Recurser r;
        r.d_recurseDepth = 79 * 1000 * 1000;
        r.s_finished = false;

        bdlmt::EventScheduler scheduler;
        int sts = scheduler.start(attr);
        ASSERT(!sts);

        scheduler.scheduleEvent(bdlt::CurrentTime::now(),
                                r);

        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(300 * 1000);    // get event pending

        scheduler.stop();

        if (verbose) {
            cout << (r.s_finished ? "Finished OK" : "Error -- did not finish")
                                                                       << endl;
        }

        ASSERT(r.s_finished);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ACCUMULATED TIME LAG, WITH EVENTS
        //
        // Concerns:
        //   That clocked events happen at regular intervals without
        //   accumulating time lag.
        //
        // Plan:
        //   Schedule a bunch of clocks as well as a couple of events,
        //   measure when they happen and make sure the times are within
        //   constant tolerances.
        //
        // Caveat:
        //   The multithreaded primitives for sleeping and waiting sometimes
        //   go far overtime -- if that happens, abort the test and start
        //   over.  The test will be attempted a maximum of 5 times before
        //   declaring failure.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting accumulated time lag, with events\n"
                               "=========================================\n";

        // reusing
        using namespace EVENTSCHEDULER_TEST_CASE_15;

        int ii;
        enum { MAX_LOOP = 4 };
        for (ii = 0; ii <= MAX_LOOP; ++ii) {
            bdlmt::EventScheduler scheduler(&ta);
            scheduler.start();
            SlowFunctor sf;
            FastFunctor ff;

            const double CLOCKTIME1 = sf.SLEEP_SECONDS / 2;

            scheduler.scheduleRecurringEvent(
                         bsls::TimeInterval(CLOCKTIME1),
                         bdlf::BindUtil::bind(&SlowFunctor::callback,
                                              &sf,
                                              veryVeryVerbose
                                                  ? (bdlt::CurrentTime::now() +
                                                     bsls::TimeInterval(2.004))
                                                        .totalMicroseconds()
                                                  : 0));
            bsls::TimeInterval afterStarted = bdlt::CurrentTime::now();

            scheduler.scheduleEvent(
                afterStarted + bsls::TimeInterval(2.0),
                bdlf::BindUtil::bind(&FastFunctor::callback,
                                     &ff,
                                     veryVerbose));

            scheduler.scheduleEvent(
               afterStarted + bsls::TimeInterval(3.0),
               bdlf::BindUtil::bind(&FastFunctor::callback, &ff, veryVerbose));

            bslmt::ThreadUtil::microSleep(40 * sf.SLEEP_MICROSECONDS);
                            // wait 40 time execution time of sf.callback().

            scheduler.stop();

            bslmt::ThreadUtil::microSleep(20 * sf.SLEEP_MICROSECONDS);
                    // wait until events can run, verify that clock queue
                    // stayed small

            if (verbose) {
                cout << "...It is now interval    "
                     << bdlt::CurrentTime::now().totalMicroseconds()
                     << "\n...I scheduled events at "
                     << (afterStarted + bsls::TimeInterval(2.0))
                            .totalMicroseconds()
                     << "\n...And                   "
                     << (afterStarted + bsls::TimeInterval(3.0))
                            .totalMicroseconds() << endl;
            }

            int slowFunctorSize = sf.timeList().size();

            ASSERT(!(slowFunctorSize & 1)); // should not be odd element
            slowFunctorSize &= ~1;          // ignore odd element if there

            if (verbose) { P_(slowFunctorSize); P(ff.timeList().size()); }

            enum { MIN_SF_SIZE = 33*2, MAX_SF_SIZE = 44*2 };

            ASSERT(ii < MAX_LOOP || slowFunctorSize >= MIN_SF_SIZE);
            ASSERT(ii < MAX_LOOP || slowFunctorSize <= MAX_SF_SIZE);
            if (slowFunctorSize < MIN_SF_SIZE || slowFunctorSize>MAX_SF_SIZE) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_) P_(MIN_SF_SIZE) P_(MAX_SF_SIZE) P(slowFunctorSize);
                }
                continue;
            }
            ASSERT(ii < MAX_LOOP || 2 == ff.timeList().size());
            if (2 != ff.timeList().size()) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_) P(ff.timeList().size());
                }
                continue;
            }

            SlowFunctor::DateTimeList::iterator sfit = sf.timeList().begin();
            double start_time = sfit->first;
            if (verbose) P(afterStarted.totalSecondsAsDouble() - start_time);

            // Go through even elements - they should be right on time.

            double prevTime = start_time;
            bool startOver = false;
            for (int i = 2; i + 2 <= slowFunctorSize; ++sfit,++sfit, i += 2) {
                double diff = sfit->first - prevTime;
                prevTime = sfit->first;
                double offBy = diff - sf.SLEEP_SECONDS;
                if (veryVerbose) {
                    bsls::Types::Int64 interval = sfit->second;
                    P_(i); P_(offBy); P_(interval);
                    P(sf.tolerance(i / 2));
                }
                ASSERT(ii < MAX_LOOP || sf.tolerance(1) > abs(offBy));
                if (sf.tolerance(1) <= abs(offBy)) {
                    if (verbose || MAX_LOOP == ii) {
                        P_(L_) P_(i) P_(sf.tolerance(1)) P_(diff) P(offBy);
                    }
                    startOver = true;
                }
            }
            if (startOver) continue;

            // Go through odd elements - they should be SLEEP_SECONDS later.

            sfit = sf.timeList().begin();
            ++sfit;
            prevTime = sfit->first;
            ++sfit; ++sfit;
            for (int i = 3; i + 2 <= slowFunctorSize; ++sfit, ++sfit, i += 2) {
                double diff = sfit->first - prevTime;
                prevTime = sfit->first;
                double offBy = diff - sf.SLEEP_SECONDS;
                if (veryVerbose) {
                    bsls::Types::Int64 interval = sfit->second;
                    P_(i); P_(offBy); P_(interval);
                    P(sf.tolerance(i / 2));
                }
                ASSERT(ii < MAX_LOOP || sf.tolerance(1) > abs(offBy));
                if (sf.tolerance(1) <= abs(offBy)) {
                    if (verbose || MAX_LOOP == ii) {
                        P_(L_) P_(i) P_(sf.tolerance(1)) P_(diff) P(offBy);
                    }
                    startOver = true;
                }
            }
            if (startOver) continue;

            FastFunctor::DateTimeList::iterator ffit = ff.timeList().begin();

            double ffdiff = *ffit - afterStarted.totalSecondsAsDouble();
            double ffoffBy = ffdiff - 2;
            if (veryVerbose) {
                P_(ffdiff); P_(ffoffBy);
                P_(ff.TOLERANCE_AHEAD); P(ff.TOLERANCE_BEHIND);
            }
            ASSERT(ii < MAX_LOOP || -ff.TOLERANCE_AHEAD < ffoffBy);
            ASSERT(ii < MAX_LOOP || ff.TOLERANCE_BEHIND > ffoffBy);
            if (ffoffBy <= -ff.TOLERANCE_AHEAD ||
                ffoffBy >  ff.TOLERANCE_BEHIND) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_) P_(ff.TOLERANCE_AHEAD) P_(ff.TOLERANCE_BEHIND)
                    P_(ffdiff) P(ffoffBy);
                }
                continue;
            }

            ++ffit;
            ffdiff = *ffit - afterStarted.totalSecondsAsDouble();
            ffoffBy = ffdiff - 3;
            if (veryVerbose) { P_(ffdiff); P(ffoffBy); }
            ASSERT(ii < MAX_LOOP || -ff.TOLERANCE_AHEAD < ffoffBy);
            ASSERT(ii < MAX_LOOP || ff.TOLERANCE_BEHIND > ffoffBy);
            if (ffoffBy<=-ff.TOLERANCE_AHEAD || ffoffBy>=ff.TOLERANCE_BEHIND) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_) P_(ff.TOLERANCE_AHEAD) P_(ff.TOLERANCE_BEHIND)
                    P_(ffdiff) P(ffoffBy);
                }
                continue;
            }

            break;
        }
        ASSERT(ii <= MAX_LOOP);
        if (verbose) { P_(L_) P(ii); }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ACCUMULATED TIME LAG, WITHOUT EVENTS
        //
        // Concerns:
        //   That clocked events occur at regular intervals without
        //   accumulating time lags.
        //
        // Plan:
        //   Schedule a recurring clock, record when the clocks happen, and
        //   verify they occur within a constant tolerance.
        //
        // Caveat:
        //   The multithreaded primitives for sleeping and waiting sometimes
        //   go far overtime -- if that happens, abort the test and start
        //   over.  The test will be attempted a maximum of 5 times before
        //   declaring failure.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting accumulated time lag\n"
                               "============================\n";

        using namespace EVENTSCHEDULER_TEST_CASE_15;

        int ii;
        enum { MAX_LOOP = 4 };
        for (ii = 0; ii <= MAX_LOOP; ++ii) {
            bdlmt::EventScheduler scheduler(&ta);
            scheduler.start();
            SlowFunctor sf;
            const double CLOCKTIME1 = sf.SLEEP_SECONDS * 6;
            const double TOLERANCE_AHEAD  = sf.SLEEP_SECONDS * 0.3;
            const double TOLERANCE_BEHIND = sf.SLEEP_SECONDS * 4;

            bsls::TimeInterval schedulerStartTI = bdlt::CurrentTime::now();
            schedulerStartTI += CLOCKTIME1;
            scheduler.scheduleRecurringEvent(
                          bsls::TimeInterval(CLOCKTIME1),
                          bdlf::BindUtil::bind(&SlowFunctor::callback, &sf, 0),
                          schedulerStartTI);
            double schedulerStart = schedulerStartTI.totalSecondsAsDouble();

            bslmt::ThreadUtil::microSleep(60 * sf.SLEEP_MICROSECONDS);
                                                    // wait 20 clock cycles

            scheduler.stop();

            bslmt::ThreadUtil::microSleep(4 * sf.SLEEP_MICROSECONDS);
                    // let running tasks finish, test that clock queue did not
                    // get large

            int slowFunctorSize = sf.timeList().size();

            if (verbose) { P(slowFunctorSize); }

            slowFunctorSize &= ~1;          // ignore odd element if there

            ASSERT(ii < MAX_LOOP || slowFunctorSize >=  8 * 2);
            ASSERT(ii < MAX_LOOP || slowFunctorSize <= 11 * 2);
            if (slowFunctorSize < 8*2 || slowFunctorSize > 11*2) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_) P(slowFunctorSize);
                }
                continue;
            }

            SlowFunctor::DateTimeList::iterator it = sf.timeList().begin();
            double start_time = it->first;
            {
                double firstOffBy = it->first - schedulerStart;

                if (verbose) P(firstOffBy);
                ASSERT(ii < MAX_LOOP || firstOffBy > -TOLERANCE_AHEAD);
                ASSERT(ii < MAX_LOOP || firstOffBy < CLOCKTIME1);
                if (firstOffBy <= -TOLERANCE_AHEAD || firstOffBy > CLOCKTIME1){
                    if (verbose || MAX_LOOP == ii) {
                        P_(L_) P_(TOLERANCE_AHEAD) P_(CLOCKTIME1)
                        P(firstOffBy);
                    }
                    continue;
                }
            }

            // go through even elements - they should be right on time
            bool startOver = false;
            for (int i = 0;  i+2 <= slowFunctorSize;  ++it, ++it, i += 2) {
                double diff = it->first - start_time;
                double offBy = diff - (i / 2) * CLOCKTIME1;

                if (veryVerbose) { P_(i); P_(offBy); }

                ASSERT(ii < MAX_LOOP || offBy + TOLERANCE_AHEAD >= 0);
                ASSERT(ii < MAX_LOOP || TOLERANCE_BEHIND > abs(offBy));
                if (offBy < -TOLERANCE_AHEAD || abs(offBy) > TOLERANCE_BEHIND){
                    if (verbose || MAX_LOOP == ii) {
                        P_(L_) P_(i) P_(TOLERANCE_AHEAD) P_(TOLERANCE_BEHIND)
                        P_(diff) P(offBy);
                    }
                    startOver = true;
                }
            }
            if (startOver) continue;

            break;
        }
        ASSERT(ii <= MAX_LOOP);
        if (verbose) { P_(L_) P(ii); }

#if 0
        // We're commenting this out.  Testing the 'microSleep' method should
        // really be done in 'bslmt_threadutil.t.cpp'.  Also, we have been
        // having really annoying problems with this part of the test failing
        // in the nightly build but not reproducibly failing on test machines
        // during the day.  Perhaps testing the 'microSleep' method in bslmt
        // rather than bcep (while all the thread pools are testing with many
        // threads) will have better results.

        if (verbose) cout << "\nTesting with microSleep\n\n";

        // go through odd elements - these are the microSleep values

        it = sf.timeList().begin();
        ++it;
        for (int i = 1;  i+2 <= slowFunctorSize;  ++it, ++it, i += 2) {
            double diff = it->first - start_time;
            double offBy = (i/2)*CLOCKTIME1 + sf.SLEEP_SECONDS - diff;
            double lagBy = it->first - ((i/2)*CLOCKTIME1 + sf.SLEEP_SECONDS +
                                                               schedulerStart);

            if (veryVerbose) { P_(i); P_(offBy); P(lagBy); }

            LOOP2_ASSERT(i, lagBy, TOLERANCE/10 + lagBy >= 0);
            LOOP4_ASSERT(i, diff, offBy, TOLERANCE, abs(offBy) < TOLERANCE);
        }
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Maximum number of concurrently scheduled events.
        //
        // Concerns:
        //   Given 'numEvents' (where 'numEvents <= 2**24 - 1'), ensure that
        //   *at least* 'numEvents' may be concurrently scheduled.
        //
        // Plan:
        //   Define a values 'numEvents', then construct a timer event
        //   scheduler using this value.  In order to prevent any scheduled
        //   events from firing, leave the scheduler in the "stopped" state.
        //   Schedule events from multiple threads and ensure *at least*
        //   'numEvents' may be concurrently scheduled.  Cancel all events,
        //   then again schedule events from multiple threads and ensure the
        //   maximum number of events may again be scheduled.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MAX CONCURRENT EVENTS\n"
                               "=============================\n";

        using namespace EVENTSCHEDULER_TEST_CASE_13;

        bslma::TestAllocator ta;

        const int TEST_DATA[] = {
            (1 <<  3),     // some small value
            (1 << 17) - 2, // the default "minimum guarantee" from version 1
        };

        const int NUM_TESTS = sizeof TEST_DATA / sizeof *TEST_DATA;
        const int NUM_THREADS = 4;

        for (int testIter = 0; testIter < NUM_TESTS; ++testIter) {
            const int NUM_OBJECTS  = TEST_DATA[testIter];
            if (veryVerbose) {
                P_(testIter); P(NUM_OBJECTS);
            }

            Obj mX(&ta);

            bsls::AtomicInt    numAdded(0);
            bslmt::Barrier     barrier(NUM_THREADS + 1);
            bslmt::ThreadGroup threadGroup;

            threadGroup.addThreads(bdlf::BindUtil::bind(&scheduleEvent,
                                                        &mX,
                                                        NUM_OBJECTS,
                                                        &numAdded,
                                                        (bsls::AtomicInt *)0,
                                                        &barrier),
                                   NUM_THREADS);
            if (veryVerbose) {
                cout << "Waiting for event schedulers to start..." << endl;
            }
            barrier.wait();
            if (veryVerbose) {
                cout << "Waiting for event schedulers to stop..." << endl;
            }
            threadGroup.joinAll();

            ASSERT(numAdded       >= NUM_OBJECTS);
            ASSERT(mX.numEvents() >= NUM_OBJECTS);

            mX.cancelAllEvents();
            ASSERT(0 == mX.numEvents());
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Maximum number of concurrently scheduled events and
        // clocks.
        //
        // Concerns:
        //   Given 'numClocks' (where 'numClocks <= 2**24 - 1'), ensure that
        //   *at least* 'numClocks' may be concurrently scheduled.
        //
        // Plan:
        //   Define a value 'numClocks', then construct a timer event scheduler
        //   using this value.  In order to prevent any scheduled events from
        //   firing, leave the scheduler in the "stopped" state.  Schedule
        //   clocks from multiple threads and ensure *at least* 'numClocks'
        //   may be concurrently scheduled.  Cancel all events, then again
        //   schedule events from multiple threads and ensure the maximum
        //   number of clocks may again be scheduled.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MAX CONCURRENT CLOCKS\n"
                               "=============================\n";

        using namespace EVENTSCHEDULER_TEST_CASE_13;

        bslma::TestAllocator ta;

        const int TEST_DATA[] = {
            (1 <<  3),     // some small value
            (1 << 17) - 2, // the default "minimum guarantee" from version 1
        };

        const int NUM_TESTS = sizeof TEST_DATA / sizeof *TEST_DATA;
        const int NUM_THREADS = 4;

        for (int testIter = 0; testIter < NUM_TESTS; ++testIter) {
            const int NUM_OBJECTS  = TEST_DATA[testIter];
            if (veryVerbose) {
                P_(testIter); P(NUM_OBJECTS);
            }

            Obj mX(&ta);

            bsls::AtomicInt    numAdded(0);
            bslmt::Barrier     barrier(NUM_THREADS + 1);
            bslmt::ThreadGroup threadGroup;

            threadGroup.addThreads(
                                  bdlf::BindUtil::bind(&scheduleRecurringEvent,
                                                       &mX,
                                                       NUM_OBJECTS,
                                                       &numAdded,
                                                       (bsls::AtomicInt *)0,
                                                       &barrier),
                                  NUM_THREADS);
            if (veryVerbose) {
                cout << "Waiting for clock schedulers to start..." << endl;
            }
            barrier.wait();
            if (veryVerbose) {
                cout << "Waiting for clock schedulers to stop..." << endl;
            }
            threadGroup.joinAll();

            ASSERT(numAdded                >= NUM_OBJECTS);
            ASSERT(mX.numRecurringEvents() >= NUM_OBJECTS);

            mX.cancelAllEvents();
            ASSERT(0 == mX.numRecurringEvents());
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'rescheduleEvent':
        //
        // Concerns:
        //   That 'rescheduleEvent' should be successful when it is possible
        //   to reschedule it.
        //
        //   That 'rescheduleEvent' should fail when it not is possible
        //   to reschedule it.
        //
        // Plan:
        //   Create and start a scheduler object, schedule an event at T2,
        //   reschedule it after T and verify that reschedule succeeds.
        //   Verify that the callback executes at the new rescheduled
        //   time.  Now verify that rescheduling the same event fails.
        //
        // Testing:
        //   int rescheduleEvent(handle, newTime);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'rescheduleEvent'" << endl
                          << "=========================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);

        using namespace EVENTSCHEDULER_TEST_CASE_12;
        {
          // Create and start a scheduler object, schedule an event at T2,
          // reschedule it at T and verify that reschedule succeeds.  Verify
          // that the callback executes at the new rescheduled time.  Now
          // verify that rescheduling the same event fails.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T = 1 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T4(4 * DECI_SEC);
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T8(8 * DECI_SEC);

          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h;
          x.scheduleEvent(
                      &h,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T, 0);
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T2) {
              nExec = testObj.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT( 0 == x.rescheduleEvent(h, now + T4) );
              microSleep(T6, 0);
              makeSureTestObjectIsExecuted(testObj, mT, 100);
              nExec = testObj.numExecuted();
              LOOP_ASSERT(nExec, 1 == nExec);
              ASSERT( 0 != x.rescheduleEvent(h, now + T8) );
          }
          x.stop();
        }

        {
          // Repeat above

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T4(4 * DECI_SEC);
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T8(8 * DECI_SEC);

          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h;
          x.scheduleEvent(
                      &h,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T, 0);
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T2) {
              nExec = testObj.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT( 0 == x.rescheduleEventAndWait(h, now + T4) );
              microSleep(T6, 0);
              makeSureTestObjectIsExecuted(testObj, mT, 100);
              nExec = testObj.numExecuted();
              LOOP_ASSERT(nExec, 1 == nExec);
              ASSERT( 0 != x.rescheduleEventAndWait(h, now + T8) );
          }
          x.stop();
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL:
        //   Verify concurrent scheduling and cancelling-All.
        //
        // Concerns:
        //   That concurrent scheduling and cancelling-all of clocks and
        //   events are safe.
        //
        // Plan:
        //   Create several threads.  Half of the threads execute 'h =
        //   scheduler.scheduleEvent(..), scheduler.cancelAllEvent()' in a loop
        //   and other half execute 'h = scheduler.scheduleRecurringEvent(..)'
        //   and 'scheduler.cancelAllEvents(h)' in a loop.  Finally join all
        //   the threads and verify that no callback has been invoked and that
        //   the scheduler is still in good state (this is done by scheduling
        //   an event and verifying that it is executed as expected).
        //
        // Testing:
        //   Concurrent scheduling and cancelling-All.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL" << endl
                 << "================================================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_11;
        const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
        ASSERT( 0 == x.start() );

        executeInParallel(NUM_THREADS, workerThread11);
        microSleep(T10, 0);

        if (!testTimingFailure) {
            for (int i = 0; i< NUM_THREADS; ++i) {
                ASSERT( 0 == testObj[i].numExecuted() );
            }
        } else {
            // In the highly unlikely event, that one thread could not invoke
            // 'cancelAllEvents' or 'cancelAllClocks' in time to guarantee that
            // the event it had scheduled earlier was cancelled, the test
            // fails.  This can be prevented by increasing the delay (initially
            // T, currently T4) with which the event is scheduled.  Note that
            // the delay T5 with which this test is executed must be increased
            // correspondingly.

            // The following message will be reported by the nightly build,
            // although it will not trigger a failure.

            cout << "TIMING FAILURE ERROR IN CASE 11 - INCREASE DELAYS\n";
        }

        // test if the scheduler is still in good state
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
        TestClass1 testObj;
        x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
        microSleep(T4, 0);
        ASSERT( 1 == testObj.numExecuted() );
        x.stop();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT SCHEDULING AND CANCELLING:
        //   Verify concurrent scheduling and cancelling.
        //
        // Concerns:
        //   That concurrent scheduling and cancelling of clocks and
        //   events are safe.
        //
        // Plan:
        //   Create several threads.  Half of the threads execute
        //   'scheduler.scheduleEvent(..), scheduler.cancelEvent(h)' in a loop
        //   and other half execute 'scheduler.scheduleRecurringEvent(..);
        //   scheduler.cancelEvent(h)' in a loop.  Finally join all the threads
        //   and verify that no callback has been invoked and that the
        //   scheduler is still in good state (this is done by scheduling an
        //   event and verifying that it is executed as expected).
        //
        // Testing:
        //   Concurrent scheduling and cancelling.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING CONCURRENT SCHEDULING AND CANCELLING" << endl
                 << "============================================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_10;

        const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
        ASSERT( 0 == x.start() );

        executeInParallel(NUM_THREADS, workerThread10);
        microSleep(T10, 0);

        if (!testTimingFailure) {
            for (int i = 0; i < NUM_THREADS; ++i) {
                LOOP2_ASSERT(i, testObj[i].numExecuted(),
                             0 == testObj[i].numExecuted() );
            }
        } else {
            // In the highly unlikely event, that one thread could not invoke
            // 'cancelEvent' or 'cancelClock' in time to guarantee that the
            // event it had scheduled earlier was cancelled, the test fails.
            // This can be prevented by increasing the delay (initially T,
            // currently T4) with which the event is scheduled.  Note that the
            // delay T10 with which this test is executed must be increased
            // correspondingly.

            // The following message will be reported by the nightly build,
            // although it will not trigger a failure.

            cout << "TIMING FAILURE ERROR IN CASE 10 - INCREASE DELAYS\n";
        }

        // test if the scheduler is still in good state
        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
        TestClass1 testObj;
        x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
        microSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );
        x.stop();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'start' and 'stop':
        //   Verifying 'start' and 'stop'.
        //
        // Concerns:
        //
        //   That invoking 'start' multiple times is harmless.
        //
        //   That invoking 'stop' multiple times is harmless.
        //
        //   That it is possible to restart the scheduler after stopping
        //   it.
        //
        //   That invoking 'stop' work correctly even when the dispatcher
        //   is blocked waiting for any callback to be scheduled.
        //
        //   That invoking 'stop' work correctly even when the dispatcher
        //   is blocked waiting for the expiration of any scheduled
        //   callback.
        //
        // Plan:
        //   Invoke 'start' multiple times and verify that the scheduler
        //   is still in good state (this is done by scheduling an event
        //   and verifying that it is executed as expected).
        //
        //   Invoke 'stop' multiple times and verify that the scheduler is
        //   still in good state (this is done by starting the scheduler,
        //   scheduling an event and verifying that it is executed as
        //   expected).
        //
        //   Restart scheduler after stopping and make sure that the
        //   scheduler is still in good state (this is done by starting
        //   the scheduler, scheduling an event and verifying that it is
        //   executed as expected).  Repeat this several time.
        //
        //   Start the scheduler, sleep for a while to ensure that
        //   scheduler gets blocked, invoke 'stop' and verify that scheduler
        //   is actually stopped.
        //
        //   Start the scheduler, schedule an event, sleep for a while to
        //   ensure that scheduler gets blocked, invoke 'stop' before the
        //   event is expired and verify the state.
        //
        // Testing:
        //   int start();
        //   void stop();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'start' and 'stop'" << endl
                          << "==========================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_9;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
          // Invoke 'start' multiple times and verify that the scheduler is
          // still in good state (this is done by scheduling an event and
          // verifying that it is executed as expected).

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          ASSERT( 0 == x.start() );
          ASSERT( 0 == x.start() );

          TestClass1 testObj;
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
          x.stop();
        }

        {
          // Invoke 'stop' multiple times and verify that the scheduler is
          // still in good state (this is done by starting the scheduler,
          // scheduling an event and verifying that it is executed as
          // expected).

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          x.stop();
          x.stop();
          ASSERT( 0 == x.start() );
          x.stop();
          x.stop();
          ASSERT( 0 == x.start() );

          TestClass1 testObj;
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
          x.stop();
        }

        {
          // Restart scheduler after stopping and make sure that the scheduler
          // is still in good state (this is done by starting the scheduler,
          // scheduling an event and verifying that it is executed as
          // expected).  Repeat this several time.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj;

          ASSERT( 0 == x.start() );
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
          x.stop();

          nExec = testObj.numExecuted();
          ASSERT( 0 == x.start() );
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
          ASSERT( 2 == testObj.numExecuted() );
          x.stop();

          nExec = testObj.numExecuted();
          ASSERT( 0 == x.start() );
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
          ASSERT( 3 == testObj.numExecuted() );
          x.stop();
        }

        {
          // Start the scheduler, sleep for a while to ensure that scheduler
          // gets blocked, invoke 'stop' and verify that scheduler is actually
          // stopped.

          const bsls::TimeInterval T(1 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          ASSERT( 0 == x.start() );
          microSleep(T3, 0);
          x.stop();

          // Make sure that scheduler is stopped.
          TestClass1 testObj;
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T3, 0);
          ASSERT( 0 == testObj.numExecuted() );
          x.stop();
        }

        {
          // Start the scheduler, schedule an event, sleep for a while to
          // ensure that scheduler gets blocked, invoke 'stop' before the event
          // is expired and verify the state.

          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T4(4 * DECI_SEC);

          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj;
          ASSERT( 0 == x.start() );
          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleEvent(
                      now + T4,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T2, 0);
          x.stop();

          // Make sure that scheduler is stopped, but allow for the possibility
          // that microSleep overslept and apply defensive techniques.
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T4) {
              microSleep(T3, 0);
              ASSERT( 0 == testObj.numExecuted() );
          }

          // Make *really* sure that scheduler is stopped.
          int numExecutedUnchanged = testObj.numExecuted();
          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T3, 0);
          ASSERT(numExecutedUnchanged == testObj.numExecuted() );
          x.stop();
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'bdlmt::EventScheduler(dispatcher, allocator = 0)':
        //   Verifying 'bdlmt::EventScheduler(dispatcher, allocator = 0)'.
        //
        // Concerns:
        //   To test the scheduler with a user-defined dispatcher.
        //
        // Plan:
        //   Define a dispatcher that simply executes the specified
        //   functor.  Create a scheduler using this dispatcher, schedule
        //   an event and make sure that it is executed by the specified
        //   dispatcher.
        //
        // Testing:
        //   bdlmt::EventScheduler(dispatcher, allocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::EventScheduler"
                          << "(dispatcher, allocator = 0)'" << endl
                          << "============================"
                          << "============================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_8;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::EventScheduler::Dispatcher dispatcher =
          bdlf::BindUtil::bind(&dispatcherFunction, _1);

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(dispatcher, &ta); x.start();

        TestClass1 testObj;

        x.scheduleEvent(
                      bdlt::CurrentTime::now() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        microSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );
        x.stop();

      } break;
      case 7: {
          // placeholder
          if (verbose) {
              cout << "...case 7 just a placeholder..." << endl;
          }
      } break;
      case -7: {
        // --------------------------------------------------------------------
        // TESTING METHODS INVOCATIONS FROM THE DISPATCHER THREAD:
        //   Verify various methods invocations from the dispatcher thread.
        //
        //   This is now undefined behavior in the API...this test is left here
        //   to check the behavior (e.g., to make sure it aborts properly)
        // Concerns:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
          << "TESTING METHODS INVOCATIONS FROM THE DISPATCHER THREAD" << endl
          << "======================================================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_7;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            // Schedule an event at T2 that itself schedules an event at
            // T2+T=T3 and a clock with period T4 (clicking at T6 and T10),
            // and verify that callbacks are executed as expected.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T8 = 8 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T10(10 * DECI_SEC);

          Obj x(&ta); x.start();

          TestClass1 event;
          TestClass1 clock;
          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleEvent(
                now + T2,
                bdlf::BindUtil::bind(&schedulingCallback, &x, &event, &clock));

          microSleep(T8, 0);
          makeSureTestObjectIsExecuted(event, mT, 100);
          LOOP_ASSERT(event.numExecuted(), 1 == event.numExecuted() );
          LOOP_ASSERT(clock.numExecuted(), 1 <= clock.numExecuted() );
        }

        {
            // Schedule an event e1 at time T such that it cancels itself with
            // wait argument (this will fail), and an event e2 that will be
            // pending when e1 will execute (this is done by waiting long
            // enough before starting the scheduler).  Make sure no deadlock
            // results.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj;

          Event* handleToBeCancelled;
          x.scheduleEventRaw(&handleToBeCancelled,
                             bdlt::CurrentTime::now() + T,
                             bdlf::BindUtil::bind(&cancelEventRawCallback,
                                                  &x,
                                                  &handleToBeCancelled,
                                                  1,
                                                  -1));

          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T6, 0);  // let testObj's callback be pending

          x.start();

          microSleep(T3, 0); // give enough time to complete testObj's callback
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
          x.cancelAllEventsAndWait();
          x.releaseEventRaw(handleToBeCancelled);
        }

        {
            // Schedule an event e1 at time T such that it invokes
            // 'cancelAllEvents' with wait argument, and another event e2 at
            // time T2 that will be pending when e1 will execute (this is done
            // by waiting long enough before starting the scheduler).  Make
            // sure no deadlock results of e1 waiting for e2 to complete (both
            // are running in the dispatcher thread).

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj1;
          TestClass1 testObj2;

          x.scheduleEvent(
                        bdlt::CurrentTime::now() + T,
                        bdlf::BindUtil::bind(&cancelAllEventsCallback, &x, 1));

          x.scheduleEvent(
                     bdlt::CurrentTime::now() + T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

          microSleep(T6, 0); // let testObj1's callback be pending

          x.start();

          microSleep(T3, 0); // give enough time to complete testObj's callback
          makeSureTestObjectIsExecuted(testObj1, mT, 100);
          // TBD ASSERT( 1 == testObj1.numExecuted() );
          ASSERT( 0 == testObj1.numExecuted() );
        }

        {
            // Schedule a clock c1 such that it cancels itself with wait
            // argument (this will fail), and an event e2 that will be pending
            // when c1 will execute (this is done by waiting long enough before
            // starting the scheduler).  Make sure no deadlock results.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3  = 3 * DECI_SEC_IN_MICRO_SEC;
          const int T6  = 6 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj;

          RecurringEventHandle handleToBeCancelled;
          x.scheduleRecurringEvent(
                            &handleToBeCancelled,
                            T,
                            bdlf::BindUtil::bind(&cancelRecurringEventCallback,
                                                 &x,
                                                 &handleToBeCancelled, 1, 0));

          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T6, 0);   // let testObj1's callback be pending

          x.start();

          microSleep(T3, 0); // give enough time to complete testObj's callback
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
          ASSERT( 0 == x.numRecurringEvents() );
        }

        {
            // Schedule a clock c1 such that it invokes 'cancelAllClocks' with
            // wait argument, and an event e2 that will be pending when c1 will
            // execute (this is done by waiting long enough before starting the
            // scheduler).  Make sure no deadlock results.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta);

          TestClass1 testObj;

          x.scheduleRecurringEvent(
                        T,
                        bdlf::BindUtil::bind(&cancelAllEventsCallback, &x, 1));

          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T6, 0); // let testObj1's callback be pending

          x.start();

          microSleep(T3, 0); // give enough time to complete testObj's callback
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
        }

        {
            // Cancel from dispatcher thread and verify that the state is still
            // valid.  For DRQS 7272737, it takes two events to reproduce the
            // problem.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

          Obj x(&ta); x.start();

          TestClass1 testObj;

          EventHandle h1, h2;
          {
              bsl::shared_ptr<int> ptr1; ptr1.createInplace(&ta, 1);
              bsl::shared_ptr<int> ptr2; ptr2.createInplace(&ta, 2);

              x.scheduleEvent(&h1,
                              bdlt::CurrentTime::now() + T,
                              bdlf::BindUtil::bind(
                                                 &cancelEventCallbackWithState,
                                                 &x,
                                                 &h1,
                                                 ptr1));
              x.scheduleEvent(&h2,
                              bdlt::CurrentTime::now() + T,
                              bdlf::BindUtil::bind(
                                                 &cancelEventCallbackWithState,
                                                 &x,
                                                 &h2,
                                                 ptr2));
          }

          x.scheduleEvent(
                      bdlt::CurrentTime::now() + T3,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          microSleep(T10, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          ASSERT( 1 == testObj.numExecuted() );
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'cancelAllClocks':
        //   Verifying 'cancelAllClocks'.
        //  NOTE: below, "clocks" are really "recurring events", mut. mut.
        //
        // Concerns:
        //   That if no clock has not yet been put onto the pending list
        //   then invoking 'cancelAllClocks' should successfully cancel
        //   all clocks.
        //
        //   That once a clock callback has started its execution, invoking
        //   'cancelAllClocks' without wait argument should not block for
        //   that execution to complete and should cancel its further
        //   executions.  It should also cancel other pending clocks.
        //
        //   That once a clock callback has started its execution, invoking
        //   'cancelAllClocks' with wait argument should block for that
        //   execution to complete and should cancel its further
        //   executions.  It should also cancel other pending clocks.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule clocks starting at T2 and T3, invoke
        //   'cancelAllClocks' at time T and make sure that both are
        //   cancelled successfully.
        //
        //   Schedule clocks c1 at T(which executes for T10 time), c2 at
        //   T2 and c3 at T3.  Let all clocks be simultaneously put onto
        //   the pending list (this is done by sleeping enough time before
        //   starting the scheduler).  Let c1's first execution be started
        //   (by sleeping enough time), invoke 'cancelAllClocks' without
        //   wait argument, verify that c1's first execution has not yet
        //   completed and verify that c2 and c3 are cancelled without any
        //   executions.
        //
        //   Schedule clocks c1 at T(which executes for T10 time), c2 at
        //   T2 and c3 at T3.  Let all clocks be simultaneously put onto
        //   the pending list (this is done by sleeping enough time before
        //   starting the scheduler).  Let c1's first execution be started
        //   (by sleeping enough time), invoke 'cancelAllClocks' with wait
        //   argument, verify that c1's first execution has completed and
        //   verify that c2 and c3 are cancelled without any executions.
        //
        // Testing:
        //   void cancelAllClocks(bool wait=false);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelAllEvents' (2)" << endl
                          << "=============================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_6;
        bslma::TestAllocator ta(veryVeryVerbose);

        pta = &ta;

        bslmt::ThreadUtil::Handle handles[3];
        int sts;
        sts = bslmt::ThreadUtil::create(&handles[0], Test6_0());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[1], Test6_1());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[2], Test6_2());
        ASSERT(0 == sts);

        for (int i = 0; i < 3; ++i) {
            sts = bslmt::ThreadUtil::join(handles[i]);
            ASSERT(0 == sts);
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'cancelClock':
        //   Verifying 'cancelClock'.
        //
        // Concerns:
        //   That if a clock has not yet been put onto the pending list
        //   then it can successfully be cancelled.
        //
        //   That if a clock callback has been put onto the pending list
        //   but not yet executed then it can still be cancelled.
        //
        //   That once a clock callback has started its execution,
        //   cancelling it without wait should not block for that
        //   execution to complete and should cancel its further
        //   executions.
        //
        //   That once a clock callback has started its execution,
        //   cancelling it with wait should block for that execution to
        //   complete and should cancel its further executions.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule a clock starting at T2, cancel it at time T and
        //   verify that it has been successfully cancelled.
        //
        //   Schedule two clocks c1 and c2 starting at T and T2.  Let both
        //   be simultaneously put onto the pending list (this is done by
        //   sleeping enough time before starting the scheduler), cancel
        //   c2 before it's callback is dispatched and verify the result.
        //
        //   Schedule a clock (whose callback executes for T5 time)
        //   starting at T.  Let its first execution be started (by
        //   sleeping for T2 time), cancel it without wait argument,
        //   verify that its execution has not yet completed, make sure
        //   that it is cancelled after this execution.
        //
        //   Schedule a clock (whose callback executes for T5 time)
        //   starting at T.  Let its first execution be started (by
        //   sleeping for T2 time), cancel it with wait argument, verify
        //   that its execution has completed, make sure that it is
        //   cancelled after this execution.
        //
        // Testing:
        //   int cancelClock(Handle handle, bool wait=false);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelClock'" << endl
                          << "=====================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        using namespace EVENTSCHEDULER_TEST_CASE_5;

        pta = &ta;

        bslmt::ThreadUtil::Handle handles[4];
        int sts;
        sts = bslmt::ThreadUtil::create(&handles[0], Test5_0());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[1], Test5_1());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[2], Test5_2());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[3], Test5_3());
        ASSERT(0 == sts);

        for (int i = 0; i < 4; ++i) {
            sts = bslmt::ThreadUtil::join(handles[i]);
            ASSERT(0 == sts);
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'cancelAllEvents':
        //   Verifying 'cancelAllEvents'.
        //
        // Concerns:
        //   That if no event has yet been put onto the pending list, then
        //   invoking 'cancelAllEvents' should be able to cancel all
        //   events.
        //
        //   That 'cancelAllEvents' without wait argument should
        //   immediately cancel the events not yet put onto the pending
        //   list and should not wait for events on the pending list to be
        //   executed.
        //
        //   That 'cancelAllEvents' with wait argument should cancel the
        //   events not yet put onto the pending list and should wait for
        //   events on the pending list to be executed.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule events at T, T2 and T3, invoke 'cancelAllEvents' and
        //   verify the result.
        //
        //   Schedule events e1, e2 and e3 at T, T2 and T8 respectively.
        //   Let both e1 and e2 be simultaneously put onto the pending
        //   list (this is done by sleeping enough time before starting
        //   the scheduler).  Invoke 'cancelAllEvents' and ensure that it
        //   cancels e3 and does not wait for e1 and e2 to complete their
        //   execution.
        //
        //   Schedule events e1, e2 and e3 at T, T2 and T8 respectively.
        //   Let both e1 and e2 be simultaneously put onto the pending
        //   list (this is done by sleeping enough time before starting
        //   the scheduler).  Invoke 'cancelAllEvents' with wait argument
        //   and ensure that it cancels e3 and wait for e1 and e2 to
        //   complete their execution.
        //
        // Testing:
        //   void cancelAllEvents(bool wait=false);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelAllEvents' (1)" << endl
                          << "=============================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_4;
        bslma::TestAllocator ta(veryVeryVerbose);
        pta = &ta;

        bslmt::ThreadUtil::Handle handles[3];
        int sts;
        sts = bslmt::ThreadUtil::create(&handles[0], Test4_0());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[1], Test4_1());
        ASSERT(0 == sts);
        sts = bslmt::ThreadUtil::create(&handles[2], Test4_2());
        ASSERT(0 == sts);

        for (int i = 0; i < 3; ++i) {
            sts = bslmt::ThreadUtil::join(handles[i]);
            ASSERT(0 == sts);
        }

        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'cancelEvent':
        //   Verifying 'cancelEvent'.
        //
        // Concerns:
        //   That if an event has not yet been put onto the pending list,
        //   then it can successfully be cancelled.
        //
        //   That once an event has been put onto the pending list, it can
        //   not be cancelled from a non-dispatcher thread.
        //
        //   That once an event has been put onto the pending list, it can
        //   not be cancelled from a non-dispatcher thread.
        //
        //   That if the dispatcher thread cancels an event that has not
        //   yet been put onto the pending list then it should succeed.
        //
        //   That if the dispatcher thread cancels an event that has
        //   already been executed then it should fail.
        //
        //   That an event can not cancel itself.
        //
        //   That if the dispatcher thread cancels an event that has been
        //   put onto the pending list but not yet executed, then the
        //   cancellation should succeed.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule an event at T2, cancel it at time T and verify the
        //   result.
        //
        //   Schedule 2 events at T and T2.  Let both be simultaneously put
        //   onto the pending list (this is done by sleeping enough time
        //   before starting the scheduler), invoke 'cancelEvent(handle)'
        //   on the second event while it is still on pending list and
        //   verify that cancel fails.
        //
        //   Schedule 2 events at T and T2.  Let both be simultaneously put
        //   onto the pending list (this is done by sleeping enough time
        //   before starting the scheduler), invoke 'cancelEvent(handle,
        //   wait)' on the second event while it is still on pending list
        //   and verify that cancel fails.
        //
        //   Schedule events e1 and e2 at T and T2 respectively, cancel
        //   e2 from e1 and verify that cancellation succeed.
        //
        //   Schedule events e1 and e2 at T and T2 respectively, cancel
        //   e1 from e2 and verify that cancellation fails.
        //
        //   Schedule an event that invokes cancel on itself.  Verify that
        //   cancellation fails.
        //
        //   Schedule e1 and e2 at T and T2 respectively.  Let both be
        //   simultaneously put onto the pending list (this is done by
        //   sleeping enough time before starting the scheduler), cancel
        //   e2 from e1 and verify that it succeeds.
        //
        // Note:
        //   Defensive programming: microSleep can (and does sometimes,
        //   especially when load is high during nightly builds) oversleep, so
        //   all assumptions have to be checked against the actual elapsed
        //   time.
        //
        // Testing:
        //   int cancelEvent(Handle handle, bool wait=false);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelEvent'" << endl
                          << "=====================" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_3;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            Obj x(&ta); x.start();
            if (verbose)
                ET("\tSchedule event and cancel before execution.");
            {
                // Schedule an event at T2, cancel it at time T and verify the
                // result.

                const int T = 1 * DECI_SEC_IN_MICRO_SEC;
                const bsls::TimeInterval T2(2 * DECI_SEC);
                const bsls::TimeInterval T5(5 * DECI_SEC);
                const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

                TestClass1 testObj;

                bsls::TimeInterval now = bdlt::CurrentTime::now();
                EventHandle h;
                x.scheduleEvent(
                      &h,
                      now + T5,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
                bslmt::ThreadUtil::microSleep(T, 0);
                bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                if (elapsed < T2) {
                    ASSERT( 0 == x.cancelEvent(h) );
                    microSleep(T6, 0);
                    ASSERT( 0 == testObj.numExecuted() );
                }
            }

            if (verbose)
                ET("\tCancel pending event (should succeed).");
            {
            // Schedule 2 events at T and T2.  Let both be simultaneously put
            // onto the pending list (this is done by sleeping enough time
            // before starting the scheduler), invoke 'cancelEvent(handle)' on
            // the second event while it is still on pending list and verify
            // that cancel fails.

                x.stop();

                bsls::TimeInterval       T (1 * DECI_SEC);
                const bsls::TimeInterval T2(2 * DECI_SEC);
                const int TI = DECI_SEC;
                const int T3  =  3 * DECI_SEC_IN_MICRO_SEC;
                const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
                const int T13 = 13 * DECI_SEC_IN_MICRO_SEC;

                TestClass1 testObj1(T10);
                TestClass1 testObj2, testObj3;

                bsls::TimeInterval now = bdlt::CurrentTime::now();
                EventHandle h1, h2;
                x.scheduleEvent(
                     &h1,
                     now + T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
                h1.release();
                x.scheduleEvent(
                     &h2,
                     now + T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
                microSleep(T3, 0);
                x.start();
                microSleep(TI, 0);

                ASSERT( 0 == x.cancelEvent(h2) );

                bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                if (elapsed < T13) {
                    ASSERT( 0 == testObj2.numExecuted() );
                }

                now = bdlt::CurrentTime::now();
                x.scheduleEvent(
                     &h2,
                     now + T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));
                ASSERT(0 == x.cancelEvent(h2));
                if (elapsed < T13) {
                    ASSERT( 0 == testObj3.numExecuted() );
                }
                x.cancelAllEventsAndWait();
            }

            if (verbose) ET("\tCancel event handle.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T1(1 * DECI_SEC);
                const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                EventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }

                ASSERT(0 == (const Event*) handleToBeCancelled);
                int rc = x.cancelEvent(&handleToBeCancelled);
                ASSERT((bdlcc::SkipList<int,int>::e_INVALID == rc));

                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T1,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                EventHandle handle2(handleToBeCancelled);
                ASSERT(0 != (const Event *) handle2);
                ASSERT((const Event *) handle2 ==
                                          (const Event *) handleToBeCancelled);

                rc = x.cancelEvent(&handleToBeCancelled);
                ASSERT(0 == rc);
                ASSERT(0 == (const Event*) handleToBeCancelled);

                ASSERT(0 != (const Event *) handle2);
                rc = x.cancelEvent(&handle2);
                ASSERT((bdlcc::SkipList<int,int>::e_NOT_FOUND == rc));

                microSleep(T2, 0);

                LOOP2_ASSERT(&testObj,
                             testObj.numExecuted(),
                             0 == testObj.numExecuted() );
                ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
            }

            if (verbose) ET("\tCancel event and wait handle.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T1(1 * DECI_SEC);
                const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                EventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }

                ASSERT(0 == (const Event*) handleToBeCancelled);
                int rc = x.cancelEventAndWait(&handleToBeCancelled);
                ASSERT((bdlcc::SkipList<int,int>::e_INVALID == rc));

                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T1,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                EventHandle handle2(handleToBeCancelled);
                ASSERT(0 != (const Event *) handle2);
                ASSERT((const Event *) handle2 ==
                                          (const Event *) handleToBeCancelled);

                rc = x.cancelEventAndWait(&handleToBeCancelled);
                ASSERT(0 == rc);
                ASSERT(0 == (const Event*) handleToBeCancelled);

                ASSERT(0 != (const Event *) handle2);
                rc = x.cancelEventAndWait(&handle2);
                ASSERT((bdlcc::SkipList<int,int>::e_NOT_FOUND == rc));

                microSleep(T2, 0);

                LOOP2_ASSERT(&testObj,
                             testObj.numExecuted(),
                             0 == testObj.numExecuted() );
                ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
            }

            if (verbose) ET("\tCancel recurring event handle.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T1(1 * DECI_SEC);
                const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                RecurringEventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }

                ASSERT(0 == (const RecurringEvent*) handleToBeCancelled);
                int rc = x.cancelEvent(&handleToBeCancelled);
                ASSERT((bdlcc::SkipList<int,int>::e_INVALID == rc));

                x.scheduleRecurringEvent(
                      &handleToBeCancelled,
                      T1,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                RecurringEventHandle handle2(handleToBeCancelled);
                ASSERT(0 != (const RecurringEvent *) handle2);
                ASSERT((const RecurringEvent *) handle2 ==
                                 (const RecurringEvent *) handleToBeCancelled);

                rc = x.cancelEvent(&handleToBeCancelled);
                ASSERT(0 == rc);
                ASSERT(0 == (const RecurringEvent*) handleToBeCancelled);

                ASSERT(0 != (const RecurringEvent *) handle2);
                rc = x.cancelEvent(&handle2);
                ASSERT((bdlcc::SkipList<int,int>::e_NOT_FOUND == rc));

                microSleep(T2, 0);

                LOOP2_ASSERT(&testObj,
                             testObj.numExecuted(),
                             0 == testObj.numExecuted() );
                ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
            }

            if (verbose) ET("\tCancel and wait recurring event handle.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T1(1 * DECI_SEC);
                const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                RecurringEventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }

                ASSERT(0 == (const RecurringEvent*) handleToBeCancelled);
                int rc = x.cancelEvent(&handleToBeCancelled);
                ASSERT((bdlcc::SkipList<int,int>::e_INVALID == rc));

                x.scheduleRecurringEvent(
                      &handleToBeCancelled,
                      T1,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                RecurringEventHandle handle2(handleToBeCancelled);
                ASSERT(0 != (const RecurringEvent *) handle2);
                ASSERT((const RecurringEvent *) handle2 ==
                                 (const RecurringEvent *) handleToBeCancelled);

                rc = x.cancelEventAndWait(&handleToBeCancelled);
                ASSERT(0 == rc);
                ASSERT(0 == (const RecurringEvent*) handleToBeCancelled);

                ASSERT(0 != (const RecurringEvent *) handle2);
                rc = x.cancelEvent(&handle2);
                ASSERT((bdlcc::SkipList<int,int>::e_NOT_FOUND == rc));

                microSleep(T2, 0);

                LOOP2_ASSERT(&testObj,
                             testObj.numExecuted(),
                             0 == testObj.numExecuted() );
                ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
            }

            if (verbose) ET("\tCancel event from another event prior.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T(1 * DECI_SEC);
                const bsls::TimeInterval T2(5 * DECI_SEC);
                const int T10 = 20 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                EventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }
                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                // It could possibly happen that testObj has already been
                // scheduled for execution, and thus the following cancelEvent
                // will fail.  Make sure that does not happen.
                bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                if (elapsed < T) {
                    x.scheduleEvent(now + T,
                                    bdlf::BindUtil::bind(&cancelEventCallback,
                                                         &x,
                                                         handleToBeCancelled,
                                                         0,
                                                         0,
                                                         L_));
                    microSleep(T10, 0);
                    LOOP2_ASSERT( &testObj,
                                  testObj.numExecuted(),
                                  0 == testObj.numExecuted() );
                    ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
                }
                // Else should we complain that too much time has elapsed?  In
                // any case, this is not a failure, do not stop.
            }

            if (verbose) ET("\tCancel event handle from another event prior.");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e2 from e1 and verify that cancellation succeed.

                const bsls::TimeInterval T(1 * DECI_SEC);
                const bsls::TimeInterval T2(5 * DECI_SEC);
                const int T10 = 20 * DECI_SEC_IN_MICRO_SEC;

                x.start();
                TestClass1 testObj;
                EventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                if (verbose) {
                    ET_("main thread:"); PT(now);
                }
                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                // It could possibly happen that testObj has already been
                // scheduled for execution, and thus the following cancelEvent
                // will fail.  Make sure that does not happen.
                bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
                if (elapsed < T) {
                    x.scheduleEvent(
                               now + T,
                               bdlf::BindUtil::bind(&cancelEventHandleCallback,
                                                    &x,
                                                    &handleToBeCancelled,
                                                    0,
                                                    0,
                                                    L_));
                    microSleep(T10, 0);
                    LOOP2_ASSERT( &testObj,
                                  testObj.numExecuted(),
                                  0 == testObj.numExecuted() );
                    ASSERT( 0 == x.numEvents() + x.numRecurringEvents());
                }
                // Else should we complain that too much time has elapsed?  In
                // any case, this is not a failure, do not stop.
            }

            if (verbose)
                ET("\tCancel event from subsequent event (should fail).");
            {
                // Schedule events e1 and e2 at T and T2 respectively, cancel
                // e1 from e2 and verify that cancellation fails.

                const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
                const bsls::TimeInterval T(1 * DECI_SEC);
                const bsls::TimeInterval T2(2 * DECI_SEC);
                const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

                TestClass1 testObj;
                EventHandle handleToBeCancelled;
                bsls::TimeInterval now = bdlt::CurrentTime::now();
                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                x.scheduleEvent(now + T2,
                                bdlf::BindUtil::bind(&cancelEventCallback,
                                                     &x,
                                                     handleToBeCancelled,
                                                     0,
                                                     1,
                                                     L_));

                microSleep(T3, 0);
                makeSureTestObjectIsExecuted(testObj, mT, 100);
                ASSERT( 1 == testObj.numExecuted() );
            }

            if (verbose)
                ET("\tCancel event from itself (should fail).");
            {
                // Schedule an event that invokes cancel on itself.  Verify
                // cancellation fails.

                const bsls::TimeInterval T(1 * DECI_SEC);
                const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

                bsls::TimeInterval now = bdlt::CurrentTime::now();
                Event* handleToBeCancelled;
                x.scheduleEventRaw(
                                  &handleToBeCancelled,
                                  now + T,
                                  bdlf::BindUtil::bind(&cancelEventRawCallback,
                                                       &x,
                                                       &handleToBeCancelled,
                                                       0,
                                                       1));

                microSleep(T3, 0);
                // The assert is performed by 'cancelEventCallback'.
                x.releaseEventRaw(handleToBeCancelled);
                handleToBeCancelled = 0;
            }

            if (verbose)
                ET("\tCancel pending event from pending event prior.");
            {
                // Schedule e1 and e2 at T and T2 respectively.  Let both be
                // simultaneously put onto the pending list (this is done by
                // sleeping enough time before starting the scheduler), cancel
                // e2 from e1 and verify that it succeeds.

                bsls::TimeInterval  T(1 * DECI_SEC);
                const bsls::TimeInterval T2(2 * DECI_SEC);
                const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

                x.stop();

                TestClass1 testObj;

                bsls::TimeInterval now = bdlt::CurrentTime::now();
                EventHandle handleToBeCancelled;
                x.scheduleEvent(
                      &handleToBeCancelled,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

                x.scheduleEvent(now + T,
                                bdlf::BindUtil::bind(&cancelEventCallback,
                                                     &x,
                                                     handleToBeCancelled,
                                                     0,
                                                     0,
                                                     L_));

                microSleep(T3, 0);
                x.start();

                microSleep(T3, 0);
                ASSERT( 0 == testObj.numExecuted() );
            }
            x.stop();
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'scheduleEvent' and 'scheduleRecurringEvent':
        //   Verifying 'scheduleEvent' and 'scheduleRecurringEvent'.
        //
        // Concerns:
        //   That multiple clocks and multiple events can be scheduled.
        //
        //   That when a scheduler runs a *very long* running callback
        //   then callbacks scheduled after that callback are shifted as
        //   expected.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule multiple clocks and multiple events and verify that
        //   their callback executes at appropriate times.
        //
        //   Schedule a long running callback and several other callbacks.
        //   Verify that other callbacks are shifted as expected.
        //
        // Note:
        //   Test sometimes fail due to too much thread contention in parallel
        //   testing with high load.  This is not necessarily a test failure,
        //   but reflects stressed conditions.  We try to be as defensive as we
        //   can, and start long after the scheduling to allow settling down of
        //   threads.  In addition, we run the test several times or until it
        //   succeeds a minimum number of times (threshold).  We consider the
        //   first outcome a failure (and assert, in order to still catch
        //   systematic failures should the code regress), but we let the
        //   second outcome be a success although some runs may have failed.
        //   In that case, we report the results for nightly build diagnostics,
        //   but do not generate a test failure.
        //
        // Testing:
        //   Handle scheduleEvent(time, callback);
        //   Handle scheduleRecurringEvent(interval, callback,
        //                                    startTime=bsls::TimeInterval(0));
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'scheduleEvent' and 'scheduleRecurringEvent'"
                 << "\n===================================================="
                 << endl;

        const int ATTEMPTS = 9;
        const int SUCCESS_THRESHOLD = 1;
        const int ALLOWED_DEVIATIONS = 1;

        using namespace EVENTSCHEDULER_TEST_CASE_2;
        {
            // schedule multiple clocks and multiple events and verify that
            // their callbacks execute at appropriate times.

            static const TestCase2Data DATA[] = {
                //line no.  start  isClock  interval executionTime  delayed
                //-------   -----  -------  -------- -------------  -------
                { L_,          1,    true,       10,            0,   false },
                { L_,          2,    true,       10,            0,   false },
                { L_,          3,    true,       10,            0,   false },
                { L_,          4,    true,       10,            0,   false },
                { L_,          5,    true,       10,            0,   false },

                { L_,          6,   false,        0,            0,   false },
                { L_,          7,   false,        0,            0,   false },
                { L_,          8,   false,        0,            0,   false },
                { L_,          9,   false,        0,            0,   false },

                { L_,         16,   false,        0,            0,   false },
                { L_,         17,   false,        0,            0,   false },
                { L_,         18,   false,        0,            0,   false },
                { L_,         19,   false,        0,            0,   false },

                { L_,         26,   false,        0,            0,   false },
                { L_,         27,   false,        0,            0,   false },
                { L_,         28,   false,        0,            0,   false },
                { L_,         29,   false,        0,            0,   false },

                { L_,         36,   false,        0,            0,   false },
                { L_,         37,   false,        0,            0,   false },
                { L_,         38,   false,        0,            0,   false },
                { L_,         39,   false,        0,            0,   false },

                { L_,         46,   false,        0,            0,   false },
                { L_,         47,   false,        0,            0,   false },
                { L_,         48,   false,        0,            0,   false },
                { L_,         49,   false,        0,            0,   false }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const float TOTAL_TIME =  29.;
            TestClass *testObjects[NUM_DATA];

            int passed = 0, failed = 0, deviations = 0;
            for (int i = 0; passed < SUCCESS_THRESHOLD && i < ATTEMPTS; ++i)
            {
                deviations = 0;
                if (testCallbacks(&deviations,
                                  TOTAL_TIME,
                                  DATA,
                                  NUM_DATA,
                                  testObjects) ||
                    deviations <= ALLOWED_DEVIATIONS) {
                    ++passed;
                    if (verbose)
                        cout << "  Test case 2: PASSED\n";
                } else {
                    ++failed;
                    if (verbose)
                        cout << "  Test case 2: FAILED (deviation "
                             << deviations << ")\n";
                }
            }
            if (passed < SUCCESS_THRESHOLD) {
                // Run one more time and let LOOP_ASSERT report the failures.
                // If this should succeed, then count it towards the successes
                // and run again, until either failure or enough successes.

                while (testCallbacks(0,
                                     TOTAL_TIME,
                                     DATA,
                                     NUM_DATA,
                                     testObjects)) {
                    ++passed;
                    if (verbose)
                        cout << "  Test case 2: PASSED\n";
                    if (passed == SUCCESS_THRESHOLD) {
                        break; // while loop
                    }
                }
            }
            if (passed < SUCCESS_THRESHOLD) {
                ++failed; // for the additional failure in the while loop above
                cout << "Failed test case 2 " << failed
                    << " out of " << passed+failed << " times.\n";
                ASSERT( 0 );
            }
        }

        {
            // schedule a long running callback and several other callbacks.
            // Verify that other callbacks are shifted as expected.

            static const TestCase2Data DATA[] = {
                //line no. start  isClock  interval executionTime  delayed
                //-------  -----  -------  -------- ------------- --------

                // a long running callback
                { L_,         1,   false,        0,           10,  false },

                { L_,         2,   false,        0,            0,   true },
                { L_,         4,   false,        0,            0,   true },
                { L_,         6,   false,        0,            0,   true },
                { L_,         8,   false,        0,            0,   true },

                { L_,         3,    true,       10,            0,   true },
                { L_,         5,    true,       10,            0,   true },
                { L_,         7,    true,       10,            0,   true }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const float TOTAL_TIME =  12.;
            TestClass *testObjects[NUM_DATA];

            int passed = 0, failed = 0, deviations = 0;
            for (int i = 0; passed < SUCCESS_THRESHOLD && i < ATTEMPTS; ++i) {
                deviations = 0;
                if (testCallbacks(&deviations,
                                  TOTAL_TIME,
                                  DATA,
                                  NUM_DATA,
                                  testObjects) ||
                    deviations <= ALLOWED_DEVIATIONS)
                {
                    ++passed;
                    if (verbose)
                        cout << "  Test case 2: PASSED\n";
                } else {
                    ++failed;
                    if (verbose)
                        cout << "  Test case 2: FAILED (deviation "
                             << deviations << ")\n";
                }
            }
            if (passed < SUCCESS_THRESHOLD) {
                // Run one more time and let LOOP_ASSERT report the failures.
                // If this should succeed, then count it towards the successes
                // and run again, until either failure or enough successes.
                while (testCallbacks(0,
                                     TOTAL_TIME,
                                     DATA,
                                     NUM_DATA,
                                     testObjects)) {
                    ++passed;
                    if (verbose)
                        cout << "  Test case 2: PASSED\n";
                    if (passed == SUCCESS_THRESHOLD) {
                        break; // while loop
                    }
                }
            }
            if (passed < SUCCESS_THRESHOLD) {
                ++failed; // for the additional failure in the while loop above
                cout << "Failed test case 2 " << failed
                    << " out of " << passed+failed << " times.\n";
                ASSERT( 0 );
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Create and start a scheduler object, schedule an event at T2,
        //   verify that it is not executed at T but is executed at T3.
        //
        //   Create and start a scheduler object, schedule 2 events at
        //   different times, verify that they execute at expected time.
        //
        //   Create and start a scheduler object, schedule a clock of T3
        //   interval, verify that clock callback gets invoked at expected
        //   times.
        //
        //   Create and start a scheduler object, schedule a clock of T3
        //   interval, schedule an event of at T3.  Verify that event and
        //   clock callbacks are being invoked at expected times.
        //
        //   Create and start a scheduler object, schedule an event at
        //   T2, cancel it at T and verify that it is cancelled.  Verify
        //   that cancelling it again results in failure.
        //
        //   Create and start a scheduler object, schedule an event at
        //   T2, cancelling it at T3 should result in failure.
        //
        //   Create and start a scheduler object, schedule 3 events at T,
        //   T2, T3.  Invoke 'cancelAllEvents' and verify it.
        //
        //   Create and start a scheduler object, schedule a clock of T3,
        //   let it execute once and then cancel it and then verify the
        //   expected result.
        //
        //   Create and start a scheduler object, schedule two clocks of
        //   T3 interval, invoke 'cancelAllEvents' and verify the result.
        //
        //   Create and start a scheduler object, schedule a clock of T3
        //   interval, schedule an event at T6, invoke 'stop' at T4 and
        //   then verify the state.  Invoke 'start' and then verify the
        //   state.
        //
        // Note:
        //   Defensive programming: microSleep can (and does sometimes,
        //   especially when load is high during nightly builds) oversleep, so
        //   all assumptions have to be checked against the actual elapsed
        //   time.  The function 'makeSureTestObjectIsExecuted' also helps in
        //   making sure that the dispatcher thread has a good chance to
        //   process timers and clocks.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace EVENTSCHEDULER_TEST_CASE_1;
        {
          // Create and start a scheduler object, schedule an event at T2,
          // verify that it is not executed at T but is executed at T3.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T = 1 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);

          ASSERT(0 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());

          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleEvent(
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T2) {
              ASSERT(1 == x.numEvents());
          }
          ASSERT(0 == x.numRecurringEvents());

          bslmt::ThreadUtil::microSleep(T, 0);
          nExec = testObj.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          // microSleep could have overslept, especially if load is high
          if (elapsed < T2) {
              LOOP_ASSERT(nExec, 0 == nExec);
          }

          bslmt::ThreadUtil::microSleep(T3, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          ASSERT(0 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule an event at T2.
          // Then try to cancel and verify that it succeeds.

          const int T = 1 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h;
          x.scheduleEvent(
                      &h,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          ASSERT(1 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());

          ASSERT( 0 == x.cancelEvent(h) );   // this could fail due to long
               // delays and unfairness of thread allocation, but very unlikely
          h.release();

          ASSERT(0 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());

          microSleep(T, 0);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 0 == nExec); // ok, even if overslept
          microSleep(T3, 0);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 0 == nExec); // ok, even if overslept
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule 2 events at
          // different times, verify that they execute at expected time.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const bsls::TimeInterval T4(4 * DECI_SEC);
          const bsls::TimeInterval T5(5 * DECI_SEC);
          const bsls::TimeInterval T6(6 * DECI_SEC);

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          ASSERT(0 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());

          TestClass1 testObj1;
          TestClass1 testObj2;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleEvent(
                     now + T4,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
          x.scheduleEvent(
                     now + T6,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              ASSERT(2 == x.numEvents());
          }
          ASSERT(0 == x.numRecurringEvents());

          bslmt::ThreadUtil::microSleep(T2, 0);
          nExec = testObj1.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT(2 == x.numEvents());
              ASSERT(0 == x.numRecurringEvents());
          }
          nExec = testObj2.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T5) {
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT(2 == x.numEvents());
              ASSERT(0 == x.numRecurringEvents());
          }

          bslmt::ThreadUtil::microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj1, mT, 100);
          nExec = testObj1.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          nExec = testObj2.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T5) {
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT(1 == x.numEvents());
              ASSERT(0 == x.numRecurringEvents());
          }

          microSleep(T2, 0);
          microSleep(T, 0);
          nExec = testObj1.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          makeSureTestObjectIsExecuted(testObj2, mT, 100);
          nExec = testObj2.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          ASSERT(0 == x.numEvents());
          ASSERT(0 == x.numRecurringEvents());
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule a clock of T3
          // interval, verify that clock callback gets invoked at expected
          // times.  The reason we cannot test that 1 == X.numRecurringEvents()
          // is that the clocks may be in the middle of execution (popped and
          // not yet rescheduled, in which case 0 == X.numRecurringEvents()).

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T6(6 * DECI_SEC);

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleRecurringEvent(
                      T3,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              ASSERT(0 == x.numEvents());
              ASSERT(1 == x.numRecurringEvents());
          }

          bslmt::ThreadUtil::microSleep(T2, 0);
          nExec = testObj.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              LOOP_ASSERT(nExec, 0 == nExec);
          }
          ASSERT(0 == x.numEvents());
          // ASSERT(1 == x.numRecurringEvents());

          bslmt::ThreadUtil::microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
          nExec = testObj.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T6) {
              LOOP_ASSERT(nExec, 1 == nExec);
          }
          else {
              LOOP_ASSERT(nExec, 1 <= nExec);
          }

          microSleep(T4, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 2 <= nExec);
          ASSERT(0 == x.numEvents());
          // ASSERT(1 == x.numRecurringEvents());
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule a clock of T3
          // interval, schedule an event at T3.  Verify that event and clock
          // callbacks are being invoked at expected times.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 100 microsecs
          const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
          const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const bsls::TimeInterval T5(5 * DECI_SEC);
          const bsls::TimeInterval T6(6 * DECI_SEC);
          const bsls::TimeInterval T8(8 * DECI_SEC);
          const bsls::TimeInterval T9(9 * DECI_SEC);

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj1;
          TestClass1 testObj2;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleRecurringEvent(
                     T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
          x.scheduleEvent(
                     now + T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              ASSERT(1 == x.numEvents());
              ASSERT(1 == x.numRecurringEvents());
          }

          bslmt::ThreadUtil::microSleep(T2, 0);
          nExec = testObj1.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT(1 == x.numEvents());
              // ASSERT(1 == x.numRecurringEvents());
          }
          nExec = testObj2.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T3) {
              LOOP_ASSERT(nExec, 0 == nExec);
              ASSERT(1 == x.numEvents());
              // ASSERT(1 == x.numRecurringEvents());
          }

          bslmt::ThreadUtil::microSleep(T2, 0);
          makeSureTestObjectIsExecuted(testObj1, mT, 100);
          nExec = testObj1.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T5) {
              LOOP_ASSERT(nExec, 1 == nExec);
          }
          makeSureTestObjectIsExecuted(testObj2, mT, 100);
          nExec = testObj2.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          ASSERT(0 == x.numEvents());
          // ASSERT(1 == x.numRecurringEvents());

          bslmt::ThreadUtil::microSleep(T2, 0);
          bslmt::ThreadUtil::microSleep(T, 0);
          makeSureTestObjectIsExecuted(testObj1, mT, 100, 1);
          nExec = testObj1.numExecuted();
          elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T8) {
              LOOP_ASSERT(nExec, 2 == nExec)
          } else {
              LOOP_ASSERT(nExec, 2 <= nExec);
          }
          nExec = testObj2.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule an event at T2,
          // cancel it at T and verify that it is cancelled.  Verify that
          // cancelling it again results in failure.

          const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h;
          x.scheduleEvent(
                      &h,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          bslmt::ThreadUtil::microSleep(T, 0);
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T2) {
              ASSERT( 0 == x.cancelEvent(h) );
              ASSERT( 0 != x.cancelEvent(h) );

              bslmt::ThreadUtil::microSleep(T3, 0);
              nExec = testObj.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
          }
          // Else should we complain that too much time has elapsed?  In any
          // case, this is not a failure, do not stop.
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule an event at T2,
          // cancelling it at T3 should result in failure.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h;
          x.scheduleEvent(
                      &h,
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          microSleep(T3, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 1 == nExec);
          ASSERT( 0 != x.cancelEvent(h) );
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule 3 events at T, T2,
          // T3.  Invoke 'cancelAllEvents' and verify it.

          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T3(3 * DECI_SEC);

          const int T5 = 5 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj1;
          TestClass1 testObj2;
          TestClass1 testObj3;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          EventHandle h1, h2, h3;
          x.scheduleEvent(
                     &h1,
                     now + T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
          x.scheduleEvent(
                     &h2,
                     now + T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
          x.scheduleEvent(
                     &h3,
                     now + T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));
          x.cancelAllEvents();
          bsls::TimeInterval elapsed = bdlt::CurrentTime::now();

          // It is possible that h1 (more likely), h2, or h3 (less likely) have
          // run before they got cancelled because of delays.  (Happened once
          // on xlC-8.2i in 64 bit mode...)  But in either case, cancelling the
          // event again should fail.  However the numExecuted() test below may
          // be 1 in that case.

          ASSERT( 0 != x.cancelEvent(h1) );
          ASSERT( 0 != x.cancelEvent(h2) );
          ASSERT( 0 != x.cancelEvent(h3) );

          bslmt::ThreadUtil::microSleep(T5, 0);

          // Be defensive about this and only assert when *guaranteed* that
          // object cannot have been called back.
          if (elapsed < T) {
              nExec = testObj1.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
          }
          if (elapsed < T2) {
              nExec = testObj2.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
          }
          if (elapsed < T3) {
              nExec = testObj3.numExecuted();
              LOOP_ASSERT(nExec, 0 == nExec);
          }
          // Else should we complain that too much time has elapsed?  In any
          // case, this is not a failure, do not stop.
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule a clock of T3, let
          // it execute once and then cancel it and then verify the expected
          // result.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          RecurringEventHandle h;
          x.scheduleRecurringEvent(
                      &h,
                      T3,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          bslmt::ThreadUtil::microSleep(T4, 0);
          makeSureTestObjectIsExecuted(testObj, mT, 100);
          nExec = testObj.numExecuted();
          LOOP_ASSERT(nExec, 1 <= nExec);

          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          if (elapsed < T6) {
              ASSERT( 0 == x.cancelEvent(h) );
              ASSERT( 0 != x.cancelEvent(h) );

              const int NEXEC = testObj.numExecuted();
              bslmt::ThreadUtil::microSleep(T4, 0);
              nExec = testObj.numExecuted();
              LOOP2_ASSERT(NEXEC, nExec, NEXEC == nExec);
          }
          // Else complain but do not stop the test suite.
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule two clocks of T3
          // interval, invoke 'cancelAllEvents' and verify the result.

          const bsls::TimeInterval T3(3 * DECI_SEC);
          const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj1;
          TestClass1 testObj2;

          RecurringEventHandle h1, h2;
          x.scheduleRecurringEvent(
                     &h1,
                     T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

          x.scheduleRecurringEvent(
                     &h2,
                     T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

          x.cancelAllEvents();
          ASSERT( 0 != x.cancelEvent(h1) );
          ASSERT( 0 != x.cancelEvent(h2) );

          const int NEXEC1 = testObj1.numExecuted();
          const int NEXEC2 = testObj2.numExecuted();
          microSleep(T10, 0);
          nExec = testObj1.numExecuted();
          LOOP2_ASSERT(NEXEC1, nExec, NEXEC1 == nExec);
          nExec = testObj2.numExecuted();
          LOOP2_ASSERT(NEXEC2, nExec, NEXEC2 == nExec);
          x.stop();
        }

        {
          // Create and start a scheduler object, schedule a clock of T3
          // interval, schedule an event at T6, invoke 'stop' at T4 and then
          // verify the state.  Invoke 'start' and then verify the state.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T3(3 * DECI_SEC);
          const bsls::TimeInterval T6(6 * DECI_SEC);

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          TestClass1 testObj1;
          TestClass1 testObj2;

          bsls::TimeInterval now = bdlt::CurrentTime::now();
          x.scheduleRecurringEvent(
                     T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

          x.scheduleEvent(
                     now + T6,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

          microSleep(T4, 0);
          makeSureTestObjectIsExecuted(testObj1, mT, 100);
          x.stop();

          bsls::TimeInterval elapsed = bdlt::CurrentTime::now() - now;
          const int NEXEC1 = testObj1.numExecuted();
          const int NEXEC2 = testObj2.numExecuted();
          if (elapsed < T6) {
              LOOP_ASSERT(NEXEC1, 1 == NEXEC1);
              LOOP_ASSERT(NEXEC2, 0 == NEXEC2);
          } else {
              LOOP_ASSERT(NEXEC1, 1 <= NEXEC1);
          }
          microSleep(T4, 0);
          nExec = testObj1.numExecuted();
          LOOP2_ASSERT(NEXEC1, nExec, NEXEC1 == nExec);
          nExec = testObj2.numExecuted();
          LOOP2_ASSERT(NEXEC2, nExec, NEXEC2 == nExec);

          if (0 == NEXEC2) {
              // If testObj2 has already executed its event, there is not much
              // point to test this.

              x.start();
              microSleep(T4, 0);
              makeSureTestObjectIsExecuted(testObj2, mT, 100, NEXEC2);
              nExec = testObj2.numExecuted();
              LOOP2_ASSERT(NEXEC2, nExec, NEXEC2 + 1 <= nExec);
              x.stop();
          }
          else {
              // However, if testObj2 has already executed its event, we should
              // make sure it do so legally, i.e., after the requisite period
              // of time.  Note that 'elapsed' was measure *after* the
              // 'x.stop()'.

              LOOP2_ASSERT(NEXEC2, elapsed, T6 < elapsed);
          }
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // SYSTEM CLOCK CHANGES CAUSE EXPECTED BEHAVIOR
        //
        // Concerns:
        //: 1 Changes to the system clock must not affect the behavior of the
        //:   component when the monotonic clock is used.
        //
        // Plan:
        //: 1 Use the realtime clock first, verify changes to the system clock
        //:   do affect the behavior, and then repeat the test with a monotonic
        //:   clock and manually verify no effect on behavior.
        //
        // Testing:
        //   SYSTEM CLOCK CHANGES CAUSE EXPECTED BEHAVIOR
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "SYSTEM CLOCK CHANGES CAUSE EXPECTED BEHAVIOR" << endl
                 << "============================================" << endl;
        }

        using namespace EVENTSCHEDULER_TEST_CASE_MINUS_1;

        {
            cout << endl
                 << "First, a realtime clock will be used to verify" << endl
                 << "unexpected behavior during a system clock" << endl
                 << "change (this test will continue until this" << endl
                 << "problem is detected; *you* must move the system" << endl
                 << "time backwards)." << endl
                 << endl;

            Obj x(bsls::SystemClockType::e_REALTIME);
            x.start();

            TestClass1 testObj;

            bsls::TimeInterval currentTime =
                     bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME);
            for (int i = 1; i <= 300; ++i) {
                const bsls::TimeInterval T(static_cast<double>(i));
                x.scheduleEvent(
                      currentTime + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            }

            int numExecuted = -1;
            while (numExecuted < testObj.numExecuted()) {
                numExecuted = testObj.numExecuted();
                microSleep(200000, 1); // 1.2 seconds
            }

            x.stop();
            x.cancelAllEvents();
        }

        {
            cout << endl
                 << "Excellent!  The expected behavior occurred.  For" << endl
                 << "the second part of the test, a monotonic clock" << endl
                 << "will be used.  Here, a message will be printed" << endl
                 << "every second and if the messages stop printing" << endl
                 << "before iteration 300 while you are changing the" << endl
                 << "system time there is an issue.  You may use" << endl
                 << "CNTRL-C to exit this test at any time.  The test" << endl
                 << "will begin in three seconds; do not change the" << endl
                 << "system time until after you see the first printed" << endl
                 << "message." << endl
                 << endl;

            microSleep(0, 3); // 3 seconds

            Obj x(bsls::SystemClockType::e_MONOTONIC);
            x.start();

            TestPrintClass testObj;

            bsls::TimeInterval currentTime =
                     bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC);
            for (int i = 1; i <= 300; ++i) {
                const bsls::TimeInterval T(static_cast<double>(i));
                x.scheduleEvent(
                  currentTime + T,
                  bdlf::MemFnUtil::memFn(&TestPrintClass::callback, &testObj));
            }

            int numExecuted = -1;
            while (numExecuted < testObj.numExecuted()) {
                numExecuted = testObj.numExecuted();
                microSleep(200000, 1); // 1.2 seconds
            }

            x.stop();
            x.cancelAllEvents();
        }

      } break;
      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        TEST_CASE_MINUS_100::run();
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
