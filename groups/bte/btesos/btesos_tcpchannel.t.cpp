// btesos_tcpchannel.t.cpp      -*-C++-*-

#include <btesos_tcpchannel.h>
#include <btesc_flag.h>

#include <bslma_testallocator.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <bteso_sockethandle.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>

#include <btes_iovecutil.h>

#include <bcemt_thread.h>
#include <bslma_testallocator.h>                // allocate memory
#include <bsls_platform.h>

#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_signal.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// The component under test provides concrete implementation of the
// synchronous communication channel ('btesc_channel') over TCP/IPv4 sockets.
// The basic plan for the testing is to ensure that all methods in
// the component work as expected.  By using the table-driven strategy, I/O
// requests on a channel will be decoded from the set of test data.  Each test
// data set is built based on both the 'black-box' concerns and code
// investigation ('white-box').  The purpose of each test data set is to
// resolve one or more concerns about that function.  For each test operation,
// the result will be compared to the expected one which is calculated
// beforehand.
//-----------------------------------------------------------------------------
// [ 1] btesos_TcpChannel()
// [ 1] ~btesos_TcpChannel()
// [ 2] int read(...)
// [ 4] int readv(...)
// [ 3] int readRaw(...)
// [ 5] int readvRaw(...)
// [ 6] int bufferedRead(...)
// [ 7] int bufferedReadRaw(...)
// [ 8] int write(...)
// [10] int writev(...)
// [ 9] int writeRaw(...)
// [11] int writevRaw(const btes_Ovec *buffers, ...)
// [11] int writevRaw(const btes_Iovec *buffers, ...)
// [ 1] void invalidate()
// [ 1] int isInvalid()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        printf("Error %s (%d): %s (failed)\n",  __FILE__, i, s);
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
bcemt_Mutex  d_mutex;   // for i/o synchronization in all threads

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();
//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btesos_TcpChannel Obj;
typedef bteso_StreamSocket<bteso_IPv4Address> StreamSocket;

static int verbose;
static int veryVerbose;

const char* HOST_NAME = "127.0.0.1";

enum {
    DEFAULT_PORT_NUMBER     =  0,
    DEFAULT_NUM_CONNECTIONS =  10,
    DEFAULT_EQUEUE_SIZE     =  5,
    SLEEP_TIME              =  100000
};

enum { // error code returned from I/O operation
    INTERRUPTED  =  1,
    CLOSED       = -1,
    INVALID      = -2,
    ERR          = -3
};

#ifdef BSLS_PLATFORM_OS_UNIX
const int MAX_BUF     = 99000;     // the biggest length of a buffer for write

    #if defined BSLS_PLATFORM_OS_AIX

        const int BUF_WRITE   = 8192;  // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 500;   // the last second buffer vector length
        const int BUF_LIMIT   = 1024;  // to set the snd/rcv buffer size
        const int HELPER_READ = 500;
        const int SYS_DEPENDENT_LEN = 8192;

    #elif defined(BSLS_PLATFORM_OS_LINUX)
        const int BUF_WRITE   = 20000; // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 1000;  // the last second vector buffer length
        const int BUF_LIMIT   = 4096;  // to set the snd/rcv buffer size
        const int HELPER_READ = 25000;
        const int SYS_DEPENDENT_LEN = 16000; // + BUF_WRITE MUST BE > 32768
        const int SYS_DEPENDENT_LEN2 = 5000; // + 7 vectors (24740) < 30000

    #else    // solaris

        const int BUF_WRITE   = 73680; // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 1000;  // the last second vector buffer length
        const int BUF_LIMIT   = 8192;  // to set the snd/rcv buffer size
        const int HELPER_READ = 35000;
        const int SYS_DEPENDENT_LEN = 20000;

    #endif
#else   // windows

    const int MAX_BUF     = 30000;
    const int BUF_LIMIT   = 8192;
    const int BUF_WRITE   = 28720; // the longest buffer length for ioVec/oVec
    const int BUF_WRITE2  = 1000;  // the last second buffer vector length
    const int HELPER_READ = 10000;
    const int SYS_DEPENDENT_LEN = 10000;
#endif

enum {
    MAX_CMD       = 50,
    WRITE_OP      = 2,
    READ_OP       = 3,
    HELPER_WRITE  = 100         // When an "AE" occurs during a 'read' which
                                // is in 'non-interruptible' mode, some data
                                // need to be written at the peer side for
                                // the read to get data from the connection.
                                // This means in THIS test driver, any read
                                // request for "AE" interrupt test should be
                                // waiting for less than "HELPER_WRITE" bytes
                                // if it's in 'non-interruptible' mode.
};

enum {
    MAX_VECBUF     = 12,
    VECBUF_LEN1    = 1,
    VECBUF_LEN2    = 2,
    VECBUF_LEN3    = 3,
    VECBUF_LEN4    = 4,
    VECBUF_LEN5    = 5,
    VECBUF_LEN6    = 6,
    VECBUF_LEN7    = 7,
    VECBUF_LEN8    = 8,
    VECBUF_LEN9    = 9,
    VECBUF_LEN10   = 10,
    VECBUF_LEN50   = 50,
    VECBUF_LEN90   = 90,

    WVECBUF_LEN1   = 1,
    WVECBUF_LEN20  = 20,
    WVECBUF_LEN40  = 40,
    WVECBUF_LEN60  = 60,
    WVECBUF_LEN80  = 80,
    WVECBUF_LEN8K  = 8192,
    WVECBUF_LEN16K = 16384,
    WVECBUF_LEN32K = 32768
};

struct ConnectInfo {
     // Use this struct to pass information to the helper thread.
     bcemt_ThreadUtil::Handle    d_tid;         // the id of the thread to
                                                // which a signal's delivered

     bteso_SocketHandle::Handle  d_socketHandle;

     int                         d_signals;        // the number of signals
                                                   // to be raised
     int                         d_signalIoFlag;   // if this flag is set,
       // meaning some on-going I/O operation is in 'non-interruptible' mode,
       // and so some data is needed to be writted at 'd_socketHandle' to
       // "let" the channel's I/O operation go.
};

// Test commands used to test 'btesos_TcpChannel':
enum {
    R              =  1,  //  read
    RA             =  2,  //  read with 'augStatus'
    RV             =  3,  //  readv
    RVA            =  4,  //  readv with 'augStatus'
    RR             =  5,  //  readRaw
    RRA            =  6,  //  readRaw with 'augStatus'
    RB             =  7,  //  bufferedRead
    RBA            =  8,  //  bufferedRead with 'augStatus'
    RVR            =  9,  //  readv
    RVRA           = 10,  //  readv with 'augStatus'
    RBR            = 11,  //  bufferedReadv
    RBRA           = 12,  //  bufferedReadv with 'augStatus'
    TR             = 13,  //  timedRead()
    TRA            = 14,  //  timedRead() with 'augStatus'
    TRV            = 15,  //  timedReadv
    TRVA           = 16,  //  timedReadv with 'augStatus'
    TRR            = 17,  //  timedReadRaw
    TRRA           = 18,  //  timedReadRaw with 'augStatus'
    TRB            = 19,  //  timedBufferedRead
    TRBA           = 20,  //  timedBufferedRead with 'augStatus'
    TRVR           = 21,  //  timedReadv
    TRVRA          = 22,  //  timedReadv with 'augStatus'
    TRBR           = 23,  //  timedBufferedReadv
    TRBRA          = 24,  //  timedBufferedReadv with 'augStatus'
    READ_FUNCTIONS = 30,  //  the number of read functions
    // The followings are "write" operations.
    W              = 35,  //  write
    WA             = 36,  //  write with 'augStatus'
    WR             = 37,  //  writeRaw
    WRA            = 38,  //  writeRaw with 'augStatus'
    WVO            = 39,  //  writev with 'btes_Iovec'
    WVOA           = 40,  //  writev with 'btes_Iovec' and 'augStatus'
    TWVO           = 41,  //  timedWritev with 'btes_Iovec'
    TWVOA          = 42,  //  timedWritev with 'btes_Iovec' and 'augStatus'
    WVRO           = 43,  //  writevRaw with 'btes_Ovec' and 'augStatus'
    WVROA          = 44,  //  writevRaw with 'btes_Ovec' and 'augStatus'
    TWVRO          = 45,  //  timedWritevRaw with 'btes_Ovec' and 'augStatus'
    TWVROA         = 46,  //  timedWritevRaw with 'btes_Ovec' and 'augStatus'
    WVI            = 47,  //  writev with 'btes_Ovec'
    WVIA           = 48,  //  writev with 'btes_Ovec' and 'augStatus'
    TWVI           = 49,  //  timedWritev with 'btes_Ovec'
    TWVIA          = 50,  //  timedWritev with 'btes_Ovec' and 'augStatus'
    WVRI           = 51,  //  writevRaw with 'btes_Iovec' and 'augStatus'
    WVRIA          = 52,  //  writevRaw with 'btes_Iovec' and 'augStatus'
    TWVRI          = 53,  //  timedWritevRaw with 'btes_Iovec' and 'augStatus'
    TWVRIA         = 54,  //  timedWritevRaw with 'btes_Iovec' and 'augStatus'
    TW             = 55,  //  timedWrite
    TWA            = 56,  //  timedWrite with 'augStatus'
    TWR            = 57,  //  timedWriteRaw
    TWRA           = 58,  //  timedWriteRaw with 'augStatus'
    WRITE_FUNCTIONS= 30,  //  the number of write functions
    // The followings are "helper" operations.
    HELP_READ      = 70,  //  read from the control endpoint of the channel
    HELP_WRITE     = 71,  //  write to the control endpoint of the channel
    HELP_WRITE2    = 72,  //  same as the above, but through a thread
    INVALIDATE     = 73,  //  invalidate the channel
    SLEEP          = 74,  //  sleep for the specified number of seconds
    SIGNAL         = 75,  //  create a thread generating a signal of "SIGSYS"
    CLOSE_CONTROL  = 76,  //  Create a thread to close the control(helper)
                          //  socket.
    CLOSE_OBSERVE  = 77,  //  Create a thread to close the observe socket.
    BUF_STAT       = 78   //  Check the internal buffer state.
};

