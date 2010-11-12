// bteso_socketimputil.t.cpp          -*-C++-*-

#include <bteso_socketimputil.h>

#include <bteso_ipv4address.h>

#include <bcemt_threadutil.h>
#include <bcemt_barrier.h>

#include <bdef_bind.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>             // atoi()
#include <bsl_cstring.h>             // memset()
#include <bsl_iostream.h>

#if !defined(BSLS_PLATFORM__CMP_MSVC)
// for getsockname
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------

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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.

typedef bteso_SocketHandle::Handle SockType;

// ADDRLEN_T is the type for the length of a sockaddr address.
#if defined(BSLS_PLATFORM__CMP_MSVC)
typedef int ADDRLEN_T;
#elif defined(BSLS_PLATFORM__OS_HPUX)
typedef int ADDRLEN_T;
#else
typedef socklen_t ADDRLEN_T;
#endif

//=============================================================================
//                           USAGE example helper functions
//-----------------------------------------------------------------------------
typedef bsl::pair<const bteso_IPv4Address, const char *> ClientData;

void serverFunction(const bteso_IPv4Address& IP_ADDR, 
                    bcemt_Barrier *barrier)
{
     bteso_SocketHandle::Handle serverSocket, sessionSocket;
     const int                  BACKLOG      = 32;
     const int                  RECEIVE_SIZE = 32;
     char                       readBuffer[RECEIVE_SIZE];
     int                        rc, errCode = 0;

     rc = bteso_SocketImpUtil::startup(&errCode);      ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                      &serverSocket,
                                      bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
                                      &errCode);
     ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::bind<bteso_IPv4Address>(serverSocket,
                                                       IP_ADDR,
                                                       &errCode);
     ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::listen(serverSocket,
                                      BACKLOG,
                                      &errCode);       ASSERT(0 == rc);

     barrier->wait();                                                       

     rc = bteso_SocketImpUtil::accept<bteso_IPv4Address>(&sessionSocket,
                                                         serverSocket,
                                                         &errCode);
     ASSERT(0 == rc);
     do {
         // Echo each packet back until the client disconnects
         rc = bteso_SocketImpUtil::read(readBuffer,
                                        sessionSocket,
                                        RECEIVE_SIZE,
                                        &errCode);
         if (rc > 0) {
             bteso_SocketImpUtil::write(sessionSocket,
                                        readBuffer,
                                        rc,
                                        &errCode);
         }
     } while (rc > 0);
     rc = bteso_SocketImpUtil::shutDown(sessionSocket,
                                      bteso_SocketImpUtil::BTESO_SHUTDOWN_BOTH,
                                      &errCode);       ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::close(sessionSocket,
                                     &errCode);        ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::close(serverSocket,
                                     &errCode);        ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::cleanup(&errCode);      ASSERT(0 == rc);
}

void clientFunction(const ClientData& clientData)
{
     bteso_SocketHandle::Handle  sendSocket;
     const int                   RECEIVE_SIZE = 32;
     char                        buffer[RECEIVE_SIZE];
     int                         rc, errorCode = 0;
     const bteso_IPv4Address&    IP_ADDR = clientData.first;
     const char                 *DATA = clientData.second;
     const int                   DATA_SIZE = bsl::strlen(DATA) + 1;

     rc = bteso_SocketImpUtil::startup(&errorCode);       ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                      &sendSocket,
                                      bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
                                      &errorCode);
                                                          ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::connect<bteso_IPv4Address>(sendSocket,
                                                          IP_ADDR,
                                                          &errorCode);
                                                          ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::write(sendSocket,
                                     DATA,
                                     DATA_SIZE,
                                     &errorCode);      ASSERT(DATA_SIZE == rc);
     rc = bteso_SocketImpUtil::read(buffer,
                                    sendSocket,
                                    RECEIVE_SIZE,
                                    &errorCode);       ASSERT(DATA_SIZE == rc);

     ASSERT(0 == bsl::strcmp(DATA, buffer));

     rc = bteso_SocketImpUtil::shutDown(
                                      sendSocket,
                                      bteso_SocketImpUtil::BTESO_SHUTDOWN_BOTH,
                                      &errorCode);      ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::close(sendSocket,
                                     &errorCode);         ASSERT(0 == rc);
     rc = bteso_SocketImpUtil::cleanup(&errorCode);       ASSERT(0 == rc);
}

