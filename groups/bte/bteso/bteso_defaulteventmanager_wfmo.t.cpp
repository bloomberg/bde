// bteso_defaulteventmanager_wfmo.t.cpp -*-C++-*-

#include <bteso_defaulteventmanager_wfmo.h>

#include <bteso_flag.h>
#include <bteso_socketimputil.h>
#include <bteso_timemetrics.h>
#include <bteso_eventmanagertester.h>

#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only
#include <bdes_platform.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bcef_vfunc0.h>
#include <bcefu_vfunc0.h>

#include <stdlib.h>     // atoi()
#include <iostream>
#include <fstream>
#include <ios>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Test the corresponding event manager component by using
// 'bteso_EventManagerTester' to exercise the "standard" test which applies to
// any event manager's test.  Since the difference exists in implementation
// between different event manager components, the "customized" test is also
// given for this event manager.  The "customized" test is implemented by
// utilizing the same script grammar and the same script interpreting
// defined in 'bteso_EventManagerTester' function but a new set of data to test
// this specific event manager component.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bteso_DefaultEventManager
// [ 2] ~bteso_DefaultEventManager
// MANIPULATORS
// [ 4] registerSocketEvent
// [ 5] deregisterSocketEvent
// [ 6] deregisterSocket
// [ 7] deregisterAll
// [ 8] dispatch
//
// ACCESSORS
// [ 3] numSocketEvents
// [ 3] numEvents
// [ 3] isRegistered
//-----------------------------------------------------------------------------
// [ 9] Usage example
// [ 1] BREATHING TEST

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
//                  STANDARD BDEX EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime)  cout <<                              \
            "### BDEX EXCEPTION TEST -- (ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) cout <<                                            \
        "### Begin bdex exception test." << endl;                           \
    int bdexExceptionCounter = 0;                                           \
    static int bdexExceptionLimit = 100;                                    \
    testInStream.setInputLimit(bdexExceptionCounter);                       \
    do {                                                                    \
        try {

#define END_BDEX_EXCEPTION_TEST                                             \
        } catch (bdex_TestInStreamException& e) {                           \
            if (veryVerbose && bdexExceptionLimit || veryVeryVerbose)       \
            {                                                               \
                --bdexExceptionLimit;                                       \
                cout << "(" <<                                              \
                bdexExceptionCounter << ')';                                \
                if (veryVeryVerbose) { cout << " BDEX_EXCEPTION: "          \
                    << "input limit = " << bdexExceptionCounter << ", "     \
                    << "last data type = " << e.dataType();                 \
                }                                                           \
                else if (0 == bdexExceptionLimit) {                         \
                    cout << " [ Note: 'bdexExceptionLimit' reached. ]";     \
                }                                                           \
                cout << endl;                                               \
            }                                                               \
            testInStream.setInputLimit(++bdexExceptionCounter);             \
            continue;                                                       \
        }                                                                   \
        testInStream.setInputLimit(-1);                                     \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) cout <<                                            \
        "### End bdex exception test." << endl;                             \
}
#else
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) { cout <<                                     \
        "### BDEX EXCEPTION TEST -- (NOT ENABLED) --" << endl;              \
        firstTime = 0;                                                      \
    }                                                                       \
}
#define END_BDEX_EXCEPTION_TEST
#endif

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
#if defined(BTESO_PLATFORM__WIN_SOCKETS)
static int veryVerbose;

typedef bteso_DefaultEventManager<bteso_Platform::WFMO> Obj;

enum { FAIL = -1, SUCCESS = 0 };

enum {
    MAX_SCRIPT = 50,
    MAX_PORT   = 50,
    BUF_LEN    = 8192
};

#if defined(BDES_PLATFORM__OS_WINDOWS)
    enum { READ_SIZE = 8192, WRITE_SIZE = 30000};
#else
    enum { READ_SIZE = 8192, WRITE_SIZE = 73728 };
#endif


//=============================================================================
//                            HELPER CLASSES
//-----------------------------------------------------------------------------

class RawEventManagerTest : public bteso_EventManager {
  // This class is a thin wrapper around 'bteso_DefaultEventManager_WfmoRaw'
  // that adheres to 'bteso_EventManager' protocol.  It is used to excercise
  // standard tests for the "raw" event manager.
  private:
      bteso_DefaultEventManager_WfmoRaw           d_impl;

  public:
    // CREATORS
    RawEventManagerTest(bteso_TimeMetrics *timeMetric     = 0,
                              bdema_Allocator   *basicAllocator = 0);
        // Create a 'select'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, global operators 'new' and
        // 'delete' are used.

    ~RawEventManagerTest();

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval&   timeout,
                 int                        flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified 'timeout'
        // is exceeded, or (3) provided that the specified 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, 0 on timeout, and a negative value otherwise; -1 is
        // reserved to indicate that an underlying system call was interrupted.
        // When such an interruption occurs this method will return (-1) if
        // 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT' and otherwise
        // will automatically restart (i.e., reissue the identical system
        // call).  Note that all callbacks are invoked in the same thread that
        // invokes 'dispatch', and the order of invocation, relative to the
        // order of registration, is unspecified.  Also note that -1 is never
        // returned if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT'.  The time values are
        // supported with the resolution of 10 ms, and, all time values
        // provided to the multiplexer are rounded up.

    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT',
        // an underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system
        // call was interrupted.  When such an interruption occurs this method
        // will return (-1) if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned if 'flags'
        // includes 'bteso_Flag::ASYNC_INTERRUPT'.

