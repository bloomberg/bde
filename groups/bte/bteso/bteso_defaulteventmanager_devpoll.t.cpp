// bteso_defaulteventmanager_devpoll.t.cpp      -*-C++-*-
#include <bteso_defaulteventmanager_devpoll.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>
#include <bteso_timemetrics.h>
#include <bteso_eventmanagertester.h>
#include <bteso_platform.h>
#include <bteso_flag.h>
#include <bslma_testallocator.h>
#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>
#include <bcemt_thread.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bsls_platform.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

#include <bsls_assert.h>

using namespace BloombergLP;
#if defined(BSLS_PLATFORM__OS_SOLARIS)
    #define BTESO_EVENTMANAGER_ENABLETEST
    typedef bteso_DefaultEventManager<bteso_Platform::DEVPOLL> Obj;
#endif

#ifdef BTESO_EVENTMANAGER_ENABLETEST

// Include files common to all variants.
#include <fcntl.h>
#include <bsl_c_errno.h>
#include <bsl_c_signal.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <bsl_c_signal.h>
#include <sys/devpoll.h>

using namespace bsl;  // automatically added by script

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              OVERVIEW
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
// [ 9] deregisterSocket
// [ 7] deregisterAll
// [ 8] dispatch
//
// ACCESSORS
// [12] canRegisterSockets
// [12] hasLimitedSocketCapacity
// [ 3] numSocketEvents
// [ 3] numEvents
// [ 3] isRegistered
//-----------------------------------------------------------------------------
// [ 13] USAGE EXAMPLE
// [ 10] SYSTEM INTERFACES ASSUMPTIONS
// [ 1] Breathing test
// [ -1] 'dispatch' PERFORMANCE DATA
// [ -2] 'registerSocketEvent' PERFORMANCE DATA
//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

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

//==========================================================================
// The level of verbosity.
//--------------------------------------------------------------------------
static int globalVerbose, globalVeryVerbose, globalVeryVeryVerbose;

//==========================================================================
// Control byte used to verify reads and writes.
//--------------------------------------------------------------------------
const char control_byte(0x53);

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
// Test success and failure codes.
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
        READ_SIZE = 8192,
        WRITE_SIZE = 73728
    };
#endif

//==========================================================================
//                      HELPER CLASSES
//--------------------------------------------------------------------------

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
      case bteso_EventType::BTESO_READ: {
          ASSERT(0 < bytes);
          char buffer[MAX_READ_SIZE];

          int rc = bteso_SocketImpUtil::read(buffer, socket, bytes, 0);
          ASSERT(0 < rc);

      } break;
      case bteso_EventType::BTESO_WRITE: {
          char wBuffer[MAX_WRITE_SIZE];
          ASSERT(0 < bytes);
          ASSERT(MAX_WRITE_SIZE >= bytes);
          memset(wBuffer,'4', bytes);
          int rc = bteso_SocketImpUtil::write(socket, &wBuffer, bytes, 0);
          ASSERT(0 < rc);
      } break;
      case bteso_EventType::BTESO_ACCEPT: {
          int errCode;
          int rc = bteso_SocketImpUtil::close(socket, &errCode);
          ASSERT(0 == rc);
      } break;
      case bteso_EventType::BTESO_CONNECT: {
          int errCode = 0;
          bteso_SocketImpUtil::close(socket, &errCode);
           ASSERT(0 == errCode);
      } break;
      default: {
          ASSERT("Invalid event code" && 0);
      } break;
    }
}

void assertCb()
{
    BSLS_ASSERT_OPT(0);
}

#endif // BTESO_EVENTMANAGER_ENABLETEST

