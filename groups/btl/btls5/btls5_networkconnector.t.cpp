// btls5_networkconnector.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_networkconnector.h>

#include <btls5_testserver.h>
#include <btls5_networkdescriptionutil.h>

#include <bdlcc_fixedqueue.h>
#include <bslma_testallocator.h>            // thread-safe allocator
#include <bslmt_threadutil.h>               // for sleep
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bsl_iostream.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsl_sstream.h>
#include <btlmt_session.h>
#include <btlmt_sessionfactory.h>
#include <btlmt_sessionpool.h>
#include <btlso_inetstreamsocketfactory.h>
#include <btlso_socketimputil.h>

#include <bsl_cerrno.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component implements connection establishment through a network of
// SOCKS5 proxies.  In order to test it, 'btls5::TestServer' is used to
// implement a SOCKS5 prixy.  In some tests, more than one test servers are
// connected (cascaded) to test connection through more than one level of
// proxy.  A test server is also used to test failure scenarios such as timeout
// and failover.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btls5::NetworkConnector(net, fact, eventMgr, *a = 0);
// [ 2] btls5::NetworkConnector(net, fact, eventMgr, min, max, *a = 0);
// [ 2] ~btls5::NetworkConnector();
// MANIPULATORS
// [ 2] makeConnectionAttemptHandle(cB, proxyTO, totalTO, server);
// [ 2] startConnectionAttempt(ConnectionAttemptHandle& attempt);
// [11] startConnectionAttemptStrict(ConnectionAttemptHandle& attempt);
// [ 8] cancelConnectionAttempt(ConnectionAttemptHandle& attempt);
// ACCESSORS
// [ 9] socks5Servers()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ 2] CONCERN: All memory allocation is from the object's allocator.
// [ 3] CONCERN: Timeout when the proxy doesn't respond.
// [ 3] CONCERN: Failover to second proxy after the first proxy timeout.
// [ 4] CONCERN: Failover to second proxy after the first proxy failure.
// [  ] CONCERN: Failover to second proxy after the first proxy doesn't exist.
// [ 5] CONCERN: Failure when the only proxy can't be resolved.
// [  ] CONCERN: Failure when the only proxy fails (destination unreachable).
// [10] CONCERN: Normal connection with proxy and destination as hostname.
// [  ] CONCERN: Normal connection with proxy as IP and destination as
//               hostname.
// [  ] CONCERN: Normal connection with proxy and destination as IP.
// [ 6] CONCERN: Two overlapping connection attempts on two connectors.
// [-2] CONCERN: Two overlapping connection attempts on one connector.
// [ 3] CONCERN: Total timeout before proxy timeout.
// [ 4] CONCERN: Connection with 2 levels.
// [ 4] CONCERN: Two proxy levels, one proxy in each level failing.
// [ 7] CONCERN: Connection with 3 levels.
// [  ] CONCERN: Connection with 3 levels, with 2 proxies in each level
//               failing.
// [  ] CONCERN: Cancelling a connection attempt.
// [ 4] CONCERN: Connect with construction-time username/password.
// [ 2] CONCERN: Import connected socket into 'btlmt::SessionPool'.
// [10] CONCERN: Failure to bind a specified source port is not fatal.
// [-1] CONCERN: Negotiate using an off-the-shelf SOCKS5 proxy.


// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
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
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                 GLOBAL TYPEDEFS AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

bool             verbose = 0;
bool         veryVerbose = 0;
bool     veryVeryVerbose = 0;
bool veryVeryVeryVerbose = 0;

// ============================================================================
//                    CLASSES AND FUNCTIONS USED IN TESTS
// ----------------------------------------------------------------------------
void breathingTestCb(
   btls5::NetworkConnector::ConnectionStatus                     status,
   btlso::StreamSocket<btlso::IPv4Address>                      *socket,
   btlso::StreamSocketFactory<btlso::IPv4Address>               *socketFactory,
   const btls5::DetailedStatus&                                  error,
   bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> *queue)
    // Process a SOCKS5 connection response with the specified 'status' and
    // 'error', with the specified 'socket' allocated by the specified
    // 'socketFactory', and append 'status' to the specified 'queue'.
{
    if (verbose) {
        cout << "Connection attempt concluded with"
             << " status=" << status << " error=" << error << endl;
    }
    if (btls5::NetworkConnector::e_SUCCESS == status) {
        socketFactory->deallocate(socket);
    }
    queue->pushBack(status);
}

// ============================================================================
//                    btlmt::SessionPool testing callback
// ----------------------------------------------------------------------------
void poolStateCb(int state, int source, void *)
{
    if (verbose) {
        cout << "Session Pool: state=" << state
             << " source=" << source
             << endl;
    }
}

void sessionStateCb(int            state,
                    int            ,
                    btlmt::Session *,
                    void           *)
{
    if (veryVerbose) { T_ P(state) }
}

struct Session : public btlmt::Session {
  public:
    virtual int start()
    {
        if (veryVerbose) { cout << "Session::start" << endl; }
        return 0;
    }

    virtual int stop()
    {
        if (veryVerbose) { cout << "Session::stop" << endl; }
        return 0;
    }

    virtual btlmt::AsyncChannel *channel() const
    {
        return 0;
    }

    virtual ~Session()
    {
    }
};

class SessionFactory : public btlmt::SessionFactory {
  public:
    virtual void allocate(btlmt::AsyncChannel                    *,
                          const btlmt::SessionFactory::Callback&  cb)
    {
        cb(0, new Session);
        if (veryVerbose) { cout << "SessionFactory::allocate" << endl; }
    }

    virtual void deallocate(btlmt::Session *session)
    {
        delete dynamic_cast<Session*>(session);
        if (veryVerbose) { cout << "SessionFactory::deallocate" << endl; }
    }
};