     int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                             const bteso_EventType::Type         event,
                             const bteso_EventManager::Callback& callback);
        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time
        // the corresponding event is detected.  'bteso_EventType::READ' and
        // 'bteso_EventType::WRITE' are the only events that can be registered
        // simulataneously for a socket. If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.  Simultaneous
        // registration of incompatible events for the same socket 'handle'
        // will result in undefined behavior.
        // Return 0 on success and a non-zero value otherwise.

    void deregisterSocketEvent(const bteso_SocketHandle::Handle& handle,
                               bteso_EventType::Type             event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.

    int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    void deregisterAll();
        // Deregister from this event manager all events on every socket
        // handle.

    // ACCESSORS
    int isRegistered(const bteso_SocketHandle::Handle& handle,
                     const bteso_EventType::Type       event) const;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    int numEvents() const;
        // Return the total number of all socket events currently registered
        // with this event manager.

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.
};

typedef RawEventManagerTest EventManagerName;

                             // --------
                             // CREATORS
                             // --------

EventManagerName::RawEventManagerTest(bteso_TimeMetrics *timeMetric,
                                      bdema_Allocator   *basicAllocator)
: d_impl(timeMetric, basicAllocator)
{

}

EventManagerName::~RawEventManagerTest() {

}

// MANIPULATORS
int EventManagerName::dispatch(int flags)
{
    return d_impl.dispatch(flags);
}

int EventManagerName::dispatch(const bdet_TimeInterval&   timeout,
                               int                        flags)
{
    return d_impl.dispatch(timeout, flags);
}

int EventManagerName::registerSocketEvent(
        const bteso_SocketHandle::Handle&   handle,
        const bteso_EventType::Type         eventType,
        const bteso_EventManager::Callback& callback)
{
    return d_impl.registerSocketEvent(handle, eventType, callback);
}

void EventManagerName::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type       event)
{
    d_impl.deregisterSocketEvent(handle, event);
}


int EventManagerName::deregisterSocket(
                const bteso_SocketHandle::Handle& handle)
{
    return d_impl.deregisterSocket(handle);
}


void EventManagerName::deregisterAll() {
    d_impl.deregisterAll();
}

// ACCESSORS
int EventManagerName::isRegistered(
    const bteso_SocketHandle::Handle& handle,
    const bteso_EventType::Type       event) const
{
    return d_impl.isRegistered(handle, event);

}

int EventManagerName::numEvents() const
{
    return d_impl.numEvents();
}

int EventManagerName::numSocketEvents (
        const bteso_SocketHandle::Handle& handle) const
{
    return d_impl.numSocketEvents(handle);
}



//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

struct Script {
    int            d_line;
    int            d_fails;    // number of failures in script
    const char    *d_script;
};

static void runScripts(bteso_EventManager    *mX,
                       const Script          *scripts,
                       int                    num_scripts,
                       int                    num_pairs,
                       int                    flags)
{
    enum { NUM_PAIRS = 4 };
    ASSERT(num_pairs == NUM_PAIRS);

    for (int i = 0; i < num_scripts; ++i) {
        bteso_EventManagerTestPair socketPairs[NUM_PAIRS];

        for (int j = 0; j < NUM_PAIRS; ++j) {
            LOOP_ASSERT(j, socketPairs[j].isValid());
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }

        int fails = bteso_EventManagerTester::gg(mX,
                                                 socketPairs,
                                                 scripts[i].d_script,
                                                 flags);
        const int LINE =  scripts[i].d_line;
        LOOP_ASSERT(LINE, scripts[i].d_fails == fails);

        if (veryVerbose) { P_(LINE); P(fails); }
    }
}

static
void timerCallback(bdet_TimeInterval *regTime, int *isInvoked) {
    ASSERT(regTime); ASSERT(isInvoked);
    *isInvoked = 1;
    ASSERT(bdetu_SystemTime::now() >= *regTime);
}

static void
genericCb(bteso_EventType::Type event, bteso_SocketHandle::Handle socket,
          int bytes, bteso_EventManager *mX)
{
    // User specified callback function that will be called after an event
    // is dispatched to do the "real" things.
    // This callback is only used in the 'usage example' test case, and will
    // be copied to the head file as a part of the usage example.
    enum {
        MAX_READ_SIZE = 8192,
        MAX_WRITE_SIZE = WRITE_SIZE
    };


    switch (event) {
      case bteso_EventType::READ: {
          ASSERT(0 < bytes);
          char buffer[MAX_READ_SIZE];

          int rc = bteso_SocketImpUtil::read(buffer, socket, bytes, 0);
          ASSERT(0 < rc);

      } break;
      case bteso_EventType::WRITE: {
          char wBuffer[MAX_WRITE_SIZE];
          ASSERT(0 < bytes);
          ASSERT(MAX_WRITE_SIZE >= bytes);
          memset(wBuffer,'4', bytes);
          int rc = bteso_SocketImpUtil::write(socket, &wBuffer, bytes, 0);
          ASSERT(0 < rc);
      } break;
      case bteso_EventType::ACCEPT: {
          int errCode;
          int rc = bteso_SocketImpUtil::close(socket, &errCode);
          ASSERT(0 == rc);
      } break;
      case bteso_EventType::CONNECT: {
          int errCode = 0;
          bteso_SocketImpUtil::close(socket, &errCode);
           ASSERT(0 == errCode);
      } break;
      default: {
          ASSERT("Invalid event code" && 0);
      } break;
    }
}