//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------
int main(int argc, char *argv[]) {
#ifdef BTESO_EVENTMANAGER_ENABLETEST
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;                 globalVerbose = verbose;
    int veryVerbose = argc > 3;         globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4; globalVeryVeryVerbose = veryVeryVerbose;

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bteso_SocketImpUtil::startup();
    bslma_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    bteso_TimeMetrics timeMetric(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
                                 bteso_TimeMetrics::BTESO_CPU_BOUND);

    switch (test) { case 0:
      case 13: {
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
        {
            bteso_TimeMetrics timeMetric(
                                   bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
                                   bteso_TimeMetrics::BTESO_CPU_BOUND);
            bteso_DefaultEventManager<bteso_Platform::DEVPOLL> mX(&timeMetric);

            bteso_SocketHandle::Handle socket[2];

            int rc = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                      socket, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);

            ASSERT(0 == rc);
            int numBytes = 5;
            bteso_EventManager::Callback readCb(
                    bdef_BindUtil::bind( &genericCb
                                       , bteso_EventType::BTESO_READ
                                       , socket[0]
                                       , numBytes
                                       , &mX));

            mX.registerSocketEvent(socket[0], bteso_EventType::BTESO_READ,
                                   readCb);

            numBytes = 25;
            bteso_EventManager::Callback writeCb1(
                    bdef_BindUtil::bind( &genericCb
                                       , bteso_EventType::BTESO_WRITE
                                       , socket[0]
                                       , numBytes
                                       , &mX));

            mX.registerSocketEvent(socket[0], bteso_EventType::BTESO_WRITE,
                                   writeCb1);

            numBytes = 15;
            bteso_EventManager::Callback writeCb2(
                    bdef_BindUtil::bind( &genericCb
                                       , bteso_EventType::BTESO_WRITE
                                       , socket[1]
                                       , numBytes
                                       , &mX));

            mX.registerSocketEvent(socket[1], bteso_EventType::BTESO_WRITE,
                                   writeCb2);

            ASSERT(3 == mX.numEvents());
            ASSERT(2 == mX.numSocketEvents(socket[0]));
            ASSERT(1 == mX.numSocketEvents(socket[1]));
            ASSERT(1 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_READ));
            ASSERT(1 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_WRITE));
            ASSERT(1 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_WRITE));
            int flags = 0;
            bdet_TimeInterval deadline(bdetu_SystemTime::now());
            deadline += 5;    // timeout 5 seconds from now.
            rc = mX.dispatch(deadline, flags);   ASSERT(2 == rc);
            mX.deregisterSocketEvent(socket[0], bteso_EventType::BTESO_WRITE);
            ASSERT(2 == mX.numEvents());
            ASSERT(1 == mX.numSocketEvents(socket[0]));
            ASSERT(1 == mX.numSocketEvents(socket[1]));
            ASSERT(1 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_WRITE));
            ASSERT(1 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_WRITE));
            ASSERT(1 == mX.deregisterSocket(socket[1]));
            ASSERT(1 == mX.numEvents());
            ASSERT(1 == mX.numSocketEvents(socket[0]));
            ASSERT(0 == mX.numSocketEvents(socket[1]));
            ASSERT(1 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_WRITE));
            ASSERT(0 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_WRITE));
            mX.deregisterAll();
            ASSERT(0 == mX.numEvents());
            ASSERT(0 == mX.numSocketEvents(socket[0]));
            ASSERT(0 == mX.numSocketEvents(socket[1]));
            ASSERT(0 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_READ));
            ASSERT(0 == mX.isRegistered(socket[0],
                                        bteso_EventType::BTESO_WRITE));
            ASSERT(0 == mX.isRegistered(socket[1],
                                        bteso_EventType::BTESO_WRITE));
        }
      } break;

      case 12: {
        // -----------------------------------------------------------------
        // TESTING 'canRegisterSockets' and 'hasLimitedSocketCapacity'
        //
        // Concern:
        //: 1 'hasLimitiedSocketCapacity' returns 'false'.
        //:
        //: 2 'canRegisterSockets' always returns 'true'.
        //
        // Plan:
        //: 1 Assert that 'hasLimitedSocketCapacity' returns 'false'.
        //:
        //: 2 Assert that 'canRegisterSockets' returns 'true'.
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
            LOOP_ASSERT(hlsc, false == hlsc);
        }

        if (verbose) cout << "Testing 'canRegisterSockets'" << endl;
        {
            Obj mX;  const Obj& X = mX;
            bool crs = X.canRegisterSockets();
            LOOP_ASSERT(crs, true == crs);
        }
      } break;

      case 11: {
        // --------------------------------------------------------------------
        // DEREGISTERING IN A CALLBACK
        // Concerns:
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

        ASSERT(1 == mX.dispatch(bdet_TimeInterval(1.0), 0));

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // VERIFYING ASSUMPTIONS REGARDING '/dev/poll' MECHANISM
        // Concerns:
        //    o /dev/poll can be opened for writing
        //    o can register multiple socket handles incrementally
        //    o can register multiple socket handles in a single shot
        //    o can deregister multiple socket handles in incrementally
        //    o can deregister multiple socket handles in a single shot
        //    o 'ioctl' signals properly when appropriate socket event occurs
        //    o 'ioctl' signals properly when appropriate socket events occur
        // Methodology:
        //    Create a set of (connected) socket pairs, register certain
        //    endpoints with the 'select' call and use the peer endpoints
        //    control.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "VERIFYING '/dev/poll' MECHANISM" << endl
              << "===============================" << endl;

        if (veryVerbose) cout << "\tInitializing socket pairs." << endl;

        enum { NUM_PAIRS = 10 };

        bteso_EventManagerTestPair testPairs[NUM_PAIRS];
        for (int i = 0; i < NUM_PAIRS; i++) {
            testPairs[i].setObservedBufferOptions(BUF_LEN, 1);
            testPairs[i].setControlBufferOptions(BUF_LEN, 1);
        }

        if (veryVerbose)
            cout << "\tAddressing concern #1: Opening /dev/poll." << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #2: Incremental registration."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                ::pollfd registration;
                registration.fd = testPairs[i].observedFd();
                registration.events = POLLIN;
                registration.revents = 0;      // Just to satisfy purify
                int rc = write(devPollFd, &registration, sizeof(::pollfd));
                LOOP_ASSERT(i, sizeof(::pollfd) == rc);
            }

            if (veryVerbose) {
                cout << "\t\tVerifying correct registration." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                ::pollfd registration;
                registration.fd = testPairs[i].observedFd();
                registration.events = 0;
                registration.revents = 0;      // Just to satisfy purify
                int rc = ioctl(devPollFd, DP_ISPOLLED, &registration);
                LOOP_ASSERT(i, 1 == rc);
                LOOP_ASSERT(i, testPairs[i].observedFd() == registration.fd)
                LOOP_ASSERT(i, POLLIN == registration.revents);
                LOOP_ASSERT(i, 0 == registration.events);
            }
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #3: Single-shot registration."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }
            ::pollfd registrations[NUM_PAIRS];
            for (int i = 0; i < NUM_PAIRS; ++i) {
                registrations[i].fd = testPairs[i].observedFd();
                registrations[i].events = POLLIN;
                registrations[i].revents = 0;      // Just to satisfy purify
            }
            int rc = write(devPollFd, &registrations,
                           NUM_PAIRS * sizeof(::pollfd));
            ASSERT(NUM_PAIRS * sizeof(::pollfd) == rc);

            if (veryVerbose) {
                cout << "\t\tVerifying correct registration." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                ::pollfd registration;
                registration.fd = testPairs[i].observedFd();
                registration.events = 0;
                registration.revents = 0;      // Just to satisfy purify
                int rc = ioctl(devPollFd, DP_ISPOLLED, &registration);
                LOOP_ASSERT(i, 1 == rc);
                LOOP_ASSERT(i, testPairs[i].observedFd() == registration.fd)
                LOOP_ASSERT(i, POLLIN == registration.revents);
                LOOP_ASSERT(i, 0 == registration.events);
            }
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #4: Incremental deregistration."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }

            ::pollfd registrations[NUM_PAIRS];
            for (int i = 0; i < NUM_PAIRS; ++i) {
                registrations[i].fd = testPairs[i].observedFd();
                registrations[i].events = POLLIN;
                registrations[i].revents = 0;      // Just to satisfy purify
            }
            int rc = write(devPollFd, &registrations,
                           NUM_PAIRS * sizeof(::pollfd));
            ASSERT(NUM_PAIRS * sizeof(::pollfd) == rc);

            if (veryVerbose) {
                cout << "\t\tDeregistering and verifying correctness." << endl;
            }
            for (int i = 0;  i < NUM_PAIRS; ++i) {
                int idx = NUM_PAIRS - 1 - i;
                ::pollfd req;
                req.fd = testPairs[idx].observedFd();
                req.events = POLLREMOVE;
                req.revents = 0;
                int rc = write(devPollFd, &req, sizeof(::pollfd));
                LOOP_ASSERT(i, sizeof(::pollfd) == rc);

                for (int j = idx; j < NUM_PAIRS; ++j) {
                    ::pollfd status;
                    status.fd = testPairs[j].observedFd();
                    status.events = POLLIN;
                    status.revents = 0;
                    int rc = ioctl(devPollFd, DP_ISPOLLED, &status);
                    LOOP2_ASSERT(i, j, 0 == rc);
                }

                for (int j = 0; j < idx; ++j) {
                    ::pollfd status;
                    status.fd = testPairs[j].observedFd();
                    status.events = POLLIN;
                    status.revents = 0;
                    int rc = ioctl(devPollFd, DP_ISPOLLED, &status);
                    LOOP2_ASSERT(i, j, 1 == rc);
                }
            }
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #5: Single-shot deregistration."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                ::pollfd registration;
                registration.fd = testPairs[i].observedFd();
                registration.events = POLLIN;
                registration.revents = 0;      // Just to satisfy purify
                int rc = write(devPollFd, &registration, sizeof(::pollfd));
                LOOP_ASSERT(i, sizeof(::pollfd) == rc);
            }

            if (veryVerbose) {
                cout << "\t\tDeregistering and verifying correctness."
                     << endl;
            }
            ::pollfd registrations[NUM_PAIRS];
            for (int i = 0; i < NUM_PAIRS; ++i) {
                registrations[i].fd = testPairs[i].observedFd();
                registrations[i].events = POLLREMOVE;
                registrations[i].revents = 0;
            }
            int rc = write(devPollFd, &registrations,
                           NUM_PAIRS * sizeof(::pollfd));
            ASSERT(NUM_PAIRS * sizeof(::pollfd) == rc);

            for (int i = 0; i < NUM_PAIRS; ++i) {
                ::pollfd registration;
                registration.fd = testPairs[i].observedFd();
                registration.events = 0;
                registration.revents = 0;      // Just to satisfy purify
                int rc = ioctl(devPollFd, DP_ISPOLLED, &registration);
                LOOP_ASSERT(i, 0 == rc);
            }
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #6: 'ioctl' behavior."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }

            ::pollfd registrations[NUM_PAIRS];
            for (int i = 0; i < NUM_PAIRS; ++i) {
                registrations[i].fd = testPairs[i].observedFd();
                registrations[i].events = POLLIN;
                registrations[i].revents = 0;      // Just to satisfy purify
            }
            int rc = write(devPollFd, &registrations,
                           NUM_PAIRS * sizeof(::pollfd));
            ASSERT(NUM_PAIRS * sizeof(::pollfd) == rc);

            if (veryVerbose) {
                cout << "\t\tSignalling and verifying correctness." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                const char controlByte = 0xAB;
                LOOP_ASSERT(i, sizeof(char) ==
                            write(testPairs[i].controlFd(), &controlByte,
                                  sizeof(char)));

                ::pollfd result[NUM_PAIRS];

                // Just to keep purify happy
                memset(&result, 0, NUM_PAIRS * sizeof(struct ::pollfd));

                dvpoll control;
                control.dp_fds = (::pollfd*) &result;
                control.dp_nfds = NUM_PAIRS;
                control.dp_timeout = 0;
                int rc = ioctl(devPollFd, DP_POLL, &control);
                LOOP_ASSERT(i, 1 == rc);
                LOOP_ASSERT(i, testPairs[i].observedFd() == result[0].fd);
                LOOP_ASSERT(i, POLLIN  == result[0].revents);

                char byte;
                rc = read(testPairs[i].observedFd(), &byte, sizeof(char));
                LOOP_ASSERT(i, sizeof(char) == rc);
                LOOP_ASSERT(i, controlByte == byte);
            }
            ASSERT(0 == close(devPollFd));
        }

        if (veryVerbose)
            cout << "\tAddressing concern #7: 'ioctl' behavior."
                 << endl;
        {
            int devPollFd = open("/dev/poll", O_RDWR);
            ASSERT(0 <= devPollFd);
            if (veryVerbose) {
                cout << "\t\tRegistering test pairs." << endl;
            }

            ::pollfd registrations[NUM_PAIRS];
            for (int i = 0; i < NUM_PAIRS; ++i) {
                registrations[i].fd = testPairs[i].observedFd();
                registrations[i].events = POLLIN;
                registrations[i].revents = 0;      // Just to satisfy purify
            }
            int rc = write(devPollFd, &registrations,
                           NUM_PAIRS * sizeof(::pollfd));
            ASSERT(NUM_PAIRS * sizeof(::pollfd) == rc);

            if (veryVerbose) {
                cout << "\t\tSignalling and verifying correctness." << endl;
            }
            for (int i = 0; i < NUM_PAIRS; ++i) {
                const char controlByte = 0xAB;
                LOOP_ASSERT(i, sizeof(char) ==
                            write(testPairs[i].controlFd(), &controlByte,
                                  sizeof(char)));

                ::pollfd result[NUM_PAIRS];
                // Just to keep purify happy
                memset(&result, 0, NUM_PAIRS * sizeof(struct ::pollfd));

                dvpoll control;
                control.dp_fds = (::pollfd*) &result;
                control.dp_nfds = NUM_PAIRS;
                control.dp_timeout = 0;
                int rc = ioctl(devPollFd, DP_POLL, &control);
                LOOP_ASSERT(i, i + 1 == rc);
                for (int j = 0; j < rc; ++j) {
                    LOOP2_ASSERT(i, j,
                                 testPairs[j].observedFd() == result[j].fd);
                    LOOP2_ASSERT(i, j,  POLLIN  == result[j].revents);
                }
            }
            ASSERT(0 == close(devPollFd));
        }
      } break;
      case 9: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocket' FUNCTION:
        // Concern:
        //   o  Deregistration from a callback of the same socket is handled
        //      correctly
        //   o  Deregistration from a callback of another socket  is handled
        //      correctly
        //   o  Deregistration from a callback of one of the _previous_
        //      sockets and subsequent registration is handled correctly -
        //      see DRQS 8124027
        // Plan:
        //   Create custom set of scripts for each concern and exercise them
        //   using 'bteso_EventManagerTester'.
        // Testing:
        //   int deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocket'" << endl
                                  << "==========================" << endl;
        if (verbose)
            cout << "\tAddressing concern# 1" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