struct TestCommand {
    // This struct includes needed information for each test operation,
    // e.g., a request to "read", "writev" etc., along with it's corresponding
    // test results.
    int d_lineNum;

    int d_command;         // command to invoke a indicated
                           // function specified above.
    union {
      int d_numBytes;      // bytes to be read/written
      int d_numBuffers;    // buffers for readv/writev
      int d_milliseconds;  // only for a "sleep" command
      int d_signals;       // only for thread generating signals
    } numToProcess;
    union {
      int d_interruptFlags; // interruptible if it's set to positive
      int d_signalIoFlag;   // 1 == read from the peer side
                            // 2 == write at the peer side
                            // 0 == no read or write
    } flag;
    int d_expReturnValue;  // the return value from this command execution,

    int d_expStatus;       // expected status value after this operation
};

char globalBuffer[HELPER_READ]; // To help read from the peer side.
bslma_TestAllocator testAllocator;
//=============================================================================
//                    HELPER FUNCTIONS/CLASSES FOR TESTING
//-----------------------------------------------------------------------------
extern "C" {

#ifdef BSLS_PLATFORM_OS_UNIX

static void signalHandler(int sig)
    // The signal handler does nothing.
{
    if (verbose) {
        if (sig == SIGPIPE) {
            write(2, "GOT A SIGPIPE signal.\n",
                    sizeof("GOT A SIGPIPE signal.\n"));
            write(2, "=====================\n",
                    sizeof("=====================\n"));
        }
        else if (sig == SIGSYS) {
            write(2, "GOT A SIGSYS signal.\n",
                    sizeof("GOT A SIGSYS signal.\n"));
            write(2, "=====================\n",
                    sizeof("=====================\n"));
        }
    }
    return;
}

static void registerSignal(int signo, void (*handler)(int) )
    // Register the signal handler for the signal 'signo' to be generated.
{
    struct sigaction act, oact;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(signo, &act, &oact) < 0) {
        perror("The signal handler can't be installed.");
    }
    return;
}

static void* threadSignalGenerator(void *arg)
    // Generate signal 'SIGSYS' and deliver it to a thread specified in 'arg'.
    // Note the test can only work on UNIX platforms since windows doesn't
    // support signal operations.
{
    ConnectInfo *threadInfo = (ConnectInfo*) arg;

    if (verbose) {
        PT(threadInfo->d_signals);
        PT(threadInfo->d_signalIoFlag);
    }

    bcemt_ThreadUtil::microSleep(5 * SLEEP_TIME);
                                    // The thread waits to make sure
                                    // the main thread is hanging in the
                                    // dispatch() call.

    for (int i = 0; i < threadInfo->d_signals; ++i) {
        pthread_kill(threadInfo->d_tid, SIGSYS);
        if (verbose) {
            P_T(bcemt_ThreadUtil::self());
            QT(" generated a SIGSYS signal.");
        }
        bcemt_ThreadUtil::microSleep(4 * SLEEP_TIME);
    }
    if (verbose) {
        PT(threadInfo->d_signalIoFlag);
    }
    if (threadInfo->d_signalIoFlag) {  // non-interruptible mode
        bcemt_ThreadUtil::microSleep(5 * SLEEP_TIME);
        if (WRITE_OP == threadInfo->d_signalIoFlag) {
            char buf[HELPER_WRITE];
            memset(buf, 'x', HELPER_WRITE);    // to keep purify happy

            int len = bteso_SocketImpUtil::write(threadInfo->d_socketHandle,
                                                 buf,
                                                 sizeof buf);

            if (veryVerbose) {
                PT(bcemt_ThreadUtil::self());
                QT(" writes to socket: ");
                P_T(len);  PT(threadInfo->d_socketHandle);
            }
            if (HELPER_WRITE != len) {
                if (veryVerbose) {
                    P_T(bcemt_ThreadUtil::self());
                    QT("Error: Failed in writing right number of bytes"
                       " to the socket:");
                    PT(threadInfo->d_socketHandle);
                }
                ASSERT(0);
            }
        }
        else if (READ_OP == threadInfo->d_signalIoFlag) {
            int len = bteso_SocketImpUtil::read(globalBuffer,
                                                threadInfo->d_socketHandle,
                                                HELPER_READ);
#ifdef BSLS_PLATFORM_OS_LINUX
                // kludge for Linux since the buffer is 16K and Window 32K
            if (len < HELPER_READ && len == 16383) {
                len = bteso_SocketImpUtil::read(globalBuffer,
                                            threadInfo->d_socketHandle,
                                            HELPER_READ - 16383);
                len += len >= 0 ? 16383 : 0;
            }
#endif
            if (veryVerbose) {
                P_T(bcemt_ThreadUtil::self());
                QT(" reads from socket: ");
                P_T(len);  PT(threadInfo->d_socketHandle);
            }
#ifndef BSLS_PLATFORM_OS_LINUX
            if (HELPER_READ != len) {
                if (veryVerbose) {
                    PT(bcemt_ThreadUtil::self());
                    QT("Error: Failed in reading right number of bytes"
                       " from the socket:");
                    PT(threadInfo->d_socketHandle);
                }
                ASSERT(0);
            }
#endif
        }
    }

    bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    if (veryVerbose) {
        QT("Signal generator thread exits now.");
    }
    testAllocator.deallocate(threadInfo);
    return 0;
}
#endif

static void* threadToCloseSocket(void *arg)
{

    ConnectInfo *threadInfo = (ConnectInfo*) arg;
    if (verbose) {
        QT("thread to close the socket: ");
        PT(threadInfo->d_socketHandle);
    }
#ifdef BSLS_PLATFORM_OS_LINUX
    //bcemt_ThreadUtil::microSleep(1 * SLEEP_TIME);
#else
    bcemt_ThreadUtil::microSleep(7 * SLEEP_TIME);
#endif

    int ret = bteso_SocketImpUtil::close(threadInfo->d_socketHandle);

    ASSERT(0 == ret);

    if (verbose) {
        QT("ThreadToCloseSocket exits.");
    }
    testAllocator.deallocate(threadInfo);
    return 0;
}

}  // end of extern "C"

static int buildChannelHelper(
                 bteso_SocketHandle::Handle                       *socketPair,
                 bteso_InetStreamSocketFactory<bteso_IPv4Address> *factory,
                 bteso_StreamSocket<bteso_IPv4Address>           **sSocket)
    // Create a pair of sockets which are connected to each other and load
    // them into the specified 'socketPair'.  Create a
    // 'bteso_InetStreamSocketFactory' object and load it into 'sSocket'.
    // Return 0 on success, and nonzero otherwise.
{
    int ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                                     socketPair,
                                     bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[0],
                                         bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                         bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                         1);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[1],
                                         bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                         bteso_SocketOptUtil::BTESO_TCPNODELAY,
                                         1);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[0],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        BUF_LIMIT);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[0],
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                                      BUF_LIMIT);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[1],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER,
                                        BUF_LIMIT);
    ASSERT(0 == ret);

    ret = bteso_SocketOptUtil::setOption(socketPair[1],
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,
                                      BUF_LIMIT);
    ASSERT(0 == ret);
    int sndBufferSize = 0, rcvBufferSize = 0;
    ret = bteso_SocketOptUtil::getOption(&sndBufferSize, socketPair[0],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_SENDBUFFER);
    if (0 != ret) {
        return ret;
    }
    ret = bteso_SocketOptUtil::getOption(&rcvBufferSize, socketPair[1],
            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
            bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
    if (0 != ret) {
        return ret;
    }
    if (verbose) {
        P_T(sndBufferSize);
        PT(rcvBufferSize);
    }
    *sSocket = factory->allocate(socketPair[0]);
    ASSERT(sSocket);
    return ret;
}

template <class VECBUFFER>
static inline
void fillBuffers(VECBUFFER *vecBuffers, int numBuffers, char ch)
{
    for (int i = 0; i < numBuffers; ++i) {
        memset((char*)vecBuffers[i].buffer(), ch, vecBuffers[i].length());
    }
}

