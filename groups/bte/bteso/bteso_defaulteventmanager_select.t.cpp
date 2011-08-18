// bteso_defaulteventmanager_select.t.cpp   -*-C++-*-
#include <bteso_defaulteventmanager_select.h>
#include <bteso_socketimputil.h>
#include <bteso_timemetrics.h>
#include <bteso_eventmanagertester.h>
#include <bteso_flag.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>
#include <bdema_pool.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bdef_function.h>
#include <bdef_bind.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_fstream.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Test the corresponding event manager component by using
// 'bteso_EventManagerTester' to exercise the "standard" test which applies to
// any event manager's test.
//
// Since differences exist in implementation between different event manager
// components, a "customized" test is also given for this event manager.  The
// "customized" test is implemented by utilizing the same script grammar and
// the same script interpreting defined in 'bteso_EventManagerTester' function
// but a new set of data to test this specific event manager component.
//-----------------------------------------------------------------------------
// CLASS bteso_DefaultEventManager<bteso_Platform::SELECT>
// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATORS
// [ 3] bteso_DefaultEventManager
// [ 3] ~bteso_DefaultEventManager
//
// MANIPULATORS
// [11] registerSocketEvent
// [12] deregisterSocketEvent
// [13] deregisterSocket
// [14] deregisterAll
// [15] dispatch
//
// ACCESSORS
// [17] canRegisterSockets
// [17] hasLimitedSocketCapacity
// [10] numSocketEvents
// [10] numEvents
// [10] isRegistered
//-----------------------------------------------------------------------------
// [18] USAGE
// [16] TESTING CONCERN: exception set on windows (DRQS 11464834)
// [ 1] BREATHING TEST
// [ 2] Assumptions about 'select' system call
// [-4] TESTING PERFORMANCE 'registerSocketEvent'
// [-3] TESTING PERFORMANCE 'dispatch'
// [-2] TESTING PERFORMANCE 'registerSocketEvent'
// [-1] TESTING PERFORMANCE 'dispatch'
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
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bteso_DefaultEventManager<bteso_Platform::SELECT> Obj;


enum {
    FAIL    = -1,
    SUCCESS = 0
};

enum {
    MAX_SCRIPT = 50,
    MAX_PORT   = 50,
    BUF_LEN    = 8192
};

#if defined(BSLS_PLATFORM__OS_WINDOWS)
    enum {
        READ_SIZE = 8192,
        WRITE_SIZE = 30000
    };
#else
    enum {
        READ_SIZE  = 8192,
        WRITE_SIZE = 73728
    };
#endif

//=============================================================================
//                        HELPER FUNCTIONS AND CLASSES
//-----------------------------------------------------------------------------

void checkDeadConnectCallback(bteso_SocketHandle::Handle  handle,
                              bool                       *hasExecutedFlag)
    // Check, for case 16, that the connection status of the specified 'handle'
    // is not good, and set 'hasExecutedFlag' to 'true'.
{
    bteso_IPv4Address result;
    ASSERT(0 != bteso_SocketImpUtil::getPeerAddress(&result, handle));
    if (hasExecutedFlag) {
        *hasExecutedFlag = true;
    }
}

class RawEventManagerTest : public bteso_EventManager {
    // This class is a thin wrapper around 'Obj' that adheres to
    // 'bteso_EventManager' protocol.

    Obj d_impl;

    RawEventManagerTest(const RawEventManagerTest&);
    RawEventManagerTest& operator=(const RawEventManagerTest);

  public:
    // CREATORS
    RawEventManagerTest(bteso_TimeMetrics *timeMetric     = 0,
                        bslma_Allocator   *basicAllocator = 0);
        // Create a 'select'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, global operators 'new' and
        // 'delete' are used.

    ~RawEventManagerTest();

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval&         timeout, int flags);

    int dispatch(int flags);

    int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                            const bteso_EventType::Type         event,
                            const bteso_EventManager::Callback& callback);

    void deregisterSocketEvent(const bteso_SocketHandle::Handle& handle,
                               bteso_EventType::Type             event);

    int deregisterSocket(const bteso_SocketHandle::Handle& handle);

    void deregisterAll();

    // ACCESSORS
    bool canRegisterSockets() const;

    bool hasLimitedSocketCapacity() const;

    int isRegistered(const bteso_SocketHandle::Handle& handle,
                     const bteso_EventType::Type       event) const;

    int numEvents() const;

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const;
};

typedef RawEventManagerTest EventManagerName;

                             // --------
                             // CREATORS
                             // --------