extern "C" void* clientFunc(void *IP_ADDR)
{
    clientFunction(* (const ClientData *) IP_ADDR);
    return 0;
}

//=============================================================================
//                            TEST FUNCTIONS
//=============================================================================

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
#if defined(BTESO_PLATFORM__WIN_SOCKETS)
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    typedef bteso_IPv4Address A;
    typedef bteso_SocketImpUtil T;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

// This component supports inter-process communication functionality, and so
// possible usage within a single process would consist of half of a dialog,
// either the "client" or the "server".  Note that the 'shutDown' operation
// is optional.  The following diagram shows a typical sequence of operations
// on the client and the server processes, for sockets using the TCP protocol:
//..
//                                                 TCP SERVER
//                                               +-----------+ prepares
//                                               | startup() | sockets
//                                               +-----------+ environment
//                                                     |
//                                               +-----------+ gets a
//                                               |   open()  | socket
//                                               +-----------+
//                                                     |
//                                               +-----------+ binds this
//                                               |   bind()  | socket to a
//                                               +-----------+ port number
//                TCP CLIENT                           |
//                                               +-----------+ queues up
//                                               |  listen() | connection
//                                               +-----------+ requests
//                                                     |
//                                               +-----------+ accepts
//                                               |  accept() | connection
//  prepares     +-----------+                   +-----------+ requests from
//  sockets      | startup() |                         |       the queue
//  environment  +-----------+                         |
//                     |                     blocks until a connection
//    gets a     +-----------+                  request is accepted
//    socket     |   open()  |                         |
//               +-----------+                         |
//                     |                               | connection request
// establishes a +-----------+                         | "accepted" by server,
// connection    |  connect()|------------------------>| creates a new socket
// request with  +-----------+                         | socket
// the server          |                               |
//               +-----------+                   +-----------+
//     writes    |   write() |------------------>| read()    | reads client
//   requests    +-----------+                   +-----------+ requests,
//     and             :                               :       process them
//    reads         :                               :       and writes
//   replies     +-----------+                   +-----------+ back replies
//               | read()    |<------------------|   write() | until client
//               +-----------+                   +-----------+ closes the
//                     |                               |       connection
//  closes the   +-----------+                   +-----------+
//  connection   | shutDown()|-------------------| shutDown()|
//  when done    +-----------+                   +-----------+
//                     |                               |       connection
//  closes the   +-----------+                   +-----------+
//  connection   |   close() |-------------------|   close() |
//  when done    +-----------+                   +-----------+
//                     |                               |       connection
//  cleans up    +-----------+                   +-----------+ cleans up
//  sockets      | cleanup() |                   | cleanup() | sockets
//  environment  +-----------+                   +-----------+ environment
//..
///Typical Server Usage
///- - - - - - - - - -
// The following snippets of code illustrate a simple single user echo server
// implemented using 'bteso_socketimputil'.
//..
     bteso_IPv4Address IP_ADDR("127.0.0.1", 8142);

     bcemt_ThreadUtil::Handle stid, ctid;

     bcemt_Barrier barrier(2);
     bcemt_ThreadUtil::create(&stid, 
                              bdef_BindUtil::bind(&serverFunction, 
                                                  IP_ADDR,
                                                  &barrier));
     barrier.wait();
//..
///Typical Client Usage
///- - - - - - - - - -
// The following snippets of code illustrate a simple client implemented using
// 'bteso_socketutil'.
//..
     const char *DATA = "Is it raining in London now?";
     ClientData clientData(IP_ADDR, DATA);
     bcemt_ThreadUtil::create(&ctid, clientFunc, (void *) &clientData);
