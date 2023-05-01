// bdlmt_timereventscheduler.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_timereventscheduler.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bdlf_memfn.h>
#include <bdlb_bitutil.h>
#include <bsl_climits.h>        // for 'CHAR_BIT'
#include <bdlt_datetime.h>
#include <bdlt_timeunitratio.h>

#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslmt_timedsemaphore.h>
#include <bslmt_semaphore.h>
#include <bslmt_barrier.h>
#include <bslmt_testutil.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cmath.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_ostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;
using bsl::ptrdiff_t;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'bdlmt::TimerEventScheduler' is divided into 2 parts (apart from
// breathing test and usage example).  The first part tests the functions in
// isolation, the second is more integrated in that it tests a particular
// situation in context, with a combination of functions.
//
// [2] Verify that callbacks are invoked as expected when multiple clocks and
// multiple events are scheduled.
//
// [3] Verify that 'cancelEvent' works correctly in various white box states.
//
// [4] Verify that 'cancelAllEvents' works correctly in various white box
// states.
//
// [5] Verify that 'cancelClock' works correctly in various white box states.
//
// [6] Verify that 'cancelAllClocks' works correctly in various white box
// states.
//
// [7] Test 'scheduleEvent', 'cancelEvent', 'cancelAllEvents',
// 'startClock', 'cancelClock' and 'cancelAllClocks' when they are invoked from
// dispatcher thread.
//
// [8] Test the scheduler with a user-defined dispatcher.
//
// [9] Verify that 'start' and 'stop' work correctly in various white box
// states.
//
// [10] Verify the concurrent scheduling and cancelling of clocks and events.
//
// [11] Verify the concurrent scheduling and cancelling-all of clocks and
// events.
//
// [14] Given 'numEvents' and 'numClocks' (where 'numEvents <= 2**24 - 1' and
// 'numClocks <= 2**24 - 1'), ensure that *at least* 'numEvents' and
// 'numClocks' may be concurrently scheduled.
// ----------------------------------------------------------------------------
// CREATORS
// [01] bdlmt::TimerEventScheduler(allocator = 0);
// [19] bdlmt::TimerEventScheduler(clockType, allocator = 0);
//
// [08] bdlmt::TimerEventScheduler(dispatcher, allocator = 0);
// [20] bdlmt::TimerEventScheduler(disp, clockType, alloc = 0);
//
// [21] bdlmt::TimerEventScheduler(nE, nC, bA = 0);
// [22] bdlmt::TimerEventScheduler(nE, nC, cT, bA = 0);
//
// [23] bdlmt::TimerEventScheduler(nE, nC, disp, bA = 0);
// [24] bdlmt::TimerEventScheduler(nE, nC, disp, cT, bA = 0);
//
//
// [01] ~bdlmt::TimerEventScheduler();
//
// MANIPULATORS
// [09] int start();
//
// [16] int start(const bslmt::ThreadAttributes& threadAttributes);
//
// [09] void stop();
//
// [02] Handle scheduleEvent(time, callback);
//
// [12] int rescheduleEvent(handle, newTime);
//
// [12] int rescheduleEvent(handle, key, newTime);
//
// [03] int cancelEvent(Handle handle, bool wait=false);
//
// [04] void cancelAllEvents(bool wait=false);
// [17] void cancelAllEvents(bool wait=false);
//
// [02] Handle startClock(interval, callback, startTime=bsls::TimeInterval(0));
//
// [05] int cancelClock(Handle handle, bool wait=false);
//
// [06] void cancelAllClocks(bool wait=false);
//
// ACCESSORS
// [25] bsls::SystemClockType::Enum clockType();
// [27] bsls::TimeInterval now();
// ----------------------------------------------------------------------------
// [01] BREATHING TEST
// [28] DRQS 150475152: AFTER TEST TIME SOURCE DESTRUCTION
// [07] TESTING METHODS INVOCATIONS FROM THE DISPATCHER THREAD
// [10] TESTING CONCURRENT SCHEDULING AND CANCELLING
// [11] TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL
// [26] CLOCK-REPLACEMENT BREATHING TEST
// [29] USAGE EXAMPLE

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

#define ASSERT                   BSLMT_TESTUTIL_ASSERT
#define ASSERTV                  BSLMT_TESTUTIL_ASSERTV

#define GUARD                    BSLMT_TESTUTIL_GUARD

#define Q                        BSLMT_TESTUTIL_Q
#define P                        BSLMT_TESTUTIL_P
#define P_                       BSLMT_TESTUTIL_P_
#define T_                       BSLMT_TESTUTIL_T_
#define L_                       BSLMT_TESTUTIL_L_

#define GUARDED_STREAM(STREAM)   BSLMT_TESTUTIL_GUARDED_STREAM(STREAM)
#define COUT                     BSLMT_TESTUTIL_COUT
#define CERR                     BSLMT_TESTUTIL_CERR

#define E(X)  cout << (X) << endl;     // Print value.
#define E_(X) cout << (X) << flush;    // Print value.

// ============================================================================
//                   THREAD-SAFE OUTPUT AND ASSERT MACROS
// ----------------------------------------------------------------------------
static bslmt::Mutex printMutex;  // mutex to protect output macros
#define ET(X) { printMutex.lock(); E(X); printMutex.unlock(); }
#define ET_(X) { printMutex.lock(); E_(X); printMutex.unlock(); }
#define PT(X) { printMutex.lock(); P(X); printMutex.unlock(); }
#define PT_(X) { printMutex.lock(); P_(X); printMutex.unlock(); }

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

typedef bdlmt::TimerEventScheduler          Obj;
typedef Obj::Handle                         Handle;
typedef bsls::Types::IntPtr                 IntPtr;

void sleepUntilMs(int ms)
{
    int sleepIncrement = bsl::min(250, (ms/4)+1);

    bsls::Stopwatch timer;
    timer.start();
    while (timer.elapsedTime() * 1000 < ms) {
        bslmt::ThreadUtil::microSleep(sleepIncrement);
    }
}

static const float DECI_SEC = 0.1f;      // 1 deci-second (a tenth of a second)

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
    (void) UNACCEPTABLE_DIFFERENCE;
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

void myMicroSleep(int microSeconds, int seconds)
    // Sleep for *at* *least* the specified 'seconds' and 'microseconds'.  This
    // function is used for testing only.  It uses the function
    // 'bslmt::ThreadUtil::microSleep' but interleaves calls to 'yield' to give
    // a chance to the event scheduler to process its dispatching thread.
    // Without this, there have been a large number of unpredictable
    // intermittent failures by this test driver, especially on AIX with
    // xlc-8.0, in the nightly builds (i.e., when the load is higher than
    // running the test driver by hand).  It was noticed that calls to 'yield'
    // helped, and this routine centralizes this as a mechanism.
{
    sleepUntilMs(microSeconds / 1000 + seconds * 1000);
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
        sleepUntilMs(microSeconds / 1000);
        bslmt::ThreadUtil::yield();
    }
}

static inline double dnow()
{
    // current time as a double

    return bsls::SystemTime::nowRealtimeClock().totalSecondsAsDouble();
}

void noop()
    // Do nothing.
{
}

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_AIX)
// On Windows, the thread name will only be set if we're running on Windows 10,
// version 1607 or later, otherwise it will be empty. AIX does not support
// thread naming.
static const bool k_threadNameCanBeEmpty = true;
#else
static const bool k_threadNameCanBeEmpty = false;
#endif

                         // ==========================
                         // function executeInParallel
                         // ==========================

static void executeInParallel(int                               numThreads,
                              bslmt::ThreadUtil::ThreadFunction func)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
    // to i'th thread.  Finally join all the threads.
{
    bslma::Allocator *alloc = &bslma::NewDeleteAllocator::singleton();

    bsl::vector<bslmt::ThreadUtil::Handle> handles(alloc);

    for (int ii = 0; ii < numThreads; ++ii) {
        bslmt::ThreadUtil::Handle handle;
        ASSERT(0 == bslmt::ThreadUtil::createWithAllocator(
                                                          &handle,
                                                          func,
                                                          (void *) (IntPtr) ii,
                                                          alloc));
        handles.push_back(handle);
    }

    for (int ii = 0; ii < numThreads; ++ii) {
        bslmt::ThreadUtil::join(handles[ii]);
    }
}

                              // ===============
                              // class TestClass
                              // ===============

class TestClass {
    // This class encapsulates the data associated with a clock or an event.

    bool                d_isClock;                 // true if this is a clock,
                                                   // false when this is an
                                                   // event

    bsls::TimeInterval  d_periodicInterval;        // periodic interval if this
                                                   // is a recurring event

    bsls::TimeInterval  d_expectedTimeAtExecution; // expected time at which
                                                   // callback should run

    bsls::AtomicInt     d_numExecuted;             // number of times callback
                                                   // has been executed

    bsls::AtomicInt     d_executionTime;           // duration for which
                                                   // callback executes

    int                 d_line;                    // for error reporting

    bsls::AtomicInt     d_delayed;                 // will be set to true if
                                                   // any execution of the
                                                   // callback is delayed from
                                                   // its expected execution
                                                   // time

    bsls::TimeInterval  d_referenceTime;           // time from which execution
                                                   // time is referenced for
                                                   // debugging purpose

    bsls::TimeInterval *d_globalLastExecutionTime; // last time *ANY* callback
                                                   // was executed

    bool                d_assertOnFailure;         // case 2 must not assert on
                                                   // failure unless it fails
                                                   // too many times

    bsls::AtomicInt     d_failures;                // timing failures

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream& , const TestClass&);

  public:
    // CREATORS
    TestClass(int                 line,
              bsls::TimeInterval  expectedTimeAtExecution,
              bsls::TimeInterval *globalLastExecutionTime ,
              int                 executionTime = 0,
              bool                assertOnFailure = true):
      d_isClock(false),
      d_periodicInterval(0),
      d_expectedTimeAtExecution(expectedTimeAtExecution),
      d_numExecuted(0),
      d_executionTime(executionTime),
      d_line(line),
      d_delayed(false),
      d_referenceTime(bsls::SystemTime::nowRealtimeClock()),
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
      d_referenceTime(bsls::SystemTime::nowRealtimeClock()),
      d_globalLastExecutionTime(globalLastExecutionTime),
      d_assertOnFailure(assertOnFailure),
      d_failures(0)
    {
    }

    TestClass(const TestClass& original):
      d_isClock(original.d_isClock),
      d_periodicInterval(original.d_periodicInterval),
      d_expectedTimeAtExecution(original.d_expectedTimeAtExecution),
      d_numExecuted(original.d_numExecuted.load()),
      d_executionTime(original.d_executionTime.load()),
      d_line(original.d_line),
      d_delayed(original.d_delayed.load()),
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
        bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
        if (veryVerbose) {
            printMutex.lock();
            cout << (d_isClock ? "CLOCK" : "EVENT") << " specified at line "
                 << d_line << " executed "
                 << (now - d_referenceTime).nanoseconds() / 1000000
                 << " milliseconds after it was scheduled" << endl;
            printMutex.unlock();
        }

        // if this execution has been delayed due to a long running callback or
        // due to high loads during parallel testing
        if (d_delayed || isApproxGreaterThan(*d_globalLastExecutionTime,
                                             d_expectedTimeAtExecution)) {
            d_delayed = true;
            d_expectedTimeAtExecution = now;
        }

        // assert that it runs on expected time
        if (d_assertOnFailure) {
            ASSERTV(d_line, now, d_expectedTimeAtExecution,
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
            sleepUntilMs(d_executionTime / 1000);
        }

        now = bsls::SystemTime::nowRealtimeClock();
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

    bsls::AtomicInt  d_numStarted;
    bsls::AtomicInt  d_numExecuted;
    int              d_executionTime; // in microseconds

    // CREATORS
    TestClass1() :
    d_numStarted(0),
    d_numExecuted(0),
    d_executionTime(0)
    {
    }

    explicit
    TestClass1(int executionTime) :
    d_numStarted(0),
    d_numExecuted(0),
    d_executionTime(executionTime)
    {
    }

    // MANIPULATORS
    void callback()
    {
        ++d_numStarted;

        bsl::string threadName;
        bslmt::ThreadUtil::getThreadName(&threadName);
        ASSERTV(threadName,
                (k_threadNameCanBeEmpty && threadName.empty()) ||
                    threadName == "bdl.TimerEvent" ||
                    threadName == "OtherName");

        if (d_executionTime) {
            sleepUntilMs(d_executionTime / 1000);
        }

        ++d_numExecuted;
    }

    // ACCESSORS
    int numStarted()
    {
        return d_numStarted;
    }

    int numExecuted()
    {
        return d_numExecuted;
    }
};

                              // ================
                              // class TestClass2
                              // ================

struct TestClass2 {
    // This class define a function 'callback' that is used as a callback for a
    // clock or an event.  The class keeps track of number of times the
    // callback has been executed.  Unlike 'TestClass1', it does not support a
    // delay, but instead supports synchronization using up to 2 barriers.

    bsls::AtomicInt  d_numExecuted;
    bslmt::Barrier  *d_startBarrier_p;
    bslmt::Barrier  *d_finishBarrier_p;

    // CREATORS
    TestClass2(bslmt::Barrier *startBarrier, bslmt::Barrier *finishBarrier)
        // Create a 'TestClass2' object that uses the specified 'startBarrier'
        // (if non-null) and the specified 'finishBarrier' (if non-null) for
        // synchronization.
    : d_numExecuted(0)
    , d_startBarrier_p(startBarrier)
    , d_finishBarrier_p(finishBarrier)
    {
    }

    // MANIPULATORS
    void callback()
        // Arrive and wait at 'd_startBarrier_p' (if non-null), then increment
        // 'd_numExecuted', and finally arrive and wait at 'd_finishBarrier_p'
        // (if non-null).
    {
        bsl::string threadName;
        bslmt::ThreadUtil::getThreadName(&threadName);
        ASSERTV(threadName,
                (k_threadNameCanBeEmpty && threadName.empty()) ||
                    threadName == "bdl.TimerEvent" ||
                    threadName == "OtherName");

        if (d_startBarrier_p) {
            d_startBarrier_p->wait();
        }

        ++d_numExecuted;

        if (d_finishBarrier_p) {
            d_finishBarrier_p->wait();
        }
    }

    // ACCESSORS
    int numExecuted() const
    {
        return d_numExecuted;
    }
};

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

void cancelEventCallback(Obj *scheduler,
                         int *handlePtr,
                         int  wait,
                         int  expectedStatus)
    // Invoke 'cancelEvent' on the specified 'scheduler' passing the specified
    // '*handlePtr' and 'wait' and assert that the result equals the specified
    // 'expectedStatus'.
{
    int ret = scheduler->cancelEvent(*handlePtr, wait);
    ASSERTV(ret, expectedStatus == ret);
}

void cancelEventCallbackWithState(Obj                         *scheduler,
                                  int                         *handlePtr,
                                  const bsl::shared_ptr<int>&  state)
    // Invoke 'cancelEvent' on the specified 'scheduler' passing '*handlePtr'
    // and assert that the specified 'state' remains valid.
{
    const int s = *state;
    int ret = scheduler->cancelEvent(*handlePtr);
    ASSERTV(ret, s == *state);
}

static void cancelClockCallback(Obj *scheduler,
                                int *handlePtr,
                                int  wait,
                                int  expectedStatus)
    // Invoke 'cancelClocks' on the specified 'scheduler' passing the specified
    // '*handlePtr' and 'wait' and assert that the result equals the specified
    // 'expectedStatus'.
{
    int ret = scheduler->cancelClock(*handlePtr, wait);
    ASSERTV(ret, expectedStatus == ret);
}

static void cancelAllEventsCallback(Obj *scheduler, int wait)
    // Invoke 'cancelAllEvents' on the specified 'scheduler' passing 'wait'.
{
    scheduler->cancelAllEvents(wait);
}

