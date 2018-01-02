// btlso_ioutil.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_ioutil.h>

#include <btlso_socketimputil.h>
#include <btlso_ipv4address.h>

#include <bslmt_mutex.h>

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unistd.h>
#include <strings.h>
#include <fcntl.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>       // atoi()
#include <bsl_cstring.h>       // strcmp()
#include <bsl_c_stdio.h>
#include <bsl_c_signal.h>
#include <bsl_c_errno.h>

#else                    // windows

#include <process.h>     // thread functions, e.g., _beginthreadex().
#include <winbase.h>     // sleep().
#include <windows.h>

#include <bsl_c_stdio.h>
#endif

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#if defined(BSLS_PLATFORM_CMP_MSVC)
#if defined(ASYNCH)
#pragma push_macro("ASYNCH")
#undef ASYNCH
#define BTESO_POP_MACRO_ASYNCH
#endif
#endif

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
// The component under test consists of a set of static member functions and
// enumeration types that are used to provide platform independence for
// socket-based programs.  The basic plan for testing is to ensure that all
// methods in the component pass correct parameters through to the system level
// calls.  First, we'll do the "usage example" test to remove the "gross" bugs.
// Then we will test "manipulator" functions by setting up connections with
// some "helper" clients to check if the right I/O mode flag is set to the
// right value.  To have some clients, some helper test cases (case 13, 14) are
// created, also threads are created to work as TCP/UDP clients for test.
// Because TCP and UDP sockets communicate with their clients differently,
// usually we test a function for TCP sockets and UDP sockets in separate test
// cases for the reason of clearance.  Next we test "accessor" functions by
// creating different sockets(TCP/UDP), modifying the sockets' corresponding
// I/O mode flag and compare the retrieval mode value against the expected mode
// value.  The set of data are selected to have a even distribution in
// different socket type and function arguments values by change one item at a
// time.
// ----------------------------------------------------------------------------
// MANIPULATORS
// [ 2] int setBlockingMode(handle, option, *errorCode);
// [ 4] int setCloseOnExec(handle, option, *errorCode);
// [ 6] int setAsync(handle, option, *errorCode);
//
// ACCESSORS
// [10] int getBlockingMode(BlockingMode *result, handle, *errorCode = 0);
// [11] int getCloseOnExec(int *result, handle, *errorCode = 0);
// [12] int getAsync(AsyncMode *result, handle, *errorCode = 0);
// ----------------------------------------------------------------------------
// [13] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [-13] Helper cases for forked processes
// [-14]

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error %s (%d): %s (failed)\n",  __FILE__, i, s);
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