EventManagerName::RawEventManagerTest(bteso_TimeMetrics *timeMetric,
                                      bslma_Allocator   *basicAllocator)
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

int EventManagerName::dispatch(const bdet_TimeInterval& timeout, int flags)

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

int EventManagerName::
                     deregisterSocket(const bteso_SocketHandle::Handle& handle)
{
    return d_impl.deregisterSocket(handle);
}

void EventManagerName::deregisterAll() {
    d_impl.deregisterAll();
}

// ACCESSORS
bool EventManagerName::canRegisterSockets() const
{
    return d_impl.canRegisterSockets();
}

bool EventManagerName::hasLimitedSocketCapacity() const
{
    return d_impl.hasLimitedSocketCapacity();
}

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

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    // TBD: these tests frequently timeout on Windows, disabling until fixed
#ifdef BSLS_PLATFORM__OS_WINDOWS
    testStatus = -1;
#else

    int controlFlag = 0;
    if (veryVeryVerbose) {
        controlFlag |= bteso_EventManagerTester::BTESO_VERY_VERY_VERBOSE;
    }
    if (veryVerbose) {
        controlFlag |= bteso_EventManagerTester::BTESO_VERY_VERBOSE;
    }
    if (verbose) {
        controlFlag |= bteso_EventManagerTester::BTESO_VERBOSE;
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    ASSERT(0 == bteso_SocketImpUtil::startup());
    bslma_TestAllocator testAllocator(veryVeryVerbose);
    bteso_TimeMetrics timeMetric(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
                                 bteso_TimeMetrics::BTESO_CPU_BOUND);

    switch (test) { case 0:  // Zero is always the leading case.
      case 19: {
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
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        if (verbose)
            cout << "For a comprehensive usage example, see " << endl
                 << "'bteso_defaulteventmanager_poll' component." << endl;

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'dispatchCallbacks'
        //   Dispatching signaled user callbacks works correctly even if one
        //   of the callbacks deregisters all socket events.
        //
        // Plan:
        //   Create a socket pair and register a read and write events on both
        //   sockets in the pair.  The registered callback would invoke
        //   'deregisterAll'.  Dispatch the callbacks (with a timeout)
        //   and verify that the callback is invoked correctly
        //
        // Testing:
        //   int dispatchCallbacks(...)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "VERIFYING 'deregisterAll' IN 'dispatch' CALLBACK" << endl
              << "================================================" << endl;

        enum { NUM_BYTES = 16 };

        Obj mX; const Obj& X = mX;

        bteso_SocketHandle::Handle socket[2];

        int rc = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                             socket, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
        ASSERT(0 == rc);

        bdef_Function<void (*)()> deregisterCallback(
                bdef_MemFnUtil::memFn(&Obj::deregisterAll, &mX));

        ASSERT(0 == mX.registerSocketEvent(socket[0],
                                           bteso_EventType::BTESO_READ,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[0],
                                           bteso_EventType::BTESO_WRITE,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[1],
                                           bteso_EventType::BTESO_READ,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[1],
                                           bteso_EventType::BTESO_WRITE,
                                           deregisterCallback));

        char wBuffer[NUM_BYTES];
        memset(wBuffer,'4', NUM_BYTES);
        rc = bteso_SocketImpUtil::write(socket[0], &wBuffer, NUM_BYTES, 0);
        ASSERT(0 < rc);
        rc = bteso_SocketImpUtil::write(socket[1], &wBuffer, NUM_BYTES, 0);
        ASSERT(0 < rc);

        ASSERT(1 == mX.dispatch(bdet_TimeInterval(1.0), 0));

      } break;

      case 17: {
        // -----------------------------------------------------------------
        // TESTING 'canRegisterSockets' and 'hasLimitedSocketCapacity'
        //
        // Concern:
        //: 1 'hasLimitiedSocketCapacity' returns 'true'.
        //:
        //: 2 'canRegisterSockets' returns 'true' upto 'BTESO_MAX_NUM_HANDLES'
        //:   handles are registered and 'false' after that.
        //
        // Plan:
        //: 1 Assert that 'hasLimitedSocketCapacity' returns 'true'.
        //:
        //: 2 Register socket events upto 'BTESO_MAX_NUM_HANDLES'.  Verify
        //:   that 'canRegisterSockets' always returns 'true'.  After that
        //:   limit confirm that 'canRegisterSockets' returns 'false'.
        //
        // Testing:
        //   bool canRegisterSockets() const;
        //   bool hasLimitedSocketCapacity() const;
        // -----------------------------------------------------------------

        if (verbose) cout << endl
                << "TESTING 'canRegisterSockets' and 'hasLimitedSocketCapacity"
                << endl
                << "=========================================================="
                << endl;

        if (verbose) cout << "Testing 'hasLimitedSocketCapacity'" << endl;
        {
            Obj mX;  const Obj& X = mX;
            bool hlsc = X.hasLimitedSocketCapacity();
            LOOP_ASSERT(hlsc, true == hlsc);
        }

        // TBD: Complete
        if (verbose) cout << "Testing 'canRegisterSockets'" << endl;
        {
            Obj mX;  const Obj& X = mX;
            if (veryVerbose) { P(Obj::BTESO_MAX_NUM_HANDLES); }

            int errorCode = 0;
            bteso_SocketHandle::Handle handle = 0;
            for (; handle < Obj::BTESO_MAX_NUM_HANDLES - 1; ++handle) {

                if (veryVerbose) { P(handle) }

                ASSERT(mX.canRegisterSockets());

                bdef_Function<void (*)()> cb1, cb2;
                int rc = mX.registerSocketEvent(
                                           (bteso_SocketHandle::Handle) handle,
                                           bteso_EventType::BTESO_READ,
                                           cb1);
                ASSERT(!rc);

                rc = mX.registerSocketEvent(
                                           (bteso_SocketHandle::Handle) handle,
                                           bteso_EventType::BTESO_WRITE,
                                           cb2);
                ASSERT(!rc);
            }

            ASSERT(handle == Obj::BTESO_MAX_NUM_HANDLES - 1);

            if (verbose) cout << "Negative Testing." << endl;
            {
                bsls_AssertFailureHandlerGuard hG(
                                              bsls_AssertTest::failTestDriver);

                if (veryVerbose) { P(handle) }

                ASSERT(mX.canRegisterSockets());

                bdef_Function<void (*)()> cb1, cb2;
                ASSERT_PASS(mX.registerSocketEvent(
                                           (bteso_SocketHandle::Handle) handle,
                                           bteso_EventType::BTESO_READ,
                                           cb1));

                ASSERT_FAIL(mX.registerSocketEvent(
                                           (bteso_SocketHandle::Handle) handle,
                                           bteso_EventType::BTESO_WRITE,
                                           cb2));

                ASSERT(!mX.canRegisterSockets());
            }
        }
      } break;
      case 16: {
        // -----------------------------------------------------------------
        // TESTING CONCERN: exception set on windows (DRQS 11464834)
        //   Connecting to a port that is not listened on, should trigger a
        //   write/connect event.  On windows, however, it selects the
        //   exception set instead of the write set.
        //
        // Plan:
        //   Create a socket and register its handle on CONNECT with the event
        //   manager.  Attempt a timed connection to a local port that is not
        //   listened on, and verify that the connection fails right away (on
        //   Unix), or that if it would block (on Windows), the event manager
        //   selects an exception set and invokes the appropriate callback.
        //
        // Testing:
        //   CONCERN: EXCEPTION SET ON WINDOWS (DRQS 11464834)
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                << "TESTING exception set on windows (DRQS 11464834)." << endl
                << "=================================================" << endl;

        {
            Obj mX(&timeMetric, &testAllocator);

            bteso_SocketHandle::Handle sendSocket;
            int        errorCode;

            bdef_Function<void (*)()> connectCallback;
            const int NUM_ATTEMPTS = 100;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                errorCode = 0;
                bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                      &sendSocket,
                                      bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
                                      &errorCode);
                ASSERT(0 == errorCode);

                bool hasExecutedCallback = false;
                connectCallback = bdef_BindUtil::bind(&checkDeadConnectCallback
                                                     , sendSocket
                                                     , &hasExecutedCallback);

                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(sendSocket,
                                       bteso_EventType::BTESO_CONNECT,
                                       connectCallback);

                if (0 == bteso_SocketImpUtil::connect(sendSocket,
                                                      bteso_IPv4Address(),
                                                      &errorCode)) {
                    if (veryVerbose)
                        cout << "Connection initiated, attempt: " << i << endl;

                    bdet_TimeInterval deadline = bdetu_SystemTime::now();
                    deadline.addMilliseconds(i % 10);
                    deadline.addNanoseconds(i % 1000);

                    if (veryVerbose)
                        cout << "Waiting for registered event until: "
                             << deadline << endl;

                    ASSERT(0 == errorCode);
                    LOOP_ASSERT(i, 1 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));
                    LOOP_ASSERT(i, hasExecutedCallback);
                } else {
                    if (veryVerbose)
                        cout << "Connection failed, attempt: " << i << endl;
                }

                errorCode = 0;
                bteso_SocketImpUtil::close(sendSocket, &errorCode);
                ASSERT(0 == errorCode);
            }
        }

      } break;
      case 15: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
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
        //
        // Testing:
        //   int dispatch();
        //   int dispatch(const bdet_TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'dispatch' METHOD." << endl
                                  << "==========================" << endl;

        if (verbose)
            cout << "Standard test for 'dispatch'" << endl
                 << "============================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDispatch(&mX, controlFlag);
        }

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
               {L_, 0, "Dn0,0"                                             },
               {L_, 0, "Dn100,0"                                           },
               {L_, 0, "+0w2; Dn,1"                                        },
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"                 },
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"              },
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"       },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                       "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8"},
               {L_, 0, "+2r3; Dn100,0; +2w40; Dn100,1;  W2,3000; Dn100,2"  },
               {L_, 0, "+0w20; +0r12; Dn0,1; W0,30; +1w6; +2w8; Dn100,4"   },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; Dn100,4;"
                        "W0,60; W1,70; +1r6; W2,60; W3,60; +2r8; +3r10;"
                        "Dn120,8"                                          },
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
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
        if (verbose)
            cout << "Verifying behavior on timeout (no sockets)." << endl;
        {
            const int NUM_ATTEMPTS = 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));

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
            bdef_Function<void (*)()> nullFunctor;
            const int NUM_ATTEMPTS = 5000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(socketPair.observedFd(),
                                       bteso_EventType::BTESO_READ,
                                       nullFunctor);

                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                     bteso_Flag::BTESO_ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