static void cancelAllClocksCallback(Obj *scheduler, int wait)
    // Invoke 'cancelAllClocks' on the specified 'scheduler' passing 'wait'.
{
    scheduler->cancelAllClocks(wait);
}

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS FOR TESTING
// ============================================================================

// ----------------------------------------------------------------------------
//                      USAGE EXAMPLE RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMER_EVENT_SCHEDULER_TEST_CASE_USAGE
{

                              // ================
                              // class my_Session
                              // ================

class my_Session{
  public:
    // MANIPULATORS
    int processData(void *data, int length);
};

// MANIPULATORS
int my_Session::processData(void *, int)
{
    return 0;
}

                              // ===============
                              // class my_Server
                              // ===============

class My_Server {

    // TYPES
    struct Connection {
        bdlmt::TimerEventScheduler::Handle d_timerId;
        my_Session *d_session_p;
    };

    // DATA
    bsl::vector<Connection*>     d_connections;
    bdlmt::TimerEventScheduler     d_scheduler;
    bsls::TimeInterval            d_ioTimeout;

    // PRIVATE MANIPULATORS
    void newConnection(Connection *connection);

    void closeConnection(Connection *connection);

    void dataAvailable(Connection *connection, void *data, int length);

  private:
    // NOT IMPLEMENTED
    My_Server(const My_Server&);
    My_Server& operator=(const My_Server&);

  public:
    // CREATORS
    explicit
    My_Server(const bsls::TimeInterval&  ioTimeout,
              bslma::Allocator          *allocator = 0);
    ~My_Server();
};

// CREATORS
My_Server::My_Server(const bsls::TimeInterval&  ioTimeout,
                     bslma::Allocator          *allocator)
: d_connections(allocator)
, d_scheduler(allocator)
, d_ioTimeout(ioTimeout)
{
     d_scheduler.start();
}

My_Server::~My_Server()
{
    d_scheduler.stop();
}

// PRIVATE MANIPULATORS
void My_Server::newConnection(My_Server::Connection *connection)
{
    connection->d_timerId = d_scheduler.scheduleEvent(
          d_scheduler.now() + d_ioTimeout,
          bdlf::BindUtil::bind(&My_Server::closeConnection, this, connection));
}

void My_Server::closeConnection(My_Server::Connection *)
{
}

void My_Server::dataAvailable(My_Server::Connection *connection,
                              void                  *data,
                              int                    length)
{
    if (d_scheduler.cancelEvent(connection->d_timerId)) {
        return;                                                       // RETURN
    }

    connection->d_session_p->processData(data, length);

    connection->d_timerId = d_scheduler.scheduleEvent(
          d_scheduler.now() + d_ioTimeout,
          bdlf::BindUtil::bind(&My_Server::closeConnection, this, connection));
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_USAGE

// ============================================================================
//                         CASE 20 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_24
{

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply execute the specified
    // 'functor'.
{
    functor();
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_24
// ============================================================================
//                         CASE 20 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_23
{

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply execute the specified
    // 'functor'.
{
    functor();
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_23
// ============================================================================
//                         CASE 22 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_22
{

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_22
// ============================================================================
//                         CASE 21 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_21
{

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_21
// ============================================================================
//                         CASE 20 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_20
{

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply execute the specified
    // 'functor'.
{
    functor();
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_20
// ============================================================================
//                         CASE 19 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_19
{

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_19
// ============================================================================
//                         CASE 17 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_18
{
struct Func {
    const int                       d_eventIndex;
        // each event is marked by a unique index
    static Obj*                     s_scheduler;
    static bsl::vector<Obj::Handle> s_handles;
        // 's_handles[i]' is the handle of event with index 'i'
    static bsl::vector<int>         s_indexes;
        // each event, when run, pushes its index to this vector
    static int                      s_kamikaze;
        // index of the event that is to kill itself
    static int                      s_final;
        // index of the event that takes place last and sets 's_finished' to
        // 'true'
    static bsls::AtomicBool         s_finished;
        // indicates all events have run


    explicit
    Func(int index) : d_eventIndex(index) {}

    void operator()();
};

Obj*                     Func::s_scheduler;
bsl::vector<Obj::Handle> Func::s_handles;
bsl::vector<int>         Func::s_indexes;
int                      Func::s_kamikaze;
int                      Func::s_final;
bsls::AtomicBool         Func::s_finished;

void Func::operator()()
{
    s_indexes.push_back(d_eventIndex);

    if (s_kamikaze == d_eventIndex) {
        ASSERT(0 != s_scheduler->cancelEvent(s_handles[d_eventIndex]));
    }
    if (s_final == d_eventIndex) {
        s_finished.storeRelease(true);
    }
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_18

// ============================================================================
//                         CASE 17 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_17
{
struct Func {
    static const Obj *s_scheduler;
    static int s_numEvents;

    void operator()()
    {
        sleepUntilMs(100 * 1000 / 1000);
        -- s_numEvents;

        int diff = bsl::abs(s_scheduler->numEvents() - s_numEvents);
        ASSERTV(s_scheduler->numEvents(), s_numEvents, diff, 2 >= diff);
    }
};
const Obj *Func::s_scheduler;
int Func::s_numEvents;

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_17

// ============================================================================
//                         CASE 16 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMER_EVENT_SCHEDULER_TEST_CASE_16 {

// As of 10/16/08, the default stack size was about 1MB on Solaris, 0.25MB on
// AIX, and 10MB on Linux.

enum { k_BUF_SIZE = 40 * 1000,
       k_STACK_SIZE_IN_BYTES    = 80 * 1000 * 1000,
       k_RECURSE_DEPTH_IN_BYTES = k_STACK_SIZE_IN_BYTES - 1000 * 1000,
       k_MAX_ITERATIONS = 3 * k_STACK_SIZE_IN_BYTES / k_BUF_SIZE };

ptrdiff_t abs(ptrdiff_t x)
{
    return 0 < x ? x : -x;
}

struct Recurser {
    static char              *s_topPtr;
    static bsls::AtomicBool   s_finished;
    static int                s_iterations;

    // CLASS METHOD
    static
    ptrdiff_t deepRecurser(char *prevBuf)
    {
        char buffer[k_BUF_SIZE];
        char * const buf_p = &buffer[0];    // Avoid compiler nitpicking
                                            // warnings because 'buffer' is
                                            // 'char (*)[...]' and not
                                            // 'char *'.

        if (k_MAX_ITERATIONS < ++s_iterations) {
            // if the above fails, the optimizer has turned the recursion into
            // an infinite loop.

            BSLS_ASSERT_INVOKE("max iterations exceeded");
        }

        if (prevBuf == buf_p) {
            // if the above fails, the optimizer has turned the recursion into
            // an infinite loop.

            BSLS_ASSERT_INVOKE("Recursion optimized away");
        }

        ptrdiff_t curDepth = abs(buf_p - s_topPtr);

        if (veryVerbose) {
            cout << "Depth: " << curDepth << endl;
        }

        if (curDepth < k_RECURSE_DEPTH_IN_BYTES) {
            // There was a problem in optimized builds on Linux gcc-11
            // implemented this recursion as a loop, which resulted in an
            // infinite loop.  Corrected by using 'makeFunctionCallNonInline'
            // an increasing checks that will abort if there's an infinite
            // loop, even in a build with asserts other than
            // 'BSLS_ASSERT_INVOKE' disabled.

            // recurse

            ptrdiff_t diff = (*bslmt::TestUtil::makeFunctionCallNonInline(
                                                        &deepRecurser))(buf_p);
            ASSERT(0 <= diff);
        }

        return curDepth;
    }

    // MANIPULATOR
    void operator()()
    {
        char topRef;

        s_topPtr     = &topRef;
        s_finished   = false;
        s_iterations = 0;

        bsls::Stopwatch timer;
        timer.start();

        const IntPtr diff = deepRecurser(&topRef);
        ASSERT(0 <= diff);

        const double elapsed = timer.elapsedTime();
        if (verbose) P(elapsed);
        if (0.15 < elapsed) {
            cout << "Running slow" << endl;
        }

        s_finished = true;
    }
};
char             *Recurser::s_topPtr = 0;
bsls::AtomicBool  Recurser::s_finished(false);
int               Recurser::s_iterations = 0;

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_16

// ============================================================================
//                         CASE 15 RELATED ENTITIES
// ----------------------------------------------------------------------------

// case 15 just reuses the case 14 related entities

// ============================================================================
//                         CASE 14 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMER_EVENT_SCHEDULER_TEST_CASE_14
{
    struct Slowfunctor {
        typedef bsl::list<double>       dateTimeList;

        enum {
            SLEEP_MICROSECONDS = 100*1000
        };
        static const double SLEEP_SECONDS;
        dateTimeList d_timeList;
        dateTimeList& timeList()
        {
            return d_timeList;
        }
        static double timeofday()
        {
            return dnow();
        }
        void callback()
        {
            d_timeList.push_back(timeofday());
            sleepUntilMs(SLEEP_MICROSECONDS / 1000);
            d_timeList.push_back(timeofday());
        }
        double tolerance(int i)
        {
            return Slowfunctor::SLEEP_SECONDS * (0.2 * i + 2);
        }
    };
    const double Slowfunctor::SLEEP_SECONDS =
                   static_cast<double>(Slowfunctor::SLEEP_MICROSECONDS) * 1e-6;
    struct Fastfunctor {
        typedef bsl::list<double>       dateTimeList;
        static const double TOLERANCE;

        dateTimeList d_timeList;
        dateTimeList& timeList()
        {
            return d_timeList;
        }
        static double timeofday()
        {
            return dnow();
        }
        void callback()
        {
            d_timeList.push_back(timeofday());
        }
    };
    const double Fastfunctor::TOLERANCE = Slowfunctor::SLEEP_SECONDS * 3;

    template<class _Tp>
    const _Tp
    abs(const _Tp& __a)
    {
        return 0 <= __a ? __a : - __a;
    }
}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_14

// ----------------------------------------------------------------------------
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace TIMER_EVENT_SCHEDULER_TEST_CASE_13
{

void countInvoked(bsls::AtomicInt *numInvoked)
{
    if (numInvoked) {
        ++*numInvoked;
    }
}

void scheduleEvent(Obj             *scheduler,
                   bsls::AtomicInt *numAdded,
                   bsls::AtomicInt *numInvoked,
                   bslmt::Barrier  *barrier)
{
    barrier->wait();

    while (true) {
        Obj::Handle handle = scheduler->scheduleEvent(
                              bsls::SystemTime::nowRealtimeClock(),
                              bdlf::BindUtil::bind(&countInvoked, numInvoked));
        if (Obj::e_INVALID_HANDLE == handle) {
            break;
        }

        ++*numAdded;
    }
}

void startClock(Obj             *scheduler,
                bsls::AtomicInt *numAdded,
                bsls::AtomicInt *numInvoked,
                bslmt::Barrier  *barrier)
{
    barrier->wait();

    while (true) {
        Obj::Handle handle = scheduler->startClock(
                              bsls::TimeInterval(1),
                              bdlf::BindUtil::bind(&countInvoked, numInvoked));
        if (Obj::e_INVALID_HANDLE == handle) {
            break;
        }

        ++*numAdded;
    }
}

// Calculate the number of bits required to store an index with the specified
// 'maxValue'.
int numBitsRequired(int maxValue)
{
    ASSERT(0 <= maxValue);

    const int sz = static_cast<int>(sizeof(maxValue));
    return (sz * CHAR_BIT) - bdlb::BitUtil::numLeadingUnsetBits(
                                         static_cast<bsl::uint32_t>(maxValue));
}

// Calculate the largest integer identifiable using the specified 'numBits'.
int maxNodeIndex(int numBits)
{
    return (1 << numBits) - 2;
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_13

// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_12
{

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_12
// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_11
{

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
    int id = static_cast<int>(reinterpret_cast<IntPtr>(arg));

    barrier.wait();
    switch(id % 2) {

      // even numbered threads run 'case 0:'
      case 0: {
          for (int i = 0; i< NUM_ITERATIONS; ++i) {
              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              x.scheduleEvent(now + T6,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj[id]));
              x.cancelAllEvents();
              bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
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
              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              x.startClock(T6,
                           bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                  &testObj[id]));
              x.cancelAllClocks();
              bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
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

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_11
// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_10
{

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
    int id = static_cast<int>(reinterpret_cast<IntPtr>(arg));

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
              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              Handle h =
                  x.scheduleEvent(now + T6,
                                  bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                         &testObj[id]));
              if (veryVeryVerbose) {
                  int *handle = reinterpret_cast<int *>(
                                  static_cast<bsl::uintptr_t>(h & 0x8fffffff));
                  printMutex.lock();
                  cout << "\t\tAdded event: "; P_(id); P_(i); P_(h); P(handle);
                  printMutex.unlock();
              }
              if (0 != x.cancelEvent(h) && !testTimingFailure) {
                  // We tried and the 'cancelEvent' failed, but we do not want
                  // to generate an error unless we can *guarantee* that the
                  // 'cancelEvent' should have succeeded.

                  bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
                  ASSERTV(id, i, h, elapsed < T6);
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
              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              Handle h =
                  x.startClock(T6,
                               bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &testObj[id]));
              if (veryVeryVerbose) {
                  void *handle = reinterpret_cast<void *>(
                                  static_cast<bsl::uintptr_t>(h & 0x8fffffff));
                  printMutex.lock();
                  cout << "\t\tAdded clock: "; P_(id); P_(i); P_(h); P(handle);
                  printMutex.unlock();
              }
              if (0 != x.cancelClock(h) && !testTimingFailure) {
                  // We tried and the 'cancelClock' failed, but we do not want
                  // to generate an error unless we can *guarantee* that the
                  // 'cancelClock' should have succeeded.

                  bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
                  ASSERTV(
                      id,
                      i,
                      reinterpret_cast<void *>(static_cast<bsl::uintptr_t>(h)),
                      elapsed < T6);
                  testTimingFailure = (elapsed >= T6);
              }
          }
      }
      break;
    };

    return NULL;
}
} // extern "C"

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_10
// ============================================================================
//                          CASE 9 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_9
{

void postSema(bslmt::TimedSemaphore *sema)
  // Invoke 'sema->post()'.  This method is necessary because 'post' is
  // overloaded and thus cannot be bound directly
{
    sema->post();
}

void waitStopAndSignal(bslmt::Barrier        *barrier,
                       Obj                   *mX,
                       bslmt::TimedSemaphore *sema)
  // Wait on the specified 'barrier', invoke 'stop' on the specified 'mX'
  // scheduler, and finally 'post' on the specified 'sema'.
{
    barrier->wait();
    mX->stop();
    sema->post();
}

void startScheduler(Obj *mX)
  // Invoke 'mX->start()'.  This method is necessary because 'start' is
  // overloaded and thus cannot be bound directly
{
    mX->start();
}

void startStopConcurrencyTest()
{
    // This test tries to expose a vulnerability in a particular implementation
    // of 'stop' and 'start' (white-box testing).  Specifically, if 'start' is
    // executed while 'stop' is trying to join (and shut down) a dispatcher
    // thread that's busy executing an event, it will start a second dispatcher
    // thread with the result that there can be two dispatcher threads running.
    //
    // To expose this issue, block the scheduler's dispatcher thread using
    // a job that waits on a semaphore, and from another thread, stop it.
    // At that point, starting it will (in the current implementation)
    // corrupt the scheduler's state, the most likely manifestation of which
    // will be that it can no longer be stopped.

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(bsls::SystemClockType::e_MONOTONIC, &ta);

    ASSERT(0 == x.start());

    bslmt::Semaphore sema;
    x.scheduleEvent(bsls::SystemTime::nowMonotonicClock(),
                    bdlf::MemFnUtil::memFn(&bslmt::Semaphore::wait, &sema));

    // From another thread, invoke stop, then signal another semaphore.
    bslmt::ThreadUtil::Handle stopThread;
    bslmt::TimedSemaphore stopSema(bsls::SystemClockType::e_MONOTONIC);
    bslmt::Barrier syncBarrier(2);
    bslmt::ThreadUtil::createWithAllocator(
                                       &stopThread,
                                       bdlf::BindUtil::bind(&waitStopAndSignal,
                                                            &syncBarrier,
                                                            &x,
                                                            &stopSema),
                                       &ta);
    syncBarrier.wait();

    // From another thread, invoke start().  (stop() is blocked at this point,
    // and future implementations might block start() if stop() is running)
    bslmt::ThreadUtil::Handle startThread;
    bslmt::ThreadUtil::createWithAllocator(
                                    &startThread,
                                    bdlf::BindUtil::bind(&startScheduler, &x),
                                    &ta);

    // Release the scheduled event so that the dispatcher thread can complete
    sema.post();

    // Finish 'start'ing
    bslmt::ThreadUtil::join(startThread);

    // 'stop' should be able to finish without any problem now. If it takes
    // anything approaching a second, it's deadlocked.

    int rc = stopSema.timedWait(
                          bsls::SystemTime::nowMonotonicClock().addSeconds(1));
    ASSERT(0 == rc);

    // Now submit a job to be executed
    bslmt::TimedSemaphore jobSema(bsls::SystemClockType::e_MONOTONIC);
    x.scheduleEvent(bsls::SystemTime::nowMonotonicClock(),
                    bdlf::BindUtil::bind(&postSema, &jobSema));

    // Depending on just how the threads above are interleaved, the scheduler
    // may be either stopped or started at this point.  Invoke 'start' again
    // (harmlessly) to make sure.
    ASSERT(0 == x.start());

    // Job should have been executed
    rc = jobSema.timedWait(
                          bsls::SystemTime::nowMonotonicClock().addSeconds(1));
    ASSERT(0 == rc);

    // all done; cleanup
    x.stop();
    bslmt::ThreadUtil::join(stopThread);
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_9
// ============================================================================
//                          CASE 8 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_8
{

void dispatcherFunction(bsl::function<void()> functor)
    // This is a dispatcher function that simply execute the specified
    // 'functor'.
{
    functor();
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_8
// ============================================================================
//                          CASE 7 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_7
{

void schedulingCallback(Obj        *scheduler,
                        TestClass1 *event,
                        TestClass1 *clock)
    // Schedule the specified 'event' and the specified 'clock' on the
    // specified 'scheduler'.  Schedule the clock first to ensure that it will
    // get executed before the event does.
{
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const bsls::TimeInterval T4(4 * DECI_SEC);

    scheduler->startClock(T2, bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     clock));

    scheduler->scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T4,
                             bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                    event));
}

void test7_a()
{
      // Schedule an event at T2 that itself schedules an event at T2+T4=T7 and
      // a clock with period T2 (clicking at T4, T6, ...), and verify that
      // callbacks are executed as expected.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T8 = 8 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T10(10 * DECI_SEC);

    TestClass1 event;
    TestClass1 clock;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.scheduleEvent(
                now + T2,
                bdlf::BindUtil::bind(&schedulingCallback, &x, &event, &clock));

    myMicroSleep(T8, 0);
    makeSureTestObjectIsExecuted(event, mT, 200);
    ASSERTV(event.numExecuted(), 1 == event.numExecuted() );
    ASSERTV(clock.numExecuted(), 1 <= clock.numExecuted() );
}

void test7_b()
{
      // Schedule an event e1 at time T such that it cancels itself with wait
      // argument (this will fail), and an event e2 that will be pending when
      // c1 will execute (this is done by waiting long enough before starting
      // the scheduler).  Make sure no deadlock results.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    Handle handleToBeCancelled =
        x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                        bdlf::BindUtil::bind(&cancelEventCallback,
                                             &x,
                                             &handleToBeCancelled,
                                             1,
                                             -1));

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T2,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    myMicroSleep(T6, 0);  // let testObj's callback be pending

    x.start();

    myMicroSleep(T3, 0); // give enough time to complete testObj's callback
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    ASSERT( 1 == testObj.numExecuted() );
}

void test7_c()
{
      // Schedule an event e1 at time T such that it invokes 'cancelAllEvents'
      // with wait argument, and another event e2 at time T2 that will be
      // pending when e1 will execute (this is done by waiting long enough
      // before starting the scheduler).  Make sure no deadlock results of e1
      // waiting for e2 to complete (both are running in the dispatcher
      // thread).

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                    bdlf::BindUtil::bind(&cancelAllEventsCallback, &x, 1));

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T2,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

    myMicroSleep(T6, 0); // let testObj1's callback be pending

    bslmt::ThreadAttributes attr;
    attr.setThreadName("OtherName");
    x.start(attr);

    myMicroSleep(T3, 0); // give enough time to complete testObj's callback
    makeSureTestObjectIsExecuted(testObj1, mT, 100);
    ASSERT( 1 == testObj1.numExecuted() );
}

void test7_d()
{
    // Schedule a clock c1 such that it cancels itself with wait argument (this
    // will fail), and an event e2 that will be pending when c1 will execute
    // (this is done by waiting long enough before starting the scheduler).
    // Make sure no deadlock results.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3  = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T6  = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    Handle handleToBeCancelled = x.startClock(
                                     T,
                                     bdlf::BindUtil::bind(&cancelClockCallback,
                                                          &x,
                                                          &handleToBeCancelled,
                                                          1,
                                                          0));

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T2,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    myMicroSleep(T6, 0);   // let testObj1's callback be pending

    x.start();

    myMicroSleep(T3, 0); // give enough time to complete testObj's callback
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    ASSERT( 1 == testObj.numExecuted() );
}

void test7_e()
{
    // Schedule a clock c1 such that it invokes 'cancelAllClocks' with wait
    // argument, and an event e2 that will be pending when c1 will execute
    // (this is done by waiting long enough before starting the scheduler).
    // Make sure no deadlock results.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    x.startClock(T, bdlf::BindUtil::bind(&cancelAllClocksCallback, &x, 1));

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T2,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    myMicroSleep(T6, 0); // let testObj1's callback be pending

    x.start();

    myMicroSleep(T3, 0); // give enough time to complete testObj's callback
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    ASSERT( 1 == testObj.numExecuted() );
}

void test7_f()
{
      // Cancel from dispatcher thread and verify that the state is still
      // valid.  For DRQS 7272737, it takes two events to reproduce the
      // problem.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    Handle h1, h2;
    {
        bsl::shared_ptr<int> ptr1; ptr1.createInplace(&ta, 1);
        bsl::shared_ptr<int> ptr2; ptr2.createInplace(&ta, 2);

        h1 = x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                             bdlf::BindUtil::bind(
                                                 &cancelEventCallbackWithState,
                                                 &x,
                                                 &h1,
                                                 ptr1));
        h2 = x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                             bdlf::BindUtil::bind(
                                                 &cancelEventCallbackWithState,
                                                 &x,
                                                 &h2,
                                                 ptr2));
    }

    x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T3,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    myMicroSleep(T10, 0);
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    ASSERT( 1 == testObj.numExecuted() );
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_7

// ============================================================================
//                          CASE 6 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_6
{

void test6_a()
{
      // Schedule clocks starting at T3 and T5, invoke 'cancelAllClocks' at
      // time T and make sure that both are cancelled successfully.

    const int T = 1 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const bsls::TimeInterval T5(5 * DECI_SEC);
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T3, bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

    x.startClock(T5, bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

    sleepUntilMs(T / 1000);
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T2) {
        // put a little margin between this and the first clock (T3).

        x.cancelAllClocks();
        myMicroSleep(T6, 0);
        ASSERT( 0 == testObj1.numExecuted() );
        ASSERT( 0 == testObj2.numExecuted() );
    }
}

void test6_b()
{
      // Schedule clocks c1 at T(which executes for T10 time), c2 at T2 and c3
      // at T3.  Let all clocks be simultaneously put onto the pending list
      // (this is done by sleeping enough time before starting the scheduler).
      // Let c1's first execution be started (by sleeping enough time), invoke
      // 'cancelAllClocks' without wait argument, verify that c1's first
      // execution has not yet completed and verify that c2 and c3 are
      // cancelled without any executions.

    bsls::TimeInterval  T(1 * DECI_SEC);
    bsls::TimeInterval  T2(2 * DECI_SEC);
    bsls::TimeInterval  T3(3 * DECI_SEC);

    const int TM1  =  1 * DECI_SEC_IN_MICRO_SEC;
    const int TM4  =  4 * DECI_SEC_IN_MICRO_SEC;
    const int TM10 = 10 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1(TM10);
    TestClass1 testObj2;
    TestClass1 testObj3;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1),
                 now - T3);

    x.startClock(T2,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2),
                 now - T2);

    x.startClock(T3,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3),
                 now - T);

    double start = dnow();
    x.start();
    myMicroSleep(TM4, 0); // let the callback of 'testObj1' be started
    x.cancelAllClocks();

    if (testObj1.numStarted() > 0 && dnow() - start < 0.9) {
        ASSERT( 0 == testObj1.numExecuted() );
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );

        while ( 1 > testObj1.numExecuted() ) {
            myMicroSleep(TM1, 0);
        }
        myMicroSleep(TM10, 0);
        ASSERT( 1 == testObj1.numExecuted() );
        ASSERT( 0 == testObj2.numExecuted() );
        ASSERT( 0 == testObj3.numExecuted() );
    }
}

void test6_c()
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

    const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const int T5 = 5 * DECI_SEC_IN_MICRO_SEC;
    const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
    const int T15 = 15 * DECI_SEC_IN_MICRO_SEC;

    if (veryVerbose) {
        P_(T4); P_(T5); P_(T10); P(T15);
    }

    TestClass1 testObj1(T10);
    TestClass1 testObj2;
    TestClass1 testObj3;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1),
                 now - T2);

    x.startClock(T2,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2),
                 now - T);

    x.startClock(T3,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3),
                 now);

    double start = dnow();
    x.start();
    myMicroSleep(T4, 0); // let the callback of 'testObj1' be started
    double endSleep = dnow();
    int wait = 1;
    x.cancelAllClocks(wait);
    int numExecuted[4];
    numExecuted[1] = testObj1.numExecuted();
    numExecuted[2] = testObj2.numExecuted();
    numExecuted[3] = testObj3.numExecuted();
    if (endSleep - start <= 0.9) {
        ASSERT( 1 == numExecuted[1] );
        ASSERT( 0 == numExecuted[2] );
        ASSERT( 0 == numExecuted[3] );
    }
    else {
        if (verbose) ET("test6_c() overslept");
    }
    myMicroSleep(T15, 0);
    ASSERT( numExecuted[1] == testObj1.numExecuted() );
    ASSERT( numExecuted[2] == testObj2.numExecuted() );
    ASSERT( numExecuted[3] == testObj3.numExecuted() );
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_6
// ============================================================================
//                          CASE 5 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_5
{
struct Unblock {
    Obj            *d_scheduler_p;
    bslmt::Barrier *d_startBarrier_p;
    bslmt::Barrier *d_finishBarrier_p;

    Unblock(Obj            *scheduler,
            bslmt::Barrier *startBarrier,
            bslmt::Barrier *finishBarrier)
    : d_scheduler_p(scheduler)
    , d_startBarrier_p(startBarrier)
    , d_finishBarrier_p(finishBarrier)
    {
    }

    void operator()()
        // Arrive and wait at '*d_startBarrier_p'.  Wait until '*d_scheduler_p'
        // no longer has any scheduled clocks, then arrive at
        // '*d_finishBarrier_p' in order to unblock the main thread.
    {
        d_startBarrier_p->wait();
        while (d_scheduler_p->numClocks()) {
            bslmt::ThreadUtil::yield();
        }
        d_finishBarrier_p->arrive();
    }
};
}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_5
// ============================================================================
//                          CASE 4 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_4
{

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_4
// ============================================================================
//                          CASE 3 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_3
{

void test3_a()
{
    if (verbose) ET_("\tSchedule event and cancel before execution.\n");

      // Schedule an event at T2, cancel it at time T and verify the result.

    const int T = 1 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const bsls::TimeInterval T5(5 * DECI_SEC);
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle h = x.scheduleEvent(now + T5,
                               bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &testObj));
    sleepUntilMs(T / 1000);
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T2) {
        ASSERT( 0 == x.cancelEvent(h) );
        myMicroSleep(T6, 0);
        ASSERT( 0 == testObj.numExecuted() );
    }
}

void test3_b()
{
    if (verbose) ET_("\tCancel pending event (should fail).\n");
      // Schedule 2 events at T and T2.  Let both be simultaneously put onto
      // the pending list (this is done by sleeping enough time before starting
      // the scheduler), invoke 'cancelEvent(handle)' on the second event while
      // it is still on pending list and verify that cancel fails.

    bsls::TimeInterval  T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1(T10);
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    (void)x.scheduleEvent(now + T,
                          bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                 &testObj1));

    Handle h2 = x.scheduleEvent(now + T2,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                       &testObj2));
    myMicroSleep(T3, 0);
    x.start();
    myMicroSleep(T3, 0);
    ASSERT( 0 != x.cancelEvent(h2) );

    if (dnow() - now.totalSecondsAsDouble() < 1.29) {
        ASSERT( 0 == testObj2.numExecuted() );
    }
    x.stop();
    ASSERT( 1 == testObj2.numExecuted() );
}

void test3_c()
{
    if (verbose) ET_("\tCancel pending event (should fail again).\n");
      // Schedule 2 events at T and T2.  Let both be simultaneously put onto
      // the pending list (this is done by sleeping enough time before starting
      // the scheduler), invoke 'cancelEvent(handle, wait)' on the second event
      // while it is still on pending list and verify that cancel fails.

    bsls::TimeInterval  T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1(T10);
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    (void)x.scheduleEvent(now + T,
                          bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                 &testObj1));

    Handle h2 = x.scheduleEvent(now + T2,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                       &testObj2));
    myMicroSleep(T3, 0);
    x.start();
    myMicroSleep(T3, 0);  // give enough time to be put on pending list
    int wait = 1;
    ASSERT( 0 != x.cancelEvent(h2, wait) );
    ASSERT( 1 == testObj2.numExecuted() );
}

