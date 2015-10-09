// btlso_inetstreamsocket.t.cpp                                       -*-C++-*-

#include <btlso_inetstreamsocket.h>

#include <btlso_streamsocketfactory.h>

#include <btlso_socketimputil.h>
#include <btlso_ipv4address.h>
#include <btlso_platform.h>

#include <bslmt_threadutil.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_signal.h>     // for signal()
#endif

#include <bsl_c_stdio.h>

#include <bslma_testallocator.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//  The btlso::InetStreamSocket represents the lowest layer at which all socket
//  functionality is combined.  This means that in addition to testing that
//  functions forward onto the lower layer functions, the functionality
//  with regards to all of a sockets implementation must be tested.
//
//  Each socket implementation is different in its behavior including
//  their interpretation of the BSD sockets interface.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btlso::InetStreamSocket(handle, bslma::Allocator *allocator);
// [ 2] ~btlso::InetStreamSocket<ADDRESS>();
//
// MANIPULATORS
// [ 3] int accept(result);
// [ 3] int accept(socket, ADDRESS *peerAddress);
// [ 3] int bind(const ADDRESS& address);
// [ 3] int connect(const ADDRESS& address);
// [ 3] int listen(int backlog);
// [ 7] int read(char *buffer, int length);
// [ 7] int readv(const btls::Iovec *buffers, int numBuffers);
// [ 6] int write(const char *buffer, int length);
// [ 6] int writev(const btls::Iovec *buffers, int numBuffers);
// [ 6] int writev(const btls::Ovec *buffers, int numBuffers);
// [ 3] int setBlockingMode(mode);
// [ 3] int shutdown(streamOption);
// [ 9] int waitForConnect(const bsls::TimeInterval& timeout);
// [ 8] int waitForAccept(const bsls::TimeInterval& timeout);
// [ 5] int waitForIO(type,const bsls::TimeInterval& timeout);
// [11] int setLingerOption(options);
// [10] int setOption(int level, int option, int value);
//
// ACCESSORS
// [ 3] int blockingMode(result) const;
// [ 3] int localAddress(ADDRESS *result) const;
// [ 3] int peerAddress(ADDRESS *result) const;
// [ 2] btlso::SocketHandle::Handle handle() const;
// [10] int connectionStatus() const;
// [11] int lingerOption(btlso::SocketOptUtil::LingerData *result) const;
// [10] int socketOption(int *result, int level, int option) const;
//-----------------------------------------------------------------------------
// [ 4] createConnectedStreamSockets()
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                  << J << "\t" << #K << ": " << K << "\n"; \
                  aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_() cout << '\t' << flush            // Print tab without '\n'
#define L_ __LINE__                           // Current line number

int verbose;
int veryVerbose;
int veryVeryVerbose;

#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
    typedef int ADDRLEN_T;
#else
    typedef socklen_t ADDRLEN_T;
#endif

//=============================================================================
//                            TEST FUNCTIONS
//=============================================================================

const int MIN_SOCKET_HANDLE = 0;
const int MAX_SOCKET_HANDLE = 20;      // The maximum number for the handle
                                       // expected within this test context.
const int MAX_SOCKETS_USED = 10;       // The maximum number of sockets opened
                                       // within this test driver.

// This test function counts the number of sockets currently open within
// a range of socket handles.  It assumes that the handle assigned to new
// sockets use the current lowest unassigned handle.  Note that the namespace
// for socket handles normally includes open files and pipes.  For example
// on UNIX handles 0, 1 and 2 are normally open for stdin, stdout and stderr.
// Windows does not conform to this behavior.

int countSockets(int base, int max)
    // return the number of open sockets in the socket handle range starting
    // with the specified 'base' up the to specified 'max'.  If the platform
    // does not allow the number of sockets to be counted return -1.
{
#if defined(BTLSO_PLATFORM_WIN_SOCKETS) || defined(BSLS_PLATFORM_OS_CYGWIN)

    // Cygwin does not work around the Windows limitation.  See the source
    // OR http://sources.redhat.com/ml/cygwin/2000-12/msg00422.html.

    return -1;
#else
    int socketNumber;
    int socketCount = 0;
    char address[20];
    ADDRLEN_T len = sizeof(address);

    for (socketNumber = base; socketNumber <= max; ++socketNumber) {
         int ret = ::getsockname((btlso::SocketHandle::Handle) socketNumber,
                               (sockaddr *) &address, &len);

         if (ret >= 0) ++socketCount;
    }

    return socketCount;
#endif
}

int verifyHandle(btlso::SocketHandle::Handle handle)
    // Return 1 if the specified handle corresponds to an open socket.
{
    char address[sizeof(sockaddr_in)];
    ADDRLEN_T len = sizeof(address);

    int ret = ::getsockname((btlso::SocketHandle::Handle) handle,
                            (sockaddr *) &address[0], &len);

#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
    if (ret != 0 && WSAGetLastError() == WSAEINVAL) ret = 0;
#endif
#if defined(BSLS_PLATFORM_OS_CYGWIN)
    if (ret != 0 && errno == EINVAL) ret = 0;
#endif

    return ret == 0 ? 1 : 0;
}

int createConnectedStreamSockets(
    btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
    btlso::StreamSocket<btlso::IPv4Address> **streamSocketA,
    btlso::StreamSocket<btlso::IPv4Address> **streamSocketB)
    // On success return 1, 0 otherwise.
{
    *streamSocketA = 0;
    *streamSocketB = 0;
    btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
      factory->allocate();

    if (serverSocket == 0) return 0;                                  // RETURN

    // Bind to localhost with anon port number

    btlso::IPv4Address localLowPort;
    localLowPort.setIpAddress("127.0.0.1");
    localLowPort.setPortNumber(0);

    // bind to a anon port
    int resp = serverSocket->bind(localLowPort);
    ASSERT(resp == 0);

    btlso::IPv4Address serverAddress;
    resp = serverSocket->localAddress(&serverAddress);
    ASSERT(resp == 0);

    resp = serverSocket->listen(1);
    ASSERT(resp == 0);

    // Create the client socket.
    *streamSocketA = factory->allocate();

    if (*streamSocketA) {
        resp = (*streamSocketA)->connect(serverAddress);
        ASSERT(resp == 0);

        if (resp == 0) {
            // Accept the connection
            btlso::IPv4Address clientAddress;
            resp = serverSocket->accept(streamSocketB, &clientAddress);
            ASSERT(resp == 0);
        }
    }

    if (resp != 0 && *streamSocketA != 0) {
        factory->deallocate(*streamSocketA);
        *streamSocketA = 0;
        *streamSocketB = 0;
    }

    factory->deallocate(serverSocket);

    return (*streamSocketA) != 0;
}

#ifdef BSLS_PLATFORM_OS_UNIX
volatile sig_atomic_t globalAlarmCount = 0;

extern "C" void sigalarm(int)
{
   ++globalAlarmCount;
   ::ualarm(10, 0);
}
#endif

                       // =============================
                       // class TestStreamSocketFactory
                       // =============================

template <class ADDRESS>
class TestStreamSocketFactory : public btlso::StreamSocketFactory<ADDRESS>
{
    // CLASS DATA
    bslma::Allocator *d_allocator_p;       // held, not owned

  public:
    // CREATORS
    TestStreamSocketFactory(bslma::Allocator *basicAllocator = 0);

    virtual ~TestStreamSocketFactory();

    // MANIPULATORS
    virtual btlso::StreamSocket<ADDRESS> *allocate();

    virtual btlso::StreamSocket<ADDRESS> *allocate(
                                           btlso::SocketHandle::Handle handle);

    virtual void deallocate(btlso::StreamSocket<ADDRESS> *socket);

    virtual void deallocate(btlso::StreamSocket<ADDRESS> *socket,
                            bool                          closeFlag);
};

// CREATORS
template <class ADDRESS>
TestStreamSocketFactory<ADDRESS>::TestStreamSocketFactory(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class ADDRESS>
TestStreamSocketFactory<ADDRESS>::~TestStreamSocketFactory()
{
}

// MANIPULATORS
template <class ADDRESS>
btlso::StreamSocket<ADDRESS> *TestStreamSocketFactory<ADDRESS>::allocate()
{
    btlso::SocketHandle::Handle newSocketHandle;

    int ret = btlso::SocketImpUtil::open<ADDRESS>(
                                        &newSocketHandle,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);

    if (ret < 0) {
        return 0;                                                     // RETURN
    }

    btlso::StreamSocket<ADDRESS> *newStreamSocket =
         new (*d_allocator_p) btlso::InetStreamSocket<ADDRESS>(newSocketHandle,
                                                               d_allocator_p);

    return newStreamSocket;
}

template <class ADDRESS>
btlso::StreamSocket<ADDRESS> *
TestStreamSocketFactory<ADDRESS>::allocate(btlso::SocketHandle::Handle handle)
{
    // Create a new 'InetStreamSocket' for this socket.

    return new (*d_allocator_p) btlso::InetStreamSocket<ADDRESS>(
                                                                handle,
                                                                d_allocator_p);
}

template <class ADDRESS>
void TestStreamSocketFactory<ADDRESS>::deallocate(
                                          btlso::StreamSocket<ADDRESS> *socket)
{
    btlso::SocketImpUtil::close(socket->handle());

    d_allocator_p->deleteObject(socket);
}

template <class ADDRESS>
void TestStreamSocketFactory<ADDRESS>::deallocate(
                                       btlso::StreamSocket<ADDRESS> *socket,
                                       bool                          closeFlag)
{
    if (closeFlag) {
        btlso::SocketImpUtil::close(socket->handle());
    }

    d_allocator_p->deleteObject(socket);
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    ASSERT(btlso::SocketImpUtil::startup() == 0);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // ----------------------------------------------------------------
        // USAGE EXAMPLE
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
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Exchanging data using a stream socket
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will show how to create two stream sockets, establish a
// connection, and then exchange data between them.
//
// First, we will open a pair of socket handles.
//..
    btlso::SocketHandle::Handle clientHandle, serverHandle;

    int rc = btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                        &clientHandle,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);
    ASSERT(0 == rc);

    rc = btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                        &serverHandle,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);
    ASSERT(0 == rc);
