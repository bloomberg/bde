// btesos_tcptimedchannel.t.cpp      -*-C++-*-

#include <btesos_tcptimedchannel.h>

#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <bteso_sockethandle.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>
#include <bteso_ioutil.h>
#include <btes_iovecutil.h>
#include <btesc_flag.h>

#include <bcemt_thread.h>

#include <bslma_testallocator.h>
#include <bdet_timeinterval.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM__OS_UNIX
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
// [ 1] btesos_TcpTimedChannel()
// [ 1] ~btesos_TcpTimedChannel()
// [ 1] int read(...)
// [ 1] int readv(...)
// [ 1] int readRaw(...)
// [ 1] int readvRaw(...)
// [ 1] int bufferedRead(...)
// [ 1] int bufferedReadRaw(...)
// [ 1] int write(...)
// [ 1] int writev(...)
// [ 1] int writeRaw(...)
// [ 1] int writevRaw(const btes_Ovec *buffers, ...)
// [ 1] int writevRaw(const btes_Iovec *buffers, ...)
// [ 1] void invalidate()
// [ 1] int isInvalid()
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
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
//                 GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btesos_TcpTimedChannel Obj;
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
    INTERRUPTED       =  1,
    CLOSED            = -1,
    INVALID           = -2,
    ERR               = -3,
    DEFAULT_AUGSTATUS = -9
};

#ifdef BSLS_PLATFORM__OS_UNIX
const int MAX_BUF     = 99000;     // the biggest length of a buffer for write

    #if defined BSLS_PLATFORM__OS_AIX

        const int BUF_WRITE   = 8192;  // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 500;   // the last second buffer vector length
        const int BUF_LIMIT   = 1024;  // to set the snd/rcv buffer size
        const int HELPER_READ = 75000;
        const int SYS_DEPENDENT_LEN = 65000;

    #elif defined(BSLS_PLATFORM__OS_LINUX)
        const int BUF_WRITE   = 20000; // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 1000;  // the last second vector buffer length
        const int BUF_LIMIT   = 4096;  // to set the snd/rcv buffer size
        const int HELPER_READ = 25000;
        const int SYS_DEPENDENT_LEN = 16000;
        const int SYS_DEPENDENT_LEN2 = 7000;

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
    WRITE_OP      = 1,
    READ_OP       = 2,
    HELPER_WRITE  = 100,        // When an "AE" occurs during a 'read' which
                                // is in 'non-interruptible' mode, some data
                                // need to be written at the peer side for
                                // the read to get data from the connection.
                                // This means in THIS test driver, any read
                                // request for "AE" interrupt test should be
                                // waiting for less than "HELPER_WRITE" bytes
                                // if it's in 'non-interruptible' mode.
   SUN_WRITE     = 1,           //
   IBM_WRITE     = 2,
   WIN_WRITE     = 3
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
    WVECBUF_LEN500 = 500,
    WVECBUF_LEN1K  = 1024,
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
     int                         d_signalIoFlag;  // if this flag is set,
       // meaning some on-going I/O operation is in 'non-interruptible' mode,
       // and so some data is needed to be writted at 'd_socketHandle' to
       // "let" the channel's I/O operation go.
     int                         d_ioType;
};

// Test commands used to test 'btesos_TcpTimedChannel':
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
    int                d_lineNum;

    int                d_command;       // command to invoke a indicated
                                           // function specified above.
    union {
      int              d_numBytes;      // bytes to be read/written
      int              d_numBuffers;    // buffers for readv/writev
      int              d_milliseconds;  // for a "sleep" command
      int              d_signals;       // for thread generating signals
    } numToProcess;

    union {
      int              d_interruptFlags;   // interruptible if it's set to
                                           // positive
      int              d_signalIoFlag;     // 1 == read from the peer side
                                           // 2 == write at the peer side
                                           // 0 == no read or write
    } flag;

    int                d_expReturnValue;   // return value of this command

    int                d_expStatus;        // expected status of this command

    bdet_TimeInterval *d_timeout;          // timeout requirement
};