void test3_d()
{
    if (verbose) ET_("\tCancel event from another event prior.\n");
      // Schedule events e1 and e2 at T and T2 respectively, cancel e2 from e1
      // and verify that cancellation succeed.

    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    Handle handleToBeCancelled;
    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    handleToBeCancelled = x.scheduleEvent(
                       now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    // It could possibly happen that testObj has already been scheduled for
    // execution, and thus the following cancelEvent will fail.  Make sure that
    // does not happen.
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T) {
        x.scheduleEvent(now + T,
                        bdlf::BindUtil::bind(&cancelEventCallback,
                                             &x,
                                             &handleToBeCancelled,
                                             0,
                                             0));

        myMicroSleep(T3, 0);
        ASSERT( 0 == testObj.numExecuted() );
    }
    // Else should we complain that too much time has elapsed?  In any case,
    // this is not a failure, do not stop.
}

void test3_e()
{
    if (verbose) ET_("\tCancel event from subsequent event (should fail).\n");
      // Schedule events e1 and e2 at T and T2 respectively, cancel e1 from e2
      // and verify that cancellation fails.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();

    Handle handleToBeCancelled;
    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    handleToBeCancelled = x.scheduleEvent(
                      now + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    x.scheduleEvent(now + T2,
                    bdlf::BindUtil::bind(&cancelEventCallback,
                                         &x,
                                         &handleToBeCancelled,
                                         0,
                                         -1));

    myMicroSleep(T3, 0);
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    ASSERT( 1 == testObj.numExecuted() );
}

void test3_f()
{
    if (verbose) ET_("\tCancel event from itself (should fail).\n");
      // Schedule an event that invokes cancel on itself.  Verify that
      // cancellation fails.

    const bsls::TimeInterval T(1 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta); x.start();
    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle handleToBeCancelled =
        x.scheduleEvent(now + T,
                        bdlf::BindUtil::bind(&cancelEventCallback,
                                             &x,
                                             &handleToBeCancelled,
                                             0,
                                             -1));

    myMicroSleep(T3, 0);
    // The assert is performed by 'cancelEventCallback'.
}

void test3_g()
{
    if (verbose) ET_("\tCancel pending event from pending event prior.\n");
      // Schedule e1 and e2 at T and T2 respectively.  Let both be
      // simultaneously put onto the pending list (this is done by sleeping
      // enough time before starting the scheduler), cancel e2 from e1 and
      // verify that it succeeds.

    bsls::TimeInterval  T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle handleToBeCancelled = x.scheduleEvent(
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

    x.scheduleEvent(now + T,
                    bdlf::BindUtil::bind(&cancelEventCallback,
                                         &x,
                                         &handleToBeCancelled,
                                         0,
                                         0));

    myMicroSleep(T3, 0);
    x.start();

    // Note that it is guaranteed that pending events are executed in time
    // order.
    myMicroSleep(T3, 0);
    ASSERT( 0 == testObj.numExecuted() );
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_3
// ============================================================================
//                          CASE 2 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_2
{

struct TestCase2Data {
    int               d_line;
    float             d_startTime;         // in 1/10th of a sec.
    bool              d_isClock;
    float             d_periodicInterval;  // in 1/10th of a sec.
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
    bsls::TimeInterval globalLastExecutionTime =
                                          bsls::SystemTime::nowRealtimeClock();

    bool assertOnFailure = (failures == 0);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();

    // the assumption is this loop will complete in insignificant time
    for (int i = 0; i < NUM_DATA; ++i) {
        const int   LINE             = DATA[i].d_line;
        const float STARTTIME        = DATA[i].d_startTime;
        const bool  ISCLOCK          = DATA[i].d_isClock;
        const float PERIODICINTERVAL = DATA[i].d_periodicInterval;
        const int   EXECUTIONTIME    = DATA[i].d_executionTime;

        if (ISCLOCK) {
            testObjects[i] = new TestClass(
                               LINE,
                               now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                               bsls::TimeInterval(PERIODICINTERVAL * DECI_SEC),
                               &globalLastExecutionTime,
                               EXECUTIONTIME * DECI_SEC_IN_MICRO_SEC,
                               assertOnFailure);

            x.startClock(bsls::TimeInterval(PERIODICINTERVAL * DECI_SEC),
                         bdlf::MemFnUtil::memFn(&TestClass::callback,
                                                testObjects[i]),
                         now + bsls::TimeInterval(STARTTIME * DECI_SEC));
        }
        else {
            testObjects[i] = new TestClass(
                                LINE,
                                now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                                &globalLastExecutionTime,
                                EXECUTIONTIME * DECI_SEC_IN_MICRO_SEC,
                                assertOnFailure);

            x.scheduleEvent(now + bsls::TimeInterval(STARTTIME * DECI_SEC),
                            bdlf::MemFnUtil::memFn(&TestClass::callback,
                                                   testObjects[i]));
        }
    }

    x.start();
    double delta = .5;
    myMicroSleep((int) ((totalTime + delta) * DECI_SEC_IN_MICRO_SEC), 0);
    x.stop();
    double finishTime = dnow();
    finishTime = (finishTime - now.totalSecondsAsDouble()) * 10 - delta;
    // if the 'microSleep' function slept for exactly how long we asked it to,
    // 'finishTime' should equal totalTime, but myMicroSleep often lags.  By a
    // lot.

    bool result = true;
    for (int i = 0; i < NUM_DATA; ++i) {
        const int   LINE             = DATA[i].d_line;
        const float STARTTIME        = DATA[i].d_startTime;
        const bool  ISCLOCK          = DATA[i].d_isClock;
        const float PERIODICINTERVAL = DATA[i].d_periodicInterval;
        const bool  DELAYED          = DATA[i].d_delayed;

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
            ASSERTV(LINE, DELAYED == testObjects[i]->delayed());
        } else if (DELAYED != testObjects[i]->delayed()) {
            result = false;
            *failures += 10000; // large number to trigger overall failure
        }
        if (ISCLOCK) {
            if (!testObjects[i]->delayed()) {
                int n1 = (int) ((finishTime-STARTTIME) / PERIODICINTERVAL) + 1;
                    // this formula is bogus in general but works for the data
                    // and relies on totalTime being a float
                int n2 = testObjects[i]->numExecuted();
                if (assertOnFailure) {
                    ASSERTV(LINE, n1, n2, n1 == n2);
                } else if (n1 != n2) {
                    result = false;
                    *failures += 10000; // idem
                }
            }
        }
        else {
            if (assertOnFailure) {
                ASSERTV(LINE, 1 == testObjects[i]->numExecuted());
            } else if (1 != testObjects[i]->numExecuted()) {
                result = false;
            }
        }
        delete testObjects[i];
    }
    return result;
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_2
// ============================================================================
//                          CASE 1 RELATED ENTITIES
// ----------------------------------------------------------------------------
namespace TIMER_EVENT_SCHEDULER_TEST_CASE_1
{

void test1_a()
{
    // Create and start a scheduler object, schedule a clock of T3 interval,
    // schedule an event at T6, invoke 'stop' at T4 and then verify the state.
    // Invoke 'start' and then verify the state.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const bsls::TimeInterval T6(6 * DECI_SEC);

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T3, bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

    x.scheduleEvent(now + T6,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

    myMicroSleep(T4, 0);
    makeSureTestObjectIsExecuted(testObj1, mT, 100);
    x.stop();

    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    const int NEXEC1 = testObj1.numExecuted();
    const int NEXEC2 = testObj2.numExecuted();
    if (elapsed < T6) {
        ASSERTV(NEXEC1, 1 == NEXEC1);
        ASSERTV(NEXEC2, 0 == NEXEC2);
    } else {
        ASSERTV(NEXEC1, 1 <= NEXEC1);
    }
    myMicroSleep(T4, 0);
    int nExec = testObj1.numExecuted();
    ASSERTV(NEXEC1, nExec, NEXEC1 == nExec);
    nExec = testObj2.numExecuted();
    ASSERTV(NEXEC2, nExec, NEXEC2 == nExec);

    if (0 == NEXEC2) {
        // If testObj2 has already executed its event, there is not much point
        // to test this.

        x.start();
        myMicroSleep(T4, 0);
        makeSureTestObjectIsExecuted(testObj2, mT, 100, NEXEC2);
        nExec = testObj2.numExecuted();
        ASSERTV(NEXEC2, nExec, NEXEC2 + 1 <= nExec);
    }
    else {
        // However, if testObj2 has already executed its event, we should make
        // sure it do so legally, i.e., after the requisite period of time.
        // Note that 'elapsed' was measure *after* the 'x.stop()'.

        ASSERTV(NEXEC2, elapsed, T6 < elapsed);
    }
}

void test1_b()
{
    // Create and start a scheduler object, schedule two clocks of T3 interval,
    // invoke 'cancelAllClocks' and verify the result.

    const bsls::TimeInterval T3(3 * DECI_SEC);
    const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    Handle h1 = x.startClock(T3,
                             bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                    &testObj1));

    Handle h2 = x.startClock(T3,
                             bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                    &testObj2));

    x.cancelAllClocks();
    ASSERT( 0 != x.cancelClock(h1) );
    ASSERT( 0 != x.cancelClock(h2) );

    const int NEXEC1 = testObj1.numExecuted();
    const int NEXEC2 = testObj2.numExecuted();
    myMicroSleep(T10, 0);
    int nExec = testObj1.numExecuted();
    ASSERTV(NEXEC1, nExec, NEXEC1 == nExec);
    nExec = testObj2.numExecuted();
    ASSERTV(NEXEC2, nExec, NEXEC2 == nExec);
}

void test1_c()
{
    // Create and start a scheduler object, schedule a clock of T3, let it
    // execute once and then cancel it and then verify the expected result.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle h = x.startClock(T3,
                            bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                   &testObj));

    sleepUntilMs(T4 / 1000);
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    int nExec = testObj.numExecuted();
    ASSERTV(nExec, 1 <= nExec);

    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T6) {
        ASSERT( 0 == x.cancelClock(h) );
        ASSERT( 0 != x.cancelClock(h) );

        const int NEXEC = testObj.numExecuted();
        sleepUntilMs(T4 / 1000);
        nExec = testObj.numExecuted();
        ASSERTV(NEXEC, nExec, NEXEC == nExec);
    }
    // Else complain but do not stop the test suite.
}

void test1_d()
{
    // Create and start a scheduler object, schedule 3 events at T,
    // T2, T3.  Invoke 'cancelAllEvents' and verify it.

    const bsls::TimeInterval T(1 * DECI_SEC);
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const bsls::TimeInterval T3(3 * DECI_SEC);

    const int T5 = 5 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj1;
    TestClass1 testObj2;
    TestClass1 testObj3;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle h1 = x.scheduleEvent(now + T,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                       &testObj1));
    Handle h2 = x.scheduleEvent(now + T2,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                       &testObj2));
    Handle h3 = x.scheduleEvent(now + T3,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                       &testObj3));
    x.cancelAllEvents();
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock();

    // It is possible that h1 (more likely), h2, or h3 (less likely) have run
    // before they got cancelled because of delays.  (Happened once on xlC-8.2i
    // in 64 bit mode...)  But in either case, cancelling the event again
    // should fail.  However the numExecuted() test below may be 1 in that
    // case.

    ASSERT( 0 != x.cancelEvent(h1) );
    ASSERT( 0 != x.cancelEvent(h2) );
    ASSERT( 0 != x.cancelEvent(h3) );

    sleepUntilMs(T5 / 1000);

    int nExec;

    // Be defensive about this and only assert when *guaranteed* that object
    // cannot have been called back.
    if (elapsed < T) {
        nExec = testObj1.numExecuted();
        ASSERTV(nExec, 0 == nExec);
    }
    if (elapsed < T2) {
        nExec = testObj2.numExecuted();
        ASSERTV(nExec, 0 == nExec);
    }
    if (elapsed < T3) {
        nExec = testObj3.numExecuted();
        ASSERTV(nExec, 0 == nExec);
    }
    // Else should we complain that too much time has elapsed?  In any case,
    // this is not a failure, do not stop.
}