//-----------------------------------------------------------------------------
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { printf(#I ": %d\n", I); aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\n", I, J); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { printf(#I ": %d\t" #J << ": %d\t" #K ": %d\n", I, J, K); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\t" #K ": %d\t" #L ": %d\n", I, \
                      J, K, L); \
               aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { printf(#I ": %d\t" #J ": %d\t" #K ": %d\t" #L ": %d\t" \
                      #M ": %d\n", I, J, K, L, M); \
                      aSsErT(1, #X, __LINE__); } }

//-----------------------------------------------------------------------------
bslmt::Mutex  d_mutex;   // for i/o synchronization in all threads

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btlso::SocketHandle::Handle SocketHandle;

#ifdef BSLS_PLATFORM_OS_UNIX
typedef void*  THREAD_PARAM;
#else
typedef LPVOID THREAD_PARAM;
#endif

const char* LOCALHOST = "127.0.0.1";

static int verbose;
// static int veryVerbose;      // not used
// static int veryVeryVerbose;  // not used

#define  ASYNCH  btlso::IoUtil::e_ASYNCHRONOUS
#define  SYNCH  btlso::IoUtil::e_SYNCHRONOUS

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
// #ifdef BSLS_PLATFORM_OS_UNIX
#if  0
static void sigio_handler(int sig)
{
    if (verbose) {
        PT("get SIGIO/SIGPOLL.");
    }
}

static int create_child_process(char* executable, int testcase, int subcase,
                                int   serverFd=0)
// create a child process and then call execv().
{

    int i, rcode;
    char *arg[6];

    for(i = 0; i < 6; i++) {
        arg[i] = (char*) malloc(61*sizeof(char));
    }
    snprintf(arg[0],61, "%s", executable);
    snprintf(arg[1],61, "%d", testcase);
    snprintf(arg[2],61, "%d", serverFd);
    for(i = 3; i < 5; i++)
        snprintf(arg[i],61, "%d",0);
    snprintf(arg[5],61, "%d", subcase);

    if(fork() == 0) {
        if(rcode = execv(arg[0], arg)) {
            if (verbose) {
                printf("return value: %d and errno: %d\n", rcode, errno);
            }
            return errno;
        }
        return 0;  //dummy...
    }
    else {
        return 0;
    }
}

static int create_child_process2(char* arg0, int testcase, int subcase,
                                 int serverFd=0)
// create a child process and then call execlp().
{
    int i, rcode;
    char *arg[6];

    for (i = 0; i < 6; i++) {
        arg[i] = (char*) malloc(61*sizeof(char));
    }
    snprintf(arg[1],61, "%d",testcase);
    snprintf(arg[2],61, "%d",serverFd);
    for (i = 3; i < 5; i++)
        snprintf(arg[i],61, "%d",0);
    snprintf(arg[5],61, "%d",subcase);

    if(fork() == 0) {
        if(rcode = execlp(arg0, arg0, arg[1], arg[2], arg[3],
                                              arg[4], arg[5]) ) {
            if (verbose) {
                printf("return value: %d and errno: %d\n", rcode, errno);
            }
            return errno;
        }
        return 0;
    }
    else {
        return 0;
    }
}
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
extern "C" {
void* thread_as_udp_client(THREAD_PARAM arg)
    // a thread as a UDP client.
{
    int portNum = *(int*)arg;
    btlso::SocketHandle::Handle sendSocket;
    btlso::IPv4Address serverAddr;
    serverAddr.setIpAddress(LOCALHOST);
    serverAddr.setPortNumber(portNum);
    int        errorCode = 0;
    const char *QUESTION = "Is it subcase 6?";

    if (verbose) {
        QT("udp thread client is sleeping 1 second.");
    }
    sleep(1);
    btlso::SocketImpUtil::startup(&errorCode);
    ASSERT(0 == errorCode);
    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                    &sendSocket,
                                    btlso::SocketImpUtil::k_SOCKET_DATAGRAM,
                                    &errorCode);
    ASSERT(0 == errorCode);

    btlso::SocketImpUtil::writeTo(sendSocket,
                                 serverAddr,
                                 QUESTION,
                                 strlen(QUESTION),
                                 &errorCode);

    ASSERT(0 == errorCode);
    if (verbose) {
        QT("udp thread client finishes writing.");
    }
    // set back to synchronous mode.
    int ret, errCode=0;
    btlso::IoUtil::AsyncMode result;
    ret = btlso::IoUtil::setAsync(sendSocket,
                                 btlso::IoUtil::e_SYNCHRONOUS,
                                 &errCode);
    ret = btlso::IoUtil::getAsync(&result,
                                 sendSocket,
                                 &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
    ASSERT(0 == errCode);
    ASSERT(0 == ret);
#else
    ASSERT(-1 == ret);
#endif
    ASSERT(SYNCH == result);

    btlso::SocketImpUtil::close(sendSocket,
                               &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("udp thread client finished");
    }
    return 0;
}

void* thread_as_tcp_client(THREAD_PARAM arg)
    // a thread as a TCP client.
{
    int portNum = *(int*)arg;
    btlso::SocketHandle::Handle sendSocket;
    btlso::IPv4Address serverAddr;
    serverAddr.setIpAddress(LOCALHOST);
    serverAddr.setPortNumber(portNum);
    int errorCode = 0;

    sleep(1);
    btlso::SocketImpUtil::startup(&errorCode);
    ASSERT(0 == errorCode);

    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                      &sendSocket,
                                      btlso::SocketImpUtil::k_SOCKET_STREAM,
                                      &errorCode);
    ASSERT(0 == errorCode);

    btlso::SocketImpUtil::connect<btlso::IPv4Address>(sendSocket,
                                                      serverAddr,
                                                      &errorCode);
    ASSERT(0 == errorCode);

    int ret, errCode=0;
    btlso::IoUtil::AsyncMode result;
    ret = btlso::IoUtil::setAsync(sendSocket,
                                  SYNCH,
                                  &errCode);
    ret = btlso::IoUtil::getAsync(&result, sendSocket, &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
    ASSERT(0 == errCode);
    ASSERT(0 == ret);
#else
    ASSERT(-1 == ret);
#endif
    ASSERT(SYNCH == result);
    btlso::SocketImpUtil::close(sendSocket, &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("TCP thread client finished");
    }
    return 0;
}

}  // end of extern "C"

#else        // windows
unsigned __stdcall thread_win_udp_client(THREAD_PARAM arg)
    // a thread as a UDP client.
{
    int portNum = *(int*)arg;
    btlso::SocketHandle::Handle sendSocket;
    btlso::IPv4Address serverAddr;
    serverAddr.setIpAddress(LOCALHOST);
    serverAddr.setPortNumber(portNum);
    const int  BACKLOG      = 32;
    const int  BUFFER_SIZE = 32;
    char       buffer[BUFFER_SIZE], receiveBuffer[BUFFER_SIZE];
    int        rc, errorCode = 0;
    const char *QUESTION = "Is it subcase 6?";

    if (verbose) {
        QT("udp thread client is sleeping 1 second.");
    }
    Sleep(1);
    btlso::SocketImpUtil::startup(&errorCode);
    ASSERT(0 == errorCode);
    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                    &sendSocket,
                                    btlso::SocketImpUtil::k_SOCKET_DATAGRAM,
                                    &errorCode);
    ASSERT(0 == errorCode);

    btlso::SocketImpUtil::writeTo(sendSocket,
                                  serverAddr,
                                  QUESTION,
                                  strlen(QUESTION),
                                  &errorCode);

    ASSERT(0 == errorCode);
    if (verbose) {
        QT("udp thread client finishes writing.");
    }
    btlso::SocketImpUtil::close(sendSocket, &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("udp thread client finished");
    }
    #ifdef BSLS_PLATFORM_OS_WINDOWS
        _endthreadex(0);
    #endif
    return 0;
}

unsigned __stdcall thread_win_tcp_client(THREAD_PARAM arg)
    // a thread as a TCP client.
{
    int portNum = *(int*)arg;
    btlso::SocketHandle::Handle sendSocket;
    btlso::IPv4Address serverAddr;
    serverAddr.setIpAddress(LOCALHOST);
    serverAddr.setPortNumber(portNum);
    const int  BACKLOG      = 32;
    const int  RECEIVE_SIZE = 32;
    char       buffer[RECEIVE_SIZE];
    int        errorCode = 0;
    const char *QUESTION = "Is it subcase 6?";
    if (verbose) {
        QT("tcp thread client is sleeping 1 second.");
    }
    Sleep(1000);

    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                      &sendSocket,
                                      btlso::SocketImpUtil::k_SOCKET_STREAM,
                                      &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("tcp thread is trying connecting.....");
    }
    btlso::SocketImpUtil::connect<btlso::IPv4Address>(sendSocket,
                                                      serverAddr,
                                                      &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("tcp thread connected.....");
    }
    btlso::SocketImpUtil::close(sendSocket, &errorCode);
    ASSERT(0 == errorCode);
    if (verbose) {
        QT("TCP thread client finished");
    }
    _endthreadex(0);

    return 0;
}

#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    // veryVerbose = argc > 3;      // not used
    // veryVeryVerbose = argc > 4;  // not used
    // int subcase = argc > 5 ? atoi(argv[5]) : 0; // to access helper cases.
    int errCode = 0;
    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    #if defined(BSLS_PLATFORM_OS_UNIX) && defined(SIGPOLL)
    signal(SIGPOLL, SIG_IGN);  // ignore SIGIO/SIGPOLL generated during test.
    #endif
    btlso::SocketImpUtil::startup(&errCode);
    ASSERT(0 == errCode);

    switch (test) { case 0:   // Zero is always the leading case.
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

        if (verbose) {
            cout << "TESTING Usage Example"
                 << "=====================" << endl;
        }

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Changing the blocking mode on a socket
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to make a socket non-blocking.
//..
        btlso::SocketHandle::Handle socketHandle;
        btlso::IoUtil::BlockingMode option =
                                          btlso::IoUtil::e_NONBLOCKING;
        int nativeErrNo=0, s;
        btlso::IoUtil::BlockingMode result;

        btlso::SocketImpUtil::open<btlso::IPv4Address>(
                         &socketHandle,
                         btlso::SocketImpUtil::k_SOCKET_STREAM,
                         &nativeErrNo);
        ASSERT(0 == nativeErrNo);
        // Set blocking mode.
        s = btlso::IoUtil::setBlockingMode(socketHandle, option,
                                          &nativeErrNo);
        ASSERT(0 == s);   ASSERT(0 == nativeErrNo);

        #if   defined(BSLS_PLATFORM_OS_UNIX) \
           && !defined(BSLS_PLATFORM_OS_CYGWIN)
        {
            s = btlso::IoUtil::getBlockingMode(&result,
                                              socketHandle,
                                              &nativeErrNo);
            ASSERT(0 == s);
            ASSERT(btlso::IoUtil::e_NONBLOCKING == result);
            ASSERT(0 == nativeErrNo);
        }
        #endif

        // Set the flag back.
        option = btlso::IoUtil::e_BLOCKING;
        s = btlso::IoUtil::setBlockingMode(socketHandle, option,
                                          &nativeErrNo);
        ASSERT(0 == s);   ASSERT(0 == nativeErrNo);

        #if   defined(BSLS_PLATFORM_OS_UNIX) \
           && !defined(BSLS_PLATFORM_OS_CYGWIN)
        {
            s = btlso::IoUtil::getBlockingMode(&result,
                                              socketHandle,
                                              &nativeErrNo);
            ASSERT(0 == s); ASSERT(btlso::IoUtil::e_BLOCKING == result);
            ASSERT(0 == nativeErrNo);
        }
        #endif
      } break;
      case 12: {
        // ----------------------------------------------------------------
        // TESTING 'getAsync' FUNCTION:
        //   Get the asynchronous mode value for the specified socket.
        //   The function return 0 and put the mode value into 'result' on
        //   success.  It will return -1 on failure and the errorCode
        //   will be set to either the platform-dependent error code
        //   if the valid 'errorCode' argument is passed.
        //
        // Plan:
        //   Create an array of struct which consists of 'lineNum',
        //   socket type, option to be set to a socket and the expected
        //   mode value.  For each element, a socket will be created,
        //   and set the flag based on the option value of the struct array
        //   element, then call 'getAsync' to compare the return 'result'
        //   against the expected mode value.
        //
        // Testing
        //   int getAsync(int *result, handle, int *errorCode = 0);
        // ----------------------------------------------------------------

        btlso::IoUtil::AsyncMode result;
        int ret=0, errCode=0;
        btlso::SocketImpUtil::Type tcp, udp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        if (verbose) {
            QT("Testing 'getSetAsync' method");
            QT("============================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    // socket type.
          btlso::IoUtil::AsyncMode   d_option;  // option to set the mode.
          int                       d_expMode; // the expected mode value
        } VALUES[] =
        //line    type            option            expMode
        //----    ----            ------            -------
        {
          { L_,    tcp,   btlso::IoUtil::e_SYNCHRONOUS,     SYNCH  },
          { L_,    tcp,   btlso::IoUtil::e_ASYNCHRONOUS,    ASYNCH },
          { L_,    udp,   btlso::IoUtil::e_SYNCHRONOUS,     SYNCH  },
          { L_,    udp,   btlso::IoUtil::e_ASYNCHRONOUS,    ASYNCH }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];
        btlso::IPv4Address serverAddr, localAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        for (int i = 0; i < NUM_VALUES; i++) {
            errCode = 0;
            btlso::SocketImpUtil::open<btlso::IPv4Address>(&serverSocket[i],
                                                           VALUES[i].d_type,
                                                           &errCode);

            LOOP_ASSERT(i, 0 == errCode);
            ret = btlso::IoUtil::setAsync(serverSocket[i],
                                         VALUES[i].d_option,
                                         &errCode);
            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                #ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);
                #else
                LOOP_ASSERT(i, -1 == ret);
                #endif

                ret = btlso::IoUtil::setAsync(serverSocket[i],
                                             VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i]);
                #ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);
                #else
                LOOP_ASSERT(i, -1 == ret);
                #endif

                btlso::SocketImpUtil::close(serverSocket[i],
                                            &errCode);

            }
            #else   // for windows
                LOOP_ASSERT(i, -1 == ret);
            #endif
        }
      } break;
      case 11: {
        // ----------------------------------------------------------------
        // TESTING 'getCloseOnExec' FUNCTION:
        //   Get the close-on-exec mode value for the specified socket.
        //   The function return 0 and put the mode value into 'result' on
        //   success.  It will return -1 on failure and the errorCode
        //   will be set to the platform-dependent error code
        //   if the valid 'errorCode' argument is passed.
        //
        // Plan:
        //   Create an array of struct which consists of 'lineNum',
        //   socket type, option to be set to a socket and the expected
        //   mode value.  For each element, a socket will be created,
        //   and set the flag based on the option value of the struct array
        //   element, then call 'getCloseOnExec' to compare the return
        //   'result' against the expected mode value.
        //
        // Testing
        //   int getCloseOnExec(int *result, handle, int *errorCode = 0);
        // ----------------------------------------------------------------
        static int result;
        int ret=0, errCode = 0;

        if (verbose) {
            QT("Testing 'getCloseOnExec' method");
            QT("===============================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    //socket type.
          int                        d_option;  //option to set the mode.
          int                       d_expMode; //the expected mode value
        } VALUES[] =
        //line              type                        option  expMode
        //----              ----                        ------  -------
        {
          { L_,  btlso::SocketImpUtil::k_SOCKET_STREAM,   INT_MIN,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_STREAM,        -1,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_STREAM,         1,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_STREAM,   INT_MAX,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_STREAM,         0,  0 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_DATAGRAM, INT_MIN,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_DATAGRAM,      -1,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_DATAGRAM,       1,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_DATAGRAM, INT_MIN,  1 },
          { L_,  btlso::SocketImpUtil::k_SOCKET_DATAGRAM,       0,  0 }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];
        btlso::IPv4Address serverAddr, localAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                 &serverSocket[i],
                                                 VALUES[i].d_type,
                                                 &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                               VALUES[i].d_option,
                                               &errCode);
            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getCloseOnExec(&result,
                                                   serverSocket[i],
                                                   &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);

                ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                   VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getCloseOnExec(&result,
                                                   serverSocket[i]);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);

            }
            #else
                LOOP_ASSERT(i, -1 == ret);
            #endif
        }
      } break;
      case 10: {
        // ----------------------------------------------------------------
        // TESTING 'getBlockingMode' FUNCTION:
        //   Get the blocking mode value for the specified socket.
        //   The function return 0 and put the mode value into 'result' on
        //   success.  It will return -1 on failure and the errorCode
        //   will be set to either the platform-dependent error code
        //   if the valid 'errorCode' argument is passed.
        //
        // Plan:
        //   Create an array of struct which consists of 'lineNum',
        //   socket type, option to be set to a socket and the expected
        //   mode value.  For each element, a socket will be created,
        //   and set the flag based on the option value of the struct array
        //   element, then call 'getBlockingMode' to compare the return
        //   'result' against the expected mode value.
        //
        // Testing
        //   int getBlockingMode(int *result, handle, int *errorCode = 0);
        // ----------------------------------------------------------------
        int ret=0, errCode=0, *errNull=NULL;
        btlso::IoUtil::BlockingMode result;
        btlso::SocketImpUtil::Type tcp, udp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;
        if (verbose) {
            QT("Testing 'getBlockingMode' method");
            QT("================================");
        }

        struct {
          int                         d_lineNum;
          btlso::SocketImpUtil::Type  d_type;    //socket type.
          btlso::IoUtil::BlockingMode d_option;  //option to set the mode.
          int                        d_expMode; //the expected mode value
        } VALUES[] =
        //line    type          option            expMode
        //----    ----          ------            -------
        {
          { L_,    tcp,    btlso::IoUtil::e_NONBLOCKING,      1 },
          { L_,    tcp,    btlso::IoUtil::e_BLOCKING,         0 },
          { L_,    udp,    btlso::IoUtil::e_NONBLOCKING,      1 },
          { L_,    udp,    btlso::IoUtil::e_BLOCKING,         0 }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];
        btlso::IPv4Address serverAddr;
        serverAddr.setIpAddress(LOCALHOST);
        serverAddr.setPortNumber(10142);

        for (int i = 0; i < NUM_VALUES; i++) {
            errCode = 0;
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                        &serverSocket[i],
                                                        VALUES[i].d_type,
                                                        &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                VALUES[i].d_option,
                                                &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            LOOP_ASSERT(i, 0 == ret);
            ret = btlso::IoUtil::getBlockingMode(&result,
                                                serverSocket[i],
                                                &errCode);
            #if    defined(BSLS_PLATFORM_OS_UNIX) \
               && !defined(BSLS_PLATFORM_OS_CYGWIN)
            {

                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i]);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    errNull);
                LOOP_ASSERT(i, 0 == errNull);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, VALUES[i].d_expMode == result);

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);

            }
            #else  // for windows.
                LOOP_ASSERT(i, -1 == ret);
            #endif
        }
      } break;
      case 9: {
        // ----------------------------------------------------------------
        // TESTING 'setAsync' FUNCTION FOR UDP SYNCHRONOUS SOCKETS:
        //   Set the specified socket to asynchronous mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The main concerns for this function are that:
        //     (1) If it's setting the right flag.
        //     (2) If it can turn off the mode flag for different kinds
        //         of sockets supported by other BDE components, e.g.,
        //         btlso_socketimputil.
        //     (3) If it can turn on the mode flag for a socket of a
        //         given type when the user passes any non-zero integer
        //         option value.
        //     (4) If it can turn on the mode flag for a socket of a
        //         given type whether a user passes an 'errorCode'
        //         argument or not.
        //     (5) If it can still set on/off the mode flag when other
        //         I/O modes have already been set.
        //     (6) The error code and return value are correctly
        //         returned.
        //   To resolve concerns (1) and (2), a server socket is created
        //   and set its the mode flag on or off respectively.
        //   A child process is created to try binding a sever address with
        //   this socket handle.  The behavior how the call to bind()
        //   returns can be used to test if the mode is set correctly.
        //   To resolve concerns (3), (4), (5) and (6), an array 'VALUES'
        //   of struct consisting of the socket type, option flag and
        //   the expected mode value.  Create a socket for each
        //   corresponding element in the array, make sure every socket's
        //   mode flag value matches with the expected mode
        //   value, also make sure for every setting operation, the
        //   function's return value and error code are set correctly.
        //
        // Note:
        //   This case only tests synchronous UDP sockets situation.
        // Testing:
        //   int setAsync(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------
        btlso::IoUtil::AsyncMode result;
        int ret=0, errCode = 0, *errNull = NULL;
        btlso::IPv4Address serverAddr, localAddr, clientAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        const int    RECEIVE_SIZE = 32;
        btlso::SocketImpUtil::Type tcp, udp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        if (verbose) {
            QT("Testing 'setAsync' method");
            QT("=========================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    // socket type.
          btlso::IoUtil::AsyncMode   d_option;  // mode option.
        } VALUES[] =
        //line         type                   option
        //----         ----                   ------
        {
          { L_,         udp,        btlso::IoUtil::e_ASYNCHRONOUS }
        };

        char         readBuffer[RECEIVE_SIZE];
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                  &serverSocket[i],
                                                  VALUES[i].d_type,
                                                  &errCode);

            LOOP_ASSERT(i, 0 == errCode);

            ret = btlso::IoUtil::setAsync(serverSocket[i],
                                         VALUES[i].d_option,
                                         &errCode);

            #if   defined(BSLS_PLATFORM_OS_UNIX) \
               && !defined(BSLS_PLATFORM_OS_CYGWIN)
            {
                LOOP_ASSERT(i, 0 == errCode);
                ASSERT( 0 == ret);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, ASYNCH == result);

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                                     serverSocket[i],
                                                     serverAddr,
                                                     &errCode );

                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));

                pthread_t id;
                int status = 0, portNum = localAddr.portNumber();

                #ifdef BDE_BUILD_TARGET_MT
                    status = pthread_create(&id, 0, thread_as_udp_client,
                                            (void*)&portNum);
                    LOOP_ASSERT(i, 0 == status);
                    btlso::SocketImpUtil::readFrom(&clientAddr,
                                                  readBuffer,
                                                  serverSocket[i],
                                                  RECEIVE_SIZE,
                                                  &errCode);
                    LOOP_ASSERT(i, 0 == errCode);
                    status = pthread_join(id,NULL);
                    LOOP_ASSERT(i, 0 == status);
                #endif
                btlso::SocketImpUtil::close(serverSocket[i],
                                       &errCode);

            }
            #else  // for windows.
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }
        if (verbose) {
            QT("Testing the function parameters");
            QT("===============================");
        }

        {
            struct {
              int                        d_lineNum;
              btlso::SocketImpUtil::Type d_type;    // socket type.
                                 // option to set the mode:
              btlso::IoUtil::AsyncMode   d_option;
            } VALUES[] =
            //line      type                 option
            //----      ----                 ------
            {
              { L_,      tcp,        btlso::IoUtil::e_SYNCHRONOUS },
              { L_,      udp,        btlso::IoUtil::e_SYNCHRONOUS }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].d_type,
                                 &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setAsync(serverSocket[i],
                                             VALUES[i].d_option,
                                             &errCode);

                #if   defined(BSLS_PLATFORM_OS_UNIX) \
                   && !defined(BSLS_PLATFORM_OS_CYGWIN)
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, SYNCH == result);

                ret = btlso::IoUtil::setAsync(
                                          serverSocket[i],
                                          btlso::IoUtil::e_ASYNCHRONOUS,
                                          &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                         serverSocket[i],
                                         &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, ASYNCH == result);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setAsync(serverSocket[i],
                                             VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, SYNCH == result);

                ret = btlso::IoUtil::setAsync(
                                          serverSocket[i],
                                          btlso::IoUtil::e_ASYNCHRONOUS,
                                          &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, ASYNCH == result);

                // Test when pass errNull.
                ret = btlso::IoUtil::setAsync(serverSocket[i],
                                             VALUES[i].d_option,
                                             errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, SYNCH == result);
                // Set the flag when it's already on(set twice).
                ret = btlso::IoUtil::setAsync(serverSocket[i],
                                             VALUES[i].d_option,
                                             errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, SYNCH == result);
                // Close the socket.
                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);

                #else  // windows code
                LOOP_ASSERT(i, 0 >= ret);
                #endif
            }
        }
      } break;
      case 8: {
        // ----------------------------------------------------------------
        // TESTING 'setAsync' FUNCTION FOR UDP ASYNCHRONOUS SOCKETS:
        //   Set the specified socket to asynchronous mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The UDP socket is set to asynchronous, the others are same as
        //   "case 7".
        //
        // Testing
        //   int setAsync(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------
        btlso::IoUtil::AsyncMode result;
        int ret=0, errCode = 0;

        const int RECEIVE_SIZE = 32;
        btlso::SocketImpUtil::Type udp;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        if (verbose) {
            QT("Testing 'setAsync' method");
            QT("=========================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    // socket type.
          btlso::IoUtil::AsyncMode   d_option;  // mode option.
        } VALUES[] =
        //line          type               option
        //----          ----               ------
        {
          { L_,          udp,        btlso::IoUtil::e_ASYNCHRONOUS }
        };

        char readBuffer[RECEIVE_SIZE];
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];
        btlso::IPv4Address serverAddr, localAddr, clientAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                  &serverSocket[i],
                                                  VALUES[i].d_type,
                                                  &errCode);

            LOOP_ASSERT(i, 0 == errCode);

            ret = btlso::IoUtil::setAsync(serverSocket[i],
                                          VALUES[i].d_option,
                                          &errCode);

            // Asynchronous mode does not work for UDP sockets on Cygwin.

            #if   defined(BSLS_PLATFORM_OS_UNIX) \
               && !defined(BSLS_PLATFORM_OS_CYGWIN)
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, ASYNCH == result); // asynchronous mode.

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                                     serverSocket[i],
                                                     serverAddr,
                                                     &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));
                int portNum = localAddr.portNumber();
                pthread_t id;
                int status = 0;

                #ifdef BDE_BUILD_TARGET_MT
                    status = pthread_create(&id, 0, thread_as_udp_client,
                                            (void*)&portNum);
                    LOOP_ASSERT(i, 0 == status);
                    btlso::SocketImpUtil::readFrom(&clientAddr,
                                                  readBuffer,
                                                  serverSocket[i],
                                                  RECEIVE_SIZE,
                                                  &errCode);
                    LOOP_ASSERT(i, 0 == errCode);
                    status = pthread_join(id,NULL);
                    LOOP_ASSERT(i, 0 == status);
                #endif
                // Set back to synchronous mode.
                ret = btlso::IoUtil::setAsync(
                                          serverSocket[i],
                                          btlso::IoUtil::e_ASYNCHRONOUS,
                                          &errCode);
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, ASYNCH == result);

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);
            }
            #else
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }
      } break;
      case 7: {
        // ----------------------------------------------------------------
        // TESTING 'setAsync' FUNCTION FOR TCP SYNCHRONOUS SOCKETS:
        //   Set the specified socket to asynchronous mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The TCP socket is set to synchronous, the others are same as
        //   "case 8".
        //
        // Testing
        //   int setAsync(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------
        btlso::IoUtil::AsyncMode result;
        int ret=0, errCode = 0;
        btlso::IPv4Address serverAddr, localAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        const int BACKLOG = 32;
        btlso::SocketImpUtil::Type tcp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;

        if (verbose) {
            QT("Testing 'setAsync' method");
            QT("=========================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    //socket type.
          btlso::IoUtil::AsyncMode   d_option;  //option to set the mode.
        } VALUES[] =
        //  line         type                option
        //  ----         ----                ------
        {
          {   L_,         tcp,        btlso::IoUtil::e_ASYNCHRONOUS }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES],
                                   sessionSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                   &serverSocket[i],
                                                   VALUES[i].d_type,
                                                   &errCode);
            LOOP_ASSERT(i, 0 == errCode);

            ret = btlso::IoUtil::setAsync(serverSocket[i],
                                         VALUES[i].d_option,
                                         &errCode);

            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                #ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, ASYNCH == result);
                #else
                LOOP_ASSERT(i, -1 == ret);
                #endif

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                                   serverSocket[i],
                                                   serverAddr,
                                                   &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));
                int portNum = localAddr.portNumber();
                btlso::SocketImpUtil::listen(serverSocket[i], BACKLOG,
                                            &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                pthread_t id;
                int status = 0;

                #ifdef BDE_BUILD_TARGET_MT
                    status = pthread_create(&id, 0, thread_as_tcp_client,
                                        (void*)&portNum);
                    LOOP_ASSERT(i, 0 == status);

                    btlso::SocketImpUtil::accept<btlso::IPv4Address>(
                                               &sessionSocket[i],
                                               serverSocket[i],
                                               &errCode);
                    LOOP_ASSERT(i, 0 == errCode);

                    status = pthread_join(id,NULL);
                    LOOP_ASSERT(i, 0 == status);
                #endif
                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);
                btlso::SocketImpUtil::close(sessionSocket[i],
                                           &errCode);
                sleep(1);
            }
            #else
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }
      } break;
      case 6: {
        // ---------------------------------------------------------------
        // TESTING 'setAsync' FUNCTION FOR TCP ASYNCHRONOUS SOCKETS:
        //   Set the specified socket to asynchronous mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The TCP socket is set to asynchronous, the others are same as
        //   "case 8".
        //
        // Testing
        //   int setAsync(handle, option, int *errorCode = 0);
        // ---------------------------------------------------------------
        btlso::IoUtil::AsyncMode result;
        int ret=0, errCode = 0;
        btlso::IPv4Address serverAddr, localAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        const int    BACKLOG      = 32;
        btlso::SocketImpUtil::Type tcp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;

        if (verbose) {
            QT("Testing 'setAsync' method");
            QT("=========================");
        }

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    // socket type.
          btlso::IoUtil::AsyncMode   d_option;  // option to set the mode.
        } VALUES[] =
        //  line     type                  option
        //  ----     ----                  ------
        {
          {   L_,     tcp,        btlso::IoUtil::e_SYNCHRONOUS}
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES],
                                    sessionSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                     &serverSocket[i],
                                     VALUES[i].d_type,
                                     &errCode);

            LOOP_ASSERT(i, 0 == errCode);

            ret = btlso::IoUtil::setAsync(serverSocket[i],
                                         VALUES[i].d_option,
                                         &errCode);
            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                #ifndef BSLS_PLATFORM_OS_CYGWIN
                ret = btlso::IoUtil::getAsync(&result,
                                             serverSocket[i],
                                             &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, SYNCH == result);
                #endif

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                               serverSocket[i],
                                               serverAddr,
                                               &errCode );
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));
                int portNum = localAddr.portNumber();
                btlso::SocketImpUtil::listen(serverSocket[i], BACKLOG,
                                            &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                pthread_t id;
                int status = 0;
                #ifdef BDE_BUILD_TARGET_MT
                    status = pthread_create(&id, 0, thread_as_tcp_client,
                                            (void*)&portNum);
                    LOOP_ASSERT(i, 0 == status);
                    btlso::SocketImpUtil::accept<btlso::IPv4Address>(
                                               &sessionSocket[i],
                                               serverSocket[i],
                                               &errCode);
                    LOOP_ASSERT(i, 0 == errCode);

                    status = pthread_join(id,0);
                    LOOP_ASSERT(i, 0 == status);
                #endif
                btlso::SocketImpUtil::close(serverSocket[i],
                                               &errCode);
                btlso::SocketImpUtil::close(sessionSocket[i],
                                               &errCode);
            }
            #else
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }
      } break;
      case 5: {
        // ----------------------------------------------------------------
        // TESTING 'setCloseOnExec' FUNCTION:
        //   Set the specified socket to close-on-exec mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The main concerns for this function are that:
        //     (1) If it's setting the right flag.
        //     (2) If it can turn off the close-on-exec mode for different
        //         kinds of sockets supported by other BDE components,
        //         e.g., btlso_socketimputil.
        //     (3) If it can turn off the close-on-exec mode for a socket
        //         of a given type when the user passes any non-zero
        //         integer option value.
        //   To resolve the above concerns, a server socket is created and
        //   set its the close-on-exec mode on or off respectively.  a
        //   child process is created to try binding a sever address with
        //   this socket handle.  The behavior how the call to bind()
        //   returns can be used to test if the mode is set correctly.
        //
        // Testing
        //   int setCloseOnExec(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------
        static int result=0;
        int ret=0, errCode = 0;

        if (verbose) {
            QT("Testing 'setCloseOnExec' method");
            QT("===============================");
        }

        btlso::SocketImpUtil::Type tcp, udp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;    // socket type.
          int                        d_option;  // option to set the mode.
        } VALUES[] =
        //line        type    option
        //----        ----    ------
        {
          { L_,        tcp,        0 },
          { L_,        udp,        0 },
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].d_type,
                                 &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                               VALUES[i].d_option,
                                               &errCode);
            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getCloseOnExec(&result,
                                                   serverSocket[i],
                                                   &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, 0 == result);

                {
                    enum { SIZE = 61 };
                    char arg[3][SIZE];

                    snprintf(arg[0], SIZE, "%s",argv[0]);
                    snprintf(arg[1], SIZE, "%d", -14);
                    snprintf(arg[2], SIZE, "%d",serverSocket[i]);

                    if (fork() == 0) {
                        if (execlp(arg[0], arg[0], arg[1], arg[2],
                                                        (char*)0)) {
                            PT(errno);
                        }
                    }
                    else {
                        int stat;
                        wait(&stat);
                        //close the socket.
                        btlso::SocketImpUtil::close(serverSocket[i],
                                                   &errCode);
                    }
                }
                sleep(1);
            }
            #else
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }
      } break;
      case 4: {
        // ----------------------------------------------------------------
        // TESTING 'setCloseOnExec' FUNCTION:
        //   Set the specified socket to close-on-exec mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The main concerns for this function are that
        //     (1) If it's setting the right flag.
        //     (2) If it can turn off the close-on-exec mode for different
        //         kinds of sockets supported by other BDE components,
        //         e.g.,  btlso_socketimputil.
        //     (3) If it can turn on the close-on-exec mode for a socket
        //         of a given type when the user passes any non-zero
        //         integer option value.
        //     (4) If it can turn on the close-on-exec mode for a socket
        //         of a given type whether a user passes an 'errorCode'
        //         argument or not.
        //     (5) If it can still set on/off the close-on-exec mode when
        //         other I/O modes have already been set.
        //     (6) The error code and return value are correctly returned.
        //   To resolve concerns (1) and (2), a server socket is created
        //   and set its the close-on-exec mode on or off respectively.
        //   a child process is created to try binding a sever address with
        //   this socket handle.  The behavior how the call to bind()
        //   returns can be used to test if the mode is set correctly.
        //   To resolve concerns (3), (4), (5) and (6), an array 'VALUES'
        //   of struct consisting of the socket type, option flag and
        //   the expected mode value.  Create a socket for each
        //   corresponding element in the array, make sure every socket's
        //   close-on-exec value matches with the expected mode
        //   value, also make sure for every setting operation, the
        //   function's return value and error code are set correctly.
        //
        // Testing
        //   int setCloseOnExec(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------
        static int result;
        int ret=0, errCode = 0, *errNull = NULL;
        btlso::SocketImpUtil::Type tcp, udp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        if (verbose) {
            QT("Testing 'setCloseOnExec' method");
            QT("===============================");
        }

        btlso::IPv4Address serverAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        struct {
          int                        d_lineNum;
          btlso::SocketImpUtil::Type d_type;   // socket type.
          int                       d_option; // set the mode.
        } VALUES[] =
        //line             type             option
        //----             ----             ------
        {
          { L_,            tcp,                 1 },
          { L_,            udp,                 1 }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                      &serverSocket[i],
                                                      VALUES[i].d_type,
                                                      &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                               VALUES[i].d_option,
                                               &errCode);
            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getCloseOnExec(&result,
                                                   serverSocket[i],
                                                   &errCode);
                LOOP_ASSERT(i, 0 == ret);
                LOOP_ASSERT(i, 1 == result);

                {
                    enum { SIZE = 61 };
                    char arg[3][SIZE];

                    snprintf(arg[0], SIZE, "%s",argv[0]);
                    snprintf(arg[1], SIZE, "%d", -13);
                    snprintf(arg[2], SIZE, "%d",serverSocket[i]);

                    if(fork() == 0) {
                        if(execlp(arg[0], arg[0], arg[1], arg[2],
                                                          (char*)0)) {
                            if (verbose) {
                                PT(errno);
                            }
                        }
                    }
                    else {
                        int stat;
                        wait(&stat);
                        // Close the socket.
                        btlso::SocketImpUtil::close(serverSocket[i],
                                                   &errCode);

                    }
                }
                sleep(1);
            }
            #else
                LOOP_ASSERT(i, 0 >= ret);
            #endif
        }

        if (verbose) {
            QT("Testing 'setCloseOnExec' method");
            QT("===============================");
        }

        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type d_type;    // socket type.
              int                        d_option;  // option to set mode.
            } VALUES[] =
            //line            type       option
            //----            ----       ------
            {
              { L_,           tcp,           1 },
              { L_,           udp,           1 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                     &serverSocket[i],
                                                     VALUES[i].d_type,
                                                     &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                   VALUES[i].d_option,
                                                   &errCode);
                #ifdef BSLS_PLATFORM_OS_UNIX
                {
                    LOOP_ASSERT(i, 0 == errCode);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                       serverSocket[i],
                                                       &errCode);
                    LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);

                    ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                   0, &errCode);
                    LOOP_ASSERT(i, 0 == errCode);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                   serverSocket[i],
                                                   &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errCode);

                    ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                       VALUES[i].d_option);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                       serverSocket[i],
                                                       &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 1 == result);

                    ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                    0, &errCode);
                    LOOP_ASSERT(i, 0 == errCode);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                       serverSocket[i],
                                                       &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errCode);
                    // Test when pass errNull
                    ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                       VALUES[i].d_option,
                                                       errNull);
                    LOOP_ASSERT(i, 0 == errNull);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                       serverSocket[i],
                                                       &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 1 == result);
                    // Set the flag when it's already on(set twice).
                    ret = btlso::IoUtil::setCloseOnExec(serverSocket[i],
                                                       VALUES[i].d_option,
                                                       errNull);
                    LOOP_ASSERT(i, 0 == errNull);
                    LOOP_ASSERT(i, 0 == ret);

                    ret = btlso::IoUtil::getCloseOnExec(&result,
                                                       serverSocket[i],
                                                       &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 1 == result);
                    // Close the socket.
                    btlso::SocketImpUtil::close(serverSocket[i],
                                               &errCode);

                }
                #else
                    LOOP_ASSERT(i, -1 == ret);
                #endif
            }
        }
      } break;
      case 3: {
        // ----------------------------------------------------------------
        // TESTING 'setBlockingMode' FUNCTION FOR UDP SOCKETS:
        //   Set the specified socket to the Blocking mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The main concerns for this function are that:
        //     (1) If it's setting the right flag.
        //     (2) If it can turn on/off the non-blocking mode for
        //         different kinds of sockets supported by other BDE
        //         components, e.g.,  btlso_socketimputil.
        //     (3) If it can turn on the non-blocking mode for a socket of
        //         a given type when the user passes any non-zero integer
        //         option value.
        //     (4) If it can turn on the non-blocking mode for a socket of
        //         a given type whether a user passes an 'errorCode'
        //         argument or not.
        //     (5) If it can still set on/off the non-blocking mode when
        //         other I/O modes have already been set.
        //     (6) The error code and return value are correctly returned.
        //   To resolve concerns (1) and (2), a server socket is created
        //   and set its the blocking mode on or off respectively.  Then a
        //   child process is created to make a connection with the
        //   creating process.  The behavior how the call to accept()
        //   return of the server can be used to test if the
        //   blocking/nonblocking mode is set correctly.
        //   To resolve concerns (3), (4), (5) and (6), an array 'VALUES'
        //   of struct consisting of the socket type, option flag and
        //   the expected mode value.  Create a socket for each
        //   corresponding element in the array, make sure every socket's
        //   blocking/non-blocking value matches with the expected mode
        //   value, also make sure for every setting operation, the
        //   function's return value and error code are set correctly.
        //
        // Testing
        //   int setBlockingMode(handle, option, int *errorCode = 0);
        // ---------------------------------------------------------------
        btlso::IoUtil::BlockingMode result;
        int ret=0, errCode = 0, *errNull = NULL;
        btlso::IPv4Address serverAddr, localAddr, clientAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        const int    RECEIVE_SIZE = 32;
        btlso::SocketImpUtil::Type udp;
        udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

        if (verbose) {
            QT("Testing 'setBlockingMode' method");
            QT("================================");
        }

        if (verbose) {
            QT("Testing 'setBlockingMode' for UDP sockets");
            QT("=========================================");
        }

        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type  type;    // socket type
              btlso::IoUtil::BlockingMode option;  // option to set mode
            } VALUES[] =
            //line          type               option
            //----          ----               ------
            {
              { L_,          udp,         btlso::IoUtil::e_NONBLOCKING}
            };

            char         readBuffer[RECEIVE_SIZE];
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                           &serverSocket[i],
                           VALUES[i].type,
                           &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].option,
                                                    &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                                      serverSocket[i],
                                                      serverAddr,
                                                      &errCode );
                LOOP_ASSERT(i, 0 == errCode);

                btlso::SocketImpUtil::readFrom(&clientAddr,
                                              readBuffer,
                                              serverSocket[i],
                                              RECEIVE_SIZE,
                                              &errCode);
                LOOP_ASSERT(i, errCode);
                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);

            }
        }

        if (verbose) {
            QT("Testing 'setBlockingMode' for UDP sockets");
            QT("=========================================");
        }

        struct {
          int                         d_lineNum;
          btlso::SocketImpUtil::Type  d_type;    //socket type.
          btlso::IoUtil::BlockingMode d_option;  //option to set mode.
        } VALUES[] =
        //line              type                     option
        //----              ----                     ------
        {
          { L_,              udp,        btlso::IoUtil::e_BLOCKING }
        };

        char         readBuffer[RECEIVE_SIZE];
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

        for (int i = 0; i < NUM_VALUES; i++) {
            errCode = 0;
            btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                     &serverSocket[i],
                                     VALUES[i].d_type,
                                     &errCode);
            LOOP_ASSERT(i, 0 == errCode);

            ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                VALUES[i].d_option,
                                                &errCode);
            LOOP_ASSERT(i, 0 == errCode);
            LOOP_ASSERT(i, 0 == ret);

            btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                                  serverSocket[i],
                                                  serverAddr,
                                                  &errCode );
            LOOP_ASSERT(i, 0 == errCode);

            LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));
            int portNum = localAddr.portNumber();

            #ifdef BSLS_PLATFORM_OS_UNIX
            {
                int status = 0;
                pthread_t id;

                #ifdef BDE_BUILD_TARGET_MT
                    status = pthread_create(&id, 0, thread_as_udp_client,
                                                       (void*)&portNum);
                    LOOP_ASSERT(i, 0 == status);
                    btlso::SocketImpUtil::readFrom(&clientAddr,
                                                  readBuffer,
                                                  serverSocket[i],
                                                  RECEIVE_SIZE,
                                                  &errCode);
                    LOOP_ASSERT(i, 0 == errCode);
                    status = pthread_join(id,NULL);
                    LOOP_ASSERT(i, 0 == status);
                #endif
            }
            #else  // Create windows thread
            {
                HANDLE t_handle;
                DWORD  portNumber = portNum;
                unsigned int t_id;
                t_handle = (HANDLE) _beginthreadex(NULL, 0,
                         thread_win_udp_client, (LPVOID)&portNumber,
                         0, &t_id);
                LOOP_ASSERT(i, NULL != t_handle);
                int rCode =0;     errCode = 0;
                rCode = btlso::SocketImpUtil::readFrom(&clientAddr,
                                                      readBuffer,
                                                      serverSocket[i],
                                                      RECEIVE_SIZE,
                                                      &errCode);
               LOOP_ASSERT(i, 0 < rCode);  LOOP_ASSERT(i, 0 == errCode);
               if (verbose) {
                   P_T("windows socket readFrom rCode: ");
                   P_T(rCode);  PT(errCode);
               }
               WaitForSingleObject(t_handle, INFINITE);
            }
            #endif

            btlso::SocketImpUtil::close(serverSocket[i],
                                       &errCode);

        }
        if (verbose) {
            QT("Testing 'setBlockingMode' for UDP sockets");
            QT("=========================================");
        }

        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type  d_type;    //socket type.
              btlso::IoUtil::BlockingMode d_option;  //option to set mode.
            } VALUES[] =
            //line            type                       option
            //----            ----                       ------
            {
              { L_,            udp,       btlso::IoUtil::e_NONBLOCKING }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                errCode = 0;

                #ifdef BSLS_PLATFORM_OS_UNIX
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].d_type,
                                 &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option,
                                                    &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);
                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);