#endif // BTESO_PLATFORM__WIN_SOCKETS)

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
#if defined(BTESO_PLATFORM__WIN_SOCKETS)
    int verbose = argc > 2;
    veryVerbose = argc > 3; // this one is global
    int veryVeryVerbose = argc > 4;


    int controlFlags = 0;
    if (veryVeryVerbose) {
        controlFlags |= bteso_EventManagerTester::VERY_VERY_VERBOSE;
    }
    if (veryVerbose) {
        controlFlags |= bteso_EventManagerTester::VERY_VERBOSE;
    }
    if (verbose) {
        controlFlags |= bteso_EventManagerTester::VERBOSE;
    }

    ASSERT(0 == bteso_SocketImpUtil::startup());
    bdema_TestAllocator testAllocator(veryVeryVerbose);
    bteso_TimeMetrics timeMetric(bteso_TimeMetrics::MIN_NUM_CATEGORIES,
                                 bteso_TimeMetrics::CPU_BOUND);
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
#if defined(BTESO_PLATFORM__WIN_SOCKETS)
      case -4: {
        // -----------------------------------------------------------------
        // PERFORMANCE TESTING 'registerSocketEvent':
        //   Get the performance data.
        //
        // Plan:
        //   Open multiple sockets and register a read event for each
        //   socket, caculate the average time taken to register a read
        //   event for a given number of registered read event.
        // Testing:
        //   'registerSocketEvent' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                bteso_DefaultEventManager_WfmoRaw::MAX_NUM_HANDLES,
            DEFAULT_NUM_PAIRS        = 2 * MAX_NUM_HANDLES,  // should be more
            DEFAULT_NUM_MEASUREMENTS = 10
        };

        int numPairs = DEFAULT_NUM_PAIRS;
        int numMeasurements = DEFAULT_NUM_MEASUREMENTS;

        if (2 < argc) {
            int pairs = atoi(argv[2]);
            if (0 > pairs) {
                verbose = 0;
                numPairs = -pairs;
                controlFlags &= ~bteso_EventManagerTester::VERBOSE;
            }
            else {
                numPairs = pairs;
            }
        }

        if (3 < argc) {
            int measurements = atoi(argv[3]);
            if (0 > measurements) {
                veryVerbose = 0;
                numMeasurements = -measurements;
                controlFlags &= ~bteso_EventManagerTester::VERY_VERBOSE;
            }
            else {
                numMeasurements = measurements;
            }
        }

        if (verbose)
            cout << endl
            << "PERFORMANCE TESTING 'registerSocketEvent'" << endl
            << "=========================================" << endl;
        {
            const char *FILENAME = "selectRegister.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }

            if (veryVerbose) {
                P(numPairs);
                P(numMeasurements);
            }

            Obj mX(&timeMetric);  // Note: no test allocator --
                                  // performance testing
            bteso_EventManagerTester::testRegisterPerformance(&mX, outFile,
                      numPairs, numMeasurements,  controlFlags);
            outFile.close();
        }
      } break;
      case -3: {
        // -----------------------------------------------------------------
        // TESTING PERFORMANCE OF 'dispatch' METHOD:
        //   Get the performance data.
        //
        // Plan:
        //   Set up multiple connections and register a read event for each
        //   connection, calculate the average time it takes to dispatch a
        //   read event for a given number of registered sockets.
        // Testing:
        //   'dispatch' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                bteso_DefaultEventManager_WfmoRaw::MAX_NUM_HANDLES,
            DEFAULT_NUM_PAIRS        = 2 * MAX_NUM_HANDLES,
            DEFAULT_NUM_MEASUREMENTS = 10
        };

        int numPairs = DEFAULT_NUM_PAIRS;
        int numMeasurements = DEFAULT_NUM_MEASUREMENTS;

        if (2 < argc) {
            int pairs = atoi(argv[2]);
            if (0 > pairs) {
                verbose = 0;
                numPairs = -pairs;
                controlFlags &= ~bteso_EventManagerTester::VERBOSE;
            }
            else {
                numPairs = pairs;
            }
        }

        if (3 < argc) {
            int measurements = atoi(argv[3]);
            if (0 > measurements) {
                veryVerbose = 0;
                numMeasurements = -measurements;
                controlFlags &= ~bteso_EventManagerTester::VERY_VERBOSE;
            }
            else {
                numMeasurements = measurements;
            }
        }

        if (verbose)
            cout << endl
                << "PERFORMANCE TESTING 'dispatch'" << endl
                << "==============================" << endl;
        {
            const char *FILENAME = "selectDispatch.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }

            if (veryVerbose) {
                P(numPairs);
                P(numMeasurements);
            }

            Obj mX(&timeMetric);  // Note: no test allocator --
                                  // performance testing
            bteso_EventManagerTester::testDispatchPerformance(&mX, outFile,
                      numPairs, numMeasurements,
                      controlFlags);
            outFile.close();
        }
      } break;
      case -2: {
        // -----------------------------------------------------------------
        // TESTING 'registerSocketEvent' capacity:
        //   Get the performance data.
        //
        // Plan:
        //   Open multiple sockets and register a read event for each
        //   socket, caculate the average time taken to register a read
        //   event for a given number of registered read event.
        // Testing:
        //   'registerSocketEvent' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                bteso_DefaultEventManager_WfmoRaw::MAX_NUM_HANDLES,
            DEFAULT_NUM_PAIRS        = MAX_NUM_HANDLES,
            DEFAULT_NUM_MEASUREMENTS = 10
        };

        int numPairs = DEFAULT_NUM_PAIRS;
        int numMeasurements = DEFAULT_NUM_MEASUREMENTS;

        if (2 < argc) {
            int pairs = atoi(argv[2]);
            if (0 > pairs) {
                verbose = 0;
                numPairs = -pairs;
                controlFlags &= ~bteso_EventManagerTester::VERBOSE;
            }
            else {
                numPairs = pairs;
            }
            if (numPairs > MAX_NUM_HANDLES) {
                numPairs = MAX_NUM_HANDLES;
            }
        }

        if (3 < argc) {
            int measurements = atoi(argv[3]);
            if (0 > measurements) {
                veryVerbose = 0;
                numMeasurements = -measurements;
                controlFlags &= ~bteso_EventManagerTester::VERY_VERBOSE;
            }
            else {
                numMeasurements = measurements;
            }
        }

        if (verbose)
            cout << endl
            << "PERFORMANCE TESTING '_WfmoRaw::registerSocketEvent'" << endl
            << "=====================================================" << endl;
        {
            const char *FILENAME = "selectRawRegister.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }

            if (veryVerbose) {
                P(numPairs);
                P(numMeasurements);
            }

            RawEventManagerTest mX(&timeMetric);  // Note: no test allocator --
                                                  // performance testing
            bteso_EventManagerTester::testRegisterPerformance(&mX, outFile,
                      numPairs, numMeasurements,  controlFlags);
            outFile.close();
        }
      } break;
      case -1: {
        // -----------------------------------------------------------------
        // TESTING PERFORMANCE OF 'dispatch' METHOD:
        //   Get the performance data.
        //
        // Plan:
        //   Set up multiple connections and register a read event for each
        //   connection, calculate the average time it takes to dispatch a
        //   read event for a given number of registered sockets.
        // Testing:
        //   'dispatch' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                bteso_DefaultEventManager_WfmoRaw::MAX_NUM_HANDLES,
            DEFAULT_NUM_PAIRS        = MAX_NUM_HANDLES,
            DEFAULT_NUM_MEASUREMENTS = 10
        };

        int numPairs = DEFAULT_NUM_PAIRS;
        int numMeasurements = DEFAULT_NUM_MEASUREMENTS;

        if (2 < argc) {
            int pairs = atoi(argv[2]);
            if (0 > pairs) {
                verbose = 0;
                numPairs = -pairs;
                controlFlags &= ~bteso_EventManagerTester::VERBOSE;
            }
            else {
                numPairs = pairs;
            }
            if (numPairs > MAX_NUM_HANDLES) {
                numPairs = MAX_NUM_HANDLES;
            }
        }

        if (3 < argc) {
            int measurements = atoi(argv[3]);
            if (0 > measurements) {
                veryVerbose = 0;
                numMeasurements = -measurements;
                controlFlags &= ~bteso_EventManagerTester::VERY_VERBOSE;
            }
            else {
                numMeasurements = measurements;
            }
        }

        if (verbose)
            cout << endl
                << "PERFORMANCE TESTING '_WfmoRaw::dispatch'" << endl
                << "==========================================" << endl;
        {
            const char *FILENAME = "selectRawDispatch.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }

            if (veryVerbose) {
                P(numPairs);
                P(numMeasurements);
            }

            RawEventManagerTest mX(&timeMetric);  // Note: no test allocator --
                                                  // performance testing
            bteso_EventManagerTester::testDispatchPerformance(&mX, outFile,
                      numPairs, numMeasurements,
                      controlFlags);
            outFile.close();
        }
      } break;
      case 15: {
        // -----------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and replace 'assert' with
        //   'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // -----------------------------------------------------------------

      } break;
      case 14: {
        // -----------------------------------------------------------------
        // TESTING 'dispatch' FUNCTION:
        //   The goal is to ensure that 'dispatch' invokes the callback
        //   method for the write socket handle and event, for all possible
        //   events.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the dispatch() in
        //   this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        // Exhausting test:
        //   Test the "timeout" from the dispatch() with the loop-driven
        //   implementation where timeout value are generated during each
        //   iteration and invoke the dispatch() with it.
        // Testing:
        //   int dispatch();
        //   int dispatch(const bdet_TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'dispatch' METHOD." << endl
                                  << "==========================" << endl;

// TBD - copy canned tests here and adjust for WFMO funniness
//        if (verbose)
//            cout << "Standard test for 'dispatch'" << endl
//                 << "============================" << endl;
//        {
//            Obj mX(&timeMetric, &testAllocator);
//            bteso_EventManagerTester::testDispatch(&mX, controlFlags);
//        }

        if (verbose)
            cout << "Customized test for 'dispatch'" << endl
                 << "==============================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w2; Dn,1"},
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"},
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                       "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8"},
               {L_, 0, "Dn100,0"},
               {L_, 0, "Dn0,0"},
               {L_, 0, "+2r3; Dn100,0; +2w40; Dn100,1;  W2,3000; Dn100,2"},
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"},
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"},
               {L_, 0, "+0w20; +0r12; Dn0,1; W0,30; +1w6; +2w8; Dn100,4"},
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; Dn100,4;"
                        "W0,60; W1,70; +1r6; W2,60; W3,60; +2r8; +3r10;"
                        "Dn120,8"},
            };