//..
// Then, we will construct the server and client stream socket objects,
// 'serverSocket' and 'clientSocket' respectively.
//..
    btlso::InetStreamSocket<btlso::IPv4Address> serverSocket(serverHandle);
    btlso::InetStreamSocket<btlso::IPv4Address> clientSocket(clientHandle);
//..
// Next, we will bind 'serverSocket' to a local address and start listening on
// it.
//..
    btlso::IPv4Address localAddress;
    localAddress.setIpAddress("127.0.0.1");
    localAddress.setPortNumber(0);

    rc = serverSocket.bind(localAddress);
    ASSERT(0 == rc);

    btlso::IPv4Address serverAddress;
    rc = serverSocket.localAddress(&serverAddress);
    ASSERT(0 == rc);

    rc = serverSocket.listen(10);
    ASSERT(0 == rc);
//..
// Then, we will connect the 'clientSocket' to 'localAddress'.
//..
    rc = clientSocket.connect(serverAddress);
    ASSERT(0 == rc);
//..
// Next, we will create a new stream socket, 'acceptSocket', after accepting
// the new connection.
//..

    btlso::StreamSocket<btlso::IPv4Address> *acceptSocket;
    btlso::IPv4Address clientAddress;

    rc = serverSocket.accept(&acceptSocket);
    ASSERT(0 == rc);
//..
// Then, we will confirm that both sockets are connected.
//..
    rc = clientSocket.connectionStatus();
    ASSERT(0 == rc);

    rc = acceptSocket->connectionStatus();
    ASSERT(0 == rc);
//..
// Next, we will write a message through 'clientSocket'.
//..
    char       writeBuffer[]  = "Hello World!";
    const int  writeBufferLen = bsl::strlen(writeBuffer);

    rc = clientSocket.write(writeBuffer, writeBufferLen);
    ASSERT(rc == writeBufferLen);
//..
// Now, we will read the written message through 'acceptSocket'.
//..
    const int  BUFSIZE = 64;
    char       readBuffer[BUFSIZE];

    rc = acceptSocket->read(readBuffer, BUFSIZE);
    ASSERT(rc == writeBufferLen);

