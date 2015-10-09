// btlsos_tcptimedcbchannel.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcptimedcbchannel.h>

#include <btlso_tcptimereventmanager.h>
#include <btlso_inetstreamsocketfactory.h>   // create a streamsocket opt.
#include <btlso_ipv4address.h>
#include <btlso_sockethandle.h>
#include <btlso_socketimputil.h>
#include <btlso_socketoptutil.h>
#include <btlso_streamsocketfactory.h>

#include <btlsc_cbchannel.h>

#include <btls_iovec.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>

#include <bdlt_currenttime.h>

#include <bslma_testallocator.h>              // for testing only
#include <bslma_testallocatorexception.h>     // for testing only

#include <bslim_testutil.h>
#include <bsls_platform.h>

#include <bsl_c_ctype.h>     // 'isdigit'
#include <bsl_c_signal.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winbase.h>    // 'sleep'.
#include <windows.h>
#endif

#include <signal.h>

#ifdef BSLS_PLATFORM_UNIX
#include <sys/socket.h>
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdlf::PlaceHolders;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a set of member functions that are used
// to provides a non-blocking communication channel over TCP/IPv4 sockets that
// adheres to 'btlsc::CbChannel' protocol.  The basic plan for the testing is
// to ensure that all methods in the component work as expected.  By using the
// table-driven strategy, I/O requests on a channel will be decoded from the
// request script, which is based on a specified "SCRIPT" language.  To see how
// a script sentence looks and works, please refer to the "SCRIPT" definition
// and document in the "GENERATOR FUNCTION 'gg' FOR TESTING" section below.
// Combining the complete set of test cases in the test script, each single
// command parsed from the script will be used to call a function to be tested.
// The result can then be compared against manually calculated values in a
// table to check if the function being tested generates the correct results.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btlsos::TcpTimedCbChannel(..., *manager, ...);
// [ 5] btlsos::TcpTimedCbChannel(..., *rManager, *wManager, ...);
// [ 2] ~btlsos::TcpTimedCbChannel();
// MANIPULATORS
// [ 6] int read();
// [ 7] int timedRead();
// [14] int readv();
// [15] int timedReadv();
// [ 8] int readRaw();
// [ 9] int timedReadRaw();
// [14] int readvRaw();
// [15] int timedReadvRaw();
// [10] int bufferedRead();
// [11] int timedBufferedRead();
// [12] int bufferedReadRaw();
// [13] int timedBufferedReadRaw();
// [18] int write();
// [19] int timedWrite();
// [16] int writeRaw();
// [17] int timedWriteRaw();
// [20] int writev(const btls::Iovec::Ovec *buffers, ...);
// [20] int writev(const btls::Iovec::Iovec *buffers, ...);
// [21] int timedWritev(const btls::Iovec::Ovec *buffers, ...);
// [21] int timedWritev(const btls::Iovec::Iovec *buffers, ...);
// [21] int writevRaw(const btls::Iovec::Ovec *buffers, ...);
// [21] int writevRaw(const btls::Iovec::Iovec *buffers, ...);
// [21] int timedWritevRaw(const btls::Iovec::Ovec *buffers, ...);
// [21] int timedWritevRaw(const btls::Iovec::Iovec *buffers, ...);
// [22] int bufferedWrite();
// [23] int timedBufferedWrite();
// [24] int bufferedWritev(const btls::Iovec::Ovec *buffers, ...);
// [24] int bufferedWritev(const btls::Iovec::Iovec *buffers, ...);
// [25] int timedBufferedWritev(const btls::Iovec::Ovec *buffers, ...);
// [25] int timedBufferedWritev(const btls::Iovec::Iovec *buffers, ...);
// [ 4] void cancelAll();
// [ 5] void cancelRead();
// [ 5] void cancelWrite();
// [ 2] void invalidateRead();
// [ 2] void invalidateWrite();
// ACCESSORS
// [ 4] int isInvalid() const;
// [ 4] int numPendingReadOperations() const;
// [ 4] int numPendingWriteOperations() const;
// [ 4] btlso::StreamSocket<btlso::IPv4Address> *socket() const;
//-----------------------------------------------------------------------------
// [26] USAGE example
// [ 1] Breathing test

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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef btlsos::TcpTimedCbChannel Obj;
#ifdef BSLS_PLATFORM_OS_UNIX
const int MAX_BUF     = 99000;     // the biggest length of a buffer for write

    #if defined BSLS_PLATFORM_OS_AIX
    const int BUF_WRITE   = 1000;  // the last buffer length for ioVec/oVec
    const int BUF_WRITE2  = 500;   // the last second buffer length for vector
    const int BUF_LIMIT   = 1024;  // to set the snd/rcv buffer size
    #elif defined BSLS_PLATFORM_OS_LINUX
    const int BUF_WRITE   = 15343;  // the last buffer length for ioVec/oVec
    const int BUF_WRITE2  = 1000;   // the last second buffer length for vector
    const int BUF_LIMIT   = 4096;  // to set the snd/rcv buffer size
    #else
    const int BUF_WRITE   = 73680; // the last buffer length for ioVec/oVec
    const int BUF_WRITE2  = 1000;  // the last second buffer length for vector
    const int BUF_LIMIT   = 8192;  // to set the snd/rcv buffer size
    #endif
#else
const int MAX_BUF     = 30000;
const int BUF_LIMIT   = 8192;
const int BUF_WRITE   = 28720; // the longest buffer length for ioVec/oVec
const int BUF_WRITE2  = 1000;  // the last second buffer length for vector

#endif

const int MAX_CMDS    = 50;   // the maximum commands in one script
const int MAX_VEC     = 16;   // the maximum buffers for a readv() or writev().

const int WRITELOWWATER = 128;
const int READLOWWATER  = 1;

static int veryVerbose;
static int veryVeryVerbose;

static const struct {
    int         d_lineNum;
    const char *d_sndBuf;        // write into the channel from this buffer
    int         d_sndLen;        // the string length to be sent.
} BUFFERS[] =
  //line           d_sndBuf        d_sndLen
  //----           --------        --------
{
    { L_,        "j1234567890",         11},
    { L_,               "abcd",          4},
    { L_,                "ABC",          3},
    { L_,                 "lj",          2},
    { L_,   "1234567890abcdefghij",     20},
    { L_,                  "",           0},

};

struct Buffer { // for mixing I/O request, e.g. bufferRead and readvRaw request
    char       *d_readBuf;       // to read into this buffer
    const char *d_writeBuf;      // to be written to the channel.
    btls::Iovec *d_readIovecBuf;  // to read into this vector
    btls::Iovec *d_writeIovecBuf; // to be written to the channel.
    btls::Ovec  *d_writeOvecBuf;  // to be written to the channel.
};

btls::Iovec ioVec[MAX_VEC]; // global object to be used in the gg() for vector
                           // read or write
btls::Ovec oVec[MAX_VEC];   // global object to be used in the gg() for vector
                           // read or write
char readBuf[MAX_BUF],
     str[BUF_WRITE+1], str2[BUF_WRITE2+1];  // to be written to a channel
#ifdef BSLS_PLATFORM_OS_LINUX
char str3[4096];
#endif

//-----------------------------------------------------------------------------

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int gg(btlsos::TcpTimedCbChannel   *channel,
              Buffer                      *buffer,
              btlso::TcpTimerEventManager *rManager,
              btlso::TcpTimerEventManager *wManager,
              const char                  *script);

#ifdef BSLS_PLATFORM_OS_UNIX

static void sigPipeHandler(int sig)
{
    if (sig == SIGPIPE && veryVerbose) {
        write(2, "GOT A SIGPIPE signal.\n", sizeof("GOT A SIGPIPE signal.\n"));
        write(2, "!!!!!!!!!!!!!!!!!!!!!\n", sizeof("!!!!!!!!!!!!!!!!!!!!!\n"));
    }
    return;
}

static void mySignal(int signo, void (*handler)(int) )
{
    struct sigaction act, oact;

    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    #ifdef SA_INTERRUPT               // SunOS 4.x
        act.sa_flags |= SA_INTERRUPT;
    #endif

    #ifdef SA_RESTART                 // SVR4, 4.4BSD
        act.sa_flags |= SA_RESTART;
    #endif

    if (sigaction(signo, &act, &oact) < 0) {
        ASSERT("SIG_ERR" && 0);
    }
    return;
}

#endif

static const char *get1stCbCommand(const char *commandSeq)
    // Get the first callback command to be executed in the user-installed
    // callback function, which is enclosed in '{' and '}'.  Return a pointer
    // to the command sequence string, on success.  Return 0 when there's no
    // this kind of command or reaches the end of the command sequence.
{
    ASSERT("command shouldn't be null" && commandSeq);

    while ('{' != *commandSeq && ';' != *commandSeq
                              && '\0' != *commandSeq)  ++commandSeq;
    if (';' == *commandSeq || '\0' == *commandSeq)
        return 0;                                                     // RETURN
    else
        return ++commandSeq;   // skip '{'                            // RETURN
}

static const char *getNextCbCommand(const char *cbCommand)
    // Return the next callback command in the command list if there's any;
    // e.g., in callback command list "{W1;r3,3,0,{r2,2,0,{cr}};r2,3,0}", there
    // are 3 commands to be executed in the user-installed callback, this
    // function is to return the next callback command (but not the first
    // callback command, which should be returned by get1stCbCommand() above.
    // return 0 if not any more.
{
    int curly = 0;

    while (curly || '}' != *cbCommand) { // if it's the end of the cbcommand
        if (';' != *cbCommand && '\0' != *cbCommand) {
            if ('{' == *cbCommand)
                curly++;
            else if ('}' == *cbCommand)
                curly--;
            ++cbCommand;
        }
        else if (curly)
            ++cbCommand;
        else
            break;
    }
    if (';' == *cbCommand) {
        while (' ' == *cbCommand || ';' == *cbCommand)  ++cbCommand;
        return cbCommand;                                             // RETURN
    }
    else
        return 0;                                                     // RETURN
}

static int helpWrite(btlso::SocketHandle::Handle  client,
                     const char                  *buf,
                     int                          bufLen)
    // Write data into the channel, by which different read function can be
    // tested.  Return 0 on success, non-zero otherwise.
{
  if (0 == buf || 0 == bufLen)
      return 0;                                                       // RETURN
  else {
      int len = btlso::SocketImpUtil::write(client, buf, bufLen);
      if (veryVerbose) {
          cout << "Wrote " << len << " bytes to the control pipe." << endl;
      }
      return len != bufLen;                                           // RETURN
  }
}

static void helpBuildVector()
    // Build a list of vectors for readv/writev.
{
    int i;

    memset(str,'5', BUF_WRITE);
    str[BUF_WRITE] = '\0';

    memset(str2,'2', BUF_WRITE2);
    str2[BUF_WRITE2] = '\0';

    for (i = 0; i < 5; ++i) {
        ioVec[i].setBuffer((void*) const_cast<char *>(BUFFERS[i].d_sndBuf),
                BUFFERS[i].d_sndLen);
        oVec[i].setBuffer(BUFFERS[i].d_sndBuf, BUFFERS[i].d_sndLen);
    }
    ioVec[5].setBuffer(str2, static_cast<int>(strlen(str2)));
    oVec[5].setBuffer(str2, static_cast<int>(strlen(str2)));
    ioVec[6].setBuffer(str, static_cast<int>(strlen(str)));
    oVec[6].setBuffer(str, static_cast<int>(strlen(str)));
#ifdef BSLS_PLATFORM_OS_LINUX
    memset(str3,'8', sizeof(str3));
    str3[sizeof(str3) - 1] = '\0';
    ioVec[7].setBuffer(str3, static_cast<int>(strlen(str3)));
    oVec[7].setBuffer(str3, static_cast<int>(strlen(str3)));
#endif

}

static void helpAssertVecData(int         i,
                              int         j,
                              int         type,
                              void       *vectorBuffer,
                              const char *expData)
{
    enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

    ASSERT(vectorBuffer);

    if (vectorBuffer)
    {
        switch (type) {
          case e_IOVECTOR: {
              btls::Iovec *vec = (btls::Iovec*)vectorBuffer;
              int idx = 0;
              int len = static_cast<int>(strlen((char*)vec[idx].buffer()));

              while (len) {
                  if (veryVerbose) {
                      P_(i);  P_(j); P_(len);
                      P((char*)vec[idx].buffer());
                  }
                  LOOP2_ASSERT(i, j, 0 ==
                      strncmp((char*)vec[idx].buffer(), expData, len));

                  ++idx;
                  expData += len;
                  len = static_cast<int>(strlen((char*)vec[idx].buffer()));
              }
          } break;
          default:
             ASSERT("Wrong vector type" && 0);
             break;
        }
    }
}

static void bufferedReadCallback(const char                  *buf,
                                 int                          status,
                                 int                          augStatus,
                                 btlso::TcpTimerEventManager *eveManager,
                                 btlsos::TcpTimedCbChannel   *channel,
                                 int                          expStatus,
                                 int                          expAugStatus,
                                 const char                  *script,
                                 Buffer                      *buffer)
    // Callback function for a buffered read request to indicate the execution
    // of a read request and to notify the user if it succeeds, partially
    // completes, fails or needs to issue other requests.
{
    ASSERT(buffer);
    LOOP2_ASSERT(expStatus, status, expStatus == status);
    LOOP2_ASSERT(augStatus, expAugStatus, augStatus == expAugStatus);
    if (veryVerbose) {
        Q("BUFFERED READ CALLBACK");
        P_(status); P(expStatus);
        P_(augStatus); P(expAugStatus);
    }

    if (expStatus == status) {
        // Read successfully: save data from the channel buffer.
        if (buf) {
            memcpy(buffer->d_readBuf, buf, status);
            buffer->d_readBuf[status] = '\0';
            if (veryVerbose) {
                P(buffer->d_readBuf);
            }
        }
    }
    // Partial result; note 'augStatus' cannot be 0
    else if (0 <= status) {
        if (0 == status) {
            ASSERT(augStatus < 0);
            // Handle read operation being dequeued due to a partial result in
            // some preceding enqueued "read" operation.
            if (veryVerbose) {
                cout << "This request is dequeued!" << endl;
            }
        }
        else if (augStatus > 0) {
            // Handle partial read was due to asynchronous event here.
            if (veryVerbose) {
                cout << "The request is interrupted ." << endl;
            }
        }
        else { // The augStatus will not be 0.
            ASSERT(status > 0 && augStatus < 0);

        }
    }
    else if (-1 == status) {
        // Handle (known) close-connection-by-peer condition here.
        if (veryVerbose) {
            cout << "The channel has been closed, no more i/o operations!"
                 << endl;
        }
    }
    else {
        // Handle unknown error condition here.
        ASSERT(channel->isInvalidRead());
        ASSERT(-1 > status);

        if (veryVerbose) {
            cout << "Error on the channel " << status << endl;
        }
    }
    // Reinstall the read operation here.
    if (script) {
        // Load it used to retry.

        const char *oneScript = script;
        // Load it used to retry.

        while (oneScript) {
            gg(channel, buffer, eveManager, 0, oneScript);
            oneScript=getNextCbCommand(oneScript);
        }
    }
}

static void readCallback(int                          status,
                         int                          augStatus,
                         btlso::TcpTimerEventManager *eveManager,
                         btlsos::TcpTimedCbChannel   *channel,
                         int                          expStatus,
                         int                          expAugStatus,
                         const char                  *script,
                         Buffer                      *buffer)
    // Callback function for a non-buffered read request to indicate the
    // execution of a read request and to notify the user if it succeeds,
    // partially completes, fails or needs to issue other requests.

{
    ASSERT(expStatus == status);
    ASSERT(augStatus == expAugStatus);
    if (veryVerbose) {
        Q("READ CALLBACK");
        P_(status); P(expStatus);
        P_(augStatus); P(expAugStatus);
    }

    if (0 <= status) {
        if (0 == status) {
            if (0 == augStatus) {
                if (veryVerbose) {
                    cout << "TIMEOUT for this request!" << endl;
                }
            }
            else {
            // Handle read operation being dequeued due to a partial result in
            // some preceding enqueued "read" operation.
                if (veryVerbose) {
                    cout << "This request is dequeued!" << endl;
                }
            }
        }
        else if (augStatus > 0) {
            // Handle partial read was due to asynchronous event here.
            if (veryVerbose) {
                cout << "The request is interrupted ." << endl;
            }
        }
        else if (expStatus != status) { // The augStatus will not be 0.
            //ASSERT(status > 0 && augStatus < 0);
            // Handle partial read was due to OS-level operation here.
            if (veryVerbose) {
                cout << "This is a partial read operation." << endl;
            }
        }
    }
    else if (-1 == status) {
        // Handle (known) close-connection-by-peer condition here.
        if (veryVerbose) {
            cout << "The channel has been closed, no more i/o operations!"
                 << endl;
        }
    }
    else {
        // Handle unknown error condition here.
        if (veryVerbose) {
            cout << "Unknown error happened to the channel! " << endl
                 << "no more i/o operations!" << endl;
        }
    }
    // Issue other requests here.
    if (script) {
        const char *oneScript = script;
        // Load it used to retry.

        while (oneScript) {
            gg(channel, buffer, eveManager, 0, oneScript);
            oneScript = getNextCbCommand(oneScript);
        }
    }
}

// The following callback function is used in the "Usage Example" test, it was
// intentionally simplified so it's easy to read and understand.
static void myReadCallback(int status,
                           int augStatus,
                           int expStatus,
                           int expAugStatus)
    // Callback function for a non-buffered read request to indicate the
    // execution of a read request and to notify the user if it succeeds,
    // partially completes, fails or needs to issue other requests.

{
    ASSERT(expStatus == status);
    ASSERT(augStatus == expAugStatus);
    if (veryVerbose) {
        P_(status); P(augStatus);
        P_(expStatus); P(expAugStatus);
    }

    if (0 <= status) {
        if (0 == status) {
            ASSERT(augStatus < 0);
            // Handle read operation being dequeued due to a partial result in
            // some preceding enqueued "read" operation.
            cout << "This request is dequeued!" << endl;
        }
        else if (augStatus > 0) {
            // Handle partial read was due to asynchronous event here.
            cout << "The request is interrupted ." << endl;
        }
        else if (expStatus != status) { // The augStatus will not be 0.
            //ASSERT(status > 0 && augStatus < 0);
            // Handle partial read was due to OS-level operation here.
            cout << "This is a partial read operation." << endl;
        }
    }
    else if (-1 == status) {
        // Handle (known) close-connection-by-peer condition here.
        cout << "The channel has been closed, no more i/o operations!"
             << endl;
    }
    else {
        // Handle unknown error condition here.
        cout << "Unknown error happened to the channel! " << endl
             << "no more i/o operations!" << endl;
    }
}

static void readvCallback(int                          status,
                          int                          augStatus,
                          btlso::TcpTimerEventManager *eveManager,
                          btlsos::TcpTimedCbChannel   *channel,
                          int                          expStatus,
                          int                          expAugStatus,
                          const char                  *script,
                          Buffer                      *buffer)
    // Callback function for a non-buffered 'readv' request to indicate the
    // execution of a read request and to notify the user if it succeeds,
    // partially completes, fails or needs to issue other request.
{

    ASSERT(expStatus == status);
    ASSERT(augStatus == expAugStatus);
    if (veryVerbose) {
        P_(status); P(augStatus);
        P_(expStatus); P(expAugStatus);
    }

    if (expStatus == status) {
        if (veryVerbose) {
            cout << "The read is successful and complete." << endl;
        }
    }
    // Partial result; note 'augStatus' cannot be 0--it's for "timed" case.
    else if (0 <= status) {
        if (0 == status) {
            ASSERT(augStatus < 0);
            // Handle read operation being dequeued due to a partial result in
            // some preceding enqueued "read" operation.
            if (veryVerbose) {
                cout << "This request is dequeued!" << endl;
            }
        }
        else if (augStatus > 0) {
            // Handle partial read was due to asynchronous event here.
            if (veryVerbose) {
                cout << "The request is interrupted ." << endl;
            }
        }
        else { // The augStatus will not be 0.
            ASSERT(status > 0 && augStatus < 0);
            // Handle partial read was due to OS-level operation here.
        }
    }
    else if (-1 == status) {
        // Handle (known) close-connection-by-peer condition here.
        if (veryVerbose) {
            cout << "The channel has been closed, no more i/o operations!"
                 << endl;
        }
    }
    else {
        // Handle unknown error condition here.
        if (veryVerbose) {
            cout << "Unknown error happened to the channel! " << endl
                 << "no more i/o operations!" << endl;
        }
    }
    // Issue other requests here.
    if (script) {
        const char *oneScript = script;

        while (oneScript) {
            gg(channel, buffer, eveManager, 0, oneScript);
            oneScript=getNextCbCommand(oneScript);
        }
    }
}

static void writeCallback(int                          status,
                          int                          augStatus,
                          btlso::TcpTimerEventManager *eveManager,
                          btlsos::TcpTimedCbChannel   *channel,
                          int                          expStatus,
                          int                          expAugStatus,
                          const char                  *script,
                          Buffer                      *buffer)
    // Callback function for a write request to indicate the execution of a
    // write request and to notify the user if it succeeds, partially
    // completes, fails or needs to issue other requests.
{
    ASSERT(expStatus == status);
    ASSERT(augStatus == expAugStatus);
    if (expStatus != status)
        cout << "status: " << status << "; expStatus: " << expStatus << endl;
    if (veryVerbose) {
        Q("WRITE CALLBACK");
        P_(status);   P(expStatus);
        P_(augStatus); P(expAugStatus);

    }
    if (expStatus == status) {
        if (veryVerbose) {
            cout << "The status and expStatus matched." << endl;
        }
    }
    else if (0 <= status) { // PARTIAL RESULTS ARE NOT AUTHORIZED BELOW
        if (0 < augStatus) {
            if (veryVerbose) {
                cout << "The request is interrupted ." << endl;
            }
        }
        else {
            ASSERT(augStatus < 0);
            if (0 == status) {
                // this means deqeue() happened.
                if (veryVerbose) {
                    cout << "Write operation dequeued due to partial write "
                         << "in some preceding enqueued operation." << endl;
                }
            }
            else {
                ASSERT(status > 0);
                if (veryVerbose) {
                    cout << "Write (efficiently) transmitted " << status
                         << " of " << status << "bytes." << endl;
                }
            }
        }
    }
    else if (-1 == status) {
        if (veryVerbose) {
            cout << "Write failed: connection was closed by peer." << endl;
        }
    }
    else {
        ASSERT(status < -1);
        if (veryVerbose) {
            cout << "Write failed: the reason is unknown." << endl;
        }
    }
    // Issue other requests here.
    if (script) {
        const char *oneScript = script;

        while (oneScript) {
            gg(channel, buffer, 0, eveManager, oneScript);
            oneScript=getNextCbCommand(oneScript);
        }
    }
}