void test1_e()
{
    // Create and start a scheduler object, schedule an event at
    // T2, cancelling it at T3 should result in failure.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle h = x.scheduleEvent(now + T2,
                               bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &testObj));

    myMicroSleep(T3, 0);
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    int nExec = testObj.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    ASSERT( 0 != x.cancelEvent(h) );
}

void test1_f()
{
    // Create and start a scheduler object, schedule an event at T2, cancel it
    // at T and verify that it is cancelled.  Verify that cancelling it again
    // results in failure.

    const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    Handle h = x.scheduleEvent(now + T2,
                               bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &testObj));

    sleepUntilMs(T / 1000);
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T2) {
        ASSERT( 0 == x.cancelEvent(h) );
        ASSERT( 0 != x.cancelEvent(h) );

        sleepUntilMs(T3 / 1000);
        int nExec = testObj.numExecuted();
        ASSERTV(nExec, 0 == nExec);
    }
    // Else should we complain that too much time has elapsed?  In any case,
    // this is not a failure, do not stop.
}

void test1_g()
{
    // Create and start a scheduler object, schedule a clock of T3 interval,
    // schedule an event at T3.  Verify that event and clock callbacks are
    // being invoked at expected times.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 100 microsecs
    const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
    const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const bsls::TimeInterval T5(5 * DECI_SEC);
    const bsls::TimeInterval T6(6 * DECI_SEC);
    const bsls::TimeInterval T8(8 * DECI_SEC);
    const bsls::TimeInterval T9(9 * DECI_SEC);

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T3, bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
    x.scheduleEvent(now + T3,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERT(1 == x.numEvents());
        ASSERT(1 == x.numClocks());
    }

    sleepUntilMs(T2 / 1000);
    int nExec = testObj1.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERTV(nExec, 0 == nExec);
        ASSERT(1 == x.numEvents());
        // ASSERT(1 == x.numClocks());
    }
    nExec = testObj2.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERTV(nExec, 0 == nExec);
        ASSERT(1 == x.numEvents());
        // ASSERT(1 == x.numClocks());
    }

    sleepUntilMs(T2 / 1000);
    makeSureTestObjectIsExecuted(testObj1, mT, 100);
    nExec = testObj1.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T5) {
        ASSERTV(nExec, 1 == nExec);
    }
    makeSureTestObjectIsExecuted(testObj2, mT, 100);
    nExec = testObj2.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    ASSERT(0 == x.numEvents());
    // ASSERT(1 == x.numClocks());

    sleepUntilMs(T2 / 1000);
    sleepUntilMs(T / 1000);
    makeSureTestObjectIsExecuted(testObj1, mT, 100, 1);
    nExec = testObj1.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T8) {
        ASSERTV(nExec, 2 == nExec);
    } else {
        ASSERTV(nExec, 2 <= nExec);
    }
    nExec = testObj2.numExecuted();
    ASSERTV(nExec, 1 == nExec);
}

void test1_h()
{
    // Create and start a scheduler object, schedule a clock of T3 interval,
    // verify that clock callback gets invoked at expected times.  The reason
    // we cannot test that 1 == X.numClocks() is that the clocks may be in the
    // middle of execution (popped and not yet rescheduled, in which case
    // 0 == X.numClocks()).

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const int T4 = 4 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T6(6 * DECI_SEC);

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.startClock(T3, bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERT(0 == x.numEvents());
        ASSERT(1 == x.numClocks());
    }

    sleepUntilMs(T2 / 1000);
    int nExec = testObj.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERTV(nExec, 0 == nExec);
    }
    ASSERT(0 == x.numEvents());
    // ASSERT(1 == x.numClocks());

    sleepUntilMs(T2 / 1000);
    makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
    nExec = testObj.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T6) {
        ASSERTV(nExec, 1 == nExec);
    }
    else {
        ASSERTV(nExec, 1 <= nExec);
    }

    myMicroSleep(T4, 0);
    makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
    nExec = testObj.numExecuted();
    ASSERTV(nExec, 2 <= nExec);
    ASSERT(0 == x.numEvents());
    // ASSERT(1 == x.numClocks());
}

void test1_i()
{
    // Create and start a scheduler object, schedule 2 events at different
    // times, verify that they execute at expected time.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
    const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T3(3 * DECI_SEC);
    const bsls::TimeInterval T4(4 * DECI_SEC);
    const bsls::TimeInterval T5(5 * DECI_SEC);
    const bsls::TimeInterval T6(6 * DECI_SEC);

    TestClass1 testObj1;
    TestClass1 testObj2;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    ASSERT(0 == x.numEvents());
    ASSERT(0 == x.numClocks());

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.scheduleEvent(now + T4,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));
    x.scheduleEvent(now + T6,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERT(2 == x.numEvents());
    }
    ASSERT(0 == x.numClocks());

    sleepUntilMs(T2 / 1000);
    int nExec = testObj1.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T3) {
        ASSERTV(nExec, 0 == nExec);
        ASSERT(2 == x.numEvents());
        ASSERT(0 == x.numClocks());
    }
    nExec = testObj2.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T5) {
        ASSERTV(nExec, 0 == nExec);
        ASSERT(2 - testObj1.numExecuted() == x.numEvents());
        ASSERT(0 == x.numClocks());
    }

    sleepUntilMs(T2 / 1000);
    makeSureTestObjectIsExecuted(testObj1, mT, 100);
    nExec = testObj1.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    nExec = testObj2.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T5) {
        ASSERTV(nExec, 0 == nExec);
        ASSERT(1 == x.numEvents());
        ASSERT(0 == x.numClocks());
    }

    myMicroSleep(T2, 0);
    myMicroSleep(T, 0);
    nExec = testObj1.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    makeSureTestObjectIsExecuted(testObj2, mT, 100);
    nExec = testObj2.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    ASSERT(0 == x.numEvents());
    ASSERT(0 == x.numClocks());
}