//..
// Finally, we will read the written message through 'acceptSocket'.
//..
    btlso::SocketImpUtil::close(clientHandle);
    btlso::SocketImpUtil::close(acceptSocket->handle());
    btlso::SocketImpUtil::close(serverHandle);

      }break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'setLingerOption' and 'lingerOption'
        //
        // Plan:
        //
        // Testing:
        //   int setLingerOption(options);
        //   int lingerOption(btlso::SocketOptUtil::LingerData *result) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'setLingerOption' and 'lingerOption'"
                          << endl
                          << "============================================"
                          << endl;

        TestStreamSocketFactory<btlso::IPv4Address> testFactory;

        btlso::StreamSocket<btlso::IPv4Address> *testSocket =
                                                        testFactory.allocate();

        btlso::SocketOptUtil::LingerData lingerResult;
        lingerResult.l_onoff = 2;
        lingerResult.l_linger = 99;
        int resp = testSocket->lingerOption(&lingerResult);

        if (veryVerbose) {
            P_(lingerResult.l_onoff);
            P(lingerResult.l_linger);
        }

        ASSERT(resp == 0);
        ASSERT(lingerResult.l_onoff == 0);

        btlso::SocketOptUtil::LingerData ling;
        ling.l_onoff = 1;
        ling.l_linger = 0;
        testSocket->setLingerOption(ling);

        ASSERT(resp == 0);

        lingerResult.l_onoff = 2;
        lingerResult.l_linger = 99;
        resp = testSocket->lingerOption(&lingerResult);
        if (veryVerbose) {
            P_(lingerResult.l_onoff);
            P(lingerResult.l_linger);
        }

        ASSERT(resp == 0);
        ASSERT(lingerResult.l_onoff != 0);
        ASSERT(lingerResult.l_linger == 0);

        ling.l_onoff = 1;
        ling.l_linger = 10;
        testSocket->setLingerOption(ling);

        ASSERT(resp == 0);

        lingerResult.l_onoff = 2;
        lingerResult.l_linger = 99;
        resp = testSocket->lingerOption(&lingerResult);
        if (veryVerbose) {
            P_(lingerResult.l_onoff);
            P(lingerResult.l_linger);
        }

        ASSERT(resp == 0);
        ASSERT(lingerResult.l_onoff != 0);
        ASSERT(lingerResult.l_linger > 0);

        ling.l_onoff = 0;
        ling.l_linger = 10;
        testSocket->setLingerOption(ling);

        ASSERT(resp == 0);

        lingerResult.l_onoff = 2;
        lingerResult.l_linger = 99;
        resp = testSocket->lingerOption(&lingerResult);
        if (veryVerbose) {
            P_(lingerResult.l_onoff);
            P(lingerResult.l_linger);
        }

        ASSERT(resp == 0);
        ASSERT(lingerResult.l_onoff == 0);

        testFactory.deallocate(testSocket);
      } break;
      case 10 : {
        // --------------------------------------------------------------------
        // TESTING 'setOption' & 'socketOption'
        //
        // Plan:
        //
        // Testing:
        //   int setOption(int level, int option, int value);
        //   int socketOption(int *result, int level, int option) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'setOption' & 'socketOption'" << endl
                          << "====================================" << endl;

        TestStreamSocketFactory<btlso::IPv4Address> testFactory;

        btlso::StreamSocket<btlso::IPv4Address> *testSocket =
            testFactory.allocate();

        int result = -1;

        int resp = testSocket->socketOption(
            &result,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER);

        ASSERT(resp == 0);
        ASSERT(result > 0);
        if (veryVerbose) cout << "SENDBUFFER " << result << endl;

        resp = testSocket->socketOption(
            &result,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_RECEIVEBUFFER);

        ASSERT(resp == 0);
        ASSERT(result > 0);
        if (veryVerbose) cout << "RECEIVEBUFFER " << result << endl;

        resp = testSocket->setOption(
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER,
            32768);

        ASSERT(resp == 0);

        result = -1;
        resp = testSocket->socketOption(
            &result,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER);

        ASSERT(resp == 0);
        ASSERT(result > 0);
        if (veryVerbose) P(result);

        testFactory.deallocate(testSocket);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'waitForConnect'
        //
        // Plan:
        //   Connect non-blocking to an address known to be invalid.
        //   Connect non-blocking to a local valid port.
        //
        // Testing:
        //   int waitForConnect(const bsls::TimeInterval& timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'waitForConnect'" << endl
                          << "========================" << endl;

        // Test bind() with "bad" external address.  Non Blocking client
        // This address is a non-routable address and the connection cannot
        // succeed.

        {
            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::IPv4Address serverAddress;
            serverAddress.setIpAddress("169.254.1.1");
            serverAddress.setPortNumber(1);

            // Create the client socket.

            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            clientSocket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);

            int resp = clientSocket->connect(serverAddress);
            ASSERT(resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);
            int status = 0;

            // Poll status
            bsls::TimeInterval negtwoseconds = bdlt::CurrentTime::now() - 2;
            resp = clientSocket->waitForConnect(negtwoseconds);
            ASSERT(resp == 0
                || resp == btlso::SocketHandle::e_ERROR_TIMEDOUT);

            enum { WAITS = 10 };

            for (int x = 0; x < WAITS; ++x) {
                bsls::TimeInterval timeout = bdlt::CurrentTime::now() + 0.1;

                if (veryVerbose) { cout << "waitForConnect "; P(resp); }
                resp = clientSocket->waitForConnect(timeout);

                if (resp == 0) {
                    status = clientSocket->connectionStatus();
                    if (veryVerbose) {
                        cout << "connectionStatus ";P(status);
                    }
                    break;
                }
                else {
                    ASSERT(resp ==
                           btlso::SocketHandle::e_ERROR_TIMEDOUT);
                }
            }

            testFactory.deallocate(clientSocket);
        }
        {
            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                testFactory.allocate();
            ASSERT(serverSocket);

            // Bind to localhost with anon port number
            btlso::IPv4Address localAnonPort;
            localAnonPort.setIpAddress("127.0.0.1");
            localAnonPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localAnonPort);
            ASSERT(resp == 0);

            btlso::IPv4Address serverAddress;
            resp = serverSocket->localAddress(&serverAddress);
            ASSERT(resp == 0);

            // The address must be the same.
            ASSERT(localAnonPort.ipAddress() == serverAddress.ipAddress());
            // The port number may not be zero.
            ASSERT(serverAddress.portNumber() > 0);

            resp = serverSocket->listen(10);
            ASSERT(resp == 0);

            // Create the client socket.

            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            clientSocket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);

            resp = clientSocket->connect(serverAddress);
            ASSERT(resp == 0 ||
                   resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

            int status = -1;

            for (int x = 0; x < 10; ++x) {

                bsls::TimeInterval timeout = bdlt::CurrentTime::now() + 0.1;

                resp = clientSocket->waitForConnect(timeout);
                if (veryVerbose) { cout << "waitForConnect "; P(resp); }

                if (resp == 0) {
                    status = clientSocket->connectionStatus();
                    if (veryVerbose) {
                        cout << "connectionStatus ";P(status);
                    }
                    break;
                }
                else {
                    ASSERT(resp ==
                           btlso::SocketHandle::e_ERROR_TIMEDOUT);
                }
            }

            ASSERT(status == 0);

            // Accept the connection
            btlso::StreamSocket<btlso::IPv4Address> *serviceSocket;
            btlso::IPv4Address clientAddress;

            resp = serverSocket->accept(&serviceSocket, &clientAddress);

            // shutdown service socket
            resp = serviceSocket->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);

            status = clientSocket->connectionStatus();
            ASSERT(status == 0);
            if (veryVerbose) P(status);
            testFactory.deallocate(serviceSocket);

            testFactory.deallocate(serverSocket);
            testFactory.deallocate(clientSocket);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'waitForAccept'
        //
        // Plan:
        //   For blocking listening socket:
        //      no pending incoming connection
        //      pending incoming connection
        //      pending incoming connection closed before wait
        //      pending incoming connection reset before wait (not impl***
        //
        // Testing:
        //   int waitForAccept(const bsls::TimeInterval& timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing waitForAccept" << endl
                          << "=====================" << endl;

        TestStreamSocketFactory<btlso::IPv4Address> testFactory;
        btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
            testFactory.allocate();
        ASSERT(serverSocket);

        // Bind to localhost with anon port number
        btlso::IPv4Address localAnonPort;
        localAnonPort.setIpAddress("127.0.0.1");
        localAnonPort.setPortNumber(0);

        // bind to a anon port
        int resp = serverSocket->bind(localAnonPort);
        ASSERT(resp == 0);

        btlso::IPv4Address serverAddress;
        resp = serverSocket->localAddress(&serverAddress);
        ASSERT(resp == 0);

        // The address must be the same.  The port number may not
        // be zero.
        ASSERT(localAnonPort.ipAddress() == serverAddress.ipAddress());
        ASSERT(serverAddress.portNumber() > 0);

        resp = serverSocket->listen(10);
        ASSERT(resp == 0);

        // Create the client socket.

        btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
            testFactory.allocate();
        ASSERT(clientSocket);

        bsls::TimeInterval timeBefore = bdlt::CurrentTime::now();
        bsls::TimeInterval timeout = timeBefore + 2;

        // There is no pending incoming connection.  Expect timeout.
        resp = serverSocket->waitForAccept(timeout);
        ASSERT(resp == btlso::SocketHandle::e_ERROR_TIMEDOUT);
        if (veryVerbose) { P(resp); };

        resp = clientSocket->connect(serverAddress);
        ASSERT(resp == 0);
        if (veryVerbose) { P(resp); };

        bsls::TimeInterval timeAfter;
        timeBefore = bdlt::CurrentTime::now();
        timeout = timeBefore + 2;

        // There is a pending connection so expect success.
        resp = serverSocket->waitForAccept(timeout);
        ASSERT(resp == 0);
        if (veryVerbose) { P(resp); };

        timeBefore = bdlt::CurrentTime::now();
        timeout = timeBefore + 2;

        // There is a pending connection so expect success.
        resp = serverSocket->waitForAccept(timeout);
        ASSERT(resp == 0);
        if (veryVerbose) { P(resp); };

        // Accept the connection
        btlso::StreamSocket<btlso::IPv4Address> *serviceSocket;
        btlso::IPv4Address clientAddress;

        resp = serverSocket->accept(&serviceSocket, &clientAddress);
        ASSERT(resp == 0);
        if (veryVerbose) { P(resp); };

        timeBefore = bdlt::CurrentTime::now();
        timeout = timeBefore + 2;

        // There is no pending incoming connection.  Expect timeout.
        resp = serverSocket->waitForAccept(timeout);
        ASSERT(resp == btlso::SocketHandle::e_ERROR_TIMEDOUT);

        // If a timeout occurred, time must be > (timeout - 10ms)
        // and within 5 seconds
        timeAfter = bdlt::CurrentTime::now();
        ASSERT((timeAfter + 0.01) > timeout);
        ASSERT(timeAfter < (timeout + 5));

        timeBefore = bdlt::CurrentTime::now();
        timeout = timeBefore - 0.02;
        // Timeout is in the past (>= 20ms).  Don't wait just return
        // status.

        resp = serverSocket->waitForAccept(timeout);
        ASSERT(resp == btlso::SocketHandle::e_ERROR_TIMEDOUT);

        // If a timeout occurred, time must be > (timeout - 10ms)
        // and within 5 seconds
        timeAfter = bdlt::CurrentTime::now();
        ASSERT((timeAfter + 0.01) > timeout);
        ASSERT((timeAfter + 0.01) > timeBefore);
        ASSERT(timeAfter < (timeout + (5 + 0.02)));

        testFactory.deallocate(serviceSocket);

        testFactory.deallocate(serverSocket);
        testFactory.deallocate(clientSocket);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'read' and 'readv'
        //
        // Plan:
        //    test read  normal
        //    test readv normal
        //
        // Testing:
        //   int read(char *buffer, int length);
        //   int readv(const btls::Iovec *buffers, int numBuffers);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'read' and 'readv'" << endl
                          << "==========================" << endl;

        {
            if (veryVerbose) cout << "\n   test read normal data"
                                  << endl;

            // Create a server socket

            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
            int resp = createConnectedStreamSockets(&testFactory,
                                                    &streamSocketA,
                                                    &streamSocketB);

            ASSERT(resp);

            // Fill the outgoing socket buffer for socket A
            char buf1[] = { 10, 9, 88, 6, 2, 10, 0, 51, 7, 99 };

            char rcvbuf1[100];
            char rcvbuf2[100];

            resp = streamSocketA->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

            // Latency
            bslmt::ThreadUtil::microSleep(500 * 1000);

            resp = streamSocketB->read(rcvbuf1, 1);
            ASSERT(resp == 1);

            resp = streamSocketB->read(rcvbuf2, 5);
            ASSERT(resp == 5);

            ASSERT(memcmp(buf1, rcvbuf1, 1) == 0);
            ASSERT(memcmp(buf1 + 1, rcvbuf2, 5) == 0);

            testFactory.deallocate(streamSocketA);
            testFactory.deallocate(streamSocketB);
        }
        {
            if (veryVerbose) cout << "\n   test readv normal data"
                                  << endl;

            // Create a server socket

            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
            int resp = createConnectedStreamSockets(&testFactory,
                                                    &streamSocketA,
                                                    &streamSocketB);

            ASSERT(resp);

            // Fill the outgoing socket buffer for socket A
            char buf1[] = { 10, 9, 88, 6, 2, 10, 0, 51, 7, 99 };

            char rcvbuf1[100];
            char rcvbuf2[100];
            char rcvbuf3[100];

            resp = streamSocketA->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

            int totalSent = resp;

            // Latency
            bslmt::ThreadUtil::microSleep(500 * 1000);

            btls::Iovec vec[2];
            vec[0].setBuffer(rcvbuf1, 1);
            vec[1].setBuffer(rcvbuf2, 5);

            resp = streamSocketB->readv(vec, 2);
            ASSERT(resp == 6);

            // Test for zero data to be read and no EOF

            vec[0].setBuffer(rcvbuf1, 0);
            vec[1].setBuffer(rcvbuf2, 0);

            resp = streamSocketB->readv(vec, 2);
            ASSERT(resp == 0);

            resp = streamSocketB->read(rcvbuf3, 2);
            ASSERT(resp == 2);

            ASSERT(memcmp(buf1, rcvbuf1, 1) == 0);
            ASSERT(memcmp(buf1 + 1, rcvbuf2, 5) == 0);
            ASSERT(memcmp(buf1 + 6, rcvbuf3, 2) == 0);

            // shutdown A
            resp = streamSocketA->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);

            // First receive what has been sent.
            vec[0].setBuffer(rcvbuf1, 10);
            vec[1].setBuffer(rcvbuf2, 10);

            resp = streamSocketB->readv(vec, 2);

            ASSERT(resp == (totalSent - 6 - 2));

            // EOF is expected next.
            vec[0].setBuffer(rcvbuf1, 1);
            vec[1].setBuffer(rcvbuf2, 5);

            resp = streamSocketB->readv(vec, 2);
            ASSERT(resp == btlso::SocketHandle::e_ERROR_EOF);

            testFactory.deallocate(streamSocketA);
            testFactory.deallocate(streamSocketB);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'write' AND 'writev'
        //
        // Plan:
        //    test write
        //    test writev
        //    test response btlso::SocketHandle::e_ERROR_INTERRUPTED (UNIX
        //                                                               only)
        //    test response btlso::SocketHandle::e_ERROR_CONNDEAD
        //    test response btlso::SocketHandle::e_ERROR_WOULDBLOCK
        //
        // Testing:
        //   int write(const char *buffer, int length);
        //   int writev(const btls::Iovec *buffers, int numBuffers);
        //   int writev(const btls::Ovec *buffers, int numBuffers);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'write' and 'writev'" << endl
                          << "============================" << endl;

