// btes5_networkconnector.t.cpp                                        -*-C++-*-

#include <btes5_networkconnector.h>
#include <btes5_testserver.h> // for testing only

#include <bdef_bind.h>
#include <bdef_placeholder.h>
#include <bsl_iostream.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsl_sstream.h>
#include <btemt_sessionpool.h> // for testing only
#include <bteso_inetstreamsocketfactory.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// [ ]
//-----------------------------------------------------------------------------
// [1] BREATHING TEST

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

bool             verbose = 0;
bool         veryVerbose = 0;
bool     veryVeryVerbose = 0;
bool veryVeryVeryVerbose = 0;

// ============================================================================
//                     btemt_SessionPool testing callback
// ----------------------------------------------------------------------------
void poolStateCb(int state, int source, void *)
{
    if (verbose) {
        cout << "Session Pool: state=" << state
             << " source=" << source
             << endl;
    }
}

void socks5Cb(btes5_NetworkConnector::Status                status,
              bteso_StreamSocket<bteso_IPv4Address>        *socket,
              bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
              const btes5_DetailedError&                    error,
              btemt_SessionPool                            *sessionPool,
              bcemt_Mutex                                  *stateLock,
              bcemt_Condition                              *stateChanged,
              volatile int                                 *state)
{
    bcemt_LockGuard<bcemt_Mutex> lock(stateLock);
    if (status == btes5_NetworkConnector::BTES5_SUCCESS) {
        *state = 1;
        cout << "connection succeeded" << endl;
        //TODO: import into sessionPool->import(...), for now just deallocate
        socketFactory->deallocate(socket);
    } else {
        *state = -1;
        cout << "Connect failed " << status << ": " << error << endl;
    }
    stateChanged->signal();
}

