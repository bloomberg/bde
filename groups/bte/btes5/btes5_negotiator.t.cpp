// btes5_negotiator.t.cpp     -*-C++-*-
#include <btes5_negotiator.h>

#include <btes5_testserver.h>

#include <bcemt_condition.h>
#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bdef_bind.h>
#include <bdef_placeholder.h>
#include <bdeut_stringref.h>
#include <bdex_bytestreamimputil.h>
#include <bsl_iostream.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bcema_threadenabledallocatoradapter.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_string.h>
#include <bteso_ipv4address.h>
#include <bteso_tcptimereventmanager.h>
#include <btemt_tcptimereventmanager.h>

#include <cstring>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This mechanism implements negotiation with a SOCKS5 proxy to establish a
// connection to the destination host. In order to test this component, a test
// class, 'btes5_TestServer' is used as a proxy to negotiate with.  The test
// scenarios create a SOCKS5 test server, connect to it (using TCP) and pass
// the connection socket to the 'btes5_Negotirator' object.
//
//-----------------------------------------------------------------------------
// CREATORS
// [2] btes5_Negotiator(bteso_TimerEventManager *eventManager, *bA = 0);
// [2] ~btes5_Negotiator();
// MANIPULATORS
// [2] int negotiate(*socket, destination, cB, timeout);
// [ ] int negotiate(*socket, destination, cB, timeout, credentials);
// [ ] int negotiate(*socket, destination, cB, timeout, provider);
//-----------------------------------------------------------------------------
// [ ] BREATHING TEST
// [2] USAGE EXAMPLE
// [ ] CONCERN: All memory allocation is from the object's allocator.
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;           // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
	LOOP3_ASSERT(text, file, line, false);
}

void cbSuccess(int status, btes5_Negotiator *negotiator)
{
    ASSERT(status == btes5_Negotiator::e_SUCCESS);
}

//=============================================================================
//               USAGE EXAMPLES
//-----------------------------------------------------------------------------
namespace {

///Example 1: Connection With Predefined Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following code snippets demonstrate how to use a 'btes5_Negotiator' to
// negotiate a SOCKS5 client-side handshake.  First we will declare and define
// the callback function. It will signal the main thread by setting 'state',
// which in turn is protected by a mutex and a condition variable.
//..
    void socks5Callback(btes5_Negotiator::NegotiationStatus result,
                        btes5_DetailedError                 error,
                        bcemt_Mutex                         *stateLock,
                        bcemt_Condition                     *stateChanged,
                        volatile int                        *state)
    {
        bcemt_LockGuard<bcemt_Mutex> lock(stateLock);
        if (result == btes5_Negotiator::e_SUCCESS) {
            *state = 1;
        } else {
            // report negotiation failure ...
            *state = -1;
        }
        stateChanged->signal();
    }
//..
// Next we define the function that will invoke the negotiator on the
// previously connected socket, using predefined name and password for
// authentication.
//..
    int negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
                  const bteso_Endpoint&                  destination)
    {
        btes5_Credentials credentials("john.smith", "PassWord123");
//..
// Next, we declare the variable for communicating the response, with a mutex
// and a condition variable to protect access to it from different threads.
//..
        bcemt_Mutex     stateLock;
        bcemt_Condition stateChanged;
        volatile int    state = 0; // value > 0 indicates success, < 0 is error
//..
// Then we will create an event manager and a 'btes5_Negotiator' and start
// negotiation.
//..
        btemt_TcpTimerEventManager eventManager;
        ASSERT(0 == eventManager.enable());
        btes5_Negotiator negotiator(&eventManager);
        using namespace bdef_PlaceHolders;
        negotiator.negotiate(socket,
                             destination,
                             bdef_BindUtil::bind(socks5Callback,
                                                 _1,
                                                 _2,
                                                 &stateLock,
                                                 &stateChanged,
                                                 &state),
                             bdet_TimeInterval(),
                             credentials);
//..
// Next, we block until the negotiation ends and 'socks5Callback' updates
// the 'state' variable.
//..
        bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
        while (!state) {
            stateChanged.wait(&stateLock);
        }
//..
// Finally, we return the status of SOCKS5 negotiation.  If '0 < state',
// 'socket' can be used to communicate with 'destination' through the proxy.
//..
        return state;
    }