//-------------->
{ L_, 0,  "+0r64,{-0}; W0,64; T1; Dn,1; T0"                              },
{ L_, 0,  "+0r64,{-0}; +1r64; W0,64;  W1,64; T2; Dn,2; T1; E1r; E0"      },
{ L_, 0,  "+0r64,{-0}; +1r64; +2r64; W0,64;  W1,64; W2,64; T3; Dn,3; T2;"
          "E0; E1r; E2r"                                                 },
{ L_, 0,  "+0r64; +1r64,{-1}; +2r64; W0,64;  W1,64; W2,64; T3; Dn,3; T2"
          "E0r; E1; E2r"                                                 },
{ L_, 0,  "+0r64; +1r64; +2r64,{-2}; W0,64;  W1,64; W2,64; T3; Dn,3; T2"
          "E0r; E1r; E2"                                                 },
{ L_, 0,  "+0r64,{-1; +1r64}; +1r64; W0,64; W1,64; T2; Dn,2; T2"         },
//-------------->
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX(&timeMetric, &testAllocator);
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
            }
        }
        if (verbose)
            cout << "\tAddressing concern# 2" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
//-------------->
/// On length 2
// Deregistering signaled socket handle
{ L_, 0,  "+0r64,{-1}; +1r; W0,64;  W1,64; T2; Dn,1; T1; E0r; E1"       },
{ L_, 0,  "+0r64; +1r64,{-0}; W0,64;  W1,64; T2; Dn,2; T1; E0; E1r"     },
// Deregistering non-signaled socket handle
{ L_, 0,  "+0r64, {-1}; +1r; W0,64; T2; Dn,1; T1; E0r; E1"              },
{ L_, 0,  "+0r; +1r64, {-0}; W1,64; T2; Dn,1; T1; E0;  E1r"             },

