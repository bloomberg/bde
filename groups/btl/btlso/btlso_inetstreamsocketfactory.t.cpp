// btlso_inetstreamsocketfactory.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_inetstreamsocketfactory.h>

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
//                              --------
//
// 'btlso::InetStreamSocketFactory' is a concrete implementation of the
// 'btlso::StreamSocketFactory' protocol providing a factory for creating
// btlso::InetStreamSocket objects.  We invoke all the methods in the protocol
// and confirm that the allocate and deallocate methods work as expected.
//-----------------------------------------------------------------------------
// [ 2] btlso::InetStreamSocketFactory(basicAllocator = 0);
// [ 2] ~btlso::InetStreamSocketFactory();
// [ 2] btlso::StreamSocket<ADDRESS> *allocate();
// [ 3] btlso::StreamSocket<ADDRESS> *allocate(handle);
// [ 2] void deallocate(btlso::StreamSocket<ADDRESS> *socket);
// [ 2] void deallocate(btlso::StreamSocket<ADDRESS> *socket, bool flag);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

#if defined(BTLSO_PLATFORM_WIN_SOCKETS) \
    || defined(BSLS_PLATFORM_OS_HPUX)
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
    int       socketNumber;
    int       socketCount = 0;
    char      address[20];
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
   ::alarm(1);
}
#endif

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
      case 4: {
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

        bslma::TestAllocator ta;

{
///Usage
///-----
// In this section we show intended usage of this component
//
///Example 1: Create a New Stream Socket
///- - - - - - - - - - - - - - - - - - -
// We can use 'btlso::InetStreamSocketFactory' to allocate a new TCP-based
// stream socket.
//
// First, we create a 'btlso::InetStreamSocketFactory' object:
//..
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Then, we create a stream socket:
//..
    btlso::StreamSocket<btlso::IPv4Address> *mySocket = factory.allocate();
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
///Example 2: Create a 'btlso::StreamSocket' Object From Existing Socket Handle
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use 'btlso::InetStreamSocketFactory' to allocate a
// 'btlso::StreamSocket' object that attaches to an existing socket handle.
// This socket handle may be created from a third-party library (such as
// OpenSSL).  Using a 'btlso::StreamSocket' object rather than the socket
// handle directly is highly desirable as it enables the use of other BTE
// components on the socket.  In this example, the socket handle is created
// from the 'btlso_socketimputil' component for illustrative purpose.
//
// First, we create a socket handle 'fd':
//..
    btlso::SocketHandle::Handle fd;
    int                         nativeErrNo = 0;

    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                         &fd,
                                         btlso::SocketImpUtil::k_SOCKET_STREAM,
                                         &nativeErrNo);
    ASSERT(0 == nativeErrNo);
//..
// Then, we create factory:
//..
    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//..
// Next, we allocate a stream socket attached to 'fd':
//..
    btlso::StreamSocket<btlso::IPv4Address> *mySocket = factory.allocate(fd);
    ASSERT(mySocket);
//..
// Notice that 'fd' is passed into the 'allocate' method as an argument.  Any
// BTE component that uses 'btlso::StreamSocket<btlso::IPv4Address>' can now be
// used on 'mySocket'.
//
// Finally, when we're done, we recycle the socket:
//..
    factory.deallocate(mySocket);
//..
}
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FACTORY ALLOCATE WITH HANDLE
        //   Created btlso::StreamSocket behaves correctly and is associated
        //   with the correct socket handle.
        //
        // Plan:
        //   Create a pair of sockets using btlso::SocketImpUtil::socketPair().
        //   Create stream sockets using the handles.
        //   Verify that the stream sockets are valid:
        //   1) use handle() accessor to verify handle.
        //   2) send data and receive data
        //   3) deallocate stream socket and verify socket is no longer open.
        //
        // Testing:
        //   btlso::StreamSocket<ADDRESS> *allocate(handle);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Factory allocate(handle)" << endl
                          << "================================" << endl;

           btlso::InetStreamSocketFactory<btlso::IPv4Address> testFactory;
           btlso::SocketHandle::Handle handles[2];

           int resp = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                  handles, btlso::SocketImpUtil::k_SOCKET_STREAM);
           ASSERT(resp == 0);
           btlso::StreamSocket<btlso::IPv4Address> *newSocketA =
                 testFactory.allocate(handles[0]);
           btlso::StreamSocket<btlso::IPv4Address> *newSocketB =
                 testFactory.allocate(handles[1]);

           ASSERT(newSocketA);
           ASSERT(newSocketB);
           ASSERT(newSocketA != newSocketB);

           btlso::SocketHandle::Handle newHandleA = newSocketA->handle();
           btlso::SocketHandle::Handle newHandleB = newSocketB->handle();
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

           ASSERT(resp == btlso::SocketHandle::e_ERROR_EOF);

           testFactory.deallocate(newSocketB);

           ASSERT(verifyHandle(newHandleA) == 0);
           ASSERT(verifyHandle(newHandleB) == 0);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FACTORY ALLOCATE WITHOUT HANDLE
        //
        // Plan:
        //
        // Testing:
        //   btlso::StreamSocket<ADDRESS> *allocate();
        //   void deallocate(btlso::StreamSocket<ADDRESS> *socket);
        //   void deallocate(btlso::StreamSocket<ADDRESS> *socket, bool flag);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Factory allocate()" << endl
                          << "==========================" << endl;

        // Counts number of sockets currently open (on certain platforms
        // only).

        int initialSockets = countSockets(MIN_SOCKET_HANDLE,
                                          MAX_SOCKET_HANDLE);

        // Ensure that there are enough unassigned handles in the range
        // counted.

        ASSERT(initialSockets < 0 ||
               (MAX_SOCKET_HANDLE - MIN_SOCKET_HANDLE + 1) >=
               (initialSockets + MAX_SOCKETS_USED));

        btlso::InetStreamSocketFactory<btlso::IPv4Address> testFactory;

        {
            btlso::StreamSocket<btlso::IPv4Address> *newSocket =
                                                        testFactory.allocate();

            ASSERT(newSocket);

            btlso::SocketHandle::Handle newHandle = newSocket->handle();

            ASSERT(verifyHandle(newHandle));

            testFactory.deallocate(newSocket);

            ASSERT(verifyHandle(newHandle) == 0);

            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);

        }

        {
            btlso::StreamSocket<btlso::IPv4Address> *newSocket1 =
                  testFactory.allocate();

            btlso::StreamSocket<btlso::IPv4Address> *newSocket2 =
                  testFactory.allocate();

            ASSERT(newSocket1);
            ASSERT(newSocket2);

            ASSERT(newSocket1->handle() != newSocket2->handle());

            btlso::SocketHandle::Handle newHandle1 = newSocket1->handle();
            btlso::SocketHandle::Handle newHandle2 = newSocket2->handle();

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
            btlso::StreamSocket<btlso::IPv4Address> *newSocket1 =
                  testFactory.allocate();

            btlso::StreamSocket<btlso::IPv4Address> *newSocket2 =
                  testFactory.allocate();

            ASSERT(newSocket1);
            ASSERT(newSocket2);

            ASSERT(newSocket1->handle() != newSocket2->handle());

            btlso::SocketHandle::Handle newHandle1 = newSocket1->handle();
            btlso::SocketHandle::Handle newHandle2 = newSocket2->handle();

            ASSERT(verifyHandle(newHandle1));
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket1, true);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2));

            testFactory.deallocate(newSocket2, false);

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2));

            ASSERT(0 == btlso::SocketImpUtil::close(newHandle2));

            ASSERT(verifyHandle(newHandle1) == 0);
            ASSERT(verifyHandle(newHandle2) == 0);

            ASSERT(initialSockets < 0 ||
                   countSockets(0,20) == initialSockets);
        }

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