// ============================================================================
//                  USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Example 1: Connect to a server through two proxy levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We want to connect to a server reachable through two levels of proxies:
// first through one of corporate SOCKS5 servers, and then through one of
// regional SOCKS5 servers.
//
// First we define a callback function to process connection status, and if
// successful perform useful work and finally deallocate the socket. After the
// work is done (or error is reported) we signal the main thread with the
// status; this also signifies that we no longer need the stream factory passed
// to us.
//..
void connectCb(int                                           status,
               bteso_StreamSocket< bteso_IPv4Address>       *socket,
               bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
               const btes5_DetailedError&                    error,
               bcemt_Mutex                                  *stateLock,
               bcemt_Condition                              *stateChanged,
               volatile int                                 *state)
{
    if (0 == status) {
        cout << "connection succeeded" << endl;
        // Success: conduct I/O operations with 'socket' ... and deallocate it.
        socketFactory->deallocate(socket);
    } else {
        cout << "Connect failed " << status << ": " << error << endl;
    }
    bcemt_LockGuard<bcemt_Mutex> lock(stateLock);
    *state = status ? -1 : 1; // 1 for success, -1 for failure
    stateChanged->signal();
}
//..
// Then we define the level of proxies that should be reachable directory.
//..
static int connectThroughProxies(const bteso_Endpoint& corpProxy1,
                                 const bteso_Endpoint& corpProxy2)
{
    btes5_NetworkDescription proxies;
    proxies.addProxy(0, corpProxy1);
    proxies.addProxy(0, corpProxy2);
//..
// Next we add a level for regional proxies reachable from the corporate
// proxies. Note that .tk stands for Tokelau in the Pacific Ocean.
//..
    proxies.addProxy(1, bteso_Endpoint("proxy1.example.tk", 1080));
    proxies.addProxy(1, bteso_Endpoint("proxy2.example.tk", 1080));
//..
// Then we set the user name and password which will be used in case one of the
// proxies in the connection path requires that type of authentication.
//..
    btes5_UserCredentials credentials("John.smith", "pass1");
    btes5_NetworkDescriptionUtil::setAllCredentials(&proxies, credentials);
//..
// Next we construct a 'btes5_NetworkConnector' which will be used to connect
// to one or more destinations.
//..
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
    btemt_TcpTimerEventManager eventManager;
    eventManager.enable();
    btes5_NetworkConnector connector(proxies, &factory, &eventManager);
//..
// Finally we attempt to connect to the destination. Input, output and eventual
// closing of the connection will be handled from 'connectCb', which will
// signal the using 'state', with the access protected by a mutex and condition
// variable.
//..
    const bdet_TimeInterval timeout(30.0);
    bcemt_Mutex     stateLock;
    bcemt_Condition stateChanged;
    volatile int    state = 0; // value > 0 indicates success, < 0 is error
    using namespace bdef_PlaceHolders;
    connector.connect(bdef_BindUtil::bind(connectCb,
                                          _1, _2, _3, _4,
                                          &stateLock,
                                          &stateChanged,
                                          &state),
                      timeout,
                      bteso_Endpoint("destination.example.com", 8194));
    bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
    while (!state) {
        stateChanged.wait(&stateLock);
    }
    return state;
}

}  // close unnamed namespace

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma_TestAllocator ta(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example compiles and can be executed usccessfully.
        //
        // Plan:
        //: 1 Create a test SOCKS5 proxy configured to simulate connection.
        //: 2 Uncomment the usage example.
        //: 3 Verify successful compilation and execution.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE 1" << endl
                          << "===============" << endl;
        btes5_TestServerArgs args;
        // TODO: configure to simulate connection on valid request

        bteso_Endpoint proxy;
        btes5_TestServer proxyServer(&proxy, &args);
        if (verbose) {
            cout << "proxy started on " << proxy << endl;
        }
        ASSERT(connectThroughProxies(proxy, proxy) > 0);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SOCKS5 SOCKET IMPORT INTO SESSION POOL
        //
        // Concerns:
        //: 1 A socket connection established by 'btes5_NetworkConnector' can
        //:   be imported into, and managed by btemt_SessionPool
        //
        // Plan:
        //: 1 Create a test destination server
        //: 2 Create a test SOCKS5 proxy.
        //: 3 Establish a connection to the destination server via the proxy.
        //: 4 Import the connection into a SessionPool.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SOCKS5 + btemt_SessionPool::import" << endl
                          << "==================================" << endl;
        btes5_TestServerArgs args;

        bteso_Endpoint destination;
        btes5_TestServer destinationServer(&destination, &args);
        if (verbose) {
            cout << "destination server started on " << destination << endl;
        }

        args.d_expectedPort = destination.port();
        bteso_Endpoint proxy;
        btes5_TestServer proxyServer(&proxy, &args);
        if (verbose) {
            cout << "proxy started on " << proxy << endl;
        }
        btes5_NetworkDescription socks5Servers(&ta);
        socks5Servers.addProxy(0, proxy);

        bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory(&ta);
        btemt_TcpTimerEventManager eventManager;
        eventManager.enable();
        btes5_NetworkConnector connector(socks5Servers,
                                         &socketFactory,
                                         &eventManager,
                                         &ta);

        btemt_ChannelPoolConfiguration config;
        using namespace bdef_PlaceHolders;
        btemt_SessionPool sessionPool(config,
                                      &poolStateCb,
                                      &ta);
        ASSERT(0 == sessionPool.start());
        if (verbose) {
            cout << "starting btemt_SessionPool" << endl;
        }

        if (verbose) {
            cout << "attempting to connect to " << destination
                 << " via " << proxy
                 << endl;
        }
        const bdet_TimeInterval timeout(30.0);
        bcemt_Mutex     stateLock;
        bcemt_Condition stateChanged;
        volatile int    state = 0; // value > 0 indicates success, < 0 is error
        connector.connect(bdef_BindUtil::bind(socks5Cb,
                                              _1, _2, _3, _4,
                                              &sessionPool,
                                              &stateLock,
                                              &stateChanged,
                                              &state),
                          timeout,
                          destination);
        bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
        while (!state) {
            stateChanged.wait(&stateLock);
        }
        ASSERT(state > 0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        // TODO:
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