/// On length 3
// Deregistering signaled socket handle
{ L_, 0,  "+0r64,{-1}; +1r; +2r64; W0,64; W1,64; W2,64; T3; Dn,2; T2;"
          "E0r; E1; E2r"                                                },

{ L_, 0,  "+0r64,{-2}; +1r64; +2r; W0,64; W1,64; W2,64; T3; Dn,2; T2;"
          "E0r; E1r; E2"                                                },

{ L_, 0,  "+0r64; +1r64,{-0}; +2r64; W0,64; W1,64; W2,64; T3; Dn,3; T2;"
          "E0; E1r; E2r"                                                },

{ L_, 0,  "+0r64; +1r64, {-2}; +2r; W0,64; W1,64; W2,64; T3; Dn,2; T2;"
          "E0r; E1r; E2"                                                },
// Deregistering non-signaled socket handle

//-------------->
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX(&timeMetric, &testAllocator);
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
            }
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

        if (verbose)
            cout << "\tStandard test for 'dispatch'" << endl;
        {
// TBD FIX ME
#ifndef BSLS_PLATFORM__OS_SOLARIS
            Obj mX(&timeMetric, &testAllocator);
            int notFailed = !bteso_EventManagerTester::testDispatch(&mX,
                                                                  controlFlag);
            ASSERT("BLACK-BOX (standard) TEST FAILED" && notFailed);
#endif
        }

        if (verbose)
            cout << "\tCustom test for 'dispatch'" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
                {L_, 0, "Dn0,0"                                              },
                {L_, 0, "Dn100,0"                                            },
                {L_, 0, "+0w2; Dn,1"                                         },
                {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"                  },
                {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"               },
                {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"        },
                {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                        "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8" },
                {L_, 0, "+2r3; Dn100,0; +2w40; Dn50,1;  W2,30; Dn55,2"       },
                {L_, 0, "+0w20; +0r12; Dn0,1; W0,30; +1w6; +2w8; Dn100,4"    },
                {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; Dn100,4;"
                        "W0,60; W1,70; +1r6; W2,60; W3,60; +2r8; +3r10;"
                        "Dn120,8"                                            },
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
            cout << "\tVerifying behavior on timeout (no sockets)." << endl;
        {
            const int NUM_ATTEMPTS = 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                bdet_TimeInterval deadline = bdetu_SystemTime::now();

                deadline.addMilliseconds(i % 10);
                deadline.addNanoseconds(i % 1000);

                LOOP_ASSERT(i, 0 == mX.dispatch(deadline,
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
            const int NUM_ATTEMPTS = 1000;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
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
            cout << "Standard test for 'deregisterAll'" << endl
                 << "=================================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            int fails = bteso_EventManagerTester::testDeregisterAll(&mX,
                                                                  controlFlag);
            ASSERT(0 == fails);
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
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the deregisterSocket()
        //   in this event manager.
        // Customized test:
        //   Create a socket, register and then unregister more than the system
        //   limit for open files and then try to dispatch.  This will make
        //   sure that the internal /dev/poll buffer is consistent with the
        //   number of open files.
        // Testing:
        //   int deregisterSocket();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocket'" << endl
                                  << "==========================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);

            int fails = bteso_EventManagerTester::testDeregisterSocket(&mX,
                                                                controlFlag);
            ASSERT(0 == fails);
        }
        {
            enum { NUM_DEREGISTERS = 70000 };
            Obj mX;

            bdef_Function<void (*)()> cb(&assertCb);

            for (int i = 0; i < NUM_DEREGISTERS; ++i) {
                int fd = socket(PF_INET, SOCK_STREAM, 0);
                BSLS_ASSERT(fd != -1);
                mX.registerSocketEvent(fd, bteso_EventType::BTESO_READ, cb);
                mX.deregisterSocket(fd);
                close(fd);
            }
            int fd = socket(PF_INET, SOCK_STREAM, 0);
            mX.registerSocketEvent(fd, bteso_EventType::BTESO_READ, cb);
            bdet_TimeInterval timeout = bdetu_SystemTime::now();
            timeout.addMilliseconds(200);
            ASSERT(0 == mX.dispatch(timeout, 0));
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
        //   corresponding test function of 'bteso_EventManagerTester', where
        //   multiple socket pairs are created to test the
        //   deregisterSocketEvent() in this event manager.
        // Customized test:
        //   Create a socket, register and then unregister more than the system
        //   limit for open files and then try to dispatch.  This will make
        //   sure that the internal /dev/poll buffer is consistent with the
        //   number of open files.
        // Testing:
        //   void deregisterSocketEvent();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING 'deregisterSocketEvent'" << endl
                                  << "===============================" << endl;
        if (verbose)
            cout << "Standard test for 'deregisterSocketEvent'" << endl
                 << "=========================================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);

            int fails = bteso_EventManagerTester::testDeregisterSocketEvent(
                                                             &mX, controlFlag);
            ASSERT(0 == fails);
        }

        if (verbose)
            cout << "Customized test for 'deregisterSocketEvent'" << endl
                 << "===========================================" << endl;
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
        {
            enum { NUM_DEREGISTERS = 70000 };
            Obj mX;

            bdef_Function<void (*)()> cb(&assertCb);

            for (int i = 0; i < NUM_DEREGISTERS; ++i) {
                int fd = socket(PF_INET, SOCK_STREAM, 0);
                BSLS_ASSERT(fd != -1);
                mX.registerSocketEvent(fd, bteso_EventType::BTESO_READ, cb);
                mX.deregisterSocketEvent(fd, bteso_EventType::BTESO_READ);
                close(fd);
            }
            int fd = socket(PF_INET, SOCK_STREAM, 0);
            mX.registerSocketEvent(fd, bteso_EventType::BTESO_READ, cb);
            bdet_TimeInterval timeout = bdetu_SystemTime::now();
            timeout.addMilliseconds(200);
            ASSERT(0 == mX.dispatch(timeout, 0));
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
            cout << "Standard test for 'registerSocketEvent'" << endl
                 << "=======================================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            int fails = bteso_EventManagerTester::testRegisterSocketEvent(&mX,
                                                                  controlFlag);
            ASSERT(0 == fails);

            if (verbose) {
                P(timeMetric.percentage(bteso_TimeMetrics::BTESO_CPU_BOUND));
            }
            ASSERT(100 == timeMetric.percentage(
                                          bteso_TimeMetrics::BTESO_CPU_BOUND));
        }

        if (verbose)
            cout << "Customized test for 'registerSocketEvent'" << endl
                 << "=========================================" << endl;
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
            if (verbose) {
                P(timeMetric.percentage(bteso_TimeMetrics::BTESO_CPU_BOUND));
            }
            ASSERT(100 == timeMetric.percentage(
                                          bteso_TimeMetrics::BTESO_CPU_BOUND));
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

        if (verbose) cout << "\tOn a non-metered object" << endl;
        {

            Obj mX((bteso_TimeMetrics*)0, &testAllocator);

            int fails = bteso_EventManagerTester::testAccessors(&mX,
                                                                controlFlag);
            ASSERT(0 == fails);
        }
        if (verbose) cout << "\tOn a metered object" << endl;
        {

            Obj mX(&timeMetric, &testAllocator);
            int fails = bteso_EventManagerTester::testAccessors(&mX,
                                                                controlFlag);
            ASSERT(0 == fails);
            if (verbose) {
                P(timeMetric.percentage(bteso_TimeMetrics::BTESO_CPU_BOUND));
            }
            ASSERT(100 == timeMetric.percentage(
                                          bteso_TimeMetrics::BTESO_CPU_BOUND));
        }
      } break;
      case 2: {
        // -----------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //
        // Plan:
        // Standard test:
        //   Create objects of the event manager under test and a list
        //   of test scripts based on the script grammar defined in
        //   'bteso_EventManagerTester', call the script interpreting function
        //   gg() of 'bteso_EventManagerTester' to execute the test data.
        // Testing:
        //   bteso_DefaultEventManager();
        //   ~bteso_DefaultEventManager();
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATORS" << endl
                                  << "============================" << endl;
        {
            Obj mX[2];
            const int NUM_OBJ = sizeof mX / sizeof mX[0];
            for (int k = 0; k < NUM_OBJ; k++) {
                 struct {
                     int         d_line;
                     int         d_fails;  // failures in this script
                     const char *d_script;
                } SCRIPTS[] =
                {
         //------------------>
         { L_, 0, "+0r; E0r; T1; -0r; E0; T0"                               },
         { L_, 0, "+0w; E0w; T1; -0w; E0; T0"                               },
         { L_, 0, "+0w; +0w; E0w; T1; -0w; E0; T0"                          },
         { L_, 0, "+0r; +0r; E0r; T1; -0r; E0; T0"                          },
         { L_, 0, "+0r; +0w; E0rw; T2; -0r; -0w; E0; T0"                    },
         { L_, 0, "+0r; +1r; E0r; E1r; T2; -0r; -1r; E0; E1; T0"            },
         { L_, 0, "+0r; +1r; +1w; E0r; E1wr; T3; -0r; -1r; -1w; E0; E1; T0" },
         { L_, 0, "+0r; +1r; +1w; +0w E0rw; E1wr; T4"                       },
         //------------------>
                };
                const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

                for (int i = 0; i < NUM_SCRIPTS; ++i) {

                    const int LINE =  SCRIPTS[i].d_line;
                    enum { NUM_PAIRS = 4 };

                    bteso_EventManagerTestPair socketPairs[NUM_PAIRS];

                    for (int j = 0; j < NUM_PAIRS; j++) {
                        socketPairs[i].setObservedBufferOptions(BUF_LEN, 1);
                        socketPairs[i].setControlBufferOptions(BUF_LEN, 1);
                    }

                    int fails = bteso_EventManagerTester::gg(&mX[k],
                                                           socketPairs,
                                                           SCRIPTS[i].d_script,
                                                           controlFlag);

                    LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                    if (veryVerbose) {
                        P_(LINE);   P(fails);
                    }
                }
            }
        }
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST
        //   Ensure the basic liveness of an event manager instance.
        //
        // Testing:
        //   Create an object of this event manager under test.  Perform
        //   some basic operations on it.
        // -----------------------------------------------------------------
        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "Dn0,0"                                            },
               {L_, 0, "Dn100,0"                                          },
               {L_, 0, "+0w2; Dn,1"                                       },
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,40; Dn0,2"                 },
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,40; Dn120,2"             },
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"      },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12;"
                        "Dn,4; W0,40; +1r6; W1,40; W2,40; W3,40; +2r8;"
                        "+3r10; Dn,8"                                     },
               {L_, 0, "+2r3; Dn100,0; +2w40; Dn50,1; W2,40; Dn55,2"      },
               {L_, 0, "+0w20; +0r12; Dn0,1; +1w6; +2w8; W0,40; Dn100,4"  },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12;"
                       "Dn100,4; W0,40; W1,40; W2,40; W3,40; +1r6; +2r8;"
                       "+3r10; Dn120,8"                                   },
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {

                Obj mX((bteso_TimeMetrics*)0, &testAllocator);
                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS  = 4 };
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
      } break;

      case -1: {
        // -----------------------------------------------------------------
        // PERFORMANCE TESTING 'dispatch':
        //   Get the performance data.
        //
        // Plan:
        //   Set up multiple connections and register a read event for each
        //   connection, calculate the average time taken to dispatch a read
        //   event for a given number of registered read event.
        // Testing:
        //   'dispatch' capacity
        // -----------------------------------------------------------------
        enum {
            DEFAULT_NUM_PAIRS        = 1024,
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
            const char *FILENAME = "devpollDispatch.dat";

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
                      numPairs, numMeasurements, controlFlag);

            outFile.close();
        }
      } break;

      case -2: {
        // -----------------------------------------------------------------
        // PERFORMANCE TESTING 'registerSocketEvent':
        //   Get the performance data.
        //
        // Plan:
        //   Open multiple sockets and register a read event for each
        //   socket, calculate the average time taken to register a read
        //   event for a given number of registered read event.
        // Testing:
        //   'registerSocketEvent' capacity
        // -----------------------------------------------------------------
        enum {
            DEFAULT_NUM_PAIRS        = 1024,
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
            const char *FILENAME = "devpollRegister.dat";

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
                      numPairs, numMeasurements, controlFlag);

            outFile.close();
        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    bteso_SocketImpUtil::cleanup();

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
#else
    return -1;
#endif // BTESO_EVENTMANAGERIMP_ENABLETEST
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
