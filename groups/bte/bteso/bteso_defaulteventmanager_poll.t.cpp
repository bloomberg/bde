// bteso_defaulteventmanager_poll.t.cpp      -*-C++-*-

#include <bteso_defaulteventmanager_poll.h>

#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>
#include <bteso_ioutil.h>
#include <bteso_timemetrics.h>
#include <bteso_eventmanagertester.h>
#include <bteso_platform.h>
#include <bteso_flag.h>

#include <bcemt_thread.h>

#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

#include <bslma_testallocator.h>
#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bsls_platform.h>

#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;

#if defined(BSLS_PLATFORM__OS_SOLARIS)    \
    || defined(BSLS_PLATFORM__OS_LINUX)   \
    || defined(BDES_PLATFORM__OS_FREEBSD) \
    || defined(BSLS_PLATFORM__OS_AIX)     \
    || defined(BSLS_PLATFORM__OS_HPUX)    \
    || defined(BSLS_PLATFORM__OS_CYGWIN)
    #define BTESO_EVENTMANAGER_ENABLETEST
    typedef bteso_DefaultEventManager<bteso_Platform::POLL> Obj;
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
// [ 7] deregisterAll
// [ 8] dispatch
//
// ACCESSORS
// [11] canRegisterSockets
// [11] hasLimitedSocketCapacity
// [ 3] numSocketEvents
// [ 3] numEvents
// [ 3] isRegistered
//-----------------------------------------------------------------------------
// [12] Usage example
// [ 1] Breathing test
// [ 9] Test for DRQS 10117512: Connect to non-listening port may result in
//      100 % CPU utilization and a connect callback is NOT invoked
// [10] Test for DRQS 10105162:
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
        READ_SIZE  = 8192,
        WRITE_SIZE = 30000
    };
#else
    enum {
        READ_SIZE  = 8192,
        WRITE_SIZE = 73728
    };
#endif

enum { POLL_IS_DEFAULT = bslmf_IsSame<
                           bteso_Platform::POLL,
                           bteso_Platform::DEFAULT_POLLING_MECHANISM>::VALUE };


//==========================================================================
//                      HELPER CLASS
//--------------------------------------------------------------------------
class SocketPair {
    // This helper class manages a client-server pair of connected sockets.
  private:
    bteso_SocketHandle::Handle d_fds[2];
  public:
    // CREATORS
    SocketPair();
    ~SocketPair();

    // ACCESSORS
    bteso_SocketHandle::Handle& clientFd();
        // Return a handle to the client socket.
    bteso_SocketHandle::Handle& serverFd();
        // Return a handle to the server socket.
};

SocketPair::SocketPair()
{
    int rc = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                d_fds, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
    ASSERT(0 == rc);
}

SocketPair::~SocketPair()
{
    ASSERT(0 == bteso_SocketImpUtil::close(d_fds[0]));
    ASSERT(0 == bteso_SocketImpUtil::close(d_fds[1]));
}

inline
bteso_SocketHandle::Handle& SocketPair::clientFd()
{
    return d_fds[0];
}

inline
bteso_SocketHandle::Handle& SocketPair::serverFd()
{
    return d_fds[1];
}