//=============================================================================
//              GENERATOR FUNCTION 'gg' FOR TESTING
//-----------------------------------------------------------------------------
//..
// LANGUAGE SPECIFICATION:
// -----------------------
// <SCRIPT>   - the script string, which includes a list of "commands" to be
//              executed
// <commands> - a list of commands, each of which is separated from other
//              commands with a ';' and could be executed independently
// <cmd>      - a single command, including a command code, optionally
//              auxiliary commands 'auxList', one or three digital
//              values, 'cbCommand' which is enclosed in a pair of '{'
//              and '}'.
// <auxList>  - auxiliary commands, e.g., the 'b' in "rb" is an auxiliary
//              command to specify it's a buffered read command; or 'v' in
//              "rv" to specify it's a readv command.
// <cmdCode>  - the main command, such as 'r' for read, 'w' for write etc.
//
// LANGUAGE DEFINITION:
// --------------------
// <SCRIPT>   := <commands>
//
// <commands> := <cmd> | <cmd>;<commands> | EMPTY
//
// <cmd>      := <cmdCode>[<auxList>][<n1>[,'('<n2>,<n3>')'][,<n4>,<n5>]]
//                        ['{'<commands>'}']
//
// <auxList>  := <auxCmd> | <auxCmd> <auxList>
//
// <cmdCode>  := 'r' | 'w' | 'd' | 'c'
//
// <auxCmd>   := 'b' | 't' | 'r' | 'v' | 'a' | 'w' | EMPTY
//
// EMPTY      :=
//..
//
//-----------------------------------------------------------------------------
// The following interprets a given 'script' in the order from left to right to
// configure a list of I/O request commands for a channel according to the
// custom language defined above.  By parsing a request script that is based on
// this language, each I/O request command, which is separated with a ';' from
// other commands, will be executed and the result can then be verified.
// Commands enclosed in a pair of '{', '}' can be executed only in the
// user-installed callback function.
//..
//   s     - sleep the specified amount of time, to test timed I/O requests.
//   d     - dispatch,
//   dr    - read dispatch,
//   dw    - write dispatch,
//   r     - read,
//   rb    - buffered read,
//   rbt   - timed buffered read,
//   rbr   - raw buffered read,
//   rbrt  - timed raw buffered read,
//   rt    - timed read,
//   rv    - readv,
//   rvt   - timed readv,
//   rvr   - raw readv,
//   rvrt  - timed raw readv,
//   rr    - raw read,
//   rrt   - timed raw read,
//   w     - write,
//   wt    - timed write,
//   wb    - buffered write,
//   wbt   - timed buffered write,
//   wbv   - buffered writev,
//   wbvi  - buffered writev by using btls::Iovec buffer,
//   wbvo  - buffered writev by using btls::Ovec buffer,
//   wbvt  - timed buffered writev,
//   wbvit - timed buffered writev by using btls::Iovec buffer,
//   wbvot - timed buffered writev by using btls::Ovec buffer,
//   wr    - raw write,
//   wrt   - timed raw write,
//   wv    - writev,
//   wvi   - writev by using btls::Iovec buffer,
//   wvo   - writev by using btls::Ovec buffer,
//   wvit  - timed writev by using btls::Iovec buffer,
//   wvot  - timed writev by using btls::Ovec buffer,
//   wvir  - raw writev by using btls::Iovec buffer,
//   wvor  - raw writev by using btls::Ovec buffer,
//   wvirt - timed raw writev by using btls::Iovec buffer,
//   wvort - timed raw writev by using btls::Ovec buffer,
//   cR    - close the channel's receive half connection,
//   cS    - close the channel's send half connection,
//   ca    - cancel all(read and write),
//   cr    - cancel read,
//   cw    - cancel write,
//   ia    - invalidate all (both read and write options for the channel).
//   ir    - invalidate the read portion of the channel,
//   iw    - invalidate the write portion of the channel,
//   R     - read from the control buffer written by the channel to control
//           the buffer state, it's not a channel operation command,
//   W     - write to the control buffer for the channel to control the buffer
//           state, it's not a channel operation command,
//..
//
// Note that any command that is enclosed in a pair of '{' and '}' should be
//   executed in the user-installed callback function.  This kind of commands
//   could be any one above except the dispatch command 'd'.
// Digital values in a command:
//   For digital values following a read or write command, e.g., "r2,1,2,0",
//     or "w15,1,15,0", the first value indicates the number of bytes to be
//     read/written.  The second value is the interruptible flag, non-zero
//     means the flag is on and so "asynchronous events" are permitted to
//     interrupt the channel operations; while 0 means the flag is off.  The
//     third and fourth integers indicate values of the expected 'status' and
//     'augStatus' respectively.
//     If there are a pair of values that are enclosed by '(' and ')' in a
//     command, this pair of values indicate the timeout value starting form
//     bdlt::CurrentTime::now(), where the first one in the pair means the
//     number of milliseconds, and the second one means the number of
//     nanoseconds respectively.  They should follow the first digital value
//     that indicates the number of bytes to be read/written.  The other
//     digital values are in the same order as when there's no timeout
//     values in the command.  For example, if there's a timeout requirement
//     for the above command, it should be "r2,(3,4),1,2,0", where the
//     meaning of other values is the same as without the timeout values.
//
//   For a digital value following a scatter/gather I/O command, e.g.,
//     "wvi3,1,45,0", "wbvi6,1,45,0" the digital value right following a char
//     (here it is 3, 6 respectively) indicates the number of buffers
//     expected to be read/written.  All the other digital values are the
//     same as the above.
//
//   For a digital value following a dispatch command ('dr' or 'dw'), e.g.,
//     "dr3", the value (here it is 3) indicates the number of callbacks
//     expected to be dispatched by executing this dispatch request.
//
//   For a digital value following a 'R' or 'W', e.g., "R10" or "w5", the
//     value means the number of bytes to be read/written to the control pipe
//     to help test any read/write function of the channel.
//
//   For a digital value following a 's', e.g., "s100", the value means the
//     number of milliseconds for the process to sleep.  This is to help
//     test the timed I/O requests.
//
// Script examples based on the above language are as follows:
//
// "r2,0,2,0;dr1" means:
//    -- Read 2 bytes with the interruptible flag off and without a
//       timeout requirement; then in next command the read-event manager
//       dispatches.
//
// "rb3,1,3,0,{r5,5,0};dr1" means:
//    -- Buffered read 3 bytes, with the interruptible flag on and without a
//       timeout requirement, also in this command issue another "read 5
//       bytes" request from the user-installed callback function.  In the next
//       command read-event manager dispatches.
//
// "w1,0,1,0,{rw3,3,0;rvr2,2,0;cr}" means:
//    -- Write 1 byte with the interruptible flag off and without a timeout
//       requirement, from inside this request's callback, do the following:
//       raw read 3 bytes, raw readv 2 bytes; then cancel all read requests.
//
// "rbr4,0,4,0;dw2" means:
//    -- Buffered raw read 4 bytes, with the interruptible flag off and
//       without a timeout requirement; then the write-event manager
//       dispatches.
//
// "rbr4,(2,3),1,4,0" means:
//    -- Buffered raw read 4 bytes, the read request also sets a timeout value
//       (2, 3) and the interruptible flag is on.
//
// "W3" means:
//    -- A helper 'write' into the control buffer outside the channel to
//       change the buffer state, the data to be written is from entry 3
//       from a predesigned table.  This table consists of a list of strings
//       from which any single entry can be chosen to write to the control
//       buffer.
//

