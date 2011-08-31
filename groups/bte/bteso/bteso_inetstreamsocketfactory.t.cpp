// bteso_inetstreamsocketfactory.t.cpp        -*-C++-*-

#include <bteso_inetstreamsocketfactory.h>

#include <bteso_socketimputil.h>
#include <bteso_ipv4address.h>
#include <bteso_platform.h>

#include <bcemt_thread.h>     // bcemt_ThreadUtil::microSleep()

#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM__OS_UNIX
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
//  The bteso_InetStreamSocket represents the lowest layer at which all socket
//  functionality is combined.  This means that in addition to testing that
//  functions forward onto the lower layer functions, the functionality
//  with regards to all of a sockets implementation must be tested.
//
//  Each socket implementation is different in its behavior including
//  their interpretation of the BSD sockets interface.
//
//-----------------------------------------------------------------------------
// Class bteso_InetStreamSocketFactory
// ===================================
// [ 2] bteso_InetStreamSocketFactory(basicAllocator = 0);
// [ 2] ~bteso_InetStreamSocketFactory();
// [ 2] bteso_StreamSocket<ADDRESS> *allocate();
// [ 3] bteso_StreamSocket<ADDRESS> *allocate(handle);
// [ 2] void deallocate(bteso_StreamSocket<ADDRESS> *socket);
// [ 2] void deallocate(bteso_StreamSocket<ADDRESS> *socket, bool closeFlag);
//
// Class bteso_InetStreamSocket
// ============================
// [ 2] bteso_InetStreamSocket(handle, bslma_Allocator *allocator);
// [ 2] ~bteso_InetStreamSocket<ADDRESS>();
// [  ] int accept(result);
// [ 4] int accept(socket, ADDRESS *peerAddress);
// [ 4] int bind(const ADDRESS& address);
// [ 4] int connect(const ADDRESS& address);
// [ 4] int listen(int backlog);
// [ 8] int read(char *buffer, int length);
// [ 8] int readv(const btes_Iovec *buffers, int numBuffers);
// [ 7] int write(const char *buffer, int length);
// [ 7] int writev(const btes_Iovec *buffers, int numBuffers);
// [ 7] int writev(const btes_Ovec *buffers, int numBuffers);
// [ 4] int setBlockingMode(mode);
// [ 4] int shutdown(streamOption);
// [10] int waitForConnect(const bdet_TimeInterval& timeout);
// [ 9] int waitForAccept(const bdet_TimeInterval& timeout);
// [ 6] int waitForIO(type,const bdet_TimeInterval& timeout);
// [  ] int setLingerOption(options);
// [11] int setOption(int level, int option, int value);
// [ 4] int blockingMode(result) const;
// [ 4] int localAddress(ADDRESS *result) const;
// [ 4] int peerAddress(ADDRESS *result) const;
// [ 2] bteso_SocketHandle::Handle handle() const;
// [10] int connectionStatus() const;
// [  ] int lingerOption(bteso_SocketOptUtil::LingerData *result) const;
// [11] int socketOption(int *result, int level, int option) const;
//-----------------------------------------------------------------------------
// [ 5] createConnectedStreamSockets()

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

#if defined(BTESO_PLATFORM__WIN_SOCKETS) \
    || defined(BSLS_PLATFORM__OS_HPUX)
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
#if defined(BTESO_PLATFORM__WIN_SOCKETS) || defined(BSLS_PLATFORM__OS_CYGWIN)

    // Cygwin does not work around the Windows limitation.  See the source
    // OR http://sources.redhat.com/ml/cygwin/2000-12/msg00422.html.

    return -1;
#else
    int socketNumber;
    int socketCount = 0;
    char address[20];
    ADDRLEN_T len = sizeof(address);

    for (socketNumber = base; socketNumber <= max; ++socketNumber) {
         int ret = ::getsockname((bteso_SocketHandle::Handle) socketNumber,
                               (sockaddr *) &address, &len);

         if (ret >= 0) ++socketCount;
    }

    return socketCount;
#endif
}

int verifyHandle(bteso_SocketHandle::Handle handle)
    // Return 1 if the specified handle corresponds to an open socket.
{
    char address[sizeof(sockaddr_in)];
    ADDRLEN_T len = sizeof(address);

    int ret = ::getsockname((bteso_SocketHandle::Handle) handle,
                            (sockaddr *) &address[0], &len);

#if defined(BTESO_PLATFORM__WIN_SOCKETS)
    if (ret != 0 && WSAGetLastError() == WSAEINVAL) ret = 0;
#endif
#if defined(BSLS_PLATFORM__OS_CYGWIN)
    if (ret != 0 && errno == EINVAL) ret = 0;
#endif

    return ret == 0 ? 1 : 0;
}