static void
genericCb(bteso_EventType::Type event, bteso_SocketHandle::Handle socket,
          int bytes, bteso_EventManager *mX, bool deregisterAll)
{
    // User specified callback function that will be called after an event
    // is dispatched to do the "real" things.
    // This callback is only used in the 'usage example' test case, and will
    // be copied to the head file as a part of the usage example.
    enum {
        MAX_READ_SIZE  = 8192,
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

    if (deregisterAll) {
        mX->deregisterAll();
    }
}

static void
testInvocationCb(bool *isInvoked,                   bteso_EventManager *mX,
                 bteso_SocketHandle::Handle handle, bteso_EventType::Type event
)
{
    ASSERT(isInvoked);
    *isInvoked = true;

    ASSERT(mX);
    mX->deregisterSocketEvent(handle, event);
}

static double dub(const bdet_TimeInterval& ti)
{
    return ti.totalSecondsAsDouble();
}

struct ShouldntBeCalled {
    // This functor is to be used in situations where we don't expect it
    // to be called.

    void operator()()
    {
        ASSERT(0 && "*** ShouldntBeCalled called ***");
    }
};

#ifdef BSLS_PLATFORM__OS_HPUX
// HPUX sometimes seems to need a lag between when events are set up and
// when the dispatcher is called.  In production this shouldn't be a
// problem since this is a polling interface -- the events would just
// occur on the next polling event.  Insert this command where it's needed
// in the gg string, but it's conditionally compiled to avoid watering down
// tests on other platforms.

# define PRE_DISPATCH_SLEEP " S40; "
#else
# define PRE_DISPATCH_SLEEP
#endif


//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
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
            bteso_DefaultEventManager<bteso_Platform::POLL> mX(&timeMetric);

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
                                       , &mX
                                       , false));

            mX.registerSocketEvent(socket[0],
                                   bteso_EventType::BTESO_READ,
                                   readCb);

            numBytes = 25;
            bteso_EventManager::Callback writeCb1(
                    bdef_BindUtil::bind( &genericCb
                                       , bteso_EventType::BTESO_WRITE
                                       , socket[0]
                                       , numBytes
                                       , &mX
                                       , false));

            mX.registerSocketEvent(socket[0], bteso_EventType::BTESO_WRITE,
                                   writeCb1);

            numBytes = 15;
            bteso_EventManager::Callback writeCb2(
                    bdef_BindUtil::bind( &genericCb
                                       , bteso_EventType::BTESO_WRITE
                                       , socket[1]
                                       , numBytes
                                       , &mX
                                       , false));

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
            int flags = 0;   // disable interrupts
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
      case 11: {
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
      case 10: {
        // --------------------------------------------------------------------
        // TESTING FOR DRQS 10105162
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

      case 9: {
        // -----------------------------------------------------------------
        // TESTING FOR DRQS 10117512
        //   When initiating a non-blocking connect to a non-listening port,
        //   the registered callback is never invoked and the process uses
        //   100 % CPU
        //
        // Plan:
        //   Create a socket and issue a non-blocking connect to a
        //   non-listening port.  Create an object under test and register a
        //   callback to be invoked.  Dispatch the callbacks (with a timeout)
        //   and verify that the callback is invoked.
        //
        // Testing:
        //   registerSocketEvent(CONNECT)
        // -----------------------------------------------------------------
        if (verbose) cout << "\n'registerSocketEvent' for NON-BLOCKING CONNECT"
                          << "\n=============================================="
                          << endl;
        {
            if (verbose) {
                bsl::cout << "-> Creating a socket" << bsl::endl;
            }

            bteso_SocketHandle::Handle handle;
            int result
                = bteso_SocketImpUtil::open<bteso_IPv4Address>(&handle,
                                     bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
            ASSERT(!result);

            result = bteso_IoUtil::setBlockingMode(
                                              handle,
                                              bteso_IoUtil::BTESO_NONBLOCKING);
            ASSERT(!result);

            if (verbose) {
                bsl::cout << "-> Issuing a non-blocking connect: ";
            }
            bteso_IPv4Address server;
            server.setPortNumber(1);

            result = bteso_SocketImpUtil::connect(handle, server);
            if (verbose) {
                bsl::cout << "rc = " << result << ", errno = " << errno
                    << ": " << strerror(errno) << bsl::endl;
            }

            ASSERT(result != 0); // connection failed

            if (result != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
                // connection failed with an error other than EWOULDBLOCK
                bteso_SocketImpUtil::close(handle);
            }

            bteso_TimeMetrics timeMetric(
                                   bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
                                   bteso_TimeMetrics::BTESO_CPU_BOUND);
            Obj mX(&timeMetric, &testAllocator);
            bool isInvoked = false;

            bteso_EventManager::Callback connectCb(
                    bdef_BindUtil::bind( &testInvocationCb
                                       , &isInvoked
                                       , &mX
                                       , handle
                                       , bteso_EventType::BTESO_CONNECT));

            ASSERT(!mX.registerSocketEvent(handle,
                                           bteso_EventType::BTESO_CONNECT,
                                           connectCb));
#ifdef BSLS_PLATFORM__OS_SOLARIS
            const double MAX_DELAY = 20.0;    // 20 sec
#else
            const double MAX_DELAY =  5.0;    //  5 sec
#endif
            ASSERT(!isInvoked);
            int rc = mX.dispatch(bdetu_SystemTime::now() + MAX_DELAY, 0);
            ASSERT(isInvoked);
            // Deregistration is done in the callback

        }
      } break;
      case 8: {
        // -----------------------------------------------------------------
        // TESTING 'deregisterSocket' FUNCTION:
        // Concern:
        //   o  Deregistration from a callback of the same socket is handled
        //      correctly
        //   o  Deregistration from a callback of another socket  is handled
        //      correctly
        //   o  Deregistration from a callback of one of the _previous_
        //      sockets and subsequent socket registration is handled
        //      correctly - see DRQS 8134027
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
{ L_, 0,  "+0r64; +1r64,{-1}; +2r64; W0,64;  W1,64; W2,64; T3; Dn,3; T2;"
          "E0r; E1; E2r"                                                 },
{ L_, 0,  "+0r64; +1r64; +2r64,{-2}; W0,64;  W1,64; W2,64; T3; Dn,3; T2;"
          "E0r; E1r; E2"                                                 },
{ L_, 0,  "+0r64,{-1; +1r64}; +1r64; W0,64; W1,64; T2; Dn,2; T2"         }
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

      case 7: {
// #ifndef BSLS_PLATFORM__OS_AIX
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

        if (verbose) cout << endl << "Testing 'dispatch' method." << endl
                                  << "==========================" << endl;

#if 0
        if (verbose)
            cout << "Standard test for 'dispatch'" << endl
                 << "============================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);

            int fails = bteso_EventManagerTester::testDispatch(&mX,
                                                               controlFlag);
            ASSERT(0 == fails);
        }
#endif

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
                // first repeat all the tests in
                // 'bteso_EventManagerTester::testDispatch', except for the
                // ones that fail due to lag.

               {L_, 0, "-a; Dn0,0"                                     },
               {L_, 0, "-a; Dn100,0"                                   },

                 // Test the event manager when one socket event exists.
               {L_, 0, "-a; +0w2; Dn,1"},
               {L_, 0, "-a; W0,64; +0r24; Dn,1; -0; T0"                 },

                 // Test the event manager when two socket events exist.
                 // The two socket events are for the same socket handle.
               {L_, 0, "-a; +0w64; +0r24; Dn,1; -0w; Di500,0; -0r; T0"  },
               {L_, 0, "-a; +0r24; +0w64; Dn,1; -0w; Di500,0; -0r; T0"  },

                // fail due to lag on hpux:
                // These cases are repeated with a sleep before the first
                // dispatch in test case 12.  The fact that there is a lag
                // is probably due to the underlying implementation and not
                // bde, and is not serious for a polling interface because
                // subsequent polls would pick up the lagging events.
               {L_, 0, "-a; +0w64; +0r24; W0,64;" PRE_DISPATCH_SLEEP
                                            "Dn,2; -0w; Di,1; -0r; T0"  },
               {L_, 0, "-a; +0r24,{-a}; +0w64; W0,64; T2;" PRE_DISPATCH_SLEEP
                                                     "Dn,1; E0; T0"     },
               {L_, 0, "-a; +0r64; +1w24; W0,64;" PRE_DISPATCH_SLEEP
                                            "Dn,2; -1w; Di500,0; T1"    },

                 // The two socket events are for different socket handles.
               {L_, 0, "-a; +0w64; +1w24; W0,64; Dn,2; -0w; Di500,1; T1"    },
               {L_, 0, "-a; +0w64; +1r24; W0,64; Dn,1; -0w; Di500,0; T1"    },
               {L_, 0, "-a; +0w64; +1a;   W0,64; Dn,1; -0w; Di500,0; T1"    },

               {L_, 0, "-a; +0r64; +1r24; W0,64; Dn,1; -0r; Di500,0; T1"    },
               {L_, 0, "-a; +0r64; +1a;   W0,64; Dn,1; -0r; Di500,0; T1"    },

               {L_, 0, "-a; +0a;   +1w24; Dn,1; -1w; Di500,0; T1"           },
               {L_, 0, "-a; +0a;   +1r64; W1,64; Dn,1; -0a; Di500,0; T1"    },

                 // Test the event manager when multiple socket events exist.
               {L_, 0, "-a; +0w64; +1r10; +2w100; +1w20; +3w60; Dn,4; -a; T0"},

                 // Test the event manager when a test script need to be
                 // executed in the user-installed callback function.
               {L_, 0, "-a; +0r24,{-a}; +1r64; W0,64; T2; Dn,1; E0; E1; T0" },

                 // The following tests cannot be in a black-box test since
                 // the order of iteration is undefined
                 // {L_, 0, "-a; +0r24,{-1}; +1r; W0,64; W1,32; T2; Dn,1; T1"},
                 // {L_, 0, "-a; +0r24; +1r12,{-0}; W0,64; W1,32; T2; Dn,2;"
                 //                                                     " T1"},
               {L_, 0, "-a; W0,60; +0w64,{+2w100; +0r10}; Dn,1; -0w; Di,2;"
                                                                  " -a; T0" },
               {L_, 0, "-a; W0,60; +0w64,{-1r; +0r10}; +1r20; E1r; Dn,1;"
                                                        " -0w; Di,1; -a; T0" },

                // That's all the tests in 'testDispatch', now do some
                // additional tests.

               {L_, 0, "+0w40; +0r3; Dn0,1; W0,30;  Dn0,2"                 },
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,30; Dn120,2"              },
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"       },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12; W3,30;"
                       "Dn,4; W0,30; +1r6; W1,30; +2r8; W2,30; +3r10; Dn,8"},
               {L_, 0, "+2r3; Dn100,0; +2w40; Dn50,1;  W2,30; Dn55,2"      },
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
            int nowFailures = 0;  // due to time going backward on some systems
            int intFailures = 0;  // due to unexpected interrupts
            const int NUM_ATTEMPTS = 5000;
            double waitFrac = 0.010 / NUM_ATTEMPTS;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);

                const double delay = i * waitFrac;
                bdet_TimeInterval start = bdetu_SystemTime::now();
                bdet_TimeInterval deadline = start + delay;

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));

                bdet_TimeInterval finish = bdetu_SystemTime::now();
                const double dormant = dub(finish - start);

                if (dormant < 0) {
                    // We observe that on Linux sometimes,
                    // bdetu_SystemTime::now() gives grossly wrong results,
                    // like 'dormant < -0.020'.

                    ++nowFailures;
                    cout << "*WARNING*  0 sockets: Time going backwards.  ";
                    P(dormant);
#if defined(BSLS_PLATFORM__OS_LINUX)
                    LOOP_ASSERT(nowFailures, nowFailures <= 2);
#else
                    LOOP_ASSERT(nowFailures, 0 == nowFailures);
#endif
                }
                else if (finish < deadline) {
                    // We're willing to tolerate a limited number of documented
                    // failures.

                    ++intFailures;
                    cout << "*WARNING*  0 sockets: Unexpected interrupt.\n";
                    P_(delay);    P_(dub(deadline - finish));    P(dormant);

                    // it is observed that on Linux, rarely does an interrupt
                    // occur before 0.0033 seconds.

                    ASSERT(dormant > 0.003);
                    LOOP_ASSERT(intFailures, intFailures <= 2);
                }

                if (veryVerbose && deadline <= finish) {
                    P_(delay); P_(dub(deadline - finish));  P(dormant);
                }
            }
        }

        if (verbose)
            cout << "Verifying behavior on timeout (at least one socket)."
                 << endl;
        {
            SocketPair pair;
            ShouldntBeCalled shouldntBeCalled;
            int nowFailures = 0;  // due to time going backward on some systems
            int intFailures = 0;  // due to unexpected interrupts
            const int NUM_ATTEMPTS = 5000;
            double waitFrac = 0.010 / NUM_ATTEMPTS;
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&timeMetric, &testAllocator);
                mX.registerSocketEvent(pair.serverFd(),
                                       bteso_EventType::BTESO_READ,
                                       shouldntBeCalled);

                const double delay = i * waitFrac;
                bdet_TimeInterval start = bdetu_SystemTime::now();
                bdet_TimeInterval deadline = start + delay;

                LOOP_ASSERT(i, 0 == mX.dispatch(
                                           deadline,
                                           bteso_Flag::BTESO_ASYNC_INTERRUPT));

                bdet_TimeInterval finish = bdetu_SystemTime::now();
                const double dormant = dub(finish - start);

                if (dormant < 0) {
                    // We observe that on Linux sometimes,
                    // bdetu_SystemTime::now() gives grossly wrong results,
                    // like 'dormant < -0.020'.

                    ++nowFailures;
                    if (nowFailures < 20) {
                        cout << "*WARNING*  1 socket: Time going backwards " <<
                                         nowFailures << " times. dormant = " <<
                                                               dormant << endl;
                    }
#if defined(BSLS_PLATFORM__OS_LINUX)
                    // Linux isn't really very important here because polling
                    // is not the default event mananger for Linux.

                    BSLMF_ASSERT(0 == POLL_IS_DEFAULT);

                    ASSERT(dormant > -0.04);
                    LOOP2_ASSERT(nowFailures, i, nowFailures <= 700);
#else
                    LOOP_ASSERT(nowFailures, 0 == nowFailures);
#endif
                }
                else if (finish < deadline) {
                    // We're willing to tolerate a limited number of documented
                    // failures.

                    ++intFailures;
                    if (intFailures < 20) {
                        cout << "*WARNING*  1 socket: Unexpected interrupt " <<
                                                    intFailures << " times.\n";
                        P_(delay);    P_(dub(deadline - finish));  P(dormant);
                    }

#ifdef BSLS_PLATFORM__OS_LINUX
                    // Linux isn't really very important here because polling
                    // is not the default event mananger for Linux.

                    BSLMF_ASSERT(0 == POLL_IS_DEFAULT);

                    LOOP_ASSERT(intFailures, intFailures <= 100);

                    // ASSERT(dormant > 0.0015);
#else
                    LOOP_ASSERT(intFailures, intFailures <= 2);
                    ASSERT(dormant > 0.003);
#endif
                }

                if (veryVerbose && deadline <= finish) {
                    P_(delay); P_(dub(deadline - finish));  P(dormant);
                }
            }