//..

     ASSERT(0 == bcemt_ThreadUtil::join(stid));
     ASSERT(0 == bcemt_ThreadUtil::join(ctid));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Plan:
        //  The 'socketPair' function is used as a basis for the breathing
        //  test.
        //  Test for bteso_IPv4Address only.
        //  Test for the two supported socket types (stream and datagram).
        //  Only socketPair(newSockets, int type) interface is tested.
        //  TODO Other interfaces.
        //
        // Testing:
        //   static int socketPair(newSockets, int type);
        //   static int socketPair(newSockets, int type, int *errorCode);
        //   static int socketPair(newSockets, int type, int protocol);
        //   static int socketPair(newSockets, type, protocol, errorCode);
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST" << endl
                          << "==============" << endl;

        bteso_SocketHandle::Handle s[2];
        int rc;
        enum T::Type typ;
        int errorCode;

        rc = T::startup(&errorCode);
        ASSERT(0 == rc);                    // verify success

        // Success - type, no error code
        if (veryVerbose) cout << "socketPair(BTESO_SOCKET_STREAM)" << endl;
        int initialSockets = countSockets(0, 20);
        s[0] = -1;                             // set socket var
        s[1] = -1;                             // set socket var
        typ = T::BTESO_SOCKET_STREAM;                  // set socket type;
        rc = T::socketPair<A>(s, typ);              // open a socket
        ASSERT(0 == rc);                    // verify success

        ASSERT(s[0] != s[1]);

        ASSERT(initialSockets < 0 ||
               (initialSockets + 2) == countSockets(0, 20));

        const char buffer[] = { 10, 9, 1, 2, 3, 4, 5 };
        char readBuffer[sizeof(buffer) + 1];

        rc = T::write(s[0], buffer, sizeof(buffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        rc = T::read(readBuffer, s[1], sizeof(readBuffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        const char buf1[] = { 90, 91 };
        const char buf2[] = { 10, 20, 1, 2 };
        btes_Ovec vec[2];
        vec[0].setBuffer((void *) buf1, sizeof(buf1));
        vec[1].setBuffer((void *) buf2, sizeof(buf2));

        rc = T::writev(s[0], vec, 2);
        ASSERT(rc == sizeof(buf1) + sizeof(buf2));

        rc = T::read(readBuffer, s[1], sizeof(readBuffer));
        ASSERT(rc == sizeof(buf1) + sizeof(buf2));

        char rbuf1[2];
        char rbuf2[4];
        btes_Iovec iovec[2];
        iovec[0].setBuffer((void *) rbuf1, sizeof(buf1));
        iovec[1].setBuffer((void *) rbuf2, sizeof(buf2));
        rc = T::write(s[0], buffer, sizeof(buffer));
        ASSERT(rc == sizeof(buffer));

        rc = T::readv(iovec, s[1],  2);
        ASSERT(rc == sizeof(rbuf1) + sizeof(rbuf2));

        T::close(s[0]);
        T::close(s[1]);
        ASSERT(initialSockets < 0 ||
               initialSockets == countSockets(0, 20));

        // Success - type, no error code
        if (veryVerbose) cout << "socketPair(BTESO_SOCKET_DATAGRAM)" << endl;
        s[0] = -1;                             // set socket var
        s[1] = -1;                             // set socket var
        typ = T::BTESO_SOCKET_DATAGRAM;                  // set socket type;
        rc = T::socketPair<A>(s, typ);              // open a socket
        ASSERT(0 == rc);                    // verify success

        ASSERT(s[0] != s[1]);

        rc = T::write(s[0], buffer, sizeof(buffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        rc = T::read(readBuffer, s[1], sizeof(readBuffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        rc = T::write(s[1], buffer, sizeof(buffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        rc = T::read(readBuffer, s[0], sizeof(readBuffer));
        ASSERT(rc == sizeof(buffer));
        if (veryVerbose) P(rc);

        ASSERT(initialSockets < 0 ||
              (initialSockets + 2)== countSockets(0, 20));

        T::close(s[0]);
        T::close(s[1]);
        ASSERT(initialSockets < 0 ||
               initialSockets == countSockets(0, 20));

        if (veryVerbose) cout << "socketPair(BTESO_SOCKET_RAW)" << endl;
        s[0] = -1;                             // set socket var
        s[1] = -1;                             // set socket var
        typ = T::BTESO_SOCKET_RAW;                  // set socket type;
        rc = T::socketPair<A>(s, typ);              // open a socket
        ASSERT(0 != rc);                    // verify failure

        rc = T::cleanup(&errorCode);
        ASSERT(0 == rc);                    // verify success
      } break;
      default: {
        cerr << "WARNING: CASE `"<< test << "' NOT FOUND." <<endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
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