void socks5Cb(btls5::NetworkConnector::ConnectionStatus       status,
              btlso::StreamSocket<btlso::IPv4Address>        *socket,
              btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
              const btls5::DetailedStatus&                    error,
              btlmt::SessionPool                             *sessionPool,
              bslmt::Mutex                                   *stateLock,
              bslmt::Condition                               *stateChanged,
              volatile int                                   *state)
{
    bslmt::LockGuard<bslmt::Mutex> lock(stateLock);
    if (status == btls5::NetworkConnector::e_SUCCESS) {
        *state = 1;
        if (verbose) cout << "connection succeeded" << endl;

        SessionFactory sessionFactory;
        int handle;
        using namespace bdlf::PlaceHolders;
        int rc = sessionPool->import(&handle,
                                     bdlf::BindUtil::bind(sessionStateCb,
                                                         _1, _2, _3, _4),
                                     socket,
                                     socketFactory,
                                     &sessionFactory);

        if (verbose) cout << "btlmt::SessionPool::import rc=" << rc << endl;
        *state = rc ? -1 : 1;  // 'rc == 0' is success
        sessionPool->stop();
    } else {
        *state = -1;
        cout << "Connect failed " << status << ": " << error << endl;
    }
    stateChanged->signal();
}

// ============================================================================
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Example 1: Connect to a Server Through Two Proxy Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to connect to a server reachable through two levels of
// proxies: first through one of our corporate SOCKS5 servers, and then through
// one of the regional SOCKS5 servers.
//
// First, we define a callback function to process connection status, and if
// successful, perform useful work and finally deallocate the socket.  After
// the work is done (or an error is reported) we signal the main thread with
// the status; this also signifies that we no longer need the stream factory
// passed to us:
//..
    void connectCb(
                int                                             status,
                btlso::StreamSocket< btlso::IPv4Address>       *socket,
                btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
                const btls5::DetailedStatus&                    detailedStatus,
                bslmt::Mutex                                   *stateLock,
                bslmt::Condition                               *stateChanged,
                volatile int                                   *state)
    {
        if (0 == status) {
            // Success: conduct I/O operations with 'socket' ... and deallocate
            socketFactory->deallocate(socket);
        } else {
            cout << "Connect failed " << status << ": " << detailedStatus
                 << endl;
        }
        bslmt::LockGuard<bslmt::Mutex> lock(stateLock);
        *state = status ? -1 : 1; // 1 for success, -1 for failure
        stateChanged->signal();
    }
//..
// Then, we define the function that will set up the proxy network description
// and attempt to connect to a remote host.  The first level of proxies should
// be reachable directly:
//..
    static int connectThroughProxies(const btlso::Endpoint& corpProxy1,
                                     const btlso::Endpoint& corpProxy2)
    {
        btls5::NetworkDescription proxies;
        proxies.addProxy(0, corpProxy1);
        proxies.addProxy(0, corpProxy2);
//..
// Next, we add a level for regional proxies reachable from the corporate
// proxies.  Note that '.tk' stands for Tokelau in the Pacific Ocean:
//..
        proxies.addProxy(1, btlso::Endpoint("proxy1.example.tk", 1080));
        proxies.addProxy(1, btlso::Endpoint("proxy2.example.tk", 1080));
//..
// Then, we set the username and password, which will be used in case one of
// the proxies in the connection path requires that type of authentication:
//..
        btls5::Credentials credentials("John.smith", "pass1");
        btls5::NetworkDescriptionUtil::setAllCredentials(&proxies,
                                                         credentials);
//..
// Now we construct a 'btls5::NetworkConnector' that will be used to connect
// to one or more destinations:
//..
        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();
        btls5::NetworkConnector connector(proxies, &factory, &eventManager);
//..
// Finally we attempt to connect to the destination.  Input, output, and
// eventual closing of the connection will be handled from 'connectCb', which
// will signal by changing 'state', with the access protected by a mutex and
// condition variable:
//..
        const bsls::TimeInterval proxyTimeout(5.0);
        const bsls::TimeInterval totalTimeout(30.0);
        bslmt::Mutex     stateLock;
        bslmt::Condition stateChanged;
        volatile int     state = 0; // value > 0 indicates success, < 0 error
        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
           connector.makeConnectionAttemptHandle(bdlf::BindUtil::bind(
                                                              connectCb,
                                                              _1, _2, _3, _4,
                                                              &stateLock,
                                                              &stateChanged,
                                                              &state),
                                                 proxyTimeout,
                                                 totalTimeout,
                                                 btlso::Endpoint(
                                                     "destination.example.com",
                                                     8194));
        connector.startConnectionAttempt(attempt);
        bslmt::LockGuard<bslmt::Mutex> lock(&stateLock);
        while (!state) {
            stateChanged.wait(&stateLock);
        }
        return state;
    }