static int gg(btlsos::TcpTimedCbChannel   *channel,
              Buffer                      *buffer,
              btlso::TcpTimerEventManager *rManager,
              btlso::TcpTimerEventManager *wManager,
              const char                  *script)
{
    // Decode the script based on the above language, execute one
    //   command in the script at a time.
    // Parameter: buffer : should be big enough for reading or writing.
    //                       the specific type for buffer is different for
    //                       different commands.
    //             rManager: a read event manager to dispatch readCallbacks.
    //             wManager: a write event manager to dispatch writeCallbacks.
    // The helper function getCbCommand() to get the command to be executed
    //   in the user-installed callback function, which is enclosed in '{'
    //   and '}'.
    // Return value:
    //         -1 -- this function failed for the latest command
    //          0 -- success for any request about a channel
    //        > 0 -- the request entry number in specified table.  This
    //               request is a "helper" command, either to write to the
    //               control buffer for the testing channel's read functions,
    //               or to read from the control buffer for the testing
    //               channel's write functions.

    int ret = 0, ret_flag = 0;
    if (0 == script || '\0' == *script)  // no command passed in
        return ret_flag;                                              // RETURN
    const char *callBackScript = get1stCbCommand(script);

    switch (*script) {
      case 'R':
          ret = sscanf(script, "R%d", &ret_flag);
          ASSERT(1 == ret);    // Return 1 numbers.
          break;
      case 'W':
          ret = sscanf(script, "W%d", &ret_flag);
          ASSERT(1 == ret);    // Return the length to be written numbers.
          break;
      case 's': {
          int milliSeconds = 0;
          ret = sscanf(script, "s%d", &milliSeconds);
          ASSERT(1 == ret);    // Return the length to be written numbers.
          #ifdef BSLS_PLATFORM_OS_UNIX // unix systems
              usleep(milliSeconds * 1000);
          #else                         // windows
              Sleep(milliSeconds);
          #endif
      } break;
      case 'c': {              // cancel or close
        switch (*(script+1)) {
          case 'a':
                               // cancel all
              channel->cancelAll();
              break;
          case 'r':
                               // cancel read
              channel->cancelRead();
              break;
          case 'w':
                               // cancel write
              channel->cancelWrite();
              break;
          case 'R':
                               // close receive
              ret = channel->socket()->shutdown(
                                              btlso::Flag::e_SHUTDOWN_RECEIVE);
              ASSERT(0 == ret);
              break;
          case 'S':
                               // close receive
              ret = channel->socket()->shutdown(btlso::Flag::e_SHUTDOWN_SEND);
              ASSERT(0 == ret);

              break;
          default:
                               // invalid close command
              ASSERT("invalid close command" && 0);
              break;
        }
      } break;
      case 'i': {              // invalidate
        switch (*(script+1)) {
          case 'a':
                               // invalidate all
              channel->invalidate();
              break;
          case 'r':
                               // invalidate read options
              channel->invalidateRead();
              break;
          case 'w':
                              // invalidate write options
              channel->invalidateWrite();
              break;
          default:

              ASSERT("invalid invalidate command" && 0);
              break;
        }
      } break;
      case 'r': {
          int readLen, optFlag = 0, milliSec = 0, nanoSec = 0,
              expStatus, expAugStatus;
          bsl::function<void(int, int)> callback;

          switch (*(script+1)) {
            case 'b': {
                btlsc::CbChannel::BufferedReadCallback callback;
                if ('r' == *(script+2)) {
                    if ('t' == *(script+3)) {    // timedBufferedReadRaw()
                        ret = sscanf(script, "rbrt%d,(%d,%d),%d,%d,%d",
                                     &readLen, &milliSec, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                        ASSERT(6 == ret);        // Return 6 numbers.
                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                        &bufferedReadCallback,
                                        _1, _2, _3,
                                        rManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);

                        ret = channel->timedBufferedReadRaw(readLen, timer,
                                                            callback, optFlag);
                        ASSERT(0 == ret || channel->isInvalidRead());
                    }
                    else {                     // bufferedReadRaw()
                        ret = sscanf(script, "rbr%d,%d,%d,%d", &readLen,
                                     &optFlag, &expStatus, &expAugStatus);

                        ASSERT(4 == ret);      // Return 4 numbers.

                        callback = bdlf::BindUtil::bind(
                                        &bufferedReadCallback,
                                        _1, _2, _3,
                                        rManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);

                        ret = channel->bufferedReadRaw(readLen, callback,
                                                               optFlag);
                        ASSERT(0 == ret || channel->isInvalidRead());
                    }
                }
                else {
                    btlsc::CbChannel::BufferedReadCallback callback;

                    if ('t' == *(script+2)) {    // 'rbt'
                        ret = sscanf(script, "rbt%d,(%d,%d),%d,%d,%d",
                                     &readLen, &milliSec, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                        ASSERT(6 == ret);        // Return 6 numbers.
                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                        &bufferedReadCallback,
                                        _1, _2, _3,
                                        rManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);

                        ret = channel->timedBufferedRead(readLen, timer,
                                                         callback, optFlag);
                        ASSERT(0 == ret || channel->isInvalidRead());
                    }
                    else {
                        // bufferedRead()
                        ret = sscanf(script, "rb%d,%d,%d,%d", &readLen,
                                     &optFlag, &expStatus, &expAugStatus);

                        ASSERT(4 == ret);   // Return 4 numbers.
                        btlsc::CbChannel::BufferedReadCallback callback;
                        callback = bdlf::BindUtil::bind(
                                        &bufferedReadCallback,
                                        _1, _2, _3,
                                        rManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);

                        ret =
                             channel->bufferedRead(readLen, callback, optFlag);
                    }
                    ASSERT(0 == ret || channel->isInvalidRead());
                }
            } break;
            case 'r': {
                if ('t' == *(script+2)) {   // timedReadRaw()
                    ret = sscanf(script, "rrt%d,(%d,%d),%d,%d,%d", &readLen,
                                 &milliSec, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(6 == ret);

                    bsls::TimeInterval timer = bdlt::CurrentTime::now();
                    timer.addMilliseconds(milliSec);
                    timer.addNanoseconds(nanoSec);

                    callback = bdlf::BindUtil::bind(
                                    &readCallback,
                                    _1, _2,
                                    rManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                    ret = channel->timedReadRaw(buffer->d_readBuf, readLen,
                                                timer, callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidRead());
                }
                else {                      // readRaw()
                    ret = sscanf(script, "rr%d,%d,%d,%d", &readLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);
                    callback = bdlf::BindUtil::bind(
                                    &readCallback,
                                    _1, _2,
                                    rManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                    ret = channel->readRaw(buffer->d_readBuf, readLen,
                                           callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidRead());
                }
            }  break;
            case 'v': {
                int numBuf = 0;
                if ('r' == *(script+2)) {
                       if ('t' == *(script+3)) { // timedReadvRaw()
                           ret = sscanf(script, "rvrt%d,(%d,%d),%d,%d,%d",
                                     &numBuf, &milliSec, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                           ASSERT(6 == ret);     // Return 6 numbers.
                           btls::Iovec *buf = buffer->d_readIovecBuf;

                           bsls::TimeInterval timer = bdlt::CurrentTime::now();
                           timer.addMilliseconds(milliSec);
                           timer.addNanoseconds(nanoSec);

                           callback = bdlf::BindUtil::bind(
                                           &readvCallback,
                                           _1, _2,
                                           rManager,
                                           channel,
                                           expStatus,
                                           expAugStatus,
                                           callBackScript,
                                           buffer);

                           ret = channel->timedReadvRaw(buf, numBuf, timer,
                                                        callback, optFlag);
                           ASSERT(0 == ret || channel->isInvalidRead());
                       }
                       else {                      // readvRaw()
                           ret = sscanf(script, "rvr%d,%d,%d,%d", &numBuf,
                                        &optFlag, &expStatus, &expAugStatus);
                           ASSERT(4 == ret);       // Return 4 numbers.
                           btls::Iovec *buf = buffer->d_readIovecBuf;

                           callback = bdlf::BindUtil::bind(
                                           &readvCallback,
                                           _1, _2,
                                           rManager,
                                           channel,
                                           expStatus,
                                           expAugStatus,
                                           callBackScript,
                                           buffer);

                           ret = channel->readvRaw(buf, numBuf,
                                                   callback, optFlag);

                           ASSERT(0 == ret || channel->isInvalidRead());
                       }
                     }
                     else {
                         // readv()
                         ret = sscanf(script, "rv%d,%d,%d,%d", &numBuf,
                                 &optFlag, &expStatus, &expAugStatus);
                         ASSERT(4 == ret);         // Return 4 numbers.
                         btls::Iovec *buf = buffer->d_readIovecBuf;
                         callback = bdlf::BindUtil::bind(
                                         &readvCallback,
                                         _1, _2,
                                         rManager,
                                         channel,
                                         expStatus,
                                         expAugStatus,
                                         callBackScript,
                                         buffer);
                         ret = channel->readv(buf, numBuf, callback, optFlag);
                         ASSERT(0 == ret || channel->isInvalidRead());
                     }
            } break;
            default: {
                if ('t' == *(script+1)) {
                    // timedRead()
                    ret = sscanf(script, "rt%d,(%d,%d),%d,%d,%d", &readLen,
                                 &milliSec, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(6 == ret);        // Return 6 numbers.
                    bsls::TimeInterval timer = bdlt::CurrentTime::now();
                    timer.addMilliseconds(milliSec);
                    timer.addNanoseconds(nanoSec);

                    callback = bdlf::BindUtil::bind(
                                    &readCallback,
                                    _1, _2,
                                    rManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                    ret = channel->timedRead(buffer->d_readBuf, readLen,
                                             timer, callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidRead());

                }
                else if (isdigit(*(script+1))) { // read()
                    // read()
                    ret = sscanf(script, "r%d,%d,%d,%d", &readLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);            // Return 4 numbers.

                    callback = bdlf::BindUtil::bind(
                                    &readCallback,
                                    _1, _2,
                                    rManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                    ret = channel->read(buffer->d_readBuf, readLen,
                                                       callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidRead());
                }
                else {
                    ret_flag = -1;
                    ASSERT ("Parse error!" && 0);
                }
            } break;
          }
      } break;
      case 'w': {
          int writeLen = 0, optFlag = 0, milliSec = 0, nanoSec = 0,
              expStatus, expAugStatus;
          btlsc::CbChannel::WriteCallback callback;
          switch (*(script+1)) {
            case 'r':
                if ('t' == *(script+2)) {     // timedWriteRaw()
                    ret = sscanf(script, "wrt%d,(%d,%d),%d,%d,%d", &writeLen,
                                 &milliSec, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(6 == ret);         // Return 6 numbers.

                    bsls::TimeInterval timer = bdlt::CurrentTime::now();
                    timer.addMilliseconds(milliSec);
                    timer.addNanoseconds(nanoSec);

                    callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                    ret = channel->timedWriteRaw(buffer->d_writeBuf,
                                                 writeLen, timer,
                                                 callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidWrite());
                }
                else {                        // writeRaw()
                    ret = sscanf(script, "wr%d,%d,%d,%d",
                                 &writeLen, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(4 == ret);         // Return 4 numbers.
                    callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                    ret = channel->writeRaw(buffer->d_writeBuf,
                                            writeLen,
                                            callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidWrite());
                }
                break;
            case 'v': {
                char ch;
                int numBuf = 0;
                if ('r' == *(script+3)) {
                    if ('t' == *(script+4) ) { // timed raw writev
                        ret = sscanf(script, "wv%crt%d,(%d,%d),%d,%d,%d",
                                     &ch, &numBuf, &milliSec, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.
                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                        if ('i' == ch) {
                            ret = channel->timedWritevRaw(
                                                      buffer->d_writeIovecBuf,
                                                      writeLen, timer,
                                                      callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->timedWritevRaw(
                                                      buffer->d_writeOvecBuf,
                                                      writeLen, timer,
                                                      callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                    else {                     // raw writev
                        ret = sscanf(script, "wv%cr%d,%d,%d,%d",
                                 &ch, &numBuf, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.
                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                        if ('i' == ch) {
                            ret = channel->writevRaw(buffer->d_writeIovecBuf,
                                                     numBuf,
                                                     callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->writevRaw(buffer->d_writeOvecBuf,
                                                     numBuf,
                                                     callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());

                    }
                }
                else {
                    if ('t' == *(script+3) ) { // timed writev
                        ret = sscanf(script, "wv%ct%d,(%d,%d),%d,%d,%d",
                                     &ch, &writeLen, &milliSec, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.

                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                        if ('i' == ch) {
                            ret = channel->timedWritev(buffer->d_writeIovecBuf,
                                                       writeLen, timer,
                                                       callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->timedWritev(buffer->d_writeOvecBuf,
                                                       writeLen, timer,
                                                       callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                    else {                     // writev()
                        ret = sscanf(script, "wv%c%d,%d,%d,%d",
                                 &ch, &writeLen, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.

                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                        if ('i' == ch) {
                            ret = channel->writev(buffer->d_writeIovecBuf,
                                                  writeLen,
                                                  callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->writev(buffer->d_writeOvecBuf,
                                                  writeLen,
                                                  callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                }
            } break;
            case 'b': {
                char ch;
                int numBuf = 0;
                if ('v' == *(script+2)) {
                    if ('t' == *(script+4)) {
                        // timedBufferedWritev()
                        ret = sscanf(script, "wbv%ct%d,(%d,%d),%d,%d,%d",
                                     &ch, &numBuf, &milliSec, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.

                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);

                        if ('i' == ch) {
                            ret = channel->timedBufferedWritev(
                                                     buffer->d_writeIovecBuf,
                                                     numBuf, timer,
                                                     callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->timedBufferedWritev(
                                                   buffer->d_writeOvecBuf,
                                                   numBuf, timer,
                                                   callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                    else {                     // bufferedWritev()
                        ret = sscanf(script, "wbv%c%d,%d,%d,%d",
                                 &ch, &numBuf, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.
                        callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                        if ('i' == ch) {
                            ret = channel->bufferedWritev(
                                                     buffer->d_writeIovecBuf,
                                                     numBuf,
                                                     callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->bufferedWritev(
                                                   buffer->d_writeOvecBuf,
                                                   numBuf,
                                                   callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                }
                else {
                    if ('t' == *(script+2)) {     // timedBufferedWrite()
                        ret = sscanf(script, "wbt%d,(%d,%d),%d,%d,%d",
                                     &writeLen, &milliSec, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                        ASSERT(6 == ret);         // Return 6 numbers

                        bsls::TimeInterval timer = bdlt::CurrentTime::now();
                        timer.addMilliseconds(milliSec);
                        timer.addNanoseconds(nanoSec);

                        callback = bdlf::BindUtil::bind(
                                        &writeCallback,
                                        _1, _2,
                                        wManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);
                        ret = channel->timedBufferedWrite(buffer->d_writeBuf,
                                                          writeLen, timer,
                                                          callback, optFlag);
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                    else {
                        ret = sscanf(script, "wb%d,%d,%d,%d",
                                     &writeLen, &optFlag, &expStatus,
                                     &expAugStatus);
                        ASSERT(4 == ret);       // Return 4 numbers.
                        callback = bdlf::BindUtil::bind(
                                        &writeCallback,
                                        _1, _2,
                                        wManager,
                                        channel,
                                        expStatus,
                                        expAugStatus,
                                        callBackScript,
                                        buffer);
                        ret = channel->bufferedWrite(buffer->d_writeBuf,
                                                 writeLen,
                                                 callback, optFlag);
                        ASSERT(0 == ret || channel->isInvalidWrite());
                    }
                }
                break;
            }
            default:
                if ('t' == *(script+1)) {         // timedWrite()
                    ret = sscanf(script, "wt%d,(%d,%d),%d,%d,%d",
                                     &writeLen, &milliSec, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                    ASSERT(6 == ret);             // Return 6 numbers

                    bsls::TimeInterval timer = bdlt::CurrentTime::now();
                    timer.addMilliseconds(milliSec);
                    timer.addNanoseconds(nanoSec);

                    callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                    ret = channel->timedWrite(buffer->d_writeBuf, writeLen,
                                              timer, callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidWrite());
                }
                else if (isdigit(*(script+1))) {  // write()
                    ret = sscanf(script, "w%d,%d,%d,%d", &writeLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);             // Return 4 numbers.

                    callback = bdlf::BindUtil::bind(
                                    &writeCallback,
                                    _1, _2,
                                    wManager,
                                    channel,
                                    expStatus,
                                    expAugStatus,
                                    callBackScript,
                                    buffer);
                    ret = channel->write(buffer->d_writeBuf, writeLen,
                                         callback,
                                         optFlag);
                    ASSERT(0 == ret || channel->isInvalidWrite());
                }
                break;
          }
      } break;
      case 'd': {
          int expRet = 0;

          ret = sscanf(script+2, "%d", &expRet); // script as "dr1" or dw1"
          ASSERT(1 == ret);

          switch (*(script+1)) {
            case 'r':
                ret = rManager->dispatch(0);
                ASSERT(expRet == ret || channel->isInvalidRead());
                if (veryVerbose) {
                    cout << "rManager ret: " << ret << ", isinvalid: "
                         << channel->isInvalidRead() << endl;
                }
                break;
            case 'w':
                ret = wManager->dispatch(0);
                ASSERT(expRet == ret || channel->isInvalidWrite());
                if (veryVerbose) {
                    cout << " wManager ret: " << ret << " isinvalid: "
                         << channel->isInvalidWrite() << endl;
                }
                break;
            default:
                ret_flag = -1;
                ASSERT ("parse dispatch error" && 0);
                break;
          }
      } break;
      default: {
          ret_flag = -1;
          ASSERT ("Parse error!" && 0);
      } break;
    }
    return ret_flag;
}
//-----------------------------------------------------------------------------
static inline
void adjustBufferSizes(btlso::SocketHandle::Handle sockFd,
                       int                         bufferSize,
                       int                         watermark)
    // Set the send and receive buffer sizes and the watermarks on the
    // specified 'sockFd' socket handle to the specified 'bufferSize' and
    // 'watermark' values respectively.
{
    int ret;
    ret = btlso::SocketOptUtil::setOption(sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER, bufferSize);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_RECEIVEBUFFER, bufferSize);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(sockFd,
            btlso::SocketOptUtil::k_TCPLEVEL,
            btlso::SocketOptUtil::k_TCPNODELAY, 1);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::setOption(sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            SO_SNDLOWAT, watermark);
#ifdef BSLS_PLATFORM_OS_AIX
    ASSERT(0 == ret);
#endif

    ret = btlso::SocketOptUtil::setOption(sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            SO_RCVLOWAT, watermark);
#ifdef BSLS_PLATFORM_OS_AIX
    ASSERT(0 == ret);
#endif
}

static inline
void printBufferSizes(btlso::SocketHandle::Handle sockFd)
    // Print the buffer sizes and the watermarks of the specified 'sockFd'
    // socket handle.
{
    int sendSize = -1, recvSize = -1, sendWat = -1, recvWat = -1;
    int ret = -1;
    ret = btlso::SocketOptUtil::getOption(&recvSize, sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_RECEIVEBUFFER);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::getOption(&sendSize, sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            btlso::SocketOptUtil::k_SENDBUFFER);
    ASSERT(0 == ret);

    ret = btlso::SocketOptUtil::getOption(&recvWat, sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            SO_RCVLOWAT);

#ifdef BSLS_PLATFORM_OS_AIX
    ASSERT(0 == ret);
#endif

    ret = btlso::SocketOptUtil::getOption(&sendWat, sockFd,
            btlso::SocketOptUtil::k_SOCKETLEVEL,
            SO_SNDLOWAT);

#ifdef BSLS_PLATFORM_OS_AIX
    ASSERT(0 == ret);
#endif
    if (veryVeryVerbose) {
        P_(sendSize); P_(sendWat); P_(recvSize); P(recvWat);
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    int verbose = argc > 2;
    veryVerbose = argc > 3; // global variable
    veryVeryVerbose = argc > 4;  // global variable
    int errCode = 0;

    // TBD: these tests frequently timeout on Windows, disabling until fixed
#ifdef BSLS_PLATFORM_OS_WINDOWS
    testStatus = -1;
#else

#ifdef BSLS_PLATFORM_OS_UNIX
    mySignal(SIGPIPE, sigPipeHandler);    // register a handler for SIGPIPE.
    // A write() on the closed socket will generate SIGPIPE.
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    btlso::SocketImpUtil::startup(&errCode);

    helpBuildVector();       // Help create global vectors: one 'Iovec', one
                             // 'Ovec'.

    switch (test) { case 0:  // Zero is always the leading case
      case 26: {
// TBD FIX ME
          { // to avoid compiler warning until this is fixed:
              bsl::function<void(int, int, int, int)> callback1(
                            bdlf::BindUtil::bind(&myReadCallback, 0, 0, 0, 0));
          }
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // -------------------------------------------------------------------
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
        // -------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        // Firstly, create a socket handle.  It's a pair of socket in our
        // example to implement the test in one process, but the channel only
        // needs one of the socket as its I/O requests' endpoint.

        btlso::SocketHandle::Handle handles[2];
        int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
        ASSERT(0 == ret);
        // The following socket options are set only if necessary.

        ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
        ASSERT(0 == ret);

        ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
        ASSERT(0 == ret);

        // Next, create a 'btlso::StreamSocket' object, which is a part of the
        // channel.  The 'btlso::StreamSocket' object has a field of type
        // 'btlso::SocketHandle::Handle', whose value is set to the socket
        // created above.
        btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
        btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
        ASSERT(sSocket);

        // Client stream socket: it's not a part of the channel, only for the
        // convenience of data reading in a test.
         btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
        ASSERT(cSocket);

        // Then the read and write event managers should be created, by which
        // the channel can dispatch different I/O requests.  The read and write
        // requests can share the same event manager, in that case read and
        // write events will be registered in the same event queue.
        btlso::TcpTimerEventManager rEventManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   wEventManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);
        {
            // We should guarantee that the 'channel's destructor is invoked
            // before the corresponding 'streamSocket' destructor or the
            // behavior is undefined.  We insure the required order by creating
            // the 'channel' inside a block while the corresponding
            // 'streamSocket' object outside the block as above.

            // At last, a channel object is created with the previously created
            // 'btlso::StreamSocket' object, the 'btlso::TcpTimerEventManager'
            // object.

            btlsos::TcpTimedCbChannel channel(sSocket, &rEventManager,
                                             &wEventManager, &testAllocator);

            ASSERT(sSocket == channel.socket());
            ASSERT(0 == channel.isInvalidRead());
            ASSERT(0 == channel.isInvalidWrite());
            ASSERT(0 == channel.numPendingReadOperations());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(&rEventManager == channel.readEventManager());
            ASSERT(&wEventManager == channel.writeEventManager());
            ASSERT(0 == channel.readEventManager()->numEvents());
            ASSERT(0 == channel.writeEventManager()->numEvents());

            // We also need creating a functor object corresponding to the
            // specific I/O request to submit the request.  Associate this
            // functor object with the expected bytest for this I/O request, a
            // function pointer to a user-installed callback function which is
            // usually to report this I/O request's execution status after
            // being dispatched.
            int readLen1 = 5;
            int optFlag1 = 0;
            char readBuf1[20] = "\0";

            bsl::function<void(int, int)> callback1(
                    bdlf::BindUtil::bind(&myReadCallback, readLen1, 0));
            // We also want to set a timeout value which is relative to the
            // current system time.
            bsls::TimeInterval timer1 = bdlt::CurrentTime::now();
            int milliSec1 = 50, nanoSec1 = 400;
            timer1.addMilliseconds(milliSec1);
            timer1.addNanoseconds(nanoSec1);

            // Issue a timed read request:
            ret = channel.timedRead(readBuf1, readLen1, timer1,
                                    callback1, optFlag1);
            ASSERT(0 == ret);
            ASSERT(1 == channel.numPendingReadOperations());
            ASSERT(2 == channel.readEventManager()->numEvents());
            ASSERT(1 == channel.readEventManager()->numTimers());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());
            ASSERT(0 == channel.writeEventManager()->numTimers());
            // Issue another read request:
            int readLen2 = 9;
            int optFlag2 = 0;
            char readBuf2[20] = "\0";
            bsl::function<void(int, int)> callback2(
                    bdlf::BindUtil::bind(&myReadCallback, readLen2, 0));
            // We also want to set a timeout value which is relative to the
            // current system time.
            bsls::TimeInterval timer2 = bdlt::CurrentTime::now();
            int milliSec2 = 50, nanoSec2 = 400;
            timer2.addMilliseconds(milliSec2);
            timer2.addNanoseconds(nanoSec2);

            // Issue another timed read request:

            ret = channel.timedRead(readBuf2, readLen2, timer2,
                                    callback2, optFlag2);
            ASSERT(0 == ret);
            ASSERT(2 == channel.numPendingReadOperations());
            ASSERT(2 == channel.readEventManager()->numEvents());
            ASSERT(1 == channel.readEventManager()->numTimers());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());
            ASSERT(0 == channel.writeEventManager()->numTimers());
            // The client now writes data into its socket for the channel to
            // read.
            char writeBuf[21] = "abcdefghij1234567890";
            int len = btlso::SocketImpUtil::write(handles[1], writeBuf,
                                                 strlen(writeBuf));
            ASSERT(len == (int)strlen(writeBuf));
            // Now the channel dispatches the request.
            ret = ((btlso::TcpTimerEventManager*)
                       channel.readEventManager())->dispatch(0);
            ASSERT(1 == ret);
            ASSERT(1 == channel.numPendingReadOperations());
            ASSERT(2 == channel.readEventManager()->numEvents());
            ASSERT(1 == channel.readEventManager()->numTimers());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());
            ASSERT(0 == channel.writeEventManager()->numTimers());
            if (veryVerbose) {
                cout << "Data read from channel for the first request: "
                     << readBuf1 << endl;
            }
            // Then the channel dispatches the request.
            ret = ((btlso::TcpTimerEventManager*)
                        channel.readEventManager())->dispatch(0);
            ASSERT(1 == ret);
            ASSERT(0 == channel.numPendingReadOperations());
            ASSERT(0 == channel.readEventManager()->numEvents());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());
            if (veryVerbose) {
                cout << "Data read from channel for the second request: "
                     << readBuf2 << endl;
            }
        }
        // Any 'btlso::StreamSocket' objects should be deleted specifically.
        factory.deallocate(sSocket);
        factory.deallocate(cSocket);
#endif
      } break;
      case 25: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING 'timedBufferedWritev(btls::Iovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        //   the specified 'numBuffers' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   Write request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int timedBufferedWritev(btls::Iovec);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedBufferedWritev(btls::Iovec)'"
                          << "\n========================================="
                          << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd               PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----               ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvit5,(255,90),1,40,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(255,10),1,1040,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,100),1,0,-1", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(255,40),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,10),1,0,-1",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvit7,(255,8),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,10),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,5),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(265,100),1,40,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,9),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,10),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,9),1,29760,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,8),1,18,0,{w4,1,4,0; wbvit1,(265,6),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit2,(275,100),1,15,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wbvit7,(255,6),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(260,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(260,100),1,40,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,9),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(260,100),1,18,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,9),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(260,9),1,18,0,{w4,1,4,0; wbvit1,(20,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit2,(260,100),1,15,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wbvit7,(255,6),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(260,10),1,0,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit7,(655,10),1,0,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbt728,(665,20),1,0,-1",  0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbt728,(265,2),1, 728, 0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R55000",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvit5,(255,90),1,40,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(255,10),1,540,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,100),1,0,-1", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(255,40),1,1024,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,10),1,0,-1",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(255,100),1,540,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(265,100),1,-3,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvit7,(255,80),1,1540,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(265,100),1,18,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wbvit7,(255,20),1,1540,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit7,(665,30),1,1540,0",0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R529",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R529",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,15),1,1540,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,18,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(275,100),1,40,0", 0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,10),1,1540,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(265,100),1,18,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(255,9),1,1540,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
{L_, "wbvit3,(265,100),1,18,0,{w4,1,4,0; wbvit1,(275,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit2,(285,100),1,15,0", 0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wbvit7,(255,60),1,1540,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit7,(265,10),1,508,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(265,10),1,0,-1",  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
   {L_, "w1024,1,1024,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit7,(255,10),1,1024,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbt528,(465,20),1,528,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },

   {L_, "R1000",                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 */
 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvit5,(20,90),1,40,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(20,10),1,1040,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,0,-1",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(20,40),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(20,40),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,10),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(20,100),1,1040,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,-3,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /*
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvit7,(20,80),1,74720,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,18,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wbvit7,(5,20),1,74720,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit7,(5,30),1,74720,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },  */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit8,(20,15),1,20478,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit3,(20,100),1,18,0",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(300,100),1,40,0", 0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/*
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(20,10),1,74720,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,18,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 // TBD
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(20,9),1,74720,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_, "wbvit3,(20,100),1,18,0,{w4,1,4,0; wbvit1,(20,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit2,(20,100),1,15,0",  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wbvit7,(20,60),1,73728,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,10),1,0,-1",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch when the pipe is not writeable:
   // timeout. after the request timeout, dispatch another request when
   // writeable.  Flood the buffers
   {L_, "w45372,1,45372,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   // This operation will be partial due to a timeout
   {L_, "wbvit7,(5,10),1,24576,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
   // This operation will be dequeued,
   {L_, "wbt728,(15,20),1, 0, -1", 0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbt728,(15,20),1, 728, 0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R55000",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 */
 #else                                  // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvit5,(20,90),1,40,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(20,10),1,1040,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,0,-1",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(20,40),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,10),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvit6,(20,100),1,1040,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,-3,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /*
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvit7,(20,80),1,74720,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,18,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wbvit7,(5,20),1,74720,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit7,(5,30),1,74720,0", 0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },  */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(20,15),1,74720,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit3,(20,100),1,18,0",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit5,(20,100),1,40,0",  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/*
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(20,10),1,74720,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,100),1,18,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 // TBD
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvit7,(20,9),1,74720,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_, "wbvit3,(20,100),1,18,0,{w4,1,4,0; wbvit1,(20,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit2,(20,100),1,15,0",  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wbvit7,(20,60),1,73728,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wbvit3,(20,10),1,0,-1",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
   // Flood the buffers
   {L_, "w45372,1,45372,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   // This operation will be partial due to a timeout
   {L_, "wbvit7,(5,10),1,24576,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
   // This operation will be dequeued,
   {L_, "wbt728,(15,20),1, 0, -1", 0,   2,    2,    2,    1, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbt728,(15,20),1, 728, 0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R55000",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 */
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            adjustBufferSizes(handles[0], BUF_LIMIT, 1);
            adjustBufferSizes(handles[1], BUF_LIMIT, 1);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(LINE);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 24: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'bufferedWritev(btls::Iovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        //   the specified 'numBuffers' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   Write request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int bufferedWritev(btls::Iovec);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'bufferedWritev(btls::Iovec)'"
                          << "\n====================================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd               PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----               ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvi5,1,40,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,0,-1",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wbvi7,1,29760,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,29760,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvi7,1,29760,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different event types, and dispatch.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w30,1,30,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi5,1,40,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,29760,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,29760,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0,{w4,1,4,0; wbvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi2,1,15,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvi5,1,40,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,540,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,0,-1",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,0,-1",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,540,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,-3,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/* 5 of 1000
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "w500,1,500,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different event types, and dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w30,1,30,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi5,1,40,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,1540,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0,{w4,1,4,0; wbvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi2,1,15,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvi5,1,40,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,540,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,0,-1",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,0,-1",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,540,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,-3,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "w500,1,500,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different event types, and dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w30,1,30,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi5,1,40,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,1540,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,1540,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0,{w4,1,4,0; wbvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi2,1,15,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvi5,1,40,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,0,-1",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wbvi8,1,20478,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,0,-1",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R10000",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,-3,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvi7,1,16383,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,16383,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi8,1,20478,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different event types, and dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w30,1,30,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi5,1,40,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi8,1,20478,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi8,1,20478,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0,{w4,1,4,0; wbvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi2,1,15,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else                                  // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wbvi5,1,40,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,0,-1",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,0,-1",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wbvi6,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,-3,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 request, then dispatch.
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/*
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },  */
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during the first dispatch.
   {L_, "w63728,1,63728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different event types, and dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w30,1,30,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi5,1,40,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0",            0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wbvi7,1,74720,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi3,1,18,0,{w4,1,4,0; wbvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wbvi2,1,15,0",            0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 23: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'timedBufferedWrite()' FUNCTION:
        //   Initiate a non-blocking operation to write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   Write request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int timedBufferedWrite();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedBufferedWrite'"
                          << "\n============================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                     PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                     ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data
 {
 {L_, "wbt1040,(20,100),1,1040,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "iw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wbt27728,(20,100),1,27728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "cw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wbt40,(250,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt15,(250,100),1,15,0",       0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
 {L_, "wbt27728,(200,100),1,27728,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt27728,(200,100),1,0,0",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt27728,(250,100),1,27728,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40,(250,100),1,40,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R22379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt27728,(250,100),1,27728,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt27728,(250,100),1,27728,0", 0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R22379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
 {L_, "wbt27728,(250,100),1,27728,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt27728,(250,100),1,27728,0", 0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R22379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
 {L_, "wbt28720,(250,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "R35000",                       0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
 {L_, "wbt28720,(250,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40,(250,100),1,40,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R25000",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
 {L_, "wbt28720,(255,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "wbt15,(255,10),1,15,0",        0,   1,    2,    1,    0, e_NVEC,  ""  },
 {L_, "R22379",                       0,   1,    2,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wbt28720,(250,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_, "wbt11,(250,8),1,11,0,{w20,0,20,0; wbt15,(255,6),0,15,0,{wb40,1,40,0}}",
                                      0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt18,(250,100),1,18,0",       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R25769",                       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    2,    1,    0, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
 {L_, "wbt27728,(255,100),1,27728,0", 0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt23728,(655,100),1,0,0",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt728,(655,20),1,0,-1",       0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt728,(255,20),1,728,0",      0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R25000",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Invalidate the channel, then issue 1 request and try dispatching.

 {L_, "wbt140,(255,100),1,140,0",     0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "iw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(265,100),1,0,-1",        0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wbt140,(20,100),1,140,0",      0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "cS",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,-3,0",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wbt1024,(20,100),1,1024,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt300,(20,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "cw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wbt40,(255,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt15,(265,100),1,15,0",       0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt1024,(255,100),1,1024,0",   0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40,(265,100),1,40,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt1020,(255,100),1,1020,0",   0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt728,(265,100),1,728,0",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R829",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
 {L_, "wbt1020,(255,100),1,1020,0",   0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt1120,(565,100),1,0,-1",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /* 1 of 1000 fail
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
 {L_, "wbt1120,(255,100),1,1120,0",   0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R500",                         0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
 {L_, "wbt1120,(255,100),1,1120,0",   0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt40,(265,100),1,40,0",       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R500",                         0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
 {L_, "wbt1120,(255,100),1,1120,0",   0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt15,(265,10),1,15,0",        0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    2,    1,    0, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },  /* 1 of 1000 fail
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during first dispatch.
 {L_, "wbt1020,(255,100),1,1020,0",   0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt1120,(465,100),1,1120,0",   0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wbt1120,(255,100),1,1120,0",   0,   2,    1,    2,    1, e_NVEC,  ""  },
   {L_,
    "wbt11,(265,100),1,11,0,{w20,0,20,0; wbt15,(275,10),0,15,0,{wb40,1,40,0}}",
                                      0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt18,(285,100),1,18,0",       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R729",                         0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    2,    1,    0, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
 {L_, "wbt728,(255,100),1,728,0",     0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt700,(265,100),1,700,0",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt720,(275,20),1,324,0",      0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R500",                         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 */
 #elif BSLS_PLATFORM_OS_LINUX

 { // Invalidate the channel, then issue 1 request and try dispatching.

 {L_, "wbt1040,(20,100),1,1040,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "iw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wbt1040,(20,100),1,1040,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "cS",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,-3,0",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "cw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wbt40,(20,100),1,40,0",        0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt15,(20,100),1,15,0",        0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
 {L_, "wbt40960,(20,100),1,40960,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40960,(20,100),1,32768,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40,(500,100),1,40,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R65536",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt10000,(500,100),1,10000,0", 0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt16383,(20,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt30000,(500,100),1,0,-1",    0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.  The first
   // requested must fill the buffers and be enqueued.  The second request
   // shall be enqueued.  Then we read the data thus making space available in
   // the system buffers.
 {L_, "wbt81920,(20,100),1,81920,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R40960",                       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
 {L_, "wbt30000,(20,100),1,30000,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt40,(20,100),1,40,0",        0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R35000",                       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },/*
 { // Issue 3 requests of different types, then dispatch.
 {L_, "wbt81920,(15,100),1,81920,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt15,(15,10),1,15,0",         0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during first dispatch.
 {L_, "wbt16383,(10,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt16383,(10,100),1,16383,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt30000,(650, 100),1,30000,0",0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R32786",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R40960",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wbt30000,(20,100),1,30000,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
  {L_, "wbt11,(20,100),1,11,0,{w20,0,20,0; wbt15,(5,10),0,15,0,{wb40,1,40,0}}",
                                      0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt18,(200,100),1,18,0",       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R35769",                       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    2,    1,    0, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R17000",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
 {L_, "wbt43728,(20,100),1,43728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt43728,(20,100),1,24576,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt728,(15,20),1,0,-1",        0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt728,(15,20),1,728,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R35000",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 } */
 #else                                // sun test data

 { // Invalidate the channel, then issue 1 request and try dispatching.

 {L_, "wbt1040,(20,100),1,1040,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "iw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wbt1040,(20,100),1,1040,0",    0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "cS",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,-3,0",         0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wbt73728,(20,100),1,73728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt3,(20,100),1,0,-1",         0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "cw",                           0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wbt40,(20,100),1,40,0",        0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt15,(20,100),1,15,0",        0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "dw0",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
 {L_, "wbt40960,(20,100),1,40960,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40960,(20,100),1,32768,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt73728,(20,100),1,73728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt40,(200,100),1,40,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R65536",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wbt43728,(20,100),1,43728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt43728,(20,100),1,43728,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
 {L_, "wbt73728,(20,100),1,73728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt73728,(20,100),1,0,-1",     0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.  The first
   // requested must fill the buffers and be enqueued.  The second request
   // shall be enqueued.  Then we read the data thus making space available in
   // the system buffers.
 {L_, "wbt81920,(20,100),1,81920,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R40960",                       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
 {L_, "wbt81920,(20,100),1,81920,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt40,(20,100),1,40,0",        0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "R35000",                       0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },/*
 { // Issue 3 requests of different types, then dispatch.
 {L_, "wbt81920,(15,100),1,81920,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "w18,1,18,0",                   0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt15,(15,10),1,15,0",         0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R52379",                       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, */
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during first dispatch.
 {L_, "wbt40960,(10,100),1,40960,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt81920,(650, 100),1,81920,0",0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R32786",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R40960",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wbt74720,(20,100),1,74720,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
  {L_, "wbt11,(20,100),1,11,0,{w20,0,20,0; wbt15,(5,10),0,15,0,{wb40,1,40,0}}",
                                      0,   2,    2,    2,    1, e_NVEC,  ""  },
 {L_, "wbt18,(200,100),1,18,0",       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "R35769",                       0,   2,    3,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    2,    1,    0, e_NVEC,  ""  },

 {L_, "dw1",                          0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   1,    1,    1,    0, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 }, /*
 { // Enqueue 1 request, then dispatch when the pipe is unwriteable: timeout.
   // after the request timeout, dispatch another request when writeable.
 {L_, "wbt43728,(20,100),1,43728,0",  0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt43728,(20,100),1,24576,0",  0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "wbt728,(15,20),1,0,-1",        0,   2,    2,    2,    1, e_NVEC,  ""  },

 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
 {L_, "wbt728,(15,20),1,728,0",       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "R35000",                       0,   2,    1,    2,    1, e_NVEC,  ""  },
 {L_, "dw1",                          0,   0,    0,    0,    0, e_NVEC,  ""  },
  {L_,  0,                             0,   0,    0,    0,    0, e_NVEC,  ""  }
 } */
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);
            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            if (verbose) {
                cout << "Channel: ";
                printBufferSizes(handles[0]);
                cout << "Control: ";
                printBufferSizes(handles[1]);
            }
            adjustBufferSizes(handles[0], BUF_LIMIT, 1);
            adjustBufferSizes(handles[1], BUF_LIMIT, 1);

            if (verbose) {
                cout << "Adj.  Channel: ";
                printBufferSizes(handles[0]);
                cout << "Adj.  Control: ";
                printBufferSizes(handles[1]);
            }

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(LINE);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                           SCRIPTS[i][j].d_expData,
                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 22: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'bufferedWrite()' FUNCTION:
        //   Initiate a non-blocking operation to write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   Write request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int bufferedWrite();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'bufferedWrite'"
                          << "\n=======================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd               PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----               ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data
 {
   {L_, "wb1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it. and dispatch.
   {L_, "wb27728,1,27728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wb40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb27728,1,27728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb23728,1,23728,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
   {L_, "wb27728,1,27728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb27728,1,27728,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R8379",                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
   {L_, "wb28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R25000",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
   {L_, "wb28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R25000",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
   {L_, "wb28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wb28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb11,1,11,0,{wb15,0,15,0; w20,0,20,0,{wb40,1,40,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb18,1,18,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R25769",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wb140,1,140,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wb140,1,140,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it. and dispatch.
   {L_, "wb1024,1,1024,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb30,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wb40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb1024,1,1024,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb850,1,850,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb850,1,850,0",           0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
   {L_, "wb1024,1,1024,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb1120,1,0,-1",           0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
   {L_, "wb1120,1,1120,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
   {L_, "wb1120,1,1120,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R50 0",                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
   {L_, "wb1120,1,1120,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /* 2 of 1000 fail:
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "wb1024,1,1024,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb1120,1,1120,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wb1120,1,1120,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb11,1,11,0,{wb15,0,15,0; w20,0,20,0,{wb40,1,40,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb18,1,18,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX
 { // Invalidate the channel, then issue 1 request and try dispatching.

   {L_, "wb1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wb1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it. and dispatch.
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wb40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb10000,1,10000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb30000,1,0,-1",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
   {L_, "wb30000,1,30000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R35000",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
   {L_, "wb30000,1,30000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R35000",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
   {L_, "wb30000,1,30000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb30000,1,30000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wb30000,1,30000,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb11,1,11,0,{wb15,0,15,0; w20,0,20,0,{wb40,1,40,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb18,1,18,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R35769",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else
                             // sun test data
 { // Invalidate the channel, then issue 1 request and try dispatching.

   {L_, "wb1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wb1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it. and dispatch.
   {L_, "wb73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wb40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wb43728,1,43728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb43728,1,43728,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if the channel's buffer will be extended properly if a big chunk of
   // data is to be written from a request.
   {L_, "wb73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb73728,1,0,-1",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a different kind of write request on the request queue can be
   // finished after the previous buffered requests before it.
   {L_, "wb74720,1,74720,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R35000",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Test if a write request on the request queue which can't be finished once
   // can be kept on the request queue and finished by following dispatches.
   {L_, "wb74720,1,74720,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R35000",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Issue 3 requests of different types, then dispatch.
   {L_, "wb74720,1,74720,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb15,1,15,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "wb73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wb73680,1,73680,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wb74720,1,74720,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb11,1,11,0,{wb15,0,15,0; w20,0,20,0,{wb40,1,40,0}}",
                                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "wb18,1,18,0",             0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "R35769",                  0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                           SCRIPTS[i][j].d_expData,
                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 21: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'timedWritev(btls::Iovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        //   the specified 'numBuffer' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int timedWritev(btls::Iovec);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedWritev(btls::Iovec)'"
                          << "\n=================================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvit5,(250,100),1,40,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(250,100),1,1040,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,0,-1",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wvit7,(250,10),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(450,10),1,0,0",    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvit7,(250,10),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit5,(250,10),1,40,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,100),1,29760,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of the same write type, and dispatch.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit5,(250,100),1,40,0",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,100),1,29760,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(255,8),1,18,0,{wb4,1,4,0; wvit1,(250,9),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit2,(250,100),1,15,0",   0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R22379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvit5,(255,100),1,40,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(255,100),1,540,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,0,-1",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,0,-1",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(665,100),1,1024,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit7,(255,100),1,0,-1",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "wvit7,(255,100),1,0,-1",  0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "wvit7,(665,100),1,0,-1",  0,   2,    4,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(255,100),1,540,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,-3,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,0,-1",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit5,(275,100),1,40,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(465,100),1,1024,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit5,(275,100),1,40,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /* 1 of 1000 failed
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(265,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(255,100),1,1024,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_, "wvit3,(265,100),1,18,0,{wb4,1,4,0; wvit1,(20,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit2,(275,100),1,15,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R729",                    0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvit5,(250,10),1,40,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(250,10),1,1040,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,-1",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit6,(250,10),1,0,0",    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(250,100),1,1040,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,-3,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvit7,(20,10),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(20,10),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(20,10),1,0,-1",    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit5,(400,100),1,40,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit8,(250,10),1,16383,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit5,(250,100),1,40,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R16383",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R16383",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R16383",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0,{wb4,1,4,0;" // cont.
        "wvit1,(250,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit2,(250,100),1,15,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #else                                  // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvit5,(250,10),1,40,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(250,10),1,1040,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,-1",   0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,-1",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and dispatch when no data in the pipe: timeout.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,0,0",    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvit6,(250,100),1,1040,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,-3,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvit7,(20,10),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(20,10),1,0,-1",    0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit5,(20,100),1,40,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit7,(250,10),1,57344,0",0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,100),1,18,0",  0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit5,(250,100),1,40,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,10),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvit3,(250,10),1,18,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R65536",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvit7,(250,100),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wvit3,(250,100),1,18,0,{wb4,1,4,0; wvit1,(250,100),0,11,0,{wb9,1,9,0}}",
                                   0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "wvit2,(250,100),1,15,0",  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "R52379",                  0,   2,    2,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    3,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   2,    1,    2,    1, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

#ifdef BSLS_PLATFORM_OS_LINUX
            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);
#endif

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(LINE);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 20: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'writev(btls::Iovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        //   the specified 'numBuffer' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int writev(btls::Iovec);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'writev(btls::Iovec)'"
                          << "\n============================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

  { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvi5,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,1040,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,29760,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,29760,0",           0,   0,    0,    0,    0, e_NVEC,   "" },
   {L_, "wvi3,1,18,0,{wb4,1,4,0; wvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi2,1,15,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvi5,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,540,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,540,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,-3,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,1024,0",           0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,1024,0",           0,   0,    0,    0,    0, e_NVEC,   "" },
   {L_, "wvi3,1,18,0,{wb4,1,4,0; wvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi2,1,15,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvi5,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,1040,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,1040,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,-3,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi8,1,16384,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,16383,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,16383,0",           0,   0,    0,    0,    0, e_NVEC,   "" },
   {L_, "wvi7,1,16383,0",           0,   0,    0,    0,    0, e_NVEC,   "" },
   {L_, "wvi3,1,18,0,{wb4,1,4,0; wvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi2,1,15,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else                                  // sun test data
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wvi5,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,1040,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wvi6,1,1040,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,-3,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't send the specified
   // length of data during dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi7,1,57344,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi5,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,73728,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wvi3,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wvi7,1,73728,0",           0,   0,    0,    0,    0, e_NVEC,   "" },
   {L_, "wvi3,1,18,0,{wb4,1,4,0; wvi1,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wvi2,1,15,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 19: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS) \
                                    && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'timedWrite()' FUNCTION:
        //   Initiate a non-blocking operation to write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int timedWrite();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedWrite'"
                          << "\n====================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                      PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                      ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wt40,(255,150),1,40,0",         0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt40,(255,50),1,40,0",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt1040,(255,150),1,1040,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "iw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(255,150),1,0,-1",          0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(255,150),1,0,-1",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and dispatch it when the pipe is unwritable.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(655,150),1,0,0",           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt18,(255,150),1,18,0",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt28720,(255,150),1,28720,0",   0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(255,150),1,0,-1",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "cw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt20,(255,150),0,20,0",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt28720,(250,1500),1,28720,0",  0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt28,(255,150),1,28,0",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wb30,1,30,0",                   0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt28720,(255,150),1,28720,0",   0,   0,    0,    0,    0, e_NVEC,  "" },
   {L_, "wt18,(255,150),1,18,0,{wb4,1,4,0; wt11,(5,150),0,11,0,{wb9,1,9,0}}",
                                       0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt2,(255,150),1,2,0",           0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R22379",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    3,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    2,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)    // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wt40,(225,150),1,40,0",         0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt40,(255,50),1,40,0",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R729",                          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt140,(255,150),1,140,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "iw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(265,150),1,0,-1",          0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt30,(655,150),1,0,-1",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R729",                          0,   2,    1,    2,    1, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and dispatch it when the pipe is unwritable.
 {L_, "wt1024,(255,150),1,1024,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt8020,(665,150),1,3072,0",     0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt140,(255,150),1,140,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "cS",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(265,150),1,-3,0",          0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt18,(265,150),1,18,0",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt1020,(675,150),1,1020,0",     0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R829",                          0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },

 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R520",                          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R1020",                         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt30,(265,150),1,0,-1",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "cw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt512,(275,150),0,512,0",       0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R1020",                         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt1020,(250,1500),1,1020,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt28,(260,150),1,28,0",         0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wb30,1,30,0",                   0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R729",                          0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during dispatch the first time.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt1120,(565,150),1,1120,0",     0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R829",                          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R829",                          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt1020,(255,150),1,1020,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
   {L_, "wt18,(265,150),1,18,0,{wb4,1,4,0; wt11,(265,150),0,11,0,{wb9,1,9,0}}",
                                       0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt2,(265,150),1,2,0",           0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R729",                          0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    3,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    2,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },

 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wt40,(20,150),1,40,0",          0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt40,(200,50),1,40,0",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt1040,(5,150),1,1040,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "iw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,-1",            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,-1",            0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and dispatch it when the pipe is unwritable.
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16383,(5,150),1,16383,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,0",             0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt1040,(5,150),1,1040,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "cS",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,-3,0",            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, /*
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wt65536,(20,150),1,65536,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16384,(20,150),1,16384,0",    0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt73728,(20,150),1,73728,0",    0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R40960",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R81920",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wt73728,(20,150),1,73728,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(20,150),1,0,-1",           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "cw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt20,(20,150),0,20,0",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt16383,(1500,1500),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16383,(1500,1500),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt28,(1050,150),1,28,0",        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wb30,1,30,0",                   0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, /*
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during dispatch the first time.
 {L_, "wt73728,(20,150),1,73728,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt73680,(90,150),1,73680,0",    0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R65536",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R65536",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
{L_, "wt73728,(20,150),1,73728,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
   {L_, "wt18,(20,150),1,18,0,{wb4,1,4,0; wt11,(20,150),0,11,0,{wb9,1,9,0}}",
                                       0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt2,(20,150),1,2,0",            0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R56384",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    3,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    2,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 */

 #else                                   // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wt40,(20,150),1,40,0",          0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wt73728,(5,150),1,73728,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt40,(5,50),1,40,0",            0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt1040,(5,150),1,1040,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "iw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,-1",            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wt73728,(5,150),1,73728,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,-1",            0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and dispatch it when the pipe is unwritable.
 {L_, "wt73728,(5,150),1,73728,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,0,0",             0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wt1040,(5,150),1,1040,0",       0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "cS",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(5,150),1,-3,0",            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "dw0",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, /*
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wt65536,(20,150),1,65536,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt16384,(20,150),1,16384,0",    0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt73728,(20,150),1,73728,0",    0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R40960",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R81920",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wt73728,(20,150),1,73728,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt3,(20,150),1,0,-1",           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "cw",                            0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt20,(20,150),0,20,0",          0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wt73728,(1500,1500),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt28,(1050,150),1,28,0",        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wb30,1,30,0",                   0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R52379",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 }, /*
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during dispatch the first time.
 {L_, "wt73728,(20,150),1,73728,0",    0,   0,    0,    0,    0, e_NVEC,  "" },
 {L_, "wt73680,(90,150),1,73680,0",    0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R65536",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "R65536",                        0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
{L_, "wt73728,(20,150),1,73728,0",     0,   0,    0,    0,    0, e_NVEC,  "" },
   {L_, "wt18,(20,150),1,18,0,{wb4,1,4,0; wt11,(20,150),0,11,0,{wb9,1,9,0}}",
                                       0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "wt2,(20,150),1,2,0",            0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "R56384",                        0,   2,    2,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    3,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    2,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   2,    1,    2,    1, e_NVEC,  "" },
 {L_, "dw1",                           0,   1,    1,    1,    0, e_NVEC,  "" },
 {L_, "dw1",                           0,   0,    0,    0,    0, e_NVEC,  "" },
  {L_,  0,                              0,   0,    0,    0,    0, e_NVEC,  "" }
 },
 */
 #endif

};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(LINE);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 18: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'write()' FUNCTION:
        //   Initiate a non-blocking operation to write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int write();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'write'"
                          << "\n===============" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "w40,1,40,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w1040,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "w29720,1,29720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20,1,20,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w28720,1,28720,0",        0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R4379",                   0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },

   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20,0,20,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w28,1,28,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w28720,1,28720,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0,{wb4,1,4,0; w11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "w40,1,40,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w40,1,40,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w140,1,140,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w30,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w140,1,140,0",            0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,-3,0",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w1220,1,1220,0",          0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/*
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w30,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w512,0,512,0",            0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R1020",                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w28,1,28,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /*
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w1120,1,1120,0",          0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w1020,1,1020,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0,{wb4,1,4,0; w11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "w40,1,40,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20000,1,20000,0",        0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R40960",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w1040,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w1040,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,-3,0",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   //{L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20,0,20,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w28,1,28,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "R32000",                  0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20000,1,20000,0",        0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16383,1,16383,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0,{wb4,1,4,0; w11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else                                  // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "w40,1,40,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "w65536,1,65536,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w16384,1,16384,0",        0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R40960",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w1040,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "w1040,1,1040,0",          0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,-3,0",               0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "w73728,1,73728,0",        0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w3,1,0,-1",               0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w20,0,20,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w28,1,28,0",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't finish the specified
   // length of data during the first dispatch.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w73680,1,73680,0",        0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "w73728,1,73728,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "w18,1,18,0,{wb4,1,4,0; w11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 17: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'timedWriteRaw()' FUNCTION:
        //   Initiate a non-blocking operation to  *atomically* write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int timedWriteRaw();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedWriteRaw'"
                          << "\n=======================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wrt40,(20,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt40,(5,200),1,40,0",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt1040,(20,100),1,1040,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "iw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,0,-1",        0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,0,-1",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, try dispatching when non-writeable, so timeout will
   // work.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,0,0",         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt18,(20,100),1,18,0",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt40,(20,100),1,40,0",       0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,0,-1",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "cw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt20,(20,100),0,20,0",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt28,(20,100),1,28,0",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wb30,1,30,0",                 0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
 {L_, "wrt28720,(15,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt29720,(5,10),1,29720,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt28720,(20,100),1,28720,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wrt18,(20,100),1,18,0,{wr4,1,4,0; wrt11,(20,100),0,11,0,{wb9,1,9,0}}",
                                     0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt2,(20,100),1,2,0",         0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R22379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    3,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    2,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data
  // Try writing 0 bytes.
 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wrt40,(20,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wrt1020,(15,100),1,1020,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt42,(525,200),1,42,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R1000",                       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt140,(20,100),1,140,0",     0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "iw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,0,-1",        0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wrt1024,(20,100),1,1024,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt30,(20,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R500",                        0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt140,(20,100),1,140,0",     0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "cS",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,-3,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, try dispatching when non-writeable, so timeout will
   // work.
 {L_, "wrt1024,(20,100),1,1024,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt30,(20,100),1,0,0",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wrt1020,(15,100),1,1020,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt18,(200,100),1,18,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt40,(205,100),1,40,0",      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R900",                        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wrt1020,(265,100),1,1020,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt32,(250,100),1,0,-1",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "cw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt512,(275,100),0,512,0",    0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R1020",                       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt1020,(250,100),1,1020,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt28,(255,100),1,28,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wb30,1,30,0",                 0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R500",                        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
 {L_, "wrt1020,(255,100),1,1020,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt1120,(265,10),1,1024,0",   0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R500",                        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt1020,(255,100),1,1020,0",  0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt18,(265,100),1,18,0,{wr4,1,4,0; wrt11,(245,100),0,11,0,{wb9,1,9,0}}",
                                     0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt2,(275,100),1,2,0",        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R500",                        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    3,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    2,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wrt40,(20,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt40,(265,200),1,40,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt1040,(20,100),1,1040,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "iw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(255,100),1,0,-1",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(200,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt1040,(20,100),1,1040,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "cS",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,-3,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, try dispatching when non-writeable, so timeout will
   // work.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(200,100),1,0,0",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "R52379",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "R52379",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt18,(800,100),1,18,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt40,(800,100),1,40,0",      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(250,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "cw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt20,(500,100),0,20,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(20,100),1,16383,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt28,(500,100),1,28,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wb30,1,30,0",                 0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
 {L_, "wrt16383,(250,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(250,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt33000,(250,10),1,16383,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt16383,(500,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt16383,(500,100),1,16383,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_, "wrt18,(500,100),1,18,0,{wr4,1,4,0; wrt11,(20,100),0,11,0,{wb9,1,9,0}}",
                                     0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt2,(500,100),1,2,0",        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    3,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    2,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else                                  // sun test data

 { // Issue 1 request, which should be finished without being enqueued.
 {L_, "wrt40,(20,100),1,40,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt40,(265,200),1,40,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt1040,(20,100),1,1040,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "iw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(255,100),1,0,-1",       0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(200,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
 {L_, "wrt1040,(20,100),1,1040,0",   0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "cS",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(20,100),1,-3,0",        0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "dw0",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, try dispatching when non-writeable, so timeout will
   // work.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(200,100),1,0,0",        0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt18,(200,100),1,18,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt40,(200,100),1,40,0",      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt3,(250,100),1,0,-1",       0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "cw",                          0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt20,(250,100),0,20,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt73728,(20,100),1,73728,0", 0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt28,(250,100),1,28,0",      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wb30,1,30,0",                 0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
 {L_, "wrt73728,(250,100),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
 {L_, "wrt73680,(250,10),1,57344,0", 0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
 {L_, "wrt73728,(200,100),1,73728,0",0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_, "wrt18,(250,100),1,18,0,{wr4,1,4,0; wrt11,(20,100),0,11,0,{wb9,1,9,0}}",
                                     0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "wrt2,(250,100),1,2,0",        0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "R52379",                      0,   2,    2,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    3,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    2,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   2,    1,    2,    1, e_NVEC,   ""  },
 {L_, "dw1",                         0,   1,    1,    1,    0, e_NVEC,   ""  },
 {L_, "dw1",                         0,   0,    0,    0,    0, e_NVEC,   ""  },
  {L_,  0,                            0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 16: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'writeRaw()' FUNCTION:
        //   Initiate a non-blocking operation to  *atomically* write *up *to*
        //   the specified 'numBytes' into this channel.
        //   The main concerns about the function are (1) if a write
        //   request can be finished right away when the channel is
        //   writeable; (2) if a write request will be properly enqueued to
        //   the request queue when the channel is not writeable, even
        //   though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   can be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a write-event can be removed
        //   when no any write request of this type of write-event.
        //   (6) if a different kind of read-event can be registered when
        //   next write-event is another kind of write request (e.g., a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the SCRIPT
        //   language as described above, different operations will be
        //   executed on the channel.  Upon the execution of each
        //   operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "write request queue", "write event queue",
        //   and "numTimers" also the buffer data in the channel.  The
        //   "TCPNODELAY" option for the peer socket is set on because some
        //   small packets will be written to the channel and we require
        //   these data be read from the channel right after this write.
        //
        // Testing:
        //   int writeRaw();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'writeRaw'"
                          << "\n==================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wr40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr220,1,220,0",           0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr30,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr20,0,20,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr28,1,28,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr29720,1,29720,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr28720,1,28720,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0,{wr4,1,4,0; wr11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R22379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wr40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr140,1,140,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr30,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr140,1,140,0",           0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, /* 2 of 1000 fail: dw1
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr30,1,0,-1",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr20,0,20,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 }, */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr28,1,28,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },/*
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr1120,1,1024,0",         0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },  */
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr1020,1,1020,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0,{wr4,1,4,0; wr11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R500",                    0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },

 #elif BSLS_PLATFORM_OS_LINUX

  // Try writing 0 bytes.
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wr40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr20,0,20,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr28,1,28,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr33000,1,16384,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr16383,1,16383,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0,{wr4,1,4,0; wr11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #else                                  // sun test data

  // Try writing 0 bytes.
 { // Issue 1 request, which should be finished without being enqueued.
   {L_, "wr40,1,40,0",             0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request, then dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "iw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request without being dispatched.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Invalidate the channel, then issue 1 request and try dispatching.
   {L_, "wr1040,1,1040,0",         0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "cS",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,-3,0",              0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "dw0",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr40,1,40,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 1 request and later cancel it, then enqueue a new request and
   // dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr3,1,0,-1",              0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "cw",                      0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr20,0,20,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr28,1,28,0",             0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wb30,1,30,0",             0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests, make the second one couldn't extract the specified
   // length of data during dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr73680,1,57344,0",       0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 { // Enqueue 2 requests of different write event types, and dispatch.
   {L_, "wr73728,1,73728,0",       0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_, "wr18,1,18,0,{wr4,1,4,0; wr11,0,11,0,{wb9,1,9,0}}",
                                   0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "wr2,1,2,0",               0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "R52379",                  0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    3,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    2,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   1,    1,    1,    0, e_NVEC,   ""  },
   {L_, "dw1",                     0,   0,    0,    0,    0, e_NVEC,   ""  },
   {L_,  0,                        0,   0,    0,    0,    0, e_NVEC,   ""  }
 },
 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);
            LOOP_ASSERT(i, cSocket);

            btlso::TcpTimerEventManager eveManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                                 &testAllocator);

                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer, &eveManager,
                                    &eveManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);   P(eveManager.numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 15: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_AIX)                         \
 && !defined(BSLS_PLATFORM_OS_SOLARIS)                     \
 && !defined(BSLS_PLATFORM_OS_LINUX)                       \
        // ------------------------------------------------------------------
        // TESTING 'timedReadv' and 'timedReadvRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBuffers' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g., a read request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int timedReadv();
        //   int timedReadvRaw();
        // -----------------------------------------------------------------

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        if (verbose) cout << endl
                          << "\nTESTING 'timedReadv' METHOD"
                          << "\n==========================="
                          << "\nTESTING 'timedReadvRaw' METHOD"
                          << "\n==============================" << endl;
        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "ir",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt6,(200,100),0,-1,0", 0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be dispatched
   // due to timeout properly when no data in the pipe.
 {L_, "rvrt2,(200,100),0,0,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt2,(200,100),0,3,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W3",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt2,(200,100),0,3,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt3,(200,100),0,7,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "1234567"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when it can't be done once and so this request need to be
   // dispatched again.
 {L_, "W20",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt4,(200,100),0,13,0", 1,    2,    0,   2,    1,  e_NVEC,  ""       },
   {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC,
                                                          "1234567890abcd"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Test if a channel can still work correctly for new read request.after the
   // previous reads are canceled.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt4,(200,100),0,0,-1", 1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "cr",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt3,(200,100),1,7,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "1234567"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rb2,0,2,0",              1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
 {L_, "rvrt3,(255,100),0,7,0",  0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_IOVEC, "4567890"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
#ifndef BSLS_PLATFORM_OS_LINUX
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt3,(255,100),0,7,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "rvrt2,(255,100),0,3,0",  2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_IOVEC, "1234567"},
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "890"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 {L_, "W5",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt2,(255,20),0,3,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "rvrt3,(255,10),0,2,0",   2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_IOVEC, "123"    },

 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "45"     },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
#endif
 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in a channel.
 {L_, "W3",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvrt2,(200,100),0,3,0",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "rb5,0,5,0",              2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "123"    },
 {L_, "W5",                     1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "12345"  },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
#ifndef BSLS_PLATFORM_OS_LINUX
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W10",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
   {L_, "rvrt2,(250,100),0,3,0,{rvrt3,(250,100),1,7,0}",
                                1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_IOVEC, "123"    },
   {L_, "rvrt2,(250,100),1,3,0,{r4,0,4,0; rvrt4,(250,100),1,13,0}",
                                2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_IOVEC, "4567890"},
 {L_, "W20",                    1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    2,    1,    0,   1,    0,  e_IOVEC, "123"    },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_NVEC,  "4567"   },
   {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC,
                                                           "890abcdefghij"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
#endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    char buf[MAX_BUF] = "\0",
                    buf0[2] = "\0", buf1[3] = "\0", buf2[5] = "\0",
                    buf3[7] = "\0", buf4[9] = "\0", buf5[10] = "\0";

                    btls::Iovec vecBuffer[MAX_VEC];
                    vecBuffer[0].setBuffer(buf0, sizeof buf0 - 1);
                    vecBuffer[1].setBuffer(buf1, sizeof buf1 - 1);
                    vecBuffer[2].setBuffer(buf2, sizeof buf2 - 1);
                    vecBuffer[3].setBuffer(buf3, sizeof buf3 - 1);
                    vecBuffer[4].setBuffer(buf4, sizeof buf4 - 1);
                    vecBuffer[5].setBuffer(buf5, sizeof buf5 - 1);

                    Buffer buffer = {buf, 0, vecBuffer, 0, 0};

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[4].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 14: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // ------------------------------------------------------------------
        // TESTING 'readv' and 'readvRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g., a read request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int readv();
        //   int readvRaw();
        // -----------------------------------------------------------------

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        if (verbose) cout << endl
                          << "\nTESTING 'readv' METHOD"
                          << "\n======================"
                          << "\nTESTING 'readvRaw' METHOD"
                          << "\n=========================" << endl;
        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "ir",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr6,0,-1,0",            0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr2,0,3,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W3",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr2,0,3,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr3,0,7,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "1234567"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when enough data in the pipe.
 {L_, "W20",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr4,0,13,0",            1,    1,    0,   1,    0,  e_NVEC,  ""       },
   {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC,
                                                          "1234567890abcd"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Test if a channel can still work correctly for new read request after the
   // previous reads are canceled.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr4,0,0,-1",            1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "cr",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr3,1,7,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "1234567"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rb2,0,2,0",              1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "123"    },
 {L_, "rvr3,0,7,0",             0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_IOVEC, "4567890"},
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr3,0,7,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "rvr2,0,3,0",             2,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "1234567"},
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "890"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 {L_, "W5",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr2,0,3,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "rvr3,0,2,0",             2,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "123"    },

 {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC, "45"     },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in the channel.
 {L_, "W3",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rvr2,0,3,0",             1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "rb5,0,5,0",              2,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "123"    },
 {L_, "W5",                     1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "12345"  },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W10",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
   {L_, "rvr2,0,3,0,{rvr3,1,7,0}",
                                1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "123"    },
   {L_, "rvr2,1,3,0,{r4,0,4,0; rvr4,1,13,0}",
                                2,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_IOVEC, "4567890"},
 {L_, "W20",                    1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    2,    1,    0,   1,    0,  e_IOVEC, "123"    },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_NVEC,  "4567"   },
   {L_, "dr1",                    0,    0,    0,   0,    0,  e_IOVEC,
                                                           "890abcdefghij"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
};

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    char buf[MAX_BUF] = "\0",
                    buf0[2] = "\0", buf1[3] = "\0", buf2[5] = "\0",
                    buf3[7] = "\0", buf4[9] = "\0", buf5[10] = "\0";

                    btls::Iovec vecBuffer[MAX_VEC];
                    vecBuffer[0].setBuffer(buf0, sizeof buf0 - 1);
                    vecBuffer[1].setBuffer(buf1, sizeof buf1 - 1);
                    vecBuffer[2].setBuffer(buf2, sizeof buf2 - 1);
                    vecBuffer[3].setBuffer(buf3, sizeof buf3 - 1);
                    vecBuffer[4].setBuffer(buf4, sizeof buf4 - 1);
                    vecBuffer[5].setBuffer(buf5, sizeof buf5 - 1);

                    Buffer buffer = {buf, 0, vecBuffer, 0, 0};

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[4].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 13: {
        // ------------------------------------------------------------------
        // TESTING 'timedBufferedReadRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g., a read request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int timedBufferedReadRaw();
        // -----------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedBufferedReadRaw'"
                          << "\n=============================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "ir",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt6,(20,100),0,-1,0",  0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt4,(20,100),0,4,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j123"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W4",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt3,(20,100),0,3,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j12"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt2,(20,100),0,2,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j1"     },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when no enough data in the pipe.
 {L_, "W4",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt5,(20,100),0,4,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j123"   },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Test if a channel can still work correctly for new read request after the
   // previous reads are canceled.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt4,(20,100),0,0,-1",  1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "cr",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt3,(20,100),1,3,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j12"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt2,(20,100),0,2,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j1"     },
 {L_, "rbrt5,(20,100),0,5,0",   0,    0,    0,   0,    0,  e_NVEC,  "23456"  },
 {L_, "dr0",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 {L_, "W11",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt4,(50,100),0,4,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "rbrt3,(5,200),0,3,0",    2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "456"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 {L_, "W5",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbt2,(5,200),0,2,0",     1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "rbt5,(5,200),0,5,0",     2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_NVEC,  "j1"     },
 {L_, "W2",                     1,    2,    0,   2,    1,  e_NVEC,   ""      },

 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "234j1"  },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },

 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in a channel.
 {L_, "W3",                     0,    0,    0,   0,    0,  e_NVEC,  ""       },
 {L_, "rbrt3,(50,200),0,3,0",   1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "r5,0,5,0",               2,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    1,    0,   1,    0,  e_NVEC,  "j12"    },
 {L_, "W5",                     1,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "j1234"  },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W10",                    0,    0,    0,   0,    0,  e_NVEC,  ""       },
   {L_, "rbrt3,(20,100),0,3,0,{rbrt2,(200,200),1,2,0}",
                                1,    2,    0,   2,    1,  e_NVEC,  ""       },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "34"     },
   {L_, "rbrt2,(500,200),1,2,0,{r4,0,4,0; rbrt3,(30,200),1,3,0}",
                                2,    1,    0,   1,    0,  e_NVEC,  "789"    },
 {L_, "W4",                     2,    1,    0,   1,    0,  e_NVEC,  ""       },
 {L_, "dr1",                    1,    2,    0,   2,    1,  e_NVEC,  "789j"   },
 {L_, "dr1",                    0,    0,    0,   0,    0,  e_NVEC,  "123"    },
  {L_,  0,                       0,    0,    0,   0,    0,  e_NVEC,  ""       }
 },
};

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
//                         LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
//                                              SCRIPTS[i][j].d_expData,
//                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
      } break;
      case 12: {
        // ------------------------------------------------------------------
        // TESTING 'bufferedReadRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g.,a timedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int bufferedReadRaw();
        // -----------------------------------------------------------------
        if (verbose) cout << "\nTesting 'bufferedReadRaw'"
                          << "\n=========================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
  // Try reading 0 bytes.
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 {L_, "W11",                    0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "ir",                     0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr6,0,-1,0",            0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "dr0",                    0,   0,    0,   0,    0,  e_NVEC,   ""       },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,   ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                     0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr4,0,4,0",             1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "j123"    },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,   ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W4",                     0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr3,0,3,0",             1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,   "j12"    },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,   ""       }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 {L_, "W11",                    0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,   "j1"     },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,    ""      }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when no enough data in the pipe.
 {L_, "W4",                     0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr5,0,4,0",             1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,   "j123"   },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,   ""       }
 },
 { // Test if a channel can still work correctly for new read request.after the
   // previous reads are canceled.
 {L_, "W11",                    0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr4,0,0,-1",            1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "cr",                     0,   0,    0,   0,    0,  e_NVEC,   ""       },
 {L_, "rbr3,1,3,0",             1,   1,    0,   1,    0,  e_NVEC,   ""       },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,   "j12"    },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,   ""       }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 {L_, "W11",                    0,   0,    0,   0,    0,  e_NVEC,  ""        },
 {L_, "rbr2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "j1"      },
 {L_, "rb5,0,5,0",              0,   0,    0,   0,    0,  e_NVEC,  "23456"   },
 {L_, "dr0",                    0,   0,    0,   0,    0,  e_NVEC,  ""        },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,  ""        }
 },
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 {L_, "W11",                    0,   0,    0,   0,    0,  e_NVEC,  ""        },
 {L_, "rbr4,0,4,0",             1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "rbr3,0,3,0",             2,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "456"     },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,  ""        }
 },
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 {L_, "W5",                     0,   0,    0,   0,    0,  e_NVEC,  ""        },
 {L_, "rbr2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "rbr5,0,3,0",             2,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "234"     },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,  ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in a channel.
 {L_,  "W3",                    0,   0,    0,   0,    0,  e_NVEC,  ""        },
 {L_,  "rbr3,0,3,0",            1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_,  "r5,0,5,0",              2,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC,  "j12"     },
 {L_,  "W5",                    1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC,  "j1234"   },
  {L_,   0,                      0,   0,    0,   0,    0,  e_NVEC,  ""        }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W10",                    0,   0,    0,   0,    0,  e_NVEC,  ""        },
   {L_, "rbr3,0,3,0,{rbr2,1,2,0}",
                                1,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "34"      },
   {L_, "rbr2,1,2,0,{r4,0,4,0; rbr3,1,3,0}",
                                2,   1,    0,   1,    0,  e_NVEC,  "789"     },
 {L_, "W4",                     2,   1,    0,   1,    0,  e_NVEC,  ""        },
 {L_, "dr1",                    1,   1,    0,   1,    0,  e_NVEC,  "789j"    },
 {L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC,  "123"     },
  {L_,  0,                       0,   0,    0,   0,    0,  e_NVEC,  ""        }
   },
};

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
//                         LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
//                                              SCRIPTS[i][j].d_expData,
//                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
      } break;
      case 11: {
        // ------------------------------------------------------------------
        // TESTING 'timedBufferedRead()' FUNCTION:
        //   Initiate a non-blocking operation to read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g., a timedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //   in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int timedBufferedRead();
        // -----------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedBufferedRead'"
                          << "\n===========================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "ir",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt6,(20,100),0,-1,0",  0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 { L_,   "W4",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "rbt4,(20,100),0,4,0",  1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, "j123"    },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,   "W4",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "rbt3,(20,100),0,3,0",  1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt2,(20,100),0,2,0",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when it can't be done once and so this request need to be
   // dispatched again.
 { L_,  "W4",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt5,(20,100),0,5,0",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "W2",                    1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j123j"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Test if a channel can still work correctly for new read request.after the
   // previous reads are canceled.
 {L_,  "W11",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "rbt4,(20,100),0,0,-1",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 {L_,  "cr",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "rbt3,(20,100),1,3,0",    1,   2,    0,   2,    1,  e_NVEC, ""        },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  {L_,   0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt2,(20,100),0,2,0",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
 { L_,  "rbt5,(20,100),0,5,0",   0,   0,    0,   0,    0,  e_NVEC, "23456"   },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt4,(50,100),0,4,0",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "rbt3,(5,200),0,3,0",    2,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "456"     },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 { L_,  "W5",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt2,(5,200),0,2,0",    1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "rbt5,(5,200),0,5,0",    2,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   2,    0,   2,    1,  e_NVEC, "j1"      },
 { L_,  "W2",                    1,   2,    0,   2,    1,  e_NVEC, ""        },

 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "234j1"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching.  This request need to be dequeued and the timer
   // should be unregistered subsequentially.
 { L_, "W7",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_, "rbt6,(50,200),0,6,0",    1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_, "rb2,0,2,0",              2,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_, "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "j12345"  },
 { L_, "W3",                     1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "6j"      },
  { L_,  0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if a different type of read
   // requests can be finished properly in a channel after the first request.
 { L_,  "W3",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rbt3,(50,200),0,3,0",   1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "r5,0,5,0",              2,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j12"     },
 { L_,  "W5",                    1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1234"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 { L_,  "W10",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
   { L_,  "rbt3,(250,100),0,3,0,{rbt2,(200,200),1,2,0}",
                                 1,   2,    0,   2,    1,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "34"      },
   { L_,  "rbt2,(500,200),1,2,0,{r4,0,4,0; rbt3,(100,200),1,3,0}",
                                 2,   1,    0,   1,    0,  e_NVEC, "789"     },
 { L_,  "W4",                    2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   2,    0,   2,    1,  e_NVEC, "789j"    },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "123"     },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP3_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead,
                                 channel.numPendingReadOperations(),
                                 SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP3_ASSERT(LINE,
                                     SCRIPTS[i][j].d_numReadEvent,
                                     channel.readEventManager()->numEvents(),
                                     SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP3_ASSERT(LINE,
                                     SCRIPTS[i][j].d_numWriteEvent,
                                     channel.writeEventManager()->numEvents(),
                                     SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP3_ASSERT(LINE,
                                     SCRIPTS[i][j].d_numTimers,
                                     channel.writeEventManager()->numTimers(),
                                     SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
//                         LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
//                                           SCRIPTS[i][j].d_expData,
//                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
      } break;
      case 10: {
        // ------------------------------------------------------------------
        // TESTING 'bufferedRead()' FUNCTION:
        //   Initiate a non-blocking operation to read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g.,a timedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //    in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int bufferedRead();
        // -----------------------------------------------------------------
        if (verbose) cout << "\nTesting 'bufferedRead'"
                          << "\n======================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "ir",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb6,0,-1,0",            0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 { L_,   "W4",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "rb4,0,4,0",            1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, "j123"    },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,   "W4",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "rb3,0,3,0",            1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },

 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when it can't be done once and so this request need to be
   // dispatched again.
 { L_,  "W4",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb5,0,5,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "W2",                    1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j123j"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Test if a channel can still work correctly for new read request after
   // previous reads are canceled.
 {L_,  "W11",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "rb4,0,0,-1",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "cr",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "rb3,1,3,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  {L_,   0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Issue 2 requests, then dispatch: test if a request can be done right away
   // when there is enough data in the channel's read buffer.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
 { L_,  "rb5,0,5,0",             0,   0,    0,   0,    0,  e_NVEC, "23456"   },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 }, /*  1 of 1000 fail: strncmp("456",;;;)
 { // Enqueue 2 requests, then dispatch when enough data in the pipe:
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb4,0,4,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "rb3,0,3,0",             2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "456"     },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 }, */
 { // Enqueue 2 requests, then dispatch when no enough data in the channel's
   // read buffer.
 { L_,  "W5",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "rb5,0,5,0",             2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j1"      },
 { L_,  "W2",                    1,   1,    0,   1,    0,  e_NVEC, ""        },

 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "234j1"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },

 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching.  This request need to be dequeued and the timer
   // should be registered at the same time.
 { L_, "W7",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_, "rb6,0,6,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "rb2,0,2,0",              2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "j12345"  },
 { L_, "W3",                     1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "6j"      },
  { L_,  0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in a channel.
 { L_,  "W3",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb3,0,3,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "r5,0,5,0",              2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j12"     },
 { L_,  "W5",                    1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1234"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_,  "W10",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
   {L_,  "rb3,0,3,0,{rb2,1,2,0}",
                                 1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "34"      },
   {L_,  "rb2,1,2,0,{r4,0,4,0; rb3,1,3,0}",
                                 2,   1,    0,   1,    0,  e_NVEC, "789"     },
 {L_,  "W4",                     2,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "789j"    },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "123"     },
  {L_,   0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
//                         LOOP3_ASSERT(LINE, buffer.d_readBuf,
//                                      SCRIPTS[i][j].d_expData,
//                                      0 == strncmp(buffer.d_readBuf,
//                                           SCRIPTS[i][j].d_expData,
//                                           strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
      } break;
      case 9: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // --------------------------------------------------------------------
        // TESTING 'timedReadRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g.,a bufferedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //    in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int timedReadRaw();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'timedReadRaw'"
                            << "\n====================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type d_expD
 //------  -----               ----- ----- ----- ------ ------ ---- ------
   // Try reading 0 bytes.
 { // Invalidate the channel, enter 1 request, then try dispatching: test
   // if any read request can be issued after the invalidation.
 {L_, "W11",                     0,   0,    0,    0,    0,  e_NVEC,  ""      },
 {L_, "ir",                      0,   0,    0,    0,    0,  e_NVEC,  ""      },
 {L_, "rrt6,(200,2000),0,-1,0",  0,   0,    0,    0,    0,  e_NVEC,  ""      },
 {L_, "dr0",                     0,   0,    0,    0,    0,  e_NVEC,  ""      },
  {L_,  0,                        0,   0,    0,    0,    0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W5",                      0,   0,    0,    0,    0,  e_NVEC,  ""      },
 {L_, "rrt4,(200,2000),0,4,0",   1,   2,    0,    2,    1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,    0,    0,  e_NVEC,  "j123"  },
  {L_,  0,                        0,   0,    0,    0,    0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W8",                      0,   0,    0,    0,    0,  e_NVEC,  ""      },
 {L_, "rrt4,(200,200),0,4,0",    1,   2,    0,    2,    1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,    0,    0,  e_NVEC,  "j123"  },
  {L_,  0,                        0,   0,    0,    0,    0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly, dispatch to test the timeout.
 {L_, "rrt4,(200,100),0,0,0",    1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly when no enough data in the pipe.  The request should be finished
   // successfully after more data is available.
 {L_, "W5",                      0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rrt6,(300,200),0,5,0",    1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "j1234" },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Fill the readBuffer in the channel first, then issue 1 request and try
   // dispatching properly.
 {L_, "W7",                      0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               1,    1,   0,    1,     0, e_NVEC,  ""      },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "j1"    },
 {L_, "rrt4,(200,1),0,4,0",      0,    0,   0,    0,     0, e_NVEC,  "2345"  },
 {L_, "dr0",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching properly.
 { L_, "W5",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
 { L_, "rb2,0,2,0",              1,    1,   0,    1,     0, e_NVEC,  ""      },
 { L_, "dr1",                    0,    0,   0,    0,     0, e_NVEC,  "j1"    },
 { L_, "rrt6,(250,20),0,3,0",    1,    2,   0,    2,     1, e_NVEC,  "234"   },
 { L_, "dr1",                    0,    0,   0,    0,     0, e_NVEC,  "234"   },
  { L_,  0,                       0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the timed request can be
   // registered correctly.
 {L_, "W7",                      0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "r3,0,3,0",                1,    1,   0,    1,     0, e_NVEC,  ""      },
 {L_, "rrt4,(200,20),0,4,0",     2,    1,   0,    1,     0, e_NVEC,  ""      },
 {L_, "dr1",                     1,    2,   0,    2,     1, e_NVEC,  "j12"   },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "3456"  },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the timer can be deregistered
   // properly while other requests exist on the queue.
 {L_, "W7",                      0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rrt4,(221,200),0,4,0",    1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "r3,0,3,0",                2,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     1,    1,   0,    1,     0, e_NVEC,  "j123"  },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "456"   },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the request can be dispatched
   // properly after dispatching other request which consumed all data in the
   // pipe.
 {L_, "W3",                      0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "r3,0,3,0",                1,    1,   0,    1,     0, e_NVEC,  ""      },
 {L_, "rrt4,(600,200),0,0,0",    2,    1,   0,    1,     0, e_NVEC,  ""      },
 {L_, "dr1",                     1,    2,   0,    2,     1, e_NVEC,  "j12"   },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if different requests can be
   // dispatched properly by registering correct events.
 {L_, "W11",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rrt6,(250,200),0,6,0",    1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "rb3,0,3,0",               2,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     1,    1,   0,    1,     0, e_NVEC,  "j12345"},
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "678"   },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rrt3,(200,1),0,3,0",      1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               2,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "rrt6,(200,2),0,6,0",      3,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     2,    1,   0,    1,     0, e_NVEC,  "j12"   },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "567890"},
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
 {L_, "rrt3,(200,1),0,3,0",      1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "rrt6,(400,2),0,6,0",      2,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               3,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     2,    2,   0,    2,     1, e_NVEC,  "j12"   },

 {L_, "dr1",                     1,    1,   0,    1,     0, e_NVEC,  "345678"},
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "90"    },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W11",                     0,    0,   0,    0,     0, e_NVEC,  ""      },
   {L_, "rrt3,(300,2),0,3,0,{rrt2,(40,2),1,2,0}",
                                 1,    2,   0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     1,    2,   0,    2,     1, e_NVEC,  "j12"   },
 {L_, "W4",                      1,    2,   0,    2,     1, e_NVEC,  ""      },
   {L_, "rrt1,(260,2),1,1,0,{rb5,0,5,0; rt3,(100,2),1,3,0}",
                                 2,    2,   0,    2,     1, e_NVEC,  ""      },

 {L_, "dr1",                     1,    2,   0,    2,     1, e_NVEC,  "34"    },
 {L_, "dr1",                     2,    1,   0,    1,     0, e_NVEC,  "5"     },
 {L_, "dr1",                     0,    0,   0,    0,     0, e_NVEC,  "j12"   },
  {L_,  0,                        0,    0,   0,    0,     0, e_NVEC,  ""      }
 },
};

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P(eveManager.numEvents());
                        P_(buf); P(channel.readEventManager()->numTimers());
                        P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 8: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // --------------------------------------------------------------------
        // TESTING 'readRaw()' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g., a bufferedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //    in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int readRaw();
        // --------------------------------------------------------------------
          if (verbose) cout << "\nTesting 'readRaw'"
                            << "\n=================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------ -----                ----- ----- ----- ------ ----- ---- ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
  {L_, "W11",                     0,   0,    0,   0,    0,  e_NVEC,  ""      },
  {L_, "ir",                      0,   0,    0,   0,    0,  e_NVEC,  ""      },
  {L_, "rr6,0,-1,0",              0,   0,    0,   0,    0,  e_NVEC,  ""      },
  {L_, "dr0",                     0,   0,    0,   0,    0,  e_NVEC,  ""      },
   {L_,  0,                        0,   0,    0,   0,    0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
  {L_, "W4",                      0,   0,    0,   0,    0,  e_NVEC,  ""      },
  {L_, "rr4,0,4,0",               1,   1,    0,   1,    0,  e_NVEC,  ""      },
  {L_, "dr1",                     0,   0,    0,   0,    0,  e_NVEC,  "j123"  },
   {L_,  0,                        0,   0,    0,   0,    0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W8",                      0,   0,    0,   0,     0, e_NVEC,   ""      },
 {L_, "rr4,0,4,0",               1,   1,    0,   1,     0, e_NVEC,   ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,   "j123"  },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,   ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                      0,   0,    0,   0,     0, e_NVEC,   ""      },
 {L_, "rr8,0,4,0",               1,   1,    0,   1,     0, e_NVEC,   ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,   "j123"  },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,   ""      }
 },
 { // Fill the readBuffer in the channel first, then issue 1 request and try
   // dispatching properly.
 {L_, "W7",                      0,   0,    0,   0,     0, e_NVEC,   ""      },
 {L_, "rb2,0,2,0",               1,   1,    0,   1,     0, e_NVEC,   ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,   "j1"    },
 {L_, "rr4,0,4,0",               0,   0,    0,   0,     0, e_NVEC,   "2345"  },
 {L_, "dr0",                     0,   0,    0,   0,     0, e_NVEC,   ""      },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,   ""      }
 },
 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching properly.
 {L_, "W5",                      0,   0,    0,   0,     0, e_NVEC,   ""      },
 {L_, "rb2,0,2,0",               1,   1,    0,   1,     0, e_NVEC,   ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,   "j1"    },
 {L_, "rr6,0,4,0",               1,   1,    0,   1,     0, e_NVEC,   "234"   },
 {L_, "W1",                      1,   1,    0,   1,     0, e_NVEC,   ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,   "234j"  },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,   ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the timer can be deregistered
   // properly while other requests exist on the queue.
 {L_, "W7",                      0,   0,    0,   0,     0, e_NVEC,  ""       },
 {L_, "rr4,0,4,0",               1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rb3,0,3,0",               2,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     1,   1,    0,   1,     0, e_NVEC,  "j123"   },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,  "456"    },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,  ""       }
 },
 { // Enqueue 2 requests, then dispatch: test if the request can be dispatched
   // properly after dispatching other request which consumed all data in the
   // pipe.
 {L_, "W3",                      0,   0,    0,   0,     0, e_NVEC,  ""       },
 {L_, "r3,0,3,0",                1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rr4,0,4,0",               2,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     1,   1,    0,   1,     0, e_NVEC,  "j12"    },
 {L_, "W4",                      1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,  ""       },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,  "j123"   }
 },
 { // Enqueue 2 requests, then dispatch: test if the request can be dispatched
   // properly after dispatching other request which consumed all data in the
   // pipe.
 {L_, "W7",                      0,   0,    0,   0,     0, e_NVEC,  ""       },
 {L_, "r3,0,3,0",                1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rt4,(50,1),0,4,0",        2,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     1,   2,    0,   2,     1, e_NVEC,  "j12"    },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,  "3456"   },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,  ""       }
 },
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,   0,    0,   0,     0, e_NVEC,  ""       },
 {L_, "rr3,0,3,0",               1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rb2,0,2,0",               2,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rr6,0,6,0",               3,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     2,   1,    0,   1,     0, e_NVEC,  "j12"    },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC,  "567890" },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,  ""       }
 },
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,   0,    0,   0,     0, e_NVEC,  ""       },
 {L_, "rr3,0,3,0",               1,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rr6,0,6,0",               2,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "rb2,0,2,0",               3,   1,    0,   1,     0, e_NVEC,  ""       },
 {L_, "dr1",                     2,   1,    0,   1,     0, e_NVEC,  "j12"    },
 {L_, "dr1",                     1,   1,    0,   1,     0, e_NVEC,  "345678" },
 {L_, "dr1",                     0,   0,    0,   0,     0, e_NVEC, "90"      },
  {L_,  0,                        0,   0,    0,   0,     0, e_NVEC,  ""       }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_, "W11",                     0,   0,    0,    0,     0, e_NVEC,  ""      },
   {L_, "rr3,0,3,0,{rt2,(40,2),1,2,0}",
                                 1,   1,    0,    1,     0, e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,    2,     1, e_NVEC,  "j12"   },
 {L_, "W4",                      1,   2,    0,    2,     1, e_NVEC,  ""      },
   {L_, "rt1,(60,2),1,1,0,{rr5,0,5,0; rb3,1,3,0}",
                                 2,   2,    0,    2,     1, e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,    2,     1, e_NVEC,  "34"    },
 {L_, "dr1",                     2,   1,    0,    1,     0, e_NVEC,  "5"     },
 {L_, "dr1",                     1,   1,    0,    1,     0, e_NVEC,  "67890" },
 {L_, "dr1",                     0,   0,    0,    0,     0, e_NVEC,  "j12"   },
  {L_,  0,                        0,   0,    0,    0,     0, e_NVEC,  ""      }
 },
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {

            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P(eveManager.numEvents());
                        P_(buf); P(channel.readEventManager()->numTimers());
                        P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 7: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // --------------------------------------------------------------------
        // TESTING 'timedRead()' FUNCTION:
        //   Initiate a non-blocking operation to read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g.,a bufferedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //    in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int timedRead();
        // --------------------------------------------------------------------
          if (verbose) cout << "\nTesting 'timedRead'"
                            << "\n===================" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd                PendR ReadE PendW WriteE Timer Type d_expD
 //------  -----                ----- ----- ----- ------ ----- ---- ------
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 {L_,  "W11",                    0,   0,    0,   0,    0,   e_NVEC,  ""      },
 {L_,  "ir",                     0,   0,    0,   0,    0,   e_NVEC,  ""      },
 {L_,  "rt6,(20,2000),0,-1,0",   0,   0,    0,   0,    0,   e_NVEC,  ""      },
 {L_,  "dr0",                    0,   0,    0,   0,    0,   e_NVEC,  ""      },
  {L_,   0,                       0,   0,    0,   0,    0,   e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W4",                      0,   0,    0,   0,    0,   e_NVEC,  ""      },
 {L_, "rt4,(90,2000),0,4,0",     1,   2,    0,   2,    1,   e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,    0,   e_NVEC,  "j123"  },
  {L_,  0,                        0,   0,    0,   0,    0,   e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "W8",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt4,(100,20),0,4,0",      1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "j123"  },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly.
 {L_, "rt4,(200,10),0,0,0",      1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly when no enough data in the pipe.  The request should be finished
   // successfully after more data is available.
 {L_, "W4",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt6,(100,200),0,6,0",     1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,   2,     1,  e_NVEC,  "j123"  },
 {L_, "W2",                      1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC, "j123j1" },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 1 request, then dispatch: test if the request can be queued
   // properly when no enough data in the pipe.
 {L_, "W4",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt6,(50,2),0,4,0",        1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,   2,     1,  e_NVEC,  "j123"  },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "j123"  },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Fill the readBuffer in the channel first, then issue 1 request and try
   // dispatching properly.
 {L_, "W7",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               1,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "j1"    },
 {L_, "rt4,(650,20),0,4,0",      0,   0,    0,   0,     0,  e_NVEC,  "2345"  },
 {L_, "dr0",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching properly.
 {L_, "W5",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               1,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "j1"    },
 {L_, "rt6,(50,200),0,6,0",      1,   2,    0,   2,     1,  e_NVEC,  "234"   },
 {L_, "W3",                      1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "234j12"},
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Fill readBuffer in the channel but not enough data, and no more data
   // filled the pipe later, issue 1 request and try dispatching properly.
 {L_, "W5",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               1,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "j1"    },
 {L_, "rt6,(50,1),0,3,0",        1,   2,    0,   2,     1,  e_NVEC,  "234"   },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "234"   },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the timed request can be
   // registered correctly.
 {L_, "W7",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "r3,0,3,0",                1,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "rt4,(1,2000),0,4,0",      2,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,   2,     1,  e_NVEC,  "j12"   },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "3456"  },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the timer can be deregistered
   // properly while other requests exist on the queue.
 {L_, "W7",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt4,(250,20),0,4,0",      1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "r3,0,3,0",                2,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   1,    0,   1,     0,  e_NVEC,  "j123"  },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "456"   },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the request can be dispatched
   // properly after dispatching other request which consumed all data in the
   // pipe.
 {L_, "W3",                      0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "r3,0,3,0",                1,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "rt4,(0,2000),0,0,0",      2,   1,    0,   1,     0,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   2,    0,   2,     1,  e_NVEC,  "j12"   },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Enqueue 2 requests, then dispatch: test if the request can be dispatched
   // properly after dispatching other request which consumed all data in the
   // pipe.
 {L_, "W11",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt6,(50,2000),0,6,0",     1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "rb3,0,3,0",               2,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     1,   1,    0,   1,     0,  e_NVEC,  "j12345"},
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "678"   },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 }, /*
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt3,(0,1),0,3,0",         1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               2,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "rt6,(0,2),0,6,0",         3,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     2,   1,    0,   1,     0,  e_NVEC,  "j12"   },
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "567890"},
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 }, */
 { // Enqueue 3 requests, then dispatch: test if every request can be
   // dispatched in the right order.
 {L_, "W11",                     0,   0,    0,   0,     0,  e_NVEC,  ""      },
 {L_, "rt3,(80,1),0,3,0",        1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "rt6,(40,2),0,6,0",        2,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "rb2,0,2,0",               3,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_, "dr1",                     2,   2,    0,   2,     1,  e_NVEC,  "j12"   },
 {L_, "dr1",                     1,   1,    0,   1,     0,  e_NVEC,  "345678"},
 {L_, "dr1",                     0,   0,    0,   0,     0,  e_NVEC,  "90"    },
  {L_,  0,                        0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_,  "W11",                    0,   0,    0,   0,     0,  e_NVEC,  ""      },
   {L_,  "rt3,(30,2),0,3,0,{rt2,(40,2),1,2,0}",
                                 1,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_,  "dr1",                    1,   2,    0,   2,     1,  e_NVEC,  "j12"   },
 {L_,  "W4",                     1,   2,    0,   2,     1,  e_NVEC,  ""      },
   {L_,  "rt1,(60,2),1,1,0,{r5,0,5,0; rt3,(10,2),1,3,0}",
                                 2,   2,    0,   2,     1,  e_NVEC,  ""      },
 {L_,  "dr1",                    1,   2,    0,   2,     1,  e_NVEC,  "34"    },
 {L_,  "dr1",                    2,   1,    0,   1,     0,  e_NVEC,  "5"     },
 {L_,  "dr1",                    1,   2,    0,   2,     1,  e_NVEC,  "67890" },
 {L_,  "dr1",                    0,   0,    0,   0,     0,  e_NVEC,  "j12"   },
  {L_,   0,                       0,   0,    0,   0,     0,  e_NVEC,  ""      }
 },
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P(eveManager.numEvents());
                        P_(buf); P(channel.readEventManager()->numTimers());
                        P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 6: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // ------------------------------------------------------------------
        // TESTING 'read()' FUNCTION:
        //   Initiate a non-blocking operation to read *up *to*
        //   the specified 'numBytes' from this channel into a specified
        //   'buffer'.  The main concerns about the function are (1) if a read
        //   request can be finished right away when the channel recv-buffer is
        //   readable; (2) if a read request will be correctly enqueued to
        //   the request queue when the channel recv-buffer is not readable,
        //   even though the request queue is currently empty; (3) if a read
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the read request in the request queue
        //   will be removed after the request is dispatched and done, or
        //   after a cancel operation; (5) if a read-event will be removed
        //   when no any write request of this read-event exists or will keep
        //   this read-event otherwise; (6) if a different kind of read-event
        //   will be registered when next read-event is another kind of read
        //   request(e.g.,a bufferedRead request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event)
        //    in the channel callback function are hard to be duplicated in
        //   the test driver, and so they are not covered in the test case.
        //   By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values that are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", and "read event queue",
        //   also the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int read();
        // -----------------------------------------------------------------
        if (verbose) cout << "\nTesting 'read'"
                          << "\n==============" << endl;

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line  d_cmd               PendR ReadE PendW WriteE Timer Type   d_expD
 //------  -----               ----- ----- ----- ------ ----- ----   ------
 { // Enqueue 1 request, then dispatch: test if the request can be queued Try
   // reading 0 bytes. properly.
 { L_,   "W5",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "r4,0,4,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,   "W4",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,   "r3,0,3,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,   "dr1",                  0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  { L_,    0,                     0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if the request can retrieve
   // specified length of data from the channel.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "r2,0,2,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Invalidate the channel, enter 1 request, then try dispatching: test if
   // any read request can be issued after the invalidation.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "ir",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "r6,0,-1,0",             0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 1 request, then dispatch: test if a read request can be finished
   // properly when it can't be done once and so this request need to be
   // dispatched again.
 { L_,  "W4",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "r5,0,5,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j123"    },
 { L_,  "W2",                    1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j123j"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if a request can be done right
   // away when there is enough data in the channel's read buffer.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
 { L_,  "r5,0,5,0",              0,   0,    0,   0,    0,  e_NVEC, "23456"   },
 { L_,  "dr0",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Fill readBuffer in the channel but not enough data, then issue 1 request
   // and try dispatching properly.
 { L_, "W5",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_, "rb2,0,2,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "j1"      },
 { L_, "r6,0,6,0",               1,   1,    0,   1,    0,  e_NVEC, "234"     },
 { L_, "W3",                     1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_, "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "234j12"  },
  { L_,  0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if a request can be done right
   // away when there isn't enough data in the channel's read buffer.  This
   // request need to be dequeued then.
 { L_,  "W5",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "j1"      },
 { L_,  "r5,0,5,0",              1,   1,    0,   1,    0,  e_NVEC, "234"     },
 { L_,  "W2",                    1,   1,    0,   1,    0,  e_NVEC, "234"     },

 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "234j1"   },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "r4,0,4,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "r3,0,3,0",              2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j123"    },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "456"     },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
 { // Enqueue 2 requests, then dispatch: test if the different type of read
   // requests can be finished properly in a channel.
 { L_,  "W11",                   0,   0,    0,   0,    0,  e_NVEC, ""        },
 { L_,  "r5,0,5,0",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "rb2,0,2,0",             2,   1,    0,   1,    0,  e_NVEC, ""        },
 { L_,  "dr1",                   1,   1,    0,   1,    0,  e_NVEC, "j1234"   },
 { L_,  "dr1",                   0,   0,    0,   0,    0,  e_NVEC, "56"      },
 { L_,  "r4,0,4,0",              0,   0,    0,   0,    0,  e_NVEC, "7890"    },
  { L_,   0,                      0,   0,    0,   0,    0,  e_NVEC, ""        }
 }, /* 1 of 1000 got error for strncmp ("j12"....)
 { // Test if a channel can still work correctly for new read request.after the
   // previous reads are canceled.
 {L_,  "W11",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "r4,0,0,-1",              1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "cr",                     0,   0,    0,   0,    0,  e_NVEC, ""        },
 {L_,  "r3,1,3,0",               1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  {L_,   0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 }, */
 { // Concern: if this channel can work correctly for new read requests from
   // the user-installed callback function.
 {L_,  "W11",                    0,   0,    0,   0,    0,  e_NVEC, ""        },
   {L_,  "r3,0,3,0,{r2,1,2,0}",
                                 1,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "j12"     },
 {L_,  "W4",                     1,   1,    0,   1,    0,  e_NVEC, ""        },
   {L_,  "r1,1,1,0,{r5,0,5,0; r3,1,3,0}",
                                 2,   1,    0,   1,    0,  e_NVEC, ""        },
 {L_,  "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "34"      },
 {L_,  "dr1",                    2,   1,    0,   1,    0,  e_NVEC, "5"       },
 {L_,  "dr1",                    1,   1,    0,   1,    0,  e_NVEC, "67890"   },
 {L_,  "dr1",                    0,   0,    0,   0,    0,  e_NVEC, "j12"     },
  {L_,   0,                       0,   0,    0,   0,    0,  e_NVEC, ""        }
 },
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF];  memset(buf, '\0', sizeof buf);
            Buffer buffer = {buf, 0, 0, 0, 0};
            btlso::SocketHandle::Handle handles[2];
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     handles,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            btlso::TcpTimerEventManager eveManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eveManager, 0, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        LOOP_ASSERT(LINE, 'W' == *command);
                        if (veryVerbose) {
                            Q("Writing to the control pipe");
                        }
                        int ret = helpWrite(handles[1],
                                            BUFFERS[0].d_sndBuf, length);
                        LOOP_ASSERT(LINE, 0 == ret);
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(i);   P_(j);
                        P_(channel.numPendingReadOperations());
                        P_(eveManager.numEvents());
                        P(channel.readEventManager()->numTimers());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }
                }
            }
            factory.deallocate(sSocket);
        }
#endif
      } break;
      case 5: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS:
        //   The main concern for the test is that: 1.  if a new event
        //   manager after assigned to the channel can work properly;
        //   2.  if all existing I/O requests can be canceled by
        //   'cancelAll()' and new I/O requests can still work properly;
        //   3.  if all existing I/O requests will be still executed, but no
        //   new I/O requests will be accepted anymore.
        //
        // Plan:
        //   First, verify the channel works fine by issuing I/O requests
        //     and checking their execution results.
        //   Next, set different event managers once a time and issue new
        //     I/O requests,  then check their execution results.
        //   Then cancel all requests, followed by issuing new I/O requests,
        //     verifying the execution results.
        //   Lastly, invalidate the channel, followed by issuing new I/O
        //     requests, verifying the execution results.
        //
        // Testing:
        //   btlsos::TcpTimedCbChannel(..., rManager, wManager, ...);
        //   void cancelAll();
        //   void invalidate();
        //   void setReadEventManager();
        //   void setWriteEventManager();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nbtesos_TcpCbChannel(rManager, wManager)."
                          << "\n========================================"
                          << "\nTesting cancelAll()."
                          << "\n===================="
                          << "\nTesting invalidate()."
                          << "\n=====================" << endl;

        {
            btlso::SocketHandle::Handle sock0[2], sock1[2], sock2[2], sock3[2];
            btlso::TcpTimerEventManager
                          rManager0(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager1(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager2(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager3(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          wManager0(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          wManager1(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          wManager2(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          wManager3(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            struct {
                int                               d_line;
                btlso::SocketHandle::Handle       *d_handle;
                btlso::TcpTimerEventManager       *d_rManager;
                btlso::TcpTimerEventManager       *d_wManager;
                const btlso::TcpTimerEventManager *d_exprManager;
                const btlso::TcpTimerEventManager *d_expwManager;
            } OBJECTS[] =
        { //line  d_handle  rManager    wManager    exprManager    expwManager
          //----  --------  --------    --------    -----------    -----------
           {L_,   sock0,   &rManager0,  &wManager0,  &rManager0,   &wManager0},
           {L_,   sock1,   &rManager1,  &wManager1,  &rManager1,   &wManager1},
           {L_,   sock2,   &rManager2,  &wManager2,  &rManager2,   &wManager2},
           {L_,   sock3,   &rManager3,  &wManager3,  &rManager3,   &wManager3},
        };

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_readTimers;       // number of timers in timerEvent
                                            // manager
            int         d_writeTimers;       // number of timers in timerEvent
                                            // manager
        } SCRIPTS[MAX_CMDS] =
{
 //d_line d_cmd              PendR ReadE PendW WriteE rTimer wTimer
 //------ -----              ----- ----- ----- ------ ------ ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

   {L_, "rbt1,(6,8),0,0,-1",     1,    2,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",            2,    2,    0,    0,    1,    0},
   {L_, "w28720,0,28720,0",      2,    2,    0,    0,    1,    0},
   {L_, "wt23720,(6,80),0,0,-1", 2,    2,    1,    2,    1,    1},
   {L_, "w25,1,0,-1",            2,    2,    2,    2,    1,    1},

   {L_, "ca",                    0,    0,    0,    0,    0,    0},
   {L_, "r15,1,0,-1",            1,    1,    0,    0,    0,    0},
   {L_, "wt5,(6,80),1,0,-1",     1,    1,    1,    2,    0,    1},
   {L_, "ia",                    1,    1,    1,    2,    0,    1},
   {L_, "r3,1,0,-1",             1,    1,    1,    2,    0,    1},

   {L_, "w5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "r5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "w9,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_,  0,                      0,    0,    0,    0,    0,    0}

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

   {L_, "rbt1,(6,8),0,0,-1",     1,    2,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",            2,    2,    0,    0,    1,    0},
   {L_, "w1020,0,1020,0",        2,    2,    0,    0,    1,    0},
   {L_, "wt720,(6,80),0,0,-1",   2,    2,    1,    2,    1,    1},
   {L_, "w25,1,0,-1",            2,    2,    2,    2,    1,    1},

   {L_, "ca",                    0,    0,    0,    0,    0,    0},
   {L_, "r15,1,0,-1",            1,    1,    0,    0,    0,    0},
   {L_, "wt5,(6,80),1,0,-1",     1,    1,    1,    2,    0,    1},
   {L_, "ia",                    1,    1,    1,    2,    0,    1},
   {L_, "r3,1,0,-1",             1,    1,    1,    2,    0,    1},

   {L_, "w5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "r5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "w9,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_,  0,                      0,    0,    0,    0,    0,    0}

 #elif BSLS_PLATFORM_OS_LINUX

   {L_, "rbt1,(6,8),0,0,-1",     1,    2,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",            2,    2,    0,    0,    1,    0},
   {L_, "w16383,0,16383,0",      2,    2,    0,    0,    1,    0},
   {L_, "w16383,0,16383,0",      2,    2,    0,    0,    1,    0},
   {L_, "wt33720,(6,80),0,0,-1", 2,    2,    1,    2,    1,    1},
   {L_, "w25,1,0,-1",            2,    2,    2,    2,    1,    1},

   {L_, "ca",                    0,    0,    0,    0,    0,    0},
   {L_, "r15,1,0,-1",            1,    1,    0,    0,    0,    0},
   {L_, "wt5,(6,80),1,0,-1",     1,    1,    1,    2,    0,    1},
   {L_, "ia",                    1,    1,    1,    2,    0,    1},
   {L_, "r3,1,0,-1",             1,    1,    1,    2,    0,    1},

   {L_, "w5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "r5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "w9,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_,  0,                      0,    0,    0,    0,    0,    0}

 #else                                  // sun test data

   {L_, "rbt1,(6,8),0,0,-1",     1,    2,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",            2,    2,    0,    0,    1,    0},
   {L_, "w73720,0,73720,0",      2,    2,    0,    0,    1,    0},
   {L_, "wt33720,(6,80),0,0,-1", 2,    2,    1,    2,    1,    1},
   {L_, "w25,1,0,-1",            2,    2,    2,    2,    1,    1},

   {L_, "ca",                    0,    0,    0,    0,    0,    0},
   {L_, "r15,1,0,-1",            1,    1,    0,    0,    0,    0},
   {L_, "wt5,(6,80),1,0,-1",     1,    1,    1,    2,    0,    1},
   {L_, "ia",                    1,    1,    1,    2,    0,    1},
   {L_, "r3,1,0,-1",             1,    1,    1,    2,    0,    1},

   {L_, "w5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "r5,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_, "w9,1,0,-1",             1,    1,    1,    2,    0,    1},
   {L_,  0,                      0,    0,    0,    0,    0,    0}

 #endif
};
// The normal alignment should be as the following statement:

            // the number of channel objects to be created for test
            const int NUM_OBJS = sizeof OBJECTS / sizeof *OBJECTS;

            for (int i = 0; i < NUM_OBJS; ++i) {
                int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     OBJECTS[i].d_handle,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::SocketOptUtil::setOption(OBJECTS[i].d_handle[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
                LOOP_ASSERT(i, 0 == ret);

                ret = btlso::SocketOptUtil::setOption(OBJECTS[i].d_handle[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
                LOOP_ASSERT(i, 0 == ret);

                btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
                btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                factory.allocate(OBJECTS[i].d_handle[0]);
                LOOP_ASSERT(i, sSocket);

                {
                    // We should guarantee that the 'channel's destructor is
                    // invoked before the corresponding 'streamSocket'
                    // destructor.  We insure the required order by creating
                    // the 'channel' inside a block while the corresponding
                    // 'streamSocket' object is created outside the block as
                    // above.

                    btlsos::TcpTimedCbChannel channel(sSocket,
                                                     OBJECTS[i].d_rManager,
                                                     OBJECTS[i].d_wManager,
                                                     &testAllocator);
                    // Verify the initial state values.
                    LOOP_ASSERT(i, sSocket == channel.socket());
                    LOOP_ASSERT(i, 0 == channel.isInvalidRead());
                    LOOP_ASSERT(i, 0 == channel.isInvalidWrite());
                    LOOP_ASSERT(i, 0 == channel.numPendingReadOperations());
                    LOOP_ASSERT(i, 0 == channel.numPendingWriteOperations());
                    LOOP_ASSERT(i, OBJECTS[i].d_exprManager ==
                                          channel.readEventManager());
                    LOOP_ASSERT(i, OBJECTS[i].d_expwManager ==
                                          channel.writeEventManager());
                    LOOP_ASSERT(i, 0 ==
                                     channel.readEventManager()->numEvents());
                    LOOP_ASSERT(i, 0 ==
                                     channel.writeEventManager()->numEvents());

                    memset(readBuf, '\0', sizeof readBuf);
                    Buffer buffer = {readBuf, str, 0, 0, 0};

                    for (int j = 0; j < MAX_CMDS; ++j) {
                        const char *command = SCRIPTS[j].d_cmd;
                        if (!command) {
                            break;
                        }
                        const int LINE = SCRIPTS[j].d_line;

                        // Buffer is a struct type where declares different
                        // pointers pointing to different I/O buffers, e.g.,
                        // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.
                        // This struct is declared at the beginning of this
                        // test driver.

                        int length = gg(&channel, &buffer,
                                        OBJECTS[i].d_rManager, 0, command);

                        // There are 9 parameters in the bufferedReadCallback()
                        // function.  This is the maximum number of parameters
                        // to call makeF() for a functor object.  If we have
                        // the following commands executed in gg(), the socket
                        // handle needs to be passed to gg() and the
                        // bufferedReadCallback().  To do so, we need to use a
                        // struct to wrap some of the parameters to reduce the
                        // number of function parameters, but doing so will
                        // make the function call less straightforward.  That's
                        // why we execute the following commands outside gg().

                        if (length > 0) {
                            LOOP_ASSERT(LINE, 'W' == *command);
                            if (veryVerbose) {
                                Q("Writing to the control pipe");
                            }
                            int ret = helpWrite(OBJECTS[i].d_handle[1],
                                                str, length);
                            LOOP_ASSERT(LINE, 0 == ret);
                        }
                        LOOP_ASSERT(LINE, 0 <= length);
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                        if (channel.readEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                            LOOP_ASSERT(LINE, SCRIPTS[j].d_readTimers ==
                                     channel.readEventManager()->numTimers());
                        }
                        if (channel.writeEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                            LOOP_ASSERT(LINE, SCRIPTS[j].d_writeTimers ==
                                     channel.writeEventManager()->numTimers());

                        }
                        if (veryVerbose) {
                            P_(i);  P_(j);
                            P_(channel.numPendingReadOperations());
                            P(channel.readEventManager()->numEvents());
                            P_(channel.numPendingWriteOperations());
                            P(channel.writeEventManager()->numEvents());
                            P(channel.readEventManager()->numTimers());
                            P(channel.writeEventManager()->numTimers());
                            cout << endl;
                        }
                    }
                }
                factory.deallocate(sSocket);
            }
        }
        if (verbose) cout << "\nTesting setReadEventManager()."
                          << "\n=============================="
                          << "\nTesting setWriteEventManager()."
                          << "\n===============================" << endl;
        {
            btlso::SocketHandle::Handle socket[2];
            btlso::TcpTimerEventManager
                          rManager0(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager1(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager2(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                          rManager3(btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

            struct {
                int                         d_line;
                btlso::TcpTimerEventManager *d_rManager;
                btlso::TcpTimerEventManager *d_wManager;
                btlso::TcpTimerEventManager *d_exprManager;
                btlso::TcpTimerEventManager *d_expwManager;
            } OBJECTS[] =
            { //line    rManager     wManager    exprManager    expwManager
              //----    --------     --------    -----------    -----------
              {L_,        0,           0,        &rManager0,    &rManager0},
              {L_,    &rManager1,      0,        &rManager1,    &rManager0},
              {L_,        0,       &rManager2,   &rManager1,    &rManager2},
              {L_,    &rManager3,  &rManager3,   &rManager3,    &rManager3},
            };

            enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };
        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_readTimers;       // number of timers in timerEvent
                                            // manager
            int         d_writeTimers;      // number of timers in timerEvent
                                            // manager
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd                 PendR ReadE PendW WriteE rTimer wTimer
 //------ -----                 ----- ----- ----- ------ ------ ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

  {
   { L_, "rbt1,(5,90),0,0,-1",      1,    2,    0,    2,    1,    1},
   { L_, "rb4,1,0,-1",              2,    2,    0,    2,    1,    1},
   { L_, "w28720,0,28720,0",        2,    2,    0,    2,    1,    1},
   { L_, "wt23720,(6,7),0,23720,0", 2,    4,    1,    4,    2,    2},
   { L_, "wt220,(4,8),0,0,-1",      2,    4,    2,    4,    2,    2},
         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request.
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
   { L_, "R22729",                  0,    2,    2,    2,    1,    1},
   { L_, "R12729",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    2,    1,    2,    1,    1},
   { L_, "rt15,(3,20),1,0,-1",      1,    4,    1,    4,    2,    2},

   { L_, "cw",                      1,    2,    0,    2,    1,    1},
   { L_, "wt28720,(3,2),0,0,-1",    1,    4,    1,    4,    2,    2},
   { L_, "r3,1,0,-1",               2,    4,    1,    4,    2,    2},
   { L_, "wt5,(5,3),1,0,-1",        2,    4,    2,    4,    2,    2},
   { L_, "ca",                      0,    0,    0,    0,    0,    0},

   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(15,9),0,8,0",        1,    2,    0,    0,    1,    0},
   { L_, "rb4,1,0,-1",              2,    2,    0,    0,    1,    0},
   { L_, "wt20,(16,100),0,0,-1",    2,    2,    1,    2,    1,    1},
   { L_, "wt720,(17,9),0,0,-1",     2,    2,    2,    2,    1,    1},
   { L_, "w220,0,0,-1",             2,    2,    3,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    1,    0,    0,    0,    0},
   { L_, "w5,1,5,0",                1,    1,    1,    1,    0,    0},
   { L_, "R22729",                  1,    1,    1,    1,    0,    0},

   { L_, "dw1",                     1,    1,    0,    0,    0,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(5,9),0,8,0",         1,    2,    0,    0,    1,    0},
   { L_, "rbt4,(5,9),1,0,-1",       2,    2,    0,    0,    1,    0},
   { L_, "wt28720,(5,9),0,28720,0", 2,    2,    0,    0,    1,    0},
   { L_, "wt23720,(5,9),0,0,-1",    2,    2,    1,    2,    1,    1},
   { L_, "wt20,(3,5),0,0,-1",       2,    2,    2,    2,    1,    1},

     // Verify that the read is not affected by cancelWrite() by dispatching a
     // write request.
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    2,    0,    0,    1,    0},
   { L_, "w5,1,5,0",                1,    2,    1,    1,    1,    0},
   { L_, "R25729",                  1,    2,    1,    1,    1,    0},

   { L_, "R25729",                  1,    2,    1,    1,    1,    0},
   { L_, "dw1",                     1,    2,    0,    0,    1,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "r18,0,0,-1",              1,    1,    0,    1,    0,    0},
   { L_, "rbt4,(5,4),1,0,-1",       2,    1,    0,    1,    0,    0},
   { L_, "w25720,0,25720,0",        2,    1,    0,    1,    0,    0},
   { L_, "wt13720,(5,6),0,13720,0", 2,    3,    1,    3,    1,    1},
   { L_, "wt720,(0,0),0,0,0",       2,    3,    2,    3,    1,    1},
     // Verify that the read is not affected by cancelWrite() by dispatching a
     // write request.
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
   { L_, "R32729",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    2,    1,    2,    1,    1},
   { L_, "W55",                     0,    2,    1,    2,    1,    1},
   { L_, "r55,1,55,0",              1,    3,    1,    3,    1,    1},

   { L_, "dr1",                     0,    2,    1,    2,    1,    1},
   { L_, "dr1",                     0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data

 {
   { L_, "rbt1,(255,90),0,0,-1",    1,    2,    0,    2,    1,    1},
   { L_, "rb4,1,0,-1",              2,    2,    0,    2,    1,    1},
   { L_, "w1020,0,1020,0",          2,    2,    0,    2,    1,    1},
   { L_, "wt23,(266,7),0,23,0",     2,    4,    1,    4,    2,    2},
   { L_, "wt220,(270,8),0,0,-1",    2,    4,    2,    4,    2,    2},
         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request.
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
   { L_, "R729",                    0,    2,    2,    2,    1,    1},
   { L_, "R729",                    0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    2,    1,    2,    1,    1},
   { L_, "rt15,(283,20),1,0,-1",    1,    4,    1,    4,    2,    2},

   { L_, "cw",                      1,    2,    0,    2,    1,    1},
   { L_, "wt1020,(288,2),0,0,-1",   1,    4,    1,    4,    2,    2},
   { L_, "r3,1,0,-1",               2,    4,    1,    4,    2,    2},
   { L_, "wt5,(295,3),1,0,-1",      2,    4,    2,    4,    2,    2},
   { L_, "ca",                      0,    0,    0,    0,    0,    0},

   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(255,9),0,8,0",       1,    2,    0,    0,    1,    0},
   { L_, "rb4,1,0,-1",              2,    2,    0,    0,    1,    0},
   { L_, "wt200,(265,100),0,0,-1",  2,    2,    1,    2,    1,    1},
   { L_, "wt720,(275,9),0,0,-1",    2,    2,    2,    2,    1,    1},
   { L_, "w220,0,0,-1",             2,    2,    3,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    1,    0,    0,    0,    0},
   { L_, "w1550,1,1550,0",          1,    1,    1,    1,    0,    0},
   { L_, "R729",                    1,    1,    1,    1,    0,    0},

   { L_, "dw1",                     1,    1,    0,    0,    0,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(255,9),0,8,0",       1,    2,    0,    0,    1,    0},
   { L_, "rbt4,(260,9),1,0,-1",     2,    2,    0,    0,    1,    0},
   { L_, "wt1020,(265,9),0,0,-1",   2,    2,    1,    2,    1,    1},
   { L_, "wt20,(275,5),0,0,-1",     2,    2,    2,    2,    1,    1},
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    2,    0,    0,    1,    0},
   { L_, "w1550,1,1550,0",          1,    2,    1,    1,    1,    0},
   { L_, "R729",                    1,    2,    1,    1,    1,    0},

   { L_, "dw1",                     1,    2,    0,    0,    1,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "r8,0,0,-1",               1,    1,    0,    1,    0,    0},
   { L_, "rbt4,(255,4),1,0,-1",     2,    1,    0,    1,    0,    0},
   { L_, "wt720,(265,6),0,720,0",   2,    3,    1,    3,    1,    1},
   { L_, "w25,0,25,0",              2,    3,    2,    3,    1,    1},
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "R729",                    0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    1,    1,    1,    0,    0},
   { L_, "W55",                     0,    1,    1,    1,    0,    0},
   { L_, "R729",                    0,    1,    1,    1,    0,    0},
   { L_, "r50,1,50,0",              1,    2,    1,    2,    0,    0},
   { L_, "dr2",                     0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },

 #elif BSLS_PLATFORM_OS_LINUX

 {
   { L_, "rbt1,(5,90),0,0,-1",      1,    2,    0,    2,    1,    1},
   { L_, "rb4,1,0,-1",              2,    2,    0,    2,    1,    1},
   { L_, "w16383,0,16383,0",        2,    2,    0,    2,    1,    1},
   { L_, "wt18000,(900,7),0,18000,0", 2,    4,    1,    4,    2,    2},
   { L_, "wt200,(250,8),0,0,-1",    2,    4,    2,    4,    2,    2},
         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request.
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
   { L_, "R16383",                  0,    2,    2,    2,    1,    1},
   { L_, "R16383",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    2,    1,    2,    1,    1},
   { L_, "R16383",                  0,    2,    1,    2,    1,    1},
   { L_, "rt15,(3,20),1,0,-1",      1,    4,    1,    4,    2,    2},

   { L_, "cw",                      1,    2,    0,    2,    1,    1},
   { L_, "wt20000,(3,2),0,0,-1",    1,    4,    1,    4,    2,    2},
   { L_, "r3,1,0,-1",               2,    4,    1,    4,    2,    2},
   { L_, "wt5,(5,3),1,0,-1",        2,    4,    2,    4,    2,    2},
   { L_, "ca",                      0,    0,    0,    0,    0,    0},

   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(5,9),0,8,0",         1,    2,    0,    0,    1,    0},
   { L_, "rb4,1,0,-1",              2,    2,    0,    0,    1,    0},
   { L_, "wt20,(20,100),0,0,-1",    2,    2,    1,    2,    1,    1},
   { L_, "wt33000,(5,9),0,0,-1",    2,    2,    2,    2,    1,    1},
   { L_, "w220,0,0,-1",             2,    2,    3,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    1,    0,    0,    0,    0},
   { L_, "w20000,0,20000,0",        1,    1,    1,    1,    0,    0},
   { L_, "R22729",                  1,    1,    1,    1,    0,    0},

   { L_, "dw1",                     1,    1,    0,    0,    0,    0},
   { L_, "R16383",                  1,    1,    0,    0,    0,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(5,9),0,8,0",         1,    2,    0,    0,    1,    0},
   { L_, "rbt4,(5,9),1,0,-1",       2,    2,    0,    0,    1,    0},
   { L_, "wt25000,(5,9),0,0,-1",    2,    2,    1,    2,    1,    1},
   { L_, "wt20,(3,5),0,0,-1",       2,    2,    2,    2,    1,    1},
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    2,    0,    0,    1,    0},
   { L_, "w20000,1,20000,0",        1,    2,    1,    1,    1,    0},
   { L_, "R16383",                  1,    2,    1,    1,    1,    0},
   { L_, "R16383",                  1,    2,    1,    1,    1,    0},

   { L_, "dw1",                     1,    2,    0,    0,    1,    0},
   { L_, "R16383",                  1,    2,    0,    0,    1,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "r8,0,0,-1",               1,    1,    0,    1,    0,    0},
   { L_, "rbt4,(5,4),1,0,-1",       2,    1,    0,    1,    0,    0},
   { L_, "w16383,0,16383,0",        2,    1,    0,    1,    0,    0},
   { L_, "w16383,0,16383,0",        2,    1,    0,    1,    0,    0},
   { L_, "wt2000,(400,6),0,2000,0", 2,   3,    1,    3,    1,    1},
   { L_, "w720,0,720,0",            2,    3,    2,    3,    1,    1},
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "R16383",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    1,    1,    1,    0,    0},
   { L_, "R16383",                  0,    1,    1,    1,    0,    0},
   { L_, "W55",                     0,    1,    1,    1,    0,    0},
   { L_, "r55,1,55,0",              1,    2,    1,    2,    0,    0},
   { L_, "dr2",                     0,    0,    0,    0,    0,    0},
   { L_, "dr0",                     0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 #else                                  // sun test data

 {
   { L_, "rbt1,(5,90),0,0,-1",      1,    2,    0,    2,    1,    1},
   { L_, "rb4,1,0,-1",              2,    2,    0,    2,    1,    1},
   { L_, "w73720,0,73720,0",        2,    2,    0,    2,    1,    1},
   { L_, "wt33720,(6,7),0,33720,0", 2,    4,    1,    4,    2,    2},
   { L_, "wt220,(4,8),0,0,-1",      2,    4,    2,    4,    2,    2},
         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request.
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
   { L_, "R32729",                  0,    2,    2,    2,    1,    1},
   { L_, "R22729",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    2,    1,    2,    1,    1},
   { L_, "rt15,(3,20),1,0,-1",      1,    4,    1,    4,    2,    2},

   { L_, "cw",                      1,    2,    0,    2,    1,    1},
   { L_, "wt73720,(3,2),0,0,-1",    1,    4,    1,    4,    2,    2},
   { L_, "r3,1,0,-1",               2,    4,    1,    4,    2,    2},
   { L_, "wt5,(5,3),1,0,-1",        2,    4,    2,    4,    2,    2},
   { L_, "ca",                      0,    0,    0,    0,    0,    0},

   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(5,9),0,8,0",         1,    2,    0,    0,    1,    0},
   { L_, "rb4,1,0,-1",              2,    2,    0,    0,    1,    0},
   { L_, "wt20,(20,100),0,0,-1",     2,    2,    1,    2,    1,    1},
   { L_, "wt720,(5,9),0,0,-1",      2,    2,    2,    2,    1,    1},
   { L_, "w220,0,0,-1",             2,    2,    3,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    1,    0,    0,    0,    0},
   { L_, "w5,1,5,0",                1,    1,    1,    1,    0,    0},
   { L_, "R22729",                  1,    1,    1,    1,    0,    0},

   { L_, "dw1",                     1,    1,    0,    0,    0,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "rt8,(5,9),0,8,0",         1,    2,    0,    0,    1,    0},
   { L_, "rbt4,(5,9),1,0,-1",       2,    2,    0,    0,    1,    0},
   { L_, "wt73720,(5,9),0,0,-1",    2,    2,    1,    2,    1,    1},
   { L_, "wt20,(3,5),0,0,-1",       2,    2,    2,    2,    1,    1},
   { L_, "cw",                      2,    2,    0,    0,    1,    0},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "W8",                      2,    2,    0,    0,    1,    0},
   { L_, "dr1",                     1,    2,    0,    0,    1,    0},
   { L_, "w5,1,5,0",                1,    2,    1,    1,    1,    0},
   { L_, "R35729",                  1,    2,    1,    1,    1,    0},
   { L_, "R35729",                  1,    2,    1,    1,    1,    0},

   { L_, "dw1",                     1,    2,    0,    0,    1,    0},
   { L_, "cr",                      0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 {
   { L_, "r8,0,0,-1",               1,    1,    0,    1,    0,    0},
   { L_, "rbt4,(5,4),1,0,-1",       2,    1,    0,    1,    0,    0},
   { L_, "w63720,0,63720,0",        2,    1,    0,    1,    0,    0},
   { L_, "wt13720,(5,6),0,13720,0", 2,    3,    1,    3,    1,    1},
   { L_, "w720,0,720,0",            2,    3,    2,    3,    1,    1},
   { L_, "cr",                      0,    2,    2,    2,    1,    1},
          // Verify that the read is not affected by cancelWrite() by
          // dispatching a write request.
   { L_, "R52729",                  0,    2,    2,    2,    1,    1},
   { L_, "dw1",                     0,    1,    1,    1,    0,    0},
   { L_, "W55",                     0,    1,    1,    1,    0,    0},
   { L_, "r55,1,55,0",              1,    2,    1,    2,    0,    0},
   { L_, "dr2",                     0,    0,    0,    0,    0,    0},
   { L_, "dr0",                     0,    0,    0,    0,    0,    0},
   { L_,  0,                        0,    0,    0,    0,    0,    0}
 },
 #endif
};
// The normal alignment should be as the following statement:
// the number of channel objects to be created for test

            const int NUM_OBJS = sizeof OBJECTS / sizeof *OBJECTS;

            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     socket,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, BUF_LIMIT);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_RECEIVEBUFFER, BUF_LIMIT);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[0],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            ASSERT(0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDLOWATER, WRITELOWWATER);
            // LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(socket[1],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                     btlso::SocketOptUtil::k_RECEIVELOWATER, READLOWWATER);
            //LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                factory.allocate(socket[0]);
            ASSERT(sSocket);

            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                factory.allocate(socket[1]);
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.
                btlsos::TcpTimedCbChannel channel(sSocket,
                                            &rManager0,
                                            &testAllocator);

                for (int i = 0; i < NUM_OBJS; ++i) {
                    if (OBJECTS[i].d_rManager) {
                        channel.setReadEventManager(OBJECTS[i].d_rManager);
                    }
                    if (OBJECTS[i].d_wManager) {
                        channel.setWriteEventManager(OBJECTS[i].d_wManager);
                    }

                    // Verify the initial state values.
                    LOOP_ASSERT(i, sSocket == channel.socket());
                    LOOP_ASSERT(i, 0 == channel.isInvalidRead());
                    LOOP_ASSERT(i, 0 == channel.isInvalidWrite());
                    LOOP_ASSERT(i, 0 == channel.numPendingReadOperations());
                    LOOP_ASSERT(i, 0 == channel.numPendingWriteOperations());
                    LOOP_ASSERT(i, OBJECTS[i].d_exprManager ==
                                          channel.readEventManager());
                    LOOP_ASSERT(i, OBJECTS[i].d_expwManager ==
                                          channel.writeEventManager());
                    LOOP_ASSERT(i, 0 ==
                                     channel.readEventManager()->numEvents());
                    LOOP_ASSERT(i, 0 ==
                                     channel.writeEventManager()->numEvents());

                    memset(readBuf, '\0', sizeof readBuf);
                    Buffer buffer = {readBuf, str, 0, 0, 0};

                    for (int j = 0; j < MAX_CMDS; ++j) {
                        const char *command = SCRIPTS[i][j].d_cmd;
                        if (!command) {
                            break;
                        }
                        const int LINE = SCRIPTS[i][j].d_line;

                        // Buffer is a struct type where declares different
                        // pointers pointing to different I/O buffers, e.g.,
                        // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.
                        // This struct is declared at the beginning of this
                        // test driver.

                        int length = gg(&channel, &buffer,
                                        OBJECTS[i].d_exprManager,
                                        OBJECTS[i].d_expwManager,
                                        command);

                        // There are 9 parameters in the bufferedReadCallback()
                        // function.  This is the maximum number of parameters
                        // to call makeF() for a functor object.  If we have
                        // the following commands executed in gg(), the socket
                        // handle needs to be passed to gg() and the
                        // bufferedReadCallback().  To do so, we need to use a
                        // struct to wrap some of the parameters to reduce the
                        // number of function parameters, but doing so will
                        // make the function call less straightforward.  That's
                        // why we execute the following commands outside gg().

                        if (length > 0) {
                            if ('W' == *command) {
                                if (veryVerbose) {
                                    Q("Writing to the control pipe");
                                }
                                int ret = helpWrite(socket[1],
                                                    str, length);
                                LOOP_ASSERT(LINE, 0 == ret);
                            }
                            else if ('R' == *command) {
                                memset(readBuf, '\0', sizeof readBuf);
                                ret = cSocket->read(readBuf, length);
                                if (veryVeryVerbose) {
                                    cout << "help read return: "
                                         << ret << endl;
                                }
                            }
                        }
                        LOOP_ASSERT(LINE, 0 <= length);
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                        if (channel.readEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_readTimers ==
                                     channel.readEventManager()->numTimers());
                        }
                        if (channel.writeEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent
                                  == channel.writeEventManager()->numEvents());
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_writeTimers ==
                                     channel.writeEventManager()->numTimers());
                        }
                        if (veryVerbose) {
                            P_(i);  P_(j);
                            P_(channel.numPendingReadOperations());
                            P(channel.readEventManager()->numEvents());
                            P_(channel.numPendingWriteOperations());
                            P(channel.writeEventManager()->numEvents());
                            P(channel.readEventManager()->numTimers());
                            P(channel.writeEventManager()->numTimers());
                            cout << endl;
                        }
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 4: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //   Each accessor directly returns the internal representations of
        //   corresponding values.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the direct accessors returns the expected value.
        //
        // Testing:
        //   int isInvalidRead() const;
        //   int isInvalidWrite() const;
        //   int numPendingReadOperations() const;
        //   int numPendingWriteOperations() const;
        //   btlso::StreamSocket<btlso::IPv4Address> *socket() const;
        //   btlso::TimerEventManager *readEventManager() const;
        //   btlso::TimerEventManager *writeEventManager() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: basic accessors."
                          << "\n=========================" << endl;

        btlso::SocketHandle::Handle sock0[2], sock2[2];
        btlso::TcpTimerEventManager rManager0(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager1(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager2(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager3(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

        struct {
            int                         d_line;
            btlso::SocketHandle::Handle *d_handle;
            btlso::TcpTimerEventManager *d_rManager;
            btlso::TcpTimerEventManager *d_wManager;
            btlso::TcpTimerEventManager *d_exprManager;
            btlso::TcpTimerEventManager *d_expwManager;

        } OBJECTS[] =
        { //line   d_handle  rManager   wManager    exprManager   expwManager
          //----------------------------------------------------------------
          {L_,    sock0,       0,          0,      &rManager0,   &rManager0 },
          {L_,    sock2,       0,      &rManager1, &rManager0,   &rManager1 },
        };

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_isInvalidReadFlag;
            int         d_isInvalidWriteFlag;

            int         d_readTimers;       // number of timers in timerEvent
                                            // manager
            int         d_writeTimers;       // number of timers in timerEvent
                                            // manager
        } SCRIPTS[][MAX_CMDS] =
{
 //d_line d_cmd           PendR ReadE PendW WriteE validR validW rTimer wTimer
 //------ -----           ----- ----- ----- ------ ------ ------ ------ ------
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    2,    0,    0,    1,    1},
   {L_, "rb4,1,0,-1",         2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w28720,0,28720,0",   2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w23720,0,23720,0",   2,    3,    1,    3,    0,    0,    1,    1},
   {L_, "wt220,(9,6),0,0,-1", 2,    3,    2,    3,    0,    0,    1,    1},

   {L_, "cr",                 0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R13729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R12729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    2,    1,    2,    0,    0,    1,    1},
   {L_, "r15,1,0,-1",         1,    3,    1,    3,    0,    0,    1,    1},

   {L_, "cw",                 1,    1,    0,    1,    0,    0,    0,    0},
   {L_, "wt28720,(5,9),0,0,-1",
                              1,    3,    1,    3,    0,    0,    1,    1},
   {L_, "ir",                 1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "r3,1,0,-1",          1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "w5,1,0,-1",          1,    3,    2,    3,    1,    0,    1,    1},

   {L_, "iw",                 1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "r5,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "w9,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    0,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",         2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w28720,0,28720,0",   2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w23720,0,23720,0",   2,    2,    1,    1,    0,    0,    1,    0},
   {L_, "wt220,(9,6),0,0,-1", 2,    2,    2,    1,    0,    0,    1,    0},

   {L_, "cr",                 0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R12729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R12729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    0,    1,    2,    0,    0,    0,    1},
   {L_, "r15,1,0,-1",         1,    1,    1,    2,    0,    0,    0,    1},

   {L_, "cw",                 1,    1,    0,    0,    0,    0,    0,    0},
   {L_, "wt28720,(5,9),0,0,-1",
                              1,    1,    1,    2,    0,    0,    0,    1},
   {L_, "ir",                 1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "r3,1,0,-1",          1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "w5,1,0,-1",          1,    1,    2,    2,    1,    0,    0,    1},

   {L_, "iw",                 1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "r5,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "w9,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    2,    0,    0,    1,    1},
   {L_, "rb4,1,0,-1",         2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w1020,0,1020,0",     2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w720,0,720,0",       2,    3,    1,    3,    0,    0,    1,    1},
   {L_, "wt220,(9,6),0,0,-1", 2,    3,    2,    3,    0,    0,    1,    1},

   {L_, "cr",                 0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R529",               0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R529",               0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    2,    1,    2,    0,    0,    1,    1},
   {L_, "r15,1,0,-1",         1,    3,    1,    3,    0,    0,    1,    1},

   {L_, "cw",                 1,    1,    0,    1,    0,    0,    0,    0},
   {L_, "wt73720,(5,9),0,0,-1",
                              1,    3,    1,    3,    0,    0,    1,    1},
   {L_, "ir",                 1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "r3,1,0,-1",          1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "w5,1,0,-1",          1,    3,    2,    3,    1,    0,    1,    1},

   {L_, "iw",                 1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "r5,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "w9,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    0,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",         2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w1020,0,1020,0",     2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w720,0,720,0",       2,    2,    1,    1,    0,    0,    1,    0},
   {L_, "wt220,(9,6),0,0,-1", 2,    2,    2,    1,    0,    0,    1,    0},

   {L_, "cr",                 0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R529",               0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R529",               0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    0,    1,    2,    0,    0,    0,    1},
   {L_, "r15,1,0,-1",         1,    1,    1,    2,    0,    0,    0,    1},

   {L_, "cw",                 1,    1,    0,    0,    0,    0,    0,    0},
   {L_, "wt2048,(5,9),0,0,-1",
                              1,    1,    1,    2,    0,    0,    0,    1},
   {L_, "ir",                 1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "r3,1,0,-1",          1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "w5,1,0,-1",          1,    1,    2,    2,    1,    0,    0,    1},

   {L_, "iw",                 1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "r5,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "w9,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },

 #elif BSLS_PLATFORM_OS_LINUX
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    2,    0,    0,    1,    1},
   {L_, "rb4,1,0,-1",         2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w16383,0,16383,0",   2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w20000,0,20000,0",   2,    3,    1,    3,    0,    0,    1,    1},
   {L_, "wt220,(9,6),0,0,-1", 2,    3,    2,    3,    0,    0,    1,    1},

   {L_, "cr",                 0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R32729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R22729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    2,    1,    2,    0,    0,    1,    1},
   {L_, "r15,1,0,-1",         1,    3,    1,    3,    0,    0,    1,    1},

   {L_, "cw",                 1,    1,    0,    1,    0,    0,    0,    0},
   {L_, "wt17000,(5,9),0,0,-1",
                              1,    3,    1,    3,    0,    0,    1,    1},
   {L_, "ir",                 1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "r3,1,0,-1",          1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "w5,1,0,-1",          1,    3,    2,    3,    1,    0,    1,    1},

   {L_, "iw",                 1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "r5,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "w9,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    0,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",         2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w16383,0,16383,0",   2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w20000,0,20000,0",   2,    2,    1,    1,    0,    0,    1,    0},
   {L_, "wt220,(9,6),0,0,-1", 2,    2,    2,    1,    0,    0,    1,    0},

   {L_, "cr",                 0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R32729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R22729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    0,    1,    2,    0,    0,    0,    1},
   {L_, "r15,1,0,-1",         1,    1,    1,    2,    0,    0,    0,    1},

   {L_, "cw",                 1,    1,    0,    0,    0,    0,    0,    0},
   {L_, "wt17000,(5,9),0,0,-1",
                              1,    1,    1,    2,    0,    0,    0,    1},
   {L_, "ir",                 1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "r3,1,0,-1",          1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "w5,1,0,-1",          1,    1,    2,    2,    1,    0,    0,    1},

   {L_, "iw",                 1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "r5,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "w9,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },

 #else                                  // sun test data
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    2,    0,    0,    1,    1},
   {L_, "rb4,1,0,-1",         2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w73720,0,73720,0",   2,    2,    0,    2,    0,    0,    1,    1},
   {L_, "w33720,0,33720,0",   2,    3,    1,    3,    0,    0,    1,    1},
   {L_, "wt220,(9,6),0,0,-1", 2,    3,    2,    3,    0,    0,    1,    1},

   {L_, "cr",                 0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R32729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "R22729",             0,    1,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    2,    1,    2,    0,    0,    1,    1},
   {L_, "r15,1,0,-1",         1,    3,    1,    3,    0,    0,    1,    1},

   {L_, "cw",                 1,    1,    0,    1,    0,    0,    0,    0},
   {L_, "wt73720,(5,9),0,0,-1",
                              1,    3,    1,    3,    0,    0,    1,    1},
   {L_, "ir",                 1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "r3,1,0,-1",          1,    3,    1,    3,    1,    0,    1,    1},
   {L_, "w5,1,0,-1",          1,    3,    2,    3,    1,    0,    1,    1},

   {L_, "iw",                 1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "r5,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},
   {L_, "w9,1,0,-1",          1,    3,    2,    3,    1,    1,    1,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },
 {
   {L_, "rbt1,(4,6),0,0,-1",  1,    2,    0,    0,    0,    0,    1,    0},
   {L_, "rb4,1,0,-1",         2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w73720,0,73720,0",   2,    2,    0,    0,    0,    0,    1,    0},
   {L_, "w33720,0,33720,0",   2,    2,    1,    1,    0,    0,    1,    0},
   {L_, "wt220,(9,6),0,0,-1", 2,    2,    2,    1,    0,    0,    1,    0},

   {L_, "cr",                 0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R32729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "R22729",             0,    0,    2,    1,    0,    0,    0,    0},
   {L_, "dw1",                0,    0,    1,    2,    0,    0,    0,    1},
   {L_, "r15,1,0,-1",         1,    1,    1,    2,    0,    0,    0,    1},

   {L_, "cw",                 1,    1,    0,    0,    0,    0,    0,    0},
   {L_, "wt73720,(5,9),0,0,-1",
                              1,    1,    1,    2,    0,    0,    0,    1},
   {L_, "ir",                 1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "r3,1,0,-1",          1,    1,    1,    2,    1,    0,    0,    1},
   {L_, "w5,1,0,-1",          1,    1,    2,    2,    1,    0,    0,    1},

   {L_, "iw",                 1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "r5,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},
   {L_, "w9,1,0,-1",          1,    1,    2,    2,    1,    1,    0,    1},

   {L_,  0,                   0,    0,    0,    0,    0,    0,    0,    0}
 },

 #endif
};

// The normal alignment should be as the following statement:

        // the number of channel objects to be created for test
        const int NUM_OBJS = sizeof OBJECTS / sizeof *OBJECTS;

        for (int i = 0; i < NUM_OBJS; ++i) {
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     OBJECTS[i].d_handle,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(OBJECTS[i].d_handle[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(OBJECTS[i].d_handle[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                factory.allocate(OBJECTS[i].d_handle[0]);
            LOOP_ASSERT(i, sSocket);
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                factory.allocate(OBJECTS[i].d_handle[1]);
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpTimedCbChannel channel(sSocket,
                                            &rManager0,
                                            &testAllocator);
                if (OBJECTS[i].d_rManager) {
                    channel.setReadEventManager(OBJECTS[i].d_rManager);
                }
                if (OBJECTS[i].d_wManager) {
                    channel.setWriteEventManager(OBJECTS[i].d_wManager);
                }

                // Verify the initial state values.
                LOOP_ASSERT(i, sSocket == channel.socket());
                LOOP_ASSERT(i, 0 == channel.isInvalidRead());
                LOOP_ASSERT(i, 0 == channel.isInvalidWrite());
                LOOP_ASSERT(i, 0 == channel.numPendingReadOperations());
                LOOP_ASSERT(i, 0 == channel.numPendingWriteOperations());
                LOOP_ASSERT(i, OBJECTS[i].d_exprManager ==
                                          channel.readEventManager());
                LOOP_ASSERT(i, OBJECTS[i].d_expwManager ==
                                          channel.writeEventManager());
                LOOP_ASSERT(i, 0 == channel.readEventManager()->numEvents());
                LOOP_ASSERT(i, 0 == channel.writeEventManager()->numEvents());

                memset(readBuf, '\0', sizeof readBuf);
                Buffer buffer = {readBuf, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.

                    int length = gg(&channel, &buffer,
                                    OBJECTS[i].d_exprManager,
                                    OBJECTS[i].d_expwManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // needs to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters, but doing so will make
                    // the function call less straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            if (veryVerbose) {
                                Q("Writing to the control pipe");
                            }
                            int ret = helpWrite(OBJECTS[i].d_handle[1],
                                                str, length);
                            LOOP_ASSERT(LINE, 0 == ret);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_isInvalidReadFlag ==
                                   channel.isInvalidRead());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_isInvalidWriteFlag ==
                                       channel.isInvalidWrite());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                       channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                       channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_readTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());

                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_writeTimers ==
                                     channel.writeEventManager()->numTimers());
                    }

                    if (veryVerbose) {
                        P_(i);  P_(j);
                        P_(channel.numPendingReadOperations());
                        P(channel.readEventManager()->numEvents());
                        P_(channel.numPendingWriteOperations());
                        P(channel.writeEventManager()->numEvents());
                        P(channel.readEventManager()->numTimers());
                        P(channel.writeEventManager()->numTimers());
                        cout << endl;
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }
#endif
      } break;
      case 3: {  // Case number will be updated later.
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'btlsos::TcpCbChannel'.
        // --------------------------------------------------------------------
      } break;
      case 2: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The main concern is to make sure: 1.  Every constructors can
        //   create correct state values for the channel; 2. Every other
        //   manipulator can make the channel work as expected.
        //
        // Plan:
        //   First, verify the constructor by testing the value of the
        //     resulting object.
        //   Next, for a sequence of independent test values, use the default
        //     constructor to create an object and use the primary manipulator
        //     to set its value.  Verify values using the direct accessors.
        //     For a state value that couldn't be accessible with a direct
        //     accessor, functions that use that value to perform its
        //     functionality will be called to test that value.
        //     Note that the destructor is exercised on each configuration
        //     as the object being tested leaves scope.
        //
        // Testing:
        //   btlsos::TcpTimedCbChannel(eventManager);
        //   ~btlsos::TcpTimedCbChannel();
        //   void cancelRead();
        //   void cancelWrite();
        //   void invalidateRead();
        //   void invalidateWrite();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        btlso::SocketHandle::Handle sock0[2], sock1[2], sock2[2], sock3[2];
        btlso::TcpTimerEventManager rManager0(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager1(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager2(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator),
                                   rManager3(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                     &testAllocator);

        struct {
            int                               d_line;
            btlso::SocketHandle::Handle       *d_handle;
            btlso::TcpTimerEventManager       *d_rManager;
            btlso::TcpTimerEventManager       *d_wManager;
            const btlso::TcpTimerEventManager *d_exprManager;
            const btlso::TcpTimerEventManager *d_expwManager;

        } OBJECTS[] =
        { //line   d_handle   rManager  wManager  exprManager   expwManager
          //----   --------   --------  --------  -----------   -----------
           {L_,     sock0,    &rManager0,   0,     &rManager0,   &rManager0 },
           {L_,     sock1,    &rManager1,   0,     &rManager1,   &rManager1 },
           {L_,     sock2,    &rManager2,   0,     &rManager2,   &rManager2 },
           {L_,     sock3,    &rManager3,   0,     &rManager3,   &rManager3 },
        };

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_numTimers;        // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[MAX_CMDS] =
{
 //d_line d_cmd                        PendR ReadE PendW WriteE Timer Type expD
 //------ -----                        ----- ----- ----- ------ ----- ---- ----
 #if defined(BSLS_PLATFORM_OS_WINDOWS) // windows test data

   // Issue 1 request, which should be finished without being enqueued.  Issue
   // 1 request, which should be finished without being enqueued.
 {L_, "rbt1,(20,100),0,0,-1",           1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "w28720,0,28720,0",               2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(50,200),0,0,-1",            3,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt23720,(50,100),0,0,0",         3,   4,   1,    4,    2,  e_NVEC, "" },

         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request. test cancel when the current request pointer is
         // null.
 {L_, "wt23720,(20,100),0,0,-1",        3,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "w220,0,0,-1",                    3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "cr",                             0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "dw1",                            0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "wt220,(50,100),0,0,-1",           0,   2,   1,    2,    1,  e_NVEC, ""},

 {L_, "cw",                             0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "rt4,(0,2000),0,4,0",             1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt29720,(20,100),0,29720,0",     1,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "wt5,(50,100),1,0,0",             1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "R28729",                         1,   4,   2,    4,    2,  e_NVEC, "" },

 {L_, "dr1",                            1,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "W220",                           1,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "dr1",                            0,   2,   1,    2,    1,  e_NVEC, "" },
 {L_, "dr1",                            0,   0,   0,    0,    0,  e_NVEC, "" },
  {L_,  0,                               0,   0,   0,    0,    0,  e_NVEC, "" }

 #elif defined(BSLS_PLATFORM_OS_AIX)   // ibm test data
 {L_, "rbt1,(20,100),0,0,-1",           1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
   // Issue 1 request, which should be finished without being enqueued.
 {L_, "w1024,0,1024,0",                 2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(0,2000),0,0,-1",            3,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt720,(360,100),0,720,0",        3,   4,   1,    4,    2,  e_NVEC, "" },

         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request. test cancel when the current request pointer is
         // null.
 {L_, "wt2000,(20,100),0,0,-1",         3,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "w220,0,0,-1",                    3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "cr",                             0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R529",                           0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R529",                           0,   2,   3,    2,    1,  e_NVEC, "" },

 {L_, "dw1",                            0,   2,   2,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(255,20),0,4,0",             1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "cw",                             1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt1020,(465,10),0,1020,0",       1,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "w5,1,5,0",                       1,   4,   2,    4,    2,  e_NVEC, "" },

 {L_, "R729",                           1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "R1020",                          1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "dr1",                            1,   3,   1,    3,    1,  e_NVEC, "" },
 {L_, "R128",                           1,   3,   1,    3,    1,  e_NVEC, "" },
 {L_, "W220",                           1,   3,   1,    3,    1,  e_NVEC, "" },
 {L_, "dr2",                            0,   0,   0,    0,    0,  e_NVEC, "" },
  {L_,  0,                               0,   0,   0,    0,    0,  e_NVEC, "" }

 #elif defined(BSLS_PLATFORM_OS_LINUX)

   // Issue 1 request, which should be finished without being enqueued.
 {L_, "rbt1,(20,100),0,0,-1",           1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "w16383,0,16383,0",               2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "w16383,0,16383,0",               2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(0,2000),0,0,-1",            3,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt2000,(2000,100),0,2000,0",     3,   4,   1,    4,    2,  e_NVEC, "" },

         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request. test cancel when the current request pointer is
         // null.
 {L_, "wt12000,(31,100),0,0,-1",        3,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "w200,0,0,-1",                    3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "cr",                             0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R20000",                         0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R20000",                         0,   2,   3,    2,    1,  e_NVEC, "" },

 {L_, "dw1",                            0,   2,   2,    2,    1,  e_NVEC, "" },
 {L_, "rt8193,(100,2000),0,8193,0",     1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "cw",                             1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "R20000",                         1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "R20000",                         1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "W8193",                          1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "dr1",                            0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "R20000",                         0,   0,   0,    0,    0,  e_NVEC, "" },

   // test cancel when the current request pointer is not null.

 {L_, "rbt1,(50,100),1,1,0,{cr}",       1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
   {L_, "wt34000,(2000,100),0,34000,0,{cw}",
                                        2,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "w221,0,0,-1",                    2,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "rbt1,(35,100),0,0,-1",           3,   4,   2,    4,    2,  e_NVEC, "" },

 {L_, "wt223,(15,150),0,0,-1",          3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     4,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "W20",                            4,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "dr2",                            0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },

 {L_, "dw1",                            0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "R32729",                         0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "R32729",                         0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "wt2000,(900,150),0,2000,0",      0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "rb400,1,400,0",                  1,   1,   0,    1,    0,  e_NVEC, "" },
 {L_, "W400",                           1,   1,   0,    1,    0,  e_NVEC, "" },
 {L_, "dr1",                            0,   0,   0,    0,    0,  e_NVEC, "" },

  {L_,  0,                               0,   0,   0,    0,    0,  e_NVEC, "" }

 #else                                  // sun test data

   // Issue 1 request, which should be finished without being enqueued.
 {L_, "rbt1,(20,100),0,0,-1",           1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "w73720,0,73720,0",               2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(0,2000),0,0,-1",            3,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt33720,(455,100),0,33720,0",    3,   4,   1,    4,    2,  e_NVEC, "" },

         // Make sure the write is not affected by cancelRead() by dispatching
         // a write request. test cancel when the current request pointer is
         // null.
 {L_, "wt23720,(31,100),0,0,-1",        3,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "w220,0,0,-1",                    3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "cr",                             0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },

 {L_, "dw1",                            0,   2,   2,    2,    1,  e_NVEC, "" },
 {L_, "rt4,(50,2000),0,4,0",            1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "cw",                             1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt53720,(95,100),0,53720,0",     1,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "w5,1,5,0",                       1,   4,   2,    4,    2,  e_NVEC, "" },

 {L_, "R52729",                         1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "R52729",                         1,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "dr1",                            1,   3,   1,    3,    1,  e_NVEC, "" },
 {L_, "W220",                           1,   3,   1,    3,    1,  e_NVEC, "" },
 {L_, "dr2",                            0,   0,   0,    0,    0,  e_NVEC, "" },

   // test cancel when the current request pointer is not null.

 {L_, "rbt1,(50,100),1,1,0,{cr}",       1,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     2,   2,   0,    2,    1,  e_NVEC, "" },
 {L_, "wt74720,(17,100),0,74720,0,{cw}",2,   4,   1,    4,    2,  e_NVEC, "" },
 {L_, "w221,0,0,-1",                    2,   4,   2,    4,    2,  e_NVEC, "" },
 {L_, "rbt1,(35,100),0,0,-1",           3,   4,   2,    4,    2,  e_NVEC, "" },

 {L_, "wt223,(15,150),0,0,-1",          3,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "rb4,1,0,-1",                     4,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "W20",                            4,   4,   3,    4,    2,  e_NVEC, "" },
 {L_, "dr1",                            0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },
 {L_, "R52729",                         0,   2,   3,    2,    1,  e_NVEC, "" },

 {L_, "dw1",                            0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "R32729",                         0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "wt20,(15,150),0,20,0",           0,   0,   0,    0,    0,  e_NVEC, "" },
 {L_, "rb400,1,400,0",                  1,   1,   0,    1,    0,  e_NVEC, "" },
 {L_, "W400",                           1,   1,   0,    1,    0,  e_NVEC, "" },
 {L_, "dr1",                            0,   0,   0,    0,    0,  e_NVEC, "" },

  {L_,  0,                               0,   0,   0,    0,    0,  e_NVEC, "" }

 #endif
};

// The data records is placed as above due to each record's size.  The normal
// alignment should be as the following statements:

        // the number of channel objects to be created for test
        const int NUM_OBJS = sizeof OBJECTS / sizeof *OBJECTS;

        for (int i = 0; i < NUM_OBJS; ++i) {
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     OBJECTS[i].d_handle,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            adjustBufferSizes(OBJECTS[i].d_handle[0], BUF_LIMIT, 1);
            adjustBufferSizes(OBJECTS[i].d_handle[1], BUF_LIMIT, 1);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                factory.allocate(OBJECTS[i].d_handle[0]);
            LOOP_ASSERT(i, sSocket);

            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                factory.allocate(OBJECTS[i].d_handle[1]);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpTimedCbChannel channel(sSocket,
                                                 OBJECTS[i].d_rManager,
                                                 &testAllocator);
                // Verify the initial state values.
                #ifndef  BSLS_PLATFORM_OS_WINDOWS
                btlso::Flag::BlockingMode bm;
                LOOP_ASSERT(i, 0 == sSocket->blockingMode(&bm));
                LOOP_ASSERT(i,
                        btlso::Flag::e_NONBLOCKING_MODE
                        == bm);
                #endif

                LOOP_ASSERT(i, sSocket == channel.socket());
                LOOP_ASSERT(i, 0 == channel.isInvalidRead());
                LOOP_ASSERT(i, 0 == channel.isInvalidWrite());
                LOOP_ASSERT(i, 0 == channel.numPendingReadOperations());
                LOOP_ASSERT(i, 0 == channel.numPendingWriteOperations());
                LOOP_ASSERT(i, OBJECTS[i].d_exprManager ==
                                          channel.readEventManager());
                LOOP_ASSERT(i, OBJECTS[i].d_expwManager ==
                                          channel.writeEventManager());
                LOOP_ASSERT(i, 0 == channel.readEventManager()->numEvents());
                LOOP_ASSERT(i, 0 == channel.writeEventManager()->numEvents());

                memset(readBuf, '\0', sizeof readBuf);
                Buffer buffer = {readBuf, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[j].d_line;

                    int length = gg(&channel, &buffer, OBJECTS[i].d_rManager,
                                     OBJECTS[i].d_rManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // need to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters.  But that will make the
                    // function call not straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            if (veryVerbose) {
                                Q("Writing to the control pipe");
                            }
                            int ret = helpWrite(OBJECTS[i].d_handle[1],
                                                str, length);
                            LOOP_ASSERT(LINE, 0 == ret);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            btlso::Flag::BlockingMode bm;
                            LOOP_ASSERT(LINE, 0 == sSocket->blockingMode(&bm));
                            cSocket->setBlockingMode(
                                    btlso::Flag::e_NONBLOCKING_MODE);
                            int toRead = length;
                            while (toRead > 0) {
                                ret = cSocket->read(readBuf, toRead);
                                if (ret > 0)
                                    toRead -= ret;
                                else
                                    break;
                                if (veryVerbose) {
                                    cout << "help read data: " << ret << endl;
                                }
                            }
                            cSocket->setBlockingMode(bm);
                        }
                    }

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P(channel.writeEventManager()->numEvents());
                        P(channel.numPendingReadOperations());
                        P(channel.numPendingWriteOperations());
                        P(channel.readEventManager()->numTimers());
                        cout << endl;
                    }
if (SCRIPTS[j].d_numPendingRead != channel.numPendingReadOperations())
  {     P_(i);  P(j);   P(channel.numPendingReadOperations()); }
if (SCRIPTS[j].d_numPendingWrite != channel.numPendingWriteOperations())
  {      P_(i);  P(j);  P(channel.numPendingWriteOperations()); }
                    if (SCRIPTS[j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[j].d_operationType,
                                             buffer.d_readIovecBuf,
                                             SCRIPTS[j].d_expData);
                    }
                    else if (SCRIPTS[j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[j].d_expData,
                                             strlen(SCRIPTS[j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }

                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            // cout << endl << endl << endl;
        }
#endif
      } break;
      case 1: {
// TBD FIX ME
#if 0
// #if !defined(BSLS_PLATFORM_OS_AIX) && !defined(BSLS_PLATFORM_OS_SOLARIS)
        // ------------------------------------------------------------------
        // BREATHING TEST:
        //   We need to exercise basic value-semantic functionalities.  In
        //   particular, make sure that functions for I/O requests such as
        //   'read()', 'readv', 'readRaw', 'readvRaw', 'bufferedRead',
        //  'bufferedReadRaw', 'write', 'writeRaw' etc. will work fine.
        //
        // Plan:
        //   Create two channels, one has separate event managers for read and
        //   write requests, while another one only has one event manager for
        //   both read and write requests.  For each channel, issue a list of
        //   I/O requests that will be enqueued and later executed upon
        //   dispatch, verify the state values of each channel after each
        //  operation.
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // ------------------------------------------------------------------

        if (verbose) cout << "\nBreathing testing."
                          << "\n==================" << endl;

        btlso::SocketHandle::Handle sock0[2];
        btlso::TcpTimerEventManager rManager0(&testAllocator),
                                   rManager1(&testAllocator),
                                   rManager2(&testAllocator),
                                   rManager3(&testAllocator);
        struct {
            int                         d_line;
            btlso::SocketHandle::Handle *d_handle;
            btlso::TcpTimerEventManager *d_rManager;
            btlso::TcpTimerEventManager *d_wManager;
        } CHANNELS[] =
        { //line  d_handle   rManager   wManager
          //----  --------   --------   --------
           {L_,   sock0,    &rManager0, &rManager1 },
        };

        enum { e_NVEC = 0, e_OVEC, e_IOVEC };
        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write event
                                            // manager
            int         d_readTimers;       // number of timers in timerEvent
                                            // manager
            int         d_writeTimers;      // number of timers in timerEvent
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
        // First test channel read and write buffer (entry 1); then test
        // cancelRead() while some write requests on the queue; next test
        // cancelWrite() while some read requests on the queue; lastly test
        // invalidateRead() and invalidateWrite().  Note that it's important to
        // make sure read and write event will not interfere with each other,
        // e.g., cancelRead() shouldn't affect any write request.
   // line      cmd         PendR ReadE PendW WriteE rT  wT Type d_expData
   // ----      ---         ----- ----- ----- ------ --  -- ---- ---------
 {
   {
    { L_, "rbrt3,(50,6),0,3,0",1,   2,   0,    0,   1,  0, e_NVEC, ""        },
    { L_, "r4,1,4,0",          2,   2,   0,    0,   1,  0, e_NVEC, ""        },
    { L_, "rbrt3,(50,6),0,3,0",3,   2,   0,    0,   1,  0, e_NVEC, ""        },
    { L_, "W10",               3,   2,   0,    0,   1,  0, e_NVEC, ""        },
    { L_, "dr1",               0,   0,   0,    0,   0,  0, e_NVEC, "890"     },

    { L_, "r4,1,4,0",          1,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "rb5,1,5,0",         2,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "rrt7,(50,1),1,7,0", 3,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "rvr2,1,3,0",        4,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "rb8,1,8,0",         5,   1,   0,    0,   0,  0, e_NVEC, ""        },

    { L_, "rv4,1,13,0",        6,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "rrt5,(50,1),1,5,0", 7,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "W5",                7,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "dr1",               6,   1,   0,    0,   0,  0, e_NVEC, "1234"    },
    { L_, "dr1",               6,   1,   0,    0,   0,  0, e_NVEC, ""        },

    { L_, "W8",                6,   1,   0,    0,   0,  0, e_NVEC, ""        },
     // the data will be mixed b/w "rb5,1,5,0" (should be "51234") and
     // "rr7,1,7,0" if the same readBuf is used, the first "5678" is read for
     // "rr7,1,7,0"......
    { L_, "dr1",               5,   2,   0,    0,   1,  0, e_NVEC, "56784"   },
    { L_, "W7",                5,   2,   0,    0,   1,  0, e_NVEC, ""        },
    { L_, "dr1",               4,   1,   0,    0,   0,  0, e_NVEC, "5678123" },
    { L_, "dr1",               3,   1,   0,    0,   0,  0, e_IOVEC,"456"     },

    { L_, "W20",               3,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "W20",               3,   1,   0,    0,   0,  0, e_NVEC, ""        },
    { L_, "dr1",               0,   0,   0,    0,   0,  0, e_NVEC, ""        },
     { L_,  0,                  0,   0,   0,    0,   0,  0, e_NVEC, ""        }
   },
 };
 // The normal alignment should be as the following statement:

        // the number of channel objects to be created for test
        const int NUM_CHANNELS = sizeof CHANNELS / sizeof *CHANNELS;

        for (int i = 0; i < NUM_CHANNELS; ++i) {
            int ret = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
                                     CHANNELS[i].d_handle,
                                    btlso::SocketImpUtil::k_SOCKET_STREAM);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(CHANNELS[i].d_handle[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);

            ret = btlso::SocketOptUtil::setOption(CHANNELS[i].d_handle[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                factory.allocate(CHANNELS[i].d_handle[0]);
            LOOP_ASSERT(i, sSocket);
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                factory.allocate(CHANNELS[i].d_handle[1]);
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpTimedCbChannel channel(sSocket,
                                            CHANNELS[i].d_rManager,
                                            CHANNELS[i].d_wManager,
                                                 &testAllocator);
                if (veryVerbose) {
                    P(testAllocator.numBytesInUse());
                }
                // Verify the initial state values.
                LOOP_ASSERT(i, sSocket == channel.socket());
                LOOP_ASSERT(i, 0 == channel.isInvalidRead());
                LOOP_ASSERT(i, 0 == channel.isInvalidWrite());
                LOOP_ASSERT(i, 0 == channel.numPendingReadOperations());
                LOOP_ASSERT(i, 0 == channel.numPendingWriteOperations());
                LOOP_ASSERT(i, CHANNELS[i].d_rManager ==
                                          channel.readEventManager());
                LOOP_ASSERT(i, CHANNELS[i].d_wManager ==
                                          channel.writeEventManager());
                LOOP_ASSERT(i, 0 == channel.readEventManager()->numEvents());
                LOOP_ASSERT(i, 0 == channel.writeEventManager()->numEvents());

                char buf[BUF_LIMIT] = "\0",
                buf0[2] = "\0", buf1[3] = "\0", buf2[5] = "\0",
                buf3[7] = "\0", buf4[9] = "\0", buf5[10] = "\0";

                btls::Iovec vecBuffer[MAX_VEC];
                vecBuffer[0].setBuffer(buf0, sizeof buf0 - 1);
                vecBuffer[1].setBuffer(buf1, sizeof buf1 - 1);
                vecBuffer[2].setBuffer(buf2, sizeof buf2 - 1);
                vecBuffer[3].setBuffer(buf3, sizeof buf3 - 1);
                vecBuffer[4].setBuffer(buf4, sizeof buf4 - 1);
                vecBuffer[5].setBuffer(buf5, sizeof buf5 - 1);

                Buffer buffer = {buf, str, vecBuffer, ioVec, oVec};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.

                    int length = gg(&channel, &buffer, CHANNELS[i].d_rManager,
                                              CHANNELS[i].d_rManager, command);

                    // There are 9 parameters in the bufferedReadCallback()
                    // function.  This is the maximum number of parameters to
                    // call makeF() for a functor object.  If we have the
                    // following commands executed in gg(), the socket handle
                    // needs to be passed to gg() and the
                    // bufferedReadCallback().  To do so, we need to use a
                    // struct to wrap some of the parameters to reduce the
                    // number of function parameters, but doing so will make
                    // the function call less straightforward.  That's why we
                    // execute the following commands outside gg().

                    if (length > 0) {
                        if ('W' == *command) {
                            if (veryVerbose) {
                                Q("Writing to the control pipe");
                            }
                            int ret = helpWrite(CHANNELS[i].d_handle[1],
                                                BUFFERS[4].d_sndBuf, length);
                            LOOP_ASSERT(LINE, 0 == ret);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_readTimers ==
                                     channel.readEventManager()->numTimers());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_writeTimers ==
                                     channel.writeEventManager()->numTimers());
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVEC) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NVEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }

                    if (veryVerbose) {
                        P_(i);  P_(j);
                        P_(channel.numPendingReadOperations());
                        P(channel.readEventManager()->numEvents());
                        P_(channel.numPendingWriteOperations());
                        P(channel.writeEventManager()->numEvents());
                        P_(buffer.d_readBuf); P_(SCRIPTS[i][j].d_expData);
                        P(channel.readEventManager()->numTimers());
                        P(channel.writeEventManager()->numTimers());
                        P(testAllocator.numBytesInUse());
                        cout << endl;
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
        }

#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

#endif // !BSLS_PLATFORM_OS_WINDOWS

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