// TBD            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;
            const int NUM_SCRIPTS = 0;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                bteso_EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                       SCRIPTS[i].d_script,
                                                       controlFlags);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
        if (verbose)
            cout << "Verifying behavior on timeout (no sockets)." << endl;
        {
            const int NUM_ATTEMPTS = 0; // TBD 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                const Obj& X = mX;
                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                                                bteso_Flag::ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVeryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
        if (verbose)
            cout << "Verifying behavior on timeout (at least one socket)."
                 << endl;
        {
            bteso_EventManagerTestPair socketPair;
            bcef_Vfunc0 nullFunctor;
            const int NUM_ATTEMPTS = 0; // TBD 5000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                const Obj& X = mX;
                mX.registerSocketEvent(socketPair.observedFd(),
                                       bteso_EventType::READ,
                                       nullFunctor);

                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                                                bteso_Flag::ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
      } break;
      case 13: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterAll' FUNCTION:
        //   It must be verified that the application of 'deregisterAll'
        //   from any state returns the event manager.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the deregisterAll() in
        //   this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   void deregisterAll();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterAll'" << endl
                                  << "=======================" << endl;
        if (verbose)
            cout << "\tStandard test for 'deregisterAll'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterAll(&mX, controlFlags);
        }
      } break;
      case 12: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocket' FUNCTION:
        //   All possible transitions from other state to 0 must be
        //   exhaustively tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the deregisterSocket()
        //   in this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   int deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocket'" << endl
                                  << "==========================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocket(&mX, controlFlags);
        }
      } break;
      case 11: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocketEvent' FUNCTION:
        //   All possible deregistration transitions must be exhaustively
        //   tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the
        //   deregisterSocketEvent() in this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   void deregisterSocketEvent();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocketEvent'" << endl
                                  << "===============================" << endl;
        if (verbose)
            cout << "\tStandard test for 'deregisterSocketEvent'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocketEvent(&mX,
                                                                controlFlags);
        }

        if (verbose)
            cout << "\tCustomized test for 'deregisterSocketEvent'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w5; -0w; T0"},
               {L_, 0, "+0w5; +0r3; -0w; E0r; T1"},
               {L_, 0, "+0w5; +1r2; -0w; E1r; T1"},
               {L_, 0, "+0w5; +1r4; -1r; E0w; T1"},
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                bteso_EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                         SCRIPTS[i].d_script,
                                                         controlFlags);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 10: {
        // -----------------------------------------------------------------
        // TESTING 'registerSocketEvent' FUNCTION:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'bteso_EventManagerTester', where a
        //   number of socket pairs are created to test the
        //   registerSocketEvent() in this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        // Testing:
        //   void registerSocketEvent();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'registerSocketEvent'" << endl
                                  << "=============================" << endl;
        if (verbose)
            cout << "\tStandard test for 'registerSocketEvent'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            const Obj& X = mX;
            bteso_EventManagerTester::testRegisterSocketEvent(&mX,
                                                              controlFlags);
        }

        if (verbose)
            cout << "\tCustomized test for 'registerSocketEvent'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w2; E0w; T1"},
               {L_, 0, "+0r20; E0r; T1"},
               {L_, 0, "+0w3; +0w4; E0w; T1"},
               {L_, 0, "+0r3; +0r14; E0r; T1"},
               {L_, 0, "+0w5; +0w6; +0r3; +0r32; E0rw; T2"},
               {L_, 0, "+0w5; +1r2; E0w; E1r; T2"},
               {L_, 0, "+0w5; +1r4; +1w5; +0r6; E0rw; E1rw; T4"},
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                bteso_EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                if (veryVeryVerbose) {
                    ctrlFlag |= bteso_EventManagerTester::VERY_VERY_VERBOSE;
                }
                else if (veryVerbose) {
                    ctrlFlag |= bteso_EventManagerTester::VERY_VERBOSE;
                }
                else if (verbose) {
                    ctrlFlag |= bteso_EventManagerTester::VERBOSE;
                }
                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                       SCRIPTS[i].d_script,
                                                       ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 9: {
        // -----------------------------------------------------------------
        // TESTING ACCESSORS:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'bteso_EventManagerTester', where a
        //   number of socket pairs are created to test the accessors in
        //   this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   int isRegistered();
        //   int numEvents() const;
        //   int numSocketEvents();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING ACCESSORS" << endl
                                  << "=================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testAccessors(&mX, controlFlags);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Ensure the basic liveness of an event manager instance.
        //
        // Testing:
        //   Create an object of this event manager under test.  Perform
        //   some basic operations on it.
        // --------------------------------------------------------------------
        typedef bteso_DefaultEventManager_WfmoRaw ObjUnderTest;
        {
            bdema_Pool pool(sizeof(ObjUnderTest), &testAllocator);

            ObjUnderTest *x = new (pool) ObjUnderTest(&timeMetric,
                                                      &testAllocator);
            pool.deleteObject(x);
        }
        enum { NUM_TESTS = 10000 };
        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj mX(&timeMetric, &testAllocator);
        }
      } break;
      case 7: {
        // -----------------------------------------------------------------
        // TESTING 'dispatch' FUNCTION:
        //   The goal is to ensure that 'dispatch' invokes the callback
        //   method for the write socket handle and event, for all possible
        //   events.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the dispatch() in
        //   this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        // Exhausting test:
        //   Test the "timeout" from the dispatch() with the loop-driven
        //   implementation where timeout value are generated during each
        //   iteration and invoke the dispatch() with it.
        // Testing:
        //   int dispatch();
        //   int dispatch(const bdet_TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose)
          cout << endl
                 << "TESTING '_WfmoRaw::dispatch'" << endl
                 << "==============================" << endl;;

                if (verbose)
            cout << "\tStandard test for 'dispatch'" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);

            // The standard dispatch test does not work with wfmo,
            // so we customize it below.

            {
                Script SCRIPTS[] =
                {   // The reason to put "-a;" at the beginning of each script
                    // is that we only use ONE event manager for all scripts.
/*
                    // Test the event manager when no socket event exists.
                    {L_, 0, "-a; Dn0,0"},
                    {L_, 0, "-a; Dn100,0"},

                    // Test the event manager when one socket event exists.
                    {L_, 0, "-a; +0w2; Dn,1"},
                    {L_, 0, "-a; W0,64; +0r24; Dn,1; -0; T0"},

                    // Test the event manager when two socket events exist.
                    // The two socket events are for the same socket handle.
                    {L_, 0, "-a; +0w64; +0r24; Dn,1; -0w; Di500,0; -0r; T0"},
                    {L_, 0, "-a; +0r24; +0w64; Dn,1; -0w; Di500,0; -0r; T0"},
                  {L_, 0, "-a; +0w64; +0r24; W0,64; Dn,2; -0w; Di,1; -0r; T0"},

                    // The two socket events are for different socket handles.
                    {L_, 1, "-a; +0w64; +1w24; W0,64; Dn,2; -0w; Di500,1; T1"},
                    {L_, 0, "-a; +0w64; +1r24; W0,64; Dn,1; -0w; Di500,0; T1"},
                    {L_, 0, "-a; +0w64; +1a;   W0,64; Dn,1; -0w; Di500,0; T1"},

                    {L_, 0, "-a; +0r64; +1w24; W0,64; Dn,2; -1w; Di500,0; T1"},
                    {L_, 0, "-a; +0r64; +1r24; W0,64; Dn,1; -0r; Di500,0; T1"},
                    {L_, 0, "-a; +0r64; +1a;   W0,64; Dn,1; -0r; Di500,0; T1"},

                    {L_, 0, "-a; +0a; +1w24; Dn,1; -1w; Di500,0; T1"},
                    {L_, 0, "-a; +0a; +1r64; W1,64; Dn,1; -0a; Di500,0; T1"},

                    // Test the event manager when multiple socket events exist
               {L_, 0, "-a; +0w64; +1r10; +2w100; +1w20; +3w60; Dn,4; -a; T0"},

                    // Test the event manager when a test script needs to be
                    // executed in the user-installed callback function.
                    {L_, 0, "-a; +0r24{-a}; +0w64; W0,64; T2; Dn,1; E0; T0"},
                  {L_, 0, "-a; +0r24{-a}; +1r64; W0,64; T2; Dn,1; E0; E1; T0"},
*/

                    {L_, 0, "-a; +0w1{+0r1}; Dn,1; Dn,1"},
//         {L_, 0, "-a; W0,60; +0w64{+2w100; +0r10}; Dn,1; -0w; Di,2; -a; T0"},
//{L_, 0, "-a; W0,60; +0w64{-1r; +0r10}; +1r20; E1r; Dn,1; -0w; Di,1; -a; T0"},

                    // Test the event manager when the socket is not writeable,
                    // which forces the request to timeout.
//             {L_, 0, "+0w500000; Dn,1; +0w26000; Dn300,1; Dn120,0; -0w; T0"},
//                  {L_, 0, "+0w28720; Dn,1; +0w26000; Dn120,0; -0w; T0"},
                };

                enum {
                    NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS,
                    NUM_PAIRS   = 4
                };

                runScripts(&mX, SCRIPTS, NUM_SCRIPTS, NUM_PAIRS, controlFlags);
            }
        }

        if (verbose)
            cout << "\tCustomized test for 'dispatch'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w32768; Di500,1; +1w1; Di500,1; Di500,0; T2" },
             //{L_, 0, "+0r1; W064; Di500,1; Di500,0; Di500,0; T2" }
                           /*                                W0,64;
                           {L_, 0, "+0w2; Dn,1"},
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"},
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                       "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8"},
               {L_, 0, "Dn100,0"},
               {L_, 0, "Dn0,0"},
               {L_, 0, "+2r3; Dn100,0; +2w40; Dn100,1;  W2,3000; Dn100,2"},
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"},
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"},
               {L_, 0, "+0w20; +0r12; Dn0,1; W0,30; +1w6; +2w8; Dn100,4"},
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; Dn100,4;"
                        "W0,60; W1,70; +1r6; W2,60; W3,60; +2r8; +3r10;"
                        "Dn120,8"},
*/
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;
                enum { NUM_PAIRS = 4 };
                bteso_EventManagerTestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; ++j) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                         SCRIPTS[i].d_script,
                                                         controlFlags);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