static int testExecutionHelper(
                      btesos_TcpChannel                     *channel,
                      TestCommand                           *command,
                      bteso_SocketHandle::Handle            *socketPair,
                      bcemt_ThreadUtil::Handle              *threadHandle,
                      char                                  *buffer,
                      const btes_Iovec                      *ioBuffer,
                      const btes_Ovec                       *oBuffer,
                      int                                   *idx)
    // Process the specified 'command' to invoke some operation of the
    // specified 'channel', or the 'helperSocket' which is the control
    // endpoint of the socket pair.  For a read operation, load either
    // 'buffer' or 'ioBuffer' corresponding to the request.  For a write
    // operation, write data from either 'buffer' or 'ioBuffer' or 'oBuffer'
    // corresponding to the request.  Return the return value of the
    // operation on success, and 0 if the called function returns nothing.
{
    int rCode = 0;

    switch (command->d_command) {
    case R: {   //
        rCode = channel->read(buffer,
                              command->numToProcess.d_numBytes,
                              command->flag.d_interruptFlags);
    } break;
    case RA: {  //
        int augStatus = 0;
        rCode = channel->read(&augStatus,
                              buffer,
                              command->numToProcess.d_numBytes,
                              command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case RV: {  //
        rCode = channel->readv(ioBuffer,
                               command->numToProcess.d_numBuffers,
                               command->flag.d_interruptFlags);
    } break;
    case RVA: { //
        int augStatus = 0;
        rCode = channel->readv(&augStatus,
                               ioBuffer,
                               command->numToProcess.d_numBuffers,
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case RR: {  //
        rCode = channel->readRaw(buffer,
                                 command->numToProcess.d_numBytes,
                                 command->flag.d_interruptFlags);
    } break;
    case RRA: {  //
        int augStatus = 0;
        rCode = channel->readRaw(&augStatus,
                                 buffer,
                                 command->numToProcess.d_numBytes,
                                 command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case RB: {  //
        buffer = 0;
        rCode = channel->bufferedRead((const char**) &buffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        if (command->d_expStatus > 0) {
             ASSERT(0 == buffer);
        }
    } break;
    case RBA: {  //
        int augStatus = 0;
        buffer = 0;
        rCode = channel->bufferedRead(&augStatus,
                                      (const char**) &buffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (augStatus != command->d_expStatus) {
            if (verbose) {
                P_T(augStatus);
                PT(command->d_expStatus);
            }
        }
        if (command->d_expStatus > 0) {
             ASSERT(0 == buffer);
        }
    } break;
    case RVR: {  //
        rCode = channel->readvRaw(ioBuffer,
                                  command->numToProcess.d_numBuffers,
                                  command->flag.d_interruptFlags);
    } break;
    case RVRA: { //
        int augStatus = 0;
        rCode = channel->readvRaw(&augStatus,
                                  ioBuffer,
                                  command->numToProcess.d_numBuffers,
                                  command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case RBR: {  //
        buffer = 0;
        rCode = channel->bufferedReadRaw((const char**) &buffer,
                                         command->numToProcess.d_numBytes,
                                         command->flag.d_interruptFlags);
    } break;
    case RBRA: {  //
        int augStatus = 0;
        buffer = 0;
        rCode = channel->bufferedReadRaw(&augStatus,
                                         (const char**) &buffer,
                                         command->numToProcess.d_numBytes,
                                         command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case W: {    //
        rCode = channel->write(buffer,
                               command->numToProcess.d_numBytes,
                               command->flag.d_interruptFlags);
    } break;
    case WA: {   //
        int augStatus = 0;
        rCode = channel->write(&augStatus,
                               buffer,
                               command->numToProcess.d_numBytes,
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (augStatus != command->d_expStatus) {
            if (verbose) {
                P_T(augStatus);
                PT(command->d_expStatus);
            }
        }
    } break;
    case WR: {   //
        rCode = channel->writeRaw(buffer,
                                  command->numToProcess.d_numBytes,
                                  command->flag.d_interruptFlags);
    } break;
    case WRA: {  //
        int augStatus = 0;
        rCode = channel->writeRaw(&augStatus,
                                  buffer,
                                  command->numToProcess.d_numBytes,
                                  command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case WVO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->writev(oBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
    } break;
    case WVI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->writev(ioBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
    } break;
    case WVOA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->writev(&augStatus,
                                oBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case WVIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->writev(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case WVRO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->writevRaw(oBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
    } break;
    case WVRI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->writevRaw(ioBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
    } break;
    case WVROA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->writevRaw(&augStatus,
                                   oBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case WVRIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->writevRaw(&augStatus,
                                   ioBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case HELP_READ: {   //
        rCode = bteso_SocketImpUtil::read( buffer,
                                           socketPair[1],
                                           command->numToProcess.d_numBytes);
        ASSERT(rCode == command->numToProcess.d_numBytes);
        if (verbose) {
            QT("help socket read bytes: ");  PT(rCode);
        }
    } break;
    case HELP_WRITE: {   //
        char ch = 'x';  memset(buffer, ch, command->numToProcess.d_numBytes);
        rCode = bteso_SocketImpUtil::write(socketPair[1],
                                           buffer,
                                           command->numToProcess.d_numBytes);
        if (verbose) {
            QT("help socket write bytes: ");     PT(rCode);
        }
    } break;
    case INVALIDATE: {    //
        channel->invalidate();
        rCode = 0;
    } break;
    case SLEEP: {    //
        bcemt_ThreadUtil::microSleep(command->numToProcess.d_milliseconds);
        rCode = 0;
    } break;

    #ifdef BSLS_PLATFORM_OS_UNIX
    case SIGNAL: {
        // Create a thread to generate signals.
        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();
        if (verbose) {
            PT(command->flag.d_signalIoFlag);
        }
        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_signalIoFlag;

        bcemt_Attribute attributes;
        int ret = bcemt_ThreadUtil::create(threadHandle,
                                           attributes,
                                           threadSignalGenerator,
                                           info);
        ASSERT(0 == ret);
        if (ret) {
            if (verbose) {
                QT("Thread creation failed, return value: ");
                PT(ret);
            }
        }
        ++(*idx);
#ifdef BSLS_PLATFORM_OS_LINUX
        // do not race with the signaling thread
        bcemt_ThreadUtil::microSleep(2 * SLEEP_TIME);
#endif
    } break;
    #endif

    case CLOSE_CONTROL: {
        // Create a thread to close the control socket (the peer socket).
        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

        ConnectInfo *info = 0;
        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;

        bcemt_Attribute attributes;
        int ret = bcemt_ThreadUtil::create(threadHandle,
                                           attributes,
                                           threadToCloseSocket,
                                           info);
        ASSERT(0 == ret);
        if (ret) {
            if (verbose) {
                QT("Thread creation failed, return value: ");
                PT(ret);
            }
        }
        ++(*idx);
#ifdef BSLS_PLATFORM_OS_LINUX
        // do not race with the closer thread
        bcemt_ThreadUtil::microSleep(7 * SLEEP_TIME);
#endif
    } break;
    case CLOSE_OBSERVE: {
        // To close the channel socket to simulate "hard" errors.
        // int ret = bteso_SocketImpUtil::close(socketPair[0]);
        //  ASSERT(0 == ret);

        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

        ConnectInfo *info = 0;
        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[0];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;

        bcemt_Attribute attributes;
        int ret = bcemt_ThreadUtil::create(threadHandle,
                                           attributes,
                                           threadToCloseSocket,
                                           info);
        ASSERT(0 == ret);
        if (ret) {
            if (verbose) {
                QT("Thread creation failed, return value: ");
                PT(ret);
            }
        }
        ++(*idx);
#ifdef BSLS_PLATFORM_OS_LINUX
        // do not race with the closer thread
        bcemt_ThreadUtil::microSleep(7 * SLEEP_TIME);
#endif
        //bcemt_ThreadUtil::microSleep(SLEEP_TIME);

    } break;
    default:
        ASSERT("The test command is wrong!" && 0);
        break;
    }
    if (veryVerbose) {
        QT("helperExec ret = ");    PT(rCode);
    }
    return rCode;
}

static
int processTest(btesos_TcpChannel                *channel,
                bteso_SocketHandle::Handle       *helperSocket,
                TestCommand                      *commands,
                char                             *buffer,
                btes_Iovec                       *ioBuffer,
                btes_Ovec                        *oBuffer)
    // The specified 'numCommands' of test commands will be issued in the
    // specified 'commands' to invoke functions in the specified 'channel',
    // or the 'helperSocket' which is the control part of the socket pair.
    // If the 'signals' is set, a thread taking the specified 'threadFunction'
    // function will be generated to generate signals.  Results after each
    // test will be compared against those expected which are also specified
    // in the specified 'commands'.  For a read operation, load either
    // 'buffer' or 'ioBuffer' corresponding to the request.  For a write
    // operation, write data from either 'buffer' or 'ioBuffer' or 'oBuffer'
    // corresponding to the request.  Return 0 on success, and a non-zero
    // value otherwise.
{
    bcemt_ThreadUtil::Handle threadHandle[MAX_CMD];
    int ret = 0, idx = 0, numErrors = 0;

    for (int i = 0; i < MAX_CMD; i++) { // different test data
        if (commands[i].d_command < 1) {
            for (int j=0; j < idx; ++j) {
                bcemt_ThreadUtil::join(threadHandle[j]);
            }
            // bcemt_ThreadUtil::microSleep(20 * SLEEP_TIME);
            break;
        }
        ret = testExecutionHelper(channel,
                                  &commands[i],
                                  helperSocket,
                                  &threadHandle[idx],
                                  buffer,
                                  ioBuffer,
                                  oBuffer,
                                  &idx);
        if (verbose) {
            QT("In processTest:");  P_T(i);  PT(ret);
        }
        if (WR != commands[i].d_command && WRA != commands[i].d_command) {
            if (INTERRUPTED == commands[i].d_expStatus) {
                #ifdef BSLS_PLATFORM_OS_LINUX
                LOOP_ASSERT(commands[i].d_lineNum, ret >= 0);
                #else
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0);
                #endif
            }
            else {
                LOOP_ASSERT(commands[i].d_lineNum,
                            ret == commands[i].d_expReturnValue);
            }
            if (ret != commands[i].d_expReturnValue) {
                P_T(ret);
                PT(commands[i].d_expReturnValue);
            }
        }
        else {
            if (ret < 0) {
                LOOP_ASSERT(commands[i].d_lineNum,
                            ret == commands[i].d_expReturnValue);
            }
            else {
                LOOP_ASSERT(commands[i].d_lineNum,
                        ret > 0 && ret <= commands[i].numToProcess.d_numBytes);
            }
        }
        if (veryVerbose) {
            P_T(ret);
            PT(commands[i].d_expReturnValue);
        }
        // bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    }
    // bcemt_ThreadUtil::join(threadHandle);

    return numErrors;
}

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2; // global variable
    veryVerbose = argc > 3; // global variable
    int veryVeryVerbose = argc > 4;
    int errCode = 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BSLS_PLATFORM_OS_UNIX
    registerSignal(SIGPIPE, signalHandler); // register a handler for SIGPIPE.
    registerSignal(SIGSYS, signalHandler); // register a handler for SIGSYS.
    // A write() on the closed socket will generate SIGPIPE.
#endif

    testAllocator.setNoAbort(1);
    testAllocator.setVerbose(veryVeryVerbose);

    bteso_SocketImpUtil::startup(&errCode);

    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
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
        // --------------------------------------------------------------------
        if (verbose) {
            QT("USAGE EXAMPLE");
            QT("=============");
        }
        {
            // First, a pair of sockets connecting each other on the local
            // host are created for our example, which could be any connected
            // sockets on different hosts.  The channel only needs one of the
            // socket as its I/O request endpoint, while the other end of
            // connection will be used to write some data for the channel to
            // read:
            bteso_SocketHandle::Handle handles[2];
            int ret = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
                                     handles,
                                     bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
            ASSERT(0 == ret);
            // The following socket options are set only if necessary.

            ret = bteso_SocketOptUtil::setOption(handles[0],
                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                        bteso_SocketOptUtil::BTESO_SENDBUFFER, 8192);
            ASSERT(0 == ret);

            ret = bteso_SocketOptUtil::setOption(handles[1],
                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                        bteso_SocketOptUtil::BTESO_RECEIVEBUFFER, 8192);
            ASSERT(0 == ret);

            ret = bteso_SocketOptUtil::setOption(handles[1],
                            bteso_SocketOptUtil::BTESO_TCPLEVEL,
                            bteso_SocketOptUtil::BTESO_TCPNODELAY, 1);
            ASSERT(0 == ret);
            bteso_InetStreamSocketFactory<bteso_IPv4Address>
                                                  factory(&testAllocator);
            bteso_StreamSocket<bteso_IPv4Address> *sSocket =
                                              factory.allocate(handles[0]);
            ASSERT(sSocket);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket'
                // destructor, the behavior is undefined otherwise.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket'
                // object outside the block as above.

                Obj channel(sSocket);
                ASSERT(0 == channel.isInvalid());

                // Write data at the other side of the channel and so "read"
                // operations can be done at the channel side.
                enum { LEN = 30 };
                char writeBuf[LEN] = "abcdefghij1234567890",
                     readBuf[LEN];
                int numBytes = 0, augStatus = -1, interruptFlag = 1;
                int len = bteso_SocketImpUtil::write(handles[1], writeBuf,
                                               strlen(writeBuf));

                ASSERT(len == (int)strlen(writeBuf));
                // Read 5 bytes from the channel.
                numBytes = 5;
                augStatus = -1;
                len = channel.read(&augStatus, readBuf,
                                   numBytes, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 < augStatus);
                }

                numBytes = 10;
                augStatus = -1;
                len = channel.readRaw(&augStatus, readBuf,
                                      numBytes, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 < augStatus);
                }

                // Write 1 byte to the channel.
                numBytes = 1;
                augStatus = -1;
                len = channel.write(&augStatus, writeBuf,
                                    numBytes, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 < augStatus);
                }

                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes to the channel with a timeout value.
                len = channel.writeRaw(&augStatus, writeBuf,
                                 numBytes, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 < augStatus);
                }
                ASSERT(0 == channel.isInvalid());
                channel.invalidate();
                ASSERT(1 == channel.isInvalid());

                numBytes = 5;
                enum { INVALID = -2 };
                // Try writing 5 bytes from the channel.
                len = channel.read(&augStatus, readBuf,
                                   numBytes, interruptFlag);
                ASSERT(INVALID == len);

                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes from the channel with a timeout value.
                len = channel.read(&augStatus, readBuf,
                                   numBytes, interruptFlag);
                ASSERT(INVALID == len);
                // Try writing 1 byte to the channel.
                numBytes = 1;
                augStatus = -1;
                len = channel.write(&augStatus, writeBuf,
                                    numBytes, interruptFlag);
                ASSERT(INVALID == len);

                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes to the channel with a timeout value.
                len = channel.writeRaw(&augStatus, writeBuf,
                                 numBytes, interruptFlag);
                ASSERT(INVALID == len);
            }
            factory.deallocate(sSocket);
        }
      } break;
      case 11: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // -------------------------------------------------------------------
        // TESTING 'writevRaw' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no write operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     4. write the expected number of bytes to the channel if
        //        enough space available in the channel;
        //     5. return after an "AE" occurs if the "write" is in
        //        'interruptible' mode, and the return value for 'augstatus'
        //        should be > 0;
        //     6. keep trying writing expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected to each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Three steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "writing".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying writing.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 6)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int writevRaw(btes_Ovec  *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(btes_Iovec *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(int        *augStatus,
        //                 btes_Ovec  *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(int        *augStatus,
        //                 btes_Iovec *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("TESTING 'writevRaw' METHOD");
            QT("==========================");
        }

        {
            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            #if defined (BSLS_PLATFORM_OS_SOLARIS)
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRO,          1,          0,           1,         0   },
              {L_,     W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,  WVRO,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_,  WVRO,           2,         0,        INVALID,      0   },
              {L_,  WVRI,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRI,          1,          0,           1,         0   },
              {L_,     W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,  WVRI,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_,  WVRI,           2,         0,        INVALID,      0   },
              {L_,  WVRO,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            // Command set 3: Close the channel from the peer side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, WVROA,          1,          0,           1,         0   },
              {L_,     W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,  WVRO,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent  write operations will not succeed any
                // more.
              {L_, WVROA,           2,         0,        INVALID,      0   },
              {L_, WVRIA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel from the peer side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, WVRIA,          1,          0,           1,         0   },
              {L_,     W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_, WVRIA,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_, WVRIA,           2,         0,        INVALID,      0   },
              {L_, WVROA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 5: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRO,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  WVRO,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent  write operations will not succeed any
                // more.
              {L_,  WVRO,           2,         0,        INVALID,      0   },
              {L_,  WVRI,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 6: Close the channel at the channel side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  WVRI,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_,  WVRI,           2,         0,        INVALID,      0   },
              {L_,  WVRO,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 7: Close the channel at the channel side to
            // test the behavior of the write method w/ the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, WVROA,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, WVROA,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_, WVROA,           2,         0,        INVALID,      0   },
              {L_, WVRIA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 8: Close the channel at the channel side to
            // test the behavior of the write method w/ the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, WVRIA,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_, WVRIA,           2,         0,        INVALID,      0   },
              {L_, WVROA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif
            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRO,           1,         0,           1,         0   },
              {L_,  WVRO,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,  WVRI,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, WVROA,           1,         0,           1,         0   },
              {L_, WVROA,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_, WVRIA,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #else                                       // unix test data
            #ifdef BSLS_PLATFORM_OS_SOLARIS

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  WVRO,           7,         1,        8192,         0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRO,           8,         1,       40960,         0   },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  WVRI,           7,         1,        8192,         0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRI,           8,         1,       40960,         0   },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, WVROA,           7,         1,        8192,         0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVROA,           8,         1,       40960,         0   },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, WVRIA,           7,         1,        8192,         0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVRIA,           8,         1,       40960,         0   },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  WVRO,           7,          1,        8192,        0   },
              {L_,  SIGNAL,         2,    READ_OP,           0,        0   },
              {L_,   WVO,           7,          0,       24740,        0   },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVRI,          7,           1,        8192,         0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,   WVI,          7,           0,      24740,          0   },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVROA,         7,           1,        8192,         0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,  WVOA,          7,           0,      24740,          0   },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVRI,          7,           1,        8192,         0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,  WVIA,          7,           0,      24740,          0   },
            },
            #elif defined(BSLS_PLATFORM_OS_LINUX)
#if 0
            // commands set 1: to resolve concern 4 - 6.
            {
              {L_,  WVRO,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN,  0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_,  WVRO,           6,         1,        8356,         0   },
            },
            {
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN,  0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_,  WVRI,           6,         1,        8356,         0   },
            },
            {
              {L_, WVROA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_, WVROA,           6,         1,        8356,         0   },
            },
            {
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_, WVRIA,           6,         1,         8356,         0   },
            },
            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA, SYS_DEPENDENT_LEN2,   0, SYS_DEPENDENT_LEN2, 0   },
              {L_,  WVRO,           7,          1,        16383,        0   },
              {L_,  SIGNAL,         8,          0,           0,        0   },
              {L_,   WVO,           7,          0,       24740,        0   },
            },
#endif

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRO,           1,         0,           1,          0   },
              {L_,    W, SYS_DEPENDENT_LEN,   1, SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        8,      READ_OP,        0,          0   },
              {L_, WVRO,           6,         0,        8356,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRI,           1,         0,           1,          0   },
              {L_,    W,  SYS_DEPENDENT_LEN,  1,   SYS_DEPENDENT_LEN,  0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        8,      READ_OP,        0,          0   },
              {L_, WVRI,           6,         0,        8356,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVROA,          1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,  SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        8,      READ_OP,        0,          0   },
              {L_, WVROA,          6,         0,        8356,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRIA,          1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN2, 1, SYS_DEPENDENT_LEN2,  0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        8,      READ_OP,        0,          0   },
              {L_, WVRIA,          6,         0,        8356,          0   },
            },
            #else
            // commands set 1: to resolve concern 4 - 6.
            {
              {L_,  WVRO,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN,  0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRO,           6,         1,        1024,         0   },
            },
            {
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN,  0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRI,           6,         1,        1024,         0   },
            },
            {
              {L_, WVROA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVROA,           6,         1,        1024,         0   },
            },
            {
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,   0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVRIA,           6,         1,        1024,         0   },
            },
            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  WVRO,           7,          1,        1024,        0   },
              {L_,  SIGNAL,         2,          0,           0,        0   },
              {L_,   WVO,           7,          0,       24740,        0   },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRO,           1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_, WVRO,           6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRI,           1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_, WVRI,           6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVROA,          1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_, WVROA,          6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, WVRIA,          1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_, WVRIA,          6,         0,        1024,          0   },
            },
            #endif

            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[WVECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     buf4[WVECBUF_LEN80] = "\0", buf5[WVECBUF_LEN8K] = "\0",
                     buf6[WVECBUF_LEN16K] = "\0", buf7[WVECBUF_LEN32K] = "\0";

                btes_Ovec ovecBuffer[MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btes_Iovec iovecBuffer[MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, iovecBuffer, ovecBuffer) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
#endif
      } break;
      case 10: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // -------------------------------------------------------------------
        // TESTING 'writev' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no write operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     4. write the expected number of bytes to the channel if
        //        enough space available in the channel;
        //     5. return after an "AE" occurs if the "write" is in
        //        'interruptible' mode, and the return value for 'augstatus'
        //        should be > 0;
        //     6. keep trying writing expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected to each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Three steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "writing".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying writing.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 6)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int writev(btes_Ovec  *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(btes_Iovec *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(int        *augStatus,
        //              btes_Ovec  *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(int        *augStatus,
        //              btes_Iovec *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("TESTING 'writev' METHOD");
            QT("=======================");
        }

        {
            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            #if defined (BSLS_PLATFORM_OS_UNIX)
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVO,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   WVO,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WVO,           2,         0,        INVALID,      0   },
              {L_,   WVI,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   WVI,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WVI,           2,         0,        INVALID,      0   },
              {L_,   WVO,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 3: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVOA,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,  WVOA,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,  WVOA,           2,         0,        INVALID,      0   },
              {L_,  WVIA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,  WVIA,           7,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,  WVIA,           2,         0,        INVALID,      0   },
              {L_,  WVOA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif
            #if defined (BSLS_PLATFORM_OS_SOLARIS)
            // Command set 5: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVO,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,   WVO,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WVO,           2,         0,        INVALID,      0   },
              {L_,   WVI,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 6: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,   WVI,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WVI,           2,         0,        INVALID,      0   },
              {L_,   WVO,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 7: Close the channel at the channel side to
            // test the behavior of the 'read' method w/ the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVOA,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  WVOA,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,  WVOA,           2,         0,        INVALID,      0   },
              {L_,  WVIA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 8: Close the channel at the channel side to
            // test the behavior of the 'read' method w/ the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  WVIA,           7,         0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,  WVIA,           2,         0,        INVALID,      0   },
              {L_,  WVOA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           7,         0,       24740,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #else                                       // unix test data

            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // commands set 9: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,   WVO,           7,         1,        8192, INTERRUPTED },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           5,         0,         164,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,   WVI,           7,         1,        8192, INTERRUPTED },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           5,         0,         164,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,  WVOA,           7,         1,        8192, INTERRUPTED },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           5,         0,         164,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,  WVIA,           7,         1,        8192, INTERRUPTED },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,          0   },
              {L_,   WVO,           7,          0,      24740,          0   },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,   WVI,          7,           0,      24740,          0   },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,  WVOA,          7,           0,      24740,          0   },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,        2,       READ_OP,        0,          0   },
              {L_,  WVIA,          7,           0,      24740,          0   },
            },
            #elif defined(BSLS_PLATFORM_OS_LINUX)
            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,   WA,        BUF_WRITE,     1,       BUF_WRITE,     0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,   WVO,           8,         1,           0, INTERRUPTED },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           5,         0,         164,         0   },
              {L_,   WA,        BUF_WRITE,     1,       BUF_WRITE,     0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,   WVI,           8,         1,        0, INTERRUPTED },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           5,         0,         164,         0   },
              {L_,   WA,        BUF_WRITE,     1,       BUF_WRITE,     0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,  WVOA,           8,         1,           0, INTERRUPTED },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           5,         0,         164,         0   },
              {L_,   WA,        BUF_WRITE,     1,       BUF_WRITE,     0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,  WVIA,           8,         1,           0, INTERRUPTED },
            },
            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   WVO,           1,        0,            1,        0   },
              {L_,   WA,  SYS_DEPENDENT_LEN2, 1,  SYS_DEPENDENT_LEN2, 0   },
              {L_,  SIGNAL,         10,       0,            0,        0   },
              {L_,   WVO,           7,        0,        24740,        0   },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   WVI,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN2, 0,  SYS_DEPENDENT_LEN2, 0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,   WVI,           7,        0,        24740,        0   },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  WVOA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN2, 0, SYS_DEPENDENT_LEN2,  0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,  WVOA,           7,        0,        24740,        0   },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  WVIA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN2, 0,  SYS_DEPENDENT_LEN2, 0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,  WVIA,           7,        0,        24740,        0   },
            },

            #else // ibm test data
            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,   WA,        40000,         1,       40000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,   WVO,           8,         1,        1024, INTERRUPTED },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           5,         0,         164,         0   },
              {L_,   WA,        40000,         1,       40000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,   WVI,           8,         1,        1024, INTERRUPTED },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           5,         0,         164,         0   },
              {L_,   WA,        40000,         1,       40000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,  WVOA,           8,         1,        1024, INTERRUPTED },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           5,         0,         164,         0   },
              {L_,   WA,        40000,         1,       40000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,        15,         0,           0,         0   },
              {L_,  WVIA,           8,         1,        1024, INTERRUPTED },
            },
            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   WVO,           1,        0,            1,        0   },
              {L_,   WA,        40000,        1,        40000,        0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,   WVO,           7,        0,        24740,        0   },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   WVI,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,   WVI,           7,        0,        24740,        0   },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  WVOA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,  WVOA,           7,        0,        24740,        0   },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  WVIA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,        0,            0,        0   },
              {L_,  WVIA,           7,        0,        24740,        0   },
            },

            #endif
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[WVECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     buf4[WVECBUF_LEN80] = "\0", buf5[WVECBUF_LEN8K] = "\0",
                     buf6[WVECBUF_LEN16K] = "\0", buf7[WVECBUF_LEN32K] = "\0";

                btes_Ovec ovecBuffer[MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btes_Iovec iovecBuffer[MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, iovecBuffer, ovecBuffer) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
#endif
      } break;
      case 9: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // -------------------------------------------------------------------
        // TESTING 'writeRaw' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no write operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     4. write the expected number of bytes to the channel if
        //        enough space available in the channel;
        //     5. write data as much as possible to the channel if no
        //        enough space available in the channel;
        //     6. return after an "AE" occurs if the 'read' is in
        //        'interruptible' mode, and the return value for 'augstatus'
        //        should be > 0;
        //
        // Plan:
        //   Create a socket pair which is connected to each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Three steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "writing".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying writing.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 6)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int writeRaw(char *buffer,
        //                int   numBytes,
        //                int   flags = 0);
        //   int writeRaw(int  *augStatus,
        //                char *buffer,
        //                int   numBytes,
        //                int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'writeRaw' method");
            QT("=========================");
        }
        {
            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WR,          1,          0,           1,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.  The first
                // write operation is just to help the second write 'hanging'
                // there waiting to write.
              {L_,   SLEEP,       200,         0,           0,         0   },
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    WR,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the write operation w/ the 'augStatus'
            // parameter.  The first write operation is just to help the
            // second write 'hanging' there waiting to write.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WRA,          1,          0,           1,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,   SLEEP,       200,         0,           0,         0   },
              {L_, CLOSE_CONTROL,  0,          0,           0,         0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WRA,         100,         0,        INVALID,      0   },
              {L_,    WR,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #if defined (BSLS_PLATFORM_OS_SOLARIS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WR,          1,          0,           1,         0   },
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         ERR,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },

            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WRA,          1,          0,           1,         0   },
              {L_,   WRA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,   WRA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         ERR,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_, SLEEP,         200,         0,           0,         0   },
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)  // windows test data
            {
              {L_,   WRA,           1,         0,               1,      0   },
              {L_,   WRA,         100,         0,             100,      0   },
              {L_,    WR,          60,         0,              60,      0   },
              {L_,    -1,           0,         0,               0,      0   },
            },
            {
              {L_,    WR,           1,         0,               1,      0   },
              {L_,    WR,         100,         0,             100,      0   },
              {L_,    WR,          60,         0,              60,      0   },
              {L_,    -1,           0,         0,               0,      0   },
            },

            #else                                  // unix test data
            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WR,          1,          0,           1,         0   },
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         8192,        0   },
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },

            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   WRA,          1,          0,           1,         0   },
              {L_,   WRA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         8192,        0   },
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,    WR,   SYS_DEPENDENT_LEN,  1,        8192,         0   },
              {L_,  SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,    WR,    BUF_WRITE,         1,       40960,         0   },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,   WRA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN,  1,        8192,         0   },
              {L_,  SIGNAL,          2,      READ_OP,        0,         0   },
              {L_,   WRA,    BUF_WRITE,         1,       40960,         0   },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,    WR,   SYS_DEPENDENT_LEN,  1,        8192,         0   },
              {L_,  SIGNAL,          2,      READ_OP,        0,         0   },
              {L_,    WR,   SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
            },

            // commands set 8: to resolve concern 4 - 6.
            {
              {L_,   WRA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN,  1,        8192,         0   },
              {L_,  SIGNAL,          2,      READ_OP,       0,          0   },
              {L_,   WRA,        10000,         0,      10000,          0   },
            },
            #elif defined(BSLS_PLATFORM_OS_LINUX)
            // commands set 3: to resolve concern 4 - 6.
            {
              {L_,    WR,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,    0   },
              {L_,   WR,    BUF_WRITE,        0,      BUF_WRITE,        0   },
              {L_,  SIGNAL,          2,   READ_OP,          0,          0   },
              {L_,    WR,    BUF_WRITE,        1,        1024,          0   },
            },
            // commands set 4: to resolve concern 4 - 6.
            {
              {L_,  WRA,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,  SYS_DEPENDENT_LEN,   0   },
              {L_,  WRA,    BUF_WRITE,        0,      BUF_WRITE,       0   },
              {L_,  SIGNAL,         2,   READ_OP,          0,          0   },
              {L_,  WRA,    BUF_WRITE,        1,        1024,          0   },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    W,           1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        SYS_DEPENDENT_LEN,
               0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_,   WR,        1000,         0,        1000,          0   },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,   WA,           1,         0,           1,          0   },
              {L_,   WA,   SYS_DEPENDENT_LEN, 1,        SYS_DEPENDENT_LEN,
               0   },
              {L_,  WRA,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_,   WRA,       1000,         0,        1000,          0   },
            },

            #else                // ibm test data
            // commands set 3: to resolve concern 4 - 6.
            {
              {L_,    WR,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN,    0   },
              {L_,   WR,    BUF_WRITE,        0,      BUF_WRITE,        0   },
              {L_,  SIGNAL,          2,   READ_OP,          0,          0   },
              {L_,    WR,    BUF_WRITE,        1,        1024,          0   },
            },
            // commands set 4: to resolve concern 4 - 6.
            {
              {L_,  WRA,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,  SYS_DEPENDENT_LEN,   0   },
              {L_,  WRA,    BUF_WRITE,        0,      BUF_WRITE,       0   },
              {L_,  SIGNAL,         2,   READ_OP,          0,          0   },
              {L_,  WRA,    BUF_WRITE,        1,        1024,          0   },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    W,           1,         0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   WR,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_,   WR,        1000,         0,        1000,          0   },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,   WA,           1,         0,           1,          0   },
              {L_,   WA,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,  WRA,    BUF_WRITE,        0,        1024,          0   },
              {L_,  SIGNAL,        2,      READ_OP,        0,          0   },
              {L_,   WRA,       1000,         0,        1000,          0   },
            },

            #endif
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));

                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ret = processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0);
                    ASSERT(0 == ret);
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
#endif
      } break;
      case 8: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // -------------------------------------------------------------------
        // TESTING 'write' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no write operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     4. write the expected number of bytes to the channel if
        //        enough space available in the channel;
        //     5. return after an "AE" occurs if the "write" is in
        //        'interruptible' mode, and the return value for 'augstatus'
        //        should be > 0;
        //     6. keep trying writing expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected to each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Three steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "writing".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying writing.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 6)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int write(char *buffer,
        //             int   numBytes,
        //             int   flags = 0);
        //   int write(int  *augStatus,
        //             char *buffer,
        //             int   numBytes,
        //             int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'write' method");
            QT("======================");
        }

        {
            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            #if defined (BSLS_PLATFORM_OS_UNIX)
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     W,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WA,          1,          0,           1,         0   },
              {L_,    WA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    WA,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WA,         100,         0,        INVALID,      0   },
              {L_,     W,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif
            #if defined (BSLS_PLATFORM_OS_SOLARIS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     W,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         ERR,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },

            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WA,          1,          0,           1,         0   },
              {L_,    WA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,   WA,   SYS_DEPENDENT_LEN,    0,       ERR,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
              {L_,    WA,           1,         0,               1,      0   },
              {L_,    WA,         100,         0,             100,      0   },
              {L_,     W,          60,         0,              60,      0   },
              {L_,    -1,           0,         0,               0,      0   },
            },
            {
              {L_,     W,           1,         0,               1,      0   },
              {L_,     W,         100,         0,             100,      0   },
              {L_,     W,          60,         0,              60,      0   },
              {L_,    -1,           0,         0,               0,      0   },
            },

            #else                                      // unix test data
            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     W,           1,         0,           1,         0   },
              {L_,     W,          15,         0,          15,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 1,        8192,         0   },
                // There is not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 6: to resolve concern
            {
                // Each request write expected number of bytes from channel.
              {L_,    WA,           1,         0,           1,         0   },
              {L_,    WA,          15,         0,          15,         0   },
              {L_,    WA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,    WA,         10000,       1,        8192, INTERRUPTED },
                // There is not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 7: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,          2,         0,           0,         0   },
              {L_,     W,        10000,         1,        8192,         0   },
            },

            // commands set 8: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,          2,         0,           0,         0   },
              {L_,    WA,        10000,         1,        8192, INTERRUPTED },
            },
            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,          2,      READ_OP,       0,          0   },
              {L_,     W,   SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  SIGNAL,          2,      READ_OP,       0,          0   },
              {L_,    WA,        10000,         0,      10000,          0   },
            },

            #elif defined(BSLS_PLATFORM_OS_LINUX)
            // commands set 3: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     W,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         5,         0,           0,         0   },
              {L_,     W,         60000,       1,         16383,       0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 4: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     WA,           1,         0,           1,         0   },
              {L_,     WRA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
            //{L_,    WRA,       30000,         1,       30000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         30,          0,           0,         0   },
              {L_,      WA,      60000,          1,       16383, INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the "write" method w/o the
                // 'augStatus' argument.
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    WA,            1,         0,          1,          0   },
              {L_,    WA,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,          2,      READ_OP,       0,          0   },
              {L_,     W,         2000,         0,       2000,          0   },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,    WA,            1,         0,           1,         0   },
              {L_,    WA,  SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,          2,      READ_OP,        0,         0   },
              {L_,    WA,         2000,         0,        2000,         0   },
            },

            #else           // ibm test data

            // commands set 3: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     W,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,     W,        30000,        1,       30000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 1,        8192,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 4: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     WA,           1,         0,           1,         0   },
              {L_,     WA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    WRA,       30000,         1,       30000,         0   },
                // There is not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         30,          0,           0,         0   },
              {L_,     WA,       60000,          1,        2000, INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the "write" method w/o the
                // 'augStatus' argument.
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    WA,            1,         0,          1,          0   },
              {L_,    WA,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,          2,      READ_OP,       0,          0   },
              {L_,     W,         2000,         0,       2000,          0   },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,    WA,            1,         0,           1,         0   },
              {L_,    WA,  SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,          2,      READ_OP,        0,         0   },
              {L_,    WA,         2000,         0,        2000,         0   },
            },

            #endif
            #endif
          };