#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif

                ret = btlso::IoUtil::setBlockingMode(
                                              serverSocket[i],
                                              btlso::IoUtil::e_BLOCKING,
                                              &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);   LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errCode);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif

                ret = btlso::IoUtil::setBlockingMode(
                                              serverSocket[i],
                                              btlso::IoUtil::e_BLOCKING,
                                              &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);   LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errCode);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif
                // test when pass errNull
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option,
                                                    errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif
                // set the flag when it's already on(set twice).
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option,
                                                    errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
#ifndef BSLS_PLATFORM_OS_CYGWIN
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);
#else
                LOOP_ASSERT(i, -1 == ret);
#endif
                // close the socket.
                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);

                #else
                LOOP_ASSERT(i, 0 >= ret);
                #endif
            }
        }
      } break;
      case 2: {
        // ----------------------------------------------------------------
        // TESTING 'setBlockingMode' FUNCTION FOR TCP SOCKETS:
        //   Set the specified socket to the Blocking mode if an option is
        //   not zero.  The function return 0 on success and -1 otherwise.
        //   If failed, the error code will be set to the
        //   platform-dependent error code if the valid 'errorCode'
        //   argument is passed.
        //
        // Plan:
        //   The main concerns for this function are that:
        //     (1) If it's setting the right flag.
        //     (2) If it can turn on/off the non-blocking mode for
        //         different kinds of sockets supported by other BDE
        //         components, e.g.,  btlso_socketimputil.
        //     (3) If it can turn on the non-blocking mode for a socket of
        //         a given type when the user passes any non-zero integer
        //         option value.
        //     (4) If it can turn on the non-blocking mode for a socket of
        //         a given type whether a user passes an 'errorCode'
        //         argument or not.
        //     (5) If it can still set on/off the non-blocking mode when
        //         other I/O modes have already been set.
        //     (6) The error code and return value are correctly returned.
        //   To resolve concerns (1) and (2), a server socket is created
        //   and set its the blocking mode on or off respectively.  Then a
        //   child process is created to make a connection with the
        //   creating process.  The behavior how the call to accept()
        //   return of the server can be used to test if the
        //   blocking/nonblocking mode is set correctly.
        //   To resolve concerns (3), (4), (5) and (6), an array 'VALUES'
        //   of struct consisting of the socket type, option flag and
        //   the expected mode value.  Create a socket for each
        //   corresponding element in the array, make sure every socket's
        //   blocking/non-blocking value matches with the expected mode
        //   value, also make sure for every setting operation, the
        //   function's return value and error code are set correctly.
        //
        // Testing
        //   setBlockingMode(handle, option, int *errorCode = 0);
        // ----------------------------------------------------------------

        btlso::IoUtil::BlockingMode result;
        int ret=0, errCode = 0, *errNull = NULL;
        btlso::IPv4Address serverAddr, localAddr;
        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);
        const int BACKLOG = 32;
        btlso::SocketImpUtil::Type tcp;

        tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;

        if (verbose) {
            QT("Testing 'setBlockingMode' method");
            QT("================================");
        }
        if (verbose) {
            QT("Testing 'setBlockingMode' for TCP sockets");
            QT("=========================================");
        }
        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type  d_type;    // socket type.
              btlso::IoUtil::BlockingMode d_option;  // set the mode
            } VALUES[] =
            //  line         type      option
            //  ----         ----      ------
            {
              {   L_,        tcp,      btlso::IoUtil::e_NONBLOCKING }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES],
                                       sessionSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                                    &serverSocket[i],
                                                    VALUES[i].d_type,
                                                    &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                     VALUES[i].d_option,
                                                     &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                   serverSocket[i],
                                   serverAddr,
                                   &errCode );
                LOOP_ASSERT(i, 0 == errCode);

                btlso::SocketImpUtil::listen(serverSocket[i], BACKLOG,
                                            &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                LOOP_ASSERT(i, 0 ==
                     btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]));

                btlso::SocketImpUtil::accept<btlso::IPv4Address>(
                                  &sessionSocket[i],
                                  serverSocket[i],
                                  &errCode);

                if (verbose) {
                    QT("accepted returned with error code: ");
                    PT(errCode);
                }
                LOOP_ASSERT(i, errCode);  // TCP non-blocking mode

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);
                btlso::SocketImpUtil::close(sessionSocket[i],
                                           &errCode);
            }
        }

        if (verbose) {
            QT("Testing 'setBlockingMode' for TCP sockets");
            QT("=========================================");
        }
        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type  d_type;    //socket type.
              btlso::IoUtil::BlockingMode d_option;  //option to set mode.
            } VALUES[] =
            //  line         type       option
            //  ----         ----       ------
            {
              {   L_,        tcp,       btlso::IoUtil::e_BLOCKING }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES],
                                       sessionSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                errCode = 0;
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].d_type,
                                 &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option,
                                                    &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                LOOP_ASSERT(i, 0 == ret);

                btlso::SocketImpUtil::bind<btlso::IPv4Address>(
                                          serverSocket[i],
                                          serverAddr,
                                          &errCode );
                LOOP_ASSERT(i, 0 == errCode);
                btlso::SocketImpUtil::listen(serverSocket[i], BACKLOG,
                                            &errCode);
                LOOP_ASSERT(i, 0 == errCode);

                btlso::SocketImpUtil::getLocalAddress(&localAddr,
                                                     serverSocket[i]);
                int portNum = localAddr.portNumber();

                #ifdef BSLS_PLATFORM_OS_UNIX
                {
                    #ifndef BSLS_PLATFORM_OS_CYGWIN
                    ret = btlso::IoUtil::getBlockingMode(&result,
                                                        serverSocket[i],
                                                        &errCode);
                    LOOP_ASSERT(i, 0 == ret);
                    LOOP_ASSERT(i, 0 == result);
                    #endif

                    int status = 0;
                    pthread_t id;

                    #ifdef BDE_BUILD_TARGET_MT
                        status = pthread_create(&id, 0,
                                                thread_as_tcp_client,
                                                (void*)&portNum);
                        LOOP_ASSERT(i, 0 == status);
                        btlso::SocketImpUtil::accept<btlso::IPv4Address>(
                                               &sessionSocket[i],
                                               serverSocket[i],
                                               &errCode);
                        LOOP_ASSERT(i, 0 == errCode);

                        status = pthread_join(id,NULL);
                        LOOP_ASSERT(i, 0 == status);
                    #endif
                }
                #else  // Create windows thread.
                {
                     HANDLE t_handle;
                     DWORD  portNumber = portNum;
                     unsigned int t_id;
                     t_handle = (HANDLE) _beginthreadex(NULL, 0,
                         thread_win_tcp_client, (LPVOID)&portNumber,
                         0, &t_id);
                     LOOP_ASSERT(i, NULL != t_handle);

                     btlso::SocketImpUtil::accept<btlso::IPv4Address>(
                                               &sessionSocket[i],
                                               serverSocket[i],
                                               &errCode);
                     LOOP_ASSERT(i, 0 == errCode);
                     WaitForSingleObject(t_handle, INFINITE);
                }
                #endif
                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errCode);
                btlso::SocketImpUtil::close(sessionSocket[i],
                                           &errCode);
            }
        }
        if (verbose) {
            QT("Testing 'setBlockingMode' for TCP sockets");
            QT("=========================================");
        }
        {
            struct {
              int                         d_lineNum;
              btlso::SocketImpUtil::Type  d_type;   //socket type.
              btlso::IoUtil::BlockingMode d_option; //option to set mode.
            } VALUES[] =
            //line            type                       option
            //----            ----                       ------
            {
              { L_,            tcp,       btlso::IoUtil::e_NONBLOCKING }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

            for (int i = 0; i < NUM_VALUES; i++) {
                errCode = 0;

                #if   defined(BSLS_PLATFORM_OS_UNIX) \
                   && !defined(BSLS_PLATFORM_OS_CYGWIN)
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                     &serverSocket[i],
                                     VALUES[i].d_type,
                                     &errCode);
                LOOP_ASSERT(i, 0 == errCode);
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                VALUES[i].d_option,
                                                &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                serverSocket[i],
                                                &errCode);
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);

                ret = btlso::IoUtil::setBlockingMode(
                                              serverSocket[i],
                                              btlso::IoUtil::e_BLOCKING,
                                              &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
                LOOP_ASSERT(i, 0 == ret);   LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errCode);

                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                VALUES[i].d_option);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);

                ret = btlso::IoUtil::setBlockingMode(
                                              serverSocket[i],
                                              btlso::IoUtil::e_BLOCKING,
                                              &errCode);
                LOOP_ASSERT(i, 0 == errCode);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                    serverSocket[i],
                                                    &errCode);
                LOOP_ASSERT(i, 0 == ret);   LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errCode);
                // Test when pass errNull
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                    VALUES[i].d_option,
                                                    errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                     serverSocket[i],
                                                     &errCode);
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);

                // Set the flag when it's already on(set twice).
                ret = btlso::IoUtil::setBlockingMode(serverSocket[i],
                                                     VALUES[i].d_option,
                                                     errNull);
                LOOP_ASSERT(i, 0 == errNull);  LOOP_ASSERT(i, 0 == ret);

                ret = btlso::IoUtil::getBlockingMode(&result,
                                                     serverSocket[i],
                                                     &errCode);
                LOOP_ASSERT(i, 0 == ret);  LOOP_ASSERT(i, 1 == result);
                // Close the socket.

                btlso::SocketImpUtil::close(serverSocket[i], &errCode);
                #else
                LOOP_ASSERT(i, 0 >= ret);
                #endif
            }
        }
      } break;
      case 1: {
        // ----------------------------------------------------------------
        // BREATHING TEST:
        //   Probe that functionality systematically and incrementally
        //   to discover basic errors in isolation.
        //
        // Plan:
        //   Open a socket and set it to different I/O modes via
        //   functions in this component.  Make sure the I/O mode value
        //   retrived match the expected value.
        //
        // Testing:
        //   This Test Case exercises general test of functionalities.
        // ----------------------------------------------------------------
        btlso::SocketHandle::Handle socketHandle;
        btlso::IoUtil::BlockingMode option = btlso::IoUtil::e_NONBLOCKING;
        btlso::IoUtil::BlockingMode result;
        int errCode=0, retCode=0;

        if (verbose) {
            QT("BREATHING TEST");
            QT("==============");
        }

        btlso::SocketImpUtil::open<btlso::IPv4Address>(
                         &socketHandle,
                         btlso::SocketImpUtil::k_SOCKET_STREAM,
                         &errCode);
        ASSERT(0 == errCode);

        retCode = btlso::IoUtil::setBlockingMode(socketHandle,
                                                 option,
                                                 &errCode);
        ASSERT(0 == retCode);   ASSERT(0 == errCode);

        #if   defined(BSLS_PLATFORM_OS_UNIX) \
           && !defined(BSLS_PLATFORM_OS_CYGWIN)

        retCode = btlso::IoUtil::getBlockingMode(&result,
                                                 socketHandle,
                                                 &errCode);
        ASSERT(0 == retCode);   ASSERT(1 == result);
        ASSERT(0 == errCode);

        // Set the flag back.
        option = btlso::IoUtil::e_BLOCKING;
        retCode = btlso::IoUtil::setBlockingMode(socketHandle,
                                                 option,
                                                 &errCode);
        ASSERT(0 == retCode);   ASSERT(0 == errCode);

        retCode = btlso::IoUtil::getBlockingMode(&result,
                                                 socketHandle,
                                                 &errCode);
        ASSERT(0 == retCode);   ASSERT(0 == result);
        ASSERT(0 == errCode);
        if (verbose)
            cout << "Exercising CLOSE-ON-EXEC flag." << endl;
        {
            int closeOnExec = 1, closeOn_flag;
            retCode = btlso::IoUtil::setCloseOnExec(socketHandle,
                                                    closeOnExec,
                                                    &errCode);
            ASSERT(0 == retCode);   ASSERT(0 == errCode);

            retCode = btlso::IoUtil::getCloseOnExec(&closeOn_flag,
                                                    socketHandle,
                                                    &errCode);
            ASSERT(0 == retCode);   ASSERT(1 == closeOn_flag);
            ASSERT(0 == errCode);

            // Set the flag back.
            closeOnExec = 0;
            retCode = btlso::IoUtil::setCloseOnExec(socketHandle,
                                                    closeOnExec,
                                                    &errCode);
            ASSERT(0 == retCode);   ASSERT(0 == errCode);

            retCode = btlso::IoUtil::getCloseOnExec(&closeOn_flag,
                                                    socketHandle,
                                                    &errCode);
            ASSERT(0 == retCode);   ASSERT(0 == closeOn_flag);
            ASSERT(0 == errCode);
        }

        #ifndef BSLS_PLATFORM_OS_CYGWIN
        if (verbose)
            cout << "Verifying the asynchronous mode." << endl;
        {
            btlso::IoUtil::AsyncMode value = ASYNCH, result;
            retCode = btlso::IoUtil::setAsync(socketHandle,
                                              value,
                                              &errCode);
            ASSERT(0 == retCode);   ASSERT(0 == errCode);

            retCode = btlso::IoUtil::getAsync(&result,
                                              socketHandle,
                                              &errCode);
            ASSERT(0 == retCode);   ASSERT(ASYNCH == result);
            ASSERT(0 == errCode);

            // Set the flag back.
            value = SYNCH;
            retCode = btlso::IoUtil::setAsync(socketHandle,
                                              value,
                                              &errCode);
            ASSERT(0 == retCode);   ASSERT(0 == errCode);

            retCode = btlso::IoUtil::getAsync(&result,
                                              socketHandle,
                                              &errCode);
            ASSERT(0 == retCode);   ASSERT(SYNCH == result);
            ASSERT(0 == errCode);
            btlso::SocketImpUtil::close(socketHandle, &errCode);
        }
        #endif
        #endif
      } break;
      case -14:      // case -14: when "closeOnExec" is off
      case -13: {    // case -13: when "closeOnExec" is on
        // ----------------------------------------------------------------
        // TESTING 'setCloseOnExec' FUNCTION:
        //   This is a helper case for testing 'setCloseOnExec' function,
        //   the main test case for this function is "case 4".
        //
        // Plan:
        //   In "case 4", each socket will be set the close-on-exec on,
        //   while in "case 5", each socket will be set the close-on-exec
        //   off.  The socket handles in "case 4 or 5"  will be passed to
        //   a child process created in "case 4 or 5" respectively, and
        //   the child process will run this specified either "case -14" or
        //   "case -13", trying binding the socket to the sever address.
        //   The return code from bind(), will be used to test if the right
        //   flag has been set correctly, if it is, the return value
        //   should be greater then zero.
        //
        // Testing:
        //   int setCloseOnExec(handle, option, *errorCode = 0);
        // ----------------------------------------------------------------

        int serverFd;
        int errCode=0;

        btlso::IPv4Address serverAddr;
        serverFd = argc > 2 ? atoi(argv[2]) : 0;

        serverAddr.setIpAddress(LOCALHOST);
        enum { DEFAULT_PORT_NUMBER = 0 };
        serverAddr.setPortNumber(DEFAULT_PORT_NUMBER);

        errCode = 0;

        btlso::SocketImpUtil::bind<btlso::IPv4Address>(serverFd,
                                                       serverAddr,
                                                       &errCode);
        switch (test) {
          case -14: {
            if (serverFd) {
                ASSERT(0 == errCode);
            }
            else {
                ASSERT(0 != errCode); // For all.pl test 14: no serverFd
            }
          } break;
          case -13: {
            ASSERT(0 != errCode);
          } break;
          default: {
            ASSERT("This statements must be unreachanble" && 0);
          } break;
        }
        btlso::SocketImpUtil::close(serverFd, &errCode);

      } break;
      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    errCode = 0;
    btlso::SocketImpUtil::cleanup(&errCode);
    ASSERT(0 == errCode);

    return testStatus;
}

#if defined(BTESO_POP_MACRO_ASYNCH)
#pragma pop_macro("ASYNCH")
#undef BTESO_POP_MACRO_ASYNCH
#endif

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