/*
        if (verbose)
            cout << "\tVerifying behavior on timeout (no sockets)." << endl;
        {
            const int NUM_ATTEMPTS = 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                const RawEventManagerTest& X = mX;
                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                                                bteso_Flag::ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVeryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
        if (verbose)
            cout << "\tVerifying behavior on timeout (at least one socket)."
                 << endl;
        {
            bteso_EventManagerTestPair socketPair;
            bcef_Vfunc0 nullFunctor;
            const int NUM_ATTEMPTS = 5000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                const RawEventManagerTest& X = mX;
                mX.registerSocketEvent(socketPair.observedFd(),
                                       bteso_EventType::READ,
                                       nullFunctor);

                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                                                bteso_Flag::ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVeryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
*/
      } break;
      case 6: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterAll' FUNCTION:
        //   It must be verified that the application of 'deregisterAll'
        //   from any state returns the event manager.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the deregisterAll() in
        //   this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   void deregisterAll();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING '_WfmoRaw::deregisterAll'" << endl
                 << "===================================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterAll(&mX, controlFlags);
        }
      } break;
      case 5: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocket' FUNCTION:
        //   All possible transitions from other state to 0 must be
        //   exhaustively tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the deregisterSocket()
        //   in this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   int deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING '_WfmoRaw::deregisterSocket'" << endl
                 << "======================================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocket(&mX, controlFlags);
        }
      } break;
      case 4: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocketEvent' FUNCTION:
        //   All possible deregistration transitions must be exhaustively
        //   tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the
        //   deregisterSocketEvent() in this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   void deregisterSocketEvent();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING '_WfmoRaw::deregisterSocketEvent'" << endl
                 << "===========================================" << endl;
        if (verbose)
            cout << "\tStandard test for 'deregisterSocketEvent'" << endl;

        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocketEvent(&mX,
                                                                controlFlags);
        }

        if (verbose)
            cout << "\tCustomized test for 'deregisterSocketEvent'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w5; -0w; T0"},
               {L_, 0, "+0w5; +0r3; -0w; E0r; T1"},
               {L_, 0, "+0w5; +1r2; -0w; E1r; T1"},
               {L_, 0, "+0w5; +1r4; -1r; E0w; T1"},
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                RawEventManagerTest mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                bteso_EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                         SCRIPTS[i].d_script,
                                                         controlFlags);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;

      case 3: {
        // -----------------------------------------------------------------
        // TESTING 'registerSocketEvent' FUNCTION:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'bteso_EventManagerTester', where a
        //   number of socket pairs are created to test the
        //   registerSocketEvent() in this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        // Testing:
        //   void registerSocketEvent();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING '_WfmoRaw::registerSocketEvent'" << endl
                 << "=========================================" << endl;

        RawEventManagerTest mX(&timeMetric, &testAllocator);

        if (verbose)
            cout << "\tStandard test for 'registerSocketEvent'" << endl;
        {
            bteso_EventManagerTester::testRegisterSocketEvent(&mX,
                                                              controlFlags);
        }

        if (verbose)
            cout << "\tCustom test for 'registerSocketEvent'" << endl;
        {
            Script SCRIPTS[] =
            {
               {L_, 0, "-a; +0w2; E0w; T1"},
               {L_, 0, "-a; +0r20; E0r; T1"},
               {L_, 0, "-a; +0w3; +0w4; E0w; T1"},
               {L_, 0, "-a; +0r3; +0r14; E0r; T1"},
               {L_, 0, "-a; +0w5; +0w6; +0r3; +0r32; E0rw; T2"},
               {L_, 0, "-a; +0w5; +1r2; E0w; E1r; T2"},
               {L_, 0, "-a; +0w5; +1r4; +1w5; +0r6; E0rw; E1rw; T4"},
            };

            enum {
                NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS,
                NUM_PAIRS   = 4
            };

            runScripts(&mX, SCRIPTS, NUM_SCRIPTS, NUM_PAIRS, controlFlags);
        }
      } break;

      case 16: {
        // -----------------------------------------------------------------
        // TESTING ACCESSORS:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'bteso_EventManagerTester', where a
        //   number of socket pairs are created to test the accessors in
        //   this event manager.
        // Customized test:
        //   No customized test since no difference in implementation
        //   between all event managers.
        // Testing:
        //   int isRegistered();
        //   int numEvents() const;
        //   int numSocketEvents();
        // -----------------------------------------------------------------
        if (verbose)
              cout << endl
                   << "TESTING ACCESSORS FOR '_WfmoRaw' MANAGER" << endl
                   << "==========================================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testAccessors(&mX, controlFlags);
        }
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // VERIFYING ASSUMPTIONS REGARDING 'select' SYSTEM CALL
        // Concerns:
        //    o the return value of and the effect on the fd sets of
        //      'select' system call when a single socket is both readable
        //      and writiable simultaneously.
        //    o adding handles to an empty fd_set and then removing them
        //      results in binary-equal empty fd_set.
        //
        // Methodology:
        //    Create a set of (connected) socket pairs, register certain
        //    endpoints with the 'select' call and use the peer endpoints
        //    control.
        // -----------------------------------------------------------------

        if (verbose) cout << endl
              << "VERIFYING 'WaitForMultipleObjects' SYSTEM CALL." << endl
              << "===============================================" << endl;

        if (veryVerbose) cout << "\tInitializing socket pairs." << endl;

