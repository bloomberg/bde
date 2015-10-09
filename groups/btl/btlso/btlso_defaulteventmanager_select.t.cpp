// btlso_defaulteventmanager_select.t.cpp                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_select.h>
#include <btlso_socketimputil.h>
#include <btlso_timemetrics.h>
#include <btlso_eventmanagertester.h>
#include <btlso_flag.h>
#include <btlso_platform.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>
#include <bdlma_pool.h>
#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bdlf_bind.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Test the corresponding event manager component by using
// 'btlso::EventManagerTester' to exercise the "standard" test which applies to
// any event manager's test.
//
// Since differences exist in implementation between different event manager
// components, a "customized" test is also given for this event manager.  The
// "customized" test is implemented by utilizing the same script grammar and
// the same script interpreting defined in 'btlso::EventManagerTester' function
// but a new set of data to test this specific event manager component.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btlso::DefaultEventManager
// [ 2] ~btlso::DefaultEventManager
//
// MANIPULATORS
// [10] registerSocketEvent
// [11] deregisterSocketEvent
// [12] deregisterSocket
// [13] deregisterAll
// [14] dispatch
//
// ACCESSORS
// [16] canRegisterSockets
// [16] hasLimitedSocketCapacity
// [ 9] numSocketEvents
// [ 9] numEvents
// [ 9] isRegistered
//-----------------------------------------------------------------------------
// [17] USAGE
// [15] TESTING CONCERN: exception set on windows
// [ 1] BREATHING TEST
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

typedef btlso::DefaultEventManager<btlso::Platform::SELECT> Obj;


enum {
    FAIL    = -1,
    SUCCESS = 0
};

enum {
    MAX_SCRIPT = 50,
    MAX_PORT   = 50,
    BUF_LEN    = 8192
};

#if defined(BSLS_PLATFORM_OS_WINDOWS)
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

void checkDeadConnectCallback(btlso::SocketHandle::Handle  handle,
                              bool                       *hasExecutedFlag)
    // Check, for case 16, that the connection status of the specified 'handle'
    // is not good, and set 'hasExecutedFlag' to 'true'.
{
    btlso::IPv4Address result;
    ASSERT(0 != btlso::SocketImpUtil::getPeerAddress(&result, handle));
    if (hasExecutedFlag) {
        *hasExecutedFlag = true;
    }
}