#if 0
        {
             if (veryVerbose) cout << "\n   test write normal data"
                                   << endl;

             // Create a server socket

             TestStreamSocketFactory<btlso::IPv4Address> testFactory;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
             int resp = createConnectedStreamSockets(&testFactory,
                                                     &streamSocketA,
                                                     &streamSocketB);

             ASSERT(resp);

             // Fill the outgoing socket buffer for socket A
             char buf1[] = { 10, 9, 8, 6, 2, 10 };
             char buf2[] = { 100, 5, 15, 0, 1 };

             char rcvbuf[100];

             resp = streamSocketA->write(buf1, sizeof(buf1));
             ASSERT(resp == sizeof(buf1));

             resp = streamSocketA->write(buf2, sizeof(buf2));
             ASSERT(resp == sizeof(buf2));

             bslmt::ThreadUtil::microSleep(100);

             resp = streamSocketB->read(rcvbuf, sizeof(rcvbuf));
             ASSERT(resp == (sizeof(buf1) + sizeof(buf2)));

             ASSERT(memcmp(buf1, rcvbuf, sizeof(buf1)) == 0);
             ASSERT(memcmp(buf2, rcvbuf + sizeof(buf1), sizeof(buf2)) == 0);

             testFactory.deallocate(streamSocketA);
             testFactory.deallocate(streamSocketB);
         }
         {
             if (veryVerbose) cout << "\n   test writev normal data"
                                   << endl;

             // Create a server socket

             TestStreamSocketFactory<btlso::IPv4Address> testFactory;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
             int resp = createConnectedStreamSockets(&testFactory,
                                                     &streamSocketA,
                                                     &streamSocketB);

             ASSERT(resp);

             // Fill the outgoing socket buffer for socket A
             char buf1[] = { 10, 9, 8, 6, 2, 10 };
             char buf2[] = { 100, 5, 15, 0, 1 };
             char buf3[] = { 1 };

             char rcvbuf[100];

             btls::Ovec vec[2];
             vec[0].setBuffer(buf1, sizeof(buf1));
             vec[1].setBuffer(buf2, sizeof(buf2));
             resp = streamSocketA->writev(vec, 2);
             ASSERT(resp == sizeof(buf1) + sizeof(buf2));
             if (veryVerbose) P(resp);

             btls::Ovec vec2[1];
             vec2[0].setBuffer(buf3, sizeof(buf3));
             resp = streamSocketA->writev(vec2, 1);
             ASSERT(resp == sizeof(buf3));
             if (veryVerbose) P(resp);

             bslmt::ThreadUtil::microSleep(100);

             resp = streamSocketB->read(rcvbuf, sizeof(rcvbuf));
             ASSERT(resp == (sizeof(buf1) + sizeof(buf2) + sizeof(buf3)));
             if (veryVerbose) P(resp);

             ASSERT(memcmp(buf1, rcvbuf, sizeof(buf1)) == 0);
             ASSERT(memcmp(buf2, rcvbuf + sizeof(buf1), sizeof(buf2)) == 0);
             ASSERT(memcmp(buf3,
                           rcvbuf + sizeof(buf1) + sizeof(buf2),
                           sizeof(buf3)) == 0);

             testFactory.deallocate(streamSocketA);
             testFactory.deallocate(streamSocketB);
         }
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
         {
             if (veryVerbose) cout << "\n   test write response "
                                   << "ERROR_INTERRUPTED (UNIX only)"
                                   << endl;
             int packetSize;

             for (packetSize = 500; packetSize < 5505; packetSize += 2501) {
                 // Create a server socket
                 if (veryVerbose) P(packetSize);

                 TestStreamSocketFactory<btlso::IPv4Address> testFactory;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
                 int resp = createConnectedStreamSockets(&testFactory,
                                                         &streamSocketA,
                                                         &streamSocketB);

                 ASSERT(resp);

                 // Fill the outgoing socket buffer for socket A
                 char zerobuf[1024*64];
                 memset(zerobuf, 0, sizeof zerobuf);

                 globalAlarmCount = 0;

                 // Set a one second timeout.  A blocking system call such as a
                 // write will fail when this timeout occurs.  The signal
                 // handler will restart the timer.
                 struct sigaction oact, act;
                 act.sa_handler = sigalarm;
                 sigemptyset(&act.sa_mask);
                 act.sa_flags = 0;
                 ::sigaction(SIGALRM, &act, &oact);
                 ::ualarm(10, 0);

                 int totalSent = 0;

                 // Keep going until write fails for interrupted call.
                 int i = 0;
                 while (1) {
                     resp = streamSocketA->write(zerobuf, packetSize);
                     if (resp <= 0) break;
                     ASSERT(resp <= packetSize);

                     if ((veryVerbose && resp < packetSize)
                      || veryVeryVerbose) {
                         P_(i); P(resp);
                     }
                     totalSent += resp;
                     ++i;
                 }

                 // Cancel the alarm

                 ::sigaction(SIGALRM, &oact, &act);
                 ::alarm(0);

                 if (veryVerbose) {
                     cout << "        ";
                     P_(i); P_(globalAlarmCount); P(totalSent);
                 }

                 ASSERT(globalAlarmCount > 0);

                 ASSERT(resp == btlso::SocketHandle::e_ERROR_INTERRUPTED);

                 testFactory.deallocate(streamSocketA);
                 testFactory.deallocate(streamSocketB);
             }
         }

         {
             if (veryVerbose) cout << "\n   test writev response "
                                   << "ERROR_INTERRUPTED (UNIX only)"
                                   << endl;

             int packetSize;

             for (packetSize = 500; packetSize < 5505; packetSize += 2501) {
                 // Create a server socket
                 if (veryVerbose) P(packetSize);

                 TestStreamSocketFactory<btlso::IPv4Address> testFactory;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
                 int resp = createConnectedStreamSockets(&testFactory,
                                                         &streamSocketA,
                                                         &streamSocketB);

                 ASSERT(resp);

                 // Fill the outgoing socket buffer for socket A
                 char zerobuf[1024*64];
                 memset(zerobuf, 0, sizeof zerobuf);

                 globalAlarmCount = 0;

                 // Set a one second timeout.  A blocking system call such as a
                 // write will fail when this timeout occurs.  The signal
                 // handler will restart the timer.
                 struct sigaction oact, act;
                 act.sa_handler = sigalarm;
                 sigemptyset(&act.sa_mask);
                 act.sa_flags = 0;
                 ::sigaction(SIGALRM, &act, &oact);
                 ::ualarm(10, 0);

                 int totalSent = 0;

                 // Keep going until writev fails for interrupted call.
                 int i = 0;
                 while (1) {
                     btls::Ovec vec[2];
                     vec[0].setBuffer(zerobuf, packetSize);
                     vec[1].setBuffer(zerobuf, 2);
                     resp = streamSocketA->writev(vec, 2);
                     if (resp <= 0) break;
                     ASSERT(resp <= (packetSize + 2));

                     if ((veryVerbose && resp < packetSize)
                      || veryVeryVerbose) {
                         P_(i); P(resp);
                     }
                     totalSent += resp;
                     ++i;
                 }

                 // Cancel the alarm

                 ::sigaction(SIGALRM, &oact, &act);
                 ::alarm(0);

                 if (veryVerbose) {
                     cout << "        ";
                     P_(i); P_(globalAlarmCount); P(totalSent);
                 }

                 ASSERT(globalAlarmCount > 0);

                 ASSERT(resp == btlso::SocketHandle::e_ERROR_INTERRUPTED);

                 testFactory.deallocate(streamSocketA);
                 testFactory.deallocate(streamSocketB);
             }
         }
