// btlsos_tcptimedchannel.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlsos_tcptimedchannel.h>

#include <btlso_inetstreamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlso_sockethandle.h>
#include <btlso_socketimputil.h>
#include <btlso_socketoptutil.h>
#include <btlso_ioutil.h>

#include <btls_iovec.h>
#include <btls_iovecutil.h>

#include <btlsc_flag.h>

#include <bdlt_currenttime.h>

#include <bslim_testutil.h>

#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslma_testallocator.h>
#include <bsls_timeinterval.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_c_signal.h>
#include <unistd.h>
#endif

#include <signal.h>

#undef ERR

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// The component under test provides concrete implementation of the synchronous
// communication channel ('btlsc_channel') over TCP/IPv4 sockets.  The basic
// plan for the testing is to ensure that all methods in the component work as
// expected.  By using the table-driven strategy, I/O requests on a channel
// will be decoded from the set of test data.  Each test data set is built
// based on both the 'black-box' concerns and code investigation ('white-box').
// The purpose of each test data set is to resolve one or more concerns about
// that function.  For each test operation, the result will be compared to the
// expected one which is calculated beforehand.
//-----------------------------------------------------------------------------
// [ 1] btlsos::TcpTimedChannel()
// [ 1] ~btlsos::TcpTimedChannel()
// [ 1] int read(...)
// [ 1] int readv(...)
// [ 1] int readRaw(...)
// [ 1] int readvRaw(...)
// [ 1] int bufferedRead(...)
// [ 1] int bufferedReadRaw(...)
// [ 1] int write(...)
// [ 1] int writev(...)
// [ 1] int writeRaw(...)
// [ 1] int writevRaw(const btls::Ovec *buffers, ...)
// [ 1] int writevRaw(const btls::Iovec *buffers, ...)
// [ 1] void invalidate()
// [ 1] int isInvalid()
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
//=============================================================================

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//-----------------------------------------------------------------------------
bslmt::Mutex  d_mutex;   // for i/o synchronization in all threads

#define PT(X) d_mutex.lock(); P(X); d_mutex.unlock();
#define QT(X) d_mutex.lock(); Q(X); d_mutex.unlock();
#define P_T(X) d_mutex.lock(); P_(X); d_mutex.unlock();
// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef btlsos::TcpTimedChannel Obj;
typedef btlso::StreamSocket<btlso::IPv4Address> StreamSocket;

static int verbose;
static int veryVerbose;

const char* HOST_NAME = "127.0.0.1";

enum {
    k_DEFAULT_PORT_NUMBER     =  0,
    k_DEFAULT_NUM_CONNECTIONS =  10,
    k_DEFAULT_EQUEUE_SIZE     =  5,
#if defined BSLS_PLATFORM_OS_LINUX
    k_SLEEP_TIME              =  1000
#else
    k_SLEEP_TIME              =  10000
#endif
};

enum { // error code returned from I/O operation
    e_INTERRUPTED       =  1,
    e_CLOSED            = -1,
    e_INVALID           = -2,
    e_ERR               = -3,
    e_DEFAULT_AUGSTATUS = -9
};

#ifdef BSLS_PLATFORM_OS_UNIX
const int MAX_BUF     = 99000;     // the biggest length of a buffer for write

    #if defined BSLS_PLATFORM_OS_AIX

        const int BUF_WRITE   = 8192;  // the last buffer length for ioVec/oVec
        const int BUF_WRITE2  = 500;   // the last second buffer vector length
        const int BUF_LIMIT   = 1024;  // to set the snd/rcv buffer size
        const int HELPER_READ = 75000;
        const int SYS_DEPENDENT_LEN = 512;

    #elif defined(BSLS_PLATFORM_OS_LINUX)
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
    k_MAX_CMD       = 50,
    e_WRITE_OP      = 1,
    e_READ_OP       = 2,
    e_HELPER_WRITE  = 100,        // When an "AE" occurs during a 'read' which
                                  // is in 'non-interruptible' mode, some data
                                  // need to be written at the peer side for
                                  // the read to get data from the connection.
                                  // This means in THIS test driver, any read
                                  // request for "AE" interrupt test should be
                                  // waiting for less than "HELPER_WRITE" bytes
                                  // if it's in 'non-interruptible' mode.
    e_SUN_WRITE     = 1,
    e_IBM_WRITE     = 2,
    e_WIN_WRITE     = 3
};

enum {
    k_MAX_VECBUF     = 12,
    k_VECBUF_LEN1    = 1,
    k_VECBUF_LEN2    = 2,
    k_VECBUF_LEN3    = 3,
    k_VECBUF_LEN4    = 4,
    k_VECBUF_LEN5    = 5,
    k_VECBUF_LEN6    = 6,
    k_VECBUF_LEN7    = 7,
    k_VECBUF_LEN8    = 8,
    k_VECBUF_LEN9    = 9,
    k_VECBUF_LEN10   = 10,
    k_VECBUF_LEN50   = 50,
    k_VECBUF_LEN90   = 90,

    k_WVECBUF_LEN1   = 1,
    k_WVECBUF_LEN20  = 20,
    k_WVECBUF_LEN40  = 40,
    k_WVECBUF_LEN60  = 60,
    k_WVECBUF_LEN80  = 80,
    k_WVECBUF_LEN500 = 500,
    k_WVECBUF_LEN1K  = 1024,
    k_WVECBUF_LEN8K  = 8192,
    k_WVECBUF_LEN16K = 16384,
    k_WVECBUF_LEN32K = 32768
};

struct ConnectInfo {
     // Use this struct to pass information to the helper thread.
     bslmt::ThreadUtil::Handle    d_tid;         // the id of the thread to
                                                // which a signal's delivered

     btlso::SocketHandle::Handle  d_socketHandle;

     int                         d_signals;        // the number of signals to
                                                   // be raised
     int                         d_signalIoFlag;  // if this flag is set,
       // meaning some on-going I/O operation is in 'non-interruptible' mode,
       // and so some data is needed to be writted at 'd_socketHandle' to "let"
       // the channel's I/O operation go.
     int                         d_ioType;
};

// Test commands used to test 'btlsos::TcpTimedChannel':
enum {
    e_R              =  1,  //  read
    e_RA             =  2,  //  read with 'augStatus'
    e_RV             =  3,  //  readv
    e_RVA            =  4,  //  readv with 'augStatus'
    e_RR             =  5,  //  readRaw
    e_RRA            =  6,  //  readRaw with 'augStatus'
    e_RB             =  7,  //  bufferedRead
    e_RBA            =  8,  //  bufferedRead with 'augStatus'
    e_RVR            =  9,  //  readv
    e_RVRA           = 10,  //  readv with 'augStatus'
    e_RBR            = 11,  //  bufferedReadv
    e_RBRA           = 12,  //  bufferedReadv with 'augStatus'
    e_TR             = 13,  //  timedRead()
    e_TRA            = 14,  //  timedRead() with 'augStatus'
    e_TRV            = 15,  //  timedReadv
    e_TRVA           = 16,  //  timedReadv with 'augStatus'
    e_TRR            = 17,  //  timedReadRaw
    e_TRRA           = 18,  //  timedReadRaw with 'augStatus'
    e_TRB            = 19,  //  timedBufferedRead
    e_TRBA           = 20,  //  timedBufferedRead with 'augStatus'
    e_TRVR           = 21,  //  timedReadv
    e_TRVRA          = 22,  //  timedReadv with 'augStatus'
    e_TRBR           = 23,  //  timedBufferedReadv
    e_TRBRA          = 24,  //  timedBufferedReadv with 'augStatus'
    k_READ_FUNCTIONS = 30,  //  the number of read functions
    // The followings are "write" operations.
    e_W              = 35,  //  write
    e_WA             = 36,  //  write with 'augStatus'
    e_WR             = 37,  //  writeRaw
    e_WRA            = 38,  //  writeRaw with 'augStatus'
    e_WVO            = 39,  //  writev with 'btls::Iovec'
    e_WVOA           = 40,  //  writev with 'btls::Iovec' and 'augStatus'
    e_TWVO           = 41,  //  timedWritev with 'btls::Iovec'
    e_TWVOA          = 42,  //  timedWritev with 'btls::Iovec' and 'augStatus'
    e_WVRO           = 43,  //  writevRaw with 'btls::Ovec' and 'augStatus'
    e_WVROA          = 44,  //  writevRaw with 'btls::Ovec' and 'augStatus'
   e_TWVRO          = 45,  //  timedWritevRaw with 'btls::Ovec' and 'augStatus'
   e_TWVROA         = 46,  //  timedWritevRaw with 'btls::Ovec' and 'augStatus'
    e_WVI            = 47,  //  writev with 'btls::Ovec'
    e_WVIA           = 48,  //  writev with 'btls::Ovec' and 'augStatus'
    e_TWVI           = 49,  //  timedWritev with 'btls::Ovec'
    e_TWVIA          = 50,  //  timedWritev with 'btls::Ovec' and 'augStatus'
    e_WVRI           = 51,  //  writevRaw with 'btls::Iovec' and 'augStatus'
    e_WVRIA          = 52,  //  writevRaw with 'btls::Iovec' and 'augStatus'
  e_TWVRI          = 53,  //  timedWritevRaw with 'btls::Iovec' and 'augStatus'
  e_TWVRIA         = 54,  //  timedWritevRaw with 'btls::Iovec' and 'augStatus'
    e_TW             = 55,  //  timedWrite
    e_TWA            = 56,  //  timedWrite with 'augStatus'
    e_TWR            = 57,  //  timedWriteRaw
    e_TWRA           = 58,  //  timedWriteRaw with 'augStatus'
    k_WRITE_FUNCTIONS= 30,  //  the number of write functions
    // The followings are "helper" operations.
    e_HELP_READ      = 70,  //  read from the control endpoint of the channel
    e_HELP_WRITE     = 71,  //  write to the control endpoint of the channel
    e_HELP_WRITE2    = 72,  //  same as the above, but through a thread
    e_INVALIDATE     = 73,  //  invalidate the channel
    e_SLEEP          = 74,  //  sleep for the specified number of seconds
    e_SIGNAL         = 75,  //  create a thread generating a signal of "SIGSYS"
    e_CLOSE_CONTROL  = 76,  //  Create a thread to close the control(helper)
                            //  socket.
    e_CLOSE_OBSERVE  = 77,  //  Create a thread to close the observe socket.
    e_BUF_STAT       = 78   //  Check the internal buffer state.
};

struct TestCommand {
    // This struct includes needed information for each test operation, e.g., a
    // request to "read", "writev" etc., along with it's corresponding test
    // results.
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
                                           // 2 == write at the peer side 0 ==
                                           // no read or write
    } flag;

    int                d_expReturnValue;   // return value of this command

    int                d_expStatus;        // expected status of this command

    bsls::TimeInterval *d_timeout;          // timeout requirement
};

char globalBuffer[HELPER_READ]; // To help read from the peer side.
bslma::TestAllocator testAllocator;
// ============================================================================
//                   HELPER FUNCTIONS/CLASSES FOR TESTING
// ----------------------------------------------------------------------------
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
            write(2, "====================\n",
                    sizeof("====================\n"));
        }
    }
    return;
}

static void registerSignal(int signo, void (*handler)(int) )
    // Register the specified signal 'handler' for the specified signal 'signo'
    // to be generated.
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

    if (e_IBM_WRITE == threadInfo->d_ioType) {
        bslmt::ThreadUtil::sleep(bsls::TimeInterval(10,0));
    }
    else {
        bslmt::ThreadUtil::microSleep(5 * k_SLEEP_TIME);
                                     // The thread waits to make sure the main
                                     // thread is hanging in an I/O call.
    }

    for (int i = 0; i < threadInfo->d_signals; ++i) {
        pthread_kill(threadInfo->d_tid, SIGSYS);
        if (verbose) {
            PT(bslmt::ThreadUtil::self());
            QT(" generated a SIGSYS signal.");
        }
        bslmt::ThreadUtil::microSleep(4 * k_SLEEP_TIME);
    }

    if (threadInfo->d_signalIoFlag) {  // non-interruptible mode
        bslmt::ThreadUtil::microSleep(2 * k_SLEEP_TIME);
        if (e_WRITE_OP == threadInfo->d_signalIoFlag) {
            char buf[e_HELPER_WRITE];
            memset(buf, 'x', e_HELPER_WRITE);    // to keep purify happy

            int len = btlso::SocketImpUtil::write(threadInfo->d_socketHandle,
                                                 buf,
                                                 sizeof buf);

            if (veryVerbose) {
                PT(bslmt::ThreadUtil::self());
                QT(" writes to socket: ");
                P_T(len);  PT(threadInfo->d_socketHandle);
            }
            if (e_HELPER_WRITE != len) {
                if (veryVerbose) {
                    P_T(bslmt::ThreadUtil::self());
                    QT("Error: Failed in writing right number of bytes"
                       " to the socket:");
                    PT(threadInfo->d_socketHandle);
                }
                ASSERT(0);
            }
        }
        else if (e_READ_OP == threadInfo->d_signalIoFlag) {
            int len = btlso::SocketImpUtil::read(globalBuffer,
                                            threadInfo->d_socketHandle,
                                            HELPER_READ);
#ifdef BSLS_PLATFORM_OS_LINUX
                // kludge for Linux since the buffer is 16K and Window 32K
            if (len < HELPER_READ && len == 16383) {
                len = btlso::SocketImpUtil::read(globalBuffer,
                                            threadInfo->d_socketHandle,
                                            HELPER_READ - 16383);
                len += len >= 0 ? 16383 : 0;
            }
#endif

            if (veryVerbose) {
                P_T(bslmt::ThreadUtil::self());
                QT(" reads from socket: ");
                P_T(len);  PT(threadInfo->d_socketHandle);
            }
            bslmt::ThreadUtil::microSleep(4 * k_SLEEP_TIME);
        }
    }

    bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
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
    bslmt::ThreadUtil::microSleep(5 * k_SLEEP_TIME);

    int ret = btlso::SocketImpUtil::close(threadInfo->d_socketHandle);

    ASSERT(0 == ret);

    if (verbose) {
        QT("ThreadToCloseSocket exits.");
    }
    testAllocator.deallocate(threadInfo);
    return 0;
}

}  // end of extern "C"