void test1_j()
{
    // Create and start a scheduler object, schedule an event at T2, using key
    // K2.  Try to cancel using K1 and verify that it fails.  Then try to
    // cancel using K2 and verify that it succeeds.

    const int T = 1 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);
    x.start();

    typedef Obj::EventKey Key;

    const Key K1(123);
    const Key K2(456);

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    double start = dnow();
    Handle h = x.scheduleEvent(now + T2,
                               bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &testObj),
                               K2);
    ASSERT(1 == x.numEvents());
    ASSERT(0 == x.numClocks());

    ASSERT( 0 != x.cancelEvent(h, K1) );
    int sts = x.cancelEvent(h, K2);
    bool fast = dnow() - start < 0.2;
    if (fast) {
        ASSERT( 0 == sts );   // this could fail due to long
         // delays and unfairness of thread allocation, but very unlikely
    }
    ASSERT(0 == x.numEvents());
    ASSERT(0 == x.numClocks());

    myMicroSleep(T, 0);
    int nExec = testObj.numExecuted();
    ASSERTV(nExec, !fast || 0 == nExec); // ok, even if overslept
    myMicroSleep(T3, 0);
    nExec = testObj.numExecuted();
    ASSERTV(nExec, !fast || 0 == nExec); // ok, even if overslept
}