#endif
      } break;
      case 14: {
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
        //
        // Testing:
        //   void deregisterAll();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterAll'" << endl
                                  << "=======================" << endl;
        if (verbose)
            cout << "\tStandard test for 'deregisterAll'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);

            bteso_EventManagerTester::testDeregisterAll(&mX, controlFlag);
        }

      } break;
      case 13: {
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
        //
        // Testing:
        //   int deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocket'" << endl
                                  << "==========================" << endl;
        {
#ifndef BSLS_PLATFORM__CPU_64_BIT
            // This test fails on 64 bit platforms.  @FIXME @TODO @TBD

            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocket(&mX, controlFlag);
#endif
        }

      } break;

      case 12: {
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
        //
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
                                                                controlFlag);
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
               {L_, 0, "+0w; -0w; T0"          },
               {L_, 0, "+0w; +0r; -0w; E0r; T1"},
               {L_, 0, "+0w; +1r; -0w; E1r; T1"},
               {L_, 0, "+0w; +1r; -1r; E0w; T1"},
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
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }

      } break;
      case 11: {
        // -----------------------------------------------------------------
        // TESTING 'registerSocketEvent' FUNCTION:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function o 'bteso_EventManagerTester', where a
        //   number of socket pairs are created to test the
        //   registerSocketEvent() in this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        //
        // Testing:
        //   void registerSocketEvent();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'registerSocketEvent'" << endl
                                  << "=============================" << endl;
        if (verbose)
            cout << "\tStandard test for 'registerSocketEvent'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);

            bteso_EventManagerTester::testRegisterSocketEvent(&mX,
                                                              controlFlag);
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
               {L_, 0, "+0w; E0w; T1"                      },
               {L_, 0, "+0r; E0r; T1"                      },
               {L_, 0, "+0w; +0w; E0w; T1"                 },
               {L_, 0, "+0r; +0r; E0r; T1"                 },
               {L_, 0, "+0w; +0w; +0r; +0r; E0rw; T2"      },
               {L_, 0, "+0w; +1r; E0w; E1r; T2"            },
               {L_, 0, "+0w; +1r; +1w; +0r; E0rw; E1rw; T4"},
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
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 10: {
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
        //
        // Testing:
        //   int isRegistered();
        //   int numEvents() const;
        //   int numSocketEvents();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING ACCESSORS" << endl
                                  << "=================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testAccessors(&mX, controlFlag);
        }

      } break;
      case 9: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
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
        //
        // Testing:
        //   int Obj::dispatch();
        //   int Obj::dispatch(
        //                                      const bdet_TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'dispatch'" << endl
                 << "==================" << endl;;

        if (verbose)
            cout << "\tStandard test for 'dispatch'" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDispatch(&mX, controlFlag);
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
               {L_, 0, "Dn0,0"                                             },
               {L_, 0, "Dn100,0"                                           },
               {L_, 0, "+0w2; Dn,1"                                        },
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"                 },
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"              },
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"       },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                       "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8"},
               {L_, 0, "W2,8192; +2r3; Dn2000,1; +2w40; Dn2000,2"          },
               {L_, 0, "+0w20; +0r12; Dn0,1; W0,30; +1w6; +2w8; Dn100,4"   },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; Dn100,4;"
                        "W0,60; W1,70; +1r6; W2,60; W3,60; +2r8; +3r10;"
                        "Dn120,8"                                          },
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;
                enum { NUM_PAIRS = 4 };
                bteso_EventManagerTestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                         SCRIPTS[i].d_script,
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
        if (verbose)
            cout << "\tVerifying behavior on timeout (no sockets)." << endl;
        {
            const int NUM_ATTEMPTS = 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));

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
            bdef_Function<void (*)()> nullFunctor;
            const int NUM_ATTEMPTS = 5000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                RawEventManagerTest mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(socketPair.observedFd(),
                                       bteso_EventType::BTESO_READ,
                                       nullFunctor);

                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));

                bdet_TimeInterval now = bdetu_SystemTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVeryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