static int buildChannelHelper(
               btlso::SocketHandle::Handle                         *socketPair,
               btlso::InetStreamSocketFactory<btlso::IPv4Address>  *factory,
               btlso::StreamSocket<btlso::IPv4Address>            **sSocket)
    // Create a pair of sockets which are connected to each other and load them
    // into the specified 'socketPair'.  Create a
    // 'btlso::InetStreamSocketFactory' object and load it into the specified
    // 'sSocket'.  Return 0 on success, and nonzero otherwise.
{
    int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     socketPair,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[0],
                                         btlso::SocketOptUtil::k_TCPLEVEL,
                                        btlso::SocketOptUtil::k_TCPNODELAY,
                                         1);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[1],
                                         btlso::SocketOptUtil::k_TCPLEVEL,
                                        btlso::SocketOptUtil::k_TCPNODELAY,
                                         1);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[0],
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_SENDBUFFER,
                                        BUF_LIMIT);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[0],
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     btlso::SocketOptUtil::k_RECEIVEBUFFER,
                                      BUF_LIMIT);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[1],
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_SENDBUFFER,
                                        BUF_LIMIT);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(socketPair[1],
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     btlso::SocketOptUtil::k_RECEIVEBUFFER,
                                      BUF_LIMIT);
    ASSERT(0 == ret);
    int sndBufferSize = 0, rcvBufferSize = 0;
    ret = btlso::SocketOptUtil::getOption(&sndBufferSize, socketPair[0],
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER);
    if (0 != ret) {
        return ret;                                                   // RETURN
    }
    ret = btlso::SocketOptUtil::getOption(&rcvBufferSize, socketPair[1],
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_RECEIVEBUFFER);
    if (0 != ret) {
        return ret;                                                   // RETURN
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
void fillBuffers(VECBUFFER *vectorBuffers, int numBuffers, char ch)
{
    for (int i = 0; i < numBuffers; ++i) {
        memset((char*) const_cast<void *>(vectorBuffers[i].buffer()),
               ch,
               vectorBuffers[i].length());
    }
}

static int testExecutionHelper(btlsos::TcpTimedChannel     *channel,
                               TestCommand                 *command,
                               btlso::SocketHandle::Handle *socketPair,
                               bslmt::ThreadUtil::Handle   *threadHandle,
                               char                        *buffer,
                               const btls::Iovec           *ioBuffer,
                               const btls::Ovec            *oBuffer,
                               int                          ioType)
    // Process the specified 'command' to invoke some operation of the
    // specified 'channel', or the 'helperSocket' which is the control endpoint
    // of the socket pair.  For a read operation, load either the specified
    // 'buffer' or 'ioBuffer' corresponding to the request.  For a write
    // operation, write data from either 'buffer' or 'ioBuffer' or the
    // specified 'oBuffer' corresponding to the request.  Note because the
    // behaviors are different for "write" operations on different platform,
    // the specified 'ioType' is to indicate that the type of the function
    // being tested, e.g., it's a "read" or "write" operation so that different
    // testing configuration can be set on a specific platform.  Return the
    // return value of the operation on success, and 0 if the called function
    // returns nothing.
{
    int rCode = 0;

    switch (command->d_command) {
    case e_R: {   //
        rCode = channel->read(buffer,
                              command->numToProcess.d_numBytes,
                              command->flag.d_interruptFlags);
    } break;
    case e_RA: {  //
        int augStatus = 0;
        rCode = channel->read(&augStatus,
                              buffer,
                              command->numToProcess.d_numBytes,
                              command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TR: {   //
        rCode = channel->timedRead(buffer,
                              command->numToProcess.d_numBytes,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                              command->flag.d_interruptFlags);
    } break;
    case e_TRA: {  //
        int augStatus = 0;
        rCode = channel->timedRead(&augStatus,
                              buffer,
                              command->numToProcess.d_numBytes,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                              command->flag.d_interruptFlags);
        LOOP3_ASSERT(command->d_lineNum, augStatus, command->d_expStatus,
                     augStatus == command->d_expStatus);
    } break;
    case e_RV: {  //
        rCode = channel->readv(ioBuffer,
                               command->numToProcess.d_numBuffers,
                               command->flag.d_interruptFlags);
    } break;
    case e_RVA: { //
        int augStatus = 0;
        rCode = channel->readv(&augStatus,
                               ioBuffer,
                               command->numToProcess.d_numBuffers,
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TRV: {  //
        rCode = channel->timedReadv(ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case e_TRVA: { //
        int augStatus = 0;
        rCode = channel->timedReadv(&augStatus,
                               ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_RR: {  //
        rCode = channel->readRaw(buffer,
                                 command->numToProcess.d_numBytes,
                                 command->flag.d_interruptFlags);
    } break;
    case e_RRA: {  //
        int augStatus = 0;
        rCode = channel->readRaw(&augStatus,
                                 buffer,
                                 command->numToProcess.d_numBytes,
                                 command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TRR: {  //
        rCode = channel->timedReadRaw(buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                 command->flag.d_interruptFlags);
    } break;
    case e_TRRA: {  //
        int augStatus = 0;
        rCode = channel->timedReadRaw(&augStatus,
                                 buffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                 command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_RB: {  //
        const char * cbuffer = buffer;
        rCode = channel->bufferedRead(&cbuffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        if (command->d_expStatus > 0) {
            LOOP_ASSERT(command->d_lineNum, 0 == cbuffer);
        }
    } break;
    case e_RBA: {  //
        int augStatus = 0;
        const char * cbuffer = buffer;
        rCode = channel->bufferedRead(&augStatus,
                                      &cbuffer,
                                      command->numToProcess.d_numBytes,
                                      command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (command->d_expStatus > 0) {
             ASSERT(0 == cbuffer);
        }
    } break;
    case e_TRB: {  //
        const char * cbuffer = 0;
        rCode = channel->timedBufferedRead(&cbuffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                 command->flag.d_interruptFlags);
        if (command->d_expStatus >= 0) { // It's interrupted or timeout
            LOOP_ASSERT(command->d_lineNum, 0 == cbuffer);
        }
    } break;
    case e_TRBA: {  //
        int augStatus = -9;
        const char * cbuffer = 0;
        rCode = channel->timedBufferedRead(&augStatus,
                                &cbuffer,
                                command->numToProcess.d_numBytes,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
        if (command->d_expStatus > 0) {
            LOOP_ASSERT(command->d_lineNum, 0 == cbuffer);
        }
    } break;
    case e_RVR: {  //
        rCode = channel->readvRaw(ioBuffer,
                                  command->numToProcess.d_numBuffers,
                                  command->flag.d_interruptFlags);
    } break;
    case e_RVRA: { //
        int augStatus = 0;
        rCode = channel->readvRaw(&augStatus,
                                  ioBuffer,
                                  command->numToProcess.d_numBuffers,
                                  command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
     case e_TRVR: {  //
        rCode = channel->timedReadvRaw(ioBuffer,
                             command->numToProcess.d_numBuffers,
                            *command->d_timeout + bdlt::CurrentTime::now(),
                             command->flag.d_interruptFlags);
    } break;
    case e_TRVRA: { //
        int augStatus = 0;
        rCode = channel->timedReadvRaw(&augStatus,
                             ioBuffer,
                             command->numToProcess.d_numBuffers,
                            *command->d_timeout + bdlt::CurrentTime::now(),
                             command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
   case e_RBR: {  //
        const char * cbuffer = 0;
        rCode = channel->bufferedReadRaw(&cbuffer,
                                         command->numToProcess.d_numBytes,
                                         command->flag.d_interruptFlags);
    } break;
    case e_RBRA: {  //
        int augStatus = 0;
        const char * cbuffer = 0;
        rCode = channel->bufferedReadRaw(&augStatus,
                                         &cbuffer,
                                         command->numToProcess.d_numBytes,
                                         command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
   case e_TRBR: {  //
        const char * cbuffer = 0;
        rCode = channel->timedBufferedReadRaw(
                                 &cbuffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                 command->flag.d_interruptFlags);
    } break;
    case e_TRBRA: {  //
        int augStatus = 0;
        const char * cbuffer = 0;
        rCode = channel->timedBufferedReadRaw(
                                 &augStatus,
                                 &cbuffer,
                                 command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                 command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_W: {    //
        rCode = channel->write(buffer,
                               command->numToProcess.d_numBytes,
                               command->flag.d_interruptFlags);
    } break;
    case e_WA: {   //
        int augStatus = 0;
        rCode = channel->write(&augStatus,
                               buffer,
                               command->numToProcess.d_numBytes,
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TW: {    //
        rCode = channel->timedWrite(buffer,
                               command->numToProcess.d_numBytes,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case e_TWA: {   //
        int augStatus = e_DEFAULT_AUGSTATUS;
        rCode = channel->timedWrite(&augStatus,
                               buffer,
                               command->numToProcess.d_numBytes,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
        if (rCode > 0 ) {
            if (0 <= augStatus) {
                LOOP_ASSERT(command->d_lineNum,
                       rCode > 0 && rCode < command->numToProcess.d_numBytes);
            }
            else if (e_DEFAULT_AUGSTATUS == augStatus) {
                LOOP_ASSERT(command->d_lineNum,
                            rCode == command->numToProcess.d_numBytes);
            }
        }
    } break;
    case e_WR: {   //
        rCode = channel->writeRaw(buffer,
                                  command->numToProcess.d_numBytes,
                                  command->flag.d_interruptFlags);
    } break;
    case e_WRA: {  //
        int augStatus = 0;
        rCode = channel->writeRaw(&augStatus,
                                  buffer,
                                  command->numToProcess.d_numBytes,
                                  command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
     case e_TWR: {   //
        rCode = channel->timedWriteRaw(buffer,
                                  command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                  command->flag.d_interruptFlags);
    } break;
    case e_TWRA: {  //
        int augStatus = 0;
        rCode = channel->timedWriteRaw(&augStatus,
                                  buffer,
                                  command->numToProcess.d_numBytes,
                                *command->d_timeout + bdlt::CurrentTime::now(),
                                  command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
   case e_WVO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->writev(oBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
    } break;
    case e_WVI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->writev(ioBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
    } break;
    case e_WVOA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->writev(&augStatus,
                                oBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_WVIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->writev(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
   case e_TWVO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->timedWritev(oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case e_TWVI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->timedWritev(ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case e_TWVOA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->timedWritev(&augStatus,
                                oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TWVIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->timedWritev(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_WVRO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->writevRaw(oBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
    } break;
    case e_WVRI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->writevRaw(ioBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
    } break;
    case e_WVROA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->writevRaw(&augStatus,
                                   oBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_WVRIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->writevRaw(&augStatus,
                                   ioBuffer,
                                   command->numToProcess.d_numBuffers,
                                   command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TWVRO: {  //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        rCode = channel->timedWritevRaw(oBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
    } break;
    case e_TWVRI: {  //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        rCode = channel->timedWritevRaw(ioBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
    } break;
    case e_TWVROA: { //
        fillBuffers(oBuffer, command->numToProcess.d_numBuffers, 'o');
        int augStatus = 0;
        rCode = channel->timedWritevRaw(&augStatus,
                               oBuffer,
                               command->numToProcess.d_numBuffers,
                              *command->d_timeout + bdlt::CurrentTime::now(),
                               command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_TWVRIA: { //
        fillBuffers(ioBuffer, command->numToProcess.d_numBuffers, 'i');
        int augStatus = 0;
        rCode = channel->timedWritevRaw(&augStatus,
                                ioBuffer,
                                command->numToProcess.d_numBuffers,
                               *command->d_timeout + bdlt::CurrentTime::now(),
                                command->flag.d_interruptFlags);
        LOOP_ASSERT(command->d_lineNum, augStatus == command->d_expStatus);
    } break;
    case e_HELP_READ: {   //
        rCode = btlso::SocketImpUtil::read( buffer,
                                           socketPair[1],
                                           command->numToProcess.d_numBytes);
        ASSERT(rCode == command->numToProcess.d_numBytes);
        if (verbose) {
            QT("help socket read bytes = ");  P_T(rCode);
        }
    } break;
    case e_HELP_WRITE: {   //
        char ch = 'x';  memset(buffer, ch, command->numToProcess.d_numBytes);
        rCode = btlso::SocketImpUtil::write(socketPair[1],
                                           buffer,
                                           command->numToProcess.d_numBytes);
        if (verbose) {
            QT("help socket write bytes: ");     PT(rCode);
        }
    } break;
    case e_INVALIDATE: {    //
        channel->invalidate();
        rCode = 0;
    } break;
    case e_SLEEP: {    //
        PT(command->numToProcess.d_milliseconds)
        bslmt::ThreadUtil::microSleep(command->numToProcess.d_milliseconds);
        rCode = 0;
    } break;

    #ifdef BSLS_PLATFORM_OS_UNIX
    case e_SIGNAL: {
        // Create a thread to generate signals.
        bslmt::ThreadUtil::Handle tid = bslmt::ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_signalIoFlag;
        info->d_ioType = ioType;

        bslmt::ThreadAttributes attributes;
        int ret = bslmt::ThreadUtil::create(threadHandle,
                                           attributes,
                                           threadSignalGenerator,
                                           info);
        ASSERT(0 == ret);
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
        if (ret) {
            if (verbose) {
                QT("Thread creation failed, return value: ");
                PT(ret);
            }
        }
    } break;
    #endif
    case e_CLOSE_CONTROL: {
        // Create a thread to close the control socket (the peer socket).
        bslmt::ThreadUtil::Handle tid = bslmt::ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[1];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;
        info->d_ioType = ioType;

        bslmt::ThreadAttributes attributes;
        int ret = bslmt::ThreadUtil::create(threadHandle,
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
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
    } break;
    case e_CLOSE_OBSERVE: {
        bslmt::ThreadUtil::Handle tid = bslmt::ThreadUtil::self();

        ConnectInfo *info = 0;
        info = (ConnectInfo *) testAllocator.allocate(sizeof (ConnectInfo));

        //        info = new(testAllocator) ConnectInfo;
        info->d_tid = tid;
        info->d_socketHandle = socketPair[0];
        info->d_signals = command->numToProcess.d_signals;
        info->d_signalIoFlag = command->flag.d_interruptFlags;
        info->d_ioType = ioType;

        bslmt::ThreadAttributes attributes;
        int ret = bslmt::ThreadUtil::create(threadHandle,
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
#ifdef BSLS_PLATFORM_OS_LINUX
        bslmt::ThreadUtil::microSleep(5 * k_SLEEP_TIME);
#else
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
#endif

    } break;
    default:
        ASSERT("The test command is wrong!" && 0);
        break;
    }
    return rCode;
}

static
int processTest(btlsos::TcpTimedChannel     *channel,
                btlso::SocketHandle::Handle *helperSocket,
                TestCommand                 *commands,
                char                        *buffer,
                btls::Iovec                 *ioBuffer,
                btls::Ovec                  *oBuffer,
                int                          ioType)
    // The specified 'numCommands' of test commands will be issued in the
    // specified 'commands' to invoke functions in the specified 'channel', or
    // the 'helperSocket' which is the control part of the socket pair.  If the
    // 'signals' is set, a thread taking the specified 'threadFunction'
    // function will be compared against those expected which are also
    // specified in the 'commands'.  For a read operation, load either the
    // specified 'buffer' or 'ioBuffer' corresponding to the request.  Note
    // because the behaviors are different for "write" operations on different
    // platform, the specified 'ioType' is to indicate that the type of the
    // function being tested, e.g. it's a "read" or "write" operation so that
    // different testing configuration can be set on a specific platform.  For
    // a write operation, write data from either 'buffer' or 'ioBuffer' or the
    // specified 'oBuffer' corresponding to the request.  Return 0 on success,
    // and a non-zero value otherwise.
{
    bslmt::ThreadUtil::Handle threadHandle;
    int ret = 0, numErrors = 0;

    for (int i = 0; i < k_MAX_CMD; i++) { // different test data
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
        if (commands[i].d_command < k_READ_FUNCTIONS) { // "Read" operations
            if (e_INTERRUPTED == commands[i].d_expStatus) {
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0);
            }
            else {
                LOOP3_ASSERT(commands[i].d_lineNum, ret,
                             commands[i].d_expReturnValue,
                             ret == commands[i].d_expReturnValue);
            }
        }
        else if (commands[i].d_command < k_WRITE_FUNCTIONS){
            if (ret < 0) {
                LOOP_ASSERT(commands[i].d_lineNum,
                            ret == commands[i].d_expReturnValue);
            }
            else if (commands[i].d_command >= e_WVO &&
                      commands[i].d_command <= e_TWVROA) {
                int length = btls::IovecUtil::length(oBuffer,
                                      commands[i].numToProcess.d_numBuffers);
                LOOP_ASSERT(commands[i].d_lineNum, ret > 0 && ret <= length);
            }
            else if (commands[i].d_command >= e_WVI &&
                     commands[i].d_command <= e_TWVRIA) {
                int length = btls::IovecUtil::length(ioBuffer,
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
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
    }
    bslmt::ThreadUtil::join(threadHandle);

    return numErrors;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2; // global variable
    veryVerbose = argc > 3; // global variable
    int veryVeryVerbose = argc > 4;
    int errCode = 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#ifdef BSLS_PLATFORM_OS_UNIX
    registerSignal(SIGPIPE, signalHandler); // register a handler for 'SIGPIPE'
    registerSignal(SIGSYS, signalHandler); // register a handler for 'SIGSYS'
    // A write() on the closed socket will generate SIGPIPE.
#endif

    testAllocator.setNoAbort(1);
    testAllocator.setVerbose(veryVeryVerbose);

    btlso::SocketImpUtil::startup(&errCode);

    btlso::InetStreamSocketFactory<btlso::IPv4Address> factory(&testAllocator);

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

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            ASSERT(0 == ret);
            // The following socket options are set only if necessary.

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, 8192);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, 8192);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                          btlso::SocketOptUtil::k_TCPLEVEL,
                          btlso::SocketOptUtil::k_TCPNODELAY, 1);
            ASSERT(0 == ret);

            // Next, create a 'btlso::StreamSocket' object, which is a part of
            // the channel.  The 'btlso::StreamSocket' object has a field of
            // type 'btlso::SocketHandle::Handle', whose value is set to a
            // socket created above.
            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            ASSERT(sSocket);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor,
                // the behavior is undefined otherwise.  We insure the required
                // order by creating the 'channel' inside a block while the
                // corresponding 'streamSocket' object outside the block as
                // above.

                Obj channel(sSocket);
                ASSERT(0 == channel.isInvalid());

                // Write data at the other side of the channel and so "read"
                // operations can be done at the channel side.
                enum { k_LEN = 30 };
                char writeBuf[k_LEN] = "abcdefghij1234567890",
                     readBuf[k_LEN];
                int numBytes = 0, augStatus = -1, interruptFlag = 1;
                int len = btlso::SocketImpUtil::write(handles[1],
                                                      writeBuf,
                                                      static_cast<int>(
                                                            strlen(writeBuf)));

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
                bsls::TimeInterval timer = bdlt::CurrentTime::now();
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
                enum { e_INVALID = -2 };
                // Try writing 5 bytes from the channel.
                len = channel.read(&augStatus, readBuf,
                                   numBytes, interruptFlag);
                ASSERT(e_INVALID == len);

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes from the channel with a timeout value.
                len = channel.timedRead(&augStatus, readBuf,
                                        numBytes, timer, interruptFlag);
                ASSERT(e_INVALID == len);
                // Try writing 1 byte to the channel.
                numBytes = 1;
                augStatus = -1;
                len = channel.write(&augStatus, writeBuf,
                                    numBytes, interruptFlag);
                ASSERT(e_INVALID == len);

                timer.addMilliseconds(milliSec);
                timer.addNanoseconds(nanoSec);
                numBytes = 10;
                augStatus = -1;
                // Try writing 10 bytes to the channel with a timeout value.
                len = channel.timedWrite(&augStatus, writeBuf,
                                         numBytes, timer, interruptFlag);
                ASSERT(e_INVALID == len);
            }
            factory.deallocate(sSocket);
            if (verbose) {
                QT("The usage example is over.");
            }
        }
      } break;
      case 21: {
#if !defined(BSLS_PLATFORM_OS_LINUX) && !(defined(BSLS_PLATFORM_OS_SOLARIS) \
                                           || BSLS_PLATFORM_OS_VER_MAJOR < 10)
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int timedWritevRaw(const btls::Ovec         *buffer,
        //                      int                      numBytes,
        //                      const bsls::TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btls::Ovec         *buffer,
        //                      int                      numBytes,
        //                      const bsls::TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btls::Iovec        *buffer,
        //                      int                      numBytes,
        //                      const bsls::TimeInterval& time,
        //                      int                      flags = 0);
        //   int timedWritevRaw(int                     *augStatus,
        //                      const btls::Iovec        *buffer,
        //                      int                      numBytes,
        //                      const bsls::TimeInterval& time,
        //                      int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("TESTING 'timedWriteRaw' METHOD");
            QT("==============================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
     {L_,  e_TWVRO,          1,        0,          1,         0,   &longTime },
     {L_,      e_W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
     {L_,    e_TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,  e_TWVRO,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,  e_TWVRO,          2,        0,        e_INVALID,     0,   &longTime },
     {L_, TWVROA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 2: the same test as the above for "btls::Iovec" buffers
    {
      // Establish a channel and verify that it works fine.
     {L_,  e_TWVRI,          1,        0,          1,         0,   &longTime },
     {L_,      e_W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
     {L_,    e_TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,  e_TWVRI,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,  e_TWVRI,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,  e_TWVIA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 3: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVROA,          1,        0,          1,         0,   &longTime },
     {L_,      e_W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
     {L_,    e_TWR,     BUF_WRITE,     0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
     {L_, TWVROA,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,  e_TWVRO,          2,        0,        e_INVALID,     0,   &longTime },
     {L_, TWVROA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 4: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
     {L_,   e_TWVRIA,        1,        0,          1,         0,   &longTime },
     {L_,      e_W, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
     {L_,     e_TWR,     BUF_WRITE,    0,       8192,         0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,  e_TWVRIA,         7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
     {L_, TWVRIA,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,  e_TWVRI,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 5: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRO,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVRO,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
      {L_, TWVRO,          2,        0,        e_INVALID,     0,   &longTime },
      {L_, TWVRI,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 6: the same test as the above for "btls::Iovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRI,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVRI,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
      {L_, TWVRI,          2,        0,        e_INVALID,     0,   &longTime },
      {L_, TWVRO,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 7: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVROA,          1,        0,           1,        0,   &longTime },
     {L_,   e_TWR,     BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_OBSERVE,   0,        0,           0,        0,   &longTime },
      {L_, TWVROA,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
     {L_, TWVROA,          2,        0,        e_INVALID,     0,   &longTime },
     {L_, TWVRIA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 8: the same test as the above for "btls::Ovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVRIA,          1,        0,           1,        0,   &longTime },
     {L_,    e_TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_OBSERVE,   0,        0,           0,        0,   &longTime },
     {L_, TWVRIA,          7,        0,         e_ERR,        0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
     {L_, TWVRIA,          2,        0,        e_INVALID,     0,   &longTime },
     {L_, TWVROA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },
    #endif
    #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    {
    {L_,    e_TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
    {L_,  e_TWVROA,        1,         0,           1,         0,   &timeout1 },
    {L_,  e_TWVROA,        7,         0,           0,         0,   &timeout2 },
      {L_,    -1,          0,         0,           0,         0,   &longTime },
    },
    {
    {L_,    e_TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
    {L_,  e_TWVRO,         1,         0,           1,         0,   &timeout1 },
    {L_,  e_TWVRO,         7,         0,           0,         0,   &timeout2 },
      {L_,   -1,           0,         0,           0,         0,   &longTime },
    },
    {
    {L_,    e_TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
    {L_,  e_TWVRIA,        1,         0,           1,         0,   &timeout1 },
    {L_,  e_TWVRIA,        7,         0,           0,         0,   &timeout2 },
      {L_,    -1,          0,         0,           0,         0,   &longTime },
    },
    {
    {L_,    e_TW,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
    {L_,  e_TWVRI,         1,         0,           1,         0,   &timeout1 },
    {L_,  e_TWVRI,         7,         0,           0,         0,   &timeout2 },
      {L_,   -1,           0,         0,           0,         0,   &longTime },
    },

    #else                                       // unix test data

    #ifdef BSLS_PLATFORM_OS_SOLARIS
    // commands set 9: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,     e_WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
     {L_,  e_TWVRO,          7,        1,         8192,       0,   &longTime },
   {L_,  e_SIGNAL,         2,  e_READ_OP,          0,         0,   &longTime },
     {L_,  e_TWVRO,          7,        1,      40960,         0,   &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,     e_WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
     {L_,  e_TWVRI,          7,        1,         8192,       0,   &longTime },
   {L_,  e_SIGNAL,         2,  e_READ_OP,          0,         0,   &longTime },
     {L_,  e_TWVRI,          7,        1,      40960,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,     e_WR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_, TWVROA,          7,        1,         8192,       0,   &longTime },
   {L_, e_SIGNAL,          2,  e_READ_OP,            0,       0,   &longTime },
      {L_, TWVROA,          7,        1,         40960,      0,   &longTime },
    },

    // commands set 12: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,     e_WR,   BUF_WRITE,       0,      BUF_WRITE,     0,   &longTime },
     {L_,  e_TWVRIA,         7,        1,         8192,       0,   &longTime },
   {L_,  e_SIGNAL,         2,  e_READ_OP,            0,       0,   &longTime },
     {L_,  e_TWVRIA,         7,        1,        40960,       0,   &longTime },
    },
    #else
    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,  e_TWVO,           1,        0,            1,       0,   &longTime },
     {L_,     e_W, SYS_DEPENDENT_LEN,  0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRO,           7,        1,         1024,       0,   &longTime },
   {L_, e_SIGNAL,          2,  e_READ_OP,            0,       0,   &longTime },
      {L_, TWVRO,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
     {L_,  e_TWVI,           1,        0,            1,       0,   &longTime },
     {L_,     e_W, SYS_DEPENDENT_LEN,  0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRI,           7,        1,         1024,       0,   &longTime },
   {L_, e_SIGNAL,          2,  e_READ_OP,            0,       0,   &longTime },
      {L_, TWVRI,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
    {L_,  e_TWVOA,           1,        0,            1,       0,   &longTime },
    {L_,     e_W, SYS_DEPENDENT_LEN,   0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVROA,           7,        1,         1024,       0,   &longTime },
  {L_, e_SIGNAL,           2,  e_READ_OP,            0,       0,   &longTime },
      {L_, TWVROA,           7,        1,         1024,       0,   &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
    {L_,  e_TWVIA,           1,        0,            1,       0,   &longTime },
    {L_,     e_W, SYS_DEPENDENT_LEN,   0, SYS_DEPENDENT_LEN,  0,   &longTime },
      {L_, TWVRIA,           7,        1,         1024,       0,   &longTime },
  {L_, e_SIGNAL,           2,  e_READ_OP,            0,       0,   &longTime },
      {L_, TWVRIA,           7,        1,         1024,       0,   &longTime },
    },

    #endif

    // commands set 13: The "write" operation reaches its timeout.
    {
    {L_,  e_TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_TWVRO,           7,        0,        8192,        0,   &longTime },
    {L_,  e_TWVRO,           7,        0,           0,        0,   &timeout1 },
    },
     // commands set 14: The "write" operation reaches its timeout.
    {
    {L_,  e_TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_TWVRI,           7,        0,        8192,        0,   &longTime },
    {L_,  e_TWVRI,           7,        0,           0,        0,   &timeout1 },
    },
    // commands set 15: The "write" operation reaches its timeout.
    {
    {L_,  e_TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_TWVROA,          7,        0,        8192,        0,   &longTime },
    {L_,  e_TWVROA,          7,        0,           0,        0,   &timeout1 },
    },
    // commands set 16: The "write" operation reaches its timeout.
    {
    {L_,  e_TW,       BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_TWVROA,          7,        0,        8192,        0,   &longTime },
    {L_,  e_TWVROA,          7,        0,           0,        0,   &timeout1 },
    },
    #endif
 };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[WVECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     #ifdef BSLS_PLATFORM_OS_SOLARIS
                         buf4[WVECBUF_LEN80] = "\0",
                         buf5[WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[WVECBUF_LEN500] = "\0",
                         buf5[WVECBUF_LEN1K] = "\0",
                     #endif
                     buf6[WVECBUF_LEN16K] = "\0",
                     buf7[WVECBUF_LEN32K] = "\0";

                btls::Ovec ovecBuffer[k_MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btls::Iovec iovecBuffer[k_MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int timedWritev(const btls::Ovec          *buffer,
        //                   int                       numBytes,
        //                   const bsls::TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btls::Ovec          *buffer,
        //                   int                       numBytes,
        //                   const bsls::TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btls::Iovec         *buffer,
        //                   int                       numBytes,
        //                   const bsls::TimeInterval&  time,
        //                   int                       flags = 0);
        //   int timedWritev(int                      *augStatus,
        //                   const btls::Iovec         *buffer,
        //                   int                       numBytes,
        //                   const bsls::TimeInterval&  time,
        //                   int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWritev' method");
            QT("============================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
     {L_,   e_TWVO,          1,        0,          1,         0,   &longTime },
     {L_,     e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,   e_TWVO,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,   e_TWVO,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,  e_TWVOA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 2: the same test as the above for "btls::Iovec" buffers
    {
      // Establish a channel and verify that it works fine.
     {L_,   e_TWVI,          1,        0,          1,         0,   &longTime },
     {L_,     e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,   e_TWVI,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,   e_TWVI,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,  e_TWVIA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 3: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
     {L_,  e_TWVOA,          1,        0,          1,         0,   &longTime },
     {L_,     e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
   {L_,  e_TWVOA,          7,        0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,   e_TWVO,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,  e_TWVOA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    // Command set 4: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
     {L_,   e_TWVIA,         1,        0,          1,         0,   &longTime },
     {L_,     e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
     {L_, e_CLOSE_CONTROL,   0,        0,          0,         0,   &longTime },
      {L_,  e_TWVIA,          7, 0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
   {L_,  e_TWVIA,          2,        0,        e_INVALID,     0,   &longTime },
   {L_,   e_TWVI,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,           0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 5: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  e_TWVO,          1,        0,           1,        0,   &longTime },
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  e_TWVO,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,  e_TWVO,          2,        0,        e_INVALID,     0,   &longTime },
    {L_,  e_TWVI,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 6: the same test as the above for "btls::Iovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_,  e_TWVI,          1,        0,           1,        0,   &longTime },
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  e_TWVI,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,  e_TWVI,          2,        0,        e_INVALID,     0,   &longTime },
    {L_,  e_TWVO,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 7: the same test as the above for "btls::Ovec" buffers with
    // 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVOA,          1,        0,           1,        0,   &longTime },
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVOA,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
      {L_, TWVOA,          2,        0,        e_INVALID,     0,   &longTime },
      {L_, TWVIA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 8: the same test as the above for "btls::Ovec" buffers
    // without 'augStatus' parameter
    {
      // Establish a channel and verify that it works fine.
      {L_, TWVIA,          1,        0,           1,        0,   &longTime },
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_, TWVIA,          7,        0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
      {L_, TWVIA,          2,        0,        e_INVALID,     0,   &longTime },
      {L_, TWVOA,          6,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif

    #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    {
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_TWVOA,        1,          0,           1,         0,   &timeout1 },
    {L_,  e_TWVOA,        7,          0,           30,        0,   &timeout2 },
      {L_,    -1,         0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_TWVO,         1,          0,           1,         0,   &timeout1 },
    {L_,  e_TWVO,         7,          0,           30,        0,   &timeout2 },
      {L_,   -1,          0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_TWVIA,        1,          0,           1,         0,   &timeout1 },
    {L_,  e_TWVIA,        7,          0,           30,        0,   &timeout2 },
      {L_,    -1,         0,          0,           0,         0,   &longTime },
    },
    {
      {L_,    e_TW,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_TWVI,         1,          0,           1,         0,   &timeout1 },
    {L_,  e_TWVI,         7,          0,           30,        0,   &timeout2 },
      {L_,   -1,          0,          0,           0,         0,   &longTime },
    },

    #else                                       // unix test data

    #ifdef BSLS_PLATFORM_OS_SOLARIS

    // commands set 9: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  e_TWVO,          1,        0,          1,         0,   &longTime },
      {L_,  e_TWVO,          5,        0,        164,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,  e_TWVO,          7,        1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,  e_TWVI,          1,        0,          1,         0,   &longTime },
      {L_,  e_TWVI,          5,        0,        164,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,  e_TWVI,          7,        1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 11: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_, TWVOA,          1,        0,          1,         0,   &longTime },
      {L_, TWVOA,          5,        0,        164,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_, TWVOA,          7,        1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 12: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_, TWVIA,          1,        0,          1,         0,   &longTime },
      {L_, TWVIA,          5,        0,        164,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
      {L_, TWVIA,          7,        1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 13: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_SIGNAL,        2,    e_READ_OP,        0,         0,   &longTime },
      {L_,  e_TWVO,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 14: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_SIGNAL,        2,    e_READ_OP,        0,         0,   &longTime },
      {L_,  e_TWVI,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 15: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_SIGNAL,        2,    e_READ_OP,        0,         0,   &longTime },
      {L_, TWVOA,          7,        0,      24740,         0,   &longTime },
    },

    // commands set 16: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
    {L_,  e_SIGNAL,        2,    e_READ_OP,        0,         0,   &longTime },
      {L_,  e_TWVIA,         7,        0,      24740,         0,   &longTime },
    },

    #elif BSLS_PLATFORM_OS_LINUX

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_, TWVO,            5,        0,       164,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            7,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_, TWVI,            5,        0,       164,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            7,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_, TWVOA,            5,        0,       164,         0,   &longTime },
     {L_,    e_W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,  e_READ_OP,         0,         0,   &longTime },
     {L_, TWVOA,            7,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_, TWVIA,            5,        0,       164,         0,   &longTime },
     {L_,    e_W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,  e_READ_OP,         0,         0,   &longTime },
     {L_, TWVIA,            7,        1,         0, e_INTERRUPTED, &longTime },
    },

    // commands set 9: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 10: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
     {L_,     e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,          2,   e_READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 12: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
     {L_,     e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,   e_READ_OP,        0,         0,   &longTime },
      {L_, TWVIA,            6,        0,      1608,         0,   &longTime },
    },
    #else

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_, TWVO,            5,        0,       164,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_, TWVI,            5,        0,       164,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
      {L_, TWVOA,            5,        0,       164,         0,   &longTime },
     {L_,    e_W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,  e_READ_OP,         0,         0,   &longTime },
     {L_, TWVOA,            6,        1,         0, e_INTERRUPTED, &longTime },
    },
    // commands set 8: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
      {L_, TWVIA,            5,        0,       164,         0,   &longTime },
     {L_,    e_W,   SYS_DEPENDENT_LEN,  1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,  e_READ_OP,         0,         0,   &longTime },
     {L_, TWVIA,            6,        1,         0, e_INTERRUPTED, &longTime },
    },

    // commands set 9: to resolve concern 4 - 6.
    {
      {L_, TWVO,            1,        0,         1,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVO,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 10: to resolve concern 4 - 6.
    {
      {L_, TWVI,            1,        0,         1,         0,   &longTime },
      {L_,    e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,          2,  e_READ_OP,         0,         0,   &longTime },
      {L_, TWVI,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 11: to resolve concern 4 - 6.
    {
      {L_, TWVOA,            1,        0,         1,         0,   &longTime },
     {L_,     e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,          2,   e_READ_OP,         0,         0,   &longTime },
      {L_, TWVOA,            6,        0,      1608,         0,   &longTime },
    },
    // commands set 12: to resolve concern 4 - 6.
    {
      {L_, TWVIA,            1,        0,         1,         0,   &longTime },
     {L_,     e_W,   SYS_DEPENDENT_LEN, 1, SYS_DEPENDENT_LEN, 0,   &longTime },
   {L_, e_SIGNAL,           2,   e_READ_OP,        0,         0,   &longTime },
      {L_, TWVIA,            6,        0,      1608,         0,   &longTime },
    },
    #endif

    // commands set 17: The "write" operation reaches its timeout.
    {
     {L_,    e_TW,    BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
     {L_,  e_TWVO,            7,       1,       8192,         0,   &timeout1 },
    },

    // commands set 18: The "write" operation reaches its timeout.
    {
      {L_,    e_TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,  e_TWVI,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 19: The "write" operation reaches its timeout.
    {
      {L_,    e_TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_, TWVOA,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 20: The "write" operation reaches its timeout.
    {
      {L_,    e_TW,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_, TWVIA,          7,        1,       8192,         0,   &timeout1 },
    },

    // commands set 21: For "write" operation in 'non-interruptible' mode, "AE"
    // can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  e_SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  e_TWVO,          6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 22: For "write" operation in 'non-interruptible' mode, "AE"
    // can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  e_SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  e_TWVI,          6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 23: For "write" operation in 'non-interruptible' mode, "AE"
    // can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  e_SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  e_TWVOA,         6,        0,       8192,         0,   &timeout2 },
    },

    // commands set 24: For "write" operation in 'non-interruptible' mode, "AE"
    // can't interrupt, while "timeout" can.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,  e_SIGNAL,        2,        0,        0,           0,   &longTime },
      {L_,  e_TWVIA,         6,        0,       8192,         0,   &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[WVECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[WVECBUF_LEN20] = "\0", buf3[WVECBUF_LEN60] = "\0",
                     #ifdef BSLS_PLATFORM_OS_SOLARIS
                         buf4[WVECBUF_LEN80] = "\0",
                         buf5[WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[WVECBUF_LEN500] = "\0",
                         buf5[WVECBUF_LEN1K] = "\0",
                     #endif
                     buf6[WVECBUF_LEN16K] = "\0",
                     buf7[WVECBUF_LEN32K] = "\0";

                btls::Ovec ovecBuffer[k_MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btls::Iovec iovecBuffer[k_MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
#if !defined(BSLS_PLATFORM_OS_LINUX) && !(defined(BSLS_PLATFORM_OS_SOLARIS) \
                                           || BSLS_PLATFORM_OS_VER_MAJOR < 10)
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                     const bsls::TimeInterval& time,
        //                     int                      flags = 0);
        //   int timedWriteRaw(int                     *augStatus,
        //                     const char              *buffer,
        //                     int                      numBytes,
        //                     const bsls::TimeInterval& time,
        //                     int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWrite' method");
            QT("===========================");
        }

        {
            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   e_TWR,          1,        0,          1,         0,   &longTime },
      {L_,   e_TW,  SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      {L_,   e_TWR,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,  0,        0,          0,         0,   &longTime },
    {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,   e_TWR,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,   e_TWR,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  e_TWRA,          1,        0,           1,        0,   &longTime },
      {L_,   e_TW,  SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      {L_,  e_TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,  0,        0,           0,        0,   &longTime },
    {L_,  e_TWRA, SYS_DEPENDENT_LEN, 0,         e_CLOSED,     0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,  e_TWRA,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,   e_TWR,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   e_TWR,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
         // The channel will be closed by the peer when the 'read'
        // request is on going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,   e_TWR,        100,        0,       e_INVALID,      0,   &longTime },
    {L_,  e_TWRA,         60,        0,       e_INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  e_TWRA,          1,        0,           1,        0,   &longTime },
      {L_,  e_TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
       // The channel will be closed by the peer when the 'read'
      // request is on going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  e_TWRA, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,   e_TWR,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,  e_TWRA,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif
    #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    {
     {L_,  e_TWRA,        1,          0,           1,         0,   &longTime },
     {L_,  e_TWRA,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
     {L_,  e_TWRA, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,    -1,        0,          0,           0,         0,   &longTime },
    },
    {
     {L_,  e_TWR,         1,          0,           1,         0,   &longTime },
     {L_,  e_TWR,    BUF_WRITE,       0,      BUF_WRITE,      0,   &longTime },
     {L_,  e_TWR, SYS_DEPENDENT_LEN,  0,           0,         0,   &timeout2 },
      {L_,   -1,         0,          0,           0,         0,   &longTime },
    },

    #else                                       // unix test data
    #ifdef BSLS_PLATFORM_OS_SOLARIS
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   e_TWR,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,   e_TWR,        100,        0,       e_INVALID,      0,   &longTime },
    {L_,  e_TWRA,         60,        0,       e_INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,  e_TWRA,          1,        0,           1,        0,   &longTime },
      {L_,  e_TWRA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,  e_TWRA, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,   e_TWR,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,  e_TWRA,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,   e_TWR,          1,        0,          1,         0,   &longTime },
      {L_,   e_TWR,         15,        0,         15,         0,   &longTime },
      {L_,   e_TWR,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,       8192,         0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
    {L_,  e_SIGNAL,        2,    e_READ_OP,        0,         0,   &longTime },
      {L_,   e_TWR,     BUF_WRITE,     1,       8192,         0,   &longTime },
    },
    // commands set 6: to resolve concern
    {
      // Each request read expected number of bytes from the channel.
      {L_,  e_TWRA,          1,        0,          1,         0,   &longTime },
      {L_,  e_TWRA,         15,        0,         15,         0,   &longTime },
      {L_,  e_TWRA,    BUF_WRITE,      0,     BUF_WRITE,      0,   &longTime },
      {L_,   e_TWR, SYS_DEPENDENT_LEN, 0,        8192,        0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
    {L_,  e_SIGNAL,        2,     e_READ_OP,       0,         0,   &longTime },
      {L_,  e_TWRA,      BUF_WRITE,    1,       8192,         0,   &longTime },
    },
    #else   // ibm test data
    // commands set 3: to resolve concern 4 - 6.
    {
      {L_, TWR,            1,        0,           1,        0,   &longTime },
      {L_,   e_W,   SYS_DEPENDENT_LEN, 1,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, TWR,    BUF_WRITE,        0,        1024,        0,   &longTime },
    {L_, e_SIGNAL,          2,   e_READ_OP,         0,        0,   &longTime },
      {L_, TWR,    BUF_WRITE,        1,        1024,        0,   &longTime },
    },

    // commands set 4: to resolve concern 4 - 6.
    {
      {L_, TWRA,            1,        0,           1,        0,   &longTime },
     {L_,    e_W,  SYS_DEPENDENT_LEN,  1,  SYS_DEPENDENT_LEN, 0,   &longTime },
      {L_, TWRA,    BUF_WRITE,        0,        1024,        0,   &longTime },
   {L_, e_SIGNAL,          2,   e_READ_OP,          0,        0,   &longTime },
      {L_, TWRA,    BUF_WRITE,        1,        1024,        0,   &longTime },
    },

    #endif

    // commands set 5: The "write" operation reaches its timeout.
    {
     {L_,  e_TWRA,   BUF_WRITE,        0,        1024,        0,   &longTime },
     {L_,  e_TWRA, SYS_DEPENDENT_LEN,  0,        1024,        0,   &longTime },
     {L_,  e_TWRA, SYS_DEPENDENT_LEN,  0,           0,        0,   &timeout1 },
    },
    // commands set 6: The "write" operation reaches its timeout.
    {
     {L_,  e_TWR,   BUF_WRITE,         0,        1024,        0,   &longTime },
     {L_,  e_TWR, SYS_DEPENDENT_LEN,   0,        1024,        0,   &longTime },
     {L_,  e_TWR, SYS_DEPENDENT_LEN,   0,           0,        0,   &timeout1 },
    },
    // commands set 7: The "write" operation reaches its timeout.
    {
     {L_,  e_TWRA,   BUF_WRITE,        0,        1024,        0,   &longTime },
     {L_,  e_TWRA, SYS_DEPENDENT_LEN,  1,        1024,        0,   &longTime },
     {L_,  e_TWRA, SYS_DEPENDENT_LEN,  1,           0,        0,   &timeout1 },
    },
    // commands set 8: The "write" operation reaches its timeout.
    {
     {L_,  e_TWR,   BUF_WRITE,         0,        1024,        0,   &longTime },
     {L_,  e_TWR, SYS_DEPENDENT_LEN,   1,        1024,        0,   &longTime },
     {L_,  e_TWR, SYS_DEPENDENT_LEN,   1,           0,        0,   &timeout1 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
#if !defined(BSLS_PLATFORM_OS_LINUX) && !(defined(BSLS_PLATFORM_OS_SOLARIS) \
                                           || BSLS_PLATFORM_OS_VER_MAJOR < 10)
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                  const bsls::TimeInterval& time,
        //                  int                      flags = 0);
        //   int timedWrite(int                     *augStatus,
        //                  const char              *buffer,
        //                  int                      numBytes,
        //                  const bsls::TimeInterval& time,
        //                  int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedWrite' method");
            QT("===========================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][MAX_CMD] =
  //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
  //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,    e_TW,          1,        0,          1,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,  0,        0,          0,         0,   &longTime },
    {L_,    e_TW, SYS_DEPENDENT_LEN, 0,        e_CLOSED,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,    e_TW,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,    e_TW,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   e_TWA,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,  0,        0,           0,        0,   &longTime },
    {L_,   e_TWA, SYS_DEPENDENT_LEN, 0,         e_CLOSED,     0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,   e_TWA,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,   e_TWA,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,    e_TW,          1,        0,           1,        0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,    e_TW, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,    e_TW,        100,        0,       e_INVALID,      0,   &longTime },
    {L_,    e_TW,         60,        0,       e_INVALID,      0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_,   e_TWA,          1,        0,           1,        0,   &longTime },
      {L_,   e_TWA,    BUF_WRITE,      0,      BUF_WRITE,     0,   &longTime },
      // The channel will be closed by the peer when the 'read' request is on
      // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_OBSERVE,  0,        0,           0,        0,   &longTime },
      {L_,   e_TWA, SYS_DEPENDENT_LEN, 0,         e_ERR,      0,   &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,   e_TWA,        100,        0,        e_INVALID,     0,   &longTime },
    {L_,   e_TWA,         60,        0,        e_INVALID,     0,   &longTime },
      {L_,    -1,          0,        0,           0,        0,   &longTime },
    },
    #endif

    #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    {
     {L_,   e_TWA,        1,          0,           1,         0,   &longTime },
     {L_,   e_TWA,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
     {L_,   e_TWA, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,    -1,        0,          0,           0,         0,   &longTime },
    },
    {
      {L_,   e_TW,        1,          0,           1,         0,   &longTime },
      {L_,   e_TW,    BUF_WRITE,      0,      BUF_WRITE,      0,   &longTime },
      {L_,   e_TW, SYS_DEPENDENT_LEN, 0,           0,         0,   &timeout2 },
      {L_,   -1,        0,          0,           0,         0,   &longTime },
    },

    #else

    #if defined(BSLS_PLATFORM_OS_SOLARIS)      // SUN test data

    // commands set 5: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,          1,        0,          1,         0,   &longTime },
      {L_,    e_TW,         15,        0,         15,         0,   &longTime },
      {L_,    e_TW,     BUF_WRITE,     0,      BUF_WRITE,     0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,    e_TW, SYS_DEPENDENT_LEN, 1,       8192, e_INTERRUPTED, &longTime },
    },
    // commands set 6: to resolve concern
    {
      // Each request read expected number of bytes from the channel.
      {L_,   e_TWA,          1,        0,          1,         0,   &longTime },
      {L_,   e_TWA,         15,        0,         15,         0,   &longTime },
      {L_,   e_TWA,    BUF_WRITE,      0,     BUF_WRITE,      0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,   e_TWA,       10000,       1,       8192, e_INTERRUPTED, &longTime },
    },
    // commands set 7: to resolve concern 4 - 6.
    {
      {L_,   e_TWA,     BUF_WRITE,     0,    BUF_WRITE,       0,   &longTime },
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,    e_TW,       10000,       1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 8: to resolve concern 4 - 6.
    {
      {L_,   e_TWA,    BUF_WRITE,      0,    BUF_WRITE,       0,   &longTime },
      {L_,  e_SIGNAL,       2,         0,        0,           0,   &longTime },
    {L_,   e_TWA,       10000,       1,      8192, e_INTERRUPTED,  &longTime },
    },
    // commands set 9: to resolve concern 4 - 6.
    {
      {L_,   e_TWA,    BUF_WRITE,      0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_SIGNAL,       2,      e_READ_OP,     0,           0,   &longTime },
      {L_,    e_TW, SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN,  0,   &longTime },
    },

    // commands set 10: to resolve concern 4 - 6.
    {
      {L_,   e_TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_SIGNAL,       2,      e_READ_OP,     0,           0,   &longTime },
      {L_,   e_TWA,       10000,       0,      10000,         0,   &longTime },
    },
    // commands set 15: The "write" operation in "non_interruptible" mode
    // reaches its timeout w/o being interrupted by "asynchronous events"
    // during that period.
    {
      {L_,   e_TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_SIGNAL,       2,      e_READ_OP,     0,           0,   &longTime },
      {L_,   e_TWA,   BUF_WRITE,       0,      49152,         0,   &timeout2 },
    },
    // commands set 16: The "write" operation in "non_interruptible" mode
    // reaches its timeout w/o being interrupted by "asynchronous events"
    // during that period.
    {
      {L_,   e_TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
    {L_,  e_SIGNAL,       2,      e_READ_OP,     0,           0,   &longTime },
      {L_,   e_TWA,   BUF_WRITE,       0,     49152,          0,   &timeout2 },
    },

    #else         // ibm test data

    // commands set 3: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,    e_TW,          1,        0,          1,         0,   &longTime },
      {L_,    e_TW, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,    e_TW, SYS_DEPENDENT_LEN, 1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 4: to resolve concern 4 - 6.
    {
      // Each request read expected number of bytes from the channel.
      {L_,   e_TWA,          1,        0,          1,         0,   &longTime },
      {L_,   e_TWA, SYS_DEPENDENT_LEN, 0,  SYS_DEPENDENT_LEN, 0,   &longTime },
        // There are not enough space in the TCP buffer for next request, now
        // we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        2,        0,          0,         0,   &longTime },
    {L_,   e_TWA, SYS_DEPENDENT_LEN, 1,       8192, e_INTERRUPTED, &longTime },
    },

    // commands set 5: to resolve concern 4 - 6.
    {
      {L_,   e_TWA,          1,         0,          1,        0,   &longTime },
      {L_,   e_TWA,  SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,         2,      e_READ_OP,       0,        0,   &longTime },
      {L_,   e_TWA,       2000,         0,       2000,        0,   &longTime },
    },

    // commands set 6: to resolve concern 4 - 6.
    {
      {L_,    e_TW,          1,         0,          1,        0,   &longTime },
      {L_,    e_TW,  SYS_DEPENDENT_LEN, 0, SYS_DEPENDENT_LEN, 0,   &longTime },
    {L_, e_SIGNAL,         2,      e_READ_OP,       0,        0,   &longTime },
      {L_,    e_TW,       2000,         0,       2000,        0,   &longTime },
    },

    #endif

    // commands set 11: The "write" operation reaches its timeout.
    {
      {L_,   e_TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,   e_TWA, SYS_DEPENDENT_LEN, 0,        1024,        0,   &timeout1 },
    },
    // commands set 12: The "write" operation reaches its timeout.
    {
      {L_,   e_TW,   BUF_WRITE,        0,    BUF_WRITE,       0,   &longTime },
      {L_,   e_TW, SYS_DEPENDENT_LEN,  0,        1024,        0,   &timeout1 },
    },
    // commands set 13: The "write" operation reaches its timeout.
    {
      {L_,   e_TWA,   BUF_WRITE,       0,    BUF_WRITE,       0,   &longTime },
      {L_,   e_TWA, SYS_DEPENDENT_LEN, 1,        8192,        0,   &timeout1 },
    },
    // commands set 14: The "write" operation reaches its timeout.
    {
      {L_,   e_TW,   BUF_WRITE,        0,    BUF_WRITE,       0,   &longTime },
      {L_,   e_TW, SYS_DEPENDENT_LEN,  1,        8192,        0,   &timeout1 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType = 0;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                            const bsls::TimeInterval&  time,
        //                            int                       flags = 0);
        //   int timedBufferedReadRaw(int                      *augStatus,
        //                            char                    **buffer,
        //                            int                       numBytes,
        //                            const bsls::TimeInterval&  time,
        //                            int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedBufferedReadRaw' method");
            QT("=====================================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,      0     },
    {L_,  e_TRBR,           1,         0,           1,         0,  &longTime },
    {L_,  e_TRBR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,  e_TRBR,         100,         0,         e_CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
  {L_,  e_TRBR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_, e_TRBRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_, e_TRBRA,          10,         0,          10,         0,  &longTime },
    {L_, e_TRBRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_, e_TRBRA,         100,         0,         e_CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
  {L_,  e_TRBR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_, e_TRBRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to Command set 3:
    // Close the channel at the channel side to test the behavior of the 'read'
    // method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,  e_TRBR,           1,         0,           1,         0,  &longTime },
    {L_,  e_TRBR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
    {L_, e_TRBRA,          20,         0,         e_ERR,       0,  &longTime },
  {L_,  e_TRBR,          80,         0,        e_INVALID,      0,  &longTime },
  {L_, e_TRBRA,          40,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_, e_TRBRA,           1,         0,           1,         0,  &longTime },
    {L_, e_TRBRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
  {L_, e_TRBRA,          20,         1,         e_ERR,         0,  &longTime },
  {L_,  e_TRBR,          80,         1,        e_INVALID,      0,  &longTime },
  {L_, e_TRBRA,          40,         1,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
     {L_, e_HELP_WRITE,     50,       0,         50,            0,       0   },
     {L_, e_TRBRA,           1,       0,          1,            0,  &longTime},
     {L_, e_TRBRA,          25,       0,         25,            0,  &longTime},
     {L_, e_TRBRA,          36,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,      0,           0,            0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
     {L_, e_HELP_WRITE,     50,       0,         50,            0,       0   },
     {L_,  e_TRBR,           1,       0,          1,            0,  &longTime},
     {L_,  e_TRBR,          25,       0,         25,            0,  &longTime},
     {L_,  e_TRBR,          36,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data

    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,  e_TRBR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll return with
         // the remaining data in the internal buffer.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_,  e_TRBR,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_, e_TRBR,           50,         0,          50,        0,  &longTime },
     {L_,  e_RBR,           70,         0,          50,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_, e_TRBRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_, e_TRBRA,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_, e_TRBRA,          50,         0,          50,        0,  &longTime },
     {L_,   e_RBR,          70,         0,          50,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
      {L_, e_HELP_WRITE,    50,         0,          50,        0,  &longTime },
     {L_,  e_TRBR,           1,         0,           1,        0,  &longTime },
     {L_, e_TRBRA,          10,         0,          10,        0,  &longTime },
     {L_,  e_TRBR,          15,         0,          15,        0,  &longTime },
     {L_, e_TRBRA,          24,         0,          24,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        1,         1,           0,        0,  &longTime },
     {L_, e_TRBRA,         125,         1,         100,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
     {L_,  e_SIGNAL,         1,         1,          0,        0,   &longTime },
     {L_, e_TRBR,          250,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
   {L_,   e_SIGNAL,        1,   e_WRITE_OP,          0,        0,  &longTime },
     {L_, e_TRBRA,         125,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_, e_TRBR,          150,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
     {L_,  e_TRBR,           1,         0,           1,        0,  &longTime },
     {L_, e_TRBRA,          10,         0,          10,        0,  &longTime },
     {L_,  e_TRBR,          15,         0,          15,        0,  &longTime },
     {L_, e_TRBRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_, e_TRBRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
     {L_,  e_TRBR,          25,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
     {L_, e_TRBRA,          25,         1,           0,        0,  &timeout1 },
         //
     {L_,  e_TRBR,          25,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,  e_TRBRA,        125,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,  e_TRBR,         150,         0,         100,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int timedReadvRaw(btls::Iovec              *buffer,
        //                     int                      numBytes,
        //                     const bsls::TimeInterval& time,
        //                     int                      flags = 0);
        //   int timedReadvRaw(int                     *augStatus,
        //                     btls::Iovec              *buffer,
        //                     int                      numBytes,
        //                     const bsls::TimeInterval& time,
        //                     int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadvRaw' method");
            QT("==============================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,      0     },
    {L_,  e_TRVR,           1,         0,           1,         0,  &longTime },
    {L_,  e_TRVR,           7,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,  e_TRVR,           6,         0,         e_CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
  {L_,  e_TRVR,           7,         0,        e_INVALID,      0,  &longTime },
  {L_, e_TRVRA,           6,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_, e_TRVRA,           1,         0,           1,         0,  &longTime },
    {L_, e_TRVRA,           7,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_, e_TRVRA,           6,         0,         e_CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
  {L_,  e_TRVR,           7,         0,        e_INVALID,      0,  &longTime },
  {L_,  e_TRVA,           6,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,  e_TRVR,           1,         0,           1,         0,  &longTime },
    {L_,  e_TRVR,           7,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
  {L_, e_TRVRA,           6,         0,         e_ERR,         0,  &longTime },
  {L_,  e_TRVR,           4,         0,        e_INVALID,      0,  &longTime },
  {L_, e_TRVRA,           6,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_, e_TRVRA,           1,         0,           1,         0,  &longTime },
    {L_, e_TRVRA,           7,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
  {L_, e_TRVRA,           6,         1,         e_ERR,         0,  &longTime },
  {L_,  e_TRVR,           4,         1,        e_INVALID,      0,  &longTime },
  {L_, e_TRVRA,           6,         1,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
     {L_, e_HELP_WRITE,     50,       0,         50,            0,       0   },
     {L_, e_TRVRA,           1,       0,          1,            0,  &longTime},
     {L_, e_TRVRA,           5,       0,         25,            0,  &longTime},
     {L_, e_TRVRA,           6,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,      0,           0,            0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
     {L_, e_HELP_WRITE,     50,       0,         50,            0,       0   },
     {L_,  e_TRVR,           1,       0,          1,            0,  &longTime},
     {L_,  e_TRVR,           5,       0,         25,            0,  &longTime},
     {L_,  e_TRVR,           6,       0,         24,            0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data
    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,  e_TRVR,           2,         0,           4,        0,  &longTime },
     {L_,  e_TRVR,           3,         0,           9,        0,  &longTime },
     {L_,  e_TRVR,           6,         0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll return with
         // the remaining data in the internal buffer.
     {L_,  e_TRVR,          4,         0,            2,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_, e_TRVRA,           2,         0,           4,        0,  &longTime },
     {L_, e_TRVRA,           3,         0,           9,        0,  &longTime },
     {L_, e_TRVRA,           6,         0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_, e_TRVRA,           4,         0,           2,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
      {L_, e_HELP_WRITE,    50,         0,          50,        0,  &longTime },
      {L_,  e_TRVR,          1,         0,           1,        0,  &longTime },
      {L_, e_TRVRA,          2,         0,           4,        0,  &longTime },
      {L_,  e_TRVR,          4,         0,          16,        0,  &longTime },
      {L_, e_TRVRA,          5,         0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
      {L_,  e_TRVR,          3,         0,           4,        0,  &longTime },
      {L_, e_HELP_WRITE,    10,         0,          10,        0,  &longTime },
      {L_, e_TRVRA,          4,         1,          10,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
     {L_,  e_SIGNAL,         1,         1,          0,        0,   &longTime },
     {L_, e_TRVR,            8,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
   {L_,   e_SIGNAL,        1,   e_WRITE_OP,          0,        0,  &longTime },
     {L_, e_TRVRA,           8,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_, e_TRVR,            8,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
     {L_,  e_TRVR,           1,         0,           1,        0,  &longTime },
     {L_, e_TRVRA,           2,         0,           4,        0,  &longTime },
     {L_,  e_TRVR,           4,         0,          16,        0,  &longTime },
     {L_, e_TRVRA,           5,         0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_, e_TRVRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
     {L_,  e_TRVR,           5,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
     {L_, e_TRVRA,           6,         1,           0,        0,  &timeout1 },
         //
     {L_,  e_TRVR,           7,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,  e_TRVRA,          8,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,  e_TRVR,           8,         0,         100,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[k_VECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[k_VECBUF_LEN5] = "\0", buf3[k_VECBUF_LEN7] = "\0",
                     buf4[k_VECBUF_LEN9] = "\0", buf5[k_VECBUF_LEN10] = "\0",
                     buf6[k_VECBUF_LEN50] = "\0", buf7[k_VECBUF_LEN90] = "\0";

                btls::Iovec vecBuffer[k_MAX_VECBUF];
                vecBuffer[0].setBuffer(buf0, sizeof buf0);
                vecBuffer[1].setBuffer(buf1, sizeof buf1);
                vecBuffer[2].setBuffer(buf2, sizeof buf2);
                vecBuffer[3].setBuffer(buf3, sizeof buf3);
                vecBuffer[4].setBuffer(buf4, sizeof buf4);
                vecBuffer[5].setBuffer(buf5, sizeof buf5);
                vecBuffer[6].setBuffer(buf6, sizeof buf6);
                vecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                 const bsls::TimeInterval&  time,
        //                 int                       flags = 0);
        //   int timedBufferedRead(int              *augStatus,
        //                 char                    **buffer,
        //                 int                       numBytes,
        //                 const bsls::TimeInterval&  time,
        //                 int                       flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedBufferedRead' method");
            QT("==================================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,         0,          50,        -9,   0 },
    {L_,   e_TRB,           1,         0,           1,        -9,  &longTime },
    {L_,   e_TRB,          49,         0,          49,        -9,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,        -9,  &longTime },
  {L_,   e_TRB,         100,         0,         e_CLOSED,     -9,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
  {L_,   e_TRB,         100,         0,        e_INVALID,     -9,  &longTime },
  {L_,  e_TRBA,          60,         0,        e_INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,        -9,  &longTime },
    {L_,  e_TRBA,          10,         0,          10,        -9,  &longTime },
    {L_,  e_TRBA,          40,         0,          40,        -9,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,        -9,  &longTime },
  {L_,  e_TRBA,         100,         0,         e_CLOSED,     -9,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
  {L_,   e_TRB,         100,         0,        e_INVALID,     -9,  &longTime },
  {L_,  e_TRBA,          60,         0,        e_INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,        -9,  &longTime },
    {L_,   e_TRB,           1,         0,           1,        -9,  &longTime },
    {L_,   e_TRB,          49,         0,          49,        -9,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,        -9,  &longTime },
  {L_,  e_TRBA,          20,         0,         e_ERR,        -9,  &longTime },
  {L_,   e_TRB,          80,         0,        e_INVALID,     -9,  &longTime },
  {L_,  e_TRBA,          40,         0,        e_INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,        -9,  &longTime },
    {L_,  e_TRBA,           1,         0,           1,        -9,  &longTime },
    {L_,  e_TRBA,          49,         0,          49,        -9,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,        -9,  &longTime },
  {L_,  e_TRBA,          20,         1,         e_ERR,        -9,  &longTime },
  {L_,   e_TRB,          80,         1,        e_INVALID,     -9,  &longTime },
  {L_,  e_TRBA,          40,         1,        e_INVALID,     -9,  &longTime },
      {L_,    -1,           0,         0,           0,        -9,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,         50,        -9,       0   },
      {L_,  e_TRB,            1,       0,          1,        -9,  &longTime},
      {L_,  e_TRB,           25,       0,         25,        -9,  &longTime},
      {L_,  e_TRB,           36,       0,         24,        -9,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,     -9,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,         50,        -9,       0   },
      {L_,   e_TRB,           1,       0,          1,        -9,  &longTime},
      {L_,   e_TRB,          25,       0,         25,        -9,  &longTime},
      {L_,   e_TRB,          36,       0,         24,        -9,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,     -9,       0   },
    },

    #else             // unix data

    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,       -9,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,       -9,  &longTime },
     {L_,    e_RB,          80,         1,          50,       -9,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,   e_TRB,          40,         0,          40,       -9,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,       -9,  &longTime },
     {L_,   e_TRB,          15,         0,          15,       -9,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,   e_TRB,          50,         0,          50,       -9,  &longTime },
     {L_,   e_RBR,          50,         0,          45,       -9,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,       -9,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,       -9,  &longTime },
     {L_,    e_RB,          80,         1,          50,       -9,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,  e_TRBA,          40,         0,          40,       -9,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,       -9,  &longTime },
     {L_,  e_TRBA,          15,         0,          15,       -9,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,  e_TRBA,          50,         0,          50,       -9,  &longTime },
     {L_,   e_RBR,          50,         0,          45,       -9,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,       -9,  &longTime },
     {L_,   e_TRB,           1,         0,           1,       -9,  &longTime },
     {L_,  e_TRBA,          10,         0,          10,       -9,  &longTime },
     {L_,   e_TRB,          15,         0,          15,       -9,  &longTime },
     {L_,  e_TRBA,          20,         0,          20,       -9,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_,  e_SIGNAL,         1,         0,           0,       -9,  &longTime },
   {L_,  e_TRBA,           5,         1,           4, e_INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
     {L_, e_HELP_WRITE,      1,         1,          1,        -9,  &longTime },
     {L_,  e_SIGNAL,         1,         0,          0,        -9,  &longTime },
   {L_,   e_TRB,           9,         1,          5, e_INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
     {L_, e_HELP_WRITE,     20,         0,          20,       -9,  &longTime },
   {L_,   e_SIGNAL,        1,   e_WRITE_OP,          0,       -9,  &longTime },
     {L_,   e_TRBA,         25,         0,          25,       -9,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,       -9,  &longTime },
     {L_,   e_TRB,         150,         0,         150,       -9,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,       -9,  &longTime },
     {L_,   e_TRB,           1,         0,           1,       -9,  &longTime },
     {L_,  e_TRBA,          10,         0,          10,       -9,  &longTime },
     {L_,   e_TRB,          15,         0,          15,       -9,  &longTime },
     {L_,  e_TRBA,          20,         0,          20,       -9,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_,  e_TRBA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
     {L_, e_HELP_WRITE,      1,         1,           1,       -9,  &longTime },
     {L_,   e_TRB,          25,         0,           5,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
     {L_, e_HELP_WRITE,     15,         1,          15,       -9,  &longTime },
     {L_,  e_TRBA,          25,         1,          20,        0,  &timeout1 },
         //
     {L_, e_HELP_WRITE,      3,         1,           3,       -9,  &longTime },
     {L_,   e_TRB,          25,         1,          23,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
     {L_, e_HELP_WRITE,     20,         0,          20,       -9,  &longTime },
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,       -9,  &longTime },
     {L_,   e_TRBA,        225,         0,         143,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
     {L_, e_HELP_WRITE,     30,         0,          30,       -9,  &longTime },
     {L_,   e_SIGNAL,        1,         0,           0,       -9,  &longTime },
     {L_,   e_TRB,         450,         0,         173,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 14: {
#if !defined(BSLS_PLATFORM_CMP_GNU) || (BSLS_PLATFORM_CMP_VER_MAJOR < 40000)
        // -------------------------------------------------------------------
        // TESTING 'timedReadv' METHOD:
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int timedReadv(btls::Iovec              *buffer,
        //                  int                      numBytes,
        //                  const bsls::TimeInterval& time,
        //                  int                      flags = 0);
        //   int timedReadv(int                     *augStatus,
        //                  btls::Iovec              *buffer,
        //                  int                      numBytes,
        //                  const bsls::TimeInterval& time,
        //                  int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadv' method");
            QT("===========================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,          50,         0,   0 },
      {L_,   e_TRV,          1,       0,           1,         0,  &longTime },
      {L_,   e_TRV,          5,       0,          25,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,   0,       0,           0,         0,  &longTime },
    {L_,   e_TRV,           6,       0,         e_CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
    {L_,    e_TRV,          7,       0,        e_INVALID,      0,  &longTime },
    {L_,   e_TRVA,          6,       0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,  e_TRVA,           1,       0,           1,         0,  &longTime },
      {L_,  e_TRVA,           5,       0,          25,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_, e_CLOSE_CONTROL,   0,       0,           0,         0,  &longTime },
    {L_,  e_TRVA,           6,       0,         e_CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
    {L_,   e_TRV,           7,       0,        e_INVALID,      0,  &longTime },
    {L_,  e_TRVA,           6,       0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,   e_TRV,           1,       0,           1,         0,  &longTime },
      {L_,   e_TRV,           5,       0,          25,         0,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
      {L_, e_CLOSE_OBSERVE,   0,       0,           0,         0,  &longTime },
      {L_,  e_TRVA,           6,       0,         e_ERR,       0,  &longTime },
    {L_,   e_TRV,           4,       0,        e_INVALID,      0,  &longTime },
    {L_,  e_TRVA,           5,       0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,          50,         0,  &longTime },
      {L_,  e_TRVA,           1,       0,           1,         0,  &longTime },
      {L_,  e_TRVA,           5,       0,          25,         0,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
      {L_, e_CLOSE_OBSERVE,   0,       0,           0,         0,  &longTime },
    {L_,  e_TRVA,           6,       1,         e_ERR,         0,  &longTime },
    {L_,   e_TRV,           4,       1,        e_INVALID,      0,  &longTime },
    {L_,  e_TRVA,           5,       1,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,       0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,         50,         0,       0   },
      {L_,  e_TRVA,           1,       0,          1,         0,  &longTime},
      {L_,  e_TRVA,           5,       0,         25,         0,  &longTime},
      {L_,  e_TRVA,           6,       0,         24,         0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,       0,         50,         0,       0   },
      {L_,   e_TRV,           1,       0,          1,         0,  &longTime},
      {L_,   e_TRV,           5,       0,         25,         0,  &longTime},
      {L_,   e_TRV,           6,       0,         24,         0,  &timeout1},
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
      {L_,    -1,           0,         0,           0,      0,       0   },
    },

    #else             // unix data
    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, e_HELP_WRITE,     50,      0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
       {L_,   e_SIGNAL,        1,      0,           0,        0,  &longTime },
       {L_,    e_RB,          80,      1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
       {L_,   e_TRV,           2,      0,           4,        0,  &longTime },
       {L_,   e_TRV,           3,      0,           9,        0,  &longTime },
       {L_,   e_TRV,           6,      0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
       {L_,   e_SIGNAL,        0,      1,           0,        0,  &longTime },
       {L_,   e_TRV,           4,      0,          16,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
       {L_,   e_TRV,           6,      0,          35,        0,  &longTime },
       {L_,   e_RBR,         150,      0,          51,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
       {L_, e_HELP_WRITE,     50,      0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
       {L_,   e_SIGNAL,        1,      0,           0,        0,  &longTime },
       {L_,    e_RB,          80,      1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
       {L_,   e_TRV,           2,      0,           4,        0,  &longTime },
       {L_,   e_TRV,           3,      0,           9,        0,  &longTime },
       {L_,   e_TRV,           6,      0,          35,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
       {L_,   e_SIGNAL,        0,      1,           0,        0,  &longTime },
       {L_,  e_TRVA,           4,      0,          16,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
       {L_,  e_TRVA,           6,      0,          35,        0,  &longTime },
       {L_,   e_RBR,         150,      0,          51,        0,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
       {L_, e_HELP_WRITE,     50,      0,          50,        0,  &longTime },
       {L_,   e_TRV,           1,      0,           1,        0,  &longTime },
       {L_,  e_TRVA,           2,      0,           4,        0,  &longTime },
       {L_,   e_TRV,           3,      0,           9,        0,  &longTime },
       {L_,  e_TRVA,           6,      0,          35,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
       {L_,  e_SIGNAL,         1,      0,           0,        0,  &longTime },
      {L_,   e_TRVA,          3,      1,           1, e_INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
       {L_, e_HELP_WRITE,     20,      0,         20,        0,   &longTime },
       {L_,  e_SIGNAL,         1,      0,          0,        0,   &longTime },
      {L_,   e_TRV,           6,      1,         20, e_INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
       {L_, e_HELP_WRITE,     20,      0,          20,        0,  &longTime },
      {L_,   e_SIGNAL,        1,   e_WRITE_OP,       0,        0,  &longTime },
       {L_,   e_TRVA,          6,      0,          35,        0,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
      {L_,   e_SIGNAL,        1,  e_WRITE_OP,        0,        0,  &longTime },
       {L_,   e_TRV,           7,      0,          85,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
       {L_, e_HELP_WRITE,     50,      0,          50,        0,  &longTime },
       {L_,   e_TRV,           1,      0,           1,        0,  &longTime },
       {L_,  e_TRVA,           2,      0,           4,        0,  &longTime },
       {L_,   e_TRV,           4,      0,          16,        0,  &longTime },
       {L_,  e_TRVA,           5,      0,          25,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
       {L_,  e_TRVA,           5,      0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
       {L_, e_HELP_WRITE,      1,      1,           1,        0,  &longTime },
       {L_,   e_TRV,           4,      0,           1,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
       {L_, e_HELP_WRITE,     15,      1,          15,        0,  &longTime },
       {L_,  e_TRVA,           5,      1,          15,        0,  &timeout1 },
         //
       {L_, e_HELP_WRITE,      3,      1,           3,        0,  &longTime },
       {L_,   e_TRV,           6,      1,           3,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
       {L_, e_HELP_WRITE,     20,      0,          20,        0,  &longTime },
      {L_,   e_SIGNAL,        1,  e_WRITE_OP,        0,        0,  &longTime },
       {L_,   e_TRVA,          8,      0,         120,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
      {L_,   e_SIGNAL,        1,  e_WRITE_OP,        0,        0,  &longTime },
       {L_,    e_TRV,          8,      0,         100,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.

                char buf0[k_VECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[k_VECBUF_LEN5] = "\0", buf3[k_VECBUF_LEN7] = "\0",
                     buf4[k_VECBUF_LEN9] = "\0", buf5[k_VECBUF_LEN10] = "\0",
                     buf6[k_VECBUF_LEN50] = "\0", buf7[k_VECBUF_LEN90] = "\0";

                btls::Iovec vecBuffer[k_MAX_VECBUF];
                vecBuffer[0].setBuffer(buf0, sizeof buf0);
                vecBuffer[1].setBuffer(buf1, sizeof buf1);
                vecBuffer[2].setBuffer(buf2, sizeof buf2);
                vecBuffer[3].setBuffer(buf3, sizeof buf3);
                vecBuffer[4].setBuffer(buf4, sizeof buf4);
                vecBuffer[5].setBuffer(buf5, sizeof buf5);
                vecBuffer[6].setBuffer(buf6, sizeof buf6);
                vecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                    const bsls::TimeInterval& time,
        //                    int                      flags = 0);
        //   int timedReadRaw(int                     *augStatus,
        //                    char                    *buffer,
        //                    int                      numBytes,
        //                    const bsls::TimeInterval& time,
        //                    int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedReadRaw' method");
            QT("=============================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,      0     },
    {L_,   e_TRR,           1,         0,           1,         0,  &longTime },
    {L_,   e_TRR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,   e_TRR,         100,         0,         e_CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
  {L_,   e_TRR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_,  e_TRRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,  e_TRRA,          10,         0,          10,         0,  &longTime },
    {L_,  e_TRRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,  e_TRRA,         100,         0,         e_CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
  {L_,   e_TRR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_,   e_TRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)
    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,   e_TRR,           1,         0,           1,         0,  &longTime },
    {L_,   e_TRR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
    {L_,  e_TRRA,          20,         0,         e_ERR,       0,  &longTime },
  {L_,   e_TRR,          80,         0,        e_INVALID,      0,  &longTime },
  {L_,  e_TRRA,          40,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,  e_TRRA,           1,         0,           1,         0,  &longTime },
    {L_,  e_TRRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
    {L_,  e_TRRA,          20,         1,         e_ERR,       0,  &longTime },
   {L_,   e_TR,          80,         1,        e_INVALID,      0,  &longTime },
  {L_,  e_TRRA,          40,         1,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,   e_TRRA,          1,         0,           1,         0,  &longTime },
    {L_,   e_TRRA,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,    e_TRR,          1,         0,           1,         0,  &longTime },
    {L_,    e_TRR,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #else                  // unix
    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,   e_TRR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, but it'll return with
         // the remaining data in the internal buffer.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_,   e_TRR,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,  e_TRR,           50,         0,          50,        0,  &longTime },
     {L_,  e_RBR,           70,         0,          50,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,  e_TRRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_,  e_TRRA,          15,         0,          10,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,  e_TRRA,          50,         0,          50,        0,  &longTime },
     {L_,   e_RBR,          70,         0,          50,        0,  &longTime },
    },

    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
      {L_, e_HELP_WRITE,    50,         0,          50,        0,  &longTime },
     {L_,   e_TRR,           1,         0,           1,        0,  &longTime },
     {L_,  e_TRRA,          10,         0,          10,        0,  &longTime },
     {L_,   e_TRR,          15,         0,          15,        0,  &longTime },
     {L_,  e_TRRA,          24,         0,          24,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
      {L_,  e_SIGNAL,        1,         1,           0,        0,  &longTime },
     {L_,  e_TRRA,         125,         1,         100,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
     {L_,  e_SIGNAL,         1,         1,          0,        0,   &longTime },
     {L_,  e_TRR,          250,         1,        100,        0,   &longTime },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
   {L_,   e_SIGNAL,        1,   e_WRITE_OP,          0,        0,  &longTime },
     {L_,  e_TRRA,         125,         0,         100,        0,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,  e_TRR,          150,         0,         100,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
     {L_,   e_TRR,           1,         0,           1,        0,  &longTime },
     {L_,  e_TRRA,          10,         0,          10,        0,  &longTime },
     {L_,   e_TRR,          15,         0,          15,        0,  &longTime },
     {L_,  e_TRRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_,  e_TRRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
     {L_,   e_TRR,          25,         0,           0,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
     {L_,  e_TRRA,          25,         1,           0,        0,  &timeout1 },
         //
     {L_,   e_TRR,          25,         1,           0,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,   e_TRRA,        125,         0,         100,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,   e_TRR,         150,         0,         100,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //                 const bsls::TimeInterval& time,
        //                 int                      flags = 0);
        //   int timedRead(int                     *augStatus,
        //                 char                    *buffer,
        //                 int                      numBytes,
        //                 const bsls::TimeInterval& time,
        //                 int                      flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'timedRead' method");
            QT("==========================");
        }

        {

            bsls::TimeInterval timeout1(0, 2 * k_SLEEP_TIME),
                              timeout2(1, 0),
                              longTime(2, 0);
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
   //line   command    numToUse  interruptFlags  expRet  expAugStat  timeout
   //----   -------    --------  --------------  ------  ----------  -------
//==========>
  {
    // Command set 1: Close the channel from the peer side to test the behavior
    // of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
      {L_, e_HELP_WRITE,     50,         0,          50,         0,   0 },
    {L_,    e_TR,           1,         0,           1,         0,  &longTime },
    {L_,    e_TR,          49,         0,          49,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,    e_TR,         100,         0,         e_CLOSED,      0,  &longTime },
        // The channel now is invalid due to the operation failure, and so the
        // subsequent read operations will not succeed any more.
  {L_,    e_TR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_,   e_TRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 2: Close the channel from the peer side to test the behavior
    // of the 'read' method w the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,   e_TRA,          10,         0,          10,         0,  &longTime },
    {L_,   e_TRA,          40,         0,          40,         0,  &longTime },
        // The channel will be closed by the peer when the 'read' request is on
        // going, so it'll return 'CLOSED'.
    {L_, e_CLOSE_CONTROL,   0,         0,           0,         0,  &longTime },
  {L_,   e_TRA,         100,         0,         e_CLOSED,      0,  &longTime },
         // The channel now is invalid due to the operation failure, and so the
         // subsequent read operations will not succeed any more.
  {L_,    e_TR,         100,         0,        e_INVALID,      0,  &longTime },
  {L_,   e_TRA,          60,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
        defined (BSLS_PLATFORM_OS_WINDOWS)

    // Command set 3: Close the channel at the channel side to test the
    // behavior of the 'read' method with the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,    e_TR,           1,         0,           1,         0,  &longTime },
    {L_,    e_TR,          49,         0,          49,         0,  &longTime },
        // Now close the channel, and try some 'read' operations, each of which
        // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
    {L_,   e_TRA,          20,         0,         e_ERR,       0,  &longTime },
  {L_,    e_TR,          80,         0,        e_INVALID,      0,  &longTime },
  {L_,   e_TRA,          40,         0,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    // Command set 4: Close the channel at the channel side to test the
    // behavior of the 'read' method w/o the 'augStatus' parameter.
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,   e_TRA,           1,         0,           1,         0,  &longTime },
    {L_,   e_TRA,          49,         0,          49,         0,  &longTime },
      // Now close the channel, and try some 'read' operations, each of which
      // should return a "ERROR".
    {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0,  &longTime },
    {L_,   e_TRA,          20,         1,         e_ERR,       0,  &longTime },
  {L_,    e_TR,          80,         1,        e_INVALID,      0,  &longTime },
  {L_,   e_TRA,          40,         1,        e_INVALID,      0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #endif

    #ifdef BSLS_PLATFORM_OS_WINDOWS
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,    e_TRA,          1,         0,           1,         0,  &longTime },
    {L_,    e_TRA,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },
    {
      // Establish a channel and verify that it works fine.
    {L_, e_HELP_WRITE,     50,         0,          50,         0,  &longTime },
    {L_,     e_TR,          1,         0,           1,         0,  &longTime },
    {L_,     e_TR,         49,         0,          49,         0,  &longTime },
      {L_,    -1,           0,         0,           0,         0,  &longTime },
    },

    #else

    // Commands set 5: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method w/o the 'augStatus' parameter (concern 4 -
    // 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,    e_TR,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_,    e_TR,          15,         0,          15,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,    e_TR,          50,         0,          50,        0,  &longTime },
     {L_,   e_RBR,          50,         0,          45,        0,  &longTime },
    },

    // Commands set 6: Establish a channel and make the expected number of
    // bytes of data available in the channel's internal buffer, test the
    // behavior of the 'read' method with the 'augStatus' parameter (concern 4
    // - 5).
    {
       // The control socket write 50 bytes to the channel for read.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
         // A 'bufferedRead' for 80 bytes is issued in 'interruptible' mode,
         // while a signal 'SIGSYS' is generated, and so the returned 50 is
         // stored in the internal buffer for later read try.
     {L_,   e_SIGNAL,        1,         0,           0,        0,  &longTime },
     {L_,    e_RB,          80,         1,          50,        0,  &longTime },
         // Now a 'read' for 40 bytes will return right away because there's
         // enough data in the internal buffer at this moment.
     {L_,   e_TRA,          40,         0,          40,        0,  &longTime },
         // The next 'read' can't find all data it needs, and so it'll try
         // reading the remaining data from the channel directly, which is at
         // last available from the control endpoint through a thread.
     {L_,   e_SIGNAL,        0,         1,           0,        0,  &longTime },
     {L_,   e_TRA,          15,         0,          15,        0,  &longTime },
         // There are 95 bytes in the channel's TCP buffer, the following
         // requests can all meet their expectations because the last one is a
         // "raw" operation.
     {L_,   e_TRA,          50,         0,          50,        0,  &longTime },
     {L_,   e_RBR,          50,         0,          45,        0,  &longTime },
    },
    // commands set 7: to resolve concern 6 - 8.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
     {L_,    e_TR,           1,         0,           1,        0,  &longTime },
     {L_,   e_TRA,          10,         0,          10,        0,  &longTime },
     {L_,    e_TR,          15,         0,          15,        0,  &longTime },
     {L_,   e_TRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_,  e_SIGNAL,         1,         0,           0,        0,  &longTime },
   {L_,   e_TRA,           5,         1,           4, e_INTERRUPTED,&longTime},
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the 'read' method w/o the 'augStatus' argument.
     {L_, e_HELP_WRITE,      1,         1,          1,        0,   &longTime },
     {L_,  e_SIGNAL,         1,         0,          0,        0,   &longTime },
   {L_,    e_TR,           5,         1,          1, e_INTERRUPTED, &longTime},
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until all the requested data were read.
         // (Here the signal generating thread is implemented to write 100
         // bytes to the channel feed the (TBD - figure out what goes here).
     {L_, e_HELP_WRITE,     20,         0,          20,        0,  &longTime },
   {L_,   e_SIGNAL,        1,   e_WRITE_OP,          0,        0,  &longTime },
     {L_,    e_TRA,         25,         0,          25,        0,  &longTime },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,    e_TR,         150,         0,         150,        0,  &longTime },
    },

    // commands set 8: to resolve concern 9 - 10.
    {
       // Each request read expected number of bytes from the channel.
     {L_, e_HELP_WRITE,     50,         0,          50,        0,  &longTime },
     {L_,    e_TR,           1,         0,           1,        0,  &longTime },
     {L_,   e_TRA,          10,         0,          10,        0,  &longTime },
     {L_,    e_TR,          15,         0,          15,        0,  &longTime },
     {L_,   e_TRA,          20,         0,          20,        0,  &longTime },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it.
     {L_,   e_TRA,           5,         0,           4,        0,  &timeout1 },
         // There are not enough bytes left in the TCP buffer for next request,
         // now we'll generate signals to interrupt it, the only difference is
         // we call the "read" method w/o the 'augStatus' argument.
     {L_, e_HELP_WRITE,      1,         1,           1,        0,  &longTime },
     {L_,    e_TR,          25,         0,           1,        0,  &timeout1 },
         // The same as the above, but "read" operations are "interruptible".
     {L_, e_HELP_WRITE,     15,         1,          15,        0,  &longTime },
     {L_,   e_TRA,          25,         1,          15,        0,  &timeout1 },
         //
     {L_, e_HELP_WRITE,      3,         1,           3,        0,  &longTime },
     {L_,    e_TR,          25,         1,           3,        0,  &timeout1 },
         // Test if a request is in 'non-interrupt' mode, it won't be
         // interrupted by an "AE" but waiting for the more data to come.  The
         // request will not return until the timeout is reached.  Here the
         // signal generating thread is to write 100 bytes at the peer side to
         // verify the "read" operations wait until timeout is reached (TBD -
         // figure out what goes here).
         //
     {L_, e_HELP_WRITE,     20,         0,          20,        0,  &longTime },
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,    e_TRA,        125,         0,         120,        0,  &timeout2 },
         // The same situation for the 'read' operation, without the
         // 'augStatus' as the parameter.  The behavior should be the same as
         // above.
     {L_, e_HELP_WRITE,     30,         0,          30,        0,  &longTime },
   {L_,   e_SIGNAL,        1,  e_WRITE_OP,           0,        0,  &longTime },
     {L_,    e_TR,         150,         0,         130,        0,  &timeout2 },
    },
    #endif
  };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int writevRaw(btls::Ovec  *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(btls::Iovec *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(int        *augStatus,
        //                 btls::Ovec  *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        //   int writevRaw(int        *augStatus,
        //                 btls::Iovec *buffers,
        //                 int         numBuffers,
        //                 int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'writevRaw' method");
            QT("==========================");
        }

        {
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRO,          1,          0,           1,         0   },
              {L_,     e_W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,         0   },
            {L_,  e_WVRO,           7,         0,         e_CLOSED,      0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRO,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRI,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRI,          1,          0,           1,         0   },
              {L_,     e_W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,         0   },
            {L_,  e_WVRI,           7,         0,         e_CLOSED,      0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRI,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRO,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            // Command set 3: Close the channel from the peer side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVROA,          1,          0,           1,         0   },
              {L_,     e_W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,         0   },
            {L_,  e_WVRO,           7,         0,         e_CLOSED,      0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVROA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVRIA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 4: Close the channel from the peer side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVRIA,          1,          0,           1,         0   },
              {L_,     e_W, SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,         0   },
            {L_, e_WVRIA,           7,         0,         e_CLOSED,      0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVRIA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVROA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRO,           1,         0,           1,         0   },
              {L_,  e_WVRO,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRI,           1,         0,           1,         0   },
              {L_,  e_WVRI,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVROA,           1,         0,           1,         0   },
              {L_, e_WVROA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVRIA,           1,         0,           1,         0   },
              {L_, e_WVRIA,           7,         0,       17792,         0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            #else                                       // unix test data
            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // Command set 5: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRO,          1,          0,           1,         0   },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  e_WVRO,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRO,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRI,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 6: Close the channel at the channel side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRI,           1,         0,           1,         0   },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  e_WVRI,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRI,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRO,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 7: Close the channel at the channel side to test the
            // behavior of the write method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVROA,          1,          0,           1,         0   },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, e_WVROA,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVROA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVRIA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 8: Close the channel at the channel side to test the
            // behavior of the write method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVRIA,           1,         0,           1,         0   },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, e_WVRIA,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVRIA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVROA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  e_WVRO,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_,  e_WVRO,           8,         1,       40960,         0   },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  e_WVRI,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_,  e_WVRI,           8,         1,       40960,         0   },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, e_WVROA,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_, e_WVROA,           8,         1,       40960,         0   },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, e_WVRIA,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_, e_WVRIA,           8,         1,       40960,         0   },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  e_WVRO,           7,          1,        8192,        0   },
            {L_,  e_SIGNAL,         2,    e_READ_OP,           0,        0   },
              {L_,   e_WVO,           7,          0,       24740,        0   },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVRI,          7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,          0   },
             {L_,   e_WVI,          7,           0,      24740,          0   },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVROA,         7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,          0   },
             {L_,  e_WVOA,          7,           0,      24740,          0   },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVRI,          7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,          0   },
             {L_,  e_WVIA,          7,           0,      24740,          0   },
            },
            #elif BSLS_PLATFORM_OS_LINUX
            // Command set 5: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRO,          1,          0,           1,         0   },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  e_WVRO,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRO,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRI,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 6: Close the channel at the channel side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVRI,           1,         0,           1,         0   },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,  0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_,  e_WVRI,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_,  e_WVRI,           2,         0,        e_INVALID,      0   },
            {L_,  e_WVRO,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 7: Close the channel at the channel side to test the
            // behavior of the write method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVROA,          1,          0,           1,         0   },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,        0   },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, e_WVROA,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVROA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVRIA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // Command set 8: Close the channel at the channel side to test the
            // behavior of the write method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_WVRIA,           1,         0,           1,         0   },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,   SYS_DEPENDENT_LEN, 0   },
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,         0   },
              {L_, e_WVRIA,           7,         0,         e_ERR,       0   },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
            {L_, e_WVRIA,           2,         0,        e_INVALID,      0   },
            {L_, e_WVROA,           6,         0,        e_INVALID,      0   },
              {L_,    -1,           0,         0,           0,         0   },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    e_WR, SYS_DEPENDENT_LEN2,  0,  SYS_DEPENDENT_LEN2, 0   },
              {L_,  e_WVRO,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          8,   e_READ_OP,           0,         0   },
              {L_,  e_WVRO,           8,         1,       40960,         0   },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_,  e_WVRI,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          8,   e_READ_OP,           0,         0   },
              {L_,  e_WVRI,           8,         1,       40960,         0   },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, e_WVROA,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          8,   e_READ_OP,           0,         0   },
              {L_, e_WVROA,           8,         1,       40960,         0   },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,      0   },
              {L_, e_WVRIA,           7,         1,        8192,         0   },
            {L_, e_SIGNAL,          8,   e_READ_OP,           0,         0   },
              {L_, e_WVRIA,           8,         1,       40960,         0   },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  e_WVRO,           7,          1,        8192,        0   },
            {L_,  e_SIGNAL,         8,    e_READ_OP,           0,        0   },
              {L_,   e_WVO,           7,          0,       24740,        0   },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVRI,          7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        8,       e_READ_OP,        0,          0   },
             {L_,   e_WVI,          7,           0,      24740,          0   },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVROA,         7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        8,       e_READ_OP,        0,          0   },
             {L_,  e_WVOA,          7,           0,      24740,          0   },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,      0   },
             {L_,  e_WVRI,          7,           1,        8192,         0   },
           {L_,  e_SIGNAL,        8,       e_READ_OP,        0,          0   },
             {L_,  e_WVIA,          7,           0,      24740,          0   },
            },
            #else
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,  e_WVRO,           1,         0,           1,         0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   e_WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_,  e_WVRO,           6,         1,        1024,         0   },
            },
            {
              {L_,  e_WVRI,           1,         0,           1,         0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   e_WR,    BUF_WRITE,         0,      BUF_WRITE,      0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_,  e_WVRI,           6,         1,        1024,         0   },
            },
            {
              {L_, e_WVROA,           1,         0,           1,         0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   e_WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_, e_WVROA,           6,         1,        1024,         0   },
            },
            {
              {L_, e_WVRIA,           1,         0,           1,         0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN,  1,        1024,         0   },
              {L_,   e_WR,    BUF_WRITE,         0,     BUF_WRITE,       0   },
            {L_, e_SIGNAL,          2,   e_READ_OP,           0,         0   },
              {L_, e_WVRIA,           6,         1,        1024,         0   },
            },
            // commands set 13: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0   },
              {L_,  e_WVRO,           7,          1,        8192,        0   },
            {L_,  e_SIGNAL,         2,    e_READ_OP,           0,        0   },
              {L_,   e_WVO,           7,          0,       12302,        0   },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, e_WVRO,           1,         0,           1,          0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   e_WR,    BUF_WRITE,        0,        1024,          0   },
            {L_,  e_SIGNAL,        2,      e_READ_OP,        0,          0   },
              {L_, e_WVRO,           6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, e_WVRI,           1,         0,           1,          0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   e_WR,    BUF_WRITE,        0,        1024,          0   },
            {L_,  e_SIGNAL,        2,      e_READ_OP,        0,          0   },
              {L_, e_WVRI,           6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, e_WVROA,          1,         0,           1,          0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   e_WR,    BUF_WRITE,        0,        1024,          0   },
            {L_,  e_SIGNAL,        2,      e_READ_OP,        0,          0   },
              {L_, e_WVROA,          6,         0,        1024,          0   },
            },
            // commands set 5: to resolve concern 4 - 6.
            {
              {L_, e_WVRIA,          1,         0,           1,          0   },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        8192,          0   },
              {L_,   e_WR,    BUF_WRITE,        0,        1024,          0   },
            {L_,  e_SIGNAL,        2,      e_READ_OP,        0,          0   },
              {L_, e_WVRIA,          6,         0,        1024,          0   },
            },
            #endif

            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[k_WVECBUF_LEN1],
                     buf1[k_VECBUF_LEN3] = "\0",
                     buf2[k_WVECBUF_LEN20] = "\0",
                     buf3[k_WVECBUF_LEN60] = "\0",

                     #ifdef BSLS_PLATFORM_OS_SOLARIS
                     buf4[k_WVECBUF_LEN80] = "\0",
                     buf5[k_WVECBUF_LEN8K] = "\0",
                     #else
                     buf4[k_WVECBUF_LEN500] = "\0",
                     buf5[k_WVECBUF_LEN1K] = "\0",
                     #endif
                     buf6[k_WVECBUF_LEN16K] = "\0",
                     buf7[k_WVECBUF_LEN32K] = "\0";

                btls::Ovec ovecBuffer[k_MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btls::Iovec iovecBuffer[k_MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int writev(btls::Ovec  *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(btls::Iovec *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(int        *augStatus,
        //              btls::Ovec  *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        //   int writev(int        *augStatus,
        //              btls::Iovec *buffers,
        //              int         numBuffers,
        //              int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'writev' method");
            QT("=========================");
        }

        {
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVO,          1,          0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
            {L_,   e_WVO,           7,         0,         e_CLOSED,     0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,   e_WVO,           2,         0,        e_INVALID,     0, 0 },
            {L_,   e_WVI,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
            {L_,   e_WVI,           7,         0,         e_CLOSED,     0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,   e_WVI,           2,         0,        e_INVALID,     0, 0 },
            {L_,   e_WVO,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 3: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVOA,          1,          0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
            {L_,  e_WVOA,           7,         0,         e_CLOSED,     0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,  e_WVOA,           2,         0,        e_INVALID,     0, 0 },
            {L_,  e_WVIA,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 4: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
            {L_,  e_WVIA,           7,         0,         e_CLOSED,     0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,  e_WVIA,           2,         0,        e_INVALID,     0, 0 },
            {L_,  e_WVOA,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 5: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVO,          1,          0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,        0, 0 },
              {L_,   e_WVO,           7,         0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,   e_WVO,           2,         0,        e_INVALID,     0, 0 },
            {L_,   e_WVI,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 6: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,        0, 0 },
              {L_,   e_WVI,           7,         0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,   e_WVI,           2,         0,        e_INVALID,     0, 0 },
            {L_,   e_WVO,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 7: Close the channel at the channel side to test the
            // behavior of the 'read' method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVOA,          1,          0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,        0, 0 },
              {L_,  e_WVOA,           7,         0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,  e_WVOA,           2,         0,        e_INVALID,     0, 0 },
            {L_,  e_WVIA,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 8: Close the channel at the channel side to test the
            // behavior of the 'read' method w/ the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_OBSERVE,   0,         0,           0,        0, 0 },
              {L_,  e_WVIA,           7,         0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
            {L_,  e_WVIA,           2,         0,        e_INVALID,     0, 0 },
            {L_,  e_WVOA,           6,         0,        e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVO,           1,         0,           1,        0, 0 },
              {L_,   e_WVO,           7,         0,       17792,        0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,   e_WVI,           7,         0,       17792,        0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVOA,           1,         0,           1,        0, 0 },
              {L_,  e_WVOA,           7,         0,       17792,        0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,  e_WVIA,           7,         0,       17792,        0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            #else                                       // unix test data

            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // commands set 9: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,   e_WVO,           1,         0,           1,        0, 0 },
              {L_,   e_WVO,           5,         0,         164,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,   e_WVO,           7,         1,     8192, e_INTERRUPTED, 0 },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,   e_WVI,           5,         0,         164,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,   e_WVI,           7,         1,     8192, e_INTERRUPTED, 0 },
            },

            // commands set 11: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,  e_WVOA,           1,         0,           1,        0, 0 },
              {L_,  e_WVOA,           5,         0,         164,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,  e_WVOA,           7,         1,     8192, e_INTERRUPTED, 0 },
            },

            // commands set 12: to resolve concern 4 - 6.
            {
                // Each request read expected number of bytes from the channel.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,  e_WVIA,           5,         0,         164,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,  e_WVIA,           7,         1,     8192, e_INTERRUPTED, 0 },
            },

            // commands set 13: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0, 0 },
           {L_,  e_SIGNAL,         2,       e_READ_OP,       0,         0, 0 },
             {L_,   e_WVO,           7,          0,      24740,         0, 0 },
            },

            // commands set 14: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0, 0 },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,         0, 0 },
             {L_,   e_WVI,          7,           0,      24740,         0, 0 },
            },

            // commands set 15: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0, 0 },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,         0, 0 },
             {L_,  e_WVOA,          7,           0,      24740,         0, 0 },
            },

            // commands set 16: to resolve concern 4 - 6.
            {
             {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,     0, 0 },
           {L_,  e_SIGNAL,        2,       e_READ_OP,        0,         0, 0 },
             {L_,  e_WVIA,          7,           0,      24740,         0, 0 },
            },
            #elif BSLS_PLATFORM_OS_LINUX

            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   e_WVO,           1,         0,           1,        0, 0 },
              {L_,   e_WVO,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         5,         0,           0,        0, 0 },
              {L_,   e_WVO,           7,         1,   1024, e_INTERRUPTED, 0 },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,   e_WVI,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         5,         0,           0,        0, 0 },
              {L_,   e_WVI,           7,         1,   1024, e_INTERRUPTED, 0 },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  e_WVOA,           1,         0,           1,        0, 0 },
              {L_,  e_WVOA,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         5,         0,           0,        0, 0 },
              {L_,  e_WVOA,           8,         1,   1024, e_INTERRUPTED, 0 },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,  e_WVIA,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         5,         0,           0,        0, 0 },
              {L_,  e_WVIA,           8,         1,   1024, e_INTERRUPTED, 0 },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   e_WVO,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         5,       e_READ_OP,       0,      0, 0 },
              {L_,   e_WVO,           6,        0,         1608,       0, 0 },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   e_WVI,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,      0, 0 },
              {L_,   e_WVI,           6,        0,         1608,       0, 0 },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  e_WVOA,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,      0, 0 },
              {L_,  e_WVOA,           6,        0,         1608,       0, 0 },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  e_WVIA,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,      0, 0 },
              {L_,  e_WVIA,           6,        0,         1608,       0, 0 },
            },

            #else // ibm test data

            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   e_WVO,           1,         0,           1,        0, 0 },
              {L_,   e_WVO,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,   e_WVO,           7,         1,     1024, e_INTERRUPTED, 0 },
            },
            // commands set 6: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,   e_WVI,           1,         0,           1,        0, 0 },
              {L_,   e_WVI,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,   e_WVI,           7,         1,     1024, e_INTERRUPTED, 0 },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  e_WVOA,           1,         0,           1,        0, 0 },
              {L_,  e_WVOA,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,  e_WVOA,           7,         1,     1024, e_INTERRUPTED, 0 },
            },
            // commands set 8: to resolve concern 4 - 6.
            {
                // Each request writes expected bytes from the channel.
              {L_,  e_WVIA,           1,         0,           1,        0, 0 },
              {L_,  e_WVIA,           5,         0,         164,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,        0, 0 },
            {L_,  e_WVIA,           7,         1,     1024, e_INTERRUPTED, 0 },
            },

            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,   e_WVO,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,       0, 0 },
              {L_,   e_WVO,           6,        0,         1608,       0, 0 },
            },
            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,   e_WVI,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,       0, 0 },
              {L_,   e_WVI,           6,        0,         1608,       0, 0 },
            },
            // commands set 11: to resolve concern 4 - 6.
            {
              {L_,  e_WVOA,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,       0, 0 },
              {L_,  e_WVOA,           6,        0,         1608,       0, 0 },
            },
            // commands set 12: to resolve concern 4 - 6.
            {
              {L_,  e_WVIA,           1,        0,            1,       0, 0 },
              {L_,    e_WA, SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
             {L_,  e_SIGNAL,         2,       e_READ_OP,       0,       0, 0 },
              {L_,  e_WVIA,           6,        0,         1608,       0, 0 },
            },

            #endif
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                char buf0[k_WVECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                    buf2[k_WVECBUF_LEN20] = "\0", buf3[k_WVECBUF_LEN60] = "\0",
                     #ifdef BSLS_PLATFORM_OS_SOLARIS
                         buf4[k_WVECBUF_LEN80] = "\0",
                         buf5[k_WVECBUF_LEN8K] = "\0",
                     #else
                         buf4[k_WVECBUF_LEN500] = "\0",
                         buf5[k_WVECBUF_LEN1K] = "\0",
                     #endif
                  buf6[k_WVECBUF_LEN16K] = "\0", buf7[k_WVECBUF_LEN32K] = "\0";

                btls::Ovec ovecBuffer[k_MAX_VECBUF];
                ovecBuffer[0].setBuffer(buf0, sizeof buf0);
                ovecBuffer[1].setBuffer(buf1, sizeof buf1);
                ovecBuffer[2].setBuffer(buf2, sizeof buf2);
                ovecBuffer[3].setBuffer(buf3, sizeof buf3);
                ovecBuffer[4].setBuffer(buf4, sizeof buf4);
                ovecBuffer[5].setBuffer(buf5, sizeof buf5);
                ovecBuffer[6].setBuffer(buf6, sizeof buf6);
                ovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btls::Iovec iovecBuffer[k_MAX_VECBUF];
                iovecBuffer[0].setBuffer(buf0, sizeof buf0);
                iovecBuffer[1].setBuffer(buf1, sizeof buf1);
                iovecBuffer[2].setBuffer(buf2, sizeof buf2);
                iovecBuffer[3].setBuffer(buf3, sizeof buf3);
                iovecBuffer[4].setBuffer(buf4, sizeof buf4);
                iovecBuffer[5].setBuffer(buf5, sizeof buf5);
                iovecBuffer[6].setBuffer(buf6, sizeof buf6);
                iovecBuffer[7].setBuffer(buf7, sizeof buf7);

                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
#if !defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR < 10
        // -------------------------------------------------------------------
        // TESTING 'writeRaw' METHOD:
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    e_WR,          1,          0,           1,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,       0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.  The first
                // write operation is just to help the second write 'hanging'
                // there waiting to write.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
              {L_,    e_WR,   SYS_DEPENDENT_LEN, 0,         e_CLOSED,   0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_WR,         100,         0,        e_INVALID,   0, 0 },
              {L_,   e_WRA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the write operation w/ the 'augStatus' parameter.
            // The first write operation is just to help the second write
            // 'hanging' there waiting to write.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WRA,          1,          0,           1,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,       0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
              {L_,   e_WRA,   SYS_DEPENDENT_LEN, 0,         e_CLOSED,   0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,   e_WRA,         100,         0,        e_INVALID,   0, 0 },
              {L_,    e_WR,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    e_WR,          1,          0,           1,        0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_WR,         100,         0,        e_INVALID,   0, 0 },
              {L_,   e_WRA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },

            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WRA,          1,          0,           1,        0, 0 },
              {L_,   e_WRA,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,   e_WRA,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_WR,         100,         0,        e_INVALID,   0, 0 },
              {L_,   e_WRA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)  // windows test data
            {
              {L_,   e_WRA,           1,         0,               1,    0, 0 },
              {L_,   e_WRA,         100,         0,             100,    0, 0 },
              {L_,    e_WR,          60,         0,              60,    0, 0 },
              {L_,    -1,           0,         0,               0,      0, 0 },
            },
            {
              {L_,    e_WR,           1,         0,               1,    0, 0 },
              {L_,    e_WR,         100,         0,             100,    0, 0 },
              {L_,    e_WR,          60,         0,              60,    0, 0 },
              {L_,    -1,           0,         0,               0,      0, 0 },
            },

            #else                                  // unix test data
            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    e_WR,          1,          0,           1,        0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,   e_WRA,   SYS_DEPENDENT_LEN, 0,         8192,       0, 0 },
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_WR,         100,         0,        e_INVALID,   0, 0 },
              {L_,   e_WRA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },

            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,   e_WRA,          1,          0,           1,        0, 0 },
              {L_,   e_WRA,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,   e_WRA,   SYS_DEPENDENT_LEN, 0,         8192,       0, 0 },
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,   e_WRA,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_WR,         100,         0,        e_INVALID,   0, 0 },
              {L_,   e_WRA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,    e_WR,   SYS_DEPENDENT_LEN,  1,        8192,       0, 0 },
            {L_,  e_SIGNAL,          2,   e_READ_OP,           0,       0, 0 },
              {L_,    e_WR,    BUF_WRITE,         1,       40960,       0, 0 },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,   e_WRA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,   e_WRA,   SYS_DEPENDENT_LEN,  1,        8192,       0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,        0,       0, 0 },
              {L_,   e_WRA,    BUF_WRITE,         1,       40960,       0, 0 },
            },
            // commands set 7: to resolve concern 4 - 6.
            {
              {L_,    e_WR,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,    e_WR,   SYS_DEPENDENT_LEN,  1,        8192,       0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,        0,       0, 0 },
              {L_,    e_WR,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,0, 0 },
            },

            // commands set 8: to resolve concern 4 - 6.
            {
              {L_,   e_WRA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,   e_WRA,   SYS_DEPENDENT_LEN,  1,        8192,       0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,       0,        0, 0 },
              {L_,   e_WRA,        10000,         0,      10000,        0, 0 },
            },
            #else                // ibm test data
            // commands set 3: to resolve concern 4 - 6.
            {
              {L_,    e_WR,            1,        0,           1,        0, 0 },
              {L_,    e_W,   SYS_DEPENDENT_LEN,  1,        1024,        0, 0 },
              {L_,   e_WR,    BUF_WRITE,        0,      BUF_WRITE,      0, 0 },
              {L_,  e_SIGNAL,          2,   e_READ_OP,        0,        0, 0 },
              {L_,    e_WR,    BUF_WRITE,        1,        1024,        0, 0 },
            },
            // commands set 4: to resolve concern 4 - 6.
            {
              {L_,  e_WRA,            1,        0,           1,         0, 0 },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        1024,         0, 0 },
              {L_,  e_WRA,    BUF_WRITE,        0,      BUF_WRITE,      0, 0 },
              {L_,  e_SIGNAL,         2,   e_READ_OP,        0,         0, 0 },
              {L_,  e_WRA,    BUF_WRITE,        1,        1024,         0, 0 },
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    e_W,           1,         0,           1,         0, 0 },
              {L_,    e_W,   SYS_DEPENDENT_LEN, 1,        8192,         0, 0 },
              {L_,   e_WR,    BUF_WRITE,        0,        1024,         0, 0 },
              {L_,  e_SIGNAL,        2,      e_READ_OP,      0,         0, 0 },
              {L_,   e_WR,        1000,         0,        1000,         0, 0 },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,   e_WA,           1,         0,           1,         0, 0 },
              {L_,   e_WA,   SYS_DEPENDENT_LEN, 1,        8192,         0, 0 },
              {L_,  e_WRA,    BUF_WRITE,        0,        1024,         0, 0 },
              {L_,  e_SIGNAL,        2,      e_READ_OP,      0,         0, 0 },
              {L_,   e_WRA,       1000,         0,        1000,         0, 0 },
            },

            #endif
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     e_W,          1,          0,           1,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,       0, 0 },
                // The channel will be closed by the peer when the write
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 0,         e_CLOSED,   0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
              {L_,     e_W,         100,         0,        e_INVALID,   0, 0 },
              {L_,    e_WA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the write method w the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    e_WA,          1,          0,           1,        0, 0 },
              {L_,     e_W,  SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN, 0, 0 },
              {L_,    e_WR,     BUF_WRITE,       0,         8192,       0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,           0,        0, 0 },
              {L_,    e_WA,   SYS_DEPENDENT_LEN, 0,         e_CLOSED,   0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
              {L_,    e_WA,         100,         0,        e_INVALID,   0, 0 },
              {L_,     e_W,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
             // Command set 3: Close the channel at the channel side to
            // test the behavior of the write method with the 'augStatus'
            // parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,     e_W,          1,          0,           1,        0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
              {L_,     e_W,         100,         0,        e_INVALID,   0, 0 },
              {L_,    e_WA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the write method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_,    e_WA,          1,          0,           1,        0, 0 },
              {L_,    e_WA,     BUF_WRITE,       0,      BUF_WRITE,     0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
            {L_, e_CLOSE_OBSERVE,  0,          0,           0,          0, 0 },
              {L_,    e_WA,   SYS_DEPENDENT_LEN, 0,         e_ERR,      0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent write operations will not succeed any
                // more.
              {L_,     e_W,         100,         0,        e_INVALID,   0, 0 },
              {L_,    e_WA,          60,         0,        e_INVALID,   0, 0 },
              {L_,    -1,           0,         0,           0,          0, 0 },
            },
            #endif

            #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
            {
              {L_,    e_WA,           1,         0,               1,    0, 0 },
              {L_,    e_WA,         100,         0,             100,    0, 0 },
              {L_,     e_W,          60,         0,              60,    0, 0 },
              {L_,    -1,           0,         0,               0,      0, 0 },
            },
            {
              {L_,     e_W,           1,         0,               1,    0, 0 },
              {L_,     e_W,         100,         0,             100,    0, 0 },
              {L_,     e_W,          60,         0,              60,    0, 0 },
              {L_,    -1,           0,         0,               0,      0, 0 },
            },

            #else                                      // unix test data

            #ifdef BSLS_PLATFORM_OS_SOLARIS
            // commands set 5: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     e_W,           1,         0,           1,       0, 0 },
              {L_,     e_W,          15,         0,          15,       0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,    0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,       0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 1,        8192,       0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 6: to resolve concern
            {
                // Each request write expected number of bytes from channel.
              {L_,    e_WA,           1,         0,           1,       0, 0 },
              {L_,    e_WA,          15,         0,          15,       0, 0 },
              {L_,    e_WA,     BUF_WRITE,       0,      BUF_WRITE,    0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,       0, 0 },
              {L_,    e_WA,         10000,       1,        8192,INTERRUPTED },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 7: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,  e_SIGNAL,          2,         0,           0,       0, 0 },
              {L_,     e_W,        10000,         1,        8192,       0, 0 },
            },

            // commands set 8: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
              {L_,  e_SIGNAL,          2,         0,           0,       0, 0 },
              {L_,    e_WA,        10000,         1,  8192, e_INTERRUPTED, 0 },
            },
            // commands set 9: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,       0,        0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,0, 0 },
            },

            // commands set 10: to resolve concern 4 - 6.
            {
              {L_,    e_WA,     BUF_WRITE,        0,      BUF_WRITE,    0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,       0,        0, 0 },
              {L_,    e_WA,        10000,         0,      10000,        0, 0 },
            },

            #elif defined(BSLS_PLATFORM_OS_LINUX)
            // commands set 3: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     e_W,           1,         0,           1,     0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,  0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,     0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 1,        0,        0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 4: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     e_WA,           1,         0,           1,       0, 0 },
              {L_,     e_WRA,     BUF_WRITE,       0,      BUF_WRITE,   0, 0 },
            //{L_,    e_WRA,       30000,         1,       30000,       0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         30,          0,          0,       0, 0 },
              {L_,      e_WA,      60000,         1, 16383, e_INTERRUPTED, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the "write" method w/o the
                // 'augStatus' argument.
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    e_WA,            1,         0,          1,        0, 0 },
              {L_,    e_WA,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,0, 0 },
              {L_,  e_SIGNAL,          2,      e_READ_OP,       0,      0, 0 },
              {L_,     e_W,         2000,         0,       2000,        0, 0 },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,    e_WA,            1,         0,           1,       0, 0 },
              {L_,    e_WA,  SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,0, 0 },
              {L_,  e_SIGNAL,          2,      e_READ_OP,        0,     0, 0 },
              {L_,    e_WA,         2000,         0,        2000,       0, 0 },
            },

            #else           // ibm test data

            // commands set 3: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     e_W,           1,         0,           1,       0, 0 },
              {L_,     e_W,     BUF_WRITE,       0,      BUF_WRITE,    0, 0 },
              {L_,     e_W,        30000,        1,       30000,       0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,         0,           0,       0, 0 },
              {L_,     e_W,   SYS_DEPENDENT_LEN, 1,        8192,       0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the write method w/o the
                // 'augStatus' argument.
            },
            // commands set 4: to resolve concern 4 - 6.
            {
                // Each request write expected number of bytes to the channel.
              {L_,     e_WA,           1,         0,           1,       0, 0 },
              {L_,     e_WA,     BUF_WRITE,       0,      BUF_WRITE,    0, 0 },
              {L_,     e_WA,       30000,         1,       30000,       0, 0 },
                // There are not enough space in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         2,          0,           0,       0, 0 },
              {L_,     e_WA,  SYS_DEPENDENT_LEN,  1,   8192,e_INTERRUPTED, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the "write" method w/o the
                // 'augStatus' argument.
            },

            // commands set 5: to resolve concern 4 - 6.
            {
              {L_,    e_WA,            1,         0,          1,        0, 0 },
              {L_,    e_WA,   SYS_DEPENDENT_LEN,  0,  SYS_DEPENDENT_LEN,0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,       0,        0, 0 },
              {L_,     e_W,         2000,         0,       2000,        0, 0 },
            },

            // commands set 6: to resolve concern 4 - 6.
            {
              {L_,    e_WA,            1,         0,           1,       0, 0 },
              {L_,    e_WA,  SYS_DEPENDENT_LEN,   0,  SYS_DEPENDENT_LEN,0, 0 },
            {L_,  e_SIGNAL,          2,      e_READ_OP,        0,       0, 0 },
              {L_,    e_WA,         2000,         0,        2000,       0, 0 },
            },

            #endif
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    int ioType;
                    #ifdef BSLS_PLATFORM_OS_AIX
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        // -------------------------------------------------------------------
        // TESTING 'bufferedReadRaw' METHOD:
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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

            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_RBR,         1,         0,         1,         0, 0 },
              {L_,     e_RBR,        15,         0,        15,         0, 0 },
              {L_,     e_RBR,       100,         0,        34,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,     e_RBR,       100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_RBR,       100,         0,      e_INVALID,    0, 0 },
              {L_,    e_RBRA,        60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method w the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RBRA,        10,         0,        10,         0, 0 },
              {L_,    e_RBRA,       100,         0,        40,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RBRA,       100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_RBR,        100,         0,      e_INVALID,   0, 0 },
              {L_,    e_RBRA,         60,         0,      e_INVALID,   0, 0 },
              {L_,    -1,            0,         0,         0,          0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,      50,         0,        50,         0, 0 },
              {L_,     e_RBR,         16,         0,        16,         0, 0 },
              {L_,     e_RBR,         60,         0,        34,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,    0,         0,         0,         0, 0 },
              {L_,    e_RBRA,         20,         0,       e_ERR,       0, 0 },
             {L_,     e_RBR,         80,         0,      e_INVALID,     0, 0 },
             {L_,    e_RBRA,         40,         0,      e_INVALID,     0, 0 },
              {L_,    -1,            0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,      50,         0,        50,         0, 0 },
              {L_,    e_RBRA,         16,         0,        16,         0, 0 },
              {L_,    e_RBRA,         60,         0,        34,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,    0,         0,         0,         0, 0 },
              {L_,    e_RBRA,         20,         1,       e_ERR,       0, 0 },
             {L_,     e_RBR,         80,         1,      e_INVALID,     0, 0 },
             {L_,    e_RBRA,         40,         1,      e_INVALID,     0, 0 },
              {L_,    -1,            0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,          0, 0 },
              {L_,  e_RBRA,           1,         0,         1,          0, 0 },
              {L_,  e_RBRA,          25,         0,        25,          0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,         0,            0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,          0, 0 },
              {L_,   e_RBR,           1,         0,         1,          0, 0 },
              {L_,   e_RBR,          25,         0,        25,          0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,         0,            0, 0 },
            },

            #else
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'readRaw' method w/o the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,      50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,    e_RB,           80,         1,    50, e_INTERRUPTED, 0 },
                // Now a "read" for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RBR,          40,         0,        40,         0, 0 },
              {L_,    e_RBR,           4,         0,         4,         0, 0 },
              {L_,    e_RBR,           2,         0,         2,         0, 0 },
              {L_,    e_RBR,           3,         0,         3,         0, 0 },
              {L_,   e_SIGNAL,         0,         1,         0,         0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_,   e_RBRA,          50,         0,         1,         0, 0 },
              {L_, e_HELP_WRITE,      20,         0,        20,         0, 0 },
            },
            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the "read" method under test with the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,          0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,          0, 0 },
              {L_,    e_RBA,         80,         1,     50, e_INTERRUPTED, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RBA,         40,         0,        40,          0, 0 },
              {L_,    e_RBA,          4,         0,         4,          0, 0 },
              {L_,    e_RBA,          2,         0,         2,          0, 0 },
              {L_,    e_RBA,          3,         0,         3,          0, 0 },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   e_SIGNAL,        0,         1,         0,          0, 0 },
              {L_,    e_RBA,         90,         0,        90,          0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_,   e_RBRA,         50,         0,        11,          0, 0 },
            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,          0, 0 },
              {L_,     e_RB,          1,         0,         1,          0, 0 },
              {L_,    e_RBA,         10,         0,        10,          0, 0 },
              {L_,     e_RB,         15,         0,        15,          0, 0 },
              {L_,    e_RBA,         20,         0,        20,          0, 0 },
                // When there is not enough data (but some data is available),
                // a 'readRaw' will return the number of bytes it read.
              {L_,    e_RBR,         25,         0,         4,          0, 0 },
              {L_, e_HELP_WRITE,     10,         0,        10,          0, 0 },
              {L_,   e_RRA,          35,         0,        10,          0, 0 },
                // There are no data left in the TCP buffer for next request,
                // so even though an "asynchronous event" happens and the
                // 'readRaw' will keep waiting for some data to come, no matter
                // if it's in 'non-interruptible' or 'interruptible' mode.
              {L_,  e_SIGNAL,         1,         1,         0,          0, 0 },
              {L_,    e_RBR,        105,         1,       100,          0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,          0, 0 },
              {L_,    e_RBR,        105,         0,       100,          0, 0 },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  e_SIGNAL,         1,         1,         0,          0, 0 },
              {L_,   e_RBRA,        105,         1,       100,          0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,          0, 0 },
              {L_,   e_RBRA,        105,         0,       100,          0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // TESTING 'bufferedRead' METHOD:
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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

            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_RB,          1,         0,         1,         0, 0 },
              {L_,     e_RB,         15,         0,        15,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,     e_RB,        100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_RB,        100,         0,      e_INVALID,    0, 0 },
              {L_,    e_RBA,         60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method w the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RBA,         10,         0,        10,         0, 0 },
              {L_,    e_RBA,         20,         0,        20,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RBA,        100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_RB,        100,         0,      e_INVALID,    0, 0 },
              {L_,    e_RBA,         60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_RB,          1,         0,         1,         0, 0 },
              {L_,     e_RB,         30,         0,        30,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RBA,         20,         0,       e_ERR,       0, 0 },
              {L_,     e_RB,         80,         0,      e_INVALID,    0, 0 },
              {L_,    e_RBA,         40,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_RB,          1,         0,         1,         0, 0 },
              {L_,    e_RBA,         10,         0,        10,         0, 0 },
              {L_,     e_RB,         15,         0,        15,         0, 0 },
              {L_,    e_RBA,         20,         0,        20,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RBA,         20,         1,       e_ERR,       0, 0 },
              {L_,     e_RB,         80,         1,      e_INVALID,    0, 0 },
              {L_,    e_RBA,         40,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RBA,           1,         0,         1,         0, 0 },
              {L_,   e_RBA,          25,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RB,           1,         0,         1,         0, 0 },
              {L_,    e_RB,          25,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,         0,           0, 0 },
            },

            #else
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'readRaw' method w/o the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,    50, e_INTERRUPTED, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RB,          40,         0,        40,         0, 0 },
              {L_,    e_RB,           4,         0,         4,         0, 0 },
              {L_,    e_RB,           2,         0,         2,         0, 0 },
              {L_,    e_RB,           3,         0,         3,         0, 0 },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,    e_RB,          90,         0,        90,         0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_,   e_RBRA,         50,         0,        11,         0, 0 },
            },
            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the "read" method under test with the
            // 'augStatus' parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RBA,         80,         1,    50, e_INTERRUPTED, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RBA,         40,         0,        40,         0, 0 },
              {L_,    e_RBA,          4,         0,         4,         0, 0 },
              {L_,    e_RBA,          2,         0,         2,         0, 0 },
              {L_,    e_RBA,          3,         0,         3,         0, 0 },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,    e_RBA,         90,         0,        90,         0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_,   e_RBRA,         50,         0,        11,         0, 0 },
            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_RB,          1,         0,         1,         0, 0 },
              {L_,    e_RBA,         10,         0,        10,         0, 0 },
              {L_,     e_RB,         15,         0,        15,         0, 0 },
              {L_,    e_RBA,         20,         0,        20,         0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,    e_RBA,          5,         1,     4, e_INTERRUPTED, 0 },
                // This request is to remove data in the internal buffer due to
                // the previous partial read, and so some other tests can be
                // done.
              {L_,    e_RBA,          4,         1,         4,         0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_, e_HELP_WRITE,      1,         1,         1,         0, 0 },
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,     e_RB,          5,         1,     1, e_INTERRUPTED, 0 },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // implemented to write 100 bytes to the channel feed the (TBD
                // - figure out what goes here).
              {L_, e_HELP_WRITE,     20,         0,        20,         0, 0 },
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,     e_RBA,        25,         0,        25,         0, 0 },
                // The same situation for the 'read' operation, without the
                // 'augStatus' as the parameter.  The behavior should be the
                // same as above.
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,     e_RB,        150,         0,       150,         0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int readvRaw(btls::Iovec *buffer,
        //                int         numBytes,
        //                int         flags = 0);
        //   int readvRaw(int        *augStatus,
        //                btls::Iovec *buffer,
        //                int         numBytes,
        //                int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'readvRaw' method");
            QT("=========================");
        }

        {

            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RVR,          1,         0,         1,         0, 0 },
              {L_,    e_RVR,          7,         0,        49,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RVR,          6,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RVR,          7,         0,      e_INVALID,    0, 0 },
              {L_,   e_RVRA,          6,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVRA,          1,         0,         1,         0, 0 },
              {L_,   e_RVRA,          7,         0,        49,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,   e_RVRA,          6,         0,      e_CLOSED,     0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RVR,          7,         0,     e_INVALID,     0, 0 },
              {L_,   e_RVRA,          6,         0,     e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RVR,          1,         0,         1,         0, 0 },
              {L_,    e_RVR,          7,         0,        49,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,   e_RVRA,          6,         1,       e_ERR,       0, 0 },
              {L_,    e_RVR,          4,         1,     e_INVALID,     0, 0 },
              {L_,   e_RVRA,          5,         1,     e_INVALID,     0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVRA,          2,         0,         4,         0, 0 },
              {L_,   e_RVRA,          7,         0,        46,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RVR,          6,         1,       e_ERR,       0, 0 },
              {L_,    e_RVR,          4,         1,      e_INVALID,    0, 0 },
              {L_,   e_RVRA,          5,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,  e_RVRA,           1,         0,         1,         0, 0 },
              {L_,  e_RVRA,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVR,           1,         0,         1,         0, 0 },
              {L_,   e_RVR,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,         0,           0, 0 },
            },

            #else
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a "read" will return right away because there's data in
                // the internal buffer at this moment.
              {L_,    e_RVR,          2,         0,         4,         0, 0 },
              {L_,    e_RVR,          3,         0,         9,         0, 0 },
              {L_,    e_RVR,          6,         0,        35,         0, 0 },
                // The next 'read' keep reading data it needs until no data in
                // the internal buffer.
              {L_,    e_RVR,          4,         0,         2,         0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_, e_HELP_WRITE,     25,         0,        25,         0, 0 },
              {L_,    e_RVR,          6,         0,        25,         0, 0 },
            },

            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a "read" will return right away because there's enough
                // data in the internal buffer at this moment.
              {L_,   e_RVRA,          2,         0,         4,         0, 0 },
              {L_,   e_RVRA,          3,         0,         9,         0, 0 },
              {L_,   e_RVRA,          6,         0,        35,         0, 0 },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint through
                // a thread.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,   e_RVRA,          4,         0,         2,         0, 0 },
                // There are enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,   e_RVRA,          6,         0,        35,         0, 0 },
              {L_,   e_RBR,         150,         0,        65,         0, 0 },
            },

            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RVR,          1,         0,         1,         0, 0 },
              {L_,   e_RVRA,          2,         0,         4,         0, 0 },
              {L_,    e_RVR,          4,         0,        16,         0, 0 },
              {L_,   e_RVRA,          5,         0,        25,         0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, but a "raw" operation will still return.
              {L_,    e_RVR,          3,         0,         4,         0, 0 },
              {L_, e_HELP_WRITE,     10,         0,        10,         0, 0 },
              {L_,   e_RVRA,          4,         1,        10,         0, 0 },
                // There are no data left in the TCP buffer for next request,
                // so even though an "asynchronous event" happens and the
                // 'readRaw' will keep waiting for some data to come, no matter
                // if it's in 'non-interruptible' or 'interruptible' mode.
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,    e_RVR,          8,         1,       100,         0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,    e_RVR,          8,         0,       100,         0, 0 },
                // The same as above except w/ 'augStatus' parameter
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,   e_RVRA,          8,         1,       100,         0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,   e_RVRA,          8,         0,       100,         0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));

                char buf0[k_VECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[k_VECBUF_LEN5] = "\0", buf3[k_VECBUF_LEN7] = "\0",
                     buf4[k_VECBUF_LEN9] = "\0", buf5[k_VECBUF_LEN10] = "\0",
                     buf6[k_VECBUF_LEN50] = "\0", buf7[k_VECBUF_LEN90] = "\0";

                btls::Iovec vecBuffer[k_MAX_VECBUF];
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
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
                if (verbose) {
                    QT("Finish testing commands set: ");
                    int setNo = i + 1;
                    PT(setNo);
                }
            }
        }
      } break;
      case 4: {
#if !defined(BSLS_PLATFORM_OS_SOLARIS) || \
    !defined(BSLS_PLATFORM_CMP_GNU) || \
    (BSLS_PLATFORM_CMP_VER_MAJOR < 40000)

        // -------------------------------------------------------------------
        // TESTING 'readv' METHOD:
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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
        //   int readv(btls::Iovec *buffer,
        //             int         numBytes,
        //             int         flags = 0);
        //   int readv(int        *augStatus,
        //             btls::Iovec *buffer,
        //             int         numBytes,
        //             int         flags = 0);
        // ----------------------------------------------------------------
        if (verbose) {
            QT("Testing 'readv' method");
            QT("======================");
        }

        {
            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RV,           1,         0,         1,         0, 0 },
              {L_,    e_RV,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RV,           6,         0,    e_CLOSED,       0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RV,           7,         0,      e_INVALID,    0, 0 },
              {L_,   e_RVA,           6,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVA,           1,         0,         1,         0, 0 },
              {L_,   e_RVA,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,   e_RVA,           6,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RV,           7,         0,      e_INVALID,    0, 0 },
              {L_,   e_RVA,           6,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RV,           1,         0,         1,         0, 0 },
              {L_,    e_RV,           5,         0,        25,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,   e_RVA,           6,         1,       e_ERR,       0, 0 },
              {L_,    e_RV,           4,         1,      e_INVALID,    0, 0 },
              {L_,   e_RVA,           5,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVA,           2,         0,         4,         0, 0 },
              {L_,   e_RVA,           6,         0,        35,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RV,           6,         1,       e_ERR,       0, 0 },
              {L_,    e_RV,           4,         1,      e_INVALID,    0, 0 },
              {L_,   e_RVA,           5,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RVA,           1,         0,         1,         0, 0 },
              {L_,   e_RVA,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RV,           1,         0,         1,         0, 0 },
              {L_,    e_RV,           5,         0,        25,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
               {L_,   -1,           0,         0,         0,           0, 0 },
            },

            #else
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a "read" will return right away because there's enough
                // data in the internal buffer at this moment.
              {L_,    e_RV,           2,         0,         4,         0, 0 },
              {L_,    e_RV,           3,         0,         9,         0, 0 },
              {L_,    e_RV,           6,         0,        35,         0, 0 },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint through
                // a thread.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,    e_RV,           4,         0,        16,         0, 0 },
                // There are enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,    e_RV,           6,         0,        35,         0, 0 },
              {L_,   e_RBR,         150,         0,        51,         0, 0 },
            },

            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a "read" will return right away because there's enough
                // data in the internal buffer at this moment.
              {L_,   e_RVA,           2,         0,         4,         0, 0 },
              {L_,   e_RVA,           3,         0,         9,         0, 0 },
              {L_,   e_RVA,           6,         0,        35,         0, 0 },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint through
                // a thread.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,   e_RVA,           4,         0,        16,         0, 0 },
                // There are enough data in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,   e_RVA,           6,         0,        35,         0, 0 },
              {L_,   e_RBR,         150,         0,        51,         0, 0 },
            },

            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RV,           1,         0,         1,         0, 0 },
              {L_,   e_RVA,           2,         0,         4,         0, 0 },
              {L_,    e_RV,           3,         0,         9,         0, 0 },
              {L_,   e_RVA,           6,         0,        35,         0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,   e_RVA,           3,         1,     1, e_INTERRUPTED, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,    e_RV,           1,         1,         0,         0, 0 },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // implemented to write 100 bytes to the channel feed the (TBD
                // - figure out what goes here).

              {L_, e_HELP_WRITE,     20,         0,        20,         0, 0 },
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,    e_RVA,          6,         0,        35,         0, 0 },
                // The same situation for the 'read' operation, without the
                // 'augStatus' as the parameter.  The behavior should be the
                // same as above.
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,    e_RV,           7,         0,        85,         0, 0 },
            },
            // commands set 8: to resolve concern 6 - 8.
            {
                // Test the situation when multiple adjustments of the buffers
                // are needed.
              {L_, e_HELP_WRITE,     75,         0,        75,         0, 0 },
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,   e_RVA,           8,         0,       175,         0, 0 },

                // Test the situation when multiple adjustments of the buffers
                // are needed.
              {L_, e_HELP_WRITE,     75,         0,        75,         0, 0 },
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,   e_RV,            8,         0,       175,         0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));

                char buf0[k_VECBUF_LEN1], buf1[k_VECBUF_LEN3] = "\0",
                     buf2[k_VECBUF_LEN5] = "\0", buf3[k_VECBUF_LEN7] = "\0",
                     buf4[k_VECBUF_LEN9] = "\0", buf5[k_VECBUF_LEN10] = "\0",
                     buf6[k_VECBUF_LEN50] = "\0", buf7[k_VECBUF_LEN90] = "\0";

                btls::Iovec vecBuffer[k_MAX_VECBUF];
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
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, vecBuffer, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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

            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RR,           1,         0,         1,         0, 0 },
              {L_,    e_RR,          15,         0,        15,         0, 0 },
              {L_,    e_RR,         100,         0,        34,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RR,         100,         0,    e_CLOSED,       0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RR,         100,         0,      e_INVALID,    0, 0 },
              {L_,   e_RRA,          60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RRA,          10,         0,        10,         0, 0 },
              {L_,   e_RRA,         100,         0,        40,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,   e_RRA,         100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,    e_RR,         100,         0,      e_INVALID,    0, 0 },
              {L_,   e_RRA,          60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)
            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RR,          16,         0,        16,         0, 0 },
              {L_,    e_RR,          60,         0,        34,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RR,          20,         1,       e_ERR,       0, 0 },
              {L_,    e_RR,          80,         1,      e_INVALID,    0, 0 },
              {L_,   e_RRA,          40,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RRA,          16,         0,        16,         0, 0 },
              {L_,   e_RRA,          60,         0,        34,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,   e_RRA,          20,         0,       e_ERR,       0, 0 },
              {L_,    e_RR,          80,         0,      e_INVALID,    0, 0 },
              {L_,   e_RRA,          40,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,   e_RRA,           1,         0,         1,         0, 0 },
              {L_,   e_RRA,           5,         0,         5,         0, 0 },
              {L_,   e_RBR,         250,         0,        44,         0, 0 },
            },
            {
              {L_, e_HELP_WRITE,    100,         0,         0,         0, 0 },
              {L_,    e_RR,           1,         0,         1,         0, 0 },
              {L_,    e_RR,           5,         0,         5,         0, 0 },
              {L_,   e_RBR,         250,         0,        94,         0, 0 },
            },

            #else
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'readRaw' method w/o the 'augStatus'
            // parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RR,          40,         0,        40,         0, 0 },
              {L_,    e_RR,           4,         0,         4,         0, 0 },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,    e_RR,          15,         0,         6,        10, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_, e_HELP_WRITE,     25,         0,        25,         0, 0 },
              {L_,   e_RRA,          50,         0,        25,         0, 0 },
            },
            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'readRaw' method with the 'augStatus'
            // parameter (concern 4).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RRA,          40,         0,        40,        0, 0 },
              {L_,    e_RRA,          4,          0,         4,        0, 0 },
                // The next 'readRaw' can't find all data it needs, so it'll
                // return data it can read.
              {L_,    e_RRA,         15,          0,         6,        0, 0 },
                // Now there's no data in the internal buffer, any new "read"
                // request has to read directly from the channel. the last one
                // is a "raw" operation.
              {L_, e_HELP_WRITE,      25,         0,        25,        0, 0 },
              {L_,   e_RRA,           50,         0,        25,        0, 0 },
            },
            // commands set 7: to resolve concern 5 - 7.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RR,           1,         0,         1,         0, 0 },
              {L_,   e_RRA,          10,         0,        10,         0, 0 },
              {L_,    e_RR,          15,         0,        15,         0, 0 },
              {L_,   e_RRA,          20,         0,        20,         0, 0 },
                // When there is not enough data (but some data is available),
                // a 'readRaw' will return the number of bytes it reads.
              {L_,    e_RR,          25,         0,         4,         0, 0 },
              {L_, e_HELP_WRITE,     10,         0,        10,         0, 0 },
              {L_,   e_RRA,          35,         0,        10,         0, 0 },
                // There are no data left in the TCP buffer for next request,
                // so even though an "asynchronous event" happens and the
                // 'readRaw' will keep waiting for some data to come, no matter
                // if it's in 'non-interruptible' or 'interruptible' mode.
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,    e_RR,         105,         1,       100,         0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,    e_RR,         105,         0,       100,         0, 0 },
                // The same thing for 'readRaw' w/ 'augStatus' parameter
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,   e_RRA,         105,         1,       100,         0, 0 },
              {L_,  e_SIGNAL,         1,         1,         0,         0, 0 },
              {L_,   e_RRA,         105,         0,       100,         0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //
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
        //   'btlsos::TcpTimedChannel' object 'channel' is created with one of
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

            TestCommand COMMANDS_SET[][k_MAX_CMD] =
            //line   command    numToUse   interruptFlags   expRet   expAugStat
            //----   -------    --------   --------------   ------   ----------
//==========>
          {
            // Command set 1: Close the channel from the peer side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_R,           1,         0,         1,         0, 0 },
              {L_,     e_R,          15,         0,        15,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,     e_R,         100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_R,         100,         0,      e_INVALID,    0, 0 },
              {L_,    e_RA,          60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 2: Close the channel from the peer side to test the
            // behavior of the 'read' method w the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RA,          10,         0,        10,         0, 0 },
              {L_,    e_RA,          20,         0,        20,         0, 0 },
                // The channel will be closed by the peer when the 'read'
                // request is on going, so it'll return 'CLOSED'.
              {L_, e_CLOSE_CONTROL,   0,         0,         0,         0, 0 },
              {L_,    e_RA,         100,         0,       e_CLOSED,    0, 0 },
                // The channel now is invalid due to the operation failure, and
                // so the subsequent read operations will not succeed any more.
              {L_,     e_R,         100,         0,      e_INVALID,    0, 0 },
              {L_,    e_RA,          60,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,           0,         0, 0 },
            },

            #if defined (BSLS_PLATFORM_OS_SOLARIS) || \
                defined (BSLS_PLATFORM_OS_WINDOWS)

            // Command set 3: Close the channel at the channel side to test the
            // behavior of the 'read' method with the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_R,           1,         0,         1,         0, 0 },
              {L_,     e_R,          30,         0,        30,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RA,          20,         0,       e_ERR,       0, 0 },
              {L_,     e_R,          80,         0,      e_INVALID,    0, 0 },
              {L_,    e_RA,          40,         0,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },

            // Command set 4: Close the channel at the channel side to test the
            // behavior of the 'read' method w/o the 'augStatus' parameter.
            {
                // Establish a channel and verify that it works fine.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_R,           1,         0,         1,         0, 0 },
              {L_,    e_RA,          10,         0,        10,         0, 0 },
              {L_,     e_R,          15,         0,        15,         0, 0 },
              {L_,    e_RA,          20,         0,        20,         0, 0 },
                // Now close the channel, and try some 'read' operations, each
                // of which should return a "ERROR".
              {L_, e_CLOSE_OBSERVE,   0,         0,         0,         0, 0 },
              {L_,    e_RA,          20,         1,       e_ERR,       0, 0 },
              {L_,     e_R,          80,         1,      e_INVALID,    0, 0 },
              {L_,    e_RA,          40,         1,      e_INVALID,    0, 0 },
              {L_,    -1,           0,         0,         0,           0, 0 },
            },
            #endif

            #ifdef BSLS_PLATFORM_OS_WINDOWS
            {
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,    e_RA,           1,         0,         1,         0, 0 },
              {L_,    e_RA,           5,         0,         5,         0, 0 },
              {L_,   e_RBR,         250,         0,        44,         0, 0 },
            },
            {
              {L_, e_HELP_WRITE,     50,         0,         0,         0, 0 },
              {L_,     e_R,           1,         0,         1,         0, 0 },
              {L_,     e_R,           5,         0,         5,         0, 0 },
              {L_,   e_RBR,         250,         0,        44,         0, 0 },
            },

            #else // !(defined BSLS_PLATFORM_OS_SOLARIS)
            // Commands set 5: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method w/o the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,     e_R,          40,         0,        40,         0, 0 },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint through
                // a thread.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,     e_R,          15,         0,        15,         0, 0 },
                // There are 95 bytes in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,     e_R,          50,         0,        50,         0, 0 },
              {L_,   e_RBR,          50,         0,        45,         0, 0 },
            },

            // Commands set 6: Establish a channel and make the expected number
            // of bytes of data available in the channel's internal buffer,
            // test the behavior of the 'read' method with the 'augStatus'
            // parameter (concern 4 - 5).
            {
                // The control socket write 50 bytes to the channel for read.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
                // A 'bufferedRead' for 80 bytes is issued in 'interruptible'
                // mode, while a signal 'SIGSYS' is generated, and so the
                // returned 50 is stored in the internal buffer for later read
                // try.
              {L_,   e_SIGNAL,        1,         0,         0,         0, 0 },
              {L_,    e_RB,          80,         1,        50,         0, 0 },
                // Now a 'read' for 40 bytes will return right away because
                // there's enough data in the internal buffer at this moment.
              {L_,    e_RA,          40,         0,        40,         0, 0 },
                // The next 'read' can't find all data it needs, and so it'll
                // try reading the remaining data from the channel directly,
                // which is at last available from the control endpoint through
                // a thread.
              {L_,   e_SIGNAL,        0,         1,         0,         0, 0 },
              {L_,    e_RA,          15,         0,        15,         0, 0 },
                // There are 95 bytes in the channel's TCP buffer, the
                // following requests can all meet their expectations because
                // the last one is a "raw" operation.
              {L_,    e_RA,          50,         0,        50,         0, 0 },
              {L_,   e_RBR,          50,         0,        45,         0, 0 },
            },
            // commands set 7: to resolve concern 6 - 8.
            {
                // Each request read expected number of bytes from the channel.
              {L_, e_HELP_WRITE,     50,         0,        50,         0, 0 },
              {L_,     e_R,           1,         0,         1,         0, 0 },
              {L_,    e_RA,          10,         0,        10,         0, 0 },
              {L_,     e_R,          15,         0,        15,         0, 0 },
              {L_,    e_RA,          20,         0,        20,         0, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it.
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,    e_RA,           5,         1,     4, e_INTERRUPTED, 0 },
                // There are not enough bytes left in the TCP buffer for next
                // request, now we'll generate signals to interrupt it, the
                // only difference is we call the 'read' method w/o the
                // 'augStatus' argument.
              {L_, e_HELP_WRITE,      1,         1,         1,         0, 0 },
              {L_,  e_SIGNAL,         1,         0,         0,         0, 0 },
              {L_,     e_R,           5,         1,         1,         0, 0 },
                // Test if a request is in 'non-interrupt' mode, it won't be
                // interrupted by an "AE" but waiting for the more data to
                // come.  The request will not return until all the requested
                // data were read.  (Here the signal generating thread is
                // implemented to write 100 bytes to the channel feed the (TBD
                // - figure out what goes here).
              {L_, e_HELP_WRITE,     20,         0,        20,         0, 0 },
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,     e_RA,         25,         0,        25,         0, 0 },
                // The same situation for the 'read' operation, without the
                // 'augStatus' as the parameter.  The behavior should be the
                // same as above.
              {L_,   e_SIGNAL,        1,         1,         0,         0, 0 },
              {L_,     e_R,         150,         0,       150,         0, 0 },
            },
            #endif
          };
//==========>
            const int NUM_SETS = sizeof COMMANDS_SET / sizeof *COMMANDS_SET;
            for (int i = 0; i < NUM_SETS; ++i) {
                char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
                btlso::SocketHandle::Handle handles[2]; // handles[0] is the
                                                       // test endpoint, while
                                                       // handles[1] is the
                                                       // control endpoint.
                btlso::StreamSocket<btlso::IPv4Address> *sSocket;
                ASSERT(0 == buildChannelHelper(handles, &factory, &sSocket));
                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor and the behavior is undefined otherwise.  We
                    // insure the required order by creating the 'channel'
                    // inside a block while the corresponding 'streamSocket'
                    // object outside the block as above.

                    Obj channel(sSocket, &testAllocator);

                    #ifdef BSLS_PLATFORM_OS_UNIX
                    registerSignal(SIGSYS, signalHandler);
                    #endif

                    ASSERT(0 == processTest(&channel,
                                            handles,
                                            COMMANDS_SET[i],
                                            buf, 0, 0, 0) );
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
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
        //   We need to exercise basic value-semantic functionality.  In
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
    btlso::SocketImpUtil::cleanup(&errCode);
    ASSERT(0 == errCode);

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