#ifdef BSLS_PLATFORM__OS_LINUX
            if (nowFailures) P(nowFailures);
            if (intFailures) P(intFailures);
#endif
        }
// #endif
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
        if (verbose) cout << endl << "TESTING 'deregisterSocket'" << endl
                                  << "==========================" << endl;
        {
            Obj mX(&timeMetric, &testAllocator);
            int fails = bteso_EventManagerTester::testDeregisterSocket(&mX,
                                                                  controlFlag);
            ASSERT(0 == fails);
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
      case 2: {
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
               {L_, 0, "Dn0,0"                                           },
               {L_, 0, "Dn100,0"                                         },
               {L_, 0, "+0w2; Dn,1"                                      },
               {L_, 0, "+0w40; +0r3; Dn0,1; W0,40; Dn0,2"                },
               {L_, 0, "+0w40; +0r3; Dn100,1; W0,40; Dn120,2"            },
               {L_, 0, "+0w20; +0r12; Dn,1; W0,30; +1w6; +2w8; Dn,4"     },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12;"
                        "Dn,4; W0,40; +1r6; W1,40; W2,40; W3,40; +2r8;"
                        "+3r10; Dn,8"                                    },
               {L_, 0, "+2r3; Dn200,0; +2w40; Dn100,1; W2,40; Dn150,2"   },
               {L_, 0, "+0w20; +0r12; Dn0,1; +1w6; +2w8; W0,40; Dn100,4" },
               {L_, 0, "+0w40; +1r6; +1w41; +2w42; +3w43; +0r12;"
                       "Dn100,4; W0,40; W1,40; W2,40; W3,40; +1r6; +2r8;"
                       "+3r10; Dn120,8"                                  },
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
                << "PERFORMANCE TESTING 'dispatch'" << endl
                << "==============================" << endl;
        {
            const char *FILENAME = "pollDispatch.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }
            if (veryVerbose) {
                cout << "Number of test socket pairs: " << numPairs
                     << "; number of measurements: " << numMeasurements
                     << bsl::endl;
            }

            Obj mX(&timeMetric, &testAllocator);
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

            const char *FILENAME = "pollRegister.dat";

            ofstream outFile(FILENAME, ios_base::out);
            if (!outFile) {
                cout << "Cannot open " << FILENAME << " for writing."
                     << endl;
                return -1;
            }
            if (veryVerbose) {
                cout << "Number of test sockets: " << numPairs
                     << "; number of measurements: " << numMeasurements
                     << bsl::endl;
            }

            Obj mX(&timeMetric, &testAllocator);
            bdet_TimeInterval infinite, timeout;
            if (!outFile) {
                cout << "Cannot open pollregisterPerformance.dat "
                     << "errno: " << errno << endl;
                return 0;
            }
            bteso_EventManagerTester::testRegisterPerformance(&mX, outFile,
                                     numPairs, numMeasurements, controlFlag);
            outFile.close();
        }
      } break;
      case -3: {
        // -----------------------------------------------------------------
        // Interactive gg test shell
        // -----------------------------------------------------------------

        while (1) {
            char script[1000];
            cout << "Enter script: " << flush;
            cin.getline(script, 1000);

            if (cin.eof() && '\0' == script[0]) {
                cout << endl;
                break;
            }
            if (0 == bsl::strncmp(script, "quit", 4)) {
                break;
            }

            int i = 0;
            for (; i < 4; ++i) {
                Obj mX(&timeMetric, &testAllocator);

                const int NUM_PAIR = 4;
                bteso_EventManagerTestPair socketPairs[NUM_PAIR];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                         script,
                                                         controlFlag);
                if (fails) {
                    break;
                }
            }
            if (4 == i) {
                cout << "Success!\n";
            }
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
}
#else
// !defined(BTESO_EVENTMANAGERIMP_ENABLETEST)
int main()
{
    return -1;
}
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