static void genericCb(btlso::EventType::Type       event,
                      btlso::SocketHandle::Handle  socket,
                      int                          bytes,
                      btlso::EventManager         *)
{
    // User specified callback function that will be called after an event is
    // dispatched to do the "real" things.  This callback is only used in the
    // 'usage example' test case, and will be copied to the head file as a part
    // of the usage example.

    enum {
        k_MAX_READ_SIZE  = 8192,
        k_MAX_WRITE_SIZE = WRITE_SIZE
    };

    switch (event) {
      case btlso::EventType::e_READ: {
          ASSERT(0 < bytes);
          char buffer[k_MAX_READ_SIZE];

          int rc = btlso::SocketImpUtil::read(buffer, socket, bytes, 0);
          ASSERT(0 < rc);

      } break;
      case btlso::EventType::e_WRITE: {
          char wBuffer[k_MAX_WRITE_SIZE];
          ASSERT(0 < bytes);
          ASSERT(k_MAX_WRITE_SIZE >= bytes);
          memset(wBuffer,'4', bytes);
          int rc = btlso::SocketImpUtil::write(socket, &wBuffer, bytes, 0);
          ASSERT(0 < rc);
      } break;
      case btlso::EventType::e_ACCEPT: {
          int errCode;
          int rc = btlso::SocketImpUtil::close(socket, &errCode);
          ASSERT(0 == rc);
      } break;
      case btlso::EventType::e_CONNECT: {
          int errCode = 0;
          btlso::SocketImpUtil::close(socket, &errCode);
          ASSERT(0 == errCode);
      } break;
      default: {
          ASSERT("Invalid event code" && 0);
      } break;
    }
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
#if 0 && defined(BSLS_PLATFORM_OS_WINDOWS)
    cout << "Test driver disabled\n";

    testStatus = -1;
#else

    int controlFlag = 0;
    if (veryVeryVerbose) {
        controlFlag |= btlso::EventManagerTester::k_VERY_VERY_VERBOSE;
    }
    if (veryVerbose) {
        controlFlag |= btlso::EventManagerTester::k_VERY_VERBOSE;
    }
    if (verbose) {
        controlFlag |= btlso::EventManagerTester::k_VERBOSE;
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    ASSERT(0 == btlso::SocketImpUtil::startup());
    bslma::TestAllocator testAllocator(veryVeryVerbose);
    btlso::TimeMetrics timeMetric(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                                 btlso::TimeMetrics::e_CPU_BOUND);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using an event manager
///- - - - - - - - - - - - - - - - -
        btlso::TimeMetrics timeMetric(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                                      btlso::TimeMetrics::e_CPU_BOUND);

        btlso::DefaultEventManager<btlso::Platform::SELECT> mX(&timeMetric);

        btlso::SocketHandle::Handle socket[2];

        int rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                        socket,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);

        ASSERT(0 == rc);

        int numBytes = 5;
        btlso::EventManager::Callback readCb(
                                 bdlf::BindUtil::bind(&genericCb,
                                                      btlso::EventType::e_READ,
                                                      socket[0],
                                                      numBytes,
                                                      &mX));

        mX.registerSocketEvent(socket[0], btlso::EventType::e_READ, readCb);

        numBytes = 25;
        btlso::EventManager::Callback writeCb1(
                                bdlf::BindUtil::bind(&genericCb,
                                                     btlso::EventType::e_WRITE,
                                                     socket[0],
                                                     numBytes,
                                                     &mX));
        mX.registerSocketEvent(socket[0], btlso::EventType::e_WRITE, writeCb1);

        numBytes = 15;
        btlso::EventManager::Callback writeCb2(
                                bdlf::BindUtil::bind(&genericCb,
                                                     btlso::EventType::e_WRITE,
                                                     socket[1],
                                                     numBytes,
                                                     &mX));
        mX.registerSocketEvent(socket[1], btlso::EventType::e_WRITE, writeCb2);

        ASSERT(3 == mX.numEvents());
        ASSERT(2 == mX.numSocketEvents(socket[0]));
        ASSERT(1 == mX.numSocketEvents(socket[1]));
        ASSERT(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
        ASSERT(1 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
        ASSERT(1 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));

        int flags = 0;
        bsls::TimeInterval deadline(bdlt::CurrentTime::now());
        deadline += 5;    // timeout 5 seconds from now.

        rc = mX.dispatch(deadline, flags);   ASSERT(2 == rc);

        mX.deregisterSocketEvent(socket[0], btlso::EventType::e_WRITE);
        ASSERT(2 == mX.numEvents());
        ASSERT(1 == mX.numSocketEvents(socket[0]));
        ASSERT(1 == mX.numSocketEvents(socket[1]));
        ASSERT(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
        ASSERT(1 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
        ASSERT(1 == mX.deregisterSocket(socket[1]));
        ASSERT(1 == mX.numEvents());
        ASSERT(1 == mX.numSocketEvents(socket[0]));
        ASSERT(0 == mX.numSocketEvents(socket[1]));
        ASSERT(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
        ASSERT(0 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));

        mX.deregisterAll();
        ASSERT(0 == mX.numEvents());
        ASSERT(0 == mX.numSocketEvents(socket[0]));
        ASSERT(0 == mX.numSocketEvents(socket[1]));
        ASSERT(0 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
        ASSERT(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
        ASSERT(0 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
      } break;

      case 17: {
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

        Obj mX;

        btlso::SocketHandle::Handle socket[2];

        int rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                             socket, btlso::SocketImpUtil::k_SOCKET_STREAM);
        ASSERT(0 == rc);

        bsl::function<void()> deregisterCallback(
                bdlf::MemFnUtil::memFn(&Obj::deregisterAll, &mX));

        ASSERT(0 == mX.registerSocketEvent(socket[0],
                                           btlso::EventType::e_READ,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[0],
                                           btlso::EventType::e_WRITE,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[1],
                                           btlso::EventType::e_READ,
                                           deregisterCallback));
        ASSERT(0 == mX.registerSocketEvent(socket[1],
                                           btlso::EventType::e_WRITE,
                                           deregisterCallback));

        char wBuffer[NUM_BYTES];
        memset(wBuffer,'4', NUM_BYTES);
        rc = btlso::SocketImpUtil::write(socket[0], &wBuffer, NUM_BYTES, 0);
        ASSERT(0 < rc);
        rc = btlso::SocketImpUtil::write(socket[1], &wBuffer, NUM_BYTES, 0);
        ASSERT(0 < rc);

        ASSERT(1 == mX.dispatch(bsls::TimeInterval(1.0), 0));

      } break;

      case 16: {
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

        if (verbose) cout << "Testing 'canRegisterSockets'" << endl;
        {
            Obj mX;
            if (veryVerbose) { P(Obj::k_MAX_NUM_HANDLES); }

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            const int MAX_HANDLES = Obj::k_MAX_NUM_HANDLES - 1;
#else
            const int MAX_HANDLES = Obj::k_MAX_NUM_HANDLES;
#endif

            btlso::SocketHandle::Handle handle = 0;
            for (; handle < MAX_HANDLES; ++handle) {

                if (veryVerbose) { P(handle) }

                ASSERT(mX.canRegisterSockets());

                bsl::function<void()> cb1, cb2;
                int rc = mX.registerSocketEvent(
                                          (btlso::SocketHandle::Handle) handle,
                                           btlso::EventType::e_READ,
                                           cb1);
                ASSERT(!rc);

                rc = mX.registerSocketEvent(
                                          (btlso::SocketHandle::Handle) handle,
                                           btlso::EventType::e_WRITE,
                                           cb2);
                ASSERT(!rc);
            }

            ASSERT(handle == MAX_HANDLES);

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            bsl::function<void()> cb1, cb2;
            int rc = mX.registerSocketEvent(
                                          (btlso::SocketHandle::Handle) handle,
                                           btlso::EventType::e_READ,
                                           cb1);
            ASSERT(!rc);
#endif

            if (verbose) cout << "Negative Testing." << endl;
            {
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                if (veryVerbose) { P(handle) }

                ASSERT(!mX.canRegisterSockets());

                bsl::function<void()> cb1, cb2;
                ASSERT_FAIL(mX.registerSocketEvent(
                                          (btlso::SocketHandle::Handle) handle,
                                           btlso::EventType::e_READ,
                                           cb1));

                ASSERT_FAIL(mX.registerSocketEvent(
                                          (btlso::SocketHandle::Handle) handle,
                                           btlso::EventType::e_WRITE,
                                           cb2));

                ASSERT(!mX.canRegisterSockets());
            }
        }
      } break;
      case 15: {
        // -----------------------------------------------------------------
        // TESTING CONCERN: exception set on windows
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
        //   CONCERN: EXCEPTION SET ON WINDOWS
        // -----------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING exception set on windows." << endl
                          << "=================================" << endl;

        {
            Obj mX(&timeMetric, &testAllocator);

            btlso::SocketHandle::Handle sendSocket;
            int        errorCode;

            bsl::function<void()> connectCallback;
            const int NUM_ATTEMPTS = 100;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                errorCode = 0;
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                      &sendSocket,
                                      btlso::SocketImpUtil::k_SOCKET_STREAM,
                                      &errorCode);
                ASSERT(0 == errorCode);

                bool hasExecutedCallback = false;
                connectCallback = bdlf::BindUtil::bind(
                                                      &checkDeadConnectCallback
                                                      , sendSocket
                                                      , &hasExecutedCallback);

                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(sendSocket,
                                       btlso::EventType::e_CONNECT,
                                       connectCallback);

                if (0 == btlso::SocketImpUtil::connect(sendSocket,
                                                       btlso::IPv4Address(),
                                                       &errorCode)) {
                    if (veryVerbose)
                        cout << "Connection initiated, attempt: " << i << endl;

                    bsls::TimeInterval deadline = bdlt::CurrentTime::now();
                    deadline.addMilliseconds(i % 10);
                    deadline.addNanoseconds(i % 1000);

                    if (veryVerbose)
                        cout << "Waiting for registered event until: "
                             << deadline << endl;

                    ASSERT(0 == errorCode);
                    LOOP_ASSERT(i, 1 == mX.dispatch(
                                           deadline,
                                           btlso::Flag::k_ASYNC_INTERRUPT));
                    LOOP_ASSERT(i, hasExecutedCallback);
                } else {
                    if (veryVerbose)
                        cout << "Connection failed, attempt: " << i << endl;
                }

                errorCode = 0;
                btlso::SocketImpUtil::close(sendSocket, &errorCode);
                ASSERT(0 == errorCode);
            }
        }

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
        //   corresponding test function of 'btlso::EventManagerTester', where
        //   multiple socket pairs are created to test the dispatch() in
        //   this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'btlso::EventManagerTester', call the script interpreting function
        //   gg() of 'btlso::EventManagerTester' to execute the test data.
        // Exhausting test:
        //   Test the "timeout" from the dispatch() with the loop-driven
        //   implementation where timeout value are generated during each
        //   iteration and invoke the dispatch() with it.
        //
        // Testing:
        //   int dispatch();
        //   int dispatch(const bsls::TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'dispatch' METHOD." << endl
                                  << "==========================" << endl;

        if (verbose)
            cout << "Standard test for 'dispatch'" << endl
                 << "============================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDispatch(&mX, controlFlag);
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

                btlso::EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
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
            const int NUM_ATTEMPTS = 50;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                bsls::TimeInterval deadline = bdlt::CurrentTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           btlso::Flag::k_ASYNC_INTERRUPT));

                bsls::TimeInterval now = bdlt::CurrentTime::now();
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
            btlso::EventManagerTestPair socketPair;
            bsl::function<void()> nullFunctor;
            const int NUM_ATTEMPTS = 50;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(socketPair.observedFd(),
                                       btlso::EventType::e_READ,
                                       nullFunctor);

                bsls::TimeInterval deadline = bdlt::CurrentTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
                     btlso::Flag::k_ASYNC_INTERRUPT));

                bsls::TimeInterval now = bdlt::CurrentTime::now();
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
        //   corresponding test function of 'btlso::EventManagerTester', where
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

            btlso::EventManagerTester::testDeregisterAll(&mX, controlFlag);
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
        //   corresponding test function of 'btlso::EventManagerTester', where
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDeregisterSocket(&mX, controlFlag);
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
        //   corresponding test function of 'btlso::EventManagerTester', where
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
            btlso::EventManagerTester::testDeregisterSocketEvent(&mX,
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

                btlso::EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
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
        // TESTING 'registerSocketEvent' FUNCTION:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function o 'btlso::EventManagerTester', where a
        //   number of socket pairs are created to test the
        //   registerSocketEvent() in this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'btlso::EventManagerTester', call the script interpreting function
        //   gg() of 'btlso::EventManagerTester' to execute the test data.
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

            btlso::EventManagerTester::testRegisterSocketEvent(&mX,
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

                btlso::EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }
                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                         SCRIPTS[i].d_script,
                                                         controlFlag);

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
        //   corresponding function of 'btlso::EventManagerTester', where a
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
            btlso::EventManagerTester::testAccessors(&mX, controlFlag);
        }

      } break;
      case 8: {
        // -----------------------------------------------------------------
        // TESTING 'dispatch' FUNCTION:
        //   The goal is to ensure that 'dispatch' invokes the callback
        //   method for the write socket handle and event, for all possible
        //   events.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'btlso::EventManagerTester', where
        //   multiple socket pairs are created to test the dispatch() in
        //   this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'btlso::EventManagerTester', call the script interpreting function
        //   gg() of 'btlso::EventManagerTester' to execute the test data.
        // Exhausting test:
        //   Test the "timeout" from the dispatch() with the loop-driven
        //   implementation where timeout value are generated during each
        //   iteration and invoke the dispatch() with it.
        //
        // Testing:
        //   int Obj::dispatch();
        //   int Obj::dispatch(const bsls::TimeInterval&, ...);
        // -----------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING 'dispatch'" << endl
                 << "==================" << endl;;

        if (verbose)
            cout << "\tStandard test for 'dispatch'" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDispatch(&mX, controlFlag);
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
                Obj mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;
                enum { NUM_PAIRS = 4 };
                btlso::EventManagerTestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
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
            const int NUM_ATTEMPTS = 50;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                bsls::TimeInterval deadline = bdlt::CurrentTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           btlso::Flag::k_ASYNC_INTERRUPT));

                bsls::TimeInterval now = bdlt::CurrentTime::now();
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
            btlso::EventManagerTestPair socketPair;
            bsl::function<void()> nullFunctor;
            const int NUM_ATTEMPTS = 50;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(socketPair.observedFd(),
                                       btlso::EventType::e_READ,
                                       nullFunctor);

                bsls::TimeInterval deadline = bdlt::CurrentTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           btlso::Flag::k_ASYNC_INTERRUPT));

                bsls::TimeInterval now = bdlt::CurrentTime::now();
                LOOP_ASSERT(i, deadline <= now);

                if (veryVeryVerbose) {
                    P_(deadline); P(now);
                }
            }
        }
      } break;
      case 7: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterAll' FUNCTION:
        //   It must be verified that the application of 'deregisterAll'
        //   from any state returns the event manager.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'btlso::EventManagerTester', where
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDeregisterAll(&mX, controlFlag);
        }

      } break;
      case 6: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocket' FUNCTION:
        //   All possible transitions from other state to 0 must be
        //   exhaustively tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'btlso::EventManagerTester', where
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDeregisterSocket(&mX, controlFlag);
        }

      } break;
      case 5: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocketEvent' FUNCTION:
        //   All possible deregistration transitions must be exhaustively
        //   tested.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding test function of 'btlso::EventManagerTester', where
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDeregisterSocketEvent(&mX,
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

                btlso::EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
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
        // TESTING 'registerSocketEvent' FUNCTION:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'btlso::EventManagerTester', where a
        //   number of socket pairs are created to test the
        //   registerSocketEvent() in this event manager.
        // Customized test:
        //   Create an object of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'btlso::EventManagerTester', call the script interpreting function
        //   gg() of 'btlso::EventManagerTester' to execute the test data.
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testRegisterSocketEvent(&mX,
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
                Obj mX(&timeMetric, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                btlso::EventManagerTestPair socketPairs[4];

                const int NUM_PAIR =
                               sizeof socketPairs / sizeof socketPairs[0];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                          SCRIPTS[i].d_script,
                                                          controlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 3: {
        // -----------------------------------------------------------------
        // TESTING ACCESSORS:
        //   The main concern about this function is to ensure full coverage
        //   of the every legal event combination that can be registered for
        //   one and two sockets.
        //
        // Plan:
        // Standard test:
        //   Create an object of the event manager under test, call the
        //   corresponding function of 'btlso::EventManagerTester', where a
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
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testAccessors(&mX, controlFlag);
        }

      } break;
      case 2: {
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

        btlso::EventManagerTestPair testPairs[NUM_PAIRS];
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
                btlso::SocketImpUtil::write(testPairs[i].controlFd(),
                                           buffer,
                                           BUF_SIZE,
                                           &errCode);

            ASSERT(BUF_SIZE == numWritten);
            if (BUF_SIZE != numWritten) {
                P_(numWritten);
                P(errCode);
            }

            fd_set readSet, writeSet;
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
            LOOP_ASSERT(i, testPairs[i].observedFd() < FD_SETSIZE);
#endif
            Obj mX(0, &testAllocator);

            struct timeval tv;
            tv.tv_sec = 5;    // wait for up to 5 seconds
            tv.tv_usec = 0;
            int maxFd = 0;

#ifdef BTLSO_PLATFORM_BSD_SOCKETS
            maxFd = testPairs[i].observedFd() + 1;
#endif

            FD_ZERO(&readSet); FD_ZERO(&writeSet);
            FD_SET(testPairs[i].observedFd(), &readSet);
            FD_SET(testPairs[i].observedFd(), &writeSet);

            // Fix test failure.  Ensure the data written above is ready to be
            // read, by calling select and not providing a write set.

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
        }

        if (veryVerbose) cout << "\tAddressing concern #3." << endl;
        {
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
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
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
                ASSERT(-1 == ::select(0, NULL, NULL, NULL, &tv));
#endif
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
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
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
                    ASSERT(0 == ::select(testPairs[i].controlFd() + 1,
                                         &testSet,
                                         NULL,
                                         NULL,
                                         &tv));
#endif
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
                    ASSERT(0 == ::select(0, &testSet, NULL, NULL, &tv));
#endif
                }
            }
        }
        //--v
        }

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
            bdlma::Pool pool(sizeof(ObjUnderTest), &testAllocator);

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
        // --------------------------------------------------------------------
        // PERFORMANCE TESTING 'dispatch':
        //   Get the performance data.
        //
        // Plan:
        //   Set up a collection of socketPairs and register one end of all the
        //   pairs with the event manager.  Write 1 byte to
        //   'fracBusy * numSocketPairs' of the connections, and measure the
        //   average time taken to dispatch a read event for a given number of
        //   registered read event.  If 'timeOut > 0' register a timeout
        //   interval with the 'dispatch' call.  If 'R|N' is 'R', actually read
        //   the bytes in the dispatch, if it's 'N', just call a null function
        //   within the dispatch.
        //
        // Testing:
        //   'dispatch' capacity
        //
        // See the compilation of results for all event managers & platforms
        // at the beginning of 'btlso_eventmanagertester.t.cpp'.
        // --------------------------------------------------------------------

        if (verbose) cout << "PERFORMANCE TESTING 'dispatch'\n"
                             "==============================\n";

        if (veryVerbose) P(FD_SETSIZE);

        {
            Obj mX(&timeMetric, &testAllocator);
            btlso::EventManagerTester::testDispatchPerformance(&mX, "select",
                                                                  controlFlag);
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
        //
        // See the compilation of results for all event managers & platforms
        // at the beginning of 'btlso_eventmanagertester.t.cpp'.
        // -----------------------------------------------------------------

        if (verbose) cout << "PERFORMANCE TESTING 'registerSocketEvent'\n"
                             "=========================================\n";

        if (veryVerbose) P(FD_SETSIZE);

        Obj mX(&timeMetric, &testAllocator);
        btlso::EventManagerTester::testRegisterPerformance(&mX, controlFlag);
      } break;
      case -3: {
        // -----------------------------------------------------------------
        // Running 'gg' interactively
        //
        // Concern:
        //   Need to be able to run the 'gg' function interactively to
        //   analyze test failures.
        //
        // Plan:
        //   Iterate, inputting a line from the console, and feeding that
        //   to the 'gg' function.
        // -----------------------------------------------------------------

        while (true) {
            Obj mX(&timeMetric, &testAllocator);

            char script[1000];
            cout << "Script: " << bsl::flush;
            cin.getline(script, sizeof(script));

            if (!bsl::strcmp("quit", script)) {
                break;
            }
            if (!script[0]) {
                continue;
            }

            enum { NUM_PAIR = 10 };
            btlso::EventManagerTestPair socketPairs[NUM_PAIR];

            for (int j = 0; j < NUM_PAIR; j++) {
                socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
            }

            int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     script,
                                                     controlFlag);
            P(fails);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == btlso::SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

#endif // !BSLS_PLATFORM_OS_WINDOWS

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