#endif

         {
#ifdef BSLS_PLATFORM_OS_UNIX
             // Under UNIX a write to a closed blocking socket causes the
             // signal SIGPIPE which by default will terminate the process.
             // Set the signal to be ignored.  This behavior does not
             // exist under Windows.
             ::signal(SIGPIPE, SIG_IGN );
#endif
         }

         if (veryVerbose) cout << "\n   test write response "
                               << "btlso::SocketHandle::e_ERROR_CONNDEAD"
                               << endl;

         // Verify error response when remote socket closed.
         // Try the write with different sizes because the behavior
         // of underlying sockets vary.  We don't try larger than
         // 32K per write because some platforms have a limit.
         for (int tolerance = 3, size = 1; size < 32; ++size) {
             TestStreamSocketFactory<btlso::IPv4Address> testFactory;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
             int resp = createConnectedStreamSockets(&testFactory,
                                                     &streamSocketA,
                                                     &streamSocketB);

             ASSERT(resp);

             char zerobuf[1024*32] = {0};

             testFactory.deallocate(streamSocketB);

             // The first write to the socket after the remote socket
             // has been closed may succeed.  Eventually the write will
             // fail (normally after the first or second write).

             int i;
             for (i = 0; i < 10; ++i) {
                 resp = streamSocketA->write(zerobuf, 1024 * size);
                 if (resp < 0) break;
                 ASSERT(resp <= (1024 * size));
             }

             if (veryVerbose) { P_(resp); P(tolerance); }
             if (resp > 0 && tolerance-- <= 0) {
                 ASSERT(resp == btlso::SocketHandle::e_ERROR_CONNDEAD);
             }
             testFactory.deallocate(streamSocketA);
         }

         if (veryVerbose) cout << "\n   test writev response "
                               << "btlso::SocketHandle::e_ERROR_CONNDEAD"
                               << endl;

         // Verify error response when remote socket closed.
         // Try the write with different sizes because the behavior
         // of underlying sockets vary.  We don't try larger than
         // 32K per write because some platforms have a limit.
         for (int tolerance = 3, size = 1; size < 32; ++size) {
             TestStreamSocketFactory<btlso::IPv4Address> testFactory;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
             btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
             int resp = createConnectedStreamSockets(&testFactory,
                                                     &streamSocketA,
                                                     &streamSocketB);

             ASSERT(resp);

             char zerobuf[1024*32] = {0};

             testFactory.deallocate(streamSocketB);

             bslmt::ThreadUtil::microSleep(100);

             // The first write to the socket after the remote socket
             // has been closed may succeed.  Eventually the write will
             // fail (normally after the first or second write).

             for (int i = 0; i < 10; ++i) {
                 btls::Ovec vec[2];
                 vec[0].setBuffer(zerobuf, 1024 * size);
                 vec[1].setBuffer(zerobuf, 2);
                 resp = streamSocketA->writev(vec, 2);
                 if (resp < 0) break;
                 ASSERT(resp <= (1024 * size + 2));
             }

             if (veryVerbose) { P_(resp); P(tolerance); }
             if (resp > 0 && tolerance-- <= 0) {
                 ASSERT(resp == btlso::SocketHandle::e_ERROR_CONNDEAD);
             }
             testFactory.deallocate(streamSocketA);
         }

         {
             if (veryVerbose) cout << "\n   test write response "
                                   << "btlso::SocketHandle::e_ERROR_WOULDBLOCK"
                                   << endl;

             int packetSize;

             for (packetSize = 500; packetSize < 5505; packetSize += 2501) {
                 // Create a server socket
                 if (veryVerbose) P(packetSize);

                 TestStreamSocketFactory<btlso::IPv4Address> testFactory;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
                 int resp = createConnectedStreamSockets(&testFactory,
                                                         &streamSocketA,
                                                         &streamSocketB);

                 ASSERT(resp);

                 streamSocketA->setBlockingMode(
                     btlso::Flag::e_NONBLOCKING_MODE);

                 streamSocketB->setBlockingMode(
                     btlso::Flag::e_NONBLOCKING_MODE);

                 // Fill the outgoing socket buffer for socket A
                 char zerobuf[1024*64];
                 memset(zerobuf, 0, sizeof zerobuf);

                 int totalSent = 0;

                 // Keep going until write fails for "would block"
                 // Note that this test does not require that both the receive
                 // and send buffers be filled.
                 int i = 0;
                 while (1) {
                     resp = streamSocketA->write(zerobuf, packetSize);
                     if (resp <= 0) break;

                     ASSERT(resp <= packetSize);
                     if ((veryVerbose && resp < packetSize)
                      || veryVeryVerbose) {
                         P_(i); P(resp);
                     }
                     totalSent += resp;
                     ++i;
                 }

                 if (veryVerbose) {
                     P(i);
                     P(totalSent);
                 }

                 LOOP_ASSERT(packetSize,
                             resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

                 // Verify that when receiver takes all the data, sender
                 // can send again.

                 char receiveBuf[sizeof(zerobuf)];

                 int totalReceived = 0;
                 int iterations = 0;

                 while (1) {
                     resp = streamSocketB->read(receiveBuf,
                                                sizeof(receiveBuf));
                     if (resp <= 0) {
                         if (totalReceived < totalSent && iterations < 30) {
                             if (veryVerbose) {
                                 P_(iterations); P_(totalReceived);
                                 P(totalSent);
                             }
                             bslmt::ThreadUtil::microSleep(100);
                             iterations++;
                         }
                         else break;
                     }
                     else {
                         totalReceived += resp;
                         LOOP_ASSERT(packetSize,
                                     resp <= (int)sizeof(receiveBuf));
                     }
                 }

                 LOOP_ASSERT(packetSize, totalReceived == totalSent);
                 LOOP_ASSERT(packetSize,
                             resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

                 // Latency

                 bslmt::ThreadUtil::microSleep(100);

                 // Write data and verify that it writes OK

                 resp = streamSocketA->write(zerobuf, packetSize);

                 if (veryVerbose) P(resp);
                 LOOP_ASSERT(packetSize, resp > 0 && resp <= packetSize);

                 testFactory.deallocate(streamSocketA);
                 testFactory.deallocate(streamSocketB);
             }
         }

         {
             if (veryVerbose) cout << "\n   test writev response "
                                   << "btlso::SocketHandle::e_ERROR_WOULDBLOCK"
                                   << endl;

             int packetSize;

             for (packetSize = 500; packetSize < 5505; packetSize += 2501) {
                 // Create a server socket
                 if (veryVerbose) P(packetSize);

                 TestStreamSocketFactory<btlso::IPv4Address> testFactory;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
                 btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
                 int resp = createConnectedStreamSockets(&testFactory,
                                                         &streamSocketA,
                                                         &streamSocketB);

                 ASSERT(resp);

                 streamSocketA->setBlockingMode(
                     btlso::Flag::e_NONBLOCKING_MODE);

                 streamSocketB->setBlockingMode(
                     btlso::Flag::e_NONBLOCKING_MODE);

                 // Fill the outgoing socket buffer for socket A
                 char zerobuf[1024*64];
                 memset(zerobuf, 0, sizeof zerobuf);

                 int totalSent = 0;

                 // Keep going until write fails for "would block"
                 // Note that this test does not require that both the receive
                 // and send buffers be filled.
                 int i = 0;
                 while (1) {
                     btls::Ovec vec[2];
                     vec[0].setBuffer(zerobuf, packetSize);
                     vec[1].setBuffer(zerobuf, 2);
                     resp = streamSocketA->writev(vec, 2);
                     if (resp <= 0) break;

                     ASSERT(resp <= (packetSize + 2));
                     if ((veryVerbose && resp < (packetSize + 2))
                      || veryVeryVerbose)
                     {
                         P_(i); P(resp);
                     }
                     totalSent += resp;
                     ++i;
                 }

                 if (veryVerbose) P(i);
                 if (veryVerbose) cout << "Total Sent " << totalSent << endl;

                 ASSERT(resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

                 // Verify that when receiver takes all the data, sender
                 // can send again.

                 char receiveBuf[sizeof(zerobuf)];
                 int totalReceived = 0;
                 int iterations = 0;

                 while (1) {
                     resp = streamSocketB->read(receiveBuf,
                                                sizeof(receiveBuf));
                     if (resp <= 0) {
                         if (totalReceived < totalSent && iterations < 30) {
                             if (veryVerbose) {
                                 P_(iterations); P_(totalReceived);
                                 P(totalSent);
                             }
                             bslmt::ThreadUtil::microSleep(100);
                             iterations++;
                         }
                         else break;
                     }
                     else {
                         totalReceived += resp;
                         LOOP_ASSERT(packetSize,
                                     resp <= (int)sizeof(receiveBuf));
                     }
                 }

                 LOOP_ASSERT(packetSize, totalReceived == totalSent);
                 LOOP_ASSERT(packetSize,
                             resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

                 // Latency

                 bslmt::ThreadUtil::microSleep(100);

                 // Write data and verify that it writes OK

                 btls::Ovec vec[2];
                 vec[0].setBuffer(zerobuf, packetSize);
                 vec[1].setBuffer(zerobuf, 2);
                 resp = streamSocketA->writev(vec, 2);

                 if (veryVerbose) P(resp);
                 ASSERT(resp > 0 && resp <= (packetSize + 2));

#ifndef BSLS_PLATFORM_OS_CYGWIN
                 testFactory.deallocate(streamSocketA);
                 testFactory.deallocate(streamSocketB);
#else
                 // Cygwin is doing something funky in its close() function,
                 // which force us to close the client socket first.

                 testFactory.deallocate(streamSocketB);
                 testFactory.deallocate(streamSocketA);
#endif
             }
         }
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'waitForIO'
        //
        // Plan:
        //   Create a pair of connected sockets (A and B).  Set socket A to
        //   non-blocking.  Fill the write queue of socket A by writing to it
        //   and not reading from socket B.
        //   Write will eventually return ERROR_WOULDBLOCK.
        //   Call waitFor(read/write, 10) on socket A; (all 3 permutations)
        //   1) no prior extra read/write
        //   2) alarm 5 seconds.  UNIX only - tests interrupted call
        //   3) read 10K bytes from socket B first (sleep first)
        //   4) write 1 byte to socket B first.
        //   5) read 10K bytes from B and write 1 byte to socket B first.
        //
        //   Test with and without TCPNODELAY option (Nagle algorithm).
        //
        // Testing:
        //   int waitForIO(type,const bsls::TimeInterval& timeout);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing waitForIO" << endl
                          << "=================" << endl;
#if !defined(BSLS_PLATFORM_OS_AIX)           \
 && !defined(BSLS_PLATFORM_OS_WINDOWS)       \
 && !defined(BSLS_PLATFORM_OS_LINUX)

        const btlso::Flag::IOWaitType RD = btlso::Flag::e_IO_READ;
        const btlso::Flag::IOWaitType WR = btlso::Flag::e_IO_WRITE;
        const btlso::Flag::IOWaitType RW = btlso::Flag::e_IO_RW;
        const int                   TO = btlso::SocketHandle::e_ERROR_TIMEDOUT;

        typedef btlso::Flag S;
        static const struct {
            int           d_lineNum;      // source line number
            S::IOWaitType d_io;           // what to wait for
            int           d_seconds;      // how many seconds to wait
            int           d_microseconds; // how many microseconds to wait
            int           d_readPercent;  // percent of the written bytes to
                                          // read
            int           d_writebytes;   // num of bytes to write to B
            int           d_delay;        // delay in millisecs before wait
            int           d_tcpndelay;    // 1 if nagle off
            int           d_expected;     // expected result
        } DATA[] = {
        //line io  secs  usecs  read   write  delay  tcpndelay expected
        //---- --  ----  -----  -----  -----  -----  --------- --------
        { L_,  RD,    0,     0,     0,     0,     0,         0, TO },
        { L_,  WR,    0,     0,     0,     0,     0,         0, TO },
        { L_,  RW,    0,     0,     0,     0,     0,         0, TO },
        { L_,  RD,    0,     0,    75,     0,   200,         0, TO },
        { L_,  WR,    0,     0,    75,     0,   200,         0, WR },
        { L_,  RW,    0,     0,    75,     0,   200,         0, WR },
        { L_,  RD,    0,     0,    75,  1000,   200,         0, RD },
        { L_,  WR,    0,     0,    75,  1000,   200,         0, WR },

//         { L_,  RW,    0,     0,    75,  1000,   200,         0, RW },
        { L_,  RD,    0,     0,     0,  1000,     0,         0, RD },
        { L_,  WR,    0,     0,     0,  1000,   200,         0, WR },
//         { L_,  RW,    0,     0,     0,  1000,   200,         0, RW },
        { L_,  RD,    0,  2000,     0,     0,     0,         0, TO },
        { L_,  WR,    0,  2000,     0,     0,     0,         0, TO },
        { L_,  RW,    0,  2000,     0,     0,     0,         0, RD },
        { L_,  RD,    0,  2000,    75,     0,   200,         0, TO },
        { L_,  WR,    0,  2000,    75,     0,   200,         0, WR },
        { L_,  RW,    0,  2000,    75,     0,   200,         0, WR },
        { L_,  RD,    0,  2000,    75,  1000,   200,         0, RD },
        { L_,  WR,    0,  2000,    75,  1000,   200,         0, WR },
//         { L_,  RW,    0,  2000,    75,  1000,   200,         0, RW },
        { L_,  RD,    0,  2000,     0,  1000,     0,         0, RD },
        { L_,  WR,    0,  2000,     0,  1000,     0,         0, WR },
//         { L_,  RW,    0,  2000,     0,  1000,     0,         0, RW },
        { L_,  RD,    1,     0,     0,     0,     0,         0, TO },
        { L_,  WR,    1,     0,     0,     0,     0,         0, WR },
        { L_,  RW,    1,     0,     0,     0,     0,         0, WR },
        { L_,  RD,    1,     0,    75,     0,     0,         0, TO },
        { L_,  WR,    1,     0,    75,     0,     0,         0, WR },
        { L_,  RW,    1,     0,    75,     0,     0,         0, WR },
        { L_,  RD,    1,     0,    75,  1000,   200,         0, RD },
        { L_,  WR,    1,     0,    75,  1000,   200,         0, WR },
//         { L_,  RW,    1,     0,    75,  1000,   200,         0, RW },
        { L_,  RD,    1,     0,     0,  1000,     0,         0, RD },
        { L_,  WR,    1,     0,     0,  1000,     0,         0, WR },
//         { L_,  RW,    1,     0,     0,  1000,     0,         0, RW },
        { L_,  RD,    0,     0,     0,     0,     0,         1, TO },
        { L_,  WR,    0,     0,     0,     0,     0,         1, TO },
        { L_,  RW,    0,     0,     0,     0,     0,         1, TO },
        { L_,  RD,    0,     0,    75,     0,   500,         1, TO },
        { L_,  WR,    0,     0,    75,     0,   500,         1, WR },
        { L_,  RW,    0,     0,    75,     0,   500,         1, WR },
        };

        enum {
            NUM_DATA      = sizeof DATA / sizeof *DATA,

            DATASIZE      = 100,  // total amount of initial data
            PACKETSIZE    = 200   // read/write packet size
        };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
           if (veryVerbose) {
               T_(); T_(); P_(ti);
                           P(DATA[ti].d_lineNum);
           }

            // Create a stream socket pair
            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
            int resp = createConnectedStreamSockets(&testFactory,
                                                    &streamSocketA,
                                                    &streamSocketB);

            LOOP_ASSERT(ti, resp);
            if (resp == 0) {
                // could not create connected socket pair
                break;
            }

            // Turn off Nagle for sending side (A) if specified
            streamSocketA->setOption(btlso::SocketOptUtil::k_TCPLEVEL,
                                     btlso::SocketOptUtil::k_TCPNODELAY,
                                     DATA[ti].d_tcpndelay);

            streamSocketA->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);

            // Fill the output buffer for socket A.
            char zerobuf[DATASIZE] = {0};
            int  totalSent = 0;

            // Attempt to fill the output buffer of A by writing until we
            // receive an ERROR_WOULDBLOCK response.  Then, try several times
            // more to ensure that ERROR_WOULDBLOCK is persistent, and that
            // the write buffer is *really* full.

            for (int attempt = 0; attempt < 10; ++attempt ) {
                bsls::TimeInterval before = bdlt::CurrentTime::now();
                while (1) {
                    resp = streamSocketA->write(zerobuf, DATASIZE);
                    if (resp <= 0) break;
                    totalSent += resp;
                }

                bsls::TimeInterval interval =
                                             bdlt::CurrentTime::now() - before;

                if (veryVeryVerbose) {
                    P_(attempt); P_(resp); P_(totalSent); P(interval);
                }

                LOOP_ASSERT(ti, resp ==
                            btlso::SocketHandle::e_ERROR_WOULDBLOCK);
            }

            // Read input on B, PACKETSIZE bytes at a time, until all input is
            // consumed.  Then write data back on B.  Total read will be total
            // sent on A or specified in table, whichever is less.  The total
            // sent is platform- and system-specific, and can be modified
            // dynamically.  Reads and writes are chunked in order to exercise
            // the 'read' and 'write' functions several times over the data
            // stream.

            char buf[DATASIZE];

            if (veryVeryVerbose) cout << "reading..." << endl;
            int leftToRead = DATA[ti].d_readPercent * totalSent / 100;
            while (leftToRead) {
                int count = leftToRead > PACKETSIZE ? PACKETSIZE : leftToRead;
                int bytesRead;
                if (veryVeryVerbose) { P_(count); P(leftToRead); }
                bytesRead = streamSocketB->read(buf, count);
                leftToRead -= bytesRead;
            }

            // Write even if we have previously not read anything.
            if (veryVeryVerbose) cout << "writing..." << endl;
            int leftToWrite = DATA[ti].d_writebytes;
            while (leftToWrite) {
                int count = leftToWrite > PACKETSIZE
                            ? PACKETSIZE : leftToWrite;
                if (veryVeryVerbose) { P_(count); P(leftToWrite); }
                streamSocketB->write(buf, count);
                leftToWrite -= count;
            }

            // delay before testing for IO to allow for latency.
            if (DATA[ti].d_delay) {
                if (veryVerbose) {
                    P(DATA[ti].d_delay);
                }
                bslmt::ThreadUtil::microSleep(DATA[ti].d_delay * 1000);
            }

           // some platforms require latency between writes and reads
#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_AIX)
            const int sleepTime = 1000;
#else
            const int sleepTime = 0;
#endif
            if (sleepTime) bslmt::ThreadUtil::microSleep(sleepTime);

            bsls::TimeInterval timeBefore = bdlt::CurrentTime::now();

            bsls::TimeInterval timeout = timeBefore;
            timeout.addSeconds(DATA[ti].d_seconds);
            timeout.addMicroseconds(DATA[ti].d_microseconds);
            resp = streamSocketA->waitForIO(DATA[ti].d_io, timeout);

            bsls::TimeInterval timeAfter = bdlt::CurrentTime::now();

            if (veryVerbose) { P_(resp); P(DATA[ti].d_expected); }

           // Assuming the latency and system responsiveness is
           // never worse than 5 seconds.

            if (resp == btlso::SocketHandle::e_ERROR_TIMEDOUT) {
                // If a timeout occurred, time must be > (timeout - 10ms)
                // and within 5 seconds
                LOOP_ASSERT(ti, (timeAfter + 0.01) > timeout);
                LOOP_ASSERT(ti, timeAfter < (timeout + 5));
            } else {
                // If a timeout did not occur, event must occur with 5
                // seconds
                LOOP_ASSERT(ti, (timeBefore + 5) > timeAfter);
            }
            if (veryVeryVerbose) {
                P_(timeBefore); P_(timeAfter); P(timeout);
            }

            LOOP3_ASSERT(DATA[ti].d_lineNum, resp, DATA[ti].d_expected,
                         resp == DATA[ti].d_expected
                      || resp == btlso::SocketHandle::e_ERROR_TIMEDOUT);

#ifndef BSLS_PLATFORM_OS_CYGWIN
            testFactory.deallocate(streamSocketA);
            testFactory.deallocate(streamSocketB);
#else
            // Cygwin is doing something funky in its close() function, which
            // force us to close the client socket first.

            testFactory.deallocate(streamSocketB);
            testFactory.deallocate(streamSocketA);
#endif
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SOCKET CREATION HELPER FUNCTION
        //
        // Plan:
        //
        // Testing:
        //   createConnectedStreamSockets()
        // --------------------------------------------------------------------

        // Test socket creation tool

        if (verbose) cout << endl
                          << "Testing Socket Creation Function" << endl
                          << "================================" << endl;

        TestStreamSocketFactory<btlso::IPv4Address> testFactory;
        btlso::StreamSocket<btlso::IPv4Address> *streamSocketA = 0;
        btlso::StreamSocket<btlso::IPv4Address> *streamSocketB = 0;
        int resp = createConnectedStreamSockets(&testFactory,
                                                &streamSocketA,
                                                &streamSocketB);

        ASSERT(resp);
        ASSERT(streamSocketA);
        ASSERT(streamSocketB);
        ASSERT(streamSocketA != streamSocketB);
        ASSERT(streamSocketA->handle() != streamSocketB->handle());

        // Verify send and receive of data through the sockets.
        const char buf1[] = { 10, 9, 1, 7, 3 };
        char bufrcv[10];
        resp = streamSocketA->write(buf1, sizeof(buf1));
        ASSERT(resp == sizeof(buf1));

        resp = streamSocketB->read(bufrcv, sizeof(bufrcv));
        ASSERT(resp == sizeof(buf1));

        ASSERT(memcmp(bufrcv, buf1, sizeof(buf1)) == 0);

        testFactory.deallocate(streamSocketA);
        testFactory.deallocate(streamSocketB);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'bind' 'accept' 'connect' FUNCTIONS
        //
        // Plan:
        //
        // Testing:
        //   int accept(socket, ADDRESS *peerAddress);
        //   int bind(const ADDRESS& address);
        //   int connect(const ADDRESS& address);
        //   int listen(int backlog);
        //   int setBlockingMode(mode);
        //   int shutdown(streamOption);
        //   int blockingMode(result) const;
        //   int localAddress(ADDRESS *result) const;
        //   int peerAddress(ADDRESS *result) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING bind/accept/connect/listen" << endl
                 << "==================================" << endl;

        // Counts number of sockets currently open (on certain platforms
        // only).
        int initialSockets = countSockets(MIN_SOCKET_HANDLE,
                                          MAX_SOCKET_HANDLE);
        TestStreamSocketFactory<btlso::IPv4Address> testFactory;

        // Test bind() with bad port number < 1024 (we are not root)
        // On windows, this is allowed.
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_CYGWIN)
        if (verbose)
            cout << "\tBinding to a bad port number." << endl;
        {
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with a low port number

            btlso::IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(1);

            if (veryVerbose) {
                P(localLowPort);
            }
            int resp = serverSocket->bind(localLowPort);

            ASSERT(resp != 0);
            testFactory.deallocate(serverSocket);
        }
#endif

        {
#ifdef BSLS_PLATFORM_OS_UNIX
            // Under UNIX, a write to a closed blocking socket causes the
            // signal SIGPIPE which by default will terminate the process.
            // Set the signal to be ignored.
            ::signal(SIGPIPE, SIG_IGN);
#endif
        }

        if (verbose)
            cout << "\tBinding to port 0 (anonymous)." << endl;
        {
            // Test bind() with anon port number.
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with anon port number

            btlso::IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localLowPort);
            ASSERT(resp == 0);

            btlso::IPv4Address serverAddress;
            resp = serverSocket->localAddress(&serverAddress);
            ASSERT(resp == 0);

            if (veryVerbose) {
                P(serverAddress);
            }

            // The address must be the same.  The port number may not
            // be zero.
            ASSERT(localLowPort.ipAddress() == serverAddress.ipAddress());
            ASSERT(serverAddress.portNumber() > 0);

            resp = serverSocket->listen(10);
            ASSERT(resp == 0);

            // Create the client socket.

            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            if (verbose)
                cout << "\tBlocking connect to " << serverAddress << endl;
            resp = clientSocket->connect(serverAddress);
            ASSERT(resp == 0);

            // Accept the connection
            btlso::StreamSocket<btlso::IPv4Address> *serviceSocket;
            btlso::IPv4Address clientAddress;

            resp = serverSocket->accept(&serviceSocket, &clientAddress);
            ASSERT(resp == 0);

            if (verbose)
                cout << "\tSending packets." << endl;
            const char buf1[] = { 10, 9, 1, 7, 3 };
            const char buf2[] = { 100, 51 };
            char bufrcv[10];
            resp = serviceSocket->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

            resp = clientSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1));

            ASSERT(memcmp(bufrcv, buf1, sizeof(buf1)) == 0);

            // Send as 2 writes + 1 read.  If truly a stream socket
            // then the read with retrieve data for both writes.  The
            // behavior would be different for a datagram socket.

            resp = clientSocket->write(buf2, sizeof(buf2));
            ASSERT(resp == sizeof(buf2));

            resp = clientSocket->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

            // Some implementations of sockets will delay the second
              // packet due to the Nagle algorithm
            bslmt::ThreadUtil::microSleep(2000 * 1000);

            resp = serviceSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1) + sizeof(buf2));
            ASSERT(memcmp(bufrcv, buf2, sizeof(buf2)) == 0);
            ASSERT(memcmp(bufrcv + sizeof(buf2), buf1, sizeof(buf1)) == 0);

            // Verify shutdown()

            // Shutdown of receive is not universally supported - it may be
            // ignored.  'both' is always supported but the implementation
            // may not shutdown receive.

            resp = serviceSocket->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
            ASSERT(resp == 0);

            resp = serviceSocket->write(buf1, sizeof(buf1));

            resp = clientSocket->read(bufrcv, sizeof(buf1));

            ASSERT(resp == btlso::SocketHandle::e_ERROR_EOF);

            testFactory.deallocate(serviceSocket);
            testFactory.deallocate(serverSocket);
            testFactory.deallocate(clientSocket);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

        if (verbose)
            cout << "\tNon-blocking connect to anonymous port." << endl;

        {
            // Test bind() with anon port number.  Non Blocking client
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with anon port number

            btlso::IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localLowPort);
            ASSERT(resp == 0);

            btlso::IPv4Address serverAddress;
            resp = serverSocket->localAddress(&serverAddress);
            ASSERT(resp == 0);

            if (veryVerbose)
                P(serverAddress);

            // The address must be the same.  The port number may not
            // be zero.
            ASSERT(localLowPort.ipAddress() == serverAddress.ipAddress());
            ASSERT(serverAddress.portNumber() > 0);

            resp = serverSocket->listen(0);
            ASSERT(resp == 0);

            // Create the client socket.

            btlso::StreamSocket<btlso::IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            btlso::Flag::BlockingMode result =
                                            btlso::Flag::e_NONBLOCKING_MODE;

            resp = clientSocket->blockingMode(&result);
            if (resp == 0) {
                // If this platform supports this feature, verify result.
                ASSERT(result == btlso::Flag::e_BLOCKING_MODE);
            }

            clientSocket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);

            resp = clientSocket->blockingMode(&result);
            if (resp == 0) {
                // If this platform supports this feature, verify result.
                ASSERT(result == btlso::Flag::e_NONBLOCKING_MODE);
            }

            // In non-blocking mode, some platforms will return 0
            // and others return btlso::SocketHandle::e_ERROR_WOULDBLOCK
            if (verbose)
                cout << "\tNon-blocking connect to " << serverAddress << endl;
            resp = clientSocket->connect(serverAddress);
            ASSERT(resp == 0 ||
                   resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

            // Accept the connection
            btlso::StreamSocket<btlso::IPv4Address> *serviceSocket;
            btlso::IPv4Address clientAddress;

            resp = serverSocket->accept(&serviceSocket, &clientAddress);
            ASSERT(resp == 0);

            btlso::IPv4Address servicePeerAddress;
            resp = serviceSocket->peerAddress(&servicePeerAddress);
            ASSERT(resp == 0);
            ASSERT(servicePeerAddress == clientAddress);

            btlso::IPv4Address clientLocalAddress;
            resp = clientSocket->localAddress(&clientLocalAddress);
            ASSERT(servicePeerAddress == clientLocalAddress);

            btlso::IPv4Address clientPeerAddress;
            resp = clientSocket->peerAddress(&clientPeerAddress);
            ASSERT(resp == 0);

            btlso::IPv4Address serviceLocalAddress;
            resp = serviceSocket->localAddress(&serviceLocalAddress);
            ASSERT(clientPeerAddress == serviceLocalAddress);
            ASSERT(clientPeerAddress == serverAddress);
            ASSERT(clientLocalAddress != serviceLocalAddress);
            ASSERT(clientPeerAddress != servicePeerAddress);

            if (veryVerbose) {
                P(clientPeerAddress);
                P_(clientLocalAddress); P(servicePeerAddress);
            }
            if (verbose)
                cout << "\tSending packets." << endl;
            const char buf1[] = { 10, 9, 1, 7, 3 };
            const char buf2[] = { 100, 51 };
            char bufrcv[10];

            resp = clientSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == btlso::SocketHandle::e_ERROR_WOULDBLOCK);

            resp = serviceSocket->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