//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
#endif
      } break;
      case 7: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_CPU_64_BIT)
        // -------------------------------------------------------------------
        // TESTING 'bufferedReadRaw' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer no matter
        //        there's enough data existing in the internal buffer or not;
        //     (when there's no data in the channel's internal buffer)
        //     4. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     5. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     6. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 5 - 7)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies:
        //       (1) which operation should be done in this command;
        //       (2) what arguments to be passed for this call and
        //       (3) the results expected to be returned.
        //     A thread is created to generate expected number of signals as
        //     "asynchronous events", also write expected number of bytes at
        //     the peer side if specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int bufferedReadRaw(char *buffer,
        //                       int   numBytes,
        //                       int   flags = 0);
        //   int bufferedReadRaw(int  *augStatus,
        //                       char *buffer,
        //                       int   numBytes,
        //                       int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'bufferedRead' method");
            QT("=============================");
        }

        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RBR,         1,         0,           1,         0   },
              {L_,     RBR,        15,         0,          15,         0   },
              {L_,     RBR,       100,         0,          34,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,     RBR,       100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     RBR,       100,         0,        INVALID,      0   },
              {L_,    RBRA,        60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RBRA,        10,         0,          10,         0   },
              {L_,    RBRA,       100,         0,          40,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RBRA,       100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     RBR,        100,         0,        INVALID,      0   },
              {L_,    RBRA,         60,         0,        INVALID,      0   },
              {L_,    -1,            0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,      50,         0,          50,         0   },
              {L_,     RBR,         16,         0,          16,         0   },
              {L_,     RBR,         60,         0,          34,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,    0,         0,           0,         0   },
              {L_,    RBRA,         20,         0,         ERR,       0   },
              {L_,     RBR,         80,         0,        INVALID,      0   },
              {L_,    RBRA,         40,         0,        INVALID,      0   },
              {L_,    -1,            0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,      50,         0,          50,         0   },
              {L_,    RBRA,         16,         0,          16,         0   },
              {L_,    RBRA,         60,         0,          34,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,    0,         0,           0,         0   },
              {L_,    RBRA,         20,         1,         ERR,       0   },
              {L_,     RBR,         80,         1,        INVALID,      0   },
              {L_,    RBRA,         40,         1,        INVALID,      0   },
              {L_,    -1,            0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,  RBRA,           1,         0,           1,         0   },
              {L_,  RBRA,          25,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RBR,           1,         0,           1,         0   },
              {L_,   RBR,          25,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,           0,         0   },
            },

            #else

            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'readRaw' method w/o the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,      50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,         1,         0,           0,         0   },
              {L_,    RB,           80,         1,          50, INTERRUPTED },
                // Now a "read" for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RBR,          40,         0,          40,         0   },
              {L_,    RBR,           4,         0,           4,         0   },
              {L_,    RBR,           2,         0,           2,         0   },
              {L_,    RBR,           3,         0,           3,         0   },
              {L_,   SIGNAL,         0,    WRITE_OP,         0,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_,   RBRA,          50,         0,           1,         0  },
              {L_, HELP_WRITE,      20,         0,          20,         0  },
              {L_,   RBRA,          50,         0,          20,         0  },
              {L_,   -1,             0,         0,           0,         0  },
            },

            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the "read" method under test with
            // the 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,     RBA,        80,         1,          50, INTERRUPTED },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RBRA,        40,         0,          40,         0   },
              {L_,    RBRA,         4,         0,           4,         0   },
              {L_,    RBRA,         2,         0,           2,         0   },
              {L_,    RBRA,         3,         0,           3,         0   },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   SIGNAL,       0,    WRITE_OP,          0,         0   },
              {L_,    RBRA,       90,          0,           1,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_,   RBRA,        50,         0,          50,         0   },
              {L_,   -1,           0,         0,           0,         0   },

            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RB,          1,         0,           1,         0   },
              {L_,    RBA,         10,         0,          10,         0   },
              {L_,     RB,         15,         0,          15,         0   },
              {L_,    RBA,         20,         0,          20,         0   },
                // When there is no enough data (but some data is available),
                // a 'readRaw' will return the number of bytes it read.
              {L_,    RBR,         25,         0,           4,         0   },
              {L_, HELP_WRITE,     10,         0,          10,         0   },
              {L_,   RRA,          35,         0,          10,         0   },
                // There is no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RBR,        105,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RBR,        105,         0,         100,         0   },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RBRA,        105,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RBRA,        105,         0,         100,         0   },
              {L_,   -1,             0,         0,           0,         0  },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