//..

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    const btls5::TestServerArgs::Severity verbosity =
        veryVeryVerbose ? btls5::TestServerArgs::e_TRACE
                        : veryVerbose
                        ? btls5::TestServerArgs::e_DEBUG
                        : verbose
                        ? btls5::TestServerArgs::e_ERROR
                        : btls5::TestServerArgs::e_NONE;

    // preserve allocator for test apparatus

    bslma::Allocator *alloc = bslma::Default::defaultAllocator();

    ASSERT(0 == btlso::SocketImpUtil::startup());  // needed on Windows,
                                                  // harmless on UNIX

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example compiles and can be executed successfully.
        //
        // Plan:
        //: 1 Create 2 levels of proxies configured to simulate connection.
        //: 2 Uncomment the usage example.
        //: 3 Verify successful compilation and execution.
        //
        // Testing:
        //   USAGE EXAMPLE
        //   CONCERN: Normal connection with proxy and destination as hostname.
        //   CONCERN: Connection with 2 levels.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        btlso::Endpoint destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        destinationArgs.d_mode = btls5::TestServerArgs::e_IGNORE;
        btls5::TestServer destinationServer(&destination, &destinationArgs);

        btlso::Endpoint region;
        btls5::TestServerArgs regionArgs;
        regionArgs.d_verbosity = verbosity;
        regionArgs.d_label = "regionProxy";
        regionArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
        regionArgs.d_expectedDestination.set("destination.example.com", 8194);
        regionArgs.d_destination = destination; // override connection address
        btls5::TestServer regionServer(&region, &regionArgs);

        btls5::TestServerArgs corpArgs;
        corpArgs.d_verbosity = verbosity;
        corpArgs.d_label = "corpProxy";
        corpArgs.d_destination = region; // override connection address
        corpArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
        btlso::Endpoint proxy;
        btls5::TestServer proxyServer(&proxy, &corpArgs);

        ASSERT(connectThroughProxies(proxy, proxy) > 0);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // STRICT SOURCE RANGE FAILURE
        //   Test that failure to bind a specified source port is fatal
        //   in strict mode.
        //
        // Concerns:
        //: 1 When using 'startConnectionAttemptStrict', if a port within a
        //:   specified range cannot be bound, the connection fails.
        //
        // Plan:
        //: 1 Create a test server instance 'destination'.
        //: 2 Create a test server 'proxy'.
        //: 3 Specify a port normally in use (Windows) and reserved (UNIX).
        //: 3 Connect to 'destination' through 'proxy' using SOCKS5.
        //: 4 Verify the connection attempt fails.
        //
        // Testing:
        //   startConnectionAttemptStrict(ConnectionAttemptHandle& attempt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRICT SOURCE RANGE FAILURE" << endl
                          << "===========================" << endl;

        // install a 'TestAllocator' as defalt to check for memory leaks

        bslma::TestAllocator ta("test1", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&ta);

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);
        if (verbose) {
            cout << "destination server started on " << destination << endl;
        }

        btlso::Endpoint       proxy;
        btls5::TestServerArgs proxyArgs;
        proxyArgs.d_verbosity = verbosity;
        proxyArgs.d_label = "proxy";
        proxyArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxyServer(&proxy, &proxyArgs);
        if (verbose) {
            cout << "proxy server started on " << proxy << endl;
        }

        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();


        const int minSourcePort = 445;  // SMB over IP
        const int maxSourcePort = minSourcePort;
        btls5::NetworkConnector connector(proxies,
                                          &factory,
                                          &eventManager,
                                          minSourcePort,
                                          maxSourcePort);
        const bsls::TimeInterval proxyTimeout(2);
        const bsls::TimeInterval totalTimeout(10);
        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
            connector.makeConnectionAttemptHandle(
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                              _1, _2, _3, _4,
                                                              &queue),
                                          proxyTimeout,
                                          totalTimeout,
                                          destination);

        connector.startConnectionAttemptStrict(attempt);

        // wait for connection result and check for failure

        btls5::NetworkConnector::ConnectionStatus status;
        queue.popFront(&status);
        ASSERT(btls5::NetworkConnector::e_ERROR == status);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // SOURCE RANGE FAILURE
        //   Test that failure to bind a specified source port is not fatal.
        //
        // Concerns:
        //: 1 If a port within a specified range cannot be bound, the
        //:   connection proceeds anyway.
        //
        // Plan:
        //: 1 Create a test server instance 'destination'.
        //: 2 Create a test server 'proxy'.
        //: 3 Specify a port normally in use (Windows) and reserved (UNIX).
        //: 3 Connect to 'destination' through 'proxy' using SOCKS5.
        //: 4 Verify that the callback is invoked with successful result.
        //
        // Testing:
        //   SOURCE RANGE FAILURE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SOURCE RANGE FAILURE" << endl
                          << "====================" << endl;

        // install a 'TestAllocator' as defalt to check for memory leaks

        bslma::TestAllocator ta("test1", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&ta);

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);
        if (verbose) {
            cout << "destination server started on " << destination << endl;
        }

        btlso::Endpoint       proxy;
        btls5::TestServerArgs proxyArgs;
        proxyArgs.d_verbosity = verbosity;
        proxyArgs.d_label = "proxy";
        proxyArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxyServer(&proxy, &proxyArgs);
        if (verbose) {
            cout << "proxy server started on " << proxy << endl;
        }

        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();


        const int minSourcePort = 445;  // SMB over IP
        const int maxSourcePort = minSourcePort;
        btls5::NetworkConnector connector(proxies,
                                          &factory,
                                          &eventManager,
                                          minSourcePort,
                                          maxSourcePort);
        const bsls::TimeInterval proxyTimeout(2);
        const bsls::TimeInterval totalTimeout(10);
        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
            connector.makeConnectionAttemptHandle(
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                              _1, _2, _3, _4,
                                                              &queue),
                                          proxyTimeout,
                                          totalTimeout,
                                          destination);

        connector.startConnectionAttempt(attempt);

        // wait for connection result and check for success

        btls5::NetworkConnector::ConnectionStatus status;
        queue.popFront(&status);
        ASSERT(btls5::NetworkConnector::e_SUCCESS == status);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ACCESSOR: socks5Servers
        //
        // Concerns:
        //: 1 The same network description that is provided during construction
        //:   is returned by the accessor.
        //: 2 The network description passed in to the connector is not
        //:   modified.
        //
        // Plan:
        //: 1 Create 'btls5::NetworkConnector' with different socks 5 servers
        //: 2 Verify the value returned by the accessor is the expected one.
        //
        // Testing:
        //   socks5Servers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nACCESSOR: socks5Servers"
                             "\n=======================\n" << bsl::flush;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        const btls5::Credentials credentials("gooduser", "goodpass");

        bslma::TestAllocator ta("testAllocator", veryVeryVerbose);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&ta);
        btlmt::TcpTimerEventManager eventManager(&ta);

        for (int LEVELS = 1; LEVELS < 4; ++LEVELS) {
            for (int SERVERS = 1; SERVERS < 10; ++SERVERS) {

                btls5::NetworkDescription proxies(&ta);
                for (int level = 0; level < LEVELS; ++level) {
                    for (int server = 0; server < SERVERS; ++server) {
                        btlso::Endpoint proxy("proxy", 8000 + server, &ta);
                        proxies.addProxy(level, proxy, credentials);
                    }
                }
                btls5::NetworkDescription copy(proxies, &ta);
                btls5::NetworkConnector   connector(proxies,
                                                    &factory,
                                                    &eventManager);

                const int testCase = LEVELS * 100 + SERVERS;
                LOOP_ASSERT(testCase, proxies == copy);
                LOOP_ASSERT(testCase, proxies == connector.socks5Servers());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CANCEL TEST
        //
        // Concerns:
        //: 1 Upon connection attempt cancelation the callback is invoked with
        //:   the appropriate status.
        //
        // Plan:
        //: 1 Connect using an unresponsive proxy with a long timeout.
        //: 2 Verify that the callback status is 'e_CANCEL'.
        //
        // Testing:
        //   cancelConnectionAttempt(ConnectionAttemptHandle& attempt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CANCEL TEST" << endl
                          << "===========" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);

        btlso::Endpoint       proxy;
        btls5::TestServerArgs proxyArgs;
        proxyArgs.d_verbosity = verbosity;
        proxyArgs.d_label = "unresponsive proxy";
        proxyArgs.d_mode = btls5::TestServerArgs::e_IGNORE;
        btls5::TestServer unresponsiveServer(&proxy, &proxyArgs);

        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        btls5::NetworkConnector::ConnectionStatus status;

        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                               _1,
                                                               _2,
                                                               _3,
                                                               _4,
                                                               &queue);
        {
            const bsls::TimeInterval TO(9);
            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  TO, // proxy
                                                                  TO, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            connector.cancelConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_CANCEL == status);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 3-LEVEL CASCADED PROXIES
        //  Test of connectivity using 3 levels of proxies.
        //
        // Concerns:
        //: 1 Connection with 3 levels of proxies.
        //
        // Plan:
        //: 1 Using a 'btls5::TestServer' construct a destination server.
        //: 2 Make a 3-level proxy network with some "bad" proxies:
        //:   1 One unresponsive proxy in level 0
        //:   2 One proxy with non-matching credentials in level 1
        //: 3 Verify successful connection to destination.
        //
        // Testing:
        //   CONCERN: Connection with 3 levels.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "3-LEVEL CASCADED PROXIES" << endl
                          << "========================" << endl;

        btls5::TestServerArgs args;  // reused for all servers
        args.d_verbosity = verbosity;
        const btls5::Credentials credentials("gooduser", "goodpass");

        btlso::Endpoint destination;
        args.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &args);

        btlso::Endpoint proxyAddress;  // address of just-started proxy server
        btls5::NetworkDescription proxies;

        args.d_label = "unresponsive-0.0";
        args.d_mode = btls5::TestServerArgs::e_IGNORE;
        btls5::TestServer proxy00(&proxyAddress, &args);
        proxies.addProxy(0, proxyAddress, credentials);

        args.d_label = "proxy-0.1";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxy01(&proxyAddress, &args);
        proxies.addProxy(0, proxyAddress, credentials);

        args.d_label = "proxy-0.2";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxy02(&proxyAddress, &args);
        proxies.addProxy(0, proxyAddress, credentials);

        args.d_label = "bad-credentials-1.0";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_expectedCredentials.set("baduser", "badpass");
        btls5::TestServer proxy10(&proxyAddress, &args);
        proxies.addProxy(1, proxyAddress, credentials);

        args.d_label = "proxy-1.1";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_expectedCredentials.set("gooduser", "goodpass");
        btls5::TestServer proxy11(&proxyAddress, &args);
        proxies.addProxy(1, proxyAddress, credentials);

        args.d_label = "proxy-2.0";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_expectedCredentials.set("gooduser", "goodpass");
        btls5::TestServer proxy20(&proxyAddress, &args);
        proxies.addProxy(2, proxyAddress, credentials);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        btls5::NetworkConnector::ConnectionStatus status;

        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                 bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4, &queue);

        if (veryVerbose) { P_(destination) P(proxies) }

        btls5::NetworkConnector connector(proxies, &factory, &eventManager);

        const bsls::TimeInterval T1(0.5), T2(5.0);

        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
        connector.startConnectionAttempt(attempt);
        queue.popFront(&status);
        LOOP_ASSERT(status, btls5::NetworkConnector::e_SUCCESS == status);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // OVERLAPPED CONNECTIONS
        //  Test concurrency through overlapping conection attempts.
        //
        // Concerns:
        //: 1 Two connections can be established concurrently, using different
        //:   connectors and different proxies.
        //
        // Plan:
        //: 1 Using a table-driven method:
        //:   1 Start two test proxies with a given response-delays
        //:   1 Construct and start a 'btls5::NetworkConnector' using one proxy
        //:   2 After a delay, start the second connector via the other proxy
        //:   3 Check for successful connection using the two connectors.
        //
        // Testing:
        //   CONCERN: Two overlapping connection attempts on two connectors.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OVERLAPPED CONNECTIONS" << endl
                          << "======================" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocator(&defaultAllocator);

        btls5::TestServerArgs args;  // reused for all servers
        args.d_verbosity = verbosity;
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        const btls5::Credentials credentials("gooduser", "goodpass");
        args.d_expectedCredentials = credentials;

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        btlso::Endpoint destination;
        args.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &args);

        btlso::Endpoint proxy;  // address of just-started proxy server

        const bsls::TimeInterval proxyTimeout(2.0);
        const bsls::TimeInterval totalTimeout(5.0);

        static const struct {
            int    d_line;                // source line number
            double d_proxy1DelaySeconds;  // delay before every response
            double d_proxy2DelaySeconds;  // delay before every response
            double d_lagSeconds;          // delay before starting attempt
        } DATA[] = {
        //LINE DELAY1  DELAY2    LAG
        //--   ------  ------  -----
        { L_,   0.000,  0.000, 0.000 },  // baseline: no delay

        { L_,   0.000,  0.000, 0.000 },  // simultaneous start with delays
        { L_,   0.010,  0.000, 0.000 },
        { L_,   0.010,  0.010, 0.000 },
        { L_,   0.040,  0.010, 0.000 },
        { L_,   0.040,  0.040, 0.000 },
        { L_,   0.010,  0.040, 0.000 },
        { L_,   0.000,  0.040, 0.000 },

        { L_,   0.000,  0.000, 0.010 },  // lag 10 mS start with delays
        { L_,   0.010,  0.000, 0.010 },
        { L_,   0.010,  0.010, 0.010 },
        { L_,   0.040,  0.010, 0.010 },
        { L_,   0.040,  0.040, 0.010 },
        { L_,   0.010,  0.040, 0.010 },
        { L_,   0.000,  0.040, 0.010 },

        { L_,   0.000,  0.000, 0.040 },  // lag 40 mS start with delays
        { L_,   0.010,  0.000, 0.040 },
        { L_,   0.010,  0.010, 0.040 },
        { L_,   0.040,  0.010, 0.040 },
        { L_,   0.040,  0.040, 0.040 },
        { L_,   0.010,  0.040, 0.040 },
        { L_,   0.000,  0.040, 0.040 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Concurrent attempts via 2 connectors" << endl;

        for (int t1 = 0; t1 < NUM_DATA; t1++) {
            const int                LINE  (DATA[t1].d_line);
            const bsls::TimeInterval DELAY1(DATA[t1].d_proxy1DelaySeconds);
            const bsls::TimeInterval DELAY2(DATA[t1].d_proxy2DelaySeconds);
            const bsls::TimeInterval LAG   (DATA[t1].d_lagSeconds);

            if (veryVeryVerbose) { T_ P_(LINE) P_(DELAY1) P_(DELAY2) P(LAG) }

            using namespace bdlf::PlaceHolders;

            args.d_delay = DELAY1;
            args.d_label = "proxy1";
            btls5::TestServer         proxy1(&proxy, &args);
            btls5::NetworkDescription proxies1;
            proxies1.addProxy(0, proxy, credentials);
            btls5::NetworkConnector connector1(proxies1,
                                               &factory,
                                               &eventManager);

            bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus>
                                                                     queue1(1);
            btls5::NetworkConnector::ConnectionStatus status1;
            btls5::NetworkConnector::ConnectionStateCallback cb1 =
                          bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4,
                                               &queue1);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt1 =
                           connector1.makeConnectionAttemptHandle(cb1,
                                                                  proxyTimeout,
                                                                  totalTimeout,
                                                                  destination);
            connector1.startConnectionAttempt(attempt1);

            args.d_delay = DELAY2;
            args.d_label = "proxy2";
            btls5::TestServer         proxy2(&proxy, &args);
            btls5::NetworkDescription proxies2;
            proxies2.addProxy(0, proxy, credentials);
            btls5::NetworkConnector connector2(proxies2,
                                               &factory,
                                               &eventManager);
            bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus>
                                                                     queue2(1);
            btls5::NetworkConnector::ConnectionStatus status2;
            btls5::NetworkConnector::ConnectionStateCallback cb2 =
                          bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4,
                                               &queue2);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt2 =
                           connector2.makeConnectionAttemptHandle(cb2,
                                                                  proxyTimeout,
                                                                  totalTimeout,
                                                                  destination);
            if (bsls::TimeInterval() != LAG) {
                bslmt::ThreadUtil::sleep(LAG);
            }
            connector2.startConnectionAttempt(attempt2);

            // wait for connection results and check for success

            queue1.popFront(&status1);
            LOOP2_ASSERT(LINE, status1,
                         btls5::NetworkConnector::e_SUCCESS == status1);
            queue2.popFront(&status2);
            LOOP2_ASSERT(LINE, status2,
                         btls5::NetworkConnector::e_SUCCESS == status2);
        }

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // UNRESOLVED PROXY
        //
        // Concerns:
        //: 1 Proper timeout processing when a proxy is not resolvable.
        //
        // Plan:
        //: 1 Connect using a bogus proxy and verify error code.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNRESOLVED PROXY" << endl
                          << "================" << endl;

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);

        btlso::Endpoint proxy("unresolvable-proxy.bogus-domain.com", 8100);
        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        btls5::NetworkConnector::ConnectionStatus status;

        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                 bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4, &queue);
        {
            const bsls::TimeInterval T1(1), T2(5);

            if (verbose) cout << "\nUnresolvable proxy" << endl;
            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_ERROR == status);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CASCADED PROXIES
        //  Test of connectivity using 2 levels of proxies.
        //
        // Concerns:
        //: 1 Connections through 2 levels, with a failure in each level.
        //: 2 proper authentication of username and password.
        //
        // Plan:
        //: 1 Using a 'btls5::TestServer' construct a destination server.
        //: 2 Make a 2-level proxy network with one "bad" proxy in each level:
        //:   1 One unresponsive proxy
        //:   2 One proxy with non-matching credentials
        //: 3 Verify successful connection to destination.
        //
        // Testing:
        //   CONCERN: Failover to second proxy after the first proxy failure.
        //   CONCERN: Two proxy levels, one proxy in each level failing.
        //   CONCERN: Connect with construction-time username/password.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CASCADED PROXIES" << endl
                          << "================" << endl;

        btls5::TestServerArgs args;  // reused for all servers
        args.d_verbosity = verbosity;
        const btls5::Credentials credentials("gooduser", "goodpass");

        btlso::Endpoint destination;
        args.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &args);

        btlso::Endpoint           proxy;  // address of just-started proxy srv
        btls5::NetworkDescription proxies;

        args.d_label = "unresponsive-0.0";
        args.d_mode = btls5::TestServerArgs::e_IGNORE;
        btls5::TestServer proxy00(&proxy, &args);
        proxies.addProxy(0, proxy, credentials);

        args.d_label = "proxy-0.1";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxy01(&proxy, &args);
        proxies.addProxy(0, proxy, credentials);

        args.d_label = "bad-credentials-1.0";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_expectedCredentials.set("baduser", "badpass");
        btls5::TestServer proxy10(&proxy, &args);
        proxies.addProxy(1, proxy, credentials);

        args.d_label = "proxy-1.1";
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_expectedCredentials.set("gooduser", "goodpass");
        btls5::TestServer proxy11(&proxy, &args);
        proxies.addProxy(1, proxy, credentials);


        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        btls5::NetworkConnector::ConnectionStatus status;

        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                 bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4, &queue);
        {
            if (veryVerbose) { P_(destination) P(proxies) }

            const bsls::TimeInterval T1(0.5), T2(5.0);

            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_SUCCESS == status);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TIMEOUT TEST
        //
        // Concerns:
        //: 1 Proper timeout processing when a proxy is unresponsive.
        //
        // Plan:
        //: 1 Connect using an unresponsive proxy and verify error code:
        //:   1 When the proxy timeout is smaller than total timeout.
        //:   2 When the proxy timeout is greater than total timeout.
        //
        // Testing:
        //   CONCERN: Timeout when the proxy doesn't respond.
        //   CONCERN: Total timeout before proxy timeout.
        //   CONCERN: Failover to second proxy after the first proxy timeout.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TIMEOUT TEST" << endl
                          << "============" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);

        btlso::Endpoint       proxy;
        btls5::TestServerArgs proxyArgs;
        proxyArgs.d_verbosity = verbosity;
        proxyArgs.d_label = "unresponsive proxy";
        proxyArgs.d_mode = btls5::TestServerArgs::e_IGNORE;
        btls5::TestServer unresponsiveServer(&proxy, &proxyArgs);

        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        btls5::NetworkConnector::ConnectionStatus status;

        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                 bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4, &queue);
        {
            if (verbose) cout << "\nSingle Proxy Timeout" << endl;

            const bsls::TimeInterval T1(2), T2(5);

            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_ERROR == status);
        }