//..
//
///Example 2: Connection With Dynamically Acquired Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The username and password are not available ahead of time; instead, the
// negotiator will use the provided object to acquire them, possibly by
// prompting the user. We'll assume 'socks5Callback' is already defined.
//
// First, we define a class derived from 'btes5_CredentialsProvider' which will
// provide username and password.
//..
    class MyCredentialsProvider : public btes5_CredentialsProvider {
      public:
        virtual void acquireCredentials(
            const bteso_Endpoint&                                  proxy,
            btes5_CredentialsProvider::SuppliedCredentialsCallback callback);
        virtual void cancelAcquiringCredentials();
    //
    };
    void MyCredentialsProvider::acquireCredentials(
            const bteso_Endpoint&                                  proxy,
            btes5_CredentialsProvider::SuppliedCredentialsCallback callback)
    {
        // normally we might prompt the user for username and password, but
        // here we use hard-coded values.
        callback(0, "User", "Password");
    }
    void MyCredentialsProvider::cancelAcquiringCredentials()
    {
        // we would normally cancel the user prompting process, if one is in
        // progress; but here it's a no-op.
    }
    int negotiateWithAcquiredCredentials(
        bteso_StreamSocket<bteso_IPv4Address> *socket,
        const bteso_Endpoint&                  destination)
    {
//..
// Finally we will create a 'btes5_Negotiator' and start negotiation, passing
// the address of a credentials provider object.
//..
        MyCredentialsProvider credentialsProvider;
        bcemt_Mutex     stateLock;
        bcemt_Condition stateChanged;
        volatile int    state = 0; // value > 0 indicates success, < 0 is error
        btemt_TcpTimerEventManager eventManager;
        ASSERT(0 == eventManager.enable());
        btes5_Negotiator negotiator(
            reinterpret_cast<bteso_TimerEventManager*>(&eventManager));
        using namespace bdef_PlaceHolders;
        negotiator.negotiate(socket,
                             destination,
                             bdef_BindUtil::bind(socks5Callback,
                                                 _1,
                                                 _2,
                                                 &stateLock,
                                                 &stateChanged,
                                                 &state),
                             bdet_TimeInterval(),
                             &credentialsProvider);
        bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
        while (!state) {
            stateChanged.wait(&stateLock);
        }
        return state;
    }