int createConnectedStreamSockets(
    bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
    bteso_StreamSocket<bteso_IPv4Address> **streamSocketA,
    bteso_StreamSocket<bteso_IPv4Address> **streamSocketB)
    // On success return 1, 0 otherwise.
{
    *streamSocketA = 0;
    *streamSocketB = 0;
    bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
      factory->allocate();

    if (serverSocket == 0) return 0;

    // Bind to localhost with anon port number

    bteso_IPv4Address localLowPort;
    localLowPort.setIpAddress("127.0.0.1");
    localLowPort.setPortNumber(0);

    // bind to a anon port
    int resp = serverSocket->bind(localLowPort);
    ASSERT(resp == 0);

    bteso_IPv4Address serverAddress;
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
            bteso_IPv4Address clientAddress;
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

#ifdef BSLS_PLATFORM__OS_UNIX
volatile sig_atomic_t globalAlarmCount = 0;

extern "C" void sigalarm(int)
{
   ++globalAlarmCount;
   ::alarm(1);
}
#endif

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    ASSERT(bteso_SocketImpUtil::startup() == 0);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
        case 13: {
            // ----------------------------------------------------------------
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
            // ----------------------------------------------------------------
            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;

            bslma_TestAllocator ta;
{
///Usage
///-----
// In this section we show intended usage of this component
//
///Example 1: Create a New Stream Socket
///- - - - - - - - - - - - - - - - - - -
// We can use 'bteso_InetStreamSocketFactory' to allocate a new TCP-based
// stream socket.
//
// First, we create a 'bteso_InetStreamSocketFactory' object:
//..
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
//..
// Then, we create a stream socket:
//..
    bteso_StreamSocket<bteso_IPv4Address> *mySocket = factory.allocate();
    ASSERT(mySocket);
//..
// 'mySocket' can now be used for TCP communication.
//
// Finally, when we're done, we recycle the socket:
//..
    factory.deallocate(mySocket);
//..
}

{
///Example 2: Create a 'bteso_StreamSocket' Object From Existing Socket Handle
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use 'bteso_InetStreamSocketFactory' to allocate a
// 'bteso_StreamSocket' object that attaches to an existing socket handle.
// This socket handle may be created from a third-party library (such as
// OpenSSL).  Using a 'bteso_StreamSocket' object rather than the socket handle
// directly is highly desirable as it enables the use of other BTE components
// on the socket.  In this example, the socket handle is created from the
// 'bteso_socketimputil' component for illustrative purpose.
//
// First, we create a socket handle 'fd':
//..
    bteso_SocketHandle::Handle fd;
    int nativeErrNo = 0;
    bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                      &fd,
                                      bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
                                      &nativeErrNo);
    ASSERT(0 == nativeErrNo);
//..
// Then, we create factory:
//..
    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&ta);
//..
// Next, we allocate a stream socket attached to 'fd':
//..
    bteso_StreamSocket<bteso_IPv4Address> *mySocket = factory.allocate(fd);
    ASSERT(mySocket);
//..
// Notice that 'fd' is passed into the 'allocate' method as an argument.  Any
// BTE component that uses 'bteso_StreamSocket<bteso_IPv4Address>' can now be
// used on 'mySocket'.
//
// Finally, when we're done, we recycle the socket:
//..
    factory.deallocate(mySocket);
//..
}
        }break;
        case 12:
        // --------------------------------------------------------------------
        // TESTING SETLINGEROPTION
        //
        // Plan:
        //
        // Testing:
        //   setLingerOption()
        // --------------------------------------------------------------------
        {

            if (verbose) cout << endl
                          << "Testing setLingerOption" << endl
                          << "=======================" << endl;

          {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;

               bteso_StreamSocket<bteso_IPv4Address> *testSocket =
                     testFactory.allocate();

               bteso_SocketOptUtil::LingerData lingerResult;
               lingerResult.l_onoff = 2;
               lingerResult.l_linger = 99;
               int resp = testSocket->lingerOption(&lingerResult);
               if (veryVerbose) {
                   P_(lingerResult.l_onoff);
                   P(lingerResult.l_linger);
               }
               ASSERT(resp == 0);
               ASSERT(lingerResult.l_onoff == 0);

               bteso_SocketOptUtil::LingerData ling;
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
          }
        } break;
        case 11:
        // --------------------------------------------------------------------
        // TESTING SETOPTION
        //
        // Plan:
        //
        // Testing:
        //   setOption()
        // --------------------------------------------------------------------
        {

            if (verbose) cout << endl
                          << "Testing setOption" << endl
                          << "=================" << endl;

          {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;

               bteso_StreamSocket<bteso_IPv4Address> *testSocket =
                     testFactory.allocate();

               int result = -1;

               int resp = testSocket->socketOption(
                     &result,
                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                     bteso_SocketOptUtil::BTESO_SENDBUFFER);

               ASSERT(resp == 0);
               ASSERT(result > 0);
               if (veryVerbose) cout << "SENDBUFFER " << result << endl;

               resp = testSocket->socketOption(
                     &result,
                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                     bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);

               ASSERT(resp == 0);
               ASSERT(result > 0);
               if (veryVerbose) cout << "RECEIVEBUFFER " << result << endl;

               resp = testSocket->setOption(
                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                     bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                                32768);

               ASSERT(resp == 0);

               result = -1;
               resp = testSocket->socketOption(
                     &result,
                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                     bteso_SocketOptUtil::BTESO_SENDBUFFER);

               ASSERT(resp == 0);
               ASSERT(result > 0);
               if (veryVerbose) P(result);

               testFactory.deallocate(testSocket);
          }
        } break;

        case 10:
        // --------------------------------------------------------------------
        // TESTING WAITFORCONNECT
        //
        // Plan:
        //   Connect non-blocking to an address known to be invalid.
        //   Connect non-blocking to a local valid port.
        //
        // Testing:
        //   waitForConnect()
        // --------------------------------------------------------------------
        {

            if (verbose) cout << endl
                          << "Testing waitForConnect" << endl
                          << "======================" << endl;

          // Test bind() with "bad" external address.  Non Blocking client
          // This address is a non-routable address and the connection cannot
          // succeed.
          {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_IPv4Address serverAddress;
               serverAddress.setIpAddress("169.254.1.1");
               serverAddress.setPortNumber(1);

               // Create the client socket.

               bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                     testFactory.allocate();

               ASSERT(clientSocket);

               clientSocket->setBlockingMode(
                                           bteso_Flag::BTESO_NONBLOCKING_MODE);

               int resp = clientSocket->connect(serverAddress);
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
               int status = 0;

               // Poll status
               bdet_TimeInterval negtwoseconds = bdetu_SystemTime::now() - 2;
               resp = clientSocket->waitForConnect(negtwoseconds);
               ASSERT(resp == 0
                   || resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);

               for (int x = 0; x < 100; ++x) {

                   bdet_TimeInterval twoseconds = bdetu_SystemTime::now() + 2;

                   if (veryVerbose) { cout << "waitForConnect "; P(resp); }
                   resp = clientSocket->waitForConnect(twoseconds);

                   if (resp == 0) {
                       status = clientSocket->connectionStatus();
                       if (veryVerbose) {
                           cout << "connectionStatus ";P(status);
                       }
                       break;
                   }
                   else {
                       LOOP_ASSERT(resp,
                             resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);
                   }
               }

               resp = clientSocket->waitForConnect(negtwoseconds);
               ASSERT(resp == 0);

               ASSERT(status != 0);

               testFactory.deallocate(clientSocket);

           }
           {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                     testFactory.allocate();
               ASSERT(serverSocket);

               // Bind to localhost with anon port number
               bteso_IPv4Address localAnonPort;
               localAnonPort.setIpAddress("127.0.0.1");
               localAnonPort.setPortNumber(0);

               // bind to a anon port
               int resp = serverSocket->bind(localAnonPort);
               ASSERT(resp == 0);

               bteso_IPv4Address serverAddress;
               resp = serverSocket->localAddress(&serverAddress);
               ASSERT(resp == 0);

               // The address must be the same.
               ASSERT(localAnonPort.ipAddress() == serverAddress.ipAddress());
               // The port number may not be zero.
               ASSERT(serverAddress.portNumber() > 0);

               resp = serverSocket->listen(10);
               ASSERT(resp == 0);

               // Create the client socket.

               bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                     testFactory.allocate();

               ASSERT(clientSocket);

               clientSocket->setBlockingMode(
                                           bteso_Flag::BTESO_NONBLOCKING_MODE);

               resp = clientSocket->connect(serverAddress);
               ASSERT(resp == 0 ||
                      resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

               int status = -1;

               for (int x = 0; x < 100; ++x) {

                   bdet_TimeInterval twoseconds = bdetu_SystemTime::now() + 2;

                   resp = clientSocket->waitForConnect(twoseconds);
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
                                     bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);
                   }
               }

               ASSERT(status == 0);

               // Accept the connection
               bteso_StreamSocket<bteso_IPv4Address> *serviceSocket;
               bteso_IPv4Address clientAddress;

               resp = serverSocket->accept(&serviceSocket, &clientAddress);

               // shutdown service socket
               resp = serviceSocket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

               status = clientSocket->connectionStatus();
               ASSERT(status == 0);
               if (veryVerbose) P(status);
               testFactory.deallocate(serviceSocket);

               testFactory.deallocate(serverSocket);
               testFactory.deallocate(clientSocket);

           }
      } break;

        case 9:
        // --------------------------------------------------------------------
        // TESTING WAITFORACCEPT
        //
        // Plan:
        //   For blocking listening socket:
        //      no pending incoming connection
        //      pending incoming connection
        //      pending incoming connection closed before wait
        //      pending incoming connection reset before wait (not impl***
        //
        // Testing:
        //   waitForAccept()
        // --------------------------------------------------------------------
        {

            if (verbose) cout << endl
                          << "Testing waitForAccept" << endl
                          << "=====================" << endl;

           {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                     testFactory.allocate();
               ASSERT(serverSocket);

               // Bind to localhost with anon port number
               bteso_IPv4Address localAnonPort;
               localAnonPort.setIpAddress("127.0.0.1");
               localAnonPort.setPortNumber(0);

               // bind to a anon port
               int resp = serverSocket->bind(localAnonPort);
               ASSERT(resp == 0);

               bteso_IPv4Address serverAddress;
               resp = serverSocket->localAddress(&serverAddress);
               ASSERT(resp == 0);

               // The address must be the same.  The port number may not
               // be zero.
               ASSERT(localAnonPort.ipAddress() == serverAddress.ipAddress());
               ASSERT(serverAddress.portNumber() > 0);

               resp = serverSocket->listen(10);
               ASSERT(resp == 0);

               // Create the client socket.

               bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                     testFactory.allocate();
               ASSERT(clientSocket);

               bdet_TimeInterval timeBefore = bdetu_SystemTime::now();
               bdet_TimeInterval timeout = timeBefore + 2;

               // There is no pending incoming connection.  Expect timeout.
               resp = serverSocket->waitForAccept(timeout);
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);
               if (veryVerbose) { P(resp); };

               resp = clientSocket->connect(serverAddress);
               ASSERT(resp == 0);
               if (veryVerbose) { P(resp); };

               bdet_TimeInterval timeAfter;
               timeBefore = bdetu_SystemTime::now();
               timeout = timeBefore + 2;

               // There is a pending connection so expect success.
               resp = serverSocket->waitForAccept(timeout);
               ASSERT(resp == 0);
               if (veryVerbose) { P(resp); };

               timeBefore = bdetu_SystemTime::now();
               timeout = timeBefore + 2;

               // There is a pending connection so expect success.
               resp = serverSocket->waitForAccept(timeout);
               ASSERT(resp == 0);
               if (veryVerbose) { P(resp); };

               // Accept the connection
               bteso_StreamSocket<bteso_IPv4Address> *serviceSocket;
               bteso_IPv4Address clientAddress;

               resp = serverSocket->accept(&serviceSocket, &clientAddress);
               ASSERT(resp == 0);
               if (veryVerbose) { P(resp); };

               timeBefore = bdetu_SystemTime::now();
               timeout = timeBefore + 2;

               // There is no pending incoming connection.  Expect timeout.
               resp = serverSocket->waitForAccept(timeout);
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);

               // If a timeout occurred, time must be > (timeout - 10ms)
               // and within 5 seconds
               timeAfter = bdetu_SystemTime::now();
               ASSERT((timeAfter + 0.01) > timeout);
               ASSERT(timeAfter < (timeout + 5));

               timeBefore = bdetu_SystemTime::now();
               timeout = timeBefore - 0.02;
               // Timeout is in the past (>= 20ms).  Don't wait just return
               // status.

               resp = serverSocket->waitForAccept(timeout);
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT);

               // If a timeout occurred, time must be > (timeout - 10ms)
               // and within 5 seconds
               timeAfter = bdetu_SystemTime::now();
               ASSERT((timeAfter + 0.01) > timeout);
               ASSERT((timeAfter + 0.01) > timeBefore);
               ASSERT(timeAfter < (timeout + (5 + 0.02)));

               testFactory.deallocate(serviceSocket);

               testFactory.deallocate(serverSocket);
               testFactory.deallocate(clientSocket);

          }

        } break;
        case 8:
        // --------------------------------------------------------------------
        // TESTING READ
        //
        // Plan:
        //    test read  normal
        //    test readv normal
        //
        // Testing:
        //   read()
        //   readv()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing read and readv" << endl
                          << "========================" << endl;

        {
         {
             if (veryVerbose) cout
              << "\n   test read normal data"
              << endl;

               // Create a server socket

               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
               int resp = createConnectedStreamSockets(&testFactory,
                                 &streamSocketA, &streamSocketB);

               ASSERT(resp);

               // Fill the outgoing socket buffer for socket A
               char buf1[] = { 10, 9, 88, 6, 2, 10, 0, 51, 7, 99 };

               char rcvbuf1[100];
               char rcvbuf2[100];

               resp = streamSocketA->write(buf1, sizeof(buf1));
               ASSERT(resp == sizeof(buf1));

               // Latency
               bcemt_ThreadUtil::microSleep(500 * 1000);

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
             if (veryVerbose) cout
              << "\n   test readv normal data"
              << endl;

               // Create a server socket

               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
               int resp = createConnectedStreamSockets(&testFactory,
                                 &streamSocketA, &streamSocketB);

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
               bcemt_ThreadUtil::microSleep(500 * 1000);

               btes_Iovec vec[2];
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
               resp = streamSocketA->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

               // First receive what has been sent.
               vec[0].setBuffer(rcvbuf1, 10);
               vec[1].setBuffer(rcvbuf2, 10);

               resp = streamSocketB->readv(vec, 2);

               ASSERT(resp == (totalSent - 6 - 2));

               // EOF is expected next.
               vec[0].setBuffer(rcvbuf1, 1);
               vec[1].setBuffer(rcvbuf2, 5);

               resp = streamSocketB->readv(vec, 2);
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_EOF);

             testFactory.deallocate(streamSocketA);
             testFactory.deallocate(streamSocketB);
         }
      } break;
      case 7:
        // --------------------------------------------------------------------
        // TESTING WRITE
        //
        // Plan:
        //    test write
        //    test writev
        //    test response bteso_SocketHandle::BTESO_ERROR_INTERRUPTED (UNIX
        //                                                               only)
        //    test response bteso_SocketHandle::BTESO_ERROR_CONNDEAD
        //    test response bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK
        //
        // Testing:
        //   write()
        //   writev()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing write and writev" << endl
                          << "========================" << endl;

        {
         {
             if (veryVerbose) cout
              << "\n   test write normal data"
              << endl;

               // Create a server socket

               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
               int resp = createConnectedStreamSockets(&testFactory,
                                 &streamSocketA, &streamSocketB);

               ASSERT(resp);

               // Fill the outgoing socket buffer for socket A
               char buf1[] = { 10, 9, 8, 6, 2, 10 };
               char buf2[] = { 100, 5, 15, 0, 1 };

               char rcvbuf[100];

               resp = streamSocketA->write(buf1, sizeof(buf1));
               ASSERT(resp == sizeof(buf1));

               resp = streamSocketA->write(buf2, sizeof(buf2));
               ASSERT(resp == sizeof(buf2));

               bcemt_ThreadUtil::microSleep(500 * 1000);

               resp = streamSocketB->read(rcvbuf, sizeof(rcvbuf));
               ASSERT(resp == (sizeof(buf1) + sizeof(buf2)));

               ASSERT(memcmp(buf1, rcvbuf, sizeof(buf1)) == 0);
               ASSERT(memcmp(buf2, rcvbuf + sizeof(buf1), sizeof(buf2)) == 0);

             testFactory.deallocate(streamSocketA);
             testFactory.deallocate(streamSocketB);
         }
         {
             if (veryVerbose) cout
              << "\n   test writev normal data"
              << endl;

               // Create a server socket

               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
               int resp = createConnectedStreamSockets(&testFactory,
                                 &streamSocketA, &streamSocketB);

               ASSERT(resp);

               // Fill the outgoing socket buffer for socket A
               char buf1[] = { 10, 9, 8, 6, 2, 10 };
               char buf2[] = { 100, 5, 15, 0, 1 };
               char buf3[] = { 1 };

               char rcvbuf[100];

               btes_Ovec vec[2];
               vec[0].setBuffer(buf1, sizeof(buf1));
               vec[1].setBuffer(buf2, sizeof(buf2));
               resp = streamSocketA->writev(vec, 2);
               ASSERT(resp == sizeof(buf1) + sizeof(buf2));
               if (veryVerbose) P(resp);

               btes_Ovec vec2[1];
               vec2[0].setBuffer(buf3, sizeof(buf3));
               resp = streamSocketA->writev(vec2, 1);
               ASSERT(resp == sizeof(buf3));
               if (veryVerbose) P(resp);

               bcemt_ThreadUtil::microSleep(500 * 1000);

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
#if defined(BSLS_PLATFORM__OS_UNIX) && !defined(BSLS_PLATFORM__OS_CYGWIN)
         {
         if (veryVerbose) cout
          << "\n   test write response ERROR_INTERRUPTED (UNIX only)"
          << endl;

         int packetSize;

         for (packetSize = 500; packetSize < 25000; packetSize += 2001) {
           // Create a server socket
           if (veryVerbose) P(packetSize);

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           // Fill the outgoing socket buffer for socket A
           char zerobuf[1024*64];
           memset(zerobuf, 0, sizeof zerobuf);

           globalAlarmCount = 0;

           // Set a one second timeout.  A blocking system call such as a
           // write will fail when this timeout occurs.  The signal handler
           // will restart the timer.
           struct sigaction oact, act;
           act.sa_handler = sigalarm;
           sigemptyset(&act.sa_mask);
           act.sa_flags = 0;
           ::sigaction(SIGALRM, &act, &oact);
           ::alarm(1);

           int totalSent = 0;

           // Keep going until write fails for interrupted call.
           int i = 0;
           while (1) {
               resp = streamSocketA->write(zerobuf, packetSize);
               if (resp <= 0) break;
               ASSERT(resp <= packetSize);

               if ((veryVerbose && resp < packetSize) || veryVeryVerbose) {
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

           ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);

           testFactory.deallocate(streamSocketA);
           testFactory.deallocate(streamSocketB);
         }
         }

         {
         if (veryVerbose) cout
          << "\n   test writev response ERROR_INTERRUPTED (UNIX only)"
          << endl;

         int packetSize;

         for (packetSize = 500; packetSize < 25000; packetSize += 2001) {
           // Create a server socket
           if (veryVerbose) P(packetSize);

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           // Fill the outgoing socket buffer for socket A
           char zerobuf[1024*64];
           memset(zerobuf, 0, sizeof zerobuf);

           globalAlarmCount = 0;

           // Set a one second timeout.  A blocking system call such as a
           // write will fail when this timeout occurs.  The signal handler
           // will restart the timer.
           struct sigaction oact, act;
           act.sa_handler = sigalarm;
           sigemptyset(&act.sa_mask);
           act.sa_flags = 0;
           ::sigaction(SIGALRM, &act, &oact);
           ::alarm(1);

           int totalSent = 0;

           // Keep going until writev fails for interrupted call.
           int i = 0;
           while (1) {
               btes_Ovec vec[2];
               vec[0].setBuffer(zerobuf, packetSize);
               vec[1].setBuffer(zerobuf, 2);
               resp = streamSocketA->writev(vec, 2);
               if (resp <= 0) break;
               ASSERT(resp <= (packetSize + 2));

               if ((veryVerbose && resp < packetSize) || veryVeryVerbose) {
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

           ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED);

           testFactory.deallocate(streamSocketA);
           testFactory.deallocate(streamSocketB);
         }
         }
#endif

         {
#ifdef BSLS_PLATFORM__OS_UNIX
               // Under UNIX a write to a closed blocking socket causes the
               // signal SIGPIPE which by default will terminate the process.
               // Set the signal to be ignored.  This behavior does not
               // exist under Windows.
               ::signal(SIGPIPE, SIG_IGN );
#endif
         }

         if (veryVerbose) cout
         << "\n   test write response bteso_SocketHandle::BTESO_ERROR_CONNDEAD"
         << endl;

         // Verify error response when remote socket closed.
         // Try the write with different sizes because the behavior
         // of underlying sockets vary.  We don't try larger than
         // 32K per write because some platforms have a limit.
         for (int tolerance = 3, size = 1; size < 32; ++size)
         {
           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           char zerobuf[1024*32] = {0};

           testFactory.deallocate(streamSocketB);

           bcemt_ThreadUtil::microSleep(200 * 1000);

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
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_CONNDEAD);
           }
           testFactory.deallocate(streamSocketA);
         }

         if (veryVerbose) cout
        << "\n   test writev response bteso_SocketHandle::BTESO_ERROR_CONNDEAD"
        << endl;

         // Verify error response when remote socket closed.
         // Try the write with different sizes because the behavior
         // of underlying sockets vary.  We don't try larger than
         // 32K per write because some platforms have a limit.
         for (int tolerance = 3, size = 1; size < 32; ++size)
         {
           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           char zerobuf[1024*32] = {0};

           testFactory.deallocate(streamSocketB);

           bcemt_ThreadUtil::microSleep(200 * 1000);

           // The first write to the socket after the remote socket
           // has been closed may succeed.  Eventually the write will
           // fail (normally after the first or second write).

           for (int i = 0; i < 10; ++i) {
               btes_Ovec vec[2];
               vec[0].setBuffer(zerobuf, 1024 * size);
               vec[1].setBuffer(zerobuf, 2);
               resp = streamSocketA->writev(vec, 2);
               if (resp < 0) break;
               ASSERT(resp <= (1024 * size + 2));
           }

           if (veryVerbose) { P_(resp); P(tolerance); }
           if (resp > 0 && tolerance-- <= 0) {
               ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_CONNDEAD);
           }
           testFactory.deallocate(streamSocketA);
         }

         {
         if (veryVerbose) cout
       << "\n   test write response bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK"
       << endl;

         int packetSize;

         for (packetSize = 500; packetSize < 25000; packetSize += 2001) {
           // Create a server socket
           if (veryVerbose) P(packetSize);

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           streamSocketA->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

           streamSocketB->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

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
               if ((veryVerbose && resp < packetSize) || veryVeryVerbose) {
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
                       resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

           // Verify that when receiver takes all the data, sender
           // can send again.

           char receiveBuf[sizeof(zerobuf)];

           int totalReceived = 0;
           int iterations = 0;

           while (1) {
               resp = streamSocketB->read(receiveBuf, sizeof(receiveBuf));
               if (resp <= 0) {
                   if (totalReceived < totalSent && iterations < 30) {
                       if (veryVerbose) {
                           P_(iterations); P_(totalReceived); P(totalSent);
                       }
                       bcemt_ThreadUtil::microSleep(100 * 1000);
                       iterations++;
                   }
                   else break;
               }
               else {
                   totalReceived += resp;
                   LOOP_ASSERT(packetSize, resp <= (int)sizeof(receiveBuf));
               }
           }

           LOOP_ASSERT(packetSize, totalReceived == totalSent);
           LOOP_ASSERT(packetSize,
                       resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

           // Latency

           bcemt_ThreadUtil::microSleep(1000 * 1000);

           // Write data and verify that it writes OK

           resp = streamSocketA->write(zerobuf, packetSize);

           if (veryVerbose) P(resp);
           LOOP_ASSERT(packetSize, resp > 0 && resp <= packetSize);

           testFactory.deallocate(streamSocketA);
           testFactory.deallocate(streamSocketB);
         }
         }

         {
         if (veryVerbose) cout
      << "\n   test writev response bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK"
      << endl;

         int packetSize;

         for (packetSize = 500; packetSize < 25000; packetSize += 2001) {
           // Create a server socket
           if (veryVerbose) P(packetSize);

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           ASSERT(resp);

           streamSocketA->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

           streamSocketB->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

           // Fill the outgoing socket buffer for socket A
           char zerobuf[1024*64];
           memset(zerobuf, 0, sizeof zerobuf);

           int totalSent = 0;

           // Keep going until write fails for "would block"
           // Note that this test does not require that both the receive
           // and send buffers be filled.
           int i = 0;
           while (1) {
               btes_Ovec vec[2];
               vec[0].setBuffer(zerobuf, packetSize);
               vec[1].setBuffer(zerobuf, 2);
               resp = streamSocketA->writev(vec, 2);
               if (resp <= 0) break;

               ASSERT(resp <= (packetSize + 2));
               if ((veryVerbose && resp < (packetSize + 2)) || veryVeryVerbose)
               {
                   P_(i); P(resp);
               }
               totalSent += resp;
               ++i;
           }

           if (veryVerbose) P(i);
           if (veryVerbose) cout << "Total Sent " << totalSent << endl;

           ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

           // Verify that when receiver takes all the data, sender
           // can send again.

           char receiveBuf[sizeof(zerobuf)];
           int totalReceived = 0;
           int iterations = 0;

           while (1) {
               resp = streamSocketB->read(receiveBuf, sizeof(receiveBuf));
               if (resp <= 0) {
                   if (totalReceived < totalSent && iterations < 30) {
                       if (veryVerbose) {
                           P_(iterations); P_(totalReceived); P(totalSent);
                       }
                       bcemt_ThreadUtil::microSleep(100 * 1000);
                       iterations++;
                   }
                   else break;
               }
               else {
                   totalReceived += resp;
                   LOOP_ASSERT(packetSize, resp <= (int)sizeof(receiveBuf));
               }
           }

           LOOP_ASSERT(packetSize, totalReceived == totalSent);
           LOOP_ASSERT(packetSize,
                       resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

           // Latency

           bcemt_ThreadUtil::microSleep(1000 * 1000);

           // Write data and verify that it writes OK

           btes_Ovec vec[2];
           vec[0].setBuffer(zerobuf, packetSize);
           vec[1].setBuffer(zerobuf, 2);
           resp = streamSocketA->writev(vec, 2);

           if (veryVerbose) P(resp);
           ASSERT(resp > 0 && resp <= (packetSize + 2));

#ifndef BSLS_PLATFORM__OS_CYGWIN
           testFactory.deallocate(streamSocketA);
           testFactory.deallocate(streamSocketB);
#else
           // Cygwin is doing something funky in its close() function, which
           // force us to close the client socket first.

           testFactory.deallocate(streamSocketB);
           testFactory.deallocate(streamSocketA);
#endif
         }
         }
      } break;
      case 6:
        // --------------------------------------------------------------------
        // TESTING WAITFORIO
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
        //   waitForIO()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing waitForIO" << endl
                          << "=================" << endl;

        {
// TBD FIX ME
#ifndef BSLS_PLATFORM__OS_AIX
           const bteso_Flag::IOWaitType RD = bteso_Flag::BTESO_IO_READ;
           const bteso_Flag::IOWaitType WR = bteso_Flag::BTESO_IO_WRITE;
           const bteso_Flag::IOWaitType RW = bteso_Flag::BTESO_IO_RW;
           const int                    TO = bteso_SocketHandle::
                                                          BTESO_ERROR_TIMEDOUT;

           typedef bteso_Flag S;
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
                { L_,  RD,    0,     0,    75,     0,  2000,         0, TO },
                { L_,  WR,    0,     0,    75,     0,  2000,         0, WR },
                { L_,  RW,    0,     0,    75,     0,  2000,         0, WR },
                { L_,  RD,    0,     0,    75,  1000,  2000,         0, RD },
                { L_,  WR,    0,     0,    75,  1000,  2000,         0, WR },
                { L_,  RW,    0,     0,    75,  1000,  2000,         0, RW },
                { L_,  RD,    0,     0,     0,  1000,     0,         0, RD },
                { L_,  WR,    0,     0,     0,  1000,  2000,         0, TO },
                { L_,  RW,    0,     0,     0,  1000,  2000,         0, RD },
                { L_,  RD,    0,  2000,     0,     0,     0,         0, TO },
                { L_,  WR,    0,  2000,     0,     0,     0,         0, TO },
                { L_,  RW,    0,  2000,     0,     0,     0,         0, TO },
                { L_,  RD,    0,  2000,    75,     0,  2000,         0, TO },
                { L_,  WR,    0,  2000,    75,     0,  2000,         0, WR },
                { L_,  RW,    0,  2000,    75,     0,  2000,         0, WR },
                { L_,  RD,    0,  2000,    75,  1000,  2000,         0, RD },
                { L_,  WR,    0,  2000,    75,  1000,  2000,         0, WR },
                { L_,  RW,    0,  2000,    75,  1000,  2000,         0, RW },
                { L_,  RD,    0,  2000,     0,  1000,     0,         0, RD },
                { L_,  WR,    0,  2000,     0,  1000,     0,         0, TO },
                { L_,  RW,    0,  2000,     0,  1000,     0,         0, RD },
                { L_,  RD,   10,     0,     0,     0,     0,         0, TO },
                { L_,  WR,   10,     0,     0,     0,     0,         0, TO },
                { L_,  RW,   10,     0,     0,     0,     0,         0, TO },
                { L_,  RD,   10,     0,    75,     0,     0,         0, TO },
                { L_,  WR,   10,     0,    75,     0,     0,         0, WR },
                { L_,  RW,   10,     0,    75,     0,     0,         0, WR },
                { L_,  RD,   10,     0,    75,  1000,  2000,         0, RD },
                { L_,  WR,   10,     0,    75,  1000,  2000,         0, WR },
                { L_,  RW,   10,     0,    75,  1000,  2000,         0, RW },
                { L_,  RD,   10,     0,     0,  1000,     0,         0, RD },
                { L_,  WR,   10,     0,     0,  1000,     0,         0, TO },
                { L_,  RW,   10,     0,     0,  1000,     0,         0, RD },
                { L_,  RD,    0,     0,     0,     0,     0,         1, TO },
                { L_,  WR,    0,     0,     0,     0,     0,         1, TO },
                { L_,  RW,    0,     0,     0,     0,     0,         1, TO },
                { L_,  RD,    0,     0,    75,     0,   500,         1, TO },
                { L_,  WR,    0,     0,    75,     0,   500,         1, WR },
                { L_,  RW,    0,     0,    75,     0,   500,         1, WR },
           };

           enum {
               NUM_DATA      = sizeof DATA / sizeof *DATA,

               DATASIZE      = 10000,    // total amount of initial data
               PACKETSIZE    = 2000      // read/write packet size
           };

           for (int ti = 0; ti < NUM_DATA; ++ti) {

           if (veryVerbose) {
               T_(); T_(); P_(ti);
                           P(DATA[ti].d_lineNum);
           }
           // Create a stream socket pair
           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

           LOOP_ASSERT(ti, resp);
           if (resp == 0) {
               // could not create connected socket pair
               break;
           }

           // Turn off Nagle for sending side (A) if specified
           streamSocketA->setOption(bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                    bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                    DATA[ti].d_tcpndelay);

           streamSocketA->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

           // Fill the output buffer for socket A.
           char zerobuf[DATASIZE] = {0};
           int  totalSent = 0;

           // Attempt to fill the output buffer of A by writing until we
           // receive an ERROR_WOULDBLOCK response.  Then, try several times
           // more to ensure that ERROR_WOULDBLOCK is persistent, and that
           // the write buffer is *really* full.

           for (int attempt = 0; attempt < 10; ++attempt ) {
               bdet_TimeInterval before = bdetu_SystemTime::now();
               while (1) {
                   resp = streamSocketA->write(zerobuf, DATASIZE);
                   if (resp <= 0) break;
                   totalSent += resp;
               }

               bcemt_ThreadUtil::microSleep(50 * 1000);
               bdet_TimeInterval interval = bdetu_SystemTime::now() - before;

               if (veryVeryVerbose) {
                   P_(attempt); P_(resp); P_(totalSent); P(interval);
               }

               LOOP_ASSERT(ti, resp ==
                                   bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);
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
               int count = leftToWrite > PACKETSIZE ? PACKETSIZE : leftToWrite;
               if (veryVeryVerbose) { P_(count); P(leftToWrite); }
               streamSocketB->write(buf, count);
               leftToWrite -= count;
           }

           // delay before testing for IO to allow for latency.
           if (DATA[ti].d_delay) {
               if (veryVerbose) {
                   P(DATA[ti].d_delay);
               }
               bcemt_ThreadUtil::microSleep(DATA[ti].d_delay * 1000);
           }

#if defined(BSLS_PLATFORM__OS_HPUX)  // TBD
// Some TCP driver implementations require some delay between write and
// read (on the loopback service) in order to recognize I/O events correctly.
           bcemt_ThreadUtil::microSleep(20 * 1000);
#endif

           bdet_TimeInterval timeBefore = bdetu_SystemTime::now();

           bdet_TimeInterval timeout = timeBefore;
           timeout.addSeconds(DATA[ti].d_seconds);
           timeout.addMicroseconds(DATA[ti].d_microseconds);
           resp = streamSocketA->waitForIO(DATA[ti].d_io, timeout);

           bdet_TimeInterval timeAfter = bdetu_SystemTime::now();

           if (veryVerbose) { P_(resp); P(DATA[ti].d_expected); }

           // Assuming the latency and system responsiveness is
           // never worse than 5 seconds.

           if (resp == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT)
           {
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

           LOOP2_ASSERT(ti, DATA[ti].d_lineNum, resp == DATA[ti].d_expected);

#ifndef BSLS_PLATFORM__OS_CYGWIN
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
      case 5:
      {
        // --------------------------------------------------------------------
        // TESTING SOCKET CREATION FUNCTION
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

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketA = 0;
           bteso_StreamSocket<bteso_IPv4Address> *streamSocketB = 0;
           int resp = createConnectedStreamSockets(&testFactory,
                             &streamSocketA, &streamSocketB);

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

      case 4: {
        // --------------------------------------------------------------------
        // TESTING BREATHING BIND ACCEPT CONNECT
        //
        // Plan:
        //
        // Testing:
        //   localAddress()
        //   peerAddress()
        //   bind()  BREATHING
        //   accept()  BREATHING
        //   connect()  BREATHING
        //   listen()  BREATHING
        //   shutdown() BREATHING
        //   setBlockingMode() BREATHING
        //   blockingMode() BREATHING
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING Breathing bind/accept/connect/listen" << endl
                 << "============================================" << endl;

        // Counts number of sockets currently open (on certain platforms
        // only).
        int initialSockets =
            countSockets(MIN_SOCKET_HANDLE, MAX_SOCKET_HANDLE);
        bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;

        // Test bind() with bad port number < 1024 (we are not root)
        // On windows, this is allowed.
#ifdef BSLS_PLATFORM__OS_UNIX
        if (verbose)
            cout << "\tBinding to a bad port number." << endl;
        {
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with a low port number

            bteso_IPv4Address localLowPort;
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
#ifdef BSLS_PLATFORM__OS_UNIX
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
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with anon port number

            bteso_IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localLowPort);
            ASSERT(resp == 0);

            bteso_IPv4Address serverAddress;
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

            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            if (verbose)
                cout << "\tBlocking connect to " << serverAddress << endl;
            resp = clientSocket->connect(serverAddress);
            ASSERT(resp == 0);

            // Accept the connection
            bteso_StreamSocket<bteso_IPv4Address> *serviceSocket;
            bteso_IPv4Address clientAddress;

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
            bcemt_ThreadUtil::microSleep(2000 * 1000);

            resp = serviceSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1) + sizeof(buf2));
            ASSERT(memcmp(bufrcv, buf2, sizeof(buf2)) == 0);
            ASSERT(memcmp(bufrcv + sizeof(buf2), buf1, sizeof(buf1)) == 0);

            // Verify shutdown()

            // Shutdown of receive is not universally supported - it may be
            // ignored.  'both' is always supported but the implementation
            // may not shutdown receive.

            resp = serviceSocket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);
            ASSERT(resp == 0);

            resp = serviceSocket->write(buf1, sizeof(buf1));

            resp = clientSocket->read(bufrcv, sizeof(buf1));

            ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_EOF);

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
            bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                testFactory.allocate();

            ASSERT(serverSocket);

            // Bind to localhost with anon port number

            bteso_IPv4Address localLowPort;
            localLowPort.setIpAddress("127.0.0.1");
            localLowPort.setPortNumber(0);

            // bind to a anon port
            int resp = serverSocket->bind(localLowPort);
            ASSERT(resp == 0);

            bteso_IPv4Address serverAddress;
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

            bteso_StreamSocket<bteso_IPv4Address> *clientSocket =
                testFactory.allocate();

            ASSERT(clientSocket);

            bteso_Flag::BlockingMode result =
                                            bteso_Flag::BTESO_NONBLOCKING_MODE;

            resp = clientSocket->blockingMode(&result);
            if (resp == 0) {
                // If this platform supports this feature, verify result.
                ASSERT(result == bteso_Flag::BTESO_BLOCKING_MODE);
            }

            clientSocket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

            resp = clientSocket->blockingMode(&result);
            if (resp == 0) {
                // If this platform supports this feature, verify result.
                ASSERT(result == bteso_Flag::BTESO_NONBLOCKING_MODE);
            }

            // In non-blocking mode, some platforms will return 0
            // and others return bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK
            if (verbose)
                cout << "\tNon-blocking connect to " << serverAddress << endl;
            resp = clientSocket->connect(serverAddress);
            ASSERT(resp == 0 ||
                   resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

            // Accept the connection
            bteso_StreamSocket<bteso_IPv4Address> *serviceSocket;
            bteso_IPv4Address clientAddress;

            resp = serverSocket->accept(&serviceSocket, &clientAddress);
            ASSERT(resp == 0);

            bteso_IPv4Address servicePeerAddress;
            resp = serviceSocket->peerAddress(&servicePeerAddress);
            ASSERT(resp == 0);
            ASSERT(servicePeerAddress == clientAddress);

            bteso_IPv4Address clientLocalAddress;
            resp = clientSocket->localAddress(&clientLocalAddress);
            ASSERT(servicePeerAddress == clientLocalAddress);

            bteso_IPv4Address clientPeerAddress;
            resp = clientSocket->peerAddress(&clientPeerAddress);
            ASSERT(resp == 0);

            bteso_IPv4Address serviceLocalAddress;
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
            ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK);

            resp = serviceSocket->write(buf1, sizeof(buf1));
            ASSERT(resp == sizeof(buf1));

#if defined(BSLS_PLATFORM__OS_HPUX) || \
    defined(BSLS_PLATFORM__OS_WINDOWS) // TBD
// Some TCP driver implementations require some delay between write and
// read (on the loopback service) in order to recognize I/O events correctly.
            bcemt_ThreadUtil::microSleep(20 * 1000);
#endif

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
            bcemt_ThreadUtil::microSleep(2000 * 1000);

            resp = serviceSocket->read(bufrcv, sizeof(bufrcv));
            ASSERT(resp == sizeof(buf1) + sizeof(buf2));
            ASSERT(memcmp(bufrcv, buf2, sizeof(buf2)) == 0);
            ASSERT(memcmp(bufrcv + sizeof(buf2), buf1, sizeof(buf1)) == 0);

            // Verify shutdown()

            resp = serviceSocket->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);
            ASSERT(resp == 0);

            resp = serviceSocket->write(buf1, sizeof(buf1));
            ASSERT(resp < 0);

#if defined(BSLS_PLATFORM__OS_HPUX) || \
    defined(BSLS_PLATFORM__OS_WINDOWS) // TBD
// Some TCP driver implementations require some delay between write and
// read (on the loopback service) in order to recognize I/O events correctly.
            bcemt_ThreadUtil::microSleep(20 * 1000);
#endif

            resp = clientSocket->read(bufrcv, sizeof(buf1));
            ASSERT(bteso_SocketHandle::BTESO_ERROR_EOF == resp ||
                   bteso_SocketHandle::BTESO_ERROR_CONNDEAD == resp);

            testFactory.deallocate(serviceSocket);
            testFactory.deallocate(serverSocket);
            testFactory.deallocate(clientSocket);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

           // Test accept when the connecting socket (client) closes before
           // the accept is performed.
           {
               bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketA;
               bteso_StreamSocket<bteso_IPv4Address> *streamSocketB;
               streamSocketA = 0;
               streamSocketB = 0;
               bteso_StreamSocket<bteso_IPv4Address> *serverSocket =
                 testFactory.allocate();

               ASSERT(serverSocket);

               // Bind to localhost with anon port number

               bteso_IPv4Address localLowPort;
               localLowPort.setIpAddress("127.0.0.1");
               localLowPort.setPortNumber(0);

               // bind to a anon port
               int resp = serverSocket->bind(localLowPort);
               ASSERT(resp == 0);

               bteso_IPv4Address serverAddress;
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
                   bteso_IPv4Address clientAddress;

                   // Generate a "rst" (linger time = 0, followed by shutdown)
                   bteso_SocketOptUtil::LingerData ling;
                   ling.l_onoff = 1;
                   ling.l_linger = 0;
                   (streamSocketA)->setLingerOption(ling);

                   // kill the connection before accept
                   (streamSocketA)->shutdown(bteso_Flag::BTESO_SHUTDOWN_BOTH);

                   // close connection  after rst
                   testFactory.deallocate(streamSocketA);

                   // Wait for 2 seconds
                   bcemt_ThreadUtil::microSleep(2000 * 1000);

#ifdef BSLS_PLATFORM__OS_UNIX
                   ::signal(SIGALRM, sigalarm);
                   ::alarm(5);
#endif

                   resp = serverSocket->accept(&streamSocketB, &clientAddress);

#ifdef BSLS_PLATFORM__OS_UNIX
                   // Cancel the alarm
                   ::signal(SIGALRM, SIG_IGN);
                   ::alarm(0);
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
                   if (resp != 0) {
                       ASSERT(bteso_SocketHandle::BTESO_ERROR_CONNDEAD == resp
                       || bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == resp);
                       testFactory.deallocate(streamSocketB);
                   }
                   else {
                       char buf[1];
                       resp = streamSocketB->read(buf,1 );
                       ASSERT(bteso_SocketHandle::BTESO_ERROR_CONNDEAD == resp
                           || bteso_SocketHandle::BTESO_ERROR_EOF == resp);
                       testFactory.deallocate(streamSocketB);
                   }
               } else {
                   testFactory.deallocate(serverSocket);
               }
          }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING FACTORY ALLOCATE WITH HANDLE
        //   We have the following concerns:
        //   Created bteso_StreamSocket behaves correctly and is associated
        //   with the correct socket handle.
        //
        // Plan:
        //   Create a pair of sockets using bteso_SocketImpUtil::socketPair().
        //   Create stream sockets using the handles.
        //   Verify that the stream sockets are valid:
        //   1) use handle() accessor to verify handle.
        //   2) send data and receive data
        //   3) deallocate stream socket and verify socket is no longer open.
        //
        // Testing:
        //   bteso_StreamSocket<ADDRESS> *allocate(handle);
        //   read() BREATHING
        //   write() BREATHING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Factory allocate(handle)" << endl
                          << "================================" << endl;

           bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;
           bteso_SocketHandle::Handle handles[2];

           int resp = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                  handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
           ASSERT(resp == 0);
           bteso_StreamSocket<bteso_IPv4Address> *newSocketA =
                 testFactory.allocate(handles[0]);
           bteso_StreamSocket<bteso_IPv4Address> *newSocketB =
                 testFactory.allocate(handles[1]);

           ASSERT(newSocketA);
           ASSERT(newSocketB);
           ASSERT(newSocketA != newSocketB);

           bteso_SocketHandle::Handle newHandleA = newSocketA->handle();
           bteso_SocketHandle::Handle newHandleB = newSocketB->handle();
           ASSERT(verifyHandle(newHandleA) != 0);
           ASSERT(verifyHandle(newHandleB) != 0);

           ASSERT(handles[0] == newHandleA);
           ASSERT(handles[1] == newHandleB);

           // Send data via the streamsockets.
           const char buf1[] = { 10, 9, 1, 7, 3 };
           char bufrcv[10];
           resp = newSocketA->write(buf1, sizeof(buf1));
           ASSERT(resp == sizeof(buf1));

           // Verify that the data was received as sent.
           resp = newSocketB->read(bufrcv, sizeof(bufrcv));
           ASSERT(resp == sizeof(buf1));

           ASSERT(memcmp(bufrcv, buf1, sizeof(buf1)) == 0);

           testFactory.deallocate(newSocketA);

           resp = newSocketB->read(bufrcv, sizeof(bufrcv));

           // socket A is closed so expect EOF
           ASSERT(resp == bteso_SocketHandle::BTESO_ERROR_EOF);

           testFactory.deallocate(newSocketB);

           ASSERT(verifyHandle(newHandleA) == 0);
           ASSERT(verifyHandle(newHandleB) == 0);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FACTORY ALLOCATE WITHOUT HANDLE
        //   We have the following concerns:
        //
        // Plan:
        //
        // Testing:
        //   bteso_StreamSocket<ADDRESS> *allocate();
        //   deallocate
        //   handle()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Factory allocate()" << endl
                          << "================================" << endl;

        // Counts number of sockets currently open (on certain platforms
        // only).
        int initialSockets =
              countSockets(MIN_SOCKET_HANDLE, MAX_SOCKET_HANDLE);

        // Ensure that there are enough unassigned handles in the range
        // counted.
        ASSERT(initialSockets < 0 ||
               (MAX_SOCKET_HANDLE - MIN_SOCKET_HANDLE + 1) >=
               (initialSockets + MAX_SOCKETS_USED));
        bteso_InetStreamSocketFactory<bteso_IPv4Address> testFactory;

        {
            bteso_StreamSocket<bteso_IPv4Address> *newSocket =
                  testFactory.allocate();

            ASSERT(newSocket);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == (initialSockets + 1));

            bteso_SocketHandle::Handle newHandle = newSocket->handle();

            ASSERT(verifyHandle(newHandle));

            testFactory.deallocate(newSocket);

            ASSERT(verifyHandle(newHandle) == 0);

            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);

        }

        {
            bteso_StreamSocket<bteso_IPv4Address> *newSocket1 =
                  testFactory.allocate();

            bteso_StreamSocket<bteso_IPv4Address> *newSocket2 =
                  testFactory.allocate();

            ASSERT(newSocket1);
            ASSERT(newSocket2);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == (initialSockets + 2));

            ASSERT(newSocket1->handle() != newSocket2->handle());

            bteso_SocketHandle::Handle newHandle1 = newSocket1->handle();
            bteso_SocketHandle::Handle newHandle2 = newSocket2->handle();

            ASSERT(verifyHandle(newHandle1));
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket1);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket2);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2) == 0);

            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

        {
            bteso_StreamSocket<bteso_IPv4Address> *newSocket1 =
                  testFactory.allocate();

            bteso_StreamSocket<bteso_IPv4Address> *newSocket2 =
                  testFactory.allocate();

            ASSERT(newSocket1);
            ASSERT(newSocket2);
            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == (initialSockets + 2));

            ASSERT(newSocket1->handle() != newSocket2->handle());

            bteso_SocketHandle::Handle newHandle1 = newSocket1->handle();
            bteso_SocketHandle::Handle newHandle2 = newSocket2->handle();

            ASSERT(verifyHandle(newHandle1));
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket1, true);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket2, false);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2));

            ASSERT(0 == bteso_SocketImpUtil::close(newHandle2));

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2) == 0);

            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

      } break;
      case 1:
      {
        // breathing test.  currently empty
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = 255;
      }
    }

    int cleanupResp;
    if ((cleanupResp = bteso_SocketImpUtil::cleanup()) != 0) {
        cout << "bteso_SocketImpUtil::cleanup failed " << cleanupResp << endl;
    }

    if (testStatus > 0 && testStatus < 255) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