#endif
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // TESTING 'bufferedRead' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer if there's
        //        enough data existing in the internal buffer;
        //     5. copy the data from the internal buffer first, then read
        //        from the channel for the remaining requested data.
        //     (when there's no data in the channel's internal buffer)
        //     6. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     7. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     8. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 5)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 6 - 8)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int bufferedRead(char *buffer,
        //                    int   numBytes,
        //                    int   flags = 0);
        //   int bufferedRead(int  *augStatus,
        //                    char *buffer,
        //                    int   numBytes,
        //                    int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'bufferedRead' method");
            QT("=============================");
        }
        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RB,          1,         0,           1,         0   },
              {L_,     RB,         15,         0,          15,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,     RB,        100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     RB,        100,         0,        INVALID,      0   },
              {L_,    RBA,         60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RBA,         10,         0,          10,         0   },
              {L_,    RBA,         20,         0,          20,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RBA,        100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     RB,        100,         0,        INVALID,      0   },
              {L_,    RBA,         60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RB,          1,         0,           1,         0   },
              {L_,     RB,         30,         0,          30,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RBA,         20,         0,         ERR,         0   },
              {L_,     RB,         80,         0,        INVALID,      0   },
              {L_,    RBA,         40,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RB,          1,         0,           1,         0   },
              {L_,    RBA,         10,         0,          10,         0   },
              {L_,     RB,         15,         0,          15,         0   },
              {L_,    RBA,         20,         0,          20,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RBA,         20,         1,         ERR,         0   },
              {L_,     RB,         80,         1,        INVALID,      0   },
              {L_,    RBA,         40,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RBA,           1,         0,           1,         0   },
              {L_,   RBA,          25,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RB,           1,         0,           1,         0   },
              {L_,    RB,          25,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,           0,         0   },
            },

            #else
            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'readRaw' method w/o the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50, INTERRUPTED },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RB,          40,         0,          40,         0   },
              {L_,    RB,           4,         0,           4,         0   },
              {L_,    RB,           2,         0,           2,         0   },
              {L_,    RB,           3,         0,           3,         0   },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,    RB,          90,         0,          90,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_,   RBRA,         50,         0,          11,         0   },
            },
            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the "read" method under test with
            // the 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RBA,         80,         1,          50, INTERRUPTED },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RBA,         40,         0,          40,         0   },
              {L_,    RBA,          4,         0,           4,         0   },
              {L_,    RBA,          2,         0,           2,         0   },
              {L_,    RBA,          3,         0,           3,         0   },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,    RBA,         90,         0,          90,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_,   RBRA,         50,         0,          11,         0   },
            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     RB,          1,         0,           1,         0   },
              {L_,    RBA,         10,         0,          10,         0   },
              {L_,     RB,         15,         0,          15,         0   },
              {L_,    RBA,         20,         0,          20,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,    RBA,          5,         1,           4, INTERRUPTED },
                // This request is to remove data in the internal buffer due
                // to the previous partial read, and so some other tests can
                // be done.
              {L_,    RBA,          4,         1,           4,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_, HELP_WRITE,      1,         1,           1,         0   },
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,     RB,          5,         1,           1, INTERRUPTED },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // dimplemented to write 100 bytes to the channel feed the
                // request.
              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,     RBA,        25,         0,          25,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,     RB,        150,         0,         150,         0   },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 5: {
        // -------------------------------------------------------------------
        // TESTING 'readvRaw' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer no matter
        //        there's enough data or not in the internal buffer;
        //     (when there's data existing in the channel's internal buffer)
        //     5. read the expected number of bytes from the channel if
        //        some data available in the channel;
        //     6. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     7. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 5 - 7)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     and/or write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int readvRaw(btes_Iovec *buffer,
        //                int         numBytes,
        //                int         flags = 0);
        //   int readvRaw(int        *augStatus,
        //                btes_Iovec *buffer,
        //                int         numBytes,
        //                int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'readvRaw' method");
            QT("=========================");
        }

        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RVR,          1,         0,           1,         0   },
              {L_,    RVR,          7,         0,          49,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RVR,          6,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RVR,          7,         0,        INVALID,      0   },
              {L_,   RVRA,          6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVRA,          1,         0,           1,         0   },
              {L_,   RVRA,          7,         0,          49,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   RVRA,          6,         0,        CLOSED,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RVR,          7,         0,       INVALID,       0   },
              {L_,   RVRA,          6,         0,       INVALID,       0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RVR,          1,         0,           1,         0   },
              {L_,    RVR,          7,         0,          49,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,   RVRA,          6,         1,         ERR,         0   },
              {L_,    RVR,          4,         1,       INVALID,       0   },
              {L_,   RVRA,          5,         1,       INVALID,       0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVRA,          2,         0,           4,         0   },
              {L_,   RVRA,          7,         0,          46,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RVR,          6,         1,         ERR,         0   },
              {L_,    RVR,          4,         1,        INVALID,      0   },
              {L_,   RVRA,          5,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,  RVRA,           1,         0,           1,         0   },
              {L_,  RVRA,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVR,           1,         0,           1,         0   },
              {L_,   RVR,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,           0,         0   },
            },

            #else
            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method w/o the
            // 'augStatus' parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a "read" will return right away because there's
                // data in the internal buffer at this moment.
              {L_,    RVR,          2,         0,           4,         0   },
              {L_,    RVR,          3,         0,           9,         0   },
              {L_,    RVR,          6,         0,          35,         0   },
                // The next 'read' keep reading data it needs until no data
                // in the internal buffer.
              {L_,    RVR,          4,         0,           2,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_, HELP_WRITE,     25,         0,          25,         0   },
              {L_,    RVR,          6,         0,          25,         0   },
            },

            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method with the
            // 'augStatus' parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a "read" will return right away because there's
                // enough data in the internal buffer at this moment.
              {L_,   RVRA,          2,         0,           4,         0   },
              {L_,   RVRA,          3,         0,           9,         0   },
              {L_,   RVRA,          6,         0,          35,         0   },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint
                // through a thread.
              {L_,   SIGNAL,        0,   WRITE_OP,          0,         0   },
              {L_,   RVRA,          4,         0,           2,         0   },
                // There is enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,   RVRA,          6,         0,          35,         0   },
              {L_,   RBR,         150,         0,          65,         0   },
            },

            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RVR,          1,         0,           1,         0   },
              {L_,   RVRA,          2,         0,           4,         0   },
              {L_,    RVR,          4,         0,          16,         0   },
              {L_,   RVRA,          5,         0,          25,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, but a "raw" operation will still return.
              {L_,    RVR,          3,         0,           4,         0   },
              {L_, HELP_WRITE,     10,         0,          10,         0   },
              {L_,   RVRA,          4,         1,          10,         0   },
                // There is no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RVR,          8,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RVR,          8,         0,         100,         0   },
                // The same as above except  w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RVRA,          8,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RVRA,          8,         0,         100,         0   },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));

                char buf0[VECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[VECBUF_LEN5] = "\0", buf3[VECBUF_LEN7] = "\0",
                     buf4[VECBUF_LEN9] = "\0", buf5[VECBUF_LEN10] = "\0",
                     buf6[VECBUF_LEN50] = "\0", buf7[VECBUF_LEN90] = "\0";

                btes_Iovec vecBuffer[MAX_VECBUF];
                vecBuffer[0].setBuffer(buf0, sizeof buf0);
                vecBuffer[1].setBuffer(buf1, sizeof buf1);
                vecBuffer[2].setBuffer(buf2, sizeof buf2);
                vecBuffer[3].setBuffer(buf3, sizeof buf3);
                vecBuffer[4].setBuffer(buf4, sizeof buf4);
                vecBuffer[5].setBuffer(buf5, sizeof buf5);
                vecBuffer[6].setBuffer(buf6, sizeof buf6);
                vecBuffer[7].setBuffer(buf7, sizeof buf7);

                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 4: {
        // -------------------------------------------------------------------
        // TESTING 'readv' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer when
        //        there's enough data in the internal buffer;
        //     5. copy the existing data from the channel's internal buffer
        //        first, then read the remaining data from the channel when
        //        there's not enough data in the internal buffer
        //        from the channel for the remaining requested data.
        //     (when there's data existing in the channel's internal buffer)
        //     6. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     7. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     8. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 5)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 6 - 8)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     and/or write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int readv(btes_Iovec *buffer,
        //             int         numBytes,
        //             int         flags = 0);
        //   int readv(int        *augStatus,
        //             btes_Iovec *buffer,
        //             int         numBytes,
        //             int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'readv' method");
            QT("======================");
        }

        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RV,           1,         0,           1,         0   },
              {L_,    RV,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RV,           6,         0,      CLOSED,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RV,           7,         0,        INVALID,      0   },
              {L_,   RVA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVA,           1,         0,           1,         0   },
              {L_,   RVA,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   RVA,           6,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RV,           7,         0,        INVALID,      0   },
              {L_,   RVA,           6,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RV,           1,         0,           1,         0   },
              {L_,    RV,           5,         0,          25,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,   RVA,           6,         1,         ERR,         0   },
              {L_,    RV,           4,         1,        INVALID,      0   },
              {L_,   RVA,           5,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVA,           2,         0,           4,         0   },
              {L_,   RVA,           6,         0,          35,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RV,           6,         1,         ERR,         0   },
              {L_,    RV,           4,         1,        INVALID,      0   },
              {L_,   RVA,           5,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RVA,           1,         0,           1,         0   },
              {L_,   RVA,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RV,           1,         0,           1,         0   },
              {L_,    RV,           5,         0,          25,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,           0,         0   },
            },

            #else
            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method w/o the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a "read" will return right away because there's
                // enough data in the internal buffer at this moment.
              {L_,    RV,           2,         0,           4,         0   },
              {L_,    RV,           3,         0,           9,         0   },
              {L_,    RV,           6,         0,          35,         0   },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint
                // through a thread.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,    RV,           4,         0,          16,         0   },
                // There is enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,    RV,           6,         0,          35,         0   },
              {L_,   RBR,         150,         0,          51,         0   },
            },

            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method with the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a "read" will return right away because there's
                // enough data in the internal buffer at this moment.
              {L_,   RVA,           2,         0,           4,         0   },
              {L_,   RVA,           3,         0,           9,         0   },
              {L_,   RVA,           6,         0,          35,         0   },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint
                // through a thread.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,   RVA,           4,         0,          16,         0   },
                // There is enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,   RVA,           6,         0,          35,         0   },
              {L_,   RBR,         150,         0,          51,         0   },
            },

            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RV,           1,         0,           1,         0   },
              {L_,   RVA,           2,         0,           4,         0   },
              {L_,    RV,           3,         0,           9,         0   },
              {L_,   RVA,           6,         0,          35,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,   RVA,           3,         1,           1, INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,    RV,           1,         1,           0,         0   },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // dimplemented to write 100 bytes to the channel feed the
                // request.

              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,    RVA,          6,         0,          35,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,    RV,           7,         0,          85,         0   },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));

                char buf0[VECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[VECBUF_LEN5] = "\0", buf3[VECBUF_LEN7] = "\0",
                     buf4[VECBUF_LEN9] = "\0", buf5[VECBUF_LEN10] = "\0",
                     buf6[VECBUF_LEN50] = "\0", buf7[VECBUF_LEN90] = "\0";

                btes_Iovec vecBuffer[MAX_VECBUF];
                vecBuffer[0].setBuffer(buf0, sizeof buf0);
                vecBuffer[1].setBuffer(buf1, sizeof buf1);
                vecBuffer[2].setBuffer(buf2, sizeof buf2);
                vecBuffer[3].setBuffer(buf3, sizeof buf3);
                vecBuffer[4].setBuffer(buf4, sizeof buf4);
                vecBuffer[5].setBuffer(buf5, sizeof buf5);
                vecBuffer[6].setBuffer(buf6, sizeof buf6);
                vecBuffer[7].setBuffer(buf7, sizeof buf7);

                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 3: {
        // -------------------------------------------------------------------
        // TESTING 'readRaw' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer no matter
        //        there's enough data in the internal buffer or not;
        //     (when there's no data in the channel's internal buffer)
        //     5. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     6. return at least one byte from the channel w/o waiting when no
        //        enough data available in the channel;
        //     7. return any positive number of bytes -- if it ever received,
        //        upon an "AE" no matter if the channel is 'non-interruptible'
        //        or not.
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 3: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 4: (for concern 4)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 5 - 7)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     and/or write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int readRaw(char *buffer,
        //               int   numBytes,
        //               int   flags = 0);
        //   int readRaw(int  *augStatus,
        //               char *buffer,
        //               int   numBytes,
        //               int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'readRaw' method");
            QT("========================");
        }

        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RR,           1,         0,           1,         0   },
              {L_,    RR,          15,         0,          15,         0   },
              {L_,    RR,         100,         0,          34,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RR,         100,         0,      CLOSED,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RR,         100,         0,        INVALID,      0   },
              {L_,   RRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RRA,          10,         0,          10,         0   },
              {L_,   RRA,         100,         0,          40,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   RRA,         100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    RR,         100,         0,        INVALID,      0   },
              {L_,   RRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RR,          16,         0,          16,         0   },
              {L_,    RR,          60,         0,          34,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RR,          20,         1,         ERR,         0   },
              {L_,    RR,          80,         1,        INVALID,      0   },
              {L_,   RRA,          40,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RRA,          16,         0,          16,         0   },
              {L_,   RRA,          60,         0,          34,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,   RRA,          20,         0,         ERR,         0   },
              {L_,    RR,          80,         0,        INVALID,      0   },
              {L_,   RRA,          40,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RRA,           1,         0,           1,         0   },
              {L_,   RRA,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          44,         0   },
            },
            {
              {L_, HELP_WRITE,    100,         0,         100,         0   },
              {L_,    RR,           1,         0,           1,         0   },
              {L_,    RR,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          94,         0   },
            },

            #else
            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'readRaw' method w/o the
            // 'augStatus' parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RR,          40,         0,          40,         0   },
              {L_,    RR,           4,         0,           4,         0   },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,    RR,          15,         0,           6,         10  },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_, HELP_WRITE,     25,         0,          25,         0   },
              {L_,   RRA,          50,         0,          25,         0   },
            },
            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'readRaw' method with the
            // 'augStatus' parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RRA,          40,         0,          40,         0  },
              {L_,    RRA,          4,          0,           4,         0  },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,    RRA,         15,          0,           6,         0  },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_, HELP_WRITE,      25,         0,          25,         0  },
              {L_,   RRA,           50,         0,          25,         0  },
            },
            // commands set 7: to resolve concern 5 - 7.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RR,           1,         0,           1,         0   },
              {L_,   RRA,          10,         0,          10,         0   },
              {L_,    RR,          15,         0,          15,         0   },
              {L_,   RRA,          20,         0,          20,         0   },
                // When there is not enough data (but some data is available),
                // a 'readRaw' will return the number of bytes it reads.
              {L_,    RR,          25,         0,           4,         0   },
              {L_, HELP_WRITE,     10,         0,          10,         0   },
              {L_,   RRA,          35,         0,          10,         0   },
                // There is no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RR,         105,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,    RR,         105,         0,         100,         0   },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RRA,         105,         1,         100,         0   },
              {L_,  SIGNAL,         1,  WRITE_OP,           0,         0   },
              {L_,   RRA,         105,         0,         100,         0   },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 2: {
// TBD FIX ME
// #ifndef BSLS_PLATFORM_OS_AIX
        // -------------------------------------------------------------------
        // TESTING 'read' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //     (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer if there's
        //        enough data existing in the internal buffer;
        //     5. copy the data from the internal buffer first, then read
        //        from the channel for the remaining requested data.
        //     (when there's no data in the channel's internal buffer)
        //     6. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     7. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     8. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpChannel' object 'channel' is created with one of the
        //   two sockets.  Four steps are designed for this function:
        //
        //   Step 1: (for concern 1 - 2)
        //     After a channel is established and verified, close the
        //     connection from the peer side while the channel is
        //     doing "reading".  Verify the return result to see if it's same
        //     as the expected.  Also verify the results of subsequent
        //     read operations.
        //
        //   Step 2: (for concern 3)
        //     After a channel is established and verified, close the
        //     connection from the channel side, and the channel sill keeps
        //     trying reading.  Verify the return result to see if it's
        //     same as the expected.
        //
        //   Step 3: (for concern 4 - 5)
        //     Establish a channel and make the expected number of bytes
        //     of data available in the channel's internal buffer by
        //     issuing a partial 'bufferedRead' request which was made a
        //     partial read by issuing an "asynchrounous event" during the
        //     operation, then a list of the "read" operations under
        //     test will be issued for the test and the results will be
        //     verified.
        //
        //   Step 4: (for concern 6 - 8)
        //     Build a set 'DATA' of 'TestCommand', where each entry
        //     specifies: (1) which operation should be done in this command;
        //     (2) what arguments to be passed for this call and (3) the
        //     results expected to be returned.  A thread is created to
        //     generate expected number of signals as "asynchronous events",
        //     also write expected number of bytes at the peer side if
        //     specified.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int read(char *buffer,
        //            int   numBytes,
        //            int   flags = 0);
        //   int read(int  *augStatus,
        //            char *buffer,
        //            int   numBytes,
        //            int   flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'read' method");
            QT("=====================");
        }

        {

            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     R,           1,         0,           1,         0   },
              {L_,     R,          15,         0,          15,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,     R,         100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     R,         100,         0,        INVALID,      0   },
              {L_,    RA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the 'read' method w the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RA,          10,         0,          10,         0   },
              {L_,    RA,          20,         0,          20,         0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    RA,         100,         0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,     R,         100,         0,        INVALID,      0   },
              {L_,    RA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     R,           1,         0,           1,         0   },
              {L_,     R,          30,         0,          30,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RA,          20,         0,         ERR,         0   },
              {L_,     R,          80,         0,        INVALID,      0   },
              {L_,    RA,          40,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            // Command set 4: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     R,           1,         0,           1,         0   },
              {L_,    RA,          10,         0,          10,         0   },
              {L_,     R,          15,         0,          15,         0   },
              {L_,    RA,          20,         0,          20,         0   },
                // Now close the channel, and try some 'read' operations,
                // each of which should return a "ERROR".
              {L_, CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,    RA,          20,         1,         ERR,         0   },
              {L_,     R,          80,         1,        INVALID,      0   },
              {L_,    RA,          40,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RA,           1,         0,           1,         0   },
              {L_,    RA,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          44,         0   },
            },
            {
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     R,           1,         0,           1,         0   },
              {L_,     R,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          44,         0   },
            },

            #else
            // Commands set 5: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method w/o the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,     R,          40,         0,          40,         0   },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint
                // through a thread.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,     R,          15,         0,          15,         0   },
                // There are 95 bytes in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,     R,          50,         0,          50,         0   },
              {L_,   RBR,          50,         0,          45,         0   },
            },

            // Commands set 6: Establish a channel and make the expected
            // number of bytes of data available in the channel's internal
            // buffer, test the behavior of the 'read' method with the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   SIGNAL,        1,         0,           0,         0   },
              {L_,    RB,          80,         1,          50,         0   },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    RA,          40,         0,          40,         0   },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint
                // through a thread.
              {L_,   SIGNAL,        0,  WRITE_OP,           0,         0   },
              {L_,    RA,          15,         0,          15,         0   },
                // There are 95 bytes in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,    RA,          50,         0,          50,         0   },
              {L_,   RBR,          50,         0,          45,         0   },
            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,     R,           1,         0,           1,         0   },
              {L_,    RA,          10,         0,          10,         0   },
              {L_,     R,          15,         0,          15,         0   },
              {L_,    RA,          20,         0,          20,         0   },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,    RA,           5,         1,           4, INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_, HELP_WRITE,      1,         1,           1,         0   },
              {L_,  SIGNAL,         1,         0,           0,         0   },
              {L_,     R,           5,         1,           1,         0   },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // dimplemented to write 100 bytes to the channel feed the
                // request.
              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,     RA,         25,         0,          25,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,  WRITE_OP,           0,         0   },
              {L_,     R,         150,         0,         150,         0   },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                bteso_SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                bteso_StreamSocket<bteso_IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.
                    // We insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0) );
                }
                factory.deallocate(sSocket);
                bteso_SocketImpUtil::close(handles[0]);
                bteso_SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
// #endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We need to exercise basic value-semantic functionalities.  In
        //   particular, make sure that functions for I/O requests such as
        //   'read()', 'readv', 'readRaw', 'readvRaw', 'bufferedRead',
        //  'bufferedReadRaw', 'write', 'writeRaw' etc. work fine.
        //
        // Plan:
        //   Create a channel, issue a list of I/O requests, verify the state
        //   values of each channel after each operation.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) {
            QT("Testing 'BREATHING TEST' method");
            QT("===============================");
        }
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
    bteso_SocketImpUtil::cleanup(&errCode);
    ASSERT(0 == errCode);

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