/***
        {
            if (verbose) cout << "\nTotal Attempt Timeout" << endl;

            const bsls::TimeInterval T1(2), T2(5);

            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_TIMEOUT == status);
        }
        {
            if (verbose) cout << "\nFailover after proxy timeout" << endl;

            // add another, good proxy server to the network description

            const bsls::TimeInterval T1(2), T2(5);

            proxyArgs.d_label = "good proxy";
            proxyArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
            btls5::TestServer goodServer(&proxy, &proxyArgs);
            proxies.addProxy(0, proxy);

            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  T1, // proxy
                                                                  T2, // total
                                                                  destination);
            connector.startConnectionAttempt(attempt);
            queue.popFront(&status);
            LOOP_ASSERT(status, btls5::NetworkConnector::e_SUCCESS == status);
        }
***/
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SOCKS5 SOCKET IMPORT INTO SESSION POOL
        //
        // Concerns:
        //: 1 A socket connection established by 'btls5::NetworkConnector' can
        //:   be imported into, and managed by btlmt::SessionPool
        //: 2 Only the specified allocator is used.
        //
        // Plan:
        //: 1 Create a test destination server
        //: 2 Create a test SOCKS5 proxy.
        //: 3 Construct a test allocator to be used by the object under test.
        //: 4 Establish a connection to the destination server via the proxy.
        //: 5 Import the connection into a SessionPool.
        //: 6 Verify that the default allocator has not been used.
        //
        // Testing:
        //   btls5::NetworkConnector(net, fact, eventMgr, *a = 0);
        //   ~btls5::NetworkConnector();
        //   makeConnectionAttemptHandle(cB, proxyTO, totalTO, server);
        //   startConnectionAttempt(ConnectionAttemptHandle& attempt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SOCKS5 SOCKET IMPORT INTO SESSION POOL" << endl
                          << "======================================" << endl;

        btls5::TestServerArgs args;
        args.d_verbosity = verbosity;

        btlso::Endpoint destination;
        args.d_mode = btls5::TestServerArgs::e_IGNORE;
        args.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &args);
        if (verbose) {
            cout << "destination server started on " << destination << endl;
        }

        args.d_expectedDestination = destination;
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        args.d_label = "proxy";
        btlso::Endpoint proxy;
        btls5::TestServer proxyServer(&proxy, &args);
        if (verbose) {
            cout << "proxy started on " << proxy << endl;
        }
        btls5::NetworkDescription socks5Servers;
        socks5Servers.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> socketFactory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        btlmt::ChannelPoolConfiguration config;
        using namespace bdlf::PlaceHolders;
        btlmt::SessionPool sessionPool(config, &poolStateCb);
        ASSERT(0 == sessionPool.start());
        if (verbose) {
            cout << "starting btlmt::SessionPool" << endl;
        }

        if (verbose) {
            cout << "attempting to connect to " << destination
                 << " via " << proxy
                 << endl;
        }
        const bsls::TimeInterval proxyTimeout(10.0);
        bslmt::Mutex     stateLock;
        bslmt::Condition stateChanged;
        volatile int     state = 0; // value > 0 indicates success, < 0 error
        bslmt::LockGuard<bslmt::Mutex> lock(&stateLock);

        btls5::NetworkConnector::ConnectionStateCallback cb =
                                           bdlf::BindUtil::bind(socks5Cb,
                                                                _1, _2, _3, _4,
                                                                &sessionPool,
                                                                &stateLock,
                                                                &stateChanged,
                                                                &state);

        btls5::NetworkConnector connector(socks5Servers,
                                          &socketFactory,
                                          &eventManager);

        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                    connector.makeConnectionAttemptHandle(cb,
                                                          proxyTimeout,
                                                          bsls::TimeInterval(),
                                                          destination);
        connector.startConnectionAttempt(attempt);
        while (!state) {
            stateChanged.wait(&stateLock);
        }
        LOOP_ASSERT(state, state > 0);

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
        //: 1 Create a test server instance 'destination'.
        //: 2 Create a test server 'proxy'.
        //: 3 Connect to 'destination' through 'proxy' using SOCKS5.
        //: 4 Verify that the callback is invoked with successful result.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // install a 'TestAllocator' as defalt to check for memory leaks

        bslma::TestAllocator ta("test1", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&ta);

        btlso::Endpoint       destination;
        btls5::TestServerArgs destinationArgs;
        destinationArgs.d_verbosity = verbosity;
        destinationArgs.d_label = "destination";
        btls5::TestServer destinationServer(&destination, &destinationArgs);
        if (verbose) {
            cout << "destination server started on " << destination << endl;
        }

        btlso::Endpoint       proxy;
        btls5::TestServerArgs proxyArgs;
        proxyArgs.d_verbosity = verbosity;
        proxyArgs.d_label = "proxy";
        proxyArgs.d_mode = btls5::TestServerArgs::e_CONNECT;
        btls5::TestServer proxyServer(&proxy, &proxyArgs);
        if (verbose) {
            cout << "proxy server started on " << proxy << endl;
        }

        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        btls5::NetworkConnector  connector(proxies, &factory, &eventManager);
        const bsls::TimeInterval proxyTimeout(2);
        const bsls::TimeInterval totalTimeout(10);
        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionAttemptHandle attempt =
            connector.makeConnectionAttemptHandle(
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                              _1, _2, _3, _4,
                                                              &queue),
                                          proxyTimeout,
                                          totalTimeout,
                                          destination);
        connector.startConnectionAttempt(attempt);

        // wait for connection result and check for success

        btls5::NetworkConnector::ConnectionStatus status;
        queue.popFront(&status);
        ASSERT(btls5::NetworkConnector::e_SUCCESS == status);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // OFF-THE-SHELF SOCKS5 SERVER
        //  Test 'btls5::NetworkConnector' using off-the-shelf servers.  Since
        //  we cannot rely on these servers being up and reachable, this is a
        //  manually-executed test.
        //
        // Concerns:
        //: 1 The component can connect through off-the-shelf SOCKS5 proxies.
        //
        // Plan:
        //: 1 Using a table of destinations, connect a SOCSK5 server.
        //
        // Testing:
        //   CONCERN: Negotiate using an off-the-shelf SOCKS5 proxy.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OFF-THE-SHELF" << endl
                          << "=============" << endl;

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();
        const bsls::TimeInterval proxyTimeout(2);
        const bsls::TimeInterval totalTimeout(3);
        const btlso::Endpoint    destination("api1.bloomberg.net", 8394);

        bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus> queue(1);
        using namespace bdlf::PlaceHolders;
        btls5::NetworkConnector::ConnectionStateCallback cb =
                 bdlf::BindUtil::bind(breathingTestCb, _1, _2, _3, _4, &queue);

        const btlso::Endpoint     proxy("10.16.21.6", 1080);
        btls5::NetworkDescription proxies;
        proxies.addProxy(0, proxy);
        btls5::NetworkConnector connector(proxies, &factory, &eventManager);

        static const struct {
            int         d_line;     // source line number
            const char *d_hostname;
            int         d_port;
            bool        d_success;  // will connection succeed?
        } DATA[] = {
            //LINE HOSTNAME              PORT  SUCCESS
            //---- --------------------  ----  -------
            { L_,  "208.134.161.62",     8194, true },
            { L_,  "api1.bloomberg.net", 8194, true },
            { L_,  "api1.bloomberg.net", 8394, false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int i = 0; i < NUM_DATA; i++) {
            const int LINE = DATA[i].d_line;
            const btlso::Endpoint DESTINATION(DATA[i].d_hostname,
                                              DATA[i].d_port);
            const bool SUCCESS = DATA[i].d_success;

            if (veryVerbose) { T_ P_(LINE) P_(proxy) P(DESTINATION) }

            btls5::NetworkConnector::ConnectionAttemptHandle attempt =
                            connector.makeConnectionAttemptHandle(cb,
                                                                  proxyTimeout,
                                                                  totalTimeout,
                                                                  DESTINATION);
            connector.startConnectionAttempt(attempt);

            // wait for connection result and check for success

            btls5::NetworkConnector::ConnectionStatus status;
            queue.popFront(&status);
            LOOP5_ASSERT(LINE, proxy, DESTINATION, status, SUCCESS,
                    SUCCESS == (status == btls5::NetworkConnector::e_SUCCESS));
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // OVERLAPPED CONNECTIONS ON ONE CONNECTOR
        //  Test concurrency through overlapping connection attempts.
        //
        // Concerns:
        //: 1 Two connections can be established concurrently, using the same
        //:   connector.
        //
        // Plan:
        //: 1 Using a table-driven method:
        //:   1 Create a connector with a two-level network with given delays.
        //:   2 Start one connection attempt with this connector.
        //:   3 After a lag given in the table start the second attempt.
        //:   4 Check for successful connection from both attempts.
        //
        // Testing:
        //   CONCERN: Two overlapping connection attempts on one connector.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OVERLAPPED CONNECTIONS ON ONE CONNECTOR" << endl
                          << "=======================================" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        btls5::TestServerArgs args;  // reused for all servers
        args.d_verbosity = verbosity;
        args.d_mode = btls5::TestServerArgs::e_CONNECT;
        const btls5::Credentials credentials("gooduser", "goodpass");
        args.d_expectedCredentials = credentials;

        btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
        btlmt::TcpTimerEventManager eventManager;
        eventManager.enable();

        btlso::Endpoint destination1;
        args.d_label = "destination1";
        btls5::TestServer destinationServer1(&destination1, &args);

        btlso::Endpoint destination2;
        args.d_label = "destination2";
        btls5::TestServer destinationServer2(&destination2, &args);

        btlso::Endpoint proxy;  // address of just-started proxy server

        const bsls::TimeInterval proxyTimeout(2.0);
        const bsls::TimeInterval totalTimeout(5.0);

        static const struct {
            int    d_line;                // source line number
            double d_proxy1DelaySeconds;  // delay before every response
            double d_proxy2DelaySeconds;  // delay before every response
            double d_lagSeconds;          // delay before starting 2nd attempt
        } DATA[] = {
        //LINE DELAY1  DELAY2    LAG
        //--   ------  ------  -----
        { L_,   0.000,  0.000, 0.000 },  // baseline: no delay

        { L_,   0.000,  0.000, 0.000 },  // simultaneous start with delays
        { L_,   0.010,  0.000, 0.000 },
        { L_,   0.010,  0.010, 0.000 },
        { L_,   0.040,  0.010, 0.000 },
        { L_,   0.040,  0.040, 0.000 },
        { L_,   0.010,  0.040, 0.000 },
        { L_,   0.000,  0.040, 0.000 },

        { L_,   0.000,  0.000, 0.010 },  // lag 10 mS start with delays
        { L_,   0.010,  0.000, 0.010 },
        { L_,   0.010,  0.010, 0.010 },
        { L_,   0.040,  0.010, 0.010 },
        { L_,   0.040,  0.040, 0.010 },
        { L_,   0.010,  0.040, 0.010 },
        { L_,   0.000,  0.040, 0.010 },

        { L_,   0.000,  0.000, 0.040 },  // lag 40 mS start with delays
        { L_,   0.010,  0.000, 0.040 },
        { L_,   0.010,  0.010, 0.040 },
        { L_,   0.040,  0.010, 0.040 },
        { L_,   0.040,  0.040, 0.040 },
        { L_,   0.010,  0.040, 0.040 },
        { L_,   0.000,  0.040, 0.040 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int t1 = 0; t1 < NUM_DATA; t1++) {
            const int                LINE  (DATA[t1].d_line);
            const bsls::TimeInterval DELAY1(DATA[t1].d_proxy1DelaySeconds);
            const bsls::TimeInterval DELAY2(DATA[t1].d_proxy2DelaySeconds);
            const bsls::TimeInterval LAG   (DATA[t1].d_lagSeconds);

            if (veryVeryVerbose) { T_ P_(LINE) P_(DELAY1) P_(DELAY2) P(LAG) }

            using namespace bdlf::PlaceHolders;

            // construct a connector with a 2-level proxy network

            btls5::NetworkDescription proxies;

            args.d_delay = DELAY1;
            args.d_label = "proxy1";
            btls5::TestServer proxy1(&proxy, &args);
            proxies.addProxy(0, proxy, credentials);

            args.d_delay = DELAY2;
            args.d_label = "proxy2";
            btls5::TestServer proxy2(&proxy, &args);
            proxies.addProxy(1, proxy, credentials);

            btls5::NetworkConnector connector(proxies,
                                              &factory,
                                              &eventManager);

            // create a queue to convey status and start the 1st connection

            bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus>
                                                                     queue1(1);
            btls5::NetworkConnector::ConnectionStatus status1;
            btls5::NetworkConnector::ConnectionStateCallback cb1 =
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                               _1, _2, _3, _4,
                                                               &queue1);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt1 =
                           connector.makeConnectionAttemptHandle(cb1,
                                                                 proxyTimeout,
                                                                 totalTimeout,
                                                                 destination1);
            connector.startConnectionAttempt(attempt1);

            // create a queue and, after a lag, start the 2nd connection

            bdlcc::FixedQueue<btls5::NetworkConnector::ConnectionStatus>
                                                                     queue2(1);
            btls5::NetworkConnector::ConnectionStatus status2;
            btls5::NetworkConnector::ConnectionStateCallback cb2 =
                                          bdlf::BindUtil::bind(breathingTestCb,
                                                               _1, _2, _3, _4,
                                                               &queue2);
            btls5::NetworkConnector::ConnectionAttemptHandle attempt2 =
                           connector.makeConnectionAttemptHandle(cb2,
                                                                 proxyTimeout,
                                                                 totalTimeout,
                                                                 destination2);
            bslmt::ThreadUtil::sleep(LAG);
            connector.startConnectionAttempt(attempt2);

            // wait for connection results and check for success

            queue1.popFront(&status1);
            LOOP2_ASSERT(LINE, status1,
                         btls5::NetworkConnector::e_SUCCESS == status1);
            queue2.popFront(&status2);
            LOOP2_ASSERT(LINE, status2,
                         btls5::NetworkConnector::e_SUCCESS == status2);
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