// TBD        enum { NUM_PAIRS = 64 };
        enum { NUM_PAIRS = 0 };

// TBD        bteso_EventManagerTestPair testPairs[NUM_PAIRS];
        bteso_EventManagerTestPair testPairs[1];
        for (int i = 0; i < NUM_PAIRS; i++) {
            testPairs[i].setObservedBufferOptions(BUF_LEN, 1);
            testPairs[i].setControlBufferOptions(BUF_LEN, 1);
        }

        if (verbose) cout << "\tVerifying WSACreateEvent." << endl;
        for (int i = 0; i < NUM_PAIRS; ++i) {
            HANDLE h = WSACreateEvent();
            long mask = FD_WRITE;
            int rc = WSAEventSelect(testPairs[i].observedFd(), h, mask);
            LOOP_ASSERT(i, 0 == rc);
            CloseHandle(h);
        }

        if (verbose) {
            cout << "\tVerifying WSAWaitForMultipleEvents on a single handle."
                 << endl;
        }
        enum {
            NUM_ITERATIONS = 10,
            TIMEOUT = 500 // milliseconds
        };

        for (int i = 0; i < NUM_PAIRS; ++i) {
            WSAEVENT h = WSACreateEvent();
            long mask = FD_WRITE;
            int rc = WSAEventSelect(testPairs[i].observedFd(), h, mask);
            LOOP_ASSERT(i, 0 == rc);
            for (int j = 0; j < NUM_ITERATIONS; ++j) {
                DWORD rc =
                        WSAWaitForMultipleEvents(1, &h, FALSE, TIMEOUT, FALSE);
                LOOP2_ASSERT(i, j, WSA_WAIT_EVENT_0 == rc);
                if (veryVerbose) {
                    P_(i); P_(j); P_(WSA_WAIT_EVENT_0); P_(rc); P(mask);
                }
                CloseHandle(h);
            }
        }

        if (verbose)
        cout << "\tVerifying WSAWaitForMultipleEvents on multiple handles."
             << endl;
        {
            enum {
                TIMEOUT = 500, // milliseconds
                NUM_ITERATIONS = 4
            };

// TBD            WSAEVENT systemEvents[NUM_PAIRS];
            WSAEVENT systemEvents[1];
            long mask = FD_WRITE;
            for (int i = 1; i < NUM_PAIRS; ++i) {
                for (int j = 0; j < i; ++j) {
                    systemEvents[j] = WSACreateEvent();
                    LOOP2_ASSERT(i, j,
                                 WSA_INVALID_EVENT != systemEvents[j]);
                    int rc = WSAEventSelect(testPairs[j].observedFd(),
                                            systemEvents[j], mask);
                    LOOP2_ASSERT(i, j, 0 == rc);
                }

                for (int j = 0; j < NUM_ITERATIONS; ++j) {
                    int numHandles = i;
                    DWORD rc = WSAWaitForMultipleEvents(i,
                                                        systemEvents,
                                                        FALSE,
                                                        TIMEOUT,
                                                        FALSE);
                    LOOP2_ASSERT(i, j, WSA_WAIT_EVENT_0 == rc);
                    if (veryVerbose) {
                        P_(i); P_(j); P_(numHandles);
                        P_(WSA_WAIT_EVENT_0);
                        P_(rc); P(mask);
                    }
                    int index = rc - WSA_WAIT_EVENT_0;
                    for (int k = index + 1; k < i; ++k) {
                        rc = WSAWaitForMultipleEvents(1,
                                                      &systemEvents[k],
                                                      TRUE,
                                                      0,
                                                      FALSE);
                        LOOP3_ASSERT(i, j, k, WSA_WAIT_EVENT_0 == rc);
                        if (veryVerbose) {
                            P_(i); P_(j); P_(k);  P_(rc);
                            P(systemEvents[k]);
                        }
                    }
                    for (int k = 0; k < i; ++k) {
                        const unsigned char byte = 0xAB;
                        int rc = bteso_SocketImpUtil::
                                           write(testPairs[k].observedFd(),
                                                 &byte,
                                                 sizeof(char));
                        LOOP3_ASSERT(i, j, k, sizeof(char) == rc);
                    }
                }

                for (int j = 0; j < i; ++j) {
                    LOOP2_ASSERT(i, j, WSACloseEvent(systemEvents[j]));
                }
            }
        }
      } break;
#endif // BTESO_PLATFORM__WIN_SOCKETS
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    ASSERT(0 == bteso_SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