void test1_k()
{
    // Create and start a scheduler object, schedule an event at T2, verify
    // that it is not executed at T but is executed at T3.

    const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
    const int T = 1 * DECI_SEC_IN_MICRO_SEC;
    const bsls::TimeInterval T2(2 * DECI_SEC);
    const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

    TestClass1 testObj;

    bslma::TestAllocator ta(veryVeryVerbose);
    Obj x(&ta);

    ASSERT(0 == x.numEvents());
    ASSERT(0 == x.numClocks());

    x.start();

    bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
    x.scheduleEvent(now + T2,
                    bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
    bsls::TimeInterval elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    if (elapsed < T2) {
        ASSERT(1 == x.numEvents());
    }
    ASSERT(0 == x.numClocks());

    sleepUntilMs(T / 1000);
    int nExec = testObj.numExecuted();
    elapsed = bsls::SystemTime::nowRealtimeClock() - now;
    // myMicroSleep could have overslept, especially if load is high
    if (elapsed < T2) {
        ASSERTV(nExec, 0 == nExec);
    }

    sleepUntilMs(T3 / 1000);
    makeSureTestObjectIsExecuted(testObj, mT, 100);
    nExec = testObj.numExecuted();
    ASSERTV(nExec, 1 == nExec);
    ASSERT(0 == x.numEvents());
    ASSERT(0 == x.numClocks());
}

}  // close namespace TIMER_EVENT_SCHEDULER_TEST_CASE_1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;
    int nExec;

    bslma::TestAllocator ta;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    bslma::TestAllocator globalAllocator;
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator;
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_USAGE;
        bslma::TestAllocator ta(veryVeryVerbose);
        My_Server server(bsls::TimeInterval(10), &ta);

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // DRQS 150475152: AFTER TEST TIME SOURCE DESTRUCTION
        //
        // Concerns:
        //: 1 When the test time source is destroyed, the current time functor
        //:   in the associated scheduler remains valid.
        //
        // Plan:
        //: 1 Directly test the scenario.  (C-1)
        //
        // Testing:
        //   DRQS 150475152: AFTER TEST TIME SOURCE DESTRUCTION
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "DRQS 150475152: AFTER TEST TIME SOURCE DESTRUCTION\n"
                 << "==================================================\n";
        }

        bdlmt::TimerEventScheduler scheduler;
        const bsls::TimeInterval   t1 = scheduler.now();
        bsls::TimeInterval         t2;
        {
            bdlmt::TimerEventSchedulerTestTimeSource timeSource(&scheduler);
            t2 = scheduler.now();
        }
        ASSERT(bsls::TimeInterval(100) < t2 - t1);

        // If the functor stored in the scheduler is no longer valid, in safe
        // mode builds the resultant access to 'd_currentTimeFunctor' results
        // in an assert due to 'd_currentTimeMutex' attempting to be locked
        // after being destroyed.

        scheduler.startClock(bsls::TimeInterval(1), noop);

        bslmt::ThreadUtil::microSleep(100000);

        ASSERT(scheduler.now() == t2);
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING NOW ACCESSOR
        //
        // Concern:
        //   That the 'now' accessor correctly returns the current time
        //   according to the clock the object was constructed with, or a test
        //   time source.
        //
        // Plan:
        //   Create objects with all values of 'clockType', and verify that the
        //   value returned by the 'now' accessor is as expected.  Then create
        //   a test time source and verify that the value returned by the 'now'
        //   accessor matches that of the test time source.
        //
        // Testing:
        //   bsls::TimeInterval now() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING NOW ACCESSOR\n"
                             "====================\n";

        const bsls::SystemClockType::Enum realTime =
                                            bsls::SystemClockType::e_REALTIME;
        const bsls::SystemClockType::Enum monotonic =
                                            bsls::SystemClockType::e_MONOTONIC;

        ASSERT(realTime != monotonic);

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) cout << "Realtime clock\n";
        {
            Obj x(realTime, &ta);    const Obj& X = x;

            ASSERT(realTime == X.clockType());

            bsls::TimeInterval xnow = X.now();
            bsls::TimeInterval cnow = bsls::SystemTime::now(X.clockType());
            ASSERT(xnow <= cnow);

            cnow = bsls::SystemTime::now(X.clockType());
            xnow = X.now();
            ASSERT(cnow <= xnow);
        }

        if (verbose) cout << "Monotonic clock\n";
        {
            Obj x(monotonic, &ta);    const Obj& X = x;

            ASSERT(monotonic == X.clockType());

            bsls::TimeInterval xnow = X.now();
            bsls::TimeInterval cnow = bsls::SystemTime::now(X.clockType());
            ASSERT(xnow <= cnow);

            cnow = bsls::SystemTime::now(X.clockType());
            xnow = X.now();
            ASSERT(cnow <= xnow);
        }

        if (verbose) cout << "Test time source\n";
        {
            Obj x(&ta);    const Obj& X = x;

            bdlmt::TimerEventSchedulerTestTimeSource timeSource(&x);

            ASSERT(X.now() == timeSource.now());

            timeSource.advanceTime(bsls::TimeInterval(
                                              bdlt::TimeUnitRatio::k_S_PER_D));
            ASSERT(X.now() == timeSource.now());
        }

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // CLOCK-REPLACEMENT BREATHING TEST:
        //   Exercise the basic functionality of the clock-replacement
        //   mechanism.
        //
        // Concerns:
        //: 1 The clock replacement mechanism exhibits a basic ability to
        //:   alter the clock used for scheduling and dispatching events.
        //
        // Plan:
        //: 1 Default-construct a scheduler.
        //:
        //: 2 Create a 'bdlmt::TimerEventSchedulerTestTimeSource' object,
        //:   passing the constructor a pointer to the scheduler.
        //:
        //: 3 Call 'now' on the 'bdlmt::TimerEventSchedulerTestTimeSource'
        //:   object, and store the result in a variable 'basisTime'.
        //:
        //: 4 Schedule a one-time event in the scheduler, using the retrieved
        //:   basis time.
        //:
        //: 5 Schedule a recurring event in the scheduler, using no absolute
        //:   time.
        //:
        //: 6 Start the scheduler.
        //:
        //: 7 Adjust the time so that the first event will run.  Ensure that
        //:   the first event has run, and not the second.
        //:
        //: 8 Adjust the time so that the recurring event will run.  Ensure
        //:   that this event has run once.
        //:
        //: 9 Adjust the time so that the recurring event will run a second
        //:    time.  Ensure that this event has run again.
        //
        // Testing:
        //   Basic clock-replacement functionality.
        // --------------------------------------------------------------------

        // Convention for this test driver:
        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms

        // Create objects for both events
        TestClass1 event1;
        TestClass1 event2;

        // Construct the scheduler
        bdlmt::TimerEventScheduler scheduler;

        // Construct the time-source.
        // Install the time-source in the scheduler.
        bdlmt::TimerEventSchedulerTestTimeSource timeSource(&scheduler);

        // Retrieve the initial time held in the time-source.
        bsls::TimeInterval initialAbsoluteTime = timeSource.now();

        // Schedule a single-run event at a 30 second offset.
        scheduler.scheduleEvent(initialAbsoluteTime + 30,
                                bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                      &event1));

        // Schedule a 60s recurring event.
        scheduler.startClock(bsls::TimeInterval(60),
                             bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                   &event2));

        // Start the dispatcher thread.
        scheduler.start();

        // Advance the time by 35 seconds so that the first event will run.
        timeSource.advanceTime(bsls::TimeInterval(35));

        // Wait while verifying:
        makeSureTestObjectIsExecuted(event1, mT, 100);

        // Confirm that only the expected event has run.
        ASSERT(1 == event1.numExecuted());
        ASSERT(0 == event2.numExecuted());

        // Advance the time by another 30 seconds, so we will be at an offset
        // of 65 seconds, meaning the recurring event will run once.
        timeSource.advanceTime(bsls::TimeInterval(30));
        makeSureTestObjectIsExecuted(event2, mT, 100);
        ASSERT(1 == event1.numExecuted());
        ASSERT(1 == event2.numExecuted());

        // Now let the recurring event run exactly once more
        timeSource.advanceTime(bsls::TimeInterval(60));
        makeSureTestObjectIsExecuted(event2, mT, 100, 1);
        ASSERT(1 == event1.numExecuted());
        ASSERT(2 == event2.numExecuted());

        // Stop the scheduler and finish the test
        scheduler.stop();
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING CLOCKTYPE ACCESSOR
        //
        // Concern:
        //   That the 'clockType' accessor correctly return the clock type the
        //   object was constructed with.
        //
        // Plan:
        //   Run all c'tors with all values of 'clockType', and verify that
        //   the value returned by the 'clockType' accessor is as expected.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING CLOCKTYPE ACCESSOR\n"
                             "==========================\n";

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_24;
        using namespace bdlf::PlaceHolders;

        bdlmt::TimerEventScheduler::Dispatcher dispatcher =
                                 bdlf::BindUtil::bind(&dispatcherFunction, _1);

        bslma::TestAllocator ta(veryVeryVerbose);

        const bsls::SystemClockType::Enum realTime =
                                            bsls::SystemClockType::e_REALTIME;
        const bsls::SystemClockType::Enum monotonic =
                                            bsls::SystemClockType::e_MONOTONIC;

        ASSERT(realTime != monotonic);

        if (verbose) cout << "Default c'tor\n";
        {
            Obj x(&ta);    const Obj& X = x;

            ASSERT(realTime == X.clockType());
        }

        if (verbose) cout << "Explicit clock type, no dispatcher\n";
        {
            Obj x(realTime,  &ta);    const Obj& X = x;
            Obj y(monotonic, &ta);    const Obj& Y = y;

            ASSERT(realTime  == X.clockType());
            ASSERT(monotonic == Y.clockType());
        }

        if (verbose) cout << "Dispatcher, clock type defaults\n";
        {
            Obj x(dispatcher, &ta);    const Obj& X = x;

            ASSERT(realTime == X.clockType());
        }

        if (verbose) cout << "Dispatcher, explicit clock type\n";
        {
            Obj x(dispatcher, realTime,  &ta);    const Obj& X = x;
            Obj y(dispatcher, monotonic, &ta);    const Obj& Y = y;

            ASSERT(realTime  == X.clockType());
            ASSERT(monotonic == Y.clockType());
        }

        if (verbose) cout << "num events, clocks\n";
        {
            Obj x(0, 0, &ta);    const Obj& X = x;

            ASSERT(realTime == X.clockType());
        }

        if (verbose) cout << "Num events, clocks, explicit clock type,"
                                                            " no dispatcher\n";
        {
            Obj x(0, 0, realTime,  &ta);    const Obj& X = x;
            Obj y(0, 0, monotonic, &ta);    const Obj& Y = y;

            ASSERT(realTime  == X.clockType());
            ASSERT(monotonic == Y.clockType());
        }

        if (verbose) cout << "Num events, clocks, dispatcher, clock type"
                                                                 " defaults\n";
        {
            Obj x(0, 0, dispatcher, &ta);    const Obj& X = x;

            ASSERT(realTime == X.clockType());
        }

        if (verbose) cout << "Num events, clocks, explicit clock type,"
                                                               " dispatcher\n";
        {
            Obj x(0, 0, dispatcher, realTime,  &ta);    const Obj& X = x;
            Obj y(0, 0, dispatcher, monotonic, &ta);    const Obj& Y = y;

            ASSERT(realTime  == X.clockType());
            ASSERT(monotonic == Y.clockType());
        }
      } break;
      case 24: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(nE, nC, disp, cT, bA = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
        //   specified dispatcher, specified clock, and provides capacity
        //   guarantees, correctly schedules events.
        //
        // Concerns:
        //: 1 The creator with the specified dispatcher, specified clock, and
        //:   capacity guarantees correctly initializes the object.
        //
        // Plan:
        //: 1 Define a dispatcher that simply executes the specified
        //    functor.  Create a scheduler using this dispatcher and the
        //    monotonic clock, schedule an event and make sure that it is
        //    executed by the specified dispatcher.
        //
        // Testing:
        //   bdlmt::TimerEventScheduler(nE, nC, disp, cT, bA = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(nE, nC, disp, cT, bA = 0)'" << endl
                          << "================================="
                          << "===========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_24;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::TimerEventScheduler::Dispatcher dispatcher =
          bdlf::BindUtil::bind(&dispatcherFunction, _1);

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(4, 4, dispatcher, bsls::SystemClockType::e_MONOTONIC, &ta);
        x.start();

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 23: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(nE, nC, disp, bA = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
        //   specified dispatcher, realtime clock, and provides capacity
        //   guarantees, correctly schedules events.
        //
        // Concerns:
        //: 1 The creator with the specified dispatcher, realtime clock, and
        //:   capacity guarantees correctly initializes the object.
        //
        // Plan:
        //: 1 Define a dispatcher that simply executes the specified
        //    functor.  Create a scheduler using this dispatcher and the
        //    realtime clock, schedule an event and make sure that it is
        //    executed by the specified dispatcher.
        //
        // Testing:
        //   bdlmt::TimerEventScheduler(nE, nC, disp, bA = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(nE, nC, disp, bA = 0)'" << endl
                          << "================================="
                          << "=======================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_23;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::TimerEventScheduler::Dispatcher dispatcher =
          bdlf::BindUtil::bind(&dispatcherFunction, _1);

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(4, 4, dispatcher, &ta); x.start();

        x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                        bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                               &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );
      } break;
      case 22: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(nE, nC, cT, bA = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
        //   default dispatcher, specified clock, and provides capacity
        //   guarantees, correctly schedules events.
        //
        // Concerns:
        //: 1 The creator with the default dispatcher, specified clock, and
        //:   capacity guarantees correctly initializes the object.
        //
        // Plan:
        //: 1 Create a scheduler using the default dispatcher and the specified
        //    clock, schedule an event and make sure that it is executed by the
        //    default dispatcher.
        //
        // Testing:
        //   bdlmt::TimerEventScheduler(nE, nC, cT, bA = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(nE, nC, cT, bA = 0)'" << endl
                          << "================================="
                          << "=====================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_22;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(4, 4, bsls::SystemClockType::e_MONOTONIC, &ta); x.start();

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 21: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(nE, nC, bA = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
        //   default dispatcher, realtime clock, and provides capacity
        //   guarantees, correctly schedules events.
        //
        // Concerns:
        //: 1 The creator with the default dispatcher and realtime clock
        //:   correctly initializes the object.
        //
        // Plan:
        //: 1 Create a scheduler using the default dispatcher and the realtime
        //    clock, schedule an event and make sure that it is executed by the
        //    default dispatcher.
        //
        // Testing:
        //   bdlmt::TimerEventScheduler(nE, nC, bA = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(nE, nC, bA = 0)'" << endl
                          << "================================="
                          << "=================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_21;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(4, 4, &ta); x.start();

        x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 20: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(disp, clockType, alloc = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
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
        //   bdlmt::TimerEventScheduler(disp, clockType, alloc = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(disp, clockType, alloc = 0)'" << endl
                          << "================================="
                          << "=============================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_20;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::TimerEventScheduler::Dispatcher dispatcher =
          bdlf::BindUtil::bind(&dispatcherFunction, _1);

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(dispatcher, bsls::SystemClockType::e_MONOTONIC, &ta); x.start();

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 19: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(clockType, allocator = 0)':
        //   Verify this 'bdlmt::TimerEventScheduler' creator, which uses the
        //   default dispatcher and specified clock, correctly schedules
        //   events.
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
        //   bdlmt::TimerEventScheduler(clockType, allocator = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(clockType, allocator = 0)'" << endl
                          << "================================="
                          << "===========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_19;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        TestClass1 testObj;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(bsls::SystemClockType::e_MONOTONIC, &ta); x.start();

        x.scheduleEvent(
                 bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC) + T,
                 bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 18: {
        // -----------------------------------------------------------------
        // Testing cancelEvent
        //
        // Concerns:
        //   Verify that 'cancelEvent' does the right thing when an event
        //   tries to cancel itself, and that pending events after the
        //   event being canceled are unaffected.
        //
        // Plan:
        //   Set up a situation where many events are pending, then have an
        //   event cancel itself, and show that the next subsequent event
        //   does not disappear.
        // -----------------------------------------------------------------

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_18;

        if (verbose) {
            cout << "cancelEvent test\n"
                    "================\n";
        }

        Obj mX;
        Func::s_scheduler = &mX;
        Func::s_finished.storeRelease(false);

        bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();

        // Schedule 10 events to run in order and immediately
        for (int i = 0; i < 10; ++i) {
            Obj::Handle handle =
                     mX.scheduleEvent(now - bsls::TimeInterval((10 - i) * 0.1),
                                      Func(i));
            Func::s_handles.push_back(handle);
        }

        Func::s_kamikaze = 4;
        Func::s_final    = 9;

        mX.start();

        // make sure the events all run
        while(!Func::s_finished.loadAcquire()) {
            bslmt::ThreadUtil::yield();
            sleepUntilMs(10);
        }

        mX.stop();

        ASSERTV(Func::s_indexes.size(), Func::s_indexes.size() == 10);
        for (int i = 0; i < static_cast<int>(Func::s_indexes.size()); ++i) {
            ASSERTV(i, Func::s_indexes[i], i == Func::s_indexes[i]);
        }
      } break;
      case 17: {
        // -----------------------------------------------------------------
        // Testing cancelAllEvents
        //
        // Concerns:
        //   There is a bug in 'cancelAllEvents' regarding the updating of
        //   'd_numEvents' that previous testing was not exposing.
        //
        // Plan:
        //   Get several events on the pending events vector, then
        //   call 'cancelAllEvents()', keeping track of the # of outstanding
        //   events, and expose the discrepancy.
        // -----------------------------------------------------------------

        if (verbose) {
            cout << "cancelAllEvents & numEvents test\n"
                    "================================\n";
        }

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_17;

        Obj mX;
        Func::s_scheduler = &mX;
        Func::s_numEvents = 0;

        bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();

        for (int i = 0; i < 7; ++i) {
            ++Func::s_numEvents;
            mX.scheduleEvent(now, Func());
        }
        mX.start();
        bslmt::ThreadUtil::yield();
        sleepUntilMs(100 * 1000 / 1000);
        mX.cancelAllEvents();

        sleepUntilMs(500 * 1000 / 1000);
        mX.stop();
      } break;
      case 16: {
        // -----------------------------------------------------------------
        // TESTING PASSING ATTRIBUTES TO START:
        //
        // Concerns:
        //   That we are able to specify attributes of dispatcher thread by
        //   passing them to 'start()'.
        //
        // Plan:
        //   Configure the stack size to be much bigger than the default,
        //   then verify that it really is bigger.  Note: veryVeryVerbose
        //   will deliberately make this test crash to find out how big the
        //   default stack size is (just displaying attr.stackSize() just
        //   shows 0 if you let stack size default on Solaris).
        //
        //   Also verify that DETACHED attribute is ignored by setting the
        //   attributes passed to be DETACHED and then doing a 'stop()',
        //   which will join with the dispatcher thread.
        //
        // Testing:
        //   int start(const bslmt::ThreadAttributes& threadAttributes);
        // -----------------------------------------------------------------

        if (verbose) {
            cout << "Attributes test\n";
        }

        namespace TC = TIMER_EVENT_SCHEDULER_TEST_CASE_16;

        bslmt::ThreadAttributes attr;

        if (!veryVeryVerbose) {
            attr.setStackSize(TC::k_STACK_SIZE_IN_BYTES);
        }
        attr.setDetachedState(bslmt::ThreadAttributes::e_CREATE_DETACHED);

        if (verbose) {
            cout << "StackSize: " << attr.stackSize() << endl;
        }

        TC::Recurser::s_finished = false;

        bdlmt::TimerEventScheduler scheduler;
        int sts = scheduler.start(attr);
        ASSERT(!sts);

        scheduler.scheduleEvent(bsls::SystemTime::nowRealtimeClock() +
                                                      bsls::TimeInterval(0.05),
                                TC::Recurser());
        sleepUntilMs(250 * 1000 / 1000);

        if (!TC::Recurser::s_finished) {
            BSLS_ASSERT_INVOKE("test not finished in time");
        }

        scheduler.stop();

        ASSERT(TC::Recurser::s_finished);
      } break;
      case 15: {
        // -----------------------------------------------------------------
        // TESTING ACCUMULATED TIME LAG
        //
        // Concerns:
        //   That if a clock is scheduled that taking longer than the clock
        //   frequency, that it will execute pretty much continuously, and
        //   that scheduled events will still get to happen.
        //
        // Plan:
        //   We have two functors, 'Slowfunctor' and 'Fastfunctor', the
        //   first of which takes 0.1 seconds, the second of which runs
        //   instantaneously.  We schedule 'Slowfunctor' as a clock
        //   occurring every 0.5 seconds, and schedule 'Fastfunctor' to
        //   happen at 2 specific times.  We sleep 4 seconds, and then
        //   observe that both functors happened roughly when they were
        //   supposed to.
        //
        if (verbose) cout <<"\nTesting accumulated time lag, with events\n"
                         "===========================================\n";

        // reusing
        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_14;

        int ii;
        enum { MAX_LOOP = 4 };
        for (ii = 0; ii <= MAX_LOOP; ++ii) {
            bdlmt::TimerEventScheduler scheduler(&ta);
            scheduler.start();
            Slowfunctor sf;
            Fastfunctor ff;

            const double CLOCKTIME1 = sf.SLEEP_SECONDS / 2;

            scheduler.startClock(bsls::TimeInterval(CLOCKTIME1),
                                 bdlf::MemFnUtil::memFn(&Slowfunctor::callback,
                                                        &sf));
            bsls::TimeInterval afterStarted =
                                          bsls::SystemTime::nowRealtimeClock();

            scheduler.scheduleEvent(
                          afterStarted + bsls::TimeInterval(2.0),
                          bdlf::MemFnUtil::memFn(&Fastfunctor::callback, &ff));

            scheduler.scheduleEvent(
                          afterStarted + bsls::TimeInterval(3.0),
                          bdlf::MemFnUtil::memFn(&Fastfunctor::callback, &ff));

            sleepUntilMs(4 * 1000 * 1000 / 1000);
                            // wait 40 time execution time of sf.callback().

            scheduler.stop();

            sleepUntilMs(2 * 1000 * 1000 / 1000);
                    // wait until events can run, verify that clock queue
                    // stayed small

            int slowfunctorSize = static_cast<int>(sf.timeList().size());

            ASSERT(!(slowfunctorSize & 1)); // should not be odd element
            slowfunctorSize &= ~1;          // ignore odd element if there

            if (verbose) { P_(slowfunctorSize); P(ff.timeList().size()); }
            ASSERT(ii < MAX_LOOP || slowfunctorSize >= 35*2);
            ASSERT(ii < MAX_LOOP || slowfunctorSize <= 44*2);
            if (slowfunctorSize < 35*2 || slowfunctorSize > 44*2) {
                if (verbose || MAX_LOOP == ii) { P_(L_); P(slowfunctorSize); }
                continue;    // start over
            }
            ASSERT(ii < MAX_LOOP || 2 == ff.timeList().size());
            if (2 != ff.timeList().size()) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_); P(ff.timeList().size());
                }
            }

            Slowfunctor::dateTimeList::iterator sfit = sf.timeList().begin();
            double startTime = *sfit;
            if (verbose) {
                P(afterStarted.totalSecondsAsDouble() - startTime);
            }

            // go through even elements - they should be right on time Note
            // this has unavoidable intermittent failures as the sleep function
            // may go over by as much as 2 seconds.
            double prevTime = startTime;
            const double sTolerance = 0.03;
            ++sfit, ++sfit;
            bool startOver = false;
            for (int i = 2; i+2 <= slowfunctorSize; ++sfit, ++sfit, i += 2) {
                double offBy = *sfit - Slowfunctor::SLEEP_SECONDS - prevTime;
                if (veryVerbose) { P_(i); P_(offBy); P(sTolerance); }
                ASSERT(ii < MAX_LOOP || sTolerance > abs(offBy));
                if (sTolerance <= abs(offBy)) {
                    if (verbose || MAX_LOOP == ii) {
                        P_(L_); P_(i); P_(sTolerance); P(offBy);
                    }
                    startOver = true;
                }
                prevTime = *sfit;
            }
            if (startOver) continue;

#if 0
            // This has been commented out because it is really inappropriate
            // to be testing the accuracy of the 'microSleep' method in this
            // component -- especially since it can be really unreasonably slow
            // on a heavily loaded machine (i.e., during the nighttime runs).
            // Testing of that routine has been moved to
            // 'bslmt_threadutil.t.cpp'.

            // go through odd elements - they should be SLEEP_SECONDS later
            sfit = sf.timeList().begin();
            ++sfit;
            for (int i = 1;  i+2 <= slowfunctorSize;  ++sfit, ++sfit, i += 2) {
                double diff = *sfit - start_time;
                double offBy = (i/2 + 1)*sf.SLEEP_SECONDS - diff;
                if (veryVerbose) { P_(L_); P_(i); P_(offBy);
                                   P(sf.tolerance(i/2)); }
                ASSERTV(i, diff, offBy, sf.tolerance(i/2),
                                             sf.tolerance(i/2) > abs(offBy));
            }
#endif

            Fastfunctor::dateTimeList::iterator ffit = ff.timeList().begin();

            double ffdiff = *ffit - afterStarted.totalSecondsAsDouble();
            double ffoffBy = ffdiff - 2;
            if (veryVerbose) { P_(ffdiff); P_(ffoffBy); P(ff.TOLERANCE); }
            ASSERT(ii < MAX_LOOP || ff.TOLERANCE > abs(ffoffBy));
            if (ff.TOLERANCE <= abs(ffoffBy)) {
                if (verbose || MAX_LOOP == ii) {
                    P_(L_); P_(ffdiff); P(ffoffBy);
                }
                continue;
            }

            ++ffit;
            ffdiff = *ffit - afterStarted.totalSecondsAsDouble();
            ffoffBy = ffdiff - 3;
            if (veryVerbose) { P_(ffdiff); P_(ffoffBy); P(ff.TOLERANCE); }
            ASSERT(ii < MAX_LOOP || ff.TOLERANCE > abs(ffoffBy));
            if (ff.TOLERANCE <= abs(ffoffBy)) {
                if (!veryVerbose && MAX_LOOP == ii) {
                    P_(L_); P_(ffdiff); P_(ffoffBy); P(ff.TOLERANCE);
                }
                continue;
            }

            break;
        }
        ASSERT(ii <= MAX_LOOP);
        if (verbose) { P_(L_); P(ii); }
      } break;
      case 14: {
        // -----------------------------------------------------------------
        // TESTING REGULARITY OF CLOCK EVENTS
        //
        // Concerns:
        //   That clock events happen when scheduled.
        //
        // Plan:
        //   Schedule a clocked event that will take less time than the
        //   period of the clock to execute, and verify that it happens
        //   regularly with a fixed accuracy, without any creep or
        //   accumulated lag.
        //
        //   <<NOTE>> -- due to unreliability of
        //   'bslmt::Condition::timedWait', this test is going to fail a
        //   certain percentage of the time.
        // -----------------------------------------------------------------

        if (verbose) cout << "\nTesting accumulated time lag\n"
                               "============================\n";

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_14;

        bslma::TestAllocator ta;

        int ii;
        enum { MAX_LOOP = 4 };
        for (ii = 0; ii <= MAX_LOOP; ++ii) {
            bdlmt::TimerEventScheduler scheduler(&ta);
            scheduler.start();
            Slowfunctor sf;
            const double CLOCKTIME1 = 0.3;
            const double TOLERANCE_BEHIND = 0.25;
            const double TOLERANCE_AHEAD  = 0.03;

            scheduler.startClock(
                          bsls::TimeInterval(CLOCKTIME1),
                          bdlf::MemFnUtil::memFn(&Slowfunctor::callback, &sf));

            sleepUntilMs(6 * 1000 * 1000 / 1000);
                                                    // wait 20 clock cycles

            scheduler.stop();

            sleepUntilMs(3 * 1000 * 1000 / 1000);
                    // let running tasks finish, test that clock queue did not
                    // get large

            int slowfunctorSize = static_cast<int>(sf.timeList().size());

            if (verbose) { P(slowfunctorSize); }

            ASSERT(!(slowfunctorSize & 1)); // should not be odd element
            slowfunctorSize &= ~1;          // ignore odd element if there

            ASSERT(ii < MAX_LOOP || slowfunctorSize >= 18*2);
            ASSERT(ii < MAX_LOOP || slowfunctorSize <= 22*2);
            if (slowfunctorSize < 18*2 || slowfunctorSize > 22*2) {
                if (verbose || ii == MAX_LOOP) { P_(L_); P(slowfunctorSize); }
                continue;
            }

            Slowfunctor::dateTimeList::iterator it = sf.timeList().begin();
            double start_time = *it;

            // go through even elements - they should be right on time
            bool startOver = false;
            for (int i = 0;  i+2 <= slowfunctorSize;  ++it, ++it, i += 2) {
                double diff = *it - start_time;
                double offBy = diff - (i/2)*CLOCKTIME1;
                if (veryVerbose) { P_(i); P(offBy); }
                ASSERT(ii < MAX_LOOP || TOLERANCE_BEHIND > offBy);
                ASSERT(ii < MAX_LOOP || TOLERANCE_AHEAD > -offBy);
                if (TOLERANCE_BEHIND <= offBy || TOLERANCE_AHEAD <= -offBy) {
                    if (verbose || ii == MAX_LOOP) {
                        P_(L_); P_(i); P_(diff);
                        P_(TOLERANCE_BEHIND); P(TOLERANCE_AHEAD);
                        P(offBy);
                    }
                    startOver = true;
                }
            }
            if (startOver) continue;

            break;
        }
        ASSERT(ii <= MAX_LOOP);
        if (verbose) { P_(L_); P(ii); }
// TBD
#if 0
        // This has been commented out because it is really inappropriate to
        // be testing the accuracy of the 'microSleep' method in this component
        // -- especially since it can be really unreasonably slow on a heavily
        // loaded machine (i.e., during the nighttime runs).  Testing of that
        // routine has been moved to 'bslmt_threadutil.t.cpp'.

        // go through odd elements - they should be SLEEP_SECONDS later
        it = sf.timeList().begin();
        ++it;
        for (int i = 1;  i+2 <= slowfunctorSize;  ++it, ++it, i += 2) {
            double diff = *it - start_time;
            double offBy = (i/2)*CLOCKTIME1 + sf.SLEEP_SECONDS - diff;
            if (veryVerbose) { P_(i); P(offBy); }
            ASSERTV(i, diff, offBy, TOLERANCE, TOLERANCE > abs(offBy));
        }
#endif
      } break;
      case 13: {
        // -----------------------------------------------------------------
        // TESTING CONCERN: Maximum number of concurrently scheduled events
        // and clocks.
        //
        // Concerns:
        //   Given 'numEvents' and 'numClocks' (where
        //   'numEvents <= 2**24 - 1' and 'numClocks <= 2**24 - 1'), ensure
        //   that *at least* 'numEvents' and 'numClocks' may be concurrently
        //   scheduled.
        //
        // Plan:
        //   Define two values 'numEvents' and 'numClocks', then construct a
        //   timer event scheduler using these values.  In order to prevent
        //   any scheduled events from firing, leave the scheduler in the
        //   "stopped" state.  Schedule events from multiple threads and
        //   ensure *at least* 'numEvents' may be concurrently scheduled.
        //   Cancel all events, then again schedule events from multiple
        //   threads and ensure the maximum number of events may again be
        //   scheduled.  Repeat for clocks.
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MAX CONCURRENT EVENTS & CLOCKS"
                          << endl
                          << "======================================"
                          << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_13;

        const int TEST_DATA[] = {
            (1 <<  3),     // some small value
            (1 << 17) - 2  // the default "minimum guarantee"
        };

        const int NUM_TESTS = sizeof TEST_DATA / sizeof *TEST_DATA;
        const int NUM_THREADS = 4;

        for (int testIter = 0; testIter < NUM_TESTS; ++testIter) {
            const int NUM_OBJECTS  = TEST_DATA[testIter];

            Obj mX(NUM_OBJECTS, NUM_OBJECTS);

            for (int i = 0; i < 2; ++i) {
                bsls::AtomicInt    numAdded(0);
                bslmt::Barrier     barrier(NUM_THREADS + 1);
                bslmt::ThreadGroup threadGroup;

                threadGroup.addThreads(
                                     bdlf::BindUtil::bind(&scheduleEvent,
                                                          &mX,
                                                          &numAdded,
                                                          (bsls::AtomicInt *)0,
                                                          &barrier),
                                     NUM_THREADS);
                barrier.wait();
                threadGroup.joinAll();

                ASSERT(numAdded       >= NUM_OBJECTS);
                ASSERT(mX.numEvents() >= NUM_OBJECTS);

                numAdded = 0;

                threadGroup.addThreads(
                                     bdlf::BindUtil::bind(&startClock,
                                                          &mX,
                                                          &numAdded,
                                                          (bsls::AtomicInt *)0,
                                                          &barrier),
                                     NUM_THREADS);
                barrier.wait();
                threadGroup.joinAll();

                ASSERT(numAdded       >= NUM_OBJECTS);
                ASSERT(mX.numClocks() >= NUM_OBJECTS);

                mX.cancelAllEvents();
                mX.cancelAllClocks();
                ASSERT(0 == mX.numEvents());
                ASSERT(0 == mX.numClocks());
            }
        }
      } break;
      case 12: {
        // -----------------------------------------------------------------
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
        //   Repeat above, but this time use an arbitrary key.
        //
        // Testing:
        //   int rescheduleEvent(handle, key, newTime);
        //   int rescheduleEvent(handle, newTime);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'rescheduleEvent'" << endl
                          << "=========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_12;
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

          TestClass1 testObj;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
          Handle h = x.scheduleEvent(
                      now + T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

          myMicroSleep(T, 0);
          bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
          if (elapsed < T2) {
              nExec = testObj.numExecuted();
              ASSERTV(nExec, 0 == nExec);
              ASSERT( 0 == x.rescheduleEvent(h, now + T4, true) );
              myMicroSleep(T6, 0);
              makeSureTestObjectIsExecuted(testObj, mT, 100);
              nExec = testObj.numExecuted();
              ASSERTV(nExec, 1 == nExec);
              ASSERT( 0 != x.rescheduleEvent(h, now + T8, true) );
          }
        }

        {
          // Repeat above, but this time use an arbitrary key.

          typedef Obj::EventKey Key;
          const Key key(123);

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T  = 1 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T4(4 * DECI_SEC);
          const int T6 = 6 * DECI_SEC_IN_MICRO_SEC;
          const bsls::TimeInterval T8(8 * DECI_SEC);

          TestClass1 testObj;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          x.start();

          bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
          Handle h = x.scheduleEvent(
                       now + T2,
                       bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj),
                       key);

          myMicroSleep(T, 0);
          bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
          if (elapsed < T2) {
              nExec = testObj.numExecuted();
              ASSERTV(nExec, 0 == nExec);
              ASSERT( 0 == x.rescheduleEvent(h, key, now + T4, true) );
              myMicroSleep(T6, 0);
              makeSureTestObjectIsExecuted(testObj, mT, 100);
              nExec = testObj.numExecuted();
              ASSERTV(nExec, 1 == nExec);
              ASSERT( 0 != x.rescheduleEvent(h, key, now + T8, true) );
          }
        }

      } break;
      case 11: {
        // -----------------------------------------------------------------
        // TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL:
        //   Verify concurrent scheduling and cancelling-All.
        //
        // Concerns:
        //   That concurrent scheduling and cancelling-all of clocks and
        //   events are safe.
        //
        // Plan:
        //   Create several threads.  Half of the threads execute 'h =
        //   scheduler.scheduleEvent(..), scheduler.cancelAllEvent()' in a
        //   loop and other half execute 'h = scheduler.startClock(..);
        //   scheduler.cancelAllClocks(h)' in a loop.  Finally join all the
        //   threads and verify that no callback has been invoked and that
        //   the scheduler is still in good state (this is done by
        //   scheduling an event and verifying that it is executed as
        //   expected).
        //
        // Testing:
        //   Concurrent scheduling and cancelling-All.
        // -----------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING CONCURRENT SCHEDULING AND CANCELLING-ALL" << endl
                 << "================================================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_11;
        const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
        ASSERT( 0 == x.start() );

        executeInParallel(NUM_THREADS, workerThread11);
        myMicroSleep(T10, 0);

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
        x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                        bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                               &testObj));
        myMicroSleep(T4, 0);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 10: {
        // -----------------------------------------------------------------
        // TESTING CONCURRENT SCHEDULING AND CANCELLING:
        //   Verify concurrent scheduling and cancelling.
        //
        // Concerns:
        //   That concurrent scheduling and cancelling of clocks and
        //   events are safe.
        //
        // Plan:
        //   Create several threads.  Half of the threads execute 'h =
        //   scheduler.scheduleEvent(..), scheduler.cancelEvent(h)' in a
        //   loop and other half execute 'h = scheduler.startClock(..);
        //   scheduler.cancelClock(h)' in a loop.  Finally join all the
        //   threads and verify that no callback has been invoked and that
        //   the scheduler is still in good state (this is done by
        //   scheduling an event and verifying that it is executed as
        //   expected).
        //
        // Testing:
        //   Concurrent scheduling and cancelling.
        // -----------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING CONCURRENT SCHEDULING AND CANCELLING" << endl
                 << "============================================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_10;

        const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
        ASSERT( 0 == x.start() );

        executeInParallel(NUM_THREADS, workerThread10);
        myMicroSleep(T10, 0);

        if (!testTimingFailure) {
            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERTV(i, testObj[i].numExecuted(),
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
        x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                        bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                               &testObj));
        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 9: {
        // -----------------------------------------------------------------
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
        //   That invoking 'start' concurrently with 'stop' does not adversely
        //   affect processing of events
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
        //   Invoke 'startStopConcurrencyTest' (see function-level doc for
        //   plan). This tests for a race condition and doesn't catch it
        //   every time, so run it a few times.
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
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'start' and 'stop'" << endl
                          << "==========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_9;
        {
            // Invoke 'start' multiple times and verify that the scheduler is
            // still in good state (this is done by scheduling an event and
            // verifying that it is executed as expected).

            const int mT = DECI_SEC_IN_MICRO_SEC / 10;  // 10ms
            const bsls::TimeInterval T(1 * DECI_SEC);
            const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            ASSERT(0 == x.start());
            ASSERT(0 == x.start());

            TestClass1 testObj;
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            makeSureTestObjectIsExecuted(testObj, mT, 100);
            ASSERT(1 == testObj.numExecuted());
        }

        {
            // Invoke 'stop' multiple times and verify that the scheduler is
            // still in good state (this is done by starting the scheduler,
            // scheduling an event and verifying that it is executed as
            // expected).

            const int mT = DECI_SEC_IN_MICRO_SEC / 10;  // 10ms
            const bsls::TimeInterval T(1 * DECI_SEC);
            const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            x.stop();
            x.stop();
            ASSERT(0 == x.start());
            x.stop();
            x.stop();
            ASSERT(0 == x.start());

            TestClass1 testObj;
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            makeSureTestObjectIsExecuted(testObj, mT, 100);
            ASSERT(1 == testObj.numExecuted());
        }

        if (veryVerbose) {
            cout << "Start/stop concurrency test\n";
        }
        enum { NUM_START_STOP_TESTS = 3 };

        for (int i = 0; i < NUM_START_STOP_TESTS; ++i) {
            startStopConcurrencyTest();
        }

        {
            // Restart scheduler after stopping and make sure that the
            // scheduler is still in good state (this is done by starting the
            // scheduler, scheduling an event and verifying that it is executed
            // as expected).  Repeat this several time.

            const int mT = DECI_SEC_IN_MICRO_SEC / 10;  // 10ms
            const bsls::TimeInterval T(1 * DECI_SEC);
            const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            TestClass1 testObj;

            ASSERT(0 == x.start());
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            makeSureTestObjectIsExecuted(testObj, mT, 100);
            ASSERT(1 == testObj.numExecuted());
            x.stop();

            nExec = testObj.numExecuted();
            ASSERT(0 == x.start());
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
            ASSERT(2 == testObj.numExecuted());
            x.stop();

            nExec = testObj.numExecuted();
            ASSERT(0 == x.start());
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            makeSureTestObjectIsExecuted(testObj, mT, 100, nExec);
            ASSERT(3 == testObj.numExecuted());
        }

        {
            // Start the scheduler, sleep for a while to ensure that scheduler
            // gets blocked, invoke 'stop' and verify that scheduler is
            // actually stopped.

            const bsls::TimeInterval T(1 * DECI_SEC);
            const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            ASSERT(0 == x.start());
            myMicroSleep(T3, 0);
            x.stop();

            // Make sure that scheduler is stopped.
            TestClass1 testObj;
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T3, 0);
            ASSERT(0 == testObj.numExecuted());
        }

        {
            // Start the scheduler, schedule an event, sleep for a while to
            // ensure that scheduler gets blocked, invoke 'stop' before the
            // event is expired and verify the state.

            const bsls::TimeInterval T(1 * DECI_SEC);
            const bsls::TimeInterval T4(4 * DECI_SEC);

            const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;
            const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            TestClass1 testObj;
            ASSERT(0 == x.start());
            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            x.scheduleEvent(
                      now + T4,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T2, 0);
            x.stop();

            // Make sure that scheduler is stopped, but allow for the
            // possibility that myMicroSleep overslept and apply defensive
            // techniques.
            bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
            if (elapsed < T4) {
                myMicroSleep(T3, 0);
                ASSERT(0 == testObj.numExecuted());
            }

            // Make *really* sure that scheduler is stopped.
            int numExecutedUnchanged = testObj.numExecuted();
            x.scheduleEvent(
                      bsls::SystemTime::nowRealtimeClock() + T,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));
            myMicroSleep(T3, 0);
            ASSERT(numExecutedUnchanged == testObj.numExecuted());
        }
      } break;
      case 8: {
        // -----------------------------------------------------------------
        // TESTING 'bdlmt::TimerEventScheduler(dispatcher, allocator = 0)':
        //   Verifying 'bdlmt::TimerEventScheduler(dispatcher, allocator = 0)'.
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
        //   bdlmt::TimerEventScheduler(dispatcher, allocator = 0);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdlmt::TimerEventScheduler"
                          << "(dispatcher, allocator = 0)'" << endl
                          << "================================="
                          << "============================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_8;
        using namespace bdlf::PlaceHolders;

        const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
        const bsls::TimeInterval T(1 * DECI_SEC);
        const int T2 = 2 * DECI_SEC_IN_MICRO_SEC;

        bdlmt::TimerEventScheduler::Dispatcher dispatcher =
          bdlf::BindUtil::bind(&dispatcherFunction, _1);

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(dispatcher, &ta); x.start();

        TestClass1 testObj;

        x.scheduleEvent(bsls::SystemTime::nowRealtimeClock() + T,
                        bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                               &testObj));

        myMicroSleep(T2, 0);
        makeSureTestObjectIsExecuted(testObj, mT, 100);
        ASSERT( 1 == testObj.numExecuted() );

      } break;
      case 7: {
        // -----------------------------------------------------------------
        // TESTING METHODS INVOCATIONS FROM THE DISPATCHER THREAD:
        //   Verify various methods invocations from the dispatcher thread.
        //
        // Concerns:
        //   That it is possible to schedule events and clocks from the
        //   dispatcher thread.
        //
        //   That when 'cancelEvent' is invoked from the dispatcher thread
        //   then the wait argument is ignored.
        //
        //   That when 'cancelAllEvent' is invoked from the dispatcher
        //   thread then the wait argument is ignored.
        //
        //   That when 'cancelClock' is invoked from the dispatcher thread
        //   then the wait argument is ignored.
        //
        //   That when 'cancelAllClocks' is invoked from the dispatcher
        //   thread then the wait argument is ignored.
        //
        //   That DRQS 7272737 is solved.
        //
        // Plan:
        //   Define T, T2, T3, T4 .. as time intervals such that T2 = T * 2,
        //   T3 = T * 3,  T4 = T * 4,  and so on.
        //
        //   Schedule an event at T2 that itself schedules an event at
        //   T2+T=T3 and a clock with period T4, verify that callbacks are
        //   executed as expected.
        //
        //   Schedule an event e1 at time T such that it cancels itself with
        //   wait argument (this will fail), and an event e2 that will be
        //   pending when c1 will execute (this is done by waiting long
        //   enough before starting the scheduler).  Make sure no deadlock
        //   results.
        //
        //   Schedule an event e1 at time T such that it invokes
        //   'cancelAllEvents' with wait argument, and another event e2 at
        //   time T2 that will be pending when e1 will execute (this is done
        //   by waiting long enough before starting the scheduler).  Make
        //   sure no deadlock results of e1 waiting for e2 to complete (both
        //   are running in the dispatcher thread).
        //
        //   Schedule a clock c1 such that it cancels itself with wait
        //   argument (this will fail), and an event e2 that will be pending
        //   when c1 will execute (this is done by waiting long enough
        //   before starting the scheduler).  Make sure no deadlock results.
        //
        //   Schedule a clock c1 such that it invokes 'cancelAllClocks' with
        //   wait argument, and an event e2 that will be pending when c1
        //   will execute (this is done by waiting long enough before
        //   starting the scheduler).  Make sure no deadlock results.
        //
        // Testing:
        //   Methods invocations from the dispatcher thread.
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING METHODS INVOCATIONS FROM THE DISPATCHER "
                          << "THREAD\n"
                          << "================================================"
                          << "======\n";

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_7;

        typedef void (*Func)();

        const Func funcPtrs[] = { &test7_a, &test7_b, &test7_c, &test7_d,
                                  &test7_e, &test7_f };
        enum { NUM_THREADS = sizeof funcPtrs / sizeof funcPtrs[0] };

        bslmt::ThreadGroup tg(&testAllocator);

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == tg.addThread(funcPtrs[i]));
        }

        tg.joinAll();
      } break;
      case 6: {
        // -----------------------------------------------------------------
        // TESTING 'cancelAllClocks':
        //   Verifying 'cancelAllClocks'.
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
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelAllClocks'" << endl
                          << "=========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_6;

        typedef void (*Func)();

        const Func funcPtrs[] = { &test6_a, &test6_b, &test6_c };
        enum { NUM_THREADS = sizeof funcPtrs / sizeof funcPtrs[0] };

        bslmt::ThreadGroup tg(&testAllocator);

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == tg.addThread(funcPtrs[i]));
        }

        tg.joinAll();
      } break;
      case 5: {
        // -----------------------------------------------------------------
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
        //   Schedule a clock starting at T and an event at 3T.  Wait for the
        //   first execution of the clock to start, cancel it with 'wait'
        //   argument equal to false while it is running, and wait for the
        //   event to start.  Verify that the clock was only executed once (not
        //   a second time at 2T).
        //
        //   Schedule a clock starting at T.  Wait for the first execution of
        //   the clock to start, and cancel it with 'wait' argument equal to
        //   true while it is running.  Because this will block the main
        //   thread, another thread is used to unblock the clock callback after
        //   the clock's cancellation has completed.  Verify that the clock has
        //   been executed once, and schedule an event at 3T.  Wait for the
        //   event to start, and verify that the clock was not executed again.
        //
        // Testing:
        //   int cancelClock(Handle handle, bool wait=false);
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'cancelClock'" << endl
                          << "=====================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_5;

        if (veryVerbose) cout << "\t...schedule at T2/cancel at T/verify"
                              << " successfully cancelled." << endl;
        {
            // Schedule a clock starting at T2, cancel it at time T and verify
            // that it has been successfully cancelled.

            const int T = 1 * DECI_SEC_IN_MICRO_SEC;
            const bsls::TimeInterval T2(2 * DECI_SEC);
            const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;

            TestClass1 testObj;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);
            x.start();

            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            Handle h = x.startClock(
                      T2,
                      bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj));

            sleepUntilMs(T / 1000);
            bsls::TimeInterval elapsed =
                                    bsls::SystemTime::nowRealtimeClock() - now;
            if (elapsed < T2) {
                ASSERT(0 == x.cancelClock(h));
                myMicroSleep(T3, 0);
                ASSERT(0 == testObj.numExecuted());
            }
            x.stop();
        }

        if (veryVerbose) cout << "\t...schedule 2 clocks simultaneously"
                              << " pending." << endl;
        {
            // Schedule two clocks c1 and c2 starting at T and T2.  Let both be
            // simultaneously put onto the pending list (this is done by
            // sleeping enough time before starting the scheduler), cancel c2
            // before its callback is dispatched and verify the result.

            bsls::TimeInterval T(1 * DECI_SEC);
            const bsls::TimeInterval T2(2 * DECI_SEC);
            const int T3 = 3 * DECI_SEC_IN_MICRO_SEC;
            const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;
            const int T20 = 20 * DECI_SEC_IN_MICRO_SEC;
            TestClass1 testObj1(T10);
            TestClass1 testObj2;

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj x(&ta);

            double start = dnow();
            (void)x.startClock(
                     T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

            Handle h2 = x.startClock(
                     T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));
            myMicroSleep(T3, 0);
            x.start();
            myMicroSleep(T3, 0);
            ASSERT(0 == x.cancelClock(h2));
            bool cancelledInTime = dnow() - start < 0.99;
            myMicroSleep(T20, 0);
            if (cancelledInTime) {
                ASSERTV(testObj2.numExecuted(),
                        0 == testObj2.numExecuted());
            }
            x.stop();
        }

        if (veryVerbose) cout << "\t...clock starting at T/event at 3T"
                              << endl;
        {
            // Schedule a clock starting at T and an event at 3T.  Wait for the
            // first execution of the clock to start, cancel it with 'wait'
            // argument equal to false while it is running, and wait for the
            // event to start.  Verify that the clock was only executed once
            // (not a second time at 3T).

            bsls::TimeInterval T(0.01);  // 10ms
            bsls::TimeInterval T2(0.02);
            bsls::TimeInterval T3(0.03);

            bslmt::Barrier startBarrier1(2);
            bslmt::Barrier finishBarrier1(2);
            TestClass2     testObj1(&startBarrier1, &finishBarrier1);
            bslmt::Barrier startBarrier2(2);
            TestClass2     testObj2(&startBarrier2, 0);

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj                  x(&ta);

            Handle h = x.startClock(
                     T,
                     bdlf::MemFnUtil::memFn(&TestClass2::callback, &testObj1));
            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            (void)x.scheduleEvent(now + T3,
                                  bdlf::MemFnUtil::memFn(&TestClass2::callback,
                                                         &testObj2));
            x.start();
            startBarrier1.wait();
            ASSERT(0 == x.cancelClock(h));
            finishBarrier1.wait();
            int numExecuted = testObj1.numExecuted();
            ASSERTV(numExecuted, 1 == numExecuted);
            startBarrier2.wait();
            numExecuted = testObj1.numExecuted();
            ASSERTV(numExecuted, 1 == numExecuted);
            x.stop();
        }

        if (veryVerbose) cout << "\t...schedule using different threads."
                              << endl;
        {
            // Schedule a clock starting at T.  Wait for the first execution of
            // the clock to start, and cancel it with 'wait' argument equal to
            // true while it is running.  Because this will block the main
            // thread, another thread is used to unblock the clock callback
            // after the clock's cancellation has completed.  Verify that the
            // clock has been executed once, and schedule an event at 3T.  Wait
            // for the event to start, and verify that the clock was not
            // executed again.  (Note that we have to be careful not to
            // schedule the event until after the 'cancelClock' call returns.
            // Otherwise, a deadlock can occur.)

            bsls::TimeInterval T(0.01);  // 10ms
            bsls::TimeInterval T2(0.02);
            bsls::TimeInterval T3(0.03);

            bslmt::Barrier startBarrier1(3);
            bslmt::Barrier finishBarrier1(2);
            TestClass2     testObj1(&startBarrier1, &finishBarrier1);
            bslmt::Barrier startBarrier2(2);
            TestClass2     testObj2(&startBarrier2, 0);

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj                  x(&ta);

            bslmt::ThreadUtil::Handle unblockThreadHandle;
            ASSERT(0 == bslmt::ThreadUtil::createWithAllocator(
                                &unblockThreadHandle,
                                Unblock(&x, &startBarrier1, &finishBarrier1),
                                &ta));

            Handle h = x.startClock(
                     T,
                     bdlf::MemFnUtil::memFn(&TestClass2::callback, &testObj1));
            x.start();
            startBarrier1.wait();
            const int wait = 1;
            ASSERT(0 == x.cancelClock(h, wait));
            int numExecuted = testObj1.numExecuted();
            ASSERTV(numExecuted, 1 == numExecuted);
            bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
            (void)x.scheduleEvent(now + T3,
                                  bdlf::MemFnUtil::memFn(&TestClass2::callback,
                                                         &testObj2));
            startBarrier2.wait();
            numExecuted = testObj1.numExecuted();
            ASSERTV(numExecuted, 1 == numExecuted);
            x.stop();
            bslmt::ThreadUtil::join(unblockThreadHandle);
        }
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
                          << "TESTING 'cancelAllEvents'" << endl
                          << "=========================" << endl;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_4;
        {
            // Schedule events at T, T2 and T3, invoke 'cancelAllEvents' and
            // verify the result.

          const bsls::TimeInterval T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T3(3 * DECI_SEC);

          TestClass1 testObj1;
          TestClass1 testObj2;
          TestClass1 testObj3;

          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta); x.start();

          bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
          Handle h1 = x.scheduleEvent(
                     now + T,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj1));

          Handle h2 = x.scheduleEvent(
                     now + T2,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj2));

          Handle h3 = x.scheduleEvent(
                     now + T3,
                     bdlf::MemFnUtil::memFn(&TestClass1::callback, &testObj3));

          x.cancelAllEvents();
          ASSERT( 0 != x.cancelEvent(h1) );
          ASSERT( 0 != x.cancelEvent(h2) );
          ASSERT( 0 != x.cancelEvent(h3) );
        }

        {
            // Schedule a few events in the past and one in the future.  The
            // second event will take a long time.  Sleep long enough to get
            // the events in the past pending, but not the one in the future.
            // CancelAllEvents without 'wait' specified.  Verify that the
            // appropriate events got canceled.  So as not to be thrown off by
            // intermittent excessively long sleeps, be prepared to start this
            // test over a certain number of times before giving up.

          const bsls::TimeInterval  T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T7(7 * DECI_SEC);
              // This will not be put onto the pending list.

          const int mT = DECI_SEC_IN_MICRO_SEC / 10; // 10ms
          const int T3  = 3 * DECI_SEC_IN_MICRO_SEC;
          const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);

          int ii;
          enum { MAX_LOOP = 4 };
          for (ii = 0; ii <= MAX_LOOP; ++ii) {
              TestClass1 testObj0;
              TestClass1 testObj1(T10);
              TestClass1 testObj2;
              TestClass1 testObj3;

              Obj x(&ta);

              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              x.scheduleEvent(now - (T2 + T),
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj0));

              x.scheduleEvent(now - T2,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj1));

              x.scheduleEvent(now - T,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj2));

              x.scheduleEvent(now + T7,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj3));

              double start = dnow();
              x.start();
              myMicroSleep(T3, 0); // give enough time to put on pending list
              x.cancelAllEvents();
              if ( 0 == testObj0.numExecuted() ) {
                  if (veryVerbose || MAX_LOOP == ii) {
                      P_(L_); Q(Events did not get on pending list);
                  }
                  continue;   // events did not get on pending list, start over
              }
              if (dnow() - start > 0.59) {
                  if (veryVerbose || MAX_LOOP == ii) {
                      P_(L_); Q(Overslept);
                  }
                  continue;    // overslept, start over
              }

              ASSERT( 1 == testObj0.numExecuted() );
              ASSERT( 0 == testObj1.numExecuted() );
              ASSERT( 0 == testObj2.numExecuted() );
              ASSERT( 0 == testObj3.numExecuted() );

              makeSureTestObjectIsExecuted(testObj1, mT, 100);
              myMicroSleep(T3, 0);
              ASSERT( 1 == testObj0.numExecuted() );
              ASSERT( 1 == testObj1.numExecuted() );
              ASSERT( 1 == testObj2.numExecuted() );
              ASSERT( 0 == testObj3.numExecuted() );

              break;
            }
            ASSERT(ii <= MAX_LOOP);
            if (veryVerbose) { P_(L_); P(ii); }
        }

        {
            // Schedule a few events in the past and one in the future.  The
            // second event will take a long time.  Sleep long enough to get
            // the events in the past pending, but not the one in the future.
            // CancelAllEvents with 'wait' specified.  Verify that the
            // appropriate events got canceled.  So as not to be thrown off by
            // intermittent excessively long sleeps, be prepared to start this
            // test over a certain number of times before giving up.

          const bsls::TimeInterval  T(1 * DECI_SEC);
          const bsls::TimeInterval T2(2 * DECI_SEC);
          const bsls::TimeInterval T6(6 * DECI_SEC);
          const bsls::TimeInterval T7(7 * DECI_SEC);

          const int T3  =  3 * DECI_SEC_IN_MICRO_SEC;
          const int T10 = 10 * DECI_SEC_IN_MICRO_SEC;

          bslma::TestAllocator ta(veryVeryVerbose);

          int ii;
          enum { MAX_LOOP = 4 };
          for (ii = 0; ii <= MAX_LOOP; ++ii) {
              TestClass1 testObj0;
              TestClass1 testObj1(T10);
              TestClass1 testObj2;
              TestClass1 testObj3;

              Obj x(&ta);

              bsls::TimeInterval now = bsls::SystemTime::nowRealtimeClock();
              x.scheduleEvent(now - (T + T2),
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj0));

              x.scheduleEvent(now - T2,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj1));

              x.scheduleEvent(now - T,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj2));

              x.scheduleEvent(now + T7,
                              bdlf::MemFnUtil::memFn(&TestClass1::callback,
                                                     &testObj3));

              x.start();
              myMicroSleep(T3, 0); // give enough time to put on pending list
              if ( 0 == testObj0.numExecuted() ) {
                  if (verbose || MAX_LOOP == ii) {
                      P_(L_); Q(Events not pending);
                  }
                  continue;
              }
              if (bsls::SystemTime::nowRealtimeClock() - now >= T6) {
                  if (verbose || MAX_LOOP == ii) {
                      P_(L_); Q(Overslept);
                  }
                  continue;
              }
              int wait = 1;
              x.cancelAllEvents(wait);
              ASSERT( 1 == testObj1.numExecuted() );
              ASSERT( 1 == testObj2.numExecuted() );
              ASSERT( 0 == testObj3.numExecuted() );
              myMicroSleep(T10, 0);
              ASSERT( 0 == testObj3.numExecuted() );

              break;
          }
          ASSERT(ii <= MAX_LOOP);
          if (verbose) { P_(L_); P(ii); }
        }
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
        //   Defensive programming: myMicroSleep can (and does sometimes,
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

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_3;

        typedef void (*Func)();

        const Func funcPtrs[] = { &test3_a, &test3_b, &test3_c, &test3_d,
                                  &test3_e, &test3_f, &test3_g };
        enum { NUM_THREADS = sizeof funcPtrs / sizeof funcPtrs[0] };

        bslmt::ThreadGroup tg(&testAllocator);

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == tg.addThread(funcPtrs[i]));
        }

        tg.joinAll();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'scheduleEvent' and 'startClock':
        //   Verifying 'scheduleEvent' and 'startClock'.
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
        //   Handle startClock(interval, callback,
        //                                    startTime=bsls::TimeInterval(0));
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'scheduleEvent' and 'startClock'" << endl
                      << "========================================" << endl;

        const int ATTEMPTS = 9;
        const int SUCCESS_THRESHOLD = 1;
        const int ALLOWED_DEVIATIONS = 1;

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_2;
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
        //   T3 interval, invoke 'cancelAllClocks' and verify the result.
        //
        //   Create and start a scheduler object, schedule a clock of T3
        //   interval, schedule an event at T6, invoke 'stop' at T4 and
        //   then verify the state.  Invoke 'start' and then verify the
        //   state.
        //
        // Note:
        //   Defensive programming: myMicroSleep can (and does sometimes,
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

        using namespace TIMER_EVENT_SCHEDULER_TEST_CASE_1;

        typedef void (*Func)();

        const Func funcPtrs[] = { &test1_a, &test1_b, &test1_c, &test1_d,
                                  &test1_e, &test1_f, &test1_g, &test1_h,
                                  &test1_i, &test1_j, &test1_k };
        enum { NUM_THREADS = sizeof funcPtrs / sizeof funcPtrs[0] };

        bslmt::ThreadGroup tg(&testAllocator);

        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 == tg.addThread(funcPtrs[i]));
        }

        tg.joinAll();
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

        {
            cout << "\n"
                    "\nFirst, a realtime clock will be used to verify"
                    "\nunexpected behavior during a system clock"
                    "\nchange (this test will continue until this"
                    "\nproblem is detected; *you* must move the system"
                    "\ntime backwards)."
                    "\n";

            TestClass1 testObj;

            Obj x(bsls::SystemClockType::e_REALTIME);
            x.start();

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
                myMicroSleep(200000, 1); // 1.2 seconds
            }

            x.cancelAllEvents(true);
        }

        {
            cout << "\n"
                    "\nExcellent!  The expected behavior occurred.  For"
                    "\nthe second part of the test, a monotonic clock"
                    "\nwill be used.  Here, a message will be printed"
                    "\nevery second and if the messages stop printing"
                    "\nbefore iteration 300 while you are changing the"
                    "\nsystem time there is an issue.  You may use"
                    "\nCNTRL-C to exit this test at any time.  The test"
                    "\nwill begin in three seconds; do not change the"
                    "\nsystem time until after you see the first printed"
                    "\nmessage."
                    "\n";

            myMicroSleep(0, 3); // 3 seconds

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
                myMicroSleep(200000, 1); // 1.2 seconds
            }

            x.cancelAllEvents(true);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == globalAllocator.numAllocations());
    ASSERT(0 == defaultAllocator.numAllocations());

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