char globalBuffer[HELPER_READ]; // To help read from the peer side.
bslma_TestAllocator testAllocator;
//=============================================================================
//                    HELPER FUNCTIONS/CLASSES FOR TESTING
//-----------------------------------------------------------------------------
extern "C" {
#ifdef BSLS_PLATFORM__OS_UNIX
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
            write(2, "====================\n",
                    sizeof("====================\n"));
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
    // Note the test can only work on UNIX platforms since window doesn't
    // support signal operations.
{
    ConnectInfo *threadInfo = (ConnectInfo*) arg;
    if (verbose) {
        QT("signal generator: ");
        PT(threadInfo->d_signals);
        PT(threadInfo->d_signalIoFlag);
        PT(threadInfo->d_ioType);
    }

    if (IBM_WRITE == threadInfo->d_ioType) {
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(10,0));
    }
    else {
        bcemt_ThreadUtil::microSleep(5 * SLEEP_TIME);
                                     // The thread waits to make sure
                                     // the main thread is hanging in an
                                     // I/O call.
    }

    for (int i = 0; i < threadInfo->d_signals; ++i) {
        pthread_kill(threadInfo->d_tid, SIGSYS);
        if (verbose) {
            PT(bcemt_ThreadUtil::self());
            QT(" generated a SIGSYS signal.");
        }
        bcemt_ThreadUtil::microSleep(4 * SLEEP_TIME);
    }

    if (threadInfo->d_signalIoFlag) {  // non-interruptible mode
        bcemt_ThreadUtil::microSleep(2 * SLEEP_TIME);
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
#ifdef BSLS_PLATFORM__OS_LINUX
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
            bcemt_ThreadUtil::microSleep(4 * SLEEP_TIME);
        }
    }

    bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    if (veryVerbose) {
        QT("Signal generator exits now.");
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
    bcemt_ThreadUtil::microSleep(5 * SLEEP_TIME);

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
                      btesos_TcpTimedChannel                *channel,
                      TestCommand                           *command,
                      bteso_SocketHandle::Handle            *socketPair,
                      bcemt_ThreadUtil::Handle              *threadHandle,
                      char                                  *buffer,
                      const btes_Iovec                      *ioBuffer,
                      const btes_Ovec                       *oBuffer,
                      int                                    ioType)
    // Process the specified 'command' to invoke some operation of the
    // specified 'channel', or the 'helperSocket' which is the control
    // endpoint of the socket pair.  For a read operation, load either
    // 'buffer' or 'ioBuffer' corresponding to the request.  For a write
    // operation, write data from either 'buffer' or 'ioBuffer' or 'oBuffer'
    // corresponding to the request.  Note because the behaviors are different
    // for "write" operations on different platform, the specified 'ioType'
    // is to indicate that the type of the function being tested, e.g. it's a
    // "read" or "write" operation so that different testing configuration
    // can be set on a specific platform.  Return the return value of the
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
    case TR: {   //
        rCode = channel->timedRead(buffer,
                              command->numToProcess.d_numBytes,
                              *command->d_timeout + bdetu_SystemTime::now(),
                              command->flag.d_interruptFlags);
    } break;
    case TRA: {  //
        int augStatus = 0;
        rCode = channel->timedRead(&augStatus,
                              buffer,
                              command->numToProcess.d_numBytes,
                              *command->d_timeout + bdetu_SystemTime::now(),
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
    case TRV: {  //
        rCode = channel->timedReadv(ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdetu_SystemTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case TRVA: { //
        int augStatus = 0;
        rCode = channel->timedReadv(&augStatus,
                               ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdetu_SystemTime::now(),
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
    case TRR: {  //
        rCode = channel->timedReadRaw(buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdetu_SystemTime::now(),
                                 command->flag.d_interruptFlags);
    } break;
    case TRRA: {  //
        int augStatus = 0;
        rCode = channel->timedReadRaw(&augStatus,
                                 buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdetu_SystemTime::now(),
                                 command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case RB: {  //
        rCode = channel->bufferedRead((const char**) &buffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        if (command->d_expStatus > 0) {
            LOOP_ASSERT(command->d_lineNum, 0 == buffer);
        }
    } break;
    case RBA: {  //
        int augStatus = 0;
        rCode = channel->bufferedRead(&augStatus,
                                      (const char**) &buffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (command->d_expStatus > 0) {
             ASSERT(0 == buffer);
        }
    } break;
    case TRB: {  //
        buffer = 0;
        rCode = channel->timedBufferedRead((const char**) &buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdetu_SystemTime::now(),
                                 command->flag.d_interruptFlags);
        if (command->d_expStatus >= 0) { // It's interrupted or timeout
            LOOP_ASSERT(command->d_lineNum, 0 == buffer);
        }
    } break;
    case TRBA: {  //
        int augStatus = -9;
        buffer = 0;
        rCode = channel->timedBufferedRead(&augStatus,
                                (const char**) &buffer,
                                command->numToProcess.d_numBytes,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (command->d_expStatus > 0) {
            LOOP_ASSERT(command->d_lineNum, 0 == buffer);
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
     case TRVR: {  //
        rCode = channel->timedReadvRaw(ioBuffer,
                             command->numToProcess.d_numBuffers,
                            *command->d_timeout + bdetu_SystemTime::now(),
                             command->flag.d_interruptFlags);
    } break;
    case TRVRA: { //
        int augStatus = 0;
        rCode = channel->timedReadvRaw(&augStatus,
                             ioBuffer,
                             command->numToProcess.d_numBuffers,
                            *command->d_timeout + bdetu_SystemTime::now(),
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
   case TRBR: {  //
        buffer = 0;
        rCode = channel->timedBufferedReadRaw((const char**) &buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdetu_SystemTime::now(),
                                 command->flag.d_interruptFlags);
    } break;
    case TRBRA: {  //
        int augStatus = 0;
        buffer = 0;
        rCode = channel->timedBufferedReadRaw(&augStatus,
                                 (const char**) &buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdetu_SystemTime::now(),
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
    } break;
    case TW: {    //
        rCode = channel->timedWrite(buffer,
                               command->numToProcess.d_numBytes,
                              *command->d_timeout + bdetu_SystemTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case TWA: {   //
        int augStatus = DEFAULT_AUGSTATUS;
        rCode = channel->timedWrite(&augStatus,
                               buffer,
                               command->numToProcess.d_numBytes,
                              *command->d_timeout + bdetu_SystemTime::now(),
                               command->flag.d_interruptFlags);
        if (rCode > 0 ) {
            if (0 <= augStatus) {
                LOOP_ASSERT(command->d_lineNum,
                       rCode > 0 && rCode < command->numToProcess.d_numBytes);
            }
            else if (DEFAULT_AUGSTATUS == augStatus) {
                LOOP_ASSERT(command->d_lineNum,
                            rCode == command->numToProcess.d_numBytes);
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
     case TWR: {   //
        rCode = channel->timedWriteRaw(buffer,
                                  command->numToProcess.d_numBytes,
                                 *command->d_timeout + bdetu_SystemTime::now(),
                                  command->flag.d_interruptFlags);
    } break;
    case TWRA: {  //
        int augStatus = 0;
        rCode = channel->timedWriteRaw(&augStatus,
                                  buffer,
                                  command->numToProcess.d_numBytes,
                                 *command->d_timeout + bdetu_SystemTime::now(),
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
   case TWVO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->timedWritev(oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case TWVI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->timedWritev(ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case TWVOA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->timedWritev(&augStatus,
                                oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case TWVIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->timedWritev(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
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
    case TWVRO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->timedWritevRaw(oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case TWVRI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->timedWritevRaw(ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdetu_SystemTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case TWVROA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->timedWritevRaw(&augStatus,
                               oBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdetu_SystemTime::now(),
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case TWVRIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->timedWritevRaw(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdetu_SystemTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case HELP_READ: {   //
        rCode = bteso_SocketImpUtil::read( buffer,
                                           socketPair[1],
                                           command->numToProcess.d_numBytes);
        ASSERT(rCode == command->numToProcess.d_numBytes);
        if (verbose) {
            QT("help socket read bytes = ");  P_T(rCode);
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

    #ifdef BSLS_PLATFORM__OS_UNIX
    case SIGNAL: {
        // Create a thread to generate signals.
        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_signalIoFlag;
        info->d_ioType = ioType;

        bcemt_Attribute attributes;
        int ret = bcemt_ThreadUtil::create(threadHandle,
                                           attributes,
                                           threadSignalGenerator,
                                           info);
        ASSERT(0 == ret);
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
        if (ret) {
            if (verbose) {
                QT("Thread creation failed, return value: ");
                PT(ret);
            }
        }
    } break;
    #endif
    case CLOSE_CONTROL: {
        // Create a thread to close the control socket (the peer socket).
        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;
        info->d_ioType = ioType;

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
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    } break;
    case CLOSE_OBSERVE: {
        bcemt_ThreadUtil::Handle tid = bcemt_ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[0];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;
        info->d_ioType = ioType;

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
        // This is necessary to "wait" for the child thread to extract the
        // passed data.
#ifdef BSLS_PLATFORM__OS_LINUX
        bcemt_ThreadUtil::microSleep(5 * SLEEP_TIME);
#else
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
#endif

    } break;
    default:
        ASSERT("The test command is wrong!" && 0);
        break;
    }
    return rCode;
}

static
int processTest(btesos_TcpTimedChannel           *channel,
                bteso_SocketHandle::Handle       *helperSocket,
                TestCommand                      *commands,
                char                             *buffer,
                btes_Iovec                       *ioBuffer,
                btes_Ovec                        *oBuffer,
                int                               ioType)
    // The specified 'numCommands' of test commands will be issued in the
    // specified 'commands' to invoke functions in the specified 'channel',
    // or the 'helperSocket' which is the control part of the socket pair.
    // If the 'signals' is set, a thread taking the specified 'threadFunction'
    // function will be generated to generate signals.  Results after each
    // test will be compared against those expected which are also specified
    // in the specified 'commands'.  For a read operation, load either
    // 'buffer' or 'ioBuffer' corresponding to the request.  Note because the
    // behaviors are different for "write" operations on different platform,
    // the specified 'ioType' is to indicate that the type of the function
    // being tested, e.g. it's a "read" or "write" operation so that different
    //  testing configuration can be set on a specific platform.  For a write
    // operation, write data from either 'buffer' or 'ioBuffer' or 'oBuffer'
    // corresponding to the request.  Return 0 on success, and a non-zero
    // value otherwise.
{
    bcemt_ThreadUtil::Handle threadHandle;
    int ret = 0, numErrors = 0;

    for (int i = 0; i < MAX_CMD; i++) { // different test data
        if (commands[i].d_command < 1) {
            break;
        }
        ret = testExecutionHelper(channel,
                                  &commands[i],
                                  helperSocket,
                                  &threadHandle,
                                  buffer,
                                  ioBuffer,
                                  oBuffer,
                                  ioType);
        if (verbose) {
            QT("In processTest:");  P_T(i);  PT(ret);
        }
        if (commands[i].d_command < READ_FUNCTIONS) { // "Read" operations
            if (INTERRUPTED == commands[i].d_expStatus) {
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0);
            }
            else {
                LOOP_ASSERT(commands[i].d_lineNum,
                                ret == commands[i].d_expReturnValue);
            }
        }
        else if (commands[i].d_command < WRITE_FUNCTIONS){
            if (ret < 0) {
                LOOP_ASSERT(commands[i].d_lineNum,
                            ret == commands[i].d_expReturnValue);
            }
            else if (commands[i].d_command >= WVO &&
                      commands[i].d_command <= TWVROA) {
                int length = btes_IovecUtil::length(oBuffer,
                                      commands[i].numToProcess.d_numBuffers);
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0 && ret <= length);
            }
            else if (commands[i].d_command >= WVI &&
                     commands[i].d_command <= TWVRIA) {
                int length = btes_IovecUtil::length(ioBuffer,
                                      commands[i].numToProcess.d_numBuffers);
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0 && ret <= length);
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
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
    }
    bcemt_ThreadUtil::join(threadHandle);

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

#ifdef BSLS_PLATFORM__OS_UNIX
    registerSignal(SIGPIPE, signalHandler); // register a handler for SIGPIPE.
    registerSignal(SIGSYS, signalHandler); // register a handler for SIGSYS.
    // A write() on the closed socket will generate SIGPIPE.
#endif

    testAllocator.setNoAbort(1);
    testAllocator.setVerbose(veryVeryVerbose);

    bteso_SocketImpUtil::startup(&errCode);

    bteso_InetStreamSocketFactory<bteso_IPv4Address> factory(&testAllocator);

    switch (test) { case 0:
      case 22: {
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
            // Firstly, a pair of sockets connecting each other on the local
            // host are created for our example, which could be any connected
            // sockets on different hosts.  The channel only needs one of the
            // socket as its I/O request endpoint, while the other end of
            // connection will be used to write some data for the channel to
            // read.

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

            // Next, create a 'bteso_StreamSocket' object, which is a part of
            // the channel.  The 'bteso_StreamSocket' object has a field of
            // type 'bteso_SocketHandle::Handle', whose value is set to a
            // socket created above.
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

                // We need to set a timeout value which is relative to the
                // current system time.
                bdet_TimeInterval timer = bdetu_SystemTime::now();
                int milliSec = 100, nanoSec = 400;
                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                len = channel.timedRead(&augStatus, readBuf,
                                        numBytes, timer, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 <= augStatus);
                }

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 20;
                augStatus = -1;
                // Try reading 20 bytes from the channel with a timeout value.
                // The timeout will be reached since no enough data in the
                // channel.
                len = channel.timedRead(&augStatus, (char*) readBuf,
                                        numBytes, timer, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 == augStatus);
                }
                // Write 1 byte to the channel.
                numBytes = 1;
                augStatus = -1;
                len = channel.write(&augStatus, writeBuf,
                                    numBytes, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 < augStatus);
                }

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes to the channel with a timeout value.
                len = channel.timedWrite(&augStatus, writeBuf,
                                         numBytes, timer, interruptFlag);
                if (len != numBytes) {
                    ASSERT(0 <= augStatus);
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

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes from the channel with a timeout value.
                len = channel.timedRead(&augStatus, readBuf,
                                        numBytes, timer, interruptFlag);
                ASSERT(INVALID == len);
                // Try writing 1 byte to the channel.
                numBytes = 1;
                augStatus = -1;
                len = channel.write(&augStatus, writeBuf,
                                    numBytes, interruptFlag);
                ASSERT(INVALID == len);

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes to the channel with a timeout value.
                len = channel.timedWrite(&augStatus, writeBuf,
                                         numBytes, timer, interruptFlag);
                ASSERT(INVALID == len);
            }
            factory.deallocate(sSocket);
            if (verbose) {
                QT("The usage example is over.");
            }
        }
      } break;
      case 21: {
#if !defined(BSLS_PLATFORM__OS_LINUX) && !(defined(BSLS_PLATFORM__OS_SOLARIS) \
                                           || BSLS_PLATFORM__OS_VER_MAJOR < 10)
        // -------------------------------------------------------------------
        // TESTING 'timedWritevRaw' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchrouous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedWritevRaw(const btes_Ovec         *buffer,
        //                      int                      numBytes,
        //                      const bdet_TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btes_Ovec         *buffer,
        //                      int                      numBytes,
        //                      const bdet_TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btes_Iovec        *buffer,
        //                      int                      numBytes,
        //                      const bdet_TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btes_Iovec        *buffer,
        //                      int                      numBytes,
        //                      const bdet_TimeInterval& time,
        //                      int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("TESTING 'timedWriteRaw' METHOD");
            QT("==============================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(2, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWVRO,          1,        0,          1,         0,   &longTime },
      {L_,      W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_,    TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  TWVRO,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TWVRO,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVROA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 2: the same test as the above for "btes_Iovec" buffers
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWVRI,          1,        0,          1,         0,   &longTime },
      {L_,      W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_,    TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  TWVRI,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TWVRI,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVIA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 3: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVROA,          1,        0,          1,         0,   &longTime },
      {L_,      W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_,    TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_, TWVROA,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TWVRO,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVROA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 4: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWVRIA,        1,        0,          1,         0,   &longTime },
      {L_,      W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_,     TWR,     BUF_WRITE,    0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  TWVRIA,         7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_, TWVRIA,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVRI,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 5: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRO,          1,        0,           1,        0,   &longTime },
      {L_,   TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVRO,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVRO,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVRI,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 6: the same test as the above for "btes_Iovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRI,          1,        0,           1,        0,   &longTime },
      {L_,   TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVRI,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVRI,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVRO,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 7: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVROA,          1,        0,           1,        0,   &longTime },
      {L_,   TWR,     BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,   0,        0,           0,        0,   &longTime },
      {L_, TWVROA,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVROA,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVRIA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 8: the same test as the above for "btes_Ovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRIA,          1,        0,           1,        0,   &longTime },
      {L_,    TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,   0,        0,           0,        0,   &longTime },
      {L_, TWVRIA,          7,        0,         ERR,        0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVRIA,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVROA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },
    #endif
    #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
    {
      {L_,    TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWVROA,        1,         0,           1,         0,   &timeout1 },
      {L_,  TWVROA,        7,         0,           0,         0,   &timeout2 },
      {L_,    -1,          0,         0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWVRO,         1,         0,           1,         0,   &timeout1 },
      {L_,  TWVRO,         7,         0,           0,         0,   &timeout2 },
      {L_,   -1,           0,         0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWVRIA,        1,         0,           1,         0,   &timeout1 },
      {L_,  TWVRIA,        7,         0,           0,         0,   &timeout2 },
      {L_,    -1,          0,         0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWVRI,         1,         0,           1,         0,   &timeout1 },
      {L_,  TWVRI,         7,         0,           0,         0,   &timeout2 },
      {L_,   -1,           0,         0,           0,         0,   &longTime },
    },

    #else                                       // unix test data

    #ifdef BSLS_PLATFORM__OS_SOLARIS
    // commands set 9: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,     WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVRO,          7,        1,         8192,       0,   &longTime },
      {L_,  SIGNAL,         2,  READ_OP,          0,         0,   &longTime },
      {L_,  TWVRO,          7,        1,      40960,         0,   &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,     WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVRI,          7,        1,         8192,       0,   &longTime },
      {L_,  SIGNAL,         2,  READ_OP,          0,         0,   &longTime },
      {L_,  TWVRI,          7,        1,      40960,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,     WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_, TWVROA,          7,        1,         8192,       0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,            0,       0,   &longTime },
      {L_, TWVROA,          7,        1,         40960,      0,   &longTime },
    },

    // commands set 12: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,     WR,   BUF_WRITE,       0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVRIA,         7,        1,         8192,       0,   &longTime },
      {L_,  SIGNAL,         2,  READ_OP,            0,       0,   &longTime },
      {L_,  TWVRIA,         7,        1,        40960,       0,   &longTime },
    },
    #else
    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVO,           1,        0,            1,       0,   &longTime },
      {L_,     W, SYS_DEPENDENT_LEN,  0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRO,           7,        1,         1024,       0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,            0,       0,   &longTime },
      {L_, TWVRO,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVI,           1,        0,            1,       0,   &longTime },
      {L_,     W, SYS_DEPENDENT_LEN,  0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRI,           7,        1,         1024,       0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,            0,       0,   &longTime },
      {L_, TWVRI,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVOA,           1,        0,            1,       0,   &longTime },
      {L_,     W, SYS_DEPENDENT_LEN,   0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVROA,           7,        1,         1024,       0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,            0,       0,   &longTime },
      {L_, TWVROA,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVIA,           1,        0,            1,       0,   &longTime },
      {L_,     W, SYS_DEPENDENT_LEN,   0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRIA,           7,        1,         1024,       0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,            0,       0,   &longTime },
      {L_, TWVRIA,           7,        1,         1024,       0,   &longTime },
    },

    #endif

    // commands set 13: The "write" operation reaches its timeout.
    {
      {L_,  TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVRO,           7,        0,        8192,        0,   &longTime },
      {L_,  TWVRO,           7,        0,           0,        0,   &timeout1 },
    },
     // commands set 14: The "write" operation reaches its timeout.
    {
      {L_,  TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVRI,           7,        0,        8192,        0,   &longTime },
      {L_,  TWVRI,           7,        0,           0,        0,   &timeout1 },
    },
    // commands set 15: The "write" operation reaches its timeout.
    {
      {L_,  TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVROA,          7,        0,        8192,        0,   &longTime },
      {L_,  TWVROA,          7,        0,           0,        0,   &timeout1 },
    },
    // commands set 16: The "write" operation reaches its timeout.
    {
      {L_,  TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVROA,          7,        0,        8192,        0,   &longTime },
      {L_,  TWVROA,          7,        0,           0,        0,   &timeout1 },
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
                char buf0[WVECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     #ifdef BSLS_PLATFORM__OS_SOLARIS
                         buf4[WVECBUF_LEN80] = "\0",
                         buf5[WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[WVECBUF_LEN500] = "\0",
                         buf5[WVECBUF_LEN1K] = "\0",
                     #endif
                     buf6[WVECBUF_LEN16K] = "\0",
                     buf7[WVECBUF_LEN32K] = "\0";

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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf,
                                            iovecBuffer,
                                            ovecBuffer,
                                            ioType) );
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
      case 20: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
        // -------------------------------------------------------------------
        // TESTING 'timedWritev' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedWritev(const btes_Ovec          *buffer,
        //                   int                       numBytes,
        //                   const bdet_TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btes_Ovec          *buffer,
        //                   int                       numBytes,
        //                   const bdet_TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btes_Iovec         *buffer,
        //                   int                       numBytes,
        //                   const bdet_TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btes_Iovec         *buffer,
        //                   int                       numBytes,
        //                   const bdet_TimeInterval&  time,
        //                   int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWritev' method");
            QT("============================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(2, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWVO,          1,        0,          1,         0,   &longTime },
      {L_,     TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,   TWVO,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWVO,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVOA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 2: the same test as the above for "btes_Iovec" buffers
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWVI,          1,        0,          1,         0,   &longTime },
      {L_,     TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,   TWVI,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWVI,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVIA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 3: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWVOA,          1,        0,          1,         0,   &longTime },
      {L_,     TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  TWVOA,          7,        0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWVO,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVOA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 4: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWVIA,         1,        0,          1,         0,   &longTime },
      {L_,     TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  TWVIA,          7, 0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TWVIA,          2,        0,        INVALID,     0,   &longTime },
      {L_,   TWVI,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 5: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWVO,          1,        0,           1,        0,   &longTime },
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  TWVO,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,  TWVO,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVI,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 6: the same test as the above for "btes_Iovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWVI,          1,        0,           1,        0,   &longTime },
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  TWVI,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,  TWVI,          2,        0,        INVALID,     0,   &longTime },
      {L_,  TWVO,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 7: the same test as the above for "btes_Ovec" buffers
    // with 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVOA,          1,        0,           1,        0,   &longTime },
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVOA,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVOA,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVIA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 8: the same test as the above for "btes_Ovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVIA,          1,        0,           1,        0,   &longTime },
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVIA,          7,        0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_, TWVIA,          2,        0,        INVALID,     0,   &longTime },
      {L_, TWVOA,          6,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif

    #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
    {
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVOA,        1,          0,           1,         0,   &timeout1 },
      {L_,  TWVOA,        7,          0,           30,        0,   &timeout2 },
      {L_,    -1,         0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVO,         1,          0,           1,         0,   &timeout1 },
      {L_,  TWVO,         7,          0,           30,        0,   &timeout2 },
      {L_,   -1,          0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVIA,        1,          0,           1,         0,   &timeout1 },
      {L_,  TWVIA,        7,          0,           30,        0,   &timeout2 },
      {L_,    -1,         0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,  TWVI,         1,          0,           1,         0,   &timeout1 },
      {L_,  TWVI,         7,          0,           30,        0,   &timeout2 },
      {L_,   -1,          0,          0,           0,         0,   &longTime },
    },

    #else                                       // unix test data

    #ifdef BSLS_PLATFORM__OS_SOLARIS

    // commands set 9: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVO,          1,        0,          1,         0,   &longTime },
      {L_,  TWVO,          5,        0,        164,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,  TWVO,          7,        1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWVI,          1,        0,          1,         0,   &longTime },
      {L_,  TWVI,          5,        0,        164,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,  TWVI,          7,        1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 11: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_, TWVOA,          1,        0,          1,         0,   &longTime },
      {L_, TWVOA,          5,        0,        164,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_, TWVOA,          7,        1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 12: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_, TWVIA,          1,        0,          1,         0,   &longTime },
      {L_, TWVIA,          5,        0,        164,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_, TWVIA,          7,        1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 13: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,    READ_OP,        0,         0,   &longTime },
      {L_,  TWVO,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 14: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,    READ_OP,        0,         0,   &longTime },
      {L_,  TWVI,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 15: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,    READ_OP,        0,         0,   &longTime },
      {L_, TWVOA,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 16: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,    READ_OP,        0,         0,   &longTime },
      {L_,  TWVIA,         7,        0,      24740,         0,   &longTime },
    },

    #elif BSLS_PLATFORM__OS_LINUX

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_, TWVO,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            7,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_, TWVI,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            7,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_, TWVOA,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            7,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_, TWVIA,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVIA,            7,        1,         0, INTERRUPTED, &longTime },
    },

    // commands set 9: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 10: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_,     W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,   READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 12: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_,     W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,   READ_OP,        0,         0,   &longTime },
      {L_, TWVIA,            6,        0,      1608,         0,   &longTime },
    },
    #else

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_, TWVO,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_, TWVI,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_, TWVOA,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            6,        1,         0, INTERRUPTED, &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_, TWVIA,            5,        0,       164,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVIA,            6,        1,         0, INTERRUPTED, &longTime },
    },

    // commands set 9: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 10: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_,    W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,  READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_,     W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,          2,   READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 12: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_,     W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,           2,   READ_OP,        0,         0,   &longTime },
      {L_, TWVIA,            6,        0,      1608,         0,   &longTime },
    },
    #endif

    // commands set 17: The "write" operation reaches its timeout.
    {
      {L_,    TW,    BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVO,            7,       1,       8192,         0,   &timeout1 },
    },

    // commands set 18: The "write" operation reaches its timeout.
    {
      {L_,    TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  TWVI,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 19: The "write" operation reaches its timeout.
    {
      {L_,    TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_, TWVOA,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 20: The "write" operation reaches its timeout.
    {
      {L_,    TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_, TWVIA,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 21: For "write" operation in 'non-interruptible' mode,
    // "AE" can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  TWVO,          6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 22: For "write" operation in 'non-interruptible' mode,
    // "AE" can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  TWVI,          6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 23: For "write" operation in 'non-interruptible' mode,
    // "AE" can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  TWVOA,         6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 24: For "write" operation in 'non-interruptible' mode,
    // "AE" can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  TWVIA,         6,        0,       8192,         0,   &timeout2 },
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
                char buf0[WVECBUF_LEN1], buf1[VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     #ifdef BSLS_PLATFORM__OS_SOLARIS
                         buf4[WVECBUF_LEN80] = "\0",
                         buf5[WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[WVECBUF_LEN500] = "\0",
                         buf5[WVECBUF_LEN1K] = "\0",
                     #endif
                     buf6[WVECBUF_LEN16K] = "\0",
                     buf7[WVECBUF_LEN32K] = "\0";

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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf,
                                            iovecBuffer,
                                            ovecBuffer,
                                            ioType) );
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
      case 19: {
#if !defined(BSLS_PLATFORM__OS_LINUX) && !(defined(BSLS_PLATFORM__OS_SOLARIS) \
                                           || BSLS_PLATFORM__OS_VER_MAJOR < 10)
        // -------------------------------------------------------------------
        // TESTING 'timedWriteRaw' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedWriteRaw(const char              *buffer,
        //                     int                      numBytes,
        //                     const bdet_TimeInterval& time,
        //                     int                      flags = 0);
        //   int timedWriteRaw(int                     *augStatus,
        //                     const char              *buffer,
        //                     int                      numBytes,
        //                     const bdet_TimeInterval& time,
        //                     int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWrite' method");
            QT("===========================");
        }

        {
            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(2, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWR,          1,        0,          1,         0,   &longTime },
      {L_,   TW,  SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      {L_,   TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,  0,        0,          0,         0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWR,        100,        0,        INVALID,     0,   &longTime },
      {L_,   TWR,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWRA,          1,        0,           1,        0,   &longTime },
      {L_,   TW,  SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      {L_,  TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,  0,        0,           0,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN, 0,         CLOSED,     0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TWRA,        100,        0,        INVALID,     0,   &longTime },
      {L_,   TWR,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWR,          1,        0,           1,        0,   &longTime },
      {L_,   TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
         // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWR,        100,        0,       INVALID,      0,   &longTime },
      {L_,  TWRA,         60,        0,       INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWRA,          1,        0,           1,        0,   &longTime },
      {L_,  TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
       // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TWR,        100,        0,        INVALID,     0,   &longTime },
      {L_,  TWRA,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif
    #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
    {
      {L_,  TWRA,        1,          0,           1,         0,   &longTime },
      {L_,  TWRA,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,    -1,        0,          0,           0,         0,   &longTime },
    },
    {
      {L_,  TWR,         1,          0,           1,         0,   &longTime },
      {L_,  TWR,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
      {L_,  TWR, SYS_DEPENDENT_LEN,  0,           0,         0,   &timeout2 },
      {L_,   -1,         0,          0,           0,         0,   &longTime },
    },

    #else                                       // unix test data
    #ifdef BSLS_PLATFORM__OS_SOLARIS
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWR,          1,        0,           1,        0,   &longTime },
      {L_,   TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWR,        100,        0,       INVALID,      0,   &longTime },
      {L_,  TWRA,         60,        0,       INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  TWRA,          1,        0,           1,        0,   &longTime },
      {L_,  TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TWR,        100,        0,        INVALID,     0,   &longTime },
      {L_,  TWRA,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,   TWR,          1,        0,          1,         0,   &longTime },
      {L_,   TWR,         15,        0,         15,         0,   &longTime },
      {L_,   TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,       8192,         0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,    READ_OP,        0,         0,   &longTime },
      {L_,   TWR,     BUF_WRITE,     1,       8192,         0,   &longTime },
    },
    // commands set 6: to resolve concern
    {
      // Each request read expected number of bytes from the channel.
      {L_,  TWRA,          1,        0,          1,         0,   &longTime },
      {L_,  TWRA,         15,        0,         15,         0,   &longTime },
      {L_,  TWRA,    BUF_WRITE,      0,     BUF_WRITE,      0,   &longTime },
      {L_,   TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,     READ_OP,       0,         0,   &longTime },
      {L_,  TWRA,      BUF_WRITE,    1,       8192,         0,   &longTime },
    },
    #else   // ibm test data
    // commands set 3: to resolve concern 4 - 6.
    {
      {L_, TWR,            1,        0,           1,        0,   &longTime },
      {L_,   W,   SYS_DEPENDENT_LEN, 1,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, TWR,    BUF_WRITE,        0,        1024,        0,   &longTime },
      {L_, SIGNAL,          2,   READ_OP,         0,        0,   &longTime },
      {L_, TWR,    BUF_WRITE,        1,        1024,        0,   &longTime },
    },

    // commands set 4: to resolve concern 4 - 6.
    {
      {L_, TWRA,            1,        0,           1,        0,   &longTime },
      {L_,    W,  SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, TWRA,    BUF_WRITE,        0,        1024,        0,   &longTime },
      {L_, SIGNAL,          2,   READ_OP,          0,        0,   &longTime },
      {L_, TWRA,    BUF_WRITE,        1,        1024,        0,   &longTime },
    },

    #endif

    // commands set 5: The "write" operation reaches its timeout.
    {
      {L_,  TWRA,   BUF_WRITE,        0,        1024,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN,  0,        1024,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN,  0,           0,        0,   &timeout1 },
    },
    // commands set 6: The "write" operation reaches its timeout.
    {
      {L_,  TWR,   BUF_WRITE,         0,        1024,        0,   &longTime },
      {L_,  TWR, SYS_DEPENDENT_LEN,   0,        1024,        0,   &longTime },
      {L_,  TWR, SYS_DEPENDENT_LEN,   0,           0,        0,   &timeout1 },
    },
    // commands set 7: The "write" operation reaches its timeout.
    {
      {L_,  TWRA,   BUF_WRITE,        0,        1024,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN,  1,        1024,        0,   &longTime },
      {L_,  TWRA, SYS_DEPENDENT_LEN,  1,           0,        0,   &timeout1 },
    },
    // commands set 8: The "write" operation reaches its timeout.
    {
      {L_,  TWR,   BUF_WRITE,         0,        1024,        0,   &longTime },
      {L_,  TWR, SYS_DEPENDENT_LEN,   1,        1024,        0,   &longTime },
      {L_,  TWR, SYS_DEPENDENT_LEN,   1,           0,        0,   &timeout1 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, ioType) );
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
      case 18: {
#if !defined(BSLS_PLATFORM__OS_LINUX) && !(defined(BSLS_PLATFORM__OS_SOLARIS) \
                                           || BSLS_PLATFORM__OS_VER_MAJOR < 10)
        // -------------------------------------------------------------------
        // TESTING 'timedWrite' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" write
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both write methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedWrite(const char              *buffer,
        //                  int                      numBytes,
        //                  const bdet_TimeInterval& time,
        //                  int                      flags = 0);
        //   int timedWrite(int                     *augStatus,
        //                  const char              *buffer,
        //                  int                      numBytes,
        //                  const bdet_TimeInterval& time,
        //                  int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWrite' method");
            QT("===========================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(2, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,    TW,          1,        0,          1,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,  0,        0,          0,         0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 0,        CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,    TW,        100,        0,        INVALID,     0,   &longTime },
      {L_,    TW,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWA,          1,        0,           1,        0,   &longTime },
      {L_,   TWA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,  0,        0,           0,        0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 0,         CLOSED,     0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TWA,        100,        0,        INVALID,     0,   &longTime },
      {L_,   TWA,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,    TW,          1,        0,           1,        0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,    TW,        100,        0,       INVALID,      0,   &longTime },
      {L_,    TW,         60,        0,       INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   TWA,          1,        0,           1,        0,   &longTime },
      {L_,   TWA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 0,         ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TWA,        100,        0,        INVALID,     0,   &longTime },
      {L_,   TWA,         60,        0,        INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif

    #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
    {
      {L_,   TWA,        1,          0,           1,         0,   &longTime },
      {L_,   TWA,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,    -1,        0,          0,           0,         0,   &longTime },
    },
    {
      {L_,   TW,        1,          0,           1,         0,   &longTime },
      {L_,   TW,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
      {L_,   TW, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,   -1,        0,          0,           0,         0,   &longTime },
    },

    #else

    #if defined(BSLS_PLATFORM__OS_SOLARIS)      // SUN test data

    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,          1,        0,          1,         0,   &longTime },
      {L_,    TW,         15,        0,         15,         0,   &longTime },
      {L_,    TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 1,       8192, INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern
    {
      // Each request read expected number of bytes from the channel.
      {L_,   TWA,          1,        0,          1,         0,   &longTime },
      {L_,   TWA,         15,        0,         15,         0,   &longTime },
      {L_,   TWA,    BUF_WRITE,      0,     BUF_WRITE,      0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,   TWA,       10000,       1,       8192, INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_,   TWA,     BUF_WRITE,     0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,    TW,       10000,       1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 8: to resolve concern 4 - 6.
    {
      {L_,   TWA,    BUF_WRITE,      0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,       2,         0,        0,           0,   &longTime },
      {L_,   TWA,       10000,       1,      8192, INTERRUPTED,  &longTime },
    },
    // commands set 9: to resolve concern 4 - 6.
    {
      {L_,   TWA,    BUF_WRITE,      0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,       2,      READ_OP,     0,           0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN,  0,   &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      {L_,   TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,       2,      READ_OP,     0,           0,   &longTime },
      {L_,   TWA,       10000,       0,      10000,         0,   &longTime },
    },
    // commands set 15: The "write" operation in "non_interruptible" mode
    // reaches its timeout w/o being interrupted by "asynchronous events"
    // during that period.
    {
      {L_,   TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,       2,      READ_OP,     0,           0,   &longTime },
      {L_,   TWA,   BUF_WRITE,       0,      49152,         0,   &timeout2 },
    },
    // commands set 16:  The "write" operation in "non_interruptible" mode
    // reaches its timeout w/o being interrupted by "asynchronous events"
    // during that period.
    {
      {L_,   TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  SIGNAL,       2,      READ_OP,     0,           0,   &longTime },
      {L_,   TWA,   BUF_WRITE,       0,     49152,          0,   &timeout2 },
    },

    #else         // ibm test data

    // commands set 3: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    TW,          1,        0,          1,         0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,    TW, SYS_DEPENDENT_LEN, 1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 4: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,   TWA,          1,        0,          1,         0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
        // There are not enough space in the TCP buffer for next
        // request, now we'll generate signals to interrupt it.
      {L_,  SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 1,       8192, INTERRUPTED, &longTime },
    },

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_,   TWA,          1,         0,          1,        0,   &longTime },
      {L_,   TWA,  SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,         2,      READ_OP,       0,        0,   &longTime },
      {L_,   TWA,       2000,         0,       2000,        0,   &longTime },
    },

    // commands set 6: to resolve concern 4 - 6.
    {
      {L_,    TW,          1,         0,          1,        0,   &longTime },
      {L_,    TW,  SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, SIGNAL,         2,      READ_OP,       0,        0,   &longTime },
      {L_,    TW,       2000,         0,       2000,        0,   &longTime },
    },

    #endif

    // commands set 11: The "write" operation reaches its timeout.
    {
      {L_,   TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 0,        1024,        0,   &timeout1 },
    },
    // commands set 12: The "write" operation reaches its timeout.
    {
      {L_,   TW,   BUF_WRITE,        0,    BUF_WRITE,       0,   &longTime },
      {L_,   TW, SYS_DEPENDENT_LEN,  0,        1024,        0,   &timeout1 },
    },
    // commands set 13: The "write" operation reaches its timeout.
    {
      {L_,   TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,   TWA, SYS_DEPENDENT_LEN, 1,        8192,        0,   &timeout1 },
    },
    // commands set 14: The "write" operation reaches its timeout.
    {
      {L_,   TW,   BUF_WRITE,        0,    BUF_WRITE,       0,   &longTime },
      {L_,   TW, SYS_DEPENDENT_LEN,  1,        8192,        0,   &timeout1 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, ioType) );
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
      case 17: {
        // -------------------------------------------------------------------
        // TESTING 'timedBufferedReadRaw' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedBufferedReadRaw(char                    **buffer,
        //                            int                       numBytes,
        //                            const bdet_TimeInterval&  time,
        //                            int                       flags = 0);
        //   int timedBufferedReadRaw(int                      *augStatus,
        //                            char                    **buffer,
        //                            int                       numBytes,
        //                            const bdet_TimeInterval&  time,
        //                            int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedBufferedReadRaw' method");
            QT("=====================================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,      0     },
      {L_,  TRBR,           1,         0,           1,         0,  &longTime },
      {L_,  TRBR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,  TRBR,         100,         0,         CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TRBR,         100,         0,        INVALID,      0,  &longTime },
      {L_, TRBRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_, TRBRA,          10,         0,          10,         0,  &longTime },
      {L_, TRBRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_, TRBRA,         100,         0,         CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,  TRBR,         100,         0,        INVALID,      0,  &longTime },
      {L_, TRBRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,  TRBR,           1,         0,           1,         0,  &longTime },
      {L_,  TRBR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_, TRBRA,          20,         0,         ERR,       0,  &longTime },
      {L_,  TRBR,          80,         0,        INVALID,      0,  &longTime },
      {L_, TRBRA,          40,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_, TRBRA,           1,         0,           1,         0,  &longTime },
      {L_, TRBRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_, TRBRA,          20,         1,         ERR,         0,  &longTime },
      {L_,  TRBR,          80,         1,        INVALID,      0,  &longTime },
      {L_, TRBRA,          40,         1,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,            0,       0   },
      {L_, TRBRA,           1,       0,          1,            0,  &longTime},
      {L_, TRBRA,          25,       0,         25,            0,  &longTime},
      {L_, TRBRA,          36,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,      0,           0,            0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,            0,       0   },
      {L_,  TRBR,           1,       0,          1,            0,  &longTime},
      {L_,  TRBR,          25,       0,         25,            0,  &longTime},
      {L_,  TRBR,          36,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data

    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,  TRBR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll
         // return with the remaining data in the internal buffer.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_,  TRBR,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_, TRBR,           50,         0,          50,        0,  &longTime },
       {L_,  RBR,           70,         0,          50,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_, TRBRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_, TRBRA,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_, TRBRA,          50,         0,          50,        0,  &longTime },
       {L_,   RBR,          70,         0,          50,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,    50,         0,          50,        0,  &longTime },
       {L_,  TRBR,           1,         0,           1,        0,  &longTime },
       {L_, TRBRA,          10,         0,          10,        0,  &longTime },
       {L_,  TRBR,          15,         0,          15,        0,  &longTime },
       {L_, TRBRA,          24,         0,          24,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  SIGNAL,        1,         1,           0,        0,  &longTime },
       {L_, TRBRA,         125,         1,         100,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_,  SIGNAL,         1,         1,          0,        0,   &longTime },
       {L_, TRBR,          250,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_,   SIGNAL,        1,   WRITE_OP,          0,        0,  &longTime },
       {L_, TRBRA,         125,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_, TRBR,          150,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
       {L_,  TRBR,           1,         0,           1,        0,  &longTime },
       {L_, TRBRA,          10,         0,          10,        0,  &longTime },
       {L_,  TRBR,          15,         0,          15,        0,  &longTime },
       {L_, TRBRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_, TRBRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_,  TRBR,          25,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, TRBRA,          25,         1,           0,        0,  &timeout1 },
         //
       {L_,  TRBR,          25,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,  TRBRA,        125,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,  TRBR,         150,         0,         100,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType) );
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
      case 16: {
        // -------------------------------------------------------------------
        // TESTING 'timedReadvRaw' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedReadvRaw(btes_Iovec              *buffer,
        //                     int                      numBytes,
        //                     const bdet_TimeInterval& time,
        //                     int                      flags = 0);
        //   int timedReadvRaw(int                     *augStatus,
        //                     btes_Iovec              *buffer,
        //                     int                      numBytes,
        //                     const bdet_TimeInterval& time,
        //                     int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadvRaw' method");
            QT("==============================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,      0     },
      {L_,  TRVR,           1,         0,           1,         0,  &longTime },
      {L_,  TRVR,           7,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,  TRVR,           6,         0,         CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,  TRVR,           7,         0,        INVALID,      0,  &longTime },
      {L_, TRVRA,           6,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_, TRVRA,           1,         0,           1,         0,  &longTime },
      {L_, TRVRA,           7,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_, TRVRA,           6,         0,         CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,  TRVR,           7,         0,        INVALID,      0,  &longTime },
      {L_,  TRVA,           6,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,  TRVR,           1,         0,           1,         0,  &longTime },
      {L_,  TRVR,           7,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_, TRVRA,           6,         0,         ERR,         0,  &longTime },
      {L_,  TRVR,           4,         0,        INVALID,      0,  &longTime },
      {L_, TRVRA,           6,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_, TRVRA,           1,         0,           1,         0,  &longTime },
      {L_, TRVRA,           7,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_, TRVRA,           6,         1,         ERR,         0,  &longTime },
      {L_,  TRVR,           4,         1,        INVALID,      0,  &longTime },
      {L_, TRVRA,           6,         1,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,            0,       0   },
      {L_, TRVRA,           1,       0,          1,            0,  &longTime},
      {L_, TRVRA,           5,       0,         25,            0,  &longTime},
      {L_, TRVRA,           6,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,      0,           0,            0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,            0,       0   },
      {L_,  TRVR,           1,       0,          1,            0,  &longTime},
      {L_,  TRVR,           5,       0,         25,            0,  &longTime},
      {L_,  TRVR,           6,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data
    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,  TRVR,           2,         0,           4,        0,  &longTime },
       {L_,  TRVR,           3,         0,           9,        0,  &longTime },
       {L_,  TRVR,           6,         0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll
         // return with the remaining data in the internal buffer.
       {L_,  TRVR,          4,         0,            2,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_, TRVRA,           2,         0,           4,        0,  &longTime },
       {L_, TRVRA,           3,         0,           9,        0,  &longTime },
       {L_, TRVRA,           6,         0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_, TRVRA,           4,         0,           2,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,    50,         0,          50,        0,  &longTime },
       {L_,  TRVR,          1,         0,           1,        0,  &longTime },
       {L_, TRVRA,          2,         0,           4,        0,  &longTime },
       {L_,  TRVR,          4,         0,          16,        0,  &longTime },
       {L_, TRVRA,          5,         0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  TRVR,          3,         0,           4,        0,  &longTime },
       {L_, HELP_WRITE,    10,         0,          10,        0,  &longTime },
       {L_, TRVRA,          4,         1,          10,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_,  SIGNAL,         1,         1,          0,        0,   &longTime },
       {L_, TRVR,            8,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_,   SIGNAL,        1,   WRITE_OP,          0,        0,  &longTime },
       {L_, TRVRA,           8,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_, TRVR,            8,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
       {L_,  TRVR,           1,         0,           1,        0,  &longTime },
       {L_, TRVRA,           2,         0,           4,        0,  &longTime },
       {L_,  TRVR,           4,         0,          16,        0,  &longTime },
       {L_, TRVRA,           5,         0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_, TRVRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_,  TRVR,           5,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, TRVRA,           6,         1,           0,        0,  &timeout1 },
         //
       {L_,  TRVR,           7,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,  TRVRA,          8,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,  TRVR,           8,         0,         100,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int  ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0,
                                            ioType) );
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
      case 15: {
        // -------------------------------------------------------------------
        // TESTING 'timedBufferedRead' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedBufferedRead(char            **buffer,
        //                 int                       numBytes,
        //                 const bdet_TimeInterval&  time,
        //                 int                       flags = 0);
        //   int timedBufferedRead(int              *augStatus,
        //                 char                    **buffer,
        //                 int                       numBytes,
        //                 const bdet_TimeInterval&  time,
        //                 int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedBufferedRead' method");
            QT("==================================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,        -9,   0 },
      {L_,   TRB,           1,         0,           1,        -9,  &longTime },
      {L_,   TRB,          49,         0,          49,        -9,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,        -9,  &longTime },
      {L_,   TRB,         100,         0,         CLOSED,     -9,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TRB,         100,         0,        INVALID,     -9,  &longTime },
      {L_,  TRBA,          60,         0,        INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,        -9,  &longTime },
      {L_,  TRBA,          10,         0,          10,        -9,  &longTime },
      {L_,  TRBA,          40,         0,          40,        -9,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,        -9,  &longTime },
      {L_,  TRBA,         100,         0,         CLOSED,     -9,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TRB,         100,         0,        INVALID,     -9,  &longTime },
      {L_,  TRBA,          60,         0,        INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,        -9,  &longTime },
      {L_,   TRB,           1,         0,           1,        -9,  &longTime },
      {L_,   TRB,          49,         0,          49,        -9,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,        -9,  &longTime },
      {L_,  TRBA,          20,         0,         ERR,        -9,  &longTime },
      {L_,   TRB,          80,         0,        INVALID,     -9,  &longTime },
      {L_,  TRBA,          40,         0,        INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,        -9,  &longTime },
      {L_,  TRBA,           1,         0,           1,        -9,  &longTime },
      {L_,  TRBA,          49,         0,          49,        -9,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,        -9,  &longTime },
      {L_,  TRBA,          20,         1,         ERR,        -9,  &longTime },
      {L_,   TRB,          80,         1,        INVALID,     -9,  &longTime },
      {L_,  TRBA,          40,         1,        INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,        -9,       0   },
      {L_,  TRB,            1,       0,          1,        -9,  &longTime},
      {L_,  TRB,           25,       0,         25,        -9,  &longTime},
      {L_,  TRB,           36,       0,         24,        -9,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,     -9,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,        -9,       0   },
      {L_,   TRB,           1,       0,          1,        -9,  &longTime},
      {L_,   TRB,          25,       0,         25,        -9,  &longTime},
      {L_,   TRB,          36,       0,         24,        -9,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,     -9,       0   },
    },

    #else             // unix data

    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,       -9,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,       -9,  &longTime },
       {L_,    RB,          80,         1,          50,       -9,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,   TRB,          40,         0,          40,       -9,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,       -9,  &longTime },
       {L_,   TRB,          15,         0,          15,       -9,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,   TRB,          50,         0,          50,       -9,  &longTime },
       {L_,   RBR,          50,         0,          45,       -9,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,       -9,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,       -9,  &longTime },
       {L_,    RB,          80,         1,          50,       -9,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,  TRBA,          40,         0,          40,       -9,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,       -9,  &longTime },
       {L_,  TRBA,          15,         0,          15,       -9,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,  TRBA,          50,         0,          50,       -9,  &longTime },
       {L_,   RBR,          50,         0,          45,       -9,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,       -9,  &longTime },
       {L_,   TRB,           1,         0,           1,       -9,  &longTime },
       {L_,  TRBA,          10,         0,          10,       -9,  &longTime },
       {L_,   TRB,          15,         0,          15,       -9,  &longTime },
       {L_,  TRBA,          20,         0,          20,       -9,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  SIGNAL,         1,         0,           0,       -9,  &longTime },
       {L_,  TRBA,           5,         1,           4, INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,      1,         1,          1,        -9,  &longTime },
       {L_,  SIGNAL,         1,         0,          0,        -9,  &longTime },
       {L_,   TRB,           9,         1,          5, INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_, HELP_WRITE,     20,         0,          20,       -9,  &longTime },
       {L_,   SIGNAL,        1,   WRITE_OP,          0,       -9,  &longTime },
       {L_,   TRBA,         25,         0,          25,       -9,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,       -9,  &longTime },
       {L_,   TRB,         150,         0,         150,       -9,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,       -9,  &longTime },
       {L_,   TRB,           1,         0,           1,       -9,  &longTime },
       {L_,  TRBA,          10,         0,          10,       -9,  &longTime },
       {L_,   TRB,          15,         0,          15,       -9,  &longTime },
       {L_,  TRBA,          20,         0,          20,       -9,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  TRBA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,      1,         1,           1,       -9,  &longTime },
       {L_,   TRB,          25,         0,           5,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, HELP_WRITE,     15,         1,          15,       -9,  &longTime },
       {L_,  TRBA,          25,         1,          20,        0,  &timeout1 },
         //
       {L_, HELP_WRITE,      3,         1,           3,       -9,  &longTime },
       {L_,   TRB,          25,         1,          23,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_, HELP_WRITE,     20,         0,          20,       -9,  &longTime },
       {L_,   SIGNAL,        1,  WRITE_OP,           0,       -9,  &longTime },
       {L_,   TRBA,        225,         0,         143,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_, HELP_WRITE,     30,         0,          30,       -9,  &longTime },
       {L_,   SIGNAL,        1,         0,           0,       -9,  &longTime },
       {L_,   TRB,         450,         0,         173,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType) );
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
      case 14: {
#if !defined(BSLS_PLATFORM__CMP_GNU) || (BSLS_PLATFORM__CMP_VER_MAJOR < 40000)
        // -------------------------------------------------------------------
        // TESTING 'timedReadv' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedReadv(btes_Iovec              *buffer,
        //                  int                      numBytes,
        //                  const bdet_TimeInterval& time,
        //                  int                      flags = 0);
        //   int timedReadv(int                     *augStatus,
        //                  btes_Iovec              *buffer,
        //                  int                      numBytes,
        //                  const bdet_TimeInterval& time,
        //                  int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadv' method");
            QT("===========================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,          50,         0,   0 },
      {L_,   TRV,          1,       0,           1,         0,  &longTime },
      {L_,   TRV,          5,       0,          25,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,       0,           0,         0,  &longTime },
      {L_,   TRV,           6,       0,         CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,    TRV,          7,       0,        INVALID,      0,  &longTime },
      {L_,   TRVA,          6,       0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,  TRVA,           1,       0,           1,         0,  &longTime },
      {L_,  TRVA,           5,       0,          25,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,       0,           0,         0,  &longTime },
      {L_,  TRVA,           6,       0,         CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TRV,           7,       0,        INVALID,      0,  &longTime },
      {L_,  TRVA,           6,       0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,   TRV,           1,       0,           1,         0,  &longTime },
      {L_,   TRV,           5,       0,          25,         0,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,       0,           0,         0,  &longTime },
      {L_,  TRVA,           6,       0,         ERR,       0,  &longTime },
      {L_,   TRV,           4,       0,        INVALID,      0,  &longTime },
      {L_,  TRVA,           5,       0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,  TRVA,           1,       0,           1,         0,  &longTime },
      {L_,  TRVA,           5,       0,          25,         0,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,       0,           0,         0,  &longTime },
      {L_,  TRVA,           6,       1,         ERR,         0,  &longTime },
      {L_,   TRV,           4,       1,        INVALID,      0,  &longTime },
      {L_,  TRVA,           5,       1,        INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,         0,       0   },
      {L_,  TRVA,           1,       0,          1,         0,  &longTime},
      {L_,  TRVA,           5,       0,         25,         0,  &longTime},
      {L_,  TRVA,           6,       0,         24,         0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,       0,         50,         0,       0   },
      {L_,   TRV,           1,       0,          1,         0,  &longTime},
      {L_,   TRV,           5,       0,         25,         0,  &longTime},
      {L_,   TRV,           6,       0,         24,         0,  &timeout1},
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data
    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,      0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,      0,           0,        0,  &longTime },
       {L_,    RB,          80,      1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,   TRV,           2,      0,           4,        0,  &longTime },
       {L_,   TRV,           3,      0,           9,        0,  &longTime },
       {L_,   TRV,           6,      0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,      1,           0,        0,  &longTime },
       {L_,   TRV,           4,      0,          16,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,   TRV,           6,      0,          35,        0,  &longTime },
       {L_,   RBR,         150,      0,          51,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,      0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,      0,           0,        0,  &longTime },
       {L_,    RB,          80,      1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,   TRV,           2,      0,           4,        0,  &longTime },
       {L_,   TRV,           3,      0,           9,        0,  &longTime },
       {L_,   TRV,           6,      0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,      1,           0,        0,  &longTime },
       {L_,  TRVA,           4,      0,          16,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,  TRVA,           6,      0,          35,        0,  &longTime },
       {L_,   RBR,         150,      0,          51,        0,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,      0,          50,        0,  &longTime },
       {L_,   TRV,           1,      0,           1,        0,  &longTime },
       {L_,  TRVA,           2,      0,           4,        0,  &longTime },
       {L_,   TRV,           3,      0,           9,        0,  &longTime },
       {L_,  TRVA,           6,      0,          35,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  SIGNAL,         1,      0,           0,        0,  &longTime },
       {L_,   TRVA,          3,      1,           1, INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,     20,      0,         20,        0,   &longTime },
       {L_,  SIGNAL,         1,      0,          0,        0,   &longTime },
       {L_,   TRV,           6,      1,         20, INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_, HELP_WRITE,     20,      0,          20,        0,  &longTime },
       {L_,   SIGNAL,        1,   WRITE_OP,       0,        0,  &longTime },
       {L_,   TRVA,          6,      0,          35,        0,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,        0,        0,  &longTime },
       {L_,   TRV,           7,      0,          85,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,      0,          50,        0,  &longTime },
       {L_,   TRV,           1,      0,           1,        0,  &longTime },
       {L_,  TRVA,           2,      0,           4,        0,  &longTime },
       {L_,   TRV,           4,      0,          16,        0,  &longTime },
       {L_,  TRVA,           5,      0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  TRVA,           5,      0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,      1,      1,           1,        0,  &longTime },
       {L_,   TRV,           4,      0,           1,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, HELP_WRITE,     15,      1,          15,        0,  &longTime },
       {L_,  TRVA,           5,      1,          15,        0,  &timeout1 },
         //
       {L_, HELP_WRITE,      3,      1,           3,        0,  &longTime },
       {L_,   TRV,           6,      1,           3,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_, HELP_WRITE,     20,      0,          20,        0,  &longTime },
       {L_,   SIGNAL,        1,  WRITE_OP,        0,        0,  &longTime },
       {L_,   TRVA,          8,      0,         120,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,        0,        0,  &longTime },
       {L_,    TRV,          8,      0,         100,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0,
                                            ioType) );
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
      case 13: {
        // -------------------------------------------------------------------
        // TESTING 'timedReadRaw' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedReadRaw(char                    *buffer,
        //                    int                      numBytes,
        //                    const bdet_TimeInterval& time,
        //                    int                      flags = 0);
        //   int timedReadRaw(int                     *augStatus,
        //                    char                    *buffer,
        //                    int                      numBytes,
        //                    const bdet_TimeInterval& time,
        //                    int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadRaw' method");
            QT("=============================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,      0     },
      {L_,   TRR,           1,         0,           1,         0,  &longTime },
      {L_,   TRR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,   TRR,         100,         0,         CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,   TRR,         100,         0,        INVALID,      0,  &longTime },
      {L_,  TRRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,  TRRA,          10,         0,          10,         0,  &longTime },
      {L_,  TRRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,  TRRA,         100,         0,         CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,   TRR,         100,         0,        INVALID,      0,  &longTime },
      {L_,   TRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,   TRR,           1,         0,           1,         0,  &longTime },
      {L_,   TRR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_,  TRRA,          20,         0,         ERR,       0,  &longTime },
      {L_,   TRR,          80,         0,        INVALID,      0,  &longTime },
      {L_,  TRRA,          40,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,  TRRA,           1,         0,           1,         0,  &longTime },
      {L_,  TRRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_,  TRRA,          20,         1,         ERR,       0,  &longTime },
      {L_,   TR,          80,         1,        INVALID,      0,  &longTime },
      {L_,  TRRA,          40,         1,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,   TRRA,          1,         0,           1,         0,  &longTime },
      {L_,   TRRA,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,    TRR,          1,         0,           1,         0,  &longTime },
      {L_,    TRR,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #else                  // unix
    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,   TRR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll
         // return with the remaining data in the internal buffer.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_,   TRR,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,  TRR,           50,         0,          50,        0,  &longTime },
       {L_,  RBR,           70,         0,          50,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,  TRRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_,  TRRA,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,  TRRA,          50,         0,          50,        0,  &longTime },
       {L_,   RBR,          70,         0,          50,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,    50,         0,          50,        0,  &longTime },
       {L_,   TRR,           1,         0,           1,        0,  &longTime },
       {L_,  TRRA,          10,         0,          10,        0,  &longTime },
       {L_,   TRR,          15,         0,          15,        0,  &longTime },
       {L_,  TRRA,          24,         0,          24,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  SIGNAL,        1,         1,           0,        0,  &longTime },
       {L_,  TRRA,         125,         1,         100,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_,  SIGNAL,         1,         1,          0,        0,   &longTime },
       {L_,  TRR,          250,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_,   SIGNAL,        1,   WRITE_OP,          0,        0,  &longTime },
       {L_,  TRRA,         125,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,  TRR,          150,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
       {L_,   TRR,           1,         0,           1,        0,  &longTime },
       {L_,  TRRA,          10,         0,          10,        0,  &longTime },
       {L_,   TRR,          15,         0,          15,        0,  &longTime },
       {L_,  TRRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  TRRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_,   TRR,          25,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_,  TRRA,          25,         1,           0,        0,  &timeout1 },
         //
       {L_,   TRR,          25,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,   TRRA,        125,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,   TRR,         150,         0,         100,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType) );
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
      case 12: {
        // -------------------------------------------------------------------
        // TESTING 'timedRead' METHOD:
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
        //        'non-interruptible' mode until more data arrives;
        //     9. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode until timeout is reached;
        //    10. keep trying reading expected number of bytes in
        //        'interruptible' mode until timeout is reached if no "AE"
        //        occurs;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Five steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //   Step 5: (for concern 9 - 10)
        //     Build a set 'DATA' OF 'TestCommand', each of which is
        //     specified with a timeout requirement.  Keep reading from the
        //     channel until no enough data available, the "hanged" read
        //     operation will at last reach its timeout requirement and return
        //     with a partial result.
        //
        //   Note that both 'read' methods (with the 'augStatus' or
        //     w/o the 'augStatus' parameter version) are tested respectively
        //     in each step.
        //
        // Testing:
        //   int timedRead(char                    *buffer,
        //                 int                      numBytes,
        //                 const bdet_TimeInterval& time,
        //                 int                      flags = 0);
        //   int timedRead(int                     *augStatus,
        //                 char                    *buffer,
        //                 int                      numBytes,
        //                 const bdet_TimeInterval& time,
        //                 int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedRead' method");
            QT("==========================");
        }

        {

            bdet_TimeInterval timeout1(0, 2 * SLEEP_TIME),
                              timeout2(3, 0),
                              longTime(120, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,   0 },
      {L_,    TR,           1,         0,           1,         0,  &longTime },
      {L_,    TR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,    TR,         100,         0,         CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure,
        // and so the subsequent read operations will not succeed any
        // more.
      {L_,    TR,         100,         0,        INVALID,      0,  &longTime },
      {L_,   TRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to
    // test the behavior of the 'read' method w the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,   TRA,          10,         0,          10,         0,  &longTime },
      {L_,   TRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
      {L_,   TRA,         100,         0,         CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure,
         // and so the subsequent read operations will not succeed any
         // more.
      {L_,    TR,         100,         0,        INVALID,      0,  &longTime },
      {L_,   TRA,          60,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
        defined (BSLS_PLATFORM__OS_WINDOWS)

    // Command set 3: Close the channel at the channel side to
    // test the behavior of the 'read' method with the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,    TR,           1,         0,           1,         0,  &longTime },
      {L_,    TR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations,
        // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_,   TRA,          20,         0,         ERR,       0,  &longTime },
      {L_,    TR,          80,         0,        INVALID,      0,  &longTime },
      {L_,   TRA,          40,         0,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to
    // test the behavior of the 'read' method w/o the 'augStatus'
    // parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,   TRA,           1,         0,           1,         0,  &longTime },
      {L_,   TRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations,
      // each of which should return a "ERROR".
      {L_, CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
      {L_,   TRA,          20,         1,         ERR,       0,  &longTime },
      {L_,    TR,          80,         1,        INVALID,      0,  &longTime },
      {L_,   TRA,          40,         1,        INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #endif

    #ifdef BSLS_PLATFORM__OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,    TRA,          1,         0,           1,         0,  &longTime },
      {L_,    TRA,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, HELP_WRITE,     50,         0,          50,         0,  &longTime },
      {L_,     TR,          1,         0,           1,         0,  &longTime },
      {L_,     TR,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #else

    // Commands set 5: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method w/o the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,    TR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_,    TR,          15,         0,          15,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,    TR,          50,         0,          50,        0,  &longTime },
       {L_,   RBR,          50,         0,          45,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected
    // number of bytes of data available in the channel's internal
    // buffer, test the behavior of the 'read' method with the
    // 'augStatus' parameter (concern 4 - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
         // mode, while a signal 'SIGSYS' is generated, and so the
         // returned 50 is stored in the internal buffer for later read
         // try.
       {L_,   SIGNAL,        1,         0,           0,        0,  &longTime },
       {L_,    RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because
         // there's enough data in the internal buffer at this moment.
       {L_,   TRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll
         // try reading the remaining data from the channel directly,
         // which is at last available from the control endpoint
         // through a thread.
       {L_,   SIGNAL,        0,         1,           0,        0,  &longTime },
       {L_,   TRA,          15,         0,          15,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the
         // following requests can all meet their expectations because
         // the last one is a "raw" operation.
       {L_,   TRA,          50,         0,          50,        0,  &longTime },
       {L_,   RBR,          50,         0,          45,        0,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
       {L_,    TR,           1,         0,           1,        0,  &longTime },
       {L_,   TRA,          10,         0,          10,        0,  &longTime },
       {L_,    TR,          15,         0,          15,        0,  &longTime },
       {L_,   TRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,  SIGNAL,         1,         0,           0,        0,  &longTime },
       {L_,   TRA,           5,         1,           4, INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the 'read' method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,      1,         1,          1,        0,   &longTime },
       {L_,  SIGNAL,         1,         0,          0,        0,   &longTime },
       {L_,    TR,           5,         1,          1, INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until all the requested
         // data were read.  (Here the signal generating thread is
         // implemented to write 100 bytes to the channel feed the
         // (TBD - figure out what goes here).
       {L_, HELP_WRITE,     20,         0,          20,        0,  &longTime },
       {L_,   SIGNAL,        1,   WRITE_OP,          0,        0,  &longTime },
       {L_,    TRA,         25,         0,          25,        0,  &longTime },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,    TR,         150,         0,         150,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, HELP_WRITE,     50,         0,          50,        0,  &longTime },
       {L_,    TR,           1,         0,           1,        0,  &longTime },
       {L_,   TRA,          10,         0,          10,        0,  &longTime },
       {L_,    TR,          15,         0,          15,        0,  &longTime },
       {L_,   TRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it.
       {L_,   TRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next
         // request, now we'll generate signals to interrupt it, the
         // only difference is we call the "read" method w/o the
         // 'augStatus' argument.
       {L_, HELP_WRITE,      1,         1,           1,        0,  &longTime },
       {L_,    TR,          25,         0,           1,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, HELP_WRITE,     15,         1,          15,        0,  &longTime },
       {L_,   TRA,          25,         1,          15,        0,  &timeout1 },
         //
       {L_, HELP_WRITE,      3,         1,           3,        0,  &longTime },
       {L_,    TR,          25,         1,           3,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to
         // come.  The request will not return until the timeout is reached.
         // Here the signal generating thread is to write 100 bytes at the
         // peer side to verify the "read" operations wait until timeout is
         // reached  (TBD - figure out what goes here).
         //
       {L_, HELP_WRITE,     20,         0,          20,        0,  &longTime },
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,    TRA,        125,         0,         120,        0,  &timeout2 },
         // The same situation for the 'read' operation, without
         // the 'augStatus' as the parameter.  The behavior should be
         // the same as above.
       {L_, HELP_WRITE,     30,         0,          30,        0,  &longTime },
       {L_,   SIGNAL,        1,  WRITE_OP,           0,        0,  &longTime },
       {L_,    TR,         150,         0,         130,        0,  &timeout2 },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int  ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType) );
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

      case 11: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__OS_LINUX)
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Three steps are designed for this function:
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
            QT("Testing 'writevRaw' method");
            QT("==========================");
        }

        {
            TestCommand COMMANDS_SET[][MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
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

            #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRO,           1,         0,           1,         0   },
              {L_,  WVRO,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,  WVRI,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, WVROA,           1,         0,           1,         0   },
              {L_, WVROA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_, WVRIA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #else                                       // unix test data
            #ifdef BSLS_PLATFORM__OS_SOLARIS
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
            #elif BSLS_PLATFORM__OS_LINUX
            // Command set 5: Close the channel at the channel side to
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVRO,          1,          0,           1,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
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
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
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
              {L_,     W,  SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
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
              {L_,     W,  SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
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

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    WR, SYS_DEPENDENT_LEN2,  0,  SYS_DEPENDENT_LEN2, 0   },
              {L_,  WVRO,           7,         1,        8192,         0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_,  WVRO,           8,         1,       40960,         0   },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  WVRI,           7,         1,        8192,         0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_,  WVRI,           8,         1,       40960,         0   },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, WVROA,           7,         1,        8192,         0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_, WVROA,           8,         1,       40960,         0   },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,    WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, WVRIA,           7,         1,        8192,         0   },
              {L_, SIGNAL,          8,   READ_OP,           0,         0   },
              {L_, WVRIA,           8,         1,       40960,         0   },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  WVRO,           7,          1,        8192,        0   },
              {L_,  SIGNAL,         8,    READ_OP,           0,        0   },
              {L_,   WVO,           7,          0,       24740,        0   },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVRI,          7,           1,        8192,         0   },
              {L_,  SIGNAL,        8,       READ_OP,        0,          0   },
              {L_,   WVI,          7,           0,      24740,          0   },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVROA,         7,           1,        8192,         0   },
              {L_,  SIGNAL,        8,       READ_OP,        0,          0   },
              {L_,  WVOA,          7,           0,      24740,          0   },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
              {L_,  WVRI,          7,           1,        8192,         0   },
              {L_,  SIGNAL,        8,       READ_OP,        0,          0   },
              {L_,  WVIA,          7,           0,      24740,          0   },
            },
            #else
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,  WVRO,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRO,           6,         1,        1024,         0   },
            },
            {
              {L_,  WVRI,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_,  WVRI,           6,         1,        1024,         0   },
            },
            {
              {L_, WVROA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVROA,           6,         1,        1024,         0   },
            },
            {
              {L_, WVRIA,           1,         0,           1,         0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
              {L_, SIGNAL,          2,   READ_OP,           0,         0   },
              {L_, WVRIA,           6,         1,        1024,         0   },
            },
            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  WVRO,           7,          1,        8192,        0   },
              {L_,  SIGNAL,         2,    READ_OP,           0,        0   },
              {L_,   WVO,           7,          0,       12302,        0   },
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

                     #ifdef BSLS_PLATFORM__OS_SOLARIS
                     buf4[WVECBUF_LEN80] = "\0",
                     buf5[WVECBUF_LEN8K] = "\0",
                     #else
                     buf4[WVECBUF_LEN500] = "\0",
                     buf5[WVECBUF_LEN1K] = "\0",
                     #endif
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, iovecBuffer, ovecBuffer,
                                            ioType) );
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
#if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Three steps are designed for this function:
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
            QT("Testing 'writev' method");
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
            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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

            #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #else                                       // unix test data

            #ifdef BSLS_PLATFORM__OS_SOLARIS
            // commands set 9: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There are not enough space in the TCP buffer for next
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
                // There are not enough space in the TCP buffer for next
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
                // There are not enough space in the TCP buffer for next
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
                // There are not enough space in the TCP buffer for next
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
            #elif BSLS_PLATFORM__OS_LINUX

            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         5,         0,           0,         0   },
              {L_,   WVO,           7,         1,        1024, INTERRUPTED },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         5,         0,           0,         0   },
              {L_,   WVI,           7,         1,        1024, INTERRUPTED },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         5,         0,           0,         0   },
              {L_,  WVOA,           8,         1,        1024, INTERRUPTED },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         5,         0,           0,         0   },
              {L_,  WVIA,           8,         1,        1024, INTERRUPTED },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   WVO,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         5,       READ_OP,       0,        0   },
              {L_,   WVO,           6,        0,         1608,        0   },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   WVI,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,   WVI,           6,        0,         1608,        0   },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  WVOA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,  WVOA,           6,        0,         1608,        0   },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  WVIA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,  WVIA,           6,        0,         1608,        0   },
            },

            #else // ibm test data

            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVO,           1,         0,           1,         0   },
              {L_,   WVO,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,   WVO,           7,         1,        1024, INTERRUPTED },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   WVI,           1,         0,           1,         0   },
              {L_,   WVI,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,   WVI,           7,         1,        1024, INTERRUPTED },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVOA,           1,         0,           1,         0   },
              {L_,  WVOA,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,  WVOA,           7,         1,        1024, INTERRUPTED },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  WVIA,           1,         0,           1,         0   },
              {L_,  WVIA,           5,         0,         164,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,  WVIA,           7,         1,        1024, INTERRUPTED },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   WVO,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,   WVO,           6,        0,         1608,        0   },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   WVI,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,   WVI,           6,        0,         1608,        0   },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  WVOA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,  WVOA,           6,        0,         1608,        0   },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  WVIA,           1,        0,            1,        0   },
              {L_,    WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,  SIGNAL,         2,       READ_OP,       0,        0   },
              {L_,  WVIA,           6,        0,         1608,        0   },
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
                     #ifdef BSLS_PLATFORM__OS_SOLARIS
                         buf4[WVECBUF_LEN80] = "\0",
                         buf5[WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[WVECBUF_LEN500] = "\0",
                         buf5[WVECBUF_LEN1K] = "\0",
                     #endif
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, iovecBuffer, ovecBuffer,
                                            ioType) );
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
#if !defined(BSLS_PLATFORM__OS_SOLARIS) || BSLS_PLATFORM__OS_VER_MAJOR < 10
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Three steps are designed for this function:
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
            // test the behavior of the 'read' method w/o the 'augStatus'
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
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,   WRA,         100,         0,        INVALID,      0   },
              {L_,    WR,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,   WRA,   SYS_DEPENDENT_LEN, 0,         ERR,         0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent read operations will not succeed any
                // more.
              {L_,    WR,         100,         0,        INVALID,      0   },
              {L_,   WRA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #if defined(BSLS_PLATFORM__OS_WINDOWS)  // windows test data
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
            #ifdef BSLS_PLATFORM__OS_SOLARIS
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
            #else                // ibm test data
            // commands set 3: to resolve concern 4 - 6.
            {
              {L_,    WR,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN,  1,        1024,          0   },
              {L_,   WR,    BUF_WRITE,        0,      BUF_WRITE,        0   },
              {L_,  SIGNAL,          2,   READ_OP,          0,          0   },
              {L_,    WR,    BUF_WRITE,        1,        1024,          0   },
            },
            // commands set 4: to resolve concern 4 - 6.
            {
              {L_,  WRA,            1,        0,           1,          0   },
              {L_,    W,   SYS_DEPENDENT_LEN, 1,        1024,          0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;

                     int ret = processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType);
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
#if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Three steps are designed for this function:
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
        //   Note that both "write" methods (with the 'augStatus' or
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
            // Command set 1: Close the channel from the peer side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     W,          1,          0,           1,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to
            // test the behavior of the write method w the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WA,          1,          0,           1,         0   },
              {L_,     W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_CONTROL,   0,         0,           0,         0   },
              {L_,    WA,   SYS_DEPENDENT_LEN, 0,         CLOSED,      0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent  write operations will not succeed
                // any more.
              {L_,    WA,         100,         0,        INVALID,      0   },
              {L_,     W,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
             // Command set 3: Close the channel at the channel side to
            // test the behavior of the write method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     W,          1,          0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed
                // any more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel at the channel side to
            // test the behavior of the write method w/o the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    WA,          1,          0,           1,         0   },
              {L_,    WA,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, CLOSE_OBSERVE,  0,          0,           0,         0   },
              {L_,    WA,   SYS_DEPENDENT_LEN, 0,         ERR,       0   },
                // The channel now is invalid due to the operation failure,
                // and so the subsequent write operations will not succeed any
                // more.
              {L_,     W,         100,         0,        INVALID,      0   },
              {L_,    WA,          60,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #if defined(BSLS_PLATFORM__OS_WINDOWS)      // windows test data
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

            #ifdef BSLS_PLATFORM__OS_SOLARIS
            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     W,           1,         0,           1,         0   },
              {L_,     W,          15,         0,          15,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 1,        8192,         0   },
                // There are not enough bytes left in the TCP buffer for next
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
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,    WA,         10000,       1,        8192, INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
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

            #elif defined(BSLS_PLATFORM__OS_LINUX)
            // commands set 3: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     W,           1,         0,           1,         0   },
              {L_,     W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,         0,           0,         0   },
              {L_,     W,   SYS_DEPENDENT_LEN, 1,        0,         0   },
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
                // There are not enough space in the TCP buffer for next
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
                // There are not enough space in the TCP buffer for next
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
              {L_,     WA,       30000,         1,       30000,         0   },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  SIGNAL,         2,          0,           0,         0   },
              {L_,     WA,  SYS_DEPENDENT_LEN,  1,        8192, INTERRUPTED },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType;
                    #ifdef BSLS_PLATFORM__OS_AIX
                         ioType = IBM_WRITE;
                    #else
                         ioType = 0;
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0,
                                            ioType) );
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
// #ifndef BSLS_PLATFORM__OS_AIX
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
        //     also write expected number of bytes at the peer side if
        //     specified.
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
            QT("Testing 'bufferedReadRaw' method");
            QT("================================");
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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

            #ifdef BSLS_PLATFORM__OS_WINDOWS
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
              {L_,   SIGNAL,         0,         1,           0,         0   },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel.
                // the last one is a "raw" operation.
              {L_,   RBRA,          50,         0,           1,         0  },
              {L_, HELP_WRITE,      20,         0,          20,         0  },
              {L_,   RBRA,          50,         0,          20,         0  },
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
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
                // When there is not enough data (but some data is available),
                // a 'readRaw' will return the number of bytes it read.
              {L_,    RBR,         25,         0,           4,         0   },
              {L_, HELP_WRITE,     10,         0,          10,         0   },
              {L_,   RRA,          35,         0,          10,         0   },
                // There are no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RBR,        105,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RBR,        105,         0,         100,         0   },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,   RBRA,        105,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,   RBRA,        105,         0,         100,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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
            QT("==============================");
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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
              {L_,    RBA,         20,         0,         ERR,       0   },
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
              {L_,    RBA,         20,         1,         ERR,       0   },
              {L_,     RB,         80,         1,        INVALID,      0   },
              {L_,    RBA,         40,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM__OS_WINDOWS
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
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
                // implemented to write 100 bytes to the channel feed the
                // (TBD - figure out what goes here).
              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,     RBA,        25,         0,          25,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,         1,           0,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of e
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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
              {L_,   RVRA,          6,         1,         ERR,       0   },
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
              {L_,    RVR,          6,         1,         ERR,       0   },
              {L_,    RVR,          4,         1,        INVALID,      0   },
              {L_,   RVRA,          5,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM__OS_WINDOWS
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
              {L_,   RVRA,          4,         0,           2,         0   },
                // There are enough data in the channel's TCP buffer, the
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
                // There are no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RVR,          8,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RVR,          8,         0,         100,         0   },
                // The same as above except  w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,   RVRA,          8,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0, 0) );
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
#if !defined(BSLS_PLATFORM__OS_SOLARIS) || \
    !defined(BSLS_PLATFORM__CMP_GNU) || \
    (BSLS_PLATFORM__CMP_VER_MAJOR < 40000)

        // -------------------------------------------------------------------
        // TESTING 'readv' METHOD:
        // Concerns:
        //   The main concerns about this function are that if it can
        //     1. return -l if the connection was closed by the peer;
        //     2. no read operations can succeed after the channel is
        //        invalid;
        //     3. return other negative value for other 'hard' errors;
        //        (when there's data existing in the channel's internal buffer)
        //     4. return the data copied from the internal buffer when
        //        there's enough data in the internal buffer;
        //     5. copy the existing data from the channel's internal buffer
        //        first, then read the remaining data from the channel when
        //        there's not enough data in the internal buffer
        //        from the channel for the remaining requested data.
        //        (when there's data existing in the channel's internal buffer)
        //     6. read the expected number of bytes from the channel if
        //        enough data available in the channel;
        //     7. return after an "AE" occurs if the 'read' is in
        //        'interruptible', and 'augstatus' should be > 0 in this case;
        //     8. keep trying reading expected number of bytes upon "AE" if in
        //        'non-interruptible' mode;
        //
        // Plan:
        //   Create a socket pair which is connected each other.  A
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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
              {L_,   RVA,           6,         1,         ERR,       0   },
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
              {L_,    RV,           6,         1,         ERR,       0   },
              {L_,    RV,           4,         1,        INVALID,      0   },
              {L_,   RVA,           5,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM__OS_WINDOWS
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
              {L_,    RV,           4,         0,          16,         0   },
                // There are enough data in the channel's TCP buffer, the
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
              {L_,   RVA,           4,         0,          16,         0   },
                // There are enough data in the channel's TCP buffer, the
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
                // implemented to write 100 bytes to the channel feed the
                // (TBD - figure out what goes here).

              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,    RVA,          6,         0,          35,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,    RV,           7,         0,          85,         0   },
            },
            // commands set 8: to resolve concern 6 - 8.
            {
                // Test the situation when multiple adjustments of the
                // buffers are needed.
              {L_, HELP_WRITE,     75,         0,          75,         0   },
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,   RVA,           8,         0,         175,         0   },

                // Test the situation when multiple adjustments of the
                // buffers are needed.
              {L_, HELP_WRITE,     75,         0,          75,         0   },
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,   RV,            8,         0,         175,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0, 0) );
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)
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
              {L_,    RR,          20,         1,         ERR,       0   },
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
              {L_,   RRA,          20,         0,         ERR,       0   },
              {L_,    RR,          80,         0,        INVALID,      0   },
              {L_,   RRA,          40,         0,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM__OS_WINDOWS
            {
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,   RRA,           1,         0,           1,         0   },
              {L_,   RRA,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          44,         0   },
            },
            {
              {L_, HELP_WRITE,    100,         0,           0,         0   },
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
                // There are no data left in the TCP buffer for next
                // request, so even though an "asynchronous event" happens
                // and the 'readRaw' will keep waiting for some data to come,
                // no matter if it's in 'non-interruptible' or 'interruptible'
                // mode.
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RR,         105,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,    RR,         105,         0,         100,         0   },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  SIGNAL,         1,         1,           0,         0   },
              {L_,   RRA,         105,         1,         100,         0   },
              {L_,  SIGNAL,         1,         1,           0,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
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
        //   'btesos_TcpTimedChannel' object 'channel' is created with one of
        //   the two sockets.  Four steps are designed for this function:
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
        //     partial read by issuing an "asynchronous event" during the
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

            #if defined (BSLS_PLATFORM__OS_SOLARIS) || \
                defined (BSLS_PLATFORM__OS_WINDOWS)

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
              {L_,    RA,          20,         1,         ERR,       0   },
              {L_,     R,          80,         1,        INVALID,      0   },
              {L_,    RA,          40,         1,        INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            #endif

            #ifdef BSLS_PLATFORM__OS_WINDOWS
            {
              {L_, HELP_WRITE,     50,         0,          50,         0   },
              {L_,    RA,           1,         0,           1,         0   },
              {L_,    RA,           5,         0,           5,         0   },
              {L_,   RBR,         250,         0,          44,         0   },
            },
            {
              {L_, HELP_WRITE,     50,         0,           0,         0   },
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
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
              {L_,   SIGNAL,        0,         1,           0,         0   },
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
                // implemented to write 100 bytes to the channel feed the
                // (TBD - figure out what goes here).
              {L_, HELP_WRITE,     20,         0,          20,         0   },
              {L_,   SIGNAL,        1,         1,           0,         0   },
              {L_,     RA,         25,         0,          25,         0   },
                // The same situation for the 'read' operation, without
                // the 'augStatus' as the parameter.  The behavior should be
                // the same as above.
              {L_,   SIGNAL,        1,         1,           0,         0   },
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

                    #ifdef BSLS_PLATFORM__OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
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
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------
        if (verbose) {
            QT("BREATHING TEST");
            QT("==============");
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
