// btls5_negotiator.t.cpp                                             -*-C++-*-
#include <btls5_negotiator.h>

#include <btls5_testserver.h>  // for testing only

#include <bdlma_concurrentallocatoradapter.h>
#include <bdlqq_condition.h>
#include <bdlqq_lockguard.h>
#include <bdlqq_mutex.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bsl_iostream.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <btlmt_tcptimereventmanager.h>
#include <btlso_inetstreamsocketfactory.h>
#include <btlso_ipv4address.h>
#include <btlso_socketimputil.h>

#include <cstring>
#include <bsl_string.h>

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
// class, 'btls5::TestServer' is used as a proxy to negotiate with.  The test
// scenarios create a SOCKS5 test server, connect to it (using TCP) and pass
// the connection socket to the 'btes5::Negotirator' object.
//
//-----------------------------------------------------------------------------
// CREATORS
// [2] btls5::Negotiator(*eventManager, *bA = 0);
// [2] ~btls5::Negotiator();
//
// MANIPULATORS
// [2] makeNegotiationHandle(*socket, dst, cb);
// [2] makeNegotiationHandle(*socket, dst, cb, timeout);
// [ ] makeNegotiationHandle(*socket, dst, cb, credentials);
// [3] makeNegotiationHandle(*socket, dst, cb, timeout, credentials);
// [2] startNegotiation(const NegotiationHandle& negotiation);
// [ ] cancelNegotiation(const NegotiationHandle& negotiation);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE
// [ ] CONCERN: All memory allocation is from the object's allocator.
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J \
          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;           // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef btls5::Negotiator Obj;

void assertHandler(const char *text, const char *file, int line)
{
    LOOP3_ASSERT(text, file, line, false);
}

void cbSuccess(int status, Obj *negotiator)
{
    ASSERT(status == Obj::e_SUCCESS);
}