// #if defined(BSLS_PLATFORM_OS_WINDOWS)
// // Some TCP driver implementations require some delay between write and
// // read (on the loopback service) in order to recognize I/O events
// // correctly.
//             bslmt::ThreadUtil::microSleep(20 * 1000);
// #endif

            resp = clientSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1));

            ASSERT(memcmp(bufrcv, buf1, sizeof(buf1)) == 0);

            // Send as 2 writes + 1 read.  If truly a stream socket
            // then the read with retrieve data for both writes.  The
            // behavior would be different for a datagram socket.

            resp = clientSocket->write(buf2, sizeof(buf2));
            ASSERT(resp == sizeof(buf2));

            resp = clientSocket->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

            // Some implementations of sockets will delay the second
            // packet due to the Nagle algorithm
            bslmt::ThreadUtil::microSleep(2000 * 1000);

            resp = serviceSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1) + sizeof(buf2));
            ASSERT(memcmp(bufrcv, buf2, sizeof(buf2)) == 0);
            ASSERT(memcmp(bufrcv + sizeof(buf2), buf1, sizeof(buf1)) == 0);

            // Verify shutdown()

            resp = serviceSocket->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);
            ASSERT(resp == 0);

            resp = serviceSocket->write(buf1, sizeof(buf1));
            ASSERT(resp < 0);