#endif
      } break;
      case 8: {
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
        //
        // Testing:
        //   void Obj::deregisterAll();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'deregisterAll'" << endl
                 << "=======================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterAll(&mX, controlFlag);
        }

      } break;
      case 7: {
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
        //
        // Testing:
        //   int Obj::deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'deregisterSocket'" << endl
                 << "==========================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocket(&mX, controlFlag);
        }

      } break;
      case 6: {
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
        //
        // Testing:
        //   void Obj::deregisterSocketEvent();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'deregisterSocketEvent'" << endl
                 << "===============================" << endl;
        if (verbose)
            cout << "\tStandard test for 'deregisterSocketEvent'" << endl;

        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testDeregisterSocketEvent(&mX,
                                                                controlFlag);
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
               {L_, 0, "+0w; -0w; T0"          },
               {L_, 0, "+0w; +0r; -0w; E0r; T1"},
               {L_, 0, "+0w; +1r; -0w; E1r; T1"},
               {L_, 0, "+0w; +1r; -1r; E0w; T1"},
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
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }

      } break;
      case 5: {
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
        //
        // Testing:
        //   void Obj::registerSocketEvent();
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'registerSocketEvent'" << endl
                 << "=============================" << endl;

        if (verbose)
            cout << "\tStandard test for 'registerSocketEvent'" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testRegisterSocketEvent(&mX,
                                                              controlFlag);
        }

        if (verbose)
            cout << "\tCustom test for 'registerSocketEvent'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "+0w; E0w; T1"                      },
               {L_, 0, "+0r; E0r; T1"                      },
               {L_, 0, "+0w; +0w; E0w; T1"                 },
               {L_, 0, "+0r; +0r; E0r; T1"                 },
               {L_, 0, "+0w; +0w; +0r; +0r; E0rw; T2"      },
               {L_, 0, "+0w; +1r; E0w; E1r; T2"            },
               {L_, 0, "+0w; +1r; +1w; +0r; E0rw; E1rw; T4"},
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
                                                         controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }

      } break;
      case 4: {
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
        //
        // Testing:
        //   int Obj::isRegistered();
        //   int Obj::numEvents() const;
        //   int Obj::numSocketEvents();
        // -----------------------------------------------------------------
        if (verbose)
              cout << endl
                   << "TESTING ACCESSORS" << endl
                   << "=================" << endl;
        {
            RawEventManagerTest mX(&timeMetric, &testAllocator);
            bteso_EventManagerTester::testAccessors(&mX, controlFlag);
        }

      } break;
      case 3: {
        // -----------------------------------------------------------------
        // VERIFYING ASSUMPTIONS REGARDING 'select' SYSTEM CALL
        // Concerns:
        //    o the return value of and the effect on the fd sets of
        //      'select' system call when a single socket is both readable
        //      and writeable simultaneously.
        //    o adding handles to an empty fd_set and then removing them
        //      results in binary-equal empty fd_set.
        //
        // Plan:
        //   Create a set of (connected) socket pairs, register certain
        //   endpoints with the 'select' call and use the peer endpoints
        //   control.
        //
        // Testing:
        //   ::select();
        // -----------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "VERIFYING 'select' SYSTEM CALL." << endl
                 << "===============================" << endl;

        if (veryVerbose) cout << "\tInitializing socket pairs." << endl;

        enum {
            NUM_PAIRS = (32 > (FD_SETSIZE - 3) / 2
                      ? (FD_SETSIZE  -3) / 2
                      : 30),
            NUM_TESTS = 5
        };
        if (veryVerbose) {
            P(NUM_PAIRS);
            P(NUM_TESTS);
            P(FD_SETSIZE);
        }

        for (int t = 0; t < NUM_TESTS; ++t) {
     // v---

        bteso_EventManagerTestPair testPairs[NUM_PAIRS];
        for (int i = 0; i < NUM_PAIRS; i++) {
            testPairs[i].setObservedBufferOptions(BUF_LEN, 1);
            testPairs[i].setControlBufferOptions(BUF_LEN, 1);
        }

        if (veryVerbose) cout << "\tAddressing concern #1." << endl;

        for (int i = 0; i < NUM_PAIRS; ++i)
        {
            enum { BUF_SIZE = 128 };
            char buffer[BUF_SIZE];
            int errCode = 0;
            int numWritten =
                bteso_SocketImpUtil::write(testPairs[i].controlFd(), 
                                           buffer,
                                           BUF_SIZE,   
                                           &errCode);

            ASSERT(BUF_SIZE == numWritten);
            if (BUF_SIZE != numWritten) {
                P_(numWritten);
                P(errCode);
            }

            fd_set readSet, writeSet;
#ifdef BTESO_PLATFORM__BSD_SOCKETS
            LOOP_ASSERT(i, testPairs[i].observedFd() < FD_SETSIZE);
#endif
            Obj mX(0, &testAllocator);

            struct timeval tv;
            tv.tv_sec = 5;    // wait for up to 5 seconds
            tv.tv_usec = 0;
            int maxFd = 0;

#ifdef BTESO_PLATFORM__BSD_SOCKETS
            maxFd = testPairs[i].observedFd() + 1;
#endif

// TBD: Uncomment
//             LOOP_ASSERT(i, mX.canBeRegistered(testPairs[i].observedFd()));
            FD_ZERO(&readSet); FD_ZERO(&writeSet);
            FD_SET(testPairs[i].observedFd(), &readSet);
            FD_SET(testPairs[i].observedFd(), &writeSet);

            // Fix test failure (DRQS 16736357). Ensure the data written above
            // is ready to be read, by calling select and not providing a write
            // set. 
            int selectReturnValue = ::select(maxFd, 
                                             &readSet, 
                                             NULL,
                                             NULL,
                                             &tv);

            ASSERT(1 == selectReturnValue);
            LOOP_ASSERT(i, FD_ISSET(testPairs[i].observedFd(), &readSet));

            FD_ZERO(&readSet); FD_ZERO(&writeSet);
            FD_SET(testPairs[i].observedFd(), &readSet);
            FD_SET(testPairs[i].observedFd(), &writeSet);

            // Test a socket that is ready for both read and write operations
            // (concern #1).
            selectReturnValue = ::select(maxFd, 
                                         &readSet, 
                                         &writeSet, 
                                         NULL,
                                         &tv);
            if (veryVerbose) {
                cout << "\t\t"; P(selectReturnValue);
            }

            LOOP_ASSERT(i, 2 == selectReturnValue);
            LOOP_ASSERT(i, FD_ISSET(testPairs[i].observedFd(), &readSet));
            LOOP_ASSERT(i, FD_ISSET(testPairs[i].observedFd(), &writeSet));
        }

        if (veryVerbose) cout << "\tAddressing concern #2." << endl;

        for (int i = 0; i < NUM_PAIRS; ++i)
        {
            fd_set testSet;
            fd_set controlSet;
            FD_ZERO(&testSet);
            FD_ZERO(&controlSet);
            for (int j = 0; j <= i; ++j) {
                FD_SET(testPairs[j].controlFd(), &testSet);
            }
            for (int j = 0; j <= i; ++j) {
                FD_CLR(testPairs[j].controlFd(), &testSet);
            }
// TBD: Uncomment
//             LOOP_ASSERT (i, 0 ==
// //--------------------------^
// Obj::compareFdSets(testSet, controlSet));
// //--------------------------^

        }

        if (veryVerbose) cout << "\tAddressing concern #3." << endl;
        {
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            {
                // The behavior of such a system call on UNIX is blocking.
                if (veryVerbose) cout << "\tEmpty sets, no timeout" << endl;
                ASSERT(-1 == ::select(0, NULL, NULL, NULL, NULL));
            }
#endif
            {
                if (veryVerbose) cout << "\tEmpty sets, a valid timeout"
                                      << endl;
                struct timeval tv;
                tv.tv_sec = 1;
                tv.tv_usec = 0;
#ifdef BTESO_PLATFORM__WIN_SOCKETS
                ASSERT(-1 == ::select(0, NULL, NULL, NULL, &tv));
#endif
#ifdef BTESO_PLATFORM__BSD_SOCKETS
                ASSERT(0 == ::select(0, NULL, NULL, NULL, &tv));
#endif
            }
            {
                if (veryVerbose) cout << "\tEmpty sets, an invalid timeout"
                                      << endl;
                struct timeval tv;
                tv.tv_sec = 1;
                tv.tv_usec = -1;
                ASSERT(-1 == ::select(0, NULL, NULL, NULL, &tv));
            }
            {
                if (veryVerbose) cout << "\tTiming out" << endl;
                for (int i = 0; i < NUM_PAIRS; ++i) {
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 50000;  // 50 ms
                    fd_set testSet;
                    FD_ZERO(&testSet);
                    FD_SET(testPairs[i].controlFd(), &testSet);
#ifdef BTESO_PLATFORM__BSD_SOCKETS
                    ASSERT(0 == ::select(testPairs[i].controlFd() + 1,
                                         &testSet,
                                         NULL,
                                         NULL,
                                         &tv));
#endif
#ifdef BTESO_PLATFORM__WIN_SOCKETS
                    ASSERT(0 == ::select(0, &testSet, NULL, NULL, &tv));
#endif
                }
            }
        }
        //--v
        }

      } break;
      case 2: {
        // -----------------------------------------------------------------
        // TESTING 'compareFdSets' and 'canBeRegistered' CLASS METHODS
        // Concerns:
        //   o the function behaves correctly on the sets having the same
        //     elements in the same order
        //   o the function behaves correctly on the sets having the same
        //     elements in different order
        //   o the function behaves correctly on the sets having different
        //     elements
        //
        // Methodology:
        //   Use table-driven approach using integers cast to socket
        //   handles for testing 'compareFdSets'
        //   Use white-box knowledge about different platforms in order
        //   to test 'canBeRegistered' method.
        //
        // Testing:
        //   Obj::compareFdSets(...)
        //   Obj::canBeRegistered(...)
        // -----------------------------------------------------------------

#if 0
        if (verbose) cout << endl
            << "TESTING 'compareFdSets' and 'canBeRegistered' CLASS METHODS."
            << endl
            << "============================================================"
            << endl;

        enum {
            MAX_HANDLES = 100
        };

        struct {
            int                        d_lineNo;
            bteso_SocketHandle::Handle d_set1[MAX_HANDLES];
            int                        d_lengthSet1;
            bteso_SocketHandle::Handle d_set2[MAX_HANDLES];
            int                        d_lengthSet2;
            int                        d_expected;
        } DATA[] = {
            // Summary length  of 0
            { L_, {},        0, {},        0,    0     },

            // Summary length  of 1
            { L_, {1      }, 1, {},        0,    1     },
            { L_, {},        0, {1      }, 1,    1     },

            // Summary length  of 2
            { L_, {},        0, {1, 2   }, 2,    1     },
            { L_, {1, 2   }, 2, {},        0,    1     },
            { L_, {1      }, 1, {1      }, 1,    0     },
            { L_, {0      }, 1, {1      }, 1,    1     },

            // Summary length  of 3
            { L_, {},    0,     {1,2,3  }, 3,    1     },
            { L_, {1      }, 1, {1,2    }, 2,    1     },
            { L_, {1,2    }, 2, {2      }, 1,    1     },
            { L_, {1,2,3  }, 3, {       }, 0,    1     },

            // Summary length  of 4
            { L_, {},    0,     {1,2,3,4}, 4,    1     },
            { L_, {1      }, 1, {1,2,3  }, 3,    1     },
            { L_, {1,2    }, 2, {2,3    }, 2,    1     },
            { L_, {1,2    }, 2, {1,2    }, 2,    0     },
            { L_, {1,2    }, 2, {2,1    }, 2,    0     },
            { L_, {1,2,3  }, 3, {1      }, 1,    1     },
            { L_, {1,2,3  }, 3, {1      }, 1,    1     },
            { L_, {1,2,3,4}, 4, {       }, 0,    1     },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int i = 0; i < NUM_DATA; ++i) {
            fd_set set1, set2;
            FD_ZERO(&set1); FD_ZERO(&set2);
            for (int j = 0; j < DATA[i].d_lengthSet1; ++j) {
                if (veryVerbose) {
                    P_(DATA[i].d_set1[j]);
                }
                FD_SET(DATA[i].d_set1[j], &set1);
            }
            if (veryVerbose) P("");

            for (int j = 0; j < DATA[i].d_lengthSet2; ++j) {
                if (veryVerbose) {
                    P_(DATA[i].d_set2[j]);
                }
                FD_SET(DATA[i].d_set2[j], &set2);
            }
            if (veryVerbose) P("");

            LOOP_ASSERT(DATA[i].d_lineNo, DATA[i].d_expected ==
            Obj::compareFdSets(set1, set2));

        }
        if (verbose)
        cout << "\t Testing 'canBeRegistered' method." << endl;
        {
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            bteso_SocketHandle::Handle testHandle = 0xAB;
            Obj mX(0, &testAllocator);
            ASSERT(1 == mX.canBeRegistered(testHandle));
#endif
#ifdef BTESO_PLATFORM__BSD_SOCKETS
            enum { GOOD_HANDLE = 0, BAD_HANDLE = FD_SETSIZE };
            Obj mX(0, &testAllocator);
            ASSERT(1 == mX.canBeRegistered(GOOD_HANDLE));
            ASSERT(0 == mX.canBeRegistered(BAD_HANDLE));
#endif
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Ensure the basic liveness of an event manager instance.
        //
        // Plan:
        //   Create an object of this event manager under test.  Perform
        //   some basic operations on it.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        typedef Obj ObjUnderTest;
        {
            bdema_Pool pool(sizeof(ObjUnderTest), &testAllocator);

            ObjUnderTest *x = new (pool) ObjUnderTest(&timeMetric,
                                                      &testAllocator);
            pool.deleteObjectRaw(x);
        }
        enum { NUM_TESTS = 10000 };
        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj mX(&timeMetric, &testAllocator);
        }

      } break;
      case -1: {
        // -----------------------------------------------------------------
        // TESTING PERFORMANCE OF 'dispatch' METHOD:
        //   Get performance data.
        //
        // Plan:
        //   Set up multiple connections and register a read event for each
        //   connection, calculate the average time it takes to dispatch a
        //   read event for a given number of registered sockets.
        //
        // Testing:
        //   Obj::dispatch
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                Obj::BTESO_MAX_NUM_HANDLES,
            DEFAULT_NUM_PAIRS        = (MAX_NUM_HANDLES - 4) / 2,
            DEFAULT_NUM_MEASUREMENTS = 10
        };

        int numPairs = DEFAULT_NUM_PAIRS;
        int numMeasurements = DEFAULT_NUM_MEASUREMENTS;

        if (2 < argc) {
            int pairs = atoi(argv[2]);
            if (0 > pairs) {
                verbose = 0;
                numPairs = -pairs;
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERBOSE;
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERY_VERBOSE;
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
                      controlFlag);
            outFile.close();
        }

      } break;
      case -2: {
        // -----------------------------------------------------------------
        // TESTING PERFORMANCE 'registerSocketEvent' METHOD:
        //   Get performance data.
        //
        // Plan:
        //   Open multiple sockets and register a read event for each
        //   socket, calculate the average time taken to register a read
        //   event for a given number of registered read event.
        //
        // Testing:
        //   Obj::registerSocketEvent
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                Obj::BTESO_MAX_NUM_HANDLES,
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERBOSE;
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERY_VERBOSE;
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
                      numPairs, numMeasurements,  controlFlag);
            outFile.close();
        }

      } break;
      case -3: {
        // -----------------------------------------------------------------
        // TESTING PERFORMANCE OF 'dispatch' METHOD:
        //   Get performance data.
        //
        // Plan:
        //   Set up multiple connections and register a read event for each
        //   connection, calculate the average time it takes to dispatch a
        //   read event for a given number of registered sockets.
        //
        // Testing:
        //   'dispatch' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES =
                Obj::BTESO_MAX_NUM_HANDLES,
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERBOSE;
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERY_VERBOSE;
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
                      controlFlag);
            outFile.close();
        }
      } break;
      case -4: {
        // -----------------------------------------------------------------
        // PERFORMANCE TESTING 'registerSocketEvent':
        //   Get performance data.
        //
        // Plan:
        //   Open multiple sockets and register a read event for each
        //   socket, calculate the average time taken to register a read
        //   event for a given number of registered read event.
        //
        // Testing:
        //   'registerSocketEvent' performance data
        // -----------------------------------------------------------------
        enum {
            MAX_NUM_HANDLES          = Obj::BTESO_MAX_NUM_HANDLES,
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERBOSE;
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
                controlFlag &= ~bteso_EventManagerTester::BTESO_VERY_VERBOSE;
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
                      numPairs, numMeasurements,  controlFlag);
            outFile.close();
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == bteso_SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

#endif // !BSLS_PLATFORM__OS_WINDOWS

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