//=============================================================================
//               USAGE EXAMPLES
//-----------------------------------------------------------------------------
namespace {

///Example 1: Negotiation With Predefined Credentials
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code snippets demonstrate how to use a 'btls5::Negotiator' to
// negotiate a SOCKS5 client-side handshake.
//
// First, we define the callback function that will receive the results of the
// negotiation.  It will signal the main thread by setting 'state', which in
// turn is protected by a mutex and a condition variable:
//..
    void socks5Callback(btls5::Negotiator::NegotiationStatus  result,
                        const btls5::DetailedStatus&          error,
                        int                                 *state,
                        bdlqq::Mutex                         *stateLock,
                        bdlqq::Condition                     *stateChanged)
    {
        bdlqq::LockGuard<bdlqq::Mutex> lock(stateLock);
        if (btls5::Negotiator::e_SUCCESS == result) {
            *state = 0;
        } else {
            // report negotiation failure ...
            *state = -1;
        }
        stateChanged->signal();
    }
//..
// Next, we define the function that will invoke the negotiator on the
// previously connected socket, using predefined username and password for
// authentication:
//..
    int negotiate(btlso::StreamSocket<btlso::IPv4Address> *socket,
                  const btlso::Endpoint&                  destination)
    {
        btls5::Credentials credentials("john.smith", "PassWord123");
//..
// Then, we declare the variable for communicating the response, with a mutex
// and a condition variable to protect access to it from different threads:
//..
        bdlqq::Mutex     stateLock;
        bdlqq::Condition stateChanged;
        int             state = 1;
            // 'state == 1' means negotiation is still in progress.
//..
// Next, we create an event manager and a 'btls5::Negotiator' and start
// negotiation:
//..
        btlmt::TcpTimerEventManager eventManager;
        int rc = eventManager.enable();
        ASSERT(0 == rc);
        btls5::Negotiator negotiator(&eventManager);

        using namespace bdlf::PlaceHolders;
        btls5::Negotiator::NegotiationHandle
            handle = negotiator.makeNegotiationHandle(
                                            socket,
                                            destination,
                                            bdlf::BindUtil::bind(socks5Callback,
                                                                _1,
                                                                _2,
                                                                &state,
                                                                &stateLock,
                                                                &stateChanged),
                                            bsls::TimeInterval(),
                                            credentials);
        negotiator.startNegotiation(handle);
//..
// Now, we wait until the negotiation ends and 'socks5Callback' updates the
// 'state' variable:
//..
        bdlqq::LockGuard<bdlqq::Mutex> lock(&stateLock);
        while (1 == state) {
            stateChanged.wait(&stateLock);
        }
//..
// Finally, we return the status of SOCKS5 negotiation.  If '0 == state',
// 'socket' can be used to communicate with 'destination' through the proxy:
//..
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

    const btls5::TestServerArgs::Severity verbosity
        = veryVerbose ? btls5::TestServerArgs::e_DEBUG
        : verbose     ? btls5::TestServerArgs::e_ERROR
        :               btls5::TestServerArgs::e_NONE;

    ASSERT(0 == btlso::SocketImpUtil::startup());  // needed on Windows,
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
        //: 2 Use 'btls5::TestServer' as a SOCKS5 proxy to negotiate with.  The
        //:   server will respond positively to connection negotiation (without
        //:   actually trying to connect).
        //
        // Testing:
        //   USAGE EXAMPLE
        //   btls5::Negotiator(*eventManager, *bA = 0);
        //   ~btls5::Negotiator();
        //   int negotiate(*socket, destination, cB, timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        btls5::TestServerArgs args;
        args.d_verbosity = verbosity;
        args.d_expectedDestination = btlso::Endpoint("example.com", 80);
        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;

        if (verbose) cout << "negotiate credentials" << endl;
        {
            btlso::Endpoint proxy;
            args.d_expectedCredentials.set("john.smith", "PassWord123");
            btls5::TestServer server(&proxy, &args);
            if (veryVerbose) { cout << " proxy started on " << proxy << endl; }

            btlso::IPv4Address proxyAddress("127.0.0.1", proxy.port());
            btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
            ASSERT(socket);
            btlso::StreamSocketFactoryAutoDeallocateGuard<btlso::IPv4Address>
                socketGuard(socket, &factory);

            int rc = socket->connect(proxyAddress);
            LOOP_ASSERT(rc, 0 == rc);

            rc = negotiate(socket, args.d_expectedDestination);
            LOOP_ASSERT(rc, rc == 0);
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
        // Plan
        //: 1 Create a server thread using 'btls5::TestServer', and try to
        //:   connect.  The test server will check messages for validity and
        //:   simulate success (without actually connecting).
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONNECT THROUGH PROXY" << endl
                                  << "=====================" << endl;

        btls5::TestServerArgs args;
        args.d_verbosity = verbosity;
        args.d_expectedPort = 8194;

        btlso::Endpoint proxy;
        btls5::TestServer server(&proxy, &args);

        btlso::IPv4Address proxyAddress("127.0.0.1", proxy.port());
        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlso::StreamSocket<btlso::IPv4Address> *socket = factory.allocate();
        ASSERT(socket);

        if (verbose) {
            cout << "Connecting to " << proxy
                 << " aka " << proxyAddress
                 << endl;
        }
        int rc = socket->connect(proxyAddress);
        ASSERT(0 == rc);

        btls5::Credentials credentials;

        bdlqq::Mutex     stateLock;
        bdlqq::Condition stateChanged;
        int             state = 0; // value > 0 indicates success, < 0 is error
        using namespace bdlf::PlaceHolders;

        btlmt::TcpTimerEventManager eventManager;
        rc = eventManager.enable();
        ASSERT(0 == rc);

        Obj negotiator(&eventManager);
        veryVerbose && cout << "created a negotiator" << endl;

        btlso::Endpoint destination("nyplat1", args.d_expectedPort);
        veryVerbose && cout << "try to connect to " << destination << endl;
        Obj::NegotiationHandle handle = negotiator.makeNegotiationHandle(
                                            socket,
                                            destination,
                                            bdlf::BindUtil::bind(socks5Callback,
                                                                _1,
                                                                _2,
                                                                &state,
                                                                &stateLock,
                                                                &stateChanged),
                                            bsls::TimeInterval());
        negotiator.startNegotiation(handle);
        bdlqq::LockGuard<bdlqq::Mutex> lock(&stateLock);
        while (1 == state) {
            veryVeryVerbose && cout << "waiting for state change" << endl;
            stateChanged.wait(&stateLock);
        }
        veryVerbose && cout << "negotiation result: " << state << endl;

        factory.deallocate(socket);
        ASSERT(state == 0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Plan:
        //: 1 Create a 'btes5::Negotiatior' object, but do not actually
        //:   negotiate a connection.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        btlmt::TcpTimerEventManager eventManager;
        int rc = eventManager.enable();
        ASSERT(0 == rc);

        Obj negotiator(&eventManager);

      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
      }
    }
    ASSERT(0 == btlso::SocketImpUtil::cleanup());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