//..

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsls::Assert::setFailureHandler(assertHandler);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    const btes5_TestServerArgs::Severity verbosity
        = veryVerbose ? btes5_TestServerArgs::e_DEBUG
        : verbose     ? btes5_TestServerArgs::e_ERROR
        :               btes5_TestServerArgs::e_NONE;

    ASSERT(0 == bteso_SocketImpUtil::startup());  // needed on Windows,
                                                  // harmless on UNIX

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //:
        //: 2 The SOCKS5 negotiation protocol is correctly implemented.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //:
        //: 2 Use 'btes5_TestServer' as a SOCKS5 proxy to negotiate with.  The
        //:   server will respond positively to connection negotiation (without
        //:   actually trying to connect).
        //
        // Testing:
        //   USAGE EXAMPLE
        //   btes5_Negotiator(bteso_TimerEventManager *eventManager, *bA = 0);
        //   ~btes5_Negotiator();
        //   int negotiate(*socket, destination, cB, timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        btes5_TestServerArgs args;
        args.d_verbosity = verbosity;
        args.d_expectedDestination = bteso_Endpoint("example.com", 80);
        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;

        if (verbose) cout << "negotiate predefined credentials" << endl;
        {
            bteso_Endpoint proxy;
            args.d_expectedCredentials.set("john.smith", "PassWord123");
            btes5_TestServer server(&proxy, &args);
            if (veryVerbose) { cout << " proxy started on " << proxy << endl; }

            bteso_IPv4Address proxyAddress("127.0.0.1", proxy.port());
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
            ASSERT(socket);
            bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
                socketGuard(socket, &factory);

            int rc = socket->connect(proxyAddress);
            LOOP_ASSERT(rc, 0 == rc);

            rc = negotiate(socket, args.d_expectedDestination);
            LOOP_ASSERT(rc, rc > 0);
        }

        if (verbose) cout << "negotiate with acquired credentials" << endl;
        {
            bteso_Endpoint proxy;
            args.d_expectedCredentials.set("User", "Password");
            btes5_TestServer server(&proxy, &args);
            if (veryVerbose) { cout << " proxy started on " << proxy << endl; }

            bteso_IPv4Address proxyAddress("127.0.0.1", proxy.port());
            bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
            ASSERT(socket);
            bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
                socketGuard(socket, &factory);

            int rc = socket->connect(proxyAddress);
            LOOP_ASSERT(rc, 0 == rc);

            rc = negotiateWithAcquiredCredentials(socket,
                                                  args.d_expectedDestination);
            LOOP_ASSERT(rc, rc > 0);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC NEGOTIATION
        //   Negotiate a connection without authentication.
        //
        // Concerns
        //: 1 Basic negotiation works per RFC 1928 protocol.
        //
        //
        // Plan
        //: 1 Create a server thread using 'btes5_TestServer', and try to
        //:   connect.  The test server will check messages for validity and
        //:   simulate success (without actually connecting).
        //
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONNECT THROUGH PROXY" << endl
                                  << "=====================" << endl;

        btes5_TestServerArgs args;
        args.d_verbosity = verbosity;
        args.d_expectedPort = 8194;

        bteso_Endpoint proxy;
        btes5_TestServer server(&proxy, &args);

        bteso_IPv4Address proxyAddress("127.0.0.1", proxy.port());
        bteso_InetStreamSocketFactory<bteso_IPv4Address> factory;
        bteso_StreamSocket<bteso_IPv4Address> *socket = factory.allocate();
        ASSERT(socket);

        if (verbose) {
            cout << "Connecting to " << proxy
                 << " aka " << proxyAddress
                 << endl;
        }
        int rc = socket->connect(proxyAddress);
        ASSERT(0 == rc);

        btes5_Credentials credentials;

        bcemt_Mutex     stateLock;
        bcemt_Condition stateChanged;
        volatile int    state = 0; // value > 0 indicates success, < 0 is error
        using namespace bdef_PlaceHolders;
        btemt_TcpTimerEventManager eventManager;
        ASSERT(0 == eventManager.enable());
        btes5_Negotiator negotiator(&eventManager);
        veryVerbose && cout << "created a negotiator" << endl;

        bteso_Endpoint destination("nyplat1", args.d_expectedPort);
        veryVerbose && cout << "try to connect to " << destination << endl;
        negotiator.negotiate(socket,
                             destination,
                             bdef_BindUtil::bind(socks5Callback,
                                                        _1,
                                                        _2,
                                                        &stateLock,
                                                        &stateChanged,
                                                        &state),
                             bdet_TimeInterval());
        bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
        while (!state) {
            veryVeryVerbose && cout << "waiting for state change" << endl;
            stateChanged.wait(&stateLock);
        }
        veryVerbose && cout << "negotiation result: " << state << endl;

        factory.deallocate(socket);
        ASSERT(state > 0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Plan:
        //: 1 Create a 'btes5_Negotiatior' object, but do not actually
        //:   negotiate a connection.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        btemt_TcpTimerEventManager eventManager;
        ASSERT(0 == eventManager.enable());
        btes5_Negotiator negotiator(&eventManager);

      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