// #if defined(BSLS_PLATFORM_OS_WINDOWS)
// // Some TCP driver implementations require some delay between write and
// // read (on the loopback service) in order to recognize I/O events
// // correctly.
//             bslmt::ThreadUtil::microSleep(20 * 1000);
// #endif

            resp = clientSocket->read(bufrcv, sizeof(buf1));
            ASSERT(btlso::SocketHandle::e_ERROR_EOF == resp ||
                   btlso::SocketHandle::e_ERROR_CONNDEAD == resp);

            testFactory.deallocate(serviceSocket);
            testFactory.deallocate(serverSocket);
            testFactory.deallocate(clientSocket);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

        // Test accept when the connecting socket (client) closes before
        // the accept is performed.
        {
            TestStreamSocketFactory<btlso::IPv4Address> testFactory;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketA;
            btlso::StreamSocket<btlso::IPv4Address> *streamSocketB;
            streamSocketA = 0;
            streamSocketB = 0;
            btlso::StreamSocket<btlso::IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with anon port number

            btlso::IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localLowPort);
            ASSERT(resp == 0);

            btlso::IPv4Address serverAddress;
            resp = serverSocket->localAddress(&serverAddress);
            ASSERT(resp == 0);

            resp = serverSocket->listen(2);
            ASSERT(resp == 0);

            // Create the client socket.
            streamSocketA = testFactory.allocate();
            streamSocketB = testFactory.allocate();

            resp = (streamSocketA)->connect(serverAddress);
            ASSERT(resp == 0);

            if (streamSocketA) {
                // Accept the connection
                btlso::IPv4Address clientAddress;

                // Generate a "rst" (linger time = 0, followed by shutdown)
                btlso::SocketOptUtil::LingerData ling;
                ling.l_onoff = 1;
                ling.l_linger = 0;
                (streamSocketA)->setLingerOption(ling);

                // kill the connection before accept
                (streamSocketA)->shutdown(btlso::Flag::e_SHUTDOWN_BOTH);

                // close connection  after rst
                testFactory.deallocate(streamSocketA);

                // Wait for 2 seconds
                bslmt::ThreadUtil::microSleep(2000 * 1000);

#ifdef BSLS_PLATFORM_OS_UNIX
                ::signal(SIGALRM, sigalarm);
                ::alarm(5);
#endif

                resp = serverSocket->accept(&streamSocketB, &clientAddress);

#ifdef BSLS_PLATFORM_OS_UNIX
                // Cancel the alarm
                ::signal(SIGALRM, SIG_IGN);
#endif

                // The behavior after the connecting socket has closed is
                // implementation dependent.
                // Possible responses can be:
                //  1) incoming connection is discarded so accept will
                //     block.  ERROR_INTERRUPTED
                //  2) accept returns an error.  ERROR_CONNDEAD
                //  3) accept returns the socket but a read confirms the
                //     socket has gone.  ERROR_EOF or ERROR_CONNDEAD
                //
                typedef btlso::SocketHandle SH;

                if (resp != 0) {
                    LOOP_ASSERT(resp,SH::e_ERROR_CONNDEAD     == resp ||
                                SH::e_ERROR_INTERRUPTED  == resp);
                }
                else {
                    char buf[1];
                    resp = streamSocketB->read(buf,1 );
                    ASSERT(SH::e_ERROR_CONNDEAD == resp ||
                           SH::e_ERROR_EOF      == resp);
                }
                testFactory.deallocate(streamSocketB);
            } else {
                testFactory.deallocate(serverSocket);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Plan:
        //
        // Testing:
        //   btlso::InetStreamSocket(handle, bslma::Allocator *allocator);
        //   ~btlso::InetStreamSocket<ADDRESS>();
        //   btlso::SocketHandle::Handle handle() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing CREATORS" << endl
                          << "================" << endl;

           btlso::SocketHandle::Handle handles[2];

           int resp = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                        handles,
                                        btlso::SocketImpUtil::k_SOCKET_STREAM);
           ASSERT(resp == 0);

           btlso::InetStreamSocket<btlso::IPv4Address> newSocketA(handles[0]);
           btlso::InetStreamSocket<btlso::IPv4Address> newSocketB(handles[1]);

           btlso::SocketHandle::Handle newHandleA = newSocketA.handle();
           btlso::SocketHandle::Handle newHandleB = newSocketB.handle();
           ASSERT(verifyHandle(newHandleA) != 0);
           ASSERT(verifyHandle(newHandleB) != 0);

           ASSERT(handles[0] == newHandleA);
           ASSERT(handles[1] == newHandleB);

           // Send data via the streamsockets.

           const char buf1[] = { 10, 9, 1, 7, 3 };
           char bufrcv[10];
           resp = newSocketA.write(buf1, sizeof(buf1));
           ASSERT(resp == sizeof(buf1));

           // Verify that the data was received as sent.

           resp = newSocketB.read(bufrcv, sizeof(bufrcv));
           ASSERT(resp == sizeof(buf1));

           ASSERT(memcmp(bufrcv, buf1, sizeof(buf1)) == 0);

           btlso::SocketImpUtil::close(newSocketA.handle());

           resp = newSocketB.read(bufrcv, sizeof(bufrcv));

           // socket A is closed so expect EOF

           ASSERT(resp == btlso::SocketHandle::e_ERROR_EOF);

           btlso::SocketImpUtil::close(newSocketB.handle());

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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = 255;
      }
    }

    int cleanupResp;
    if ((cleanupResp = btlso::SocketImpUtil::cleanup()) != 0) {
        cout << "btlso::SocketImpUtil::cleanup failed " << cleanupResp << endl;
    }

    if (testStatus > 0 && testStatus < 255) {
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
