// btlsos_tcpcbchannel.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcpcbchannel.h>

#include <btlso_tcptimereventmanager.h>
#include <btlso_sockethandle.h>
#include <btlso_socketimputil.h>
#include <btlso_inetstreamsocketfactory.h>      // create a streamsocket opt.
#include <btlso_ipv4address.h>
#include <btlso_socketoptutil.h>
#include <btlso_streamsocket.h>
#include <btlso_streamsocketfactory.h>

#include <btls_iovec.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bslmt_threadutil.h>

#include <bsls_platform.h>

#include <bsl_c_ctype.h>     // 'isdigit'
#include <bsl_csignal.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>
#endif

#ifdef BSLS_PLATFORM_CMP_SUN
#include <signal.h>
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
// [ 2] btlsos::TcpCbChannel(..., *manager, ...);
// [ 5] btlsos::TcpCbChannel(..., *rManager, *wManager, ...);
// [ 2] ~btlsos::TcpCbChannel();
// MANIPULATORS
// [ 9] int read();
// [11] int readv();
// [ 6] int readRaw();
// [11] int readvRaw();
// [10] int bufferedRead();
// [12] int bufferedReadRaw();
// [ 8] int write();
// [ 7] int writeRaw();
// [14] int writev(const btls::Iovec::Ovec *buffers, ...);
// [13] int writev(const btls::Iovec::Iovec *buffers, ...);
// [14] int writevRaw(const btls::Iovec::Ovec *buffers, ...);
// [13] int writevRaw(const btls::Iovec::Iovec *buffers, ...);
// [15] int bufferedWrite();
// [16] int bufferedWritev(const btls::Iovec::Ovec *buffers, ...);
// [16] int bufferedWritev(const btls::Iovec::Iovec *buffers, ...);
// [ 4] void cancelAll();
// [ 2] void cancelRead();
// [ 2] void cancelWrite();
// [ 2] void invalidateRead();
// [ 2] void invalidateWrite();
// ACCESSORS
// [ 4] int isInvalid() const;
// [ 4] int numPendingReadOperations() const;
// [ 4] int numPendingWriteOperations() const;
// [ 4] btlso::StreamSocket<btlso::IPv4Address> *socket() const;
//-----------------------------------------------------------------------------
// [17] USAGE example
// [ 1] BREATHING TEST
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
typedef btlsos::TcpCbChannel Obj;

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

static int veryVerbose;

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
char str[BUF_WRITE+1], str2[BUF_WRITE2+1];  // to be written to a channel
#ifdef BSLS_PLATFORM_OS_LINUX
char str3[4096];
#endif

//-----------------------------------------------------------------------------

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int
gg(btlsos::TcpCbChannel        *channel,
   Buffer                      *buffer,
   btlso::TcpTimerEventManager *rManager,
   btlso::TcpTimerEventManager *wManager,
   const char                  *script);

#ifdef BSLS_PLATFORM_OS_UNIX

extern "C"
{
static void sigPipeHandler(int sig)
{
    if (sig == SIGPIPE && veryVerbose) {
        write(2, "GOT A SIGPIPE signal.\n",
                sizeof("GOT A SIGPIPE signal.\n"));
        write(2, "!!!!!!!!!!!!!!!!!!!!!\n",
                sizeof("!!!!!!!!!!!!!!!!!!!!!\n"));
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
              btls::Iovec *vec = (btls::Iovec*) vectorBuffer;
              int idx = 0;
              int len = static_cast<int>(strlen((char*)vec[idx].buffer()));

              while (len) {
                  if (veryVerbose) {
                      P_(i);  P_(j); P_(len);
                      P((char*)vec[idx].buffer());
                  }
//                   LOOP2_ASSERT(i, j, 0 ==
//                       strncmp((char*)vec[idx].buffer(), expData, len));

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
                                 btlsos::TcpCbChannel        *channel,
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
                Q("");
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
            oneScript = getNextCbCommand(oneScript);
        }
    }
}

static void readCallback(int                          status,
                         int                          augStatus,
                         btlso::TcpTimerEventManager *eveManager,
                         btlsos::TcpCbChannel        *channel,
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
        P_(status); P(augStatus);
        P_(expStatus); P(expAugStatus);
    }

    if (0 <= status) {
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
                cout << "The request is interrupted." << endl;
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
                          btlsos::TcpCbChannel        *channel,
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
                          btlsos::TcpCbChannel        *channel,
                          int                          expStatus,
                          int                          expAugStatus,
                          const char                  *script,
                          Buffer                      *buffer)
    // Callback function for a write request to indicate the execution of a
    // read request and to notify the user if it succeeds, partially completes,
    // fails or needs to issue other requests.
{
    LOOP2_ASSERT(expStatus, status, expStatus == status);
    LOOP2_ASSERT(augStatus, expAugStatus, augStatus == expAugStatus);

    if (veryVerbose) {
        P_(status);   P(augStatus);
        P_(expStatus); P(expAugStatus);
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
                         << " of " << status << " bytes." << endl;
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
//   d     - dispatch,
//   dr    - read dispatch,
//   dw    - write dispatch,
//   r     - read,
//   rb    - buffered read,
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
//     number of seconds, and the second one in the pair means the number of
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

//   For a digital value following a 'R' or 'W', e.g., "R10" or "w5", the
//     value means the number of bytes to be read/written to the control pipe
//     to help test any read/write function of the channel.
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

static int gg(btlsos::TcpCbChannel        *channel,
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
          case 'R': {
                               // close receive
              ret = channel->socket()->shutdown(
                     btlso::Flag::e_SHUTDOWN_RECEIVE);
              ASSERT(0 == ret);
          } break;
          case 'S':
                               // close receive
              ret = channel->socket()->shutdown(
                     btlso::Flag::e_SHUTDOWN_SEND);
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
          int readLen, optFlag = 0, second = 0, nanoSec = 0,
              expStatus, expAugStatus;
          bsl::function<void(int, int)> callback;

          switch (*(script+1)) {
            case 'b': {
                if ('r' == *(script+2)) {
                    if ('t' == *(script+3)) {  // 'rbrt'
                        ret = sscanf(script, "rbrt%d,(%d,%d),%d,%d,%d",
                                     &readLen, &second, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                    }
                    else {                     // bufferedReadRaw()
                        ret = sscanf(script, "rbr%d,%d,%d,%d", &readLen,
                                     &optFlag, &expStatus, &expAugStatus);

                        ASSERT(4 == ret);      // Return 4 numbers.
                        btlsc::CbChannel::BufferedReadCallback callback(
                                bdlf::BindUtil::bind( &bufferedReadCallback
                                                   , _1, _2, _3
                                                   , rManager
                                                   , channel
                                                   , expStatus
                                                   , expAugStatus
                                                   , callBackScript
                                                   , buffer));

                       ret = channel->bufferedReadRaw(readLen, callback,
                                                                optFlag);
                    }
                }
                else {
                    // bufferedRead()
                    ret = sscanf(script, "rb%d,%d,%d,%d", &readLen,
                                 &optFlag, &expStatus, &expAugStatus);

                    ASSERT(4 == ret);   // Return 4 numbers.
                    btlsc::CbChannel::BufferedReadCallback callback(
                            bdlf::BindUtil::bind( &bufferedReadCallback
                                               , _1, _2, _3
                                               , rManager
                                               , channel
                                               , expStatus
                                               , expAugStatus
                                               , callBackScript
                                               , buffer));

                    ret = channel->bufferedRead(readLen, callback, optFlag);
                }
                ASSERT(0 == ret || channel->isInvalidRead());
            } break;
            case 'r': {
                if ('t' == *(script+2)) {   // 'rrt'
                    ret = sscanf(script, "rrt%d,(%d,%d),%d,%d,%d", &readLen,
                                 &second, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                }
                else {                     // 'readRaw'
                    ret = sscanf(script, "rr%d,%d,%d,%d", &readLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);
                    callback = bdlf::BindUtil::bind( &readCallback
                                                  , _1, _2
                                                  , rManager
                                                  , channel
                                                  , expStatus
                                                  , expAugStatus
                                                  , callBackScript
                                                  , buffer);
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
                                     &numBuf, &second, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                       }
                       else {                    // readvRaw()
                           ret = sscanf(script, "rvr%d,%d,%d,%d", &numBuf,
                                        &optFlag, &expStatus, &expAugStatus);
                           btls::Iovec *buf = buffer->d_readIovecBuf;
                           callback = bdlf::BindUtil::bind( &readvCallback
                                                         , _1, _2
                                                         , rManager
                                                         , channel
                                                         , expStatus
                                                         , expAugStatus
                                                         , callBackScript
                                                         , buffer);
                           ret = channel->readvRaw(buf, numBuf, callback,
                                                                optFlag);
                           ASSERT(0 == ret || channel->isInvalidRead());
                       }
                     }
                     else {
                         // readv()
                         ret = sscanf(script, "rv%d,%d,%d,%d", &numBuf,
                                 &optFlag, &expStatus, &expAugStatus);
                         ASSERT(4 == ret);         // Return 4 numbers.
                         btls::Iovec *buf = buffer->d_readIovecBuf;
                         callback = bdlf::BindUtil::bind( &readvCallback
                                                        , _1, _2
                                                        , rManager
                                                        , channel
                                                        , expStatus
                                                        , expAugStatus
                                                        , callBackScript
                                                        , buffer);
                         ret = channel->readv(buf, numBuf, callback, optFlag);
                         ASSERT(0 == ret || channel->isInvalidRead());
                     }
            } break;
            default: {
                if ('t' == *(script+1)) {
                    // timedRead()
                    ret = sscanf(script, "r%d,(%d,%d),%d,%d,%d", &readLen,
                                 &second, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(6 == ret);        // Return 6 numbers.
                }
                else if (isdigit(*(script+1))) { // read()
                    // read()
                    ret = sscanf(script, "r%d,%d,%d,%d", &readLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);            // Return 4 numbers.

                    callback = bdlf::BindUtil::bind( &readCallback
                                                  , _1, _2
                                                  , rManager
                                                  , channel
                                                  , expStatus
                                                  , expAugStatus
                                                  , callBackScript
                                                  , buffer);
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
          int writeLen, optFlag = 0, second = 0, nanoSec = 0,
              expStatus, expAugStatus;
          btlsc::CbChannel::WriteCallback callback;
          switch (*(script+1)) {
            case 'r':
                if ('t' == *(script+2)) {     // timedWriteRaw()
                    ret = sscanf(script, "wrt%d,(%d,%d),%d,%d,%d", &writeLen,
                                 &second, &nanoSec, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(7 == ret);         // Return 7 numbers.
                }
                else {                        // writeRaw()
                    ret = sscanf(script, "wr%d,%d,%d,%d",
                                 &writeLen, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(4 == ret);         // Return 4 numbers.
                    callback = bdlf::BindUtil::bind( &writeCallback
                                                  , _1, _2
                                                  , wManager
                                                  , channel
                                                  , expStatus
                                                  , expAugStatus
                                                  , callBackScript
                                                  , buffer);
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
                                     &ch, &numBuf, &second, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.
                    }
                    else {                     // raw writev
                        ret = sscanf(script, "wv%cr%d,%d,%d,%d",
                                 &ch, &numBuf, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.
                        callback = bdlf::BindUtil::bind( &writeCallback
                                                      , _1, _2
                                                      , wManager
                                                      , channel
                                                      , expStatus
                                                      , expAugStatus
                                                      , callBackScript
                                                      , buffer);
                        if ('i' == ch) {
                            ret = channel->writevRaw(
                                                     buffer->d_writeIovecBuf,
                                                     numBuf,
                                                     callback, optFlag);
                        }
                        else if ('o' == ch) {
                            ret = channel->writevRaw(
                                                     buffer->d_writeOvecBuf,
                                                     numBuf,
                                                     callback, optFlag);
                        }
                        ASSERT(0 == ret || channel->isInvalidWrite());

                    }
                }
                else {
                    if ('t' == *(script+3) ) { // timed writev
                        ret = sscanf(script, "wv%ct%d,(%d,%d),%d,%d,%d",
                                     &ch, &writeLen, &second, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.
                    }
                    else {                     // writev()
                        ret = sscanf(script, "wv%c%d,%d,%d,%d",
                                 &ch, &writeLen, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.
                        callback = bdlf::BindUtil::bind( &writeCallback
                                                      , _1, _2
                                                      , wManager
                                                      , channel
                                                      , expStatus
                                                      , expAugStatus
                                                      , callBackScript
                                                      , buffer);
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
                        // timedBuffered Writev()
                        ret = sscanf(script, "wbv%ct%d,(%d,%d),%d,%d,%d",
                                     &ch, &numBuf, &second, &nanoSec,
                                     &optFlag, &expStatus, &expAugStatus);
                        ASSERT(7 == ret);      // Return 7 numbers.
                    }
                    else {                     // bufferedWritev()
                        ret = sscanf(script, "wbv%c%d,%d,%d,%d",
                                 &ch, &numBuf, &optFlag, &expStatus,
                                 &expAugStatus);
                        ASSERT(5 == ret);      // Return 5 numbers.
                        callback = bdlf::BindUtil::bind( &writeCallback
                                                      , _1, _2
                                                      , wManager
                                                      , channel
                                                      , expStatus
                                                      , expAugStatus
                                                      , callBackScript
                                                      , buffer);
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
                    ret = sscanf(script, "wb%d,%d,%d,%d",
                                 &writeLen, &optFlag, &expStatus,
                                 &expAugStatus);
                    ASSERT(4 == ret);       // Return 4 numbers.
                    callback = bdlf::BindUtil::bind( &writeCallback
                                                  , _1, _2
                                                  , wManager
                                                  , channel
                                                  , expStatus
                                                  , expAugStatus
                                                  , callBackScript
                                                  , buffer);
                    ret = channel->bufferedWrite(buffer->d_writeBuf,
                                                 writeLen,
                                                 callback, optFlag);
                    ASSERT(0 == ret || channel->isInvalidWrite());
                }
                break;
            }
            default:
                if ('t' == *(script+2)) {         // timedWrite()
                    ret = sscanf(script, "wt%d,(%d,%d),%d,%d,%d",
                                     &writeLen, &second, &nanoSec, &optFlag,
                                     &expStatus, &expAugStatus);
                    ASSERT(6 == ret);             // Return 6 numbers
                }
                else if (isdigit(*(script+1))) {  // write()
                    ret = sscanf(script, "w%d,%d,%d,%d", &writeLen,
                                 &optFlag, &expStatus, &expAugStatus);
                    ASSERT(4 == ret);             // Return 4 numbers.

                    callback = bdlf::BindUtil::bind( &writeCallback
                                                  , _1, _2
                                                  , wManager
                                                  , channel
                                                  , expStatus
                                                  , expAugStatus
                                                  , callBackScript
                                                  , buffer);
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
                LOOP2_ASSERT(expRet, ret,
                             expRet == ret || channel->isInvalidRead());
                if (veryVerbose) {
                    cout << "rManager ret: " << ret << ", isinvalid: "
                         << channel->isInvalidWrite() << endl;
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

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int errCode = 0;

    // TBD: these tests frequently timeout on Windows, disabling until fixed
#ifdef BSLS_PLATFORM_OS_WINDOWS
    testStatus = -1;
#else

#ifdef BSLS_PLATFORM_OS_UNIX
    mySignal(SIGPIPE, sigPipeHandler);    // register a handler for SIGPIPE.
    // A write() on the closed socket will generate SIGPIPE.
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);
    btlso::SocketImpUtil::startup(&errCode);

    helpBuildVector();       // Help create global vectors: one 'Iovec', one
                             //'Ovec'.
    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
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
        // write events will be registered in the same event queue in order.
        btlso::TcpTimerEventManager rEventManager(
                                    btlso::TcpTimerEventManager::e_NO_HINT,
                                    &testAllocator);
        btlso::TcpTimerEventManager wEventManager(
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

            btlsos::TcpCbChannel channel(sSocket, &rEventManager,
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
            // functor object with the expected bytes for this I/O request, a
            // function pointer to a user-installed callback function which is
            // usually to report this I/O request's execution status after
            // being dispatched.
            int readLen1 = 5;
            int optFlag1 = 0;
            char readBuf1[20] = "\0";

            bsl::function<void(int, int)> callback1(
                   bdlf::BindUtil::bind(&myReadCallback, _1, _2, readLen1, 0));
            // Issue a read request:
            ret = channel.read(readBuf1, readLen1, callback1, optFlag1);
            ASSERT(0 == ret);
            ASSERT(1 == channel.numPendingReadOperations());
            ASSERT(1 == channel.readEventManager()->numEvents());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());

            // Issue another read request:
            int readLen2 = 9;
            int optFlag2 = 0;
            char readBuf2[20] = "\0";
            bsl::function<void(int, int)> callback2(
                   bdlf::BindUtil::bind(&myReadCallback, _1, _2, readLen2, 0));
            // Issue a read request:
            ret = channel.read(readBuf2, readLen2, callback2, optFlag2);
            ASSERT(0 == ret);
            ASSERT(2 == channel.numPendingReadOperations());
            ASSERT(1 == channel.readEventManager()->numEvents());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());

            // The client now writes data into its socket for the channel to
            // read.
            char writeBuf[21] = "abcdefghij1234567890";
            int len = btlso::SocketImpUtil::write(handles[1],
                                                  writeBuf,
                                                  static_cast<int>(
                                                            strlen(writeBuf)));
            ASSERT(len == (int)strlen(writeBuf));
            // Now the channel dispatches the request.
            ret = ((btlso::TcpTimerEventManager*)
                     channel.readEventManager())->dispatch(0);
            ASSERT(1 == ret);
            ASSERT(1 == channel.numPendingReadOperations());
            ASSERT(1 == channel.readEventManager()->numEvents());
            ASSERT(0 == channel.numPendingWriteOperations());
            ASSERT(0 == channel.writeEventManager()->numEvents());
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
        btlso::SocketImpUtil::close(handles[0]);
        btlso::SocketImpUtil::close(handles[1]);
      } break;
      case 16: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING 'bufferedWritev(const btls::Iovec::Ovec *buffers)' &
        //         'bufferedWritev(const btls::Iovec::Iovec *buffers)'
        //         FUNCTION:
        //
        //   The main concerns about this function are (1) if a write
        //   request can be finished right away when the channel send-buffer is
        //   writeable; (2) if a write request will be correctly enqueued to
        //   the request queue when the channel send-buffer is not writeable,
        //   even though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   will be removed after the request is dispatched and done; (5) if
        //   a write-event will be removed when no any write request of this
        //   write-event exists or will keep this write-event otherwise;
        //   (6) if a different kind of write-event will be registered when
        //   next write-event is another kind of write request(e.g.,a
        //   write() request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   some cases such as "asynchronous event", "ERROR_WOULDBLOCK",
        //   "ERROR_EOF" in the channel callback function are hard to be
        //   duplicated in the test driver, and so they are not covered in the
        //   test case.  By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values which are calculated manually
        //   and put in a table.  Those states include the lengths of
        //   "read request queue", "read event queue", "write request queue",
        //   and "write event queue", also the buffer data in the channel.
        //   The "TCPNODELAY" option for the peer socket is set on because
        //   some small packets will be written to the channel and we the
        //   require these data be read from channel right after this
        //   write.
        //
        // Testing:
        //   int bufferedWritev(const btls::Iovec::Ovec *buffers...);
        //   int bufferedWritev(const btls::Iovec::Iovec *buffers);
        // -------------------------------------------------------------------

        if (verbose) {
            cout << "\n Testing bufferedWritev() METHOD I."
                 << "\n ==================================" << endl;
        }
        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                            // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   //line       cmd       PendingR  ReadE  PendingW  WriteE  Type  d_expData
   //----       ----      --------  -----  --------  ------  ----  ---------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvi7,1,29760,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "wbvi6,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvi3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvi7,1,29760,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "wbvi5,1,40,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "R25000",           0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },/*
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvi7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi7,1,29760,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   }, */
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvi7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R25000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },

     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvi7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,40,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvi7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi3,1,18,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      2,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvi7,1,29760,0", 0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  "wbvi1,1,11,0,{wbvi2,0,15,0; wbvi4,0,20,0,{wbvi5,1,40,0}}",
                              0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi3,1,18,0",    0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing bufferedWritev(oVec) function.
     // =====================================================================
   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvo7,1,29760,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "wbvo6,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvo3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvo7,1,29760,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "wbvo5,1,40,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "R25000",           0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvo7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo7,1,29760,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },  /*
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvo7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R25000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },

     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },  */
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvo7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,40,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvo7,1,29760,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo3,1,18,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      2,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   }, /*
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvo7,1,29760,0", 0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  "wbvo1,1,11,0,{wbvo2,0,15,0; wbvo4,0,20,0,{wbvo5,1,40,0}}",
                              0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo3,1,18,0",    0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
   */
   #elif defined(BSLS_PLATFORM_OS_AIX)        // ibm test data

   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvi7,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvi3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvi7,1,1540,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R500",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvi7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi7,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvi7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvi3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvi7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvi7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi3,1,18,0",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
     {L_,  "wbvi6,1,540,0",   0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvi1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvi7,1,1540,0",  0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi1,1,11,0,{wbvi2,0,15,0; wbvi4,0,20,0,{wbvi5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R500",            0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing bufferedWritev(oVec) function.
     // =====================================================================
   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvo7,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvo3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvo7,1,1540,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R500",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvo7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo7,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvo7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvo3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvo7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvo7,1,1540,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo3,1,18,0",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R500",            0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
     {L_,  "wbvo6,1,540,0",   0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvo1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvo7,1,1540,0",  0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo1,1,11,0,{wbvo2,0,15,0; wbvo4,0,20,0,{wbvo5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R500",            0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
   #elif BSLS_PLATFORM_OS_LINUX

   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvi8,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvi3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvi8,1,20478,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R55000",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvi8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi8,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvi8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvi3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvi8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch. bug ?? !!! there's
     // bug here for mixed request: buffered and non-buffered.
     {L_,  "wbvi8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi3,1,0,-1",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request.  There's bug in the code found by this test:
     {L_,  "wbvi6,1,1040,0",  0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvi1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvi8,1,20478,0", 0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi1,1,11,0,{wbvi2,0,15,0; wbvi4,0,20,0,{wbvi5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing bufferedWritev(oVec) function.
     // =====================================================================
   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvo8,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvo3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvo8,1,20478,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R55000",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvo8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo8,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R17000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvo8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvo3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvo8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvo8,1,20478,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo3,1,18,0",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request.  There's bug in the code found by this test:
     {L_,  "wbvo6,1,1040,0",  0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvo1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvo8,1,20478,0", 0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo1,1,11,0,{wbvo2,0,15,0; wbvo4,0,20,0,{wbvo5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },

   #else                                       // sun test data

   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvi7,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvi3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvi2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvi7,1,74720,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R55000",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvi7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi7,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvi7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvi3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvi7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch. bug ?? !!! there's
     // bug here for mixed request: buffered and non-buffered.
     {L_,  "wbvi7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvi3,1,0,-1",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
     {L_,  "wbvi6,1,1040,0",  0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvi1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvi7,1,74720,0", 0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi1,1,11,0,{wbvi2,0,15,0; wbvi4,0,20,0,{wbvi5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvi3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing bufferedWritev(oVec) function.
     // =====================================================================
   { // Invalidate a channel.
     {L_,  "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
     {L_,  "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "dw0",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
     {L_, "wbvo7,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "cw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_, "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
     {L_,  "wbvo3,1,18,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  "wbvo2,1,15,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
     {L_, "wbvo7,1,74720,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_, "wbvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "R55000",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_, "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
     {L_,  "wbvo7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo7,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
     {L_,  "wbvo7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wvo3,1,18,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
     {L_,  "wbvo7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "R35000",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
     {L_,  "wbvo7,1,74720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "w15,1,15,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "wbvo3,1,18,0",    0,       1,      3,       1,  e_NON_VEC,   ""  },
     {L_,  "R35021",          0,       1,      3,       1,  e_NON_VEC,   ""  },

     {L_,  "dw1",             0,       1,      2,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                 0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
     {L_,  "wbvo6,1,1040,0",  0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "cS",              0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "wbvo1,1,-3,0",    0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  "dw0",             0,      0,       0,      0,   e_NON_VEC,   ""  },
     {L_,  0,                 0,      0,       0,      0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
     {L_,  "wbvo7,1,74720,0", 0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo1,1,11,0,{wbvo2,0,15,0; wbvo4,0,20,0,{wbvo5,1,40,0}}",
                              0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "wbvo3,1,18,0",    0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "R35769",          0,      1,       3,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       2,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      1,       1,      1,  e_NON_VEC,   ""   },
     {L_,  "dw1",             0,      0,       0,      0,  e_NON_VEC,   ""   },
     {L_,  0,                 0,      0,       0,      0,  e_NON_VEC,   ""   }
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

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);
            char readBuf[MAX_BUF];
            memset(readBuf, '\0', sizeof readBuf);
            Buffer buffer = {readBuf, str, 0, ioVec, oVec};
            // Buffer is a struct type where declares different pointers
            // pointing to different I/O buffers, e.g., readBuffer,
            // writeBuffer, iovecBuffer, ovecBuffer.  This struct is declared
            // at the beginning of this test driver.
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eventManager, &eventManager, command);

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
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P(eventManager.numEvents());
                        cout << endl;
                    }

                    if (SCRIPTS[i][j].d_operationType == e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
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
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 15: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'bufferedWrite()' FUNCTION:
        //   The main concerns about this function are (1) if a write
        // request can be finished right away when the channel send-buffer is
        // writeable; (2) if a write request will be correctly enqueued to the
        // request queue when the channel send-buffer is not writeable, even
        // though the request queue is currently empty; (3) if a write request
        // will be rejected when the channel is invalidated or the channel is
        // closed; (4) if the write request in the request queue will be
        // removed after the request is dispatched and done; (5) if a
        // write-event will be removed when no any write request of this
        // write-event exists or will keep this write-event otherwise; (6) if a
        // different kind of write-event will be registered when next
        // write-event is another kind of write request(e.g.,a write()
        // request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   some cases such as "asynchronous event", "ERROR_WOULDBLOCK",
        //   "ERROR_EOF" in the channel callback function are hard to be
        //   duplicated in the test driver, and so they are not covered in the
        //   test case.  By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values which are calculated manually
        //   and put in a table.  Those states include the lengths of
        //   "read request queue", "read event queue", "write request queue",
        //   and "write event queue", also the buffer data in the channel.
        //   The "TCPNODELAY" option for the peer socket is set on because
        //   some small packets will be written to the channel and we the
        //   require these data be read from channel right after this
        //   write.
        //
        // Testing:
        //   int bufferedWrite();
        // -------------------------------------------------------------------

        if (verbose) {
            cout << "\n Testing bufferedWrite() METHOD."
                 << "\n ===============================" << endl;
        }
        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                            // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   //line       cmd       PendingR  ReadE  PendingW  WriteE  Type  d_expData
   //----       ----      --------  -----  --------  ------  ----  ---------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
   { // Invalidate a channel.
    {L_,  "iw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
    {L_, "wb28720,1,28720,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_, "wb720,1,0,-1",      0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
    {L_,  "wb18,1,18,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
    {L_, "wb28720,1,28720,0", 0,       0,      0,       0,   e_NON_VEC,  ""  },
    {L_, "wb40,1,40,0",       0,       1,      1,       1,   e_NON_VEC,  ""  },
    {L_, "R25000",            0,       1,      1,       1,   e_NON_VEC,  ""  },
    {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,  ""  },
    {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,  ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
   {L_,  "wb28720,1,28720,0", 0,      0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "wb28720,1,28720,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "R25000",           0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },  /*
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
   {L_, "wb28720,1,28720,0", 0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_, "w18,1,18,0",        0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "R35000",            0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },

   {L_, 0,                   0,       0,      0,       0,   e_NON_VEC,   ""  },
   }, */
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
   {L_, "wb28720,1,28720,0", 0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_, "wb40,1,40,0",       0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "R25000",            0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
   {L_,  "wb28720,1,28720,0", 0,      0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "w15,1,15,0",       0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "R25021",           0,       1,      2,       1,   e_NON_VEC,   ""  },

   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
   {L_,  "wb28720,1,28720,0", 0,      0,      0,       0,   e_NON_VEC,   ""  },
     {L_,  "wb11,1,11,0,{wb15,0,15,0; wb20,0,20,0,{wb40,1,40,0}}",
                             0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "R25769",           0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  }
   },

   #elif defined(BSLS_PLATFORM_OS_AIX)        // ibm test data

   { // Invalidate a channel.
    {L_,  "iw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
    {L_,  "cS",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb40,1,-3,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
    {L_, "wb1025,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
    {L_,  "wb18,1,18,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
    {L_, "wb1025,1,1025,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_, "wb40,1,40,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
    {L_,  "wb1025,1,1025,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_,  "wb1025,1,0,-1",    0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "R500",             0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
    {L_, "wb1025,1,1025,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_, "w18,1,18,0",        0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },

    {L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_, 0,                   0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
    {L_, "wb1025,1,1025,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_, "wb40,1,40,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
    {L_,  "wb1025,1,1025,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_,  "w15,1,15,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "wb18,1,18,0",      0,       1,      3,       1,  e_NON_VEC,   ""  },
    {L_,  "R500",             0,       1,      3,       1,  e_NON_VEC,   ""  },

    {L_,  "dw1",              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
    {L_,  "wb240,1,240,0",    0,       0,      0,       0,   e_NON_VEC,   "" },
    {L_,  "cS",               0,       0,      0,       0,   e_NON_VEC,   "" },
    {L_,  "wb11,1,-3,0",      0,       0,      0,       0,   e_NON_VEC,   "" },
    {L_,  "dw0",              0,       0,      0,       0,   e_NON_VEC,   "" },
    {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   "" },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
    {L_,  "wb1025,1,1025,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     {L_,  "wb11,1,11,0,{wb15,0,15,0; wb20,0,20,0,{wb40,1,40,0}}",
                              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "wb18,1,18,0",      0,       1,      3,       1,  e_NON_VEC,   ""  },
    {L_,  "R500",             0,       1,      3,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       1,      2,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       1,      1,       1,  e_NON_VEC,   ""  },
    {L_,  "dw1",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif BSLS_PLATFORM_OS_LINUX

   { // Invalidate a channel.
    {L_,  "iw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
    {L_,  "cS",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb40,1,-3,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
   {L_, "wb40000,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
   {L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""   },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
   {L_,  "wb18,1,18,0",      0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""   },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
   {L_, "wb30000,1,30000,0", 0,       1,      1,       1,   e_NON_VEC,  ""   },
   {L_, "wb40,1,40,0",       0,       1,      2,       1,   e_NON_VEC,  ""   },
   {L_, "R55000",            0,       1,      2,       1,   e_NON_VEC,  ""   },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,  ""   },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,  ""   },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
    {L_,  "wb30000,1,30000,0",0,      1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "wb32766,1,0,-1",   0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "R35000",           0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
   {L_, "wb30000,1,30000,0", 0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "w18,1,18,0",        0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "R35000",            0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       1,      1,       1,   e_NON_VEC,   ""  },

   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_, 0,                   0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
   {L_, "wb30000,1,30000,0", 0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "wb40,1,40,0",       0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "R55000",            0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
   {L_,  "wb30000,1,30000,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "w15,1,15,0",       0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "R35021",           0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      2,       1,   e_NON_VEC,   ""  },

   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
   {L_,  "wb1040,1,1040,0",  0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "cS",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "wb11,1,-3,0",      0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "dw0",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
   {L_,  "wb30000,1,30000,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
     {L_,  "wb11,1,11,0,{wb15,0,15,0; wb20,0,20,0,{wb40,1,40,0}}",
                             0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "R35769",           0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  }
   },

   #else                                    // sun test data

   { // Invalidate a channel.
    {L_,  "iw",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Close the channel, try issuing a write request, then dispatch, verify
     // by trying dispatching a request, which shouldn't happen.
    {L_,  "cS",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "wb40,1,-3,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  "dw0",              0,       0,      0,       0,  e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  },
   },
   { // Test if all write requests can be canceled after a "cancel write"
     // command is executed.  Verify by trying dispatching a request, which
     // shouldn't have any request to disp.
   {L_, "wb74720,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
   {L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""   },
   },
   { // Issue two requests when the channel is writeable, and the write request
     // queue is empty.
   {L_,  "wb18,1,18,0",      0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  "wb15,1,15,0",      0,       0,      0,       0,  e_NON_VEC,   ""   },
   {L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""   },
   },
   { // Issue a partially write will be queued correctly for a write with
     // ERROR_WOULDBLOCK during the first try, then dispatch the queued
     // request.
   {L_, "wb74720,1,74720,0", 0,       1,      1,       1,   e_NON_VEC,  ""   },
   {L_, "wb40,1,40,0",       0,       1,      2,       1,   e_NON_VEC,  ""   },
   {L_, "R55000",            0,       1,      2,       1,   e_NON_VEC,  ""   },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,  ""   },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,  ""   },
   },
   { // Test if the channel's buffer will be extended properly if a big chunk
     // of data is to be written from a request.
   {L_,  "wb74720,1,74720,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "wb74720,1,0,-1",   0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "R35000",           0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if an other kind of write request on the request queue can be
     // finished after the previous buffered requests before it.
   {L_, "wb74720,1,74720,0", 0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "w18,1,18,0",        0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "R35000",            0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       1,      1,       1,   e_NON_VEC,   ""  },

   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_, 0,                   0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if a write request on the request queue which can't be finished
     // once can be kept on the request queue and finished by following
     // dispatches.
   {L_, "wb74720,1,74720,0", 0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_, "wb40,1,40,0",       0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "R55000",            0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_, "dw1",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Issue 3 requests of different types, then dispatch.
   {L_,  "wb74720,1,74720,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "w15,1,15,0",       0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "R35021",           0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      2,       1,   e_NON_VEC,   ""  },

   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Test if a channel will write nothing as expected after it is closed for
     // any write request. there's bug in the code found by this test:
   {L_,  "wb1040,1,1040,0",  0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "cS",               0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "wb11,1,-3,0",      0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  "dw0",              0,       0,      0,       0,   e_NON_VEC,   ""  },
   {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  },
   },
   { // Enqueue a request with some requests inside its user-installed
     // callback.
   {L_,  "wb74720,1,74720,0", 0,      1,      1,       1,   e_NON_VEC,   ""  },
     {L_,  "wb11,1,11,0,{wb15,0,15,0; wb20,0,20,0,{wb40,1,40,0}}",
                             0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "wb18,1,18,0",      0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "R35769",           0,       1,      3,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      2,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       1,      1,       1,   e_NON_VEC,   ""  },
   {L_,  "dw1",              0,       0,      0,       0,   e_NON_VEC,   ""  },
    {L_,  0,                  0,       0,      0,       0,   e_NON_VEC,   ""  }
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

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);

            ret = btlso::SocketOptUtil::setOption(handles[0],
                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                      btlso::SocketOptUtil::k_SENDBUFFER, BUF_LIMIT);
            LOOP_ASSERT(i, 0 == ret);

            ret = btlso::SocketOptUtil::setOption(handles[1],
                      btlso::SocketOptUtil::k_TCPLEVEL,
                      btlso::SocketOptUtil::k_TCPNODELAY, 1);
            LOOP_ASSERT(i, 0 == ret);

            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);
            char readBuf[MAX_BUF];
            memset(readBuf, '\0', sizeof readBuf);
            Buffer buffer = {readBuf, str, 0, ioVec, 0};
            // Buffer is a struct type where declares different pointers
            // pointing to different I/O buffers, e.g., readBuffer,
            // writeBuffer, iovecBuffer, ovecBuffer.  This struct is declared
            // at the beginning of this test driver.
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel, &buffer,
                                    &eventManager, &eventManager, command);

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
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *command) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P(eventManager.numEvents());
                        cout << endl;
                    }

                    if (SCRIPTS[i][j].d_operationType ==e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
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
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 14: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING 'writevRaw(btls::Ovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        // the specified 'numBytes' from the specified 'buffer' to this
        // channel.  The main concerns about this function are (1) if a write
        // request can be finished right away when the channel send-buffer is
        // writeable; (2) if a write request will be correctly enqueued to the
        // request queue when the channel send-buffer is not writeable, even
        // though the request queue is currently empty; (3) if a write request
        // will be rejected when the channel is invalidated or the channel is
        // closed; (4) if the write request in the request queue will be
        // removed after the request is dispatched and done; (5) if a
        // write-event will be removed when no any write request of this
        // write-event exists or will keep this write-event otherwise; (6) if a
        // different kind of write-event will be registered when next
        // write-event is another kind of write request(e.g.,a bufferedWrite
        // request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   some cases such as "asynchronous event", "ERROR_WOULDBLOCK",
        //   "ERROR_EOF" in the channel callback function are hard to be
        //   duplicated in the test driver, and so they are not covered in the
        //   test case.  By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values which are calculated manually
        //   and put in a table.  Those states include the lengths of
        //   "read request queue", "read event queue", "write request queue",
        //   and "write event queue", also the buffer data in the channel.
        //   The "TCPNODELAY" option for the peer socket is set on because
        //   some small packets will be written to the channel and we the
        //   require these data be read from channel right after this
        //   write.
        //
        // Testing:
        //   int writevRaw(btls::Ovec);
        // -------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'writev(btls::Ovec)' METHOD"
                          << "\n=================================="
                          << "\nTESTING 'writevRaw(btls::Ovec)' METHOD"
                          << "\n====================================="
                          << endl;

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                             // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   // First test channel read and write buffer (entry 1); then test
   // cancelALL(); next test invalidate().  Note that it's important to make
   // sure read and write event will not interfere with each other. line cmd
   // PendingR ReadE PendingW WriteE Type d_expData
   //----- ---             --------  -----  --------  ------  ----  ---------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvor5,1,40,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,1040,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },/*
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   }, */
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R8192",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,29760,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R6000",           0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,29760,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0,{wvor4,1,0,-1; cw; wvor1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor2,1,0,-1",    0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================
    { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvo5,1,40,0",     0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,1040,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R8192",           0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   }, /*
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,29760,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R6000",           0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   }, */
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,29760,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0,{wvo4,1,0,-1; cw; wvo1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo2,1,0,-1",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif defined(BSLS_PLATFORM_OS_AIX)        // ibm test data

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvor5,1,40,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvor6,1,540,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor6,1,540,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,540,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,540,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R540",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor6,1,540,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0,{wvor4,1,0,-1; cw; wvor1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor2,1,0,-1",    0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================
   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvo5,1,40,0",     0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvo6,1,540,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo6,1,540,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,540,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,540,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R540",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo6,1,540,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,1024,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0,{wvo4,1,0,-1; cw; wvo1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo2,1,0,-1",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif BSLS_PLATFORM_OS_LINUX

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvor5,1,40,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,1040,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,1040,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor8,1,16384,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0,{wvor4,1,0,-1; cw; wvor1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor2,1,0,-1",    0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================
   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvo5,1,40,0",     0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,1040,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,1040,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo8,1,16384,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0,{wvo4,1,0,-1; cw; wvo1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo2,1,0,-1",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #else                               // sun test data

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvor5,1,40,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,1040,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvor6,1,1040,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvor3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor7,1,57344,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvor7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvor3,1,18,0,{wvor4,1,0,-1; cw; wvor1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvor2,1,0,-1",    0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================
   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvo5,1,40,0",     0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,1040,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvo6,1,1040,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "wvo3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "dw0",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "cw",              0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvo7,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo7,1,57344,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvo7,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wvo3,1,18,0,{wvo4,1,0,-1; cw; wvo1,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
     { L_, "wvo2,1,0,-1",     0,       1,      2,       1,  e_OVECTOR,   ""  },
     { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,   ""  },
     { L_, "dw1",             0,       1,      1,       1,  e_OVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""  }
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

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                char readBuf[MAX_BUF];  // To read from the channel.
                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.
                Buffer buffer = {0, str, 0, 0, oVec};

                for (int j = 0; j < MAX_CMDS; j++) {
                    const char* iter = SCRIPTS[i][j].d_cmd;
                    if (!iter) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

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

                    int length = gg(&channel, &buffer, 0,
                                    &eventManager, iter);
                    if (length > 0) {
                        if ('W' == *iter) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *iter) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }

                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P_(eventManager.numEvents());
                        P(eventManager.numEvents());
                        cout << endl;
                    }
                    readBuf[0] = '\0';
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 13: {
// TBD FIX ME
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING 'writevRaw(btls::Iovec)' FUNCTION:
        //   Initiate a non-blocking operation to *atomically* write *up *to*
        // the specified 'numBytes' from the specified 'buffer' to this
        // channel.  The main concerns about this function are (1) if a write
        // request can be finished right away when the channel send-buffer is
        // writeable; (2) if a write request will be correctly enqueued to the
        // request queue when the channel send-buffer is not writeable, even
        // though the request queue is currently empty; (3) if a write request
        // will be rejected when the channel is invalidated or the channel is
        // closed; (4) if the write request in the request queue will be
        // removed after the request is dispatched and done; (5) if a
        // write-event will be removed when no any write request of this
        // write-event exists or will keep this write-event otherwise; (6) if a
        // different kind of write-event will be registered when next
        // write-event is another kind of write request(e.g.,a bufferedWrite
        // request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   some cases such as "asynchronous event", "ERROR_WOULDBLOCK",
        //   "ERROR_EOF" in the channel callback function are hard to be
        //   duplicated in the test driver, and so they are not covered in the
        //   test case.  By parsing the command script which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against those expected state values which are calculated manually
        //   and put in a table.  Those states include the lengths of
        //   "read request queue", "read event queue", "write request queue",
        //   and "write event queue", also the buffer data in the channel.
        //   The "TCPNODELAY" option for the peer socket is set on because
        //   some small packets will be written to the channel and we the
        //   require these data be read from channel right after this
        //   write.
        //
        // Testing:
        //   int writevRaw(btls::Iovec);
        // -------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'writevRaw()' METHOD"
                          << "\n============================" << endl;

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                            // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   // First test channel read and write buffer (entry 1); then test
   // cancelALL(); next test invalidate().  Note that it's important to make
   // sure read and write event will not interfere with each other. line cmd
   // PendingR ReadE PendingW WriteE Type d_expData
   //----------------------------------------------------------------------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)      // windows test data
    { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvir5,1,40,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvir6,1,1040,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvir3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "wvir5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   }, /*
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R8192",           0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   }, */
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir7,1,29760,0", 0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R6000",           0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,29760,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvir3,1,18,0,{wvir4,1,0,-1; cw; wvir1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wvir2,1,0,-1",    0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing writev() function.
     // =========================================================
   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvi5,1,40,0",     0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   }, /*
   { // Enqueue 1 request, then dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   }, */
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,1040,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvi3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "wvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R22379",          0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R8192",           0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi7,1,29760,0",  0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R6000",           0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },/*
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,29760,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi3,1,18,0,{wvi4,1,0,-1; cw; wvi1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wvi2,1,0,-1",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R22379",          0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   }, */
   #elif defined(BSLS_PLATFORM_OS_AIX)        // ibm test data

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvir5,1,40,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvir6,1,540,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir6,1,540,0",   0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvir6,1,540,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvir3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvir6,1,540,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvir3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "wvir5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R540",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir6,1,540,0",   0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,1024,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvir3,1,18,0,{wvir4,1,0,-1; cw; wvir1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wvir2,1,0,-1",    0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
     // The following test data is for testing writev() function.
     // =========================================================
   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvi5,1,40,0",     0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvi6,1,540,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi6,1,540,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,540,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvi3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,540,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,   ""   },
    { L_, "wvi3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "wvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R540",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi6,1,540,0",    0,       1,      1,       1,  e_NON_VEC,   ""   },
    { L_, "R500",            0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,1024,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi3,1,18,0,{wvi4,1,0,-1; cw; wvi1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,   ""   },
     { L_, "wvi2,1,0,-1",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R500",            0,       1,      2,       1,  e_NON_VEC,   ""   },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,   ""   },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,   ""   }
   },
   #elif BSLS_PLATFORM_OS_LINUX

   { // Issue 1 request, which should be finished without being enqueued.
    { L_, "wvir5,1,40,0",    0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "dw0",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request, then dispatch.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wvir6,1,1040,0",  0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvir3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "dw0",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request without being dispatched.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wvir6,1,1040,0",  0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvir3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "dw0",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "wvir5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
    { L_, "dw1",             0,       1,      1,       1,  e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "cw",              0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "wb20,1,20,0",     0,       1,      2,       1,  e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
    { L_, "dw1",             0,       1,      1,       1,  e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir7,1,16383,0", 0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "wvir8,1,16384,0", 0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R30000",          0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_IOVECTOR,   ""  },
    { L_, "R40000",          0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,16383,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvir7,1,16383,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvir3,1,18,0,{wvir4,1,0,-1; cw; wvir1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wvir2,1,0,-1",    0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvi5,1,40,0",     0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,1040,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvi3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,1040,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvi3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wvi5,1,40,0",     0,       1,      2,       1,  e_NON_VEC,     ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi8,1,16384,0",  0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi7,1,16383,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi3,1,18,0,{wvi4,1,0,-1; cw; wvi1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wvi2,1,0,-1",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   #else                                       // sun test data

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvir5,1,40,0",    0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvir6,1,1040,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvir3,1,0,-1",    0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvir6,1,1040,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvir3,1,-3,0",    0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "wvir5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,0,-1",    0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir4,0,20,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir3,1,18,0",    0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvir7,1,57344,0", 0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvir7,1,73728,0", 0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvir3,1,18,0,{wvir4,1,0,-1; cw; wvir1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wvir2,1,0,-1",    0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
     // The following test data is for testing writev() function.
     // =========================================================

   { // Issue 1 request, which should be finished without being enqueued.
     { L_, "wvi5,1,40,0",     0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request, then dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,1040,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "iw",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvi3,1,0,-1",     0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request without being dispatched.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
     { L_, "wvi6,1,1040,0",   0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "cS",              0,       0,      0,       0,  e_NON_VEC,    ""  },
    { L_, "wvi3,1,-3,0",     0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_, "dw0",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_, "wvi5,1,40,0",    0,       1,      2,       1,  e_NON_VEC,     ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,0,-1",     0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "cw",              0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi4,0,20,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi3,1,18,0",     0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wb20,1,20,0",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
    { L_, "wvi7,1,57344,0",  0,       1,      1,       1,  e_NON_VEC,    ""  },
    { L_, "R52379",          0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
     { L_, "wvi7,1,73728,0",  0,       0,      0,       0, e_IOVECTOR,   ""  },
     { L_, "wvi3,1,18,0,{wvi4,1,0,-1; cw; wvi1,0,11,0,{wb9,1,9,0};iw}",
                             0,       1,      1,       1,  e_NON_VEC,    ""  },
     { L_, "wvi2,1,0,-1",     0,       1,      2,       1, e_IOVECTOR,   ""  },
    { L_, "R52379",          0,       1,      2,       1,  e_NON_VEC,    ""  },
     { L_, "dw1",             0,       1,      1,       1, e_IOVECTOR,   ""  },
    { L_, "dw1",             0,       0,      0,       0,  e_NON_VEC,    ""  },
     { L_,  0,                0,       0,      0,       0,  e_NON_VEC,    ""  }
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

            btlso::InetStreamSocketFactory<btlso::IPv4Address>
                                                    factory(&testAllocator);
            btlso::StreamSocket<btlso::IPv4Address> *sSocket =
                                            factory.allocate(handles[0]);
            LOOP_ASSERT(i, sSocket);
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);

            {

                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                char readBuf[MAX_BUF];  // To read from the channel.
                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.
                Buffer buffer = {0, str, 0, ioVec, 0};

                for (int j = 0; j < MAX_CMDS; j++) {
                    const char* iter = SCRIPTS[i][j].d_cmd;
                    if (!iter) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

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

                    int length = gg(&channel, &buffer, 0,
                                    &eventManager, iter);
                    if (length > 0) {
                        if ('W' == *iter) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *iter) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
                                cout << "Help read data from the pipe: "
                                     << ret << " bytes." << endl;
                            }
                        }
                    }
                    LOOP_ASSERT(LINE, 0 <= length);

                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P_(eventManager.numEvents());
                        P(eventManager.numEvents());
                        cout << endl;
                    }
                    readBuf[0] = '\0';
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
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
        //   request(e.g., a readvRaw request).
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
        //   int bufferedReadRaw();
        // -------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'bufferedReadRaw' METHOD"
                          << "\n================================" << endl;

        const int e_IOVECTOR = 2, e_NON_VEC = 0;

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
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
        {
   // line cmd PendingR ReadE PendingW WriteE Type d_expData
   { // entry 1 Enqueue 1 request, then dispatch: test if a request is queued
     // and later executed properly.
   { L_, "W4",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr4,0,4,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "1234"    },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // entry 2 Enqueue 1 request, then dispatch: test if a channel can
     // retrieve the specified length of data from the channel.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr7,0,7,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "1234567" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // entry 3 Invalidate the channel, then enter 1 request and try
     // dispatching: test if a request can be issued after the invalidation.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "ir",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr6,0,-1,0",  0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // entry 4 Enqueue 1 request, then dispatch: test if the request can
     // retrieve all data when there isn't enough data in the channel .
   { L_, "W12",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr14,0,12,0", 1,     1,     0,        0,  e_NON_VEC,    ""        },
     { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,
                                                              "1234567890ab" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   }, /*
   { // entry 5 Enqueue 1 requests, then dispatch: test if a request can be
     // done right away when there is enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr2,0,2,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12"      },
   { L_, "rbr7,0,7,0",   0,     0,     0,        0,  e_NON_VEC,    "3456789" },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   }, */
   { // entry 6 Enqueue 1 requests, then dispatch: test if a request can be
     // done right away when there is no enough data in channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rbr5,0,5,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12345"   },
    { L_, "rvr3,0,4,0",   0,     0,     0,        0, e_IOVECTOR,   "6789"    },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // entry 7 Enqueue 2 requests, then dispatch: test if all requests can be
     // finished accordingly.
   { L_, "W16",          0,     0,     0,        0,  e_NON_VEC,      ""      },
   { L_, "rbr3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""      },
   { L_, "rbr13,0,13,0", 2,     1,     0,        0,  e_NON_VEC,      ""      },
     { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,
                                                              "4567890abcdef"},
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // entry 8 Enqueue 2 requests, then dispatch: test if all requests can be
     // finished where there's no enough data in the channel.
   { L_, "W3",           0,     0,     0,        0,  e_NON_VEC,      ""      },
   { L_, "rbr3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""      },
   { L_, "rbr14,0,10,0", 2,     1,     0,        0,  e_NON_VEC,      ""      },
   { L_, "dr1",          1,     1,     0,        0,  e_NON_VEC,     "123"    },
   { L_, "W10",          1,     1,     0,        0,  e_NON_VEC,      ""      },

     { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,
                                                                "1234567890" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // entry 9 Enqueue 2 requests, then dispatch: test if different type of
     // read requests can be finished properly.
    { L_, "W12",         0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rbr3,0,3,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""      },
     { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR, "4567890ab" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // entry 10 Enqueue 2 requests, then dispatch: test if different type of
     // read requests can be finished properly.
    { L_, "W3",          0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rbr3,0,3,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "dr1",         1,     1,     0,        0,  e_NON_VEC,     "123"    },
    { L_, "W9",          1,     1,     0,        0,  e_NON_VEC,      ""      },

     { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR, "123456789" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   }, /*
   { // entry 11 Enqueue 2 requests, then dispatch: test if different type of
     // read requests can be finished properly in a different order from above.
    { L_, "W9",          0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rbr3,0,3,0",  2,     1,     0,        0,  e_NON_VEC,      ""      },
     { L_, "dr1",         1,     1,     0,        0, e_IOVECTOR, "123456789" },
    { L_, "W3",          1,     1,     0,        0,  e_NON_VEC,      ""      },

    { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,     "123"    },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   }, */
   { // entry 12 Test if a channel can still work correctly for new read
     // request after the previous reads are canceled.
    {L_, "W13",          0,      0,     0,        0,  e_NON_VEC,      ""     },
    {L_, "rbr4,0,0,-1",  1,      1,     0,        0,  e_NON_VEC,      ""     },
    {L_, "cr",           0,      0,     0,        0,  e_NON_VEC,      ""     },
    {L_, "rbr13,1,13,0", 1,      1,     0,        0,  e_NON_VEC,      ""     },
     {L_, "dr1",          0,      0,     0,        0,  e_NON_VEC,
                                                             "1234567890abc"},

    {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""      },
   },
   { // entry 13 Concern: if this channel can work correctly for new read
     // requests from the user-installed callback function.
    {L_, "W3",          0,      0,     0,        0,  e_NON_VEC,      ""      },
     {L_, "rbr3,0,3,0,{rbr3,1,3,0}",
                        1,      1,     0,        0,  e_NON_VEC,      ""      },
    {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      "123"   },
    {L_, "W3",          1,      1,     0,        0,  e_NON_VEC,      ""      },
     {L_, "rbr1,1,1,0,{rbr13,1,2,0; rbr7,0,7,0}",
                        2,      1,     0,        0,  e_NON_VEC,      ""      },

    {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      "123"   },
    {L_, "W3",          1,      1,     0,        0,  e_NON_VEC,      ""      },
    {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      "23"    },
    {L_, "W20",         1,      1,     0,        0,  e_NON_VEC,      ""      },
    {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,   "1234567"  },

     {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""      }
   },
 };

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF] = "\0";

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

            btlso::TcpTimerEventManager rEventManager(&testAllocator),
                                       wEventManager(&testAllocator);
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpCbChannel channel(sSocket, &rEventManager,
                                            &wEventManager, &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    char buf0[2] = "\0", buf1[3] = "\0", buf2[5] = "\0",
                    buf3[7] = "\0", buf4[9] = "\0", buf5[10] = "\0";

                    btls::Iovec vecBuffer[MAX_VEC];
                    vecBuffer[0].setBuffer(buf0, sizeof buf0 - 1);
                    vecBuffer[1].setBuffer(buf1, sizeof buf1 - 1);
                    vecBuffer[2].setBuffer(buf2, sizeof buf2 - 1);
                    vecBuffer[3].setBuffer(buf3, sizeof buf3 - 1);
                    vecBuffer[4].setBuffer(buf4, sizeof buf4 - 1);
                    vecBuffer[5].setBuffer(buf5, sizeof buf5 - 1);

                    Buffer buffer = {buf, 0, vecBuffer, 0, 0};

                    int length = gg(&channel, &buffer, &rEventManager,
                                     &wEventManager, command);

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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(rEventManager.numEvents());
                        P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType ==e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
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
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'readv(), readvRaw()' FUNCTION:
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
        //   against those expected state values which are calculated manually.
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
        // -------------------------------------------------------------------
        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        if (verbose) cout << endl
                          << "\nTESTING 'readv' METHOD"
                          << "\n======================"
                          << "\nTESTING 'readvRaw' METHOD"
                          << "\n=========================" << endl;
        {
            struct {
                int         d_line;  // line number
                const char *d_cmd;   // the command to be executed on a channel

                int         d_numPendingRead; // expected number of pending
                                              // read operations on a channel
                int         d_numReadEvent;   // expected number of callbacks
                                              // registered with a read event
                                              // manager
                int         d_numPendingWrite;// expected number of pending
                                           // write operations on a channel
                int         d_numWriteEvent;  // expected number of callbacks
                                              // registered with a write event
                                              // manager
                int         d_operationType;  // if it's a read with vector

                const char *d_expData;        // expected data read from the
                                              // channel
            } SCRIPTS[][MAX_CMDS] =
            {
     // line cmd PendingR ReadE PendingW WriteE Type d_expData
/* { // Enqueue 1 request, then dispatch: test if the request is queued
     // and later executed properly.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rvr1,0,1,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
    { L_, "dr1",          0,     0,     0,        0, e_IOVECTOR,   "1"       },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   }, */
   { // Enqueue 1 request, then dispatch: test if the channel can retrieve the
     // specified length of data from the channel.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rvr3,0,7,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_IOVECTOR,   "1234567" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Invalidate the channel, then enter 1 request and try dispatching: test
     // if any read request can be issued after the invalidation.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "ir",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rvr6,0,-1,0",  0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   }, /*
   { // Enqueue 1 request, then dispatch: test if the request can retrieve all
     // data when there isn't enough data in the channel .
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rvr3,0,4,0",   1,     1,     0,        0,  e_NON_VEC,    ""        },
    { L_, "dr1",          0,     0,     0,        0, e_IOVECTOR,   "1234"    },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   */
   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb2,0,2,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12"      },
   { L_, "rvr3,0,7,0",   0,     0,     0,        0,  e_IOVECTOR,   "3456789" },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is not enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb5,0,5,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12345"   },
   { L_, "rvr3,0,4,0",   0,     0,     0,        0,  e_IOVECTOR,   "6789"    },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // accordingly.
    { L_, "W16",         0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr2,0,3,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,13,0", 2,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "dr1",         1,     1,     0,        0,  e_IOVECTOR,     "123"   },
     { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR,
                                                            "4567890abcdef" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   }, /*
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // accordingly.
    { L_, "W16",         0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr2,0,3,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,13,0", 2,     1,     0,        0,  e_NON_VEC,      ""      },
     { L_, "dr1",         1,     1,     0,        0, e_IOVECTOR,     "123"   },
     { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR,
                                                            "4567890abcdef" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   }, */
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
    { L_, "W12",         0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR, "4567890ab" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
    { L_, "W3",          0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "dr1",         1,     1,     0,        0,  e_NON_VEC,     "123"    },
    { L_, "W9",          1,     1,     0,        0,  e_NON_VEC,      ""      },

    { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR, "123456789" },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly in a different order.
    { L_, "W9",          0,     0,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rvr4,0,9,0",  1,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "rb3,0,3,0",   2,     1,     0,        0,  e_NON_VEC,      ""      },
    { L_, "dr1",         1,     1,     0,        0,  e_IOVECTOR, "123456789" },
    { L_, "W3",          1,     1,     0,        0,  e_NON_VEC,      ""      },

    { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,     "123"    },
     { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""      }
   },
   { // Test if a channel can still work correctly for new read request after
     // the previous reads are canceled.
    {L_, "W13",         0,      0,     0,        0,  e_NON_VEC,      ""      },
    {L_, "r4,0,0,-1",   1,      1,     0,        0,  e_NON_VEC,      ""      },
    {L_, "cr",          0,      0,     0,        0,  e_NON_VEC,      ""      },
    {L_, "rvr4,1,13,0", 1,      1,     0,        0,  e_NON_VEC,      ""      },
     {L_, "dr1",         0,      0,     0,        0,  e_IOVECTOR,
                                                             "1234567890abc"},

    {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""      },
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
    {L_, "W20",         0,      0,     0,        0,  e_NON_VEC,      ""      },
     {L_, "rvr2,0,3,0,{rvr2,1,3,0}",
                        1,      1,     0,        0,  e_NON_VEC,      ""      },
    {L_, "dr1",         1,      1,     0,        0,  e_IOVECTOR,     "123"   },
    {L_, "W7",          1,      1,     0,        0,  e_NON_VEC,      ""      },
     {L_, "rvr1,1,1,0,{rvr4,1,13,0; rvr3,0,7,0}",
                        2,      1,     0,        0,  e_NON_VEC,      ""      },

    {L_, "dr1",         1,      1,     0,        0,  e_IOVECTOR,     "456"   },
    {L_, "dr1",         2,      1,     0,        0,  e_IOVECTOR,     "7"     },
     {L_, "dr1",         1,      1,     0,        0,  e_IOVECTOR,
                                                             "890abcdefghij"},
    {L_, "dr1",         0,      0,     0,        0,  e_IOVECTOR,    "1234567"},
     {L_,  0,            0,      0,     0,        0,   e_NON_VEC,     ""      }
   },
     // The following test data is to test readv() function.
     // ====================================================
   { // Enqueue 1 request, then dispatch: test if the request is queued and
     // later executed properly.
   { L_, "W4",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rv1,0,1,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_IOVECTOR,   "1"       },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 request, then dispatch: test if the channel can retrieve the
     // specified length of data from the channel.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rv3,0,7,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_IOVECTOR,   "1234567" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Invalidate the channel, then enter 1 request and try dispatching: test
     // if any read request can be issued after the invalidation.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "ir",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rv6,0,-1,0",   0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 request, then dispatch: test if the request can retrieve all
     // data when there isn't enough data in the channel .
   { L_, "W4",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rv3,0,4,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_IOVECTOR,   "1234"    },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },

   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb2,0,2,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12"      },
   { L_, "rv3,0,7,0",    0,     0,     0,        0,  e_IOVECTOR,   "3456789" },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   }, /*
   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is not enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb5,0,5,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12345"   },
    { L_, "rv3,0,4,0",    0,     0,     0,        0, e_IOVECTOR,   "6789"    },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },*/
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // accordingly.
   { L_, "W16",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv2,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv4,0,13,0",  2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         1,     1,     0,        0,  e_IOVECTOR,     "123"    },
     { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR,
                                                            "4567890abcdef"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   }, /*
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // accordingly.
   { L_, "W16",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv2,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv4,0,13,0",  2,     1,     0,        0,  e_NON_VEC,      ""       },
    { L_, "dr1",         1,     1,     0,        0, e_IOVECTOR,     "123"    },
     { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR,
                                                            "4567890abcdef"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
   { L_, "W12",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv4,0,9,0",   2,     1,     0,        0,  e_NON_VEC,      ""       },
    { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR, "4567890ab"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   }, */
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
   { L_, "W3",          0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv4,0,9,0",   2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         1,     1,     0,        0,  e_NON_VEC,     "123"     },
   { L_, "W9",          1,     1,     0,        0,  e_NON_VEC,      ""       },

   { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR, "123456789"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly in a different order.
   { L_, "W9",          0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rv4,0,9,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         1,     1,     0,        0,  e_IOVECTOR, "123456789"  },
   { L_, "W3",          1,     1,     0,        0,  e_NON_VEC,      ""       },

   { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,     "123"     },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Test if a channel can still work correctly for new read request after
     // the previous reads are canceled.
   {L_, "W13",         0,      0,     0,        0,  e_NON_VEC,      ""       },
   {L_, "r4,0,0,-1",   1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "cr",          0,      0,     0,        0,  e_NON_VEC,      ""       },
   {L_, "rv4,1,13,0",  1,      1,     0,        0,  e_NON_VEC,      ""       },
     {L_, "dr1",         0,      0,     0,        0,  e_IOVECTOR,
                                                              "1234567890abc"},

   {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       },
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
   {L_, "W20",         0,      0,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rv2,0,3,0,{rv2,1,3,0}",
                       1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         1,      1,     0,        0,  e_IOVECTOR,     "123"    },
   {L_, "W7",          1,      1,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rv1,1,1,0,{rv4,1,13,0; rv3,0,7,0}",
                       2,      1,     0,        0,  e_NON_VEC,      ""       },

   {L_, "dr1",         1,      1,     0,        0,  e_IOVECTOR,     "456"    },
   {L_, "dr1",         2,      1,     0,        0,  e_IOVECTOR,     "7"      },
     {L_, "dr1",         1,      1,     0,        0, e_IOVECTOR,
                                                              "890abcdefghij"},
    {L_, "dr1",         0,      0,     0,        0, e_IOVECTOR,    "1234567" },
    {L_,  0,            0,      0,     0,        0,   e_NON_VEC,     ""       }
   },
 };

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

                btlso::TcpTimerEventManager rEventManager(&testAllocator),
                                       wEventManager(&testAllocator);
                {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                    btlsos::TcpCbChannel channel(sSocket, &rEventManager,
                                           &wEventManager, &testAllocator);

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

                        int length = gg(&channel, &buffer, &rEventManager,
                                        &wEventManager, command);

                        // There are 9 parameters in the bufferedReadCallback()
                        // function.  This is the maximum number of parameters
                        // to call makeF() for a functor object.  If we have
                        // the following commands executed in gg(), the socket
                        // handle need to be passed to gg() and the
                        // bufferedReadCallback().  To do so, we need to use a
                        // struct to wrap some of the parameters to reduce the
                        // number of function parameters.  But that will make
                        // the function call not straightforward.  That's why
                        // we execute the following commands outside gg().

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
                        }
                        if (channel.writeEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent
                                  == channel.writeEventManager()->numEvents());
                        }
                        if (veryVerbose) {
                            P_(LINE);
                            P_(channel.numPendingReadOperations());
                            P(rEventManager.numEvents());
                            P(SCRIPTS[i][j].d_expData);
                            cout << endl;
                        }
                        if (SCRIPTS[i][j].d_operationType ==e_IOVECTOR) {
                            helpAssertVecData(i, j,
                                          SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                        }
                        else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
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
                btlso::SocketImpUtil::close(handles[0]);
                btlso::SocketImpUtil::close(handles[1]);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bufferedRead()' FUNCTION:
        //   The main concerns about the function are (1) if a read request
        //   can be finished right away when the channel recv-buffer is
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
        //   request(e.g., a readvRaw request).
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
        // -------------------------------------------------------------------
        if (verbose) cout << endl
                          << "\nTESTING 'bufferedRead' METHOD"
                          << "\n=============================" << endl;

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
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
        {
   // line cmd PendingR ReadE PendingW WriteE Type d_expData
   { // Enqueue 1 request, then dispatch: test if the request is queued and
     // later executed properly.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb4,0,4,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "1234"    },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 request, then dispatch: test if the channel can retrieve the
     // specified length of data from the channel.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb7,0,7,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "1234567" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Invalidate the channel, then enter 1 request and try dispatching: test
     // if any read request can be issued after the invalidation.
   { L_, "W11",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "ir",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb6,0,-1,0",   0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 request, then dispatch: test if the channel can retrieve all
     // data when there is enough data in the channel .
   { L_, "W12",          0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb12,0,12,0",  1,     1,     0,        0,  e_NON_VEC,    ""        },
     { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,
                                                              "1234567890ab" },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb2,0,2,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12"      },
   { L_, "rvr3,0,7,0",   0,     0,     0,        0,  e_IOVECTOR,   "3456789" },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 1 requests, then dispatch: test if a request can be done right
     // away when there is not enough data in the channel's read buffer.
   { L_, "W9",           0,     0,     0,        0,  e_NON_VEC,    ""        },
   { L_, "rb5,0,5,0",    1,     1,     0,        0,  e_NON_VEC,    ""        },
   { L_, "dr1",          0,     0,     0,        0,  e_NON_VEC,    "12345"   },
   { L_, "rvr3,0,4,0",   0,     0,     0,        0,  e_IOVECTOR,   "6789"    },
   { L_, "dr0",          0,     0,     0,        0,  e_NON_VEC,    ""        },
    { L_,  0,             0,     0,     0,        0,  e_NON_VEC,    ""        }
   },
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // accordingly.
   { L_, "W16",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb13,0,13,0", 2,     1,     0,        0,  e_NON_VEC,      ""       },
     { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,
                                                              "4567890abcdef"},
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Enqueue 2 requests, then dispatch: test if all requests can be finished
     // where there's no enough data in the channel.
   { L_, "W13",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb14,0,14,0", 2,     1,     0,        0,  e_NON_VEC,      ""       },
     { L_, "dr1",         1,     1,     0,        0,  e_NON_VEC,
                                                                    "123"    },
   { L_, "W5",          1,     1,     0,        0,  e_NON_VEC,      ""       },

     { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,
                                                             "4567890abc1234"},
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
   { L_, "W12",         0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         0,     0,     0,        0,  e_IOVECTOR, "4567890ab"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   }, /*
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly.
   { L_, "W3",          0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rvr4,0,9,0",  2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         1,     1,     0,        0,  e_NON_VEC,     "123"     },
   { L_, "W9",          1,     1,     0,        0,  e_NON_VEC,      ""       },

    { L_, "dr1",         0,     0,     0,        0, e_IOVECTOR, "123456789"  },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   }, */
   { // Enqueue 2 requests, then dispatch: test if different type of read
     // requests can be finished properly in a different order.
   { L_, "W9",          0,     0,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rvr4,0,9,0",  1,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "rb3,0,3,0",   2,     1,     0,        0,  e_NON_VEC,      ""       },
   { L_, "dr1",         1,     1,     0,        0,  e_IOVECTOR, "123456789"  },
   { L_, "W3",          1,     1,     0,        0,  e_NON_VEC,      ""       },

   { L_, "dr1",         0,     0,     0,        0,  e_NON_VEC,     "123"     },
    { L_,  0,            0,     0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Test if a channel can still work correctly for new read request after
     // the previous reads are canceled.
   {L_, "W13",         0,      0,     0,        0,  e_NON_VEC,      ""       },
   {L_, "rb4,0,0,-1",  1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "cr",          0,      0,     0,        0,  e_NON_VEC,      ""       },
   {L_, "rb13,1,13,0", 1,      1,     0,        0,  e_NON_VEC,      ""       },
     {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,
                                                              "1234567890abc"},

   {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       },
   }, /*
   { // Test if a channel works correctly for new read request if read request
     // length is always larger than the length of data in the buffer, so it
     // depends on the number of system calls to decide if this request is
     // finished.
   {L_, "W1",          0,      0,     0,        0,  e_NON_VEC,      ""       },
   {L_, "rb3,1,3,0",   1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "W1",          1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      ""       },

   {L_, "W1",          1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,      "111"    },
   {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       },
   },   */
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
   {L_, "W3",          0,      0,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rb3,0,3,0,{rb3,1,3,0}",
                       1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      "123"    },
   {L_, "W3",          1,      1,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rb1,1,1,0,{rb13,1,13,0; rb7,0,7,0}",
                       2,      1,     0,        0,  e_NON_VEC,      ""       },

   {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,      "123"    },
   {L_, "W3",          1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         2,      1,     0,        0,  e_NON_VEC,      "1"      },
   {L_, "W20",         2,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,   "bcdefgh"   },

    {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
   {L_, "W10",         0,      0,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rb3,0,3,0,{rb4,1,4,0}",
                       1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,      "4567"   },
   {L_, "W20",         0,      0,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rb1,1,1,0,{rb13,1,13,0; rb7,0,7,0}",
                       2,      1,     0,        0,  e_NON_VEC,      ""       },

   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,    "bcdefgh"  },
    {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       }
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
   {L_, "W10",         0,      0,     0,        0,  e_NON_VEC,      ""       },
     {L_, "rb3,0,3,0,{rb2,1,2,0}",
                       1,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,      "45"     },
     {L_, "rb2,1,2,0,{r4,1,4,0; rb3,0,3,0}",
                       2,      1,     0,        0,  e_NON_VEC,      ""       },

   {L_, "W4",          2,      1,     0,        0,  e_NON_VEC,      ""       },
   {L_, "dr1",         1,      1,     0,        0,  e_NON_VEC,    "8901"     },
   {L_, "dr1",         0,      0,     0,        0,  e_NON_VEC,    "234"      },
    {L_,  0,            0,      0,     0,        0,  e_NON_VEC,      ""       }
   },
 };

        const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

        for (int i = 0; i < NUM_SCRIPTS; ++i) {
            char buf[MAX_BUF] = "\0";

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

            btlso::TcpTimerEventManager rEventManager(&testAllocator),
                                       wEventManager(&testAllocator);
            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor
                // and the behavior is undefined otherwise.  We insure the
                // required order by creating the 'channel' inside a block
                // while the corresponding 'streamSocket' object outside the
                // block as above.

                btlsos::TcpCbChannel channel(sSocket, &rEventManager,
                                            &wEventManager, &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    char buf0[2] = "\0", buf1[3] = "\0", buf2[5] = "\0",
                    buf3[7] = "\0", buf4[9] = "\0", buf5[10] = "\0";

                    btls::Iovec vecBuffer[MAX_VEC];
                    vecBuffer[0].setBuffer(buf0, sizeof buf0 - 1);
                    vecBuffer[1].setBuffer(buf1, sizeof buf1 - 1);
                    vecBuffer[2].setBuffer(buf2, sizeof buf2 - 1);
                    vecBuffer[3].setBuffer(buf3, sizeof buf3 - 1);
                    vecBuffer[4].setBuffer(buf4, sizeof buf4 - 1);
                    vecBuffer[5].setBuffer(buf5, sizeof buf5 - 1);

                    Buffer buffer = {buf, 0, vecBuffer, 0, 0};

                    int length = gg(&channel, &buffer, &rEventManager,
                                     &wEventManager, command);

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
                            P_(BUFFERS[4].d_sndBuf);
                            P(length);
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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(rEventManager.numEvents());
                        P(SCRIPTS[i][j].d_expData);
                        P(buffer.d_readBuf);
                        cout << endl;
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
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
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'read()' FUNCTION:
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
        //   int read();
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "\nTESTING 'read' METHOD"
                          << "\n=====================" << endl;

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
            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
        {

   //d_line   d_cmd  PendingRead  ReadEvent  PendingWrite  WriteEve  d_expData
   { // Enqueue 1 request, then dispatch: test if the request can be queued
     // properly.
     { L_,   "W4",         0,        0,            0,         0,     ""      },
     { L_,   "r4,0,4,0",   1,        1,            0,         1,     ""      },
     { L_,   "dr1",        0,        0,            0,         0,     "j123"  },
     { L_,   0,            0,        0,            0,         0,     ""      }
   },
   { // Enqueue 1 request, then dispatch: test if the request can retrieve
     // specified length of data from the channel.
     { L_,   "W4",         0,        0,            0,         0,     ""      },
     { L_,   "r3,0,3,0",   1,        1,            0,         1,     ""      },
     { L_,   "dr1",        0,        0,            0,         0,     "j12"   },
     { L_,   0,            0,        0,            0,         0,     ""      }
   },
   { // Enqueue 1 request, then dispatch: test if the request can retrieve
     // specified length of data from the channel.
     { L_,  "W11",         0,        0,            0,         0,     ""      },
     { L_,  "r2,0,2,0",    1,        1,            0,         1,     ""      },
     { L_,  "dr1",         0,        0,            0,         0,     "j1"    },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Invalidate the channel, enter 1 request, then try dispatching: test if
     // any read request can be issued after the invalidation.
     { L_,  "W11",         0,        0,            0,         0,     ""      },
     { L_,  "ir",          0,        0,            0,         0,     ""      },
     { L_,  "rr6,0,-1,0",  0,        0,            0,         0,     ""      },
     { L_,  "dr0",         0,        0,            0,         0,     ""      },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Enqueue 1 request, then dispatch: test if a read request can be
     // finished properly when it can't be done once and so this request need
     // to be dispatched again.
     { L_,  "W4",          0,        0,            0,         0,     ""      },
     { L_,  "r5,0,5,0",    1,        1,            0,         1,     ""      },
     { L_,  "dr1",         1,        1,            0,         1,     "j123"  },
     { L_,  "W2",          1,        1,            0,         1,     ""      },
     { L_,  "dr1",         0,        0,            0,         0,     "j123j" },

     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: test if a request can be done right
     // away when there is enough data in the channel's read buffer.
     { L_,  "W11",         0,        0,            0,         0,     ""      },
     { L_,  "rb2,0,2,0",   1,        1,            0,         1,     ""      },
     { L_,  "dr1",         0,        0,            0,         0,     "j1"    },
     { L_,  "r5,0,5,0",    0,        0,            0,         0,     "23456" },
     { L_,  "dr0",         0,        0,            0,         0,     ""      },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: test if a request can be done right
     // away when there isn't enough data in the channel's read buffer.  This
     // request need to be dequeued then.
     { L_,  "W5",          0,        0,            0,         0,     ""      },
     { L_,  "rb2,0,2,0",   1,        1,            0,         1,     ""      },
     { L_,  "dr1",         0,        0,            0,         0,     "j1"    },
     { L_,  "r5,0,5,0",    1,        1,            0,         1,     "234"   },
     { L_,  "W2",          1,        1,            0,         1,     "234"   },

     { L_,  "dr1",         0,        0,            0,         0,     "234j1" },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: concern (4) and (5).
     { L_,  "W11",         0,        0,            0,         0,     ""      },
     { L_,  "r4,0,4,0",    1,        1,            0,         1,     ""      },
     { L_,  "r3,0,3,0",    2,        1,            0,         1,     ""      },
     { L_,  "dr1",         1,        1,            0,         1,     "j123"  },
     { L_,  "dr1",         0,        0,            0,         0,     "456"   },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: test if the different type of read
     // requests can be finished properly in a channel.
     { L_,  "W11",         0,        0,            0,         0,     ""      },
     { L_,  "r5,0,5,0",    1,        1,            0,         1,     ""      },
     { L_,  "rb2,0,2,0",   2,        1,            0,         1,     ""      },
     { L_,  "dr1",         1,        1,            0,         1,     "j1234" },
     { L_,  "dr1",         0,        0,            0,         0,     "56"    },
     { L_,  0,             0,        0,            0,         0,     ""      }
   },
   { // Test if a channel can still work correctly for new read request.after
     // the previous reads are canceled.
     {L_,  "W11",          0,        0,            0,         0,     ""      },
     {L_,  "r4,0,0,-1",    1,        1,            0,         1,     ""      },
     {L_,  "cr",           0,        0,            0,         0,     ""      },
     {L_,  "r3,1,3,0",     1,        1,            0,         1,     ""      },
     {L_,  "dr1",          0,        0,            0,         0,     "j12"   },
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
     {L_,  "W11",           0,        0,            0,         0,     ""     },
     {L_,  "r3,0,3,0,{r2,1,2,0}",
                           1,        1,            0,         1,     ""      },
     {L_,  "dr1",          1,        1,            0,         1,     "j12"   },
     {L_,  "W4",           1,        1,            0,         1,     ""      },
     {L_,  "r1,1,1,0,{r5,0,5,0; r3,1,3,0}",
                           2,        1,            0,         1,     ""      },
     {L_,  "dr1",          1,        1,            0,         1,     "34"    },
     {L_,  "dr1",          2,        1,            0,         1,     "5"     },
     {L_,  "dr1",          1,        1,            0,         1,     "67890" },
     {L_,  "dr1",          0,        0,            0,         0,     "j12"   },
     {L_,  0,              0,        0,            0,         0,     ""      }
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

                btlsos::TcpCbChannel channel(sSocket, &eveManager,
                                        &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    int length = gg(&channel,
                                    &buffer,
                                    &eveManager,
                                    0,
                                    command);

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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(eveManager.numEvents());
                        P_(buf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }

//                     LOOP_ASSERT(LINE, 0 == strncmp(buf,
//                                           SCRIPTS[i][j].d_expData,
//                                           strlen(SCRIPTS[i][j].d_expData)));
                }
            }
            factory.deallocate(sSocket);
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
      } break;
      case 8: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX)
        // --------------------------------------------------------------------
        // TESTING 'write()' FUNCTION:
        //   The main concerns about this function are (1) if a write
        //   request can be finished right away when the channel send-buffer
        //   is writeable; (2) if a write request is correctly enqueued to
        //   the request queue when the channel send-buffer is not writeable,
        //   even though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   will be removed after the request is dispatched and done; (5) if
        //   a write-event will be removed when no any write request of this
        //   write-event exists or will keep this write-event otherwise; (6)
        //   if a different kind of write-event will be registered when next
        //   write-event is another kind of write request(e.g.,a bufferedWrite
        //   request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   some cases such as signal interruption (e.g., asynchronous
        //   interrupt), "ERROR_WOULDBLOCK" in the channel callback function
        //   are hard to be duplicated in the test driver, and so they are
        //   not covered in the test case.  By parsing the command script
        //   which is defined by the SCRIPT language as described above,
        //   different operations will be executed on the channel.  Upon the
        //   execution of each operation, the states of the channel will be
        //   compared against those expected state values which are calculated
        //   manually and put in a table.  Those states include the lengths
        //   of "read request queue", "read event queue", "write request
        //   queue", and "write event queue", also the buffer data in the
        //   channel.  The "TCPNODELAY" option for the peer socket is set on
        //   because some small packets will be written to the channel and we
        //   the require these data be read from channel right after this
        //   write.
        //
        // Testing
        //   write()
        // -------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'write()' METHOD" << endl
                          << "========================" << endl;

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                             // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   // First test channel read and write buffer (entry 1); then test
   // cancelALL(); next test invalidate().  Note that it's important to make
   // sure read and write event will not interfere with each other. line cmd
   // PendingR ReadE PendingW WriteE Type d_expData
   //----------------------------------------------------------------------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)        // windows test data

   { // Issue 1 request, which should be finished without being enqueued.
   { L_, "w40,1,40,0",        0,       0,      0,       0,  e_NON_VEC,  ""   },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,  ""   },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,  ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w1040,1,1040,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w220,1,220,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enter 2 requests, make the second only partial write before dispatch,
     // then dispatch to finish that request.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w28720,1,28720,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w18,1,18,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w40,1,40,0",        0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },

   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w20,0,20,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },

   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w28,1,28,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },

   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },  /*
   { // Enqueue 2 requests, make the first one couldn't extract the specified
     // length of data during a dispatch, and so another dispatch is needed to
     // finish that request..
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w28720,1,28720,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w680,1,680,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },

   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   }, */
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w28720,1,28720,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_, "w18,1,18,0,{w4,1,0,-1; cw; w11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w2,1,0,-1",         0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },

   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif defined(BSLS_PLATFORM_OS_AIX)          // ibm test data

   { // Issue 1 request, which should be finished without being enqueued.
    { L_, "w40,1,40,0",        0,       0,      0,       0,  e_NON_VEC,  ""  },
    { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,  ""  },
     { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,  ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w240,1,240,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w220,1,220,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enter 2 requests, make the second only partial write before dispatch,
     // then dispatch to finish that request.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w1020,1,1020,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w240,1,240,0",      0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "cS",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,-3,0",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w18,1,18,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w40,1,40,0",        0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w20,0,20,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w28,1,28,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the first one couldn't extract the specified
     // length of data during a dispatch, and so another dispatch is needed to
     // finish that request..
   { L_, "w1024,1,1024,0",    0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w1025,1,1025,0",    0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w680,1,680,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      2,       1,  e_OVECTOR,   ""  },

   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w1024,1,1024,0",  0,       0,      0,       0,  e_OVECTOR,   ""    },
     { L_, "w18,1,18,0,{w4,1,0,-1; cw; w11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w2,1,0,-1",         0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif BSLS_PLATFORM_OS_LINUX

   { // Issue 1 request, which should be finished without being enqueued.
   { L_, "w40,1,40,0",        0,       0,      0,       0,  e_NON_VEC,  ""   },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,  ""   },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,  ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w1040,1,1040,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w220,1,220,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enter 2 requests, make the second only partial write before dispatch,
     // then dispatch to finish that request.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w20000,1,20000,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w10,1,0,-1",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w1040,1,1040,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "cS",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,-3,0",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w18,1,18,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w40,1,40,0",        0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w20,0,20,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w28,1,28,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the first one couldn't extract the specified
     // length of data during a dispatch, and so another dispatch is needed to
     // finish that request..
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w30000,1,30000,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w680,1,680,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w16383,1,16383,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "w18,1,18,0,{w4,1,0,-1; cw; w11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w2,1,0,-1",         0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #else                                         // sun test data

   { // Issue 1 request, which should be finished without being enqueued.
   { L_, "w40,1,40,0",        0,       0,      0,       0,  e_NON_VEC,  ""   },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,  ""   },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,  ""   }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w1040,1,1040,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w220,1,220,0",      0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enter 2 requests, make the second only partial write before dispatch,
     // then dispatch to finish that request.
   { L_, "w23680,1,23680,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w73680,1,73680,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "w1040,1,1040,0",    0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "cS",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,-3,0",         0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w18,1,18,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w40,1,40,0",        0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w3,1,0,-1",         0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "w20,0,20,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w28,1,28,0",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the first one couldn't extract the specified
     // length of data during a dispatch, and so another dispatch is needed to
     // finish that request..
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "w73680,1,73680,0",  0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w680,1,680,0",      0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "w73728,1,73728,0",  0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "w18,1,18,0,{w4,1,0,-1; cw; w11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "w2,1,0,-1",         0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R52379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
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
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                char readBuf[MAX_BUF];  // To read from the channel.
                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.
                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; j++) {
                    const char* iter = SCRIPTS[i][j].d_cmd;
                    if (!iter) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

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

                    int length = gg(&channel, &buffer, 0,
                                    &eventManager, iter);
                    if (length > 0) {
                        if ('W' == *iter) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *iter) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P_(eventManager.numEvents());
                        P(eventManager.numEvents());
                        cout << endl;
                    }
                    readBuf[0] = '\0';
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 7: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
        // --------------------------------------------------------------------
        // TESTING 'writeRaw()' FUNCTION:
        //   The main concerns about this function are (1) if a write
        //   request can be finished right away when the channel send-buffer is
        //   writeable; (2) if a write request will be correctly enqueued to
        //   the request queue when the channel send-buffer is not writeable,
        //   even though the request queue is currently empty; (3) if a write
        //   request will be rejected when the channel is invalidated or the
        //   channel is closed; (4) if the write request in the request queue
        //   will be removed after the request is dispatched and done; (5) if
        //   a write-event will be removed when no any write request of this
        //   write-event exists or will keep this write-event otherwise;
        //   (6) if a different kind of write-event will be registered when
        //   next write-event is another kind of write request(e.g.,a
        //   bufferedWrite request).
        //
        // Plan:
        //   As the white-box test, identify each test-branch in the function
        //   and its corresponding event-callback function to make a through
        //   test, design the test commands such that these test commands
        //   will go through each separate test-branch identified before,
        //   to make sure all the above concerns be resolved.  Note that
        //   cases such as signal interruption (e.g., asynchronous event),
        //   "ERROR_WOULDBLOCK" in the channel callback function are hard
        //   to be duplicated in the test driver, and so they are not covered
        //   in the test case.  By parsing the command script which is
        //   defined by the SCRIPT language as described above, different
        //   operations will be executed on the channel.  Upon the execution
        //   of each operation, the states of the channel will be compared
        //   against those expected state values which are manually calculated
        //   and put in a table.  Those states include the lengths of
        //   "read request queue", "read event queue", "write request queue",
        //   and "write event queue", also the buffer data in the channel.
        //   The "TCPNODELAY" option for the peer socket is set on because
        //   some small packets will be written to the channel and we the
        //   require these data be read from channel right after this write.
        //
        // Testing:
        //   int writeRaw();
        // -------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'writeRaw' METHOD"
                          << "\n=========================" << endl;

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;  // line number
            const char *d_cmd;   // command to be executed on a channel

            int         d_numPendingRead;// expected number of pendingRead
                                         // operations on a channel
            int         d_numReadEvent;  // expected number of callbacks
                                         // registered with a read-event
                                         // manager
            int         d_numPendingWrite;  // expected number of pending
                                             // write operations on a channel
            int         d_numWriteEvent; // expected number of callbacks
                                         // registered with a write-event
                                         // manager
            int         d_operationType; // if it's a read with vector

            const char *d_expData;       // expected data read from the channel
        } SCRIPTS[][MAX_CMDS] =
 {
   // First test channel read and write buffer (entry 1); then test
   // cancelALL(); next test invalidate().  Note that it's important to make
   // sure read and write event will not interfere with each other. line cmd
   // PendingR ReadE PendingW WriteE Type d_expData
   //----------------------------------------------------------------------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)        // windows test data
   { // Issue 1 request, which should be finished without being enqueued.
   { L_, "wr40,1,40,0",       0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr220,1,220,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "wr1040,1,1040,0",   0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr3,1,0,-1",        0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr3,1,0,-1",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R32379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr18,1,18,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wr40,1,40,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr3,1,0,-1",        0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr20,0,20,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R52379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },/*
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr28,1,28,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R22379",            0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   }, */
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr28720,1,28720,0", 0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R22379",            0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr28720,1,28720,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wr18,1,18,0,{wr4,1,0,-1; cw; wr11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wr2,1,0,-1",        0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R2379",             0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },

   #elif defined(BSLS_PLATFORM_OS_AIX)          // ibm test data

   { // Issue 1 request, which should be finished without being enqueued.
   { L_, "wr40,1,40,0",       0,       0,      0,       0,  e_NON_VEC,  ""   },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,  ""   },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,  ""   }
   },
   { // Enqueue 1 request, then dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr220,1,220,0",     0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "wr240,1,240,0",     0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "iw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr3,1,0,-1",        0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr50,1,0,-1",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
   { L_, "wr240,1,240,0",     0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "cS",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr3,1,-3,0",        0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "dw0",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr18,1,18,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wr40,1,40,0",       0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr50,1,0,-1",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "cw",                0,       0,      0,       0,  e_NON_VEC,   ""  },
   { L_, "wr20,0,20,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr28,1,28,0",       0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr1020,1,1020,0",   0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "R500",              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_,  0,                  0,       0,      0,       0,  e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr1020,1,1020,0",   0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wr18,1,18,0,{wr4,1,0,-1; cw; wr11,0,11,0,{wb9,1,9,0};iw}",
                              0,       1,      1,       1,  e_NON_VEC,   ""  },
   { L_, "wr2,1,0,-1",        0,       1,      2,       1,  e_OVECTOR,   ""  },
   { L_, "R500",              0,       1,      2,       1,  e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   #elif BSLS_PLATFORM_OS_LINUX
   { // Issue 1 request, which should be finished without being enqueued.
    { L_, "wr40,1,40,0",       0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr20,1,20,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wr1040,1,1040,0",   0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "iw",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wr1040,1,1040,0",   0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "cS",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,-3,0",        0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr20,1,20,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "wr40,1,40,0",       0,       1,      2,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr16383,1,16383,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "cw",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr20,0,20,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_, "wr28,1,28,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_, "wr33000,1,16384,0", 0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_, "R52379",            0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
   { L_, "wr16383,1,16383,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wr18,1,18,0,{wr4,1,0,-1; cw; wr11,0,11,0,{wb9,1,9,0};iw}",
                               0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "wr2,1,0,-1",        0,       1,      2,       1,  e_OVECTOR,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   #else   // sun test data

   { // Issue 1 request, which should be finished without being enqueued.
    { L_, "wr40,1,40,0",       0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request, then dispatch.
    { L_, "wr73728,1,73728,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr220,1,220,0",     0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wr1040,1,1040,0",   0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "iw",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request without being dispatched.
    { L_, "wr73728,1,73728,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Invalidate the channel, then issue 1 request and try dispatching.
    { L_, "wr1040,1,1040,0",   0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "cS",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,-3,0",        0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "dw0",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, enqueue the last request directly, then dispatch.
    { L_, "wr73728,1,73728,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr18,1,18,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "wr40,1,40,0",       0,       1,      2,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 1 request and later cancel it, then enqueue a new request and
     // dispatch.
    { L_, "wr73728,1,73728,0", 0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr3,1,0,-1",        0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "cw",                0,       0,      0,       0, e_NON_VEC,   ""  },
    { L_, "wr20,0,20,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr73728,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_, "wr28,1,28,0",       0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "wb30,1,30,0",       0,       1,      2,       1,  e_OVECTOR,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests, make the second one couldn't extract the specified
     // length of data during dispatch.
   { L_, "wr73728,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
    { L_, "wr73680,1,57344,0", 0,       1,      1,       1, e_NON_VEC,   ""  },
    { L_, "R52379",            0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
   },
   { // Enqueue 2 requests of different write event types, and dispatch.
   { L_, "wr73728,1,73728,0", 0,       0,      0,       0,  e_OVECTOR,   ""  },
     { L_, "wr18,1,18,0,{wr4,1,0,-1; cw; wr11,0,11,0,{wb9,1,9,0};iw}",
                               0,       1,      1,       1, e_NON_VEC,   ""  },
   { L_, "wr2,1,0,-1",        0,       1,      2,       1,  e_OVECTOR,   ""  },
    { L_, "R52379",            0,       1,      2,       1, e_NON_VEC,   ""  },
   { L_, "dw1",               0,       1,      1,       1,  e_OVECTOR,   ""  },
    { L_, "dw1",               0,       0,      0,       0, e_NON_VEC,   ""  },
     { L_,  0,                  0,       0,      0,       0, e_NON_VEC,   ""  }
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
            // client stream socket
            btlso::StreamSocket<btlso::IPv4Address> *cSocket =
                                            factory.allocate(handles[1]);

            btlso::TcpTimerEventManager eventManager(
               btlso::TcpTimerEventManager::e_NO_HINT,
               &testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                char readBuf[MAX_BUF];  // To read from the channel.

                // Buffer is a struct type where declares different pointers
                // pointing to different I/O buffers, e.g., readBuffer,
                // writeBuffer, iovecBuffer, ovecBuffer.  This struct is
                // declared at the beginning of this test driver.
                Buffer buffer = {0, str, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; j++) {
                    const char* iter = SCRIPTS[i][j].d_cmd;
                    if (!iter) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

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

                    int length = gg(&channel, &buffer, 0,
                                    &eventManager, iter);
                    if (length > 0) {
                        if ('W' == *iter) {
                            ret = helpWrite(handles[0],
                                            str, length);
                        }
                        else if ('R' == *iter) {
                            memset(readBuf, '\0', sizeof readBuf);
                            ret = cSocket->read(readBuf, length);
                            if (veryVeryVerbose) {
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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P_(channel.numPendingWriteOperations());
                        P_(eventManager.numEvents());
                        P(eventManager.numEvents());
                        cout << endl;
                    }
                    readBuf[0] = '\0';
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'readRaw()' FUNCTION:
        //   The main concerns about the function are (1) if a read request
        //   can be finished right away when the channel recv-buffer is
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
        //   request (e.g.,a bufferedRead request).
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
        //   By parsing the command script, which is defined by the
        //   SCRIPT language as described above, different operations
        //   will be executed on the channel.  Upon the execution of
        //   each operation, the states of the channel will be compared
        //   against the expected state values which are calculated manually.
        //   Those states include the lengths of "read request queue",
        //   "read event queue", "read request queue", "read event queue",
        //   and the buffer data in the channel.  The "TCPNODELAY" option
        //   for the peer socket is set on because some small packets will
        //   be written to the channel and we require these data be read from
        //   the channel right after this write.
        //
        // Testing:
        //   int readRaw();
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'readRaw' METHOD" << endl
                          << "========================" << endl;

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read-event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write-event
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[][MAX_CMDS] =
        {
   //line cmd       PendingR  ReadE  PendingW  WriteE  Type   d_expData
   //---- ---       --------  -----  --------  ------  ----   ---------
   { // Enqueue 1 request, then dispatch: concern (2).
     { L_, "W4",         0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rr4,0,4,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j123"  },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Enqueue 1 request, then dispatch: concern (2) for diff length.
     { L_, "W4",         0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rr3,0,3,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j12"   },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Enqueue 1 request, then dispatch: concern (2) for diff length.
     { L_, "W4",         0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rr2,0,2,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j1"    },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Enqueue 1 request, then dispatch: concern (2) for diff length.
     { L_, "W4",         0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rr5,0,4,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j123"  },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Enqueue 1 request, then dispatch: concern (2) for diff length.
     { L_, "W4",         0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rr6,0,4,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j123"  },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: concern (1).
     { L_, "W11",        0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_, "rb2,0,2,0",  1,      1,      0,       1,  e_NON_VEC,     ""      },
     { L_, "dr1",        0,      0,      0,       0,  e_NON_VEC,     "j1"    },
     { L_, "rr5,0,5,0",  0,      0,      0,       0,  e_NON_VEC,     "23456" },
     { L_, "dr0",        0,      0,      0,       0,  e_NON_VEC,     ""      },
     { L_,  0,           0,      0,      0,       0,  e_NON_VEC,     ""      }
   },
   { // Invalidate the channel, enter 1 request, then try dispatching: concern
     // (3).
     { L_, "W11",        0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_, "ir",         0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_, "rr6,0,-1,0", 0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_, "dr0",        0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_,  0,           0,      0,      0,       0, e_NON_VEC,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: concern (4) and (5).
     { L_, "W11",        0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_, "rr4,0,4,0",  1,      1,      0,       1, e_NON_VEC,     ""      },
     { L_, "rr3,0,3,0",  2,      1,      0,       1, e_NON_VEC,     ""      },
     { L_, "dr1",        1,      1,      0,       1, e_NON_VEC,     "j123"  },
     { L_, "dr1",        0,      0,      0,       0, e_NON_VEC,     "456"   },
     { L_,  0,           0,      0,      0,       0, e_NON_VEC,     ""      }
   },
   { // Enqueue 2 requests, then dispatch: concern (6).
     { L_, "W11",        0,      0,      0,       0, e_NON_VEC,     ""      },
     { L_, "rr5,0,5,0",  1,      1,      0,       1, e_NON_VEC,     ""      },
     { L_, "rb2,0,2,0",  2,      1,      0,       1, e_NON_VEC,     ""      },
     { L_, "dr1",        1,      1,      0,       1, e_NON_VEC,     "j1234" },
     { L_, "dr1",        0,      0,      0,       0, e_NON_VEC,     "56"    },
     { L_,  0,           0,      0,      0,       0, e_NON_VEC,     ""      }
   },
   { // Concern: after cancel previous reads for a valid channel, if this
     // channel can still work correctly for new read request.
     {L_, "W11",         0,      0,      0,       0, e_NON_VEC,     ""      },
     {L_, "rr4,0,0,-1",  1,      1,      0,       1, e_NON_VEC,     ""      },
     {L_, "cr",          0,      0,      0,       0, e_NON_VEC,     ""      },
     {L_, "rr3,1,3,0",   1,      1,      0,       1, e_NON_VEC,     ""      },
     {L_, "dr1",         0,      0,      0,       0, e_NON_VEC,     "j12"   },
   },
   { // Concern: if this channel can work correctly for new read requests from
     // the user-installed callback function.
     {L_, "W11",          0,      0,      0,      0, e_NON_VEC,     ""      },
     {L_, "rr3,0,3,0,{rr2,1,2,0}",
                         1,      1,      0,       1, e_NON_VEC,     ""      },
     {L_, "dr1",         1,      1,      0,       1, e_NON_VEC,     "j12"   },
     {L_, "W4",         1,      1,      0,        1, e_NON_VEC,     ""      },
     {L_, "rr1,1,1,0,{rr5,0,5,0; rr3,1,3,0}",
                         2,      1,      0,       1, e_NON_VEC,     ""      },
     {L_, "dr1",         1,      1,      0,       1, e_NON_VEC,     "34"    },
     {L_, "dr1",         2,      1,      0,       1, e_NON_VEC,     "5"     },
     {L_, "dr1",         1,      1,      0,       1, e_NON_VEC,     "67890" },
     {L_, "dr1",         0,      0,      0,       0, e_NON_VEC,     "j12"   },
     {L_,  0,            0,      0,      0,       0, e_NON_VEC,     ""      }
   },
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

            btlso::TcpTimerEventManager eventManager(&testAllocator);

            {
                // We should guarantee that the 'channel's destructor is
                // invoked before the corresponding 'streamSocket' destructor.
                // We insure the required order by creating the 'channel'
                // inside a block while the corresponding 'streamSocket' object
                // is created outside the block as above.

                btlsos::TcpCbChannel channel(sSocket, &eventManager,
                                            &testAllocator);

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[i][j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[i][j].d_line;

                    char readBuf[MAX_BUF];
                    memset(readBuf, '\0', sizeof readBuf);
                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.
                    Buffer buffer = {readBuf, 0, 0, 0, 0};
                    int length = gg(&channel, &buffer, &eventManager, 0,
                                                                     command);

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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(eventManager.numEvents());
                        P_(buffer.d_readBuf); P(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }

                    if (SCRIPTS[i][j].d_operationType == e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType ==e_NON_VEC) {
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
            btlso::SocketImpUtil::close(handles[0]);
            btlso::SocketImpUtil::close(handles[1]);
        }
      } break;
      case 5: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
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
        //   btlsos::TcpCbChannel(..., rManager, wManager, ...);
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
              rManager0(&testAllocator), wManager0(&testAllocator),
              rManager1(&testAllocator), wManager1(&testAllocator),
              rManager2(&testAllocator), wManager2(&testAllocator),
              rManager3(&testAllocator), wManager3(&testAllocator);

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
                int         d_line;  // line number
                const char *d_cmd;   // command to be executed on a channel

                int         d_numPendingRead;// expected number of pendingRead
                                             // operations on a channel
                int         d_numReadEvent;  // expected number of callbacks
                                             // registered with a read-event
                                             // manager
                int         d_numPendingWrite;  // expected number of pending
                                             // write operations on a channel
                int         d_numWriteEvent; // expected number of callbacks
                                             // registered with a write-event
                                             // manager
                int         d_operationType; // if it's a read with vector

                const char *d_expData;       // expected data read from the
                                             // channel
            } SCRIPTS[MAX_CMDS] =
   //line cmd             PendingR  ReadE  PendingW  WriteE  Type  d_expData
   //---- ---             --------  -----  --------  ------  ----  ---------
     // First test channel read and write buffer (entry 1); then test
     // cancelALL(); next test invalidate().  Note that it's important to make
     // sure read and write event will not interfere with each other.
   #if defined(BSLS_PLATFORM_OS_WINDOWS)     // windows test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w13720,0,0,-1",    2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w25,1,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },

    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "ia",               1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };
   #elif defined(BSLS_PLATFORM_OS_AIX)       // ibm test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w1020,0,1020,0",   2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w50,0,0,-1",       2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w25,1,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },

    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "ia",               1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };
   #elif BSLS_PLATFORM_OS_LINUX
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w16000,0,16000,0", 2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w30000,0,0,-1",    2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w25,1,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },

    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "ia",               1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };
   #else                                      // sun test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w73720,0,73720,0", 2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w33720,0,0,-1",    2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w25,1,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },

    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "ia",               1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "w5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     1,      1,        1, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };

   #endif

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

                    btlsos::TcpCbChannel channel(sSocket,
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

                    char readBuf[MAX_BUF], sendBuf[MAX_BUF];
                    memset(readBuf, '\0', sizeof readBuf);
                    memset(sendBuf, '\0', sizeof sendBuf);
                    Buffer buffer = {readBuf, sendBuf, 0, 0, 0};

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
                        }
                        if (channel.writeEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                        }
                        if (veryVerbose) {
                            P_(LINE);
                            P_(channel.numPendingReadOperations());
                            P(channel.readEventManager()->numEvents());
                            P_(channel.numPendingWriteOperations());
                            P(channel.writeEventManager()->numEvents());
                            P_(buffer.d_readBuf); P(SCRIPTS[j].d_expData);
                            cout << endl;
                        }
                    }
                }
                factory.deallocate(sSocket);
                btlso::SocketImpUtil::close(OBJECTS[i].d_handle[0]);
                btlso::SocketImpUtil::close(OBJECTS[i].d_handle[1]);
            }
        }
        if (verbose) cout << "\nTesting setReadEventManager()."
                          << "\n=============================="
                          << "\nTesting setWriteEventManager()."
                          << "\n===============================" << endl;
        {
            btlso::SocketHandle::Handle socket[2];
            btlso::TcpTimerEventManager rManager0(&testAllocator),
                                       rManager1(&testAllocator),
                                       rManager2(&testAllocator),
                                       rManager3(&testAllocator);
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
                int         d_line;  // line number
                const char *d_cmd;   // command to be executed on a channel

                int         d_numPendingRead;// expected number of pendingRead
                                             // operations on a channel
                int         d_numReadEvent;  // expected number of callbacks
                                             // registered with a read-event
                                             // manager
                int         d_numPendingWrite;  // expected number of pending
                                             // write operations on a channel
                int         d_numWriteEvent; // expected number of callbacks
                                             // registered with a write-event
                                             // manager
                int         d_operationType; // if it's a read with vector

                const char *d_expData;       // expected data read from the
                                             // channel
            } SCRIPTS[][MAX_CMDS] =
 {
     // First test channel read and write buffer (entry 1); then test
     // cancelALL(); next test invalidate().  Note that it's important to make
     // sure read and write event will not interfere with each other.
    //line cmd             PendingR  ReadE  PendingW  WriteE  Type  d_expData
    //---- ---             --------  -----  --------  ------  ----  ---------
    #if defined(BSLS_PLATFORM_OS_WINDOWS)     // windows test data
    {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w13720,0,13720,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w720,0,0,-1",      1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },

     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w720,0,0,-1",      2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     1,      3,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,5,0",         1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "R22729",           1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,5,0",         1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,0,-1",        1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w13720,0,13720,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w720,0,720,0",     2,     2,      2,        2, e_NON_VEC,   ""    },

    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "W53",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r51,1,51,0",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "R22729",           1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "dr2",              0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr0",              0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },

   #elif defined(BSLS_PLATFORM_OS_AIX)       // ibm test data

   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w1020,0,1020,0",   2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w50,0,50,0",       2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R500",             0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },

    { L_, "w1020,0,0,-1",     1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w1020,0,0,-1",     2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w120,0,0,-1",      2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "w70,0,0,-1",       2,     1,      3,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "W10",              2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w1025,1,1025,0",   1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R500",             1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w1025,0,0,-1",     2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w1025,1,1025,0",   1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R500",             1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },

    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,0,-1",        1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w1020,0,1020,0",   2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w100,0,100,0",     2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w70,0,70,0",       2,     2,      2,        2, e_NON_VEC,   ""    },

     // Verify that the read is not affected by cancelWrite() by dispatching a
     // write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R500",             0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "W53",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r55,1,55,0",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "dr2",              0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr0",              0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   #elif BSLS_PLATFORM_OS_LINUX
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w16000,0,16000,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w30000,0,30000,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R32729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w30000,0,0,-1",    1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },

     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w720,0,0,-1",      2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     1,      3,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w30000,1,30000,0", 1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R32000",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w30000,0,0,-1",    2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w30000,1,30000,0", 1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R35729",           1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R35729",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,0,-1",        1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w6000,0,6000,0",   2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w720,0,720,0",     2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "R50000",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "W6000",            0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r6000,1,6000,0",   1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "dr2",              0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr0",              0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   #else                                      // sun test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w73720,0,73720,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w33720,0,33720,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R32729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w73720,0,0,-1",    1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "ca",               0,     0,      0,        0, e_NON_VEC,   ""    },

     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w720,0,0,-1",      2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     1,      3,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
     { L_, "W8",              2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,5,0",         1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,8,0",         1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w73720,0,0,-1",    2,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "w20,0,0,-1",       2,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "cw",               2,     1,      0,        0, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "W8",               2,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "w5,1,5,0",         1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R35729",           1,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "R35729",           1,     1,      1,        1, e_NON_VEC,   ""    },

    { L_, "dw1",              1,     1,      0,        0, e_NON_VEC,   ""    },
    { L_, "cr",               0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   },
   {
    { L_, "r8,0,0,-1",        1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w63720,0,63720,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w13720,0,13720,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w720,0,720,0",     2,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
            // Verify that the read is not affected by cancelWrite() by
            // dispatching a write request.
    { L_, "R52729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "W55",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r55,1,55,0",       1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "dr2",              0,     0,      0,        0, e_NON_VEC,   ""    },
    { L_, "dr0",              0,     0,      0,        0, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
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

            ret = btlso::SocketOptUtil::setOption(
                                   socket[1],
                                   btlso::SocketOptUtil::k_TCPLEVEL,
                                   btlso::SocketOptUtil::k_TCPNODELAY, 1);
            ASSERT(0 == ret);

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
                btlsos::TcpCbChannel channel(sSocket,
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

                    char readBuf[MAX_BUF], sendBuf[MAX_BUF];
                    memset(readBuf, '\0', sizeof readBuf);
                    memset(sendBuf, '\0', sizeof sendBuf);
                    Buffer buffer = {readBuf, sendBuf, 0, 0, 0};

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
                                btlso::Flag::BlockingMode bm;
                                LOOP_ASSERT(LINE, 0 == sSocket->
                                        blockingMode(&bm));
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
                                        cout << "help read data: " << ret
                                            << endl;
                                    }
                                }
                                cSocket->setBlockingMode(bm);
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
                        }
                        if (channel.writeEventManager()) {
                            LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent
                                  == channel.writeEventManager()->numEvents());
                        }
                        if (veryVerbose) {
                            P_(LINE);
                            P_(channel.numPendingReadOperations());
                            P(channel.readEventManager()->numEvents());
                            P_(channel.numPendingWriteOperations());
                            P(channel.writeEventManager()->numEvents());
                            P_(buffer.d_readBuf); P(SCRIPTS[i][j].d_expData);
                            cout << endl;
                        }
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(socket[0]);
            btlso::SocketImpUtil::close(socket[1]);
        }
#endif
      } break;
      case 4: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_CPU_X86_64)
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
        btlso::TcpTimerEventManager rManager0(&testAllocator),
                                   rManager1(&testAllocator),
                                   rManager2(&testAllocator),
                                   rManager3(&testAllocator);

        struct {
            int                               d_line;
            btlso::SocketHandle::Handle       *d_handle;
            btlso::TcpTimerEventManager       *d_rManager;
            btlso::TcpTimerEventManager       *d_wManager;
            btlso::TcpTimerEventManager *d_exprManager;
            btlso::TcpTimerEventManager *d_expwManager;

        } OBJECTS[] =
        { //line   d_handle  rManager   wManager    exprManager   expwManager
          //----------------------------------------------------------------
           {L_,    sock0,       0,          0,      &rManager0,   &rManager0 },
           {L_,    sock2,       0,      &rManager1, &rManager0,   &rManager1 },
        };

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read-event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write-event
                                            // manager
            int         d_isInvalidReadFlag;
            int         d_isInvalidWriteFlag;
        } SCRIPTS[][MAX_CMDS] =
 {
   // line cmd PendingR ReadE PendingW WriteE invalidR invalidW
   //----------------------------------------------------------------------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)     // windows test data
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        1,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        1,      0,       0  },
     { L_, "w28720,0,28720,0", 2,     1,      0,        1,      0,       0  },
     { L_, "w13720,0,13720,0", 2,     2,      1,        2,      0,       0  },
     { L_, "w220,0,0,-1",      2,     2,      2,        2,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     1,      2,        1,      0,       0  },
     { L_, "R22729",           0,     1,      2,        1,      0,       0  },
     { L_, "R22729",           0,     1,      2,        1,      0,       0  },
     { L_, "dw1",              0,     1,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     2,      1,        2,      0,       0  },

     { L_, "cw",               1,     1,      0,        1,      0,       0  },
     { L_, "w28720,0,28720,0", 1,     1,      0,        1,      0,       0  },
     { L_, "w1720,0,0,-1",     1,     2,      1,        2,      0,       0  },
     { L_, "ir",               1,     2,      1,        2,      1,       0  },
     { L_, "r3,1,0,-1",        1,     2,      1,        2,      1,       0  },
     { L_, "w5,1,0,-1",        1,     2,      2,        2,      1,       0  },

     { L_, "iw",               1,     2,      2,        2,      1,       1  },
     { L_, "r5,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_, "w9,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        0,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        0,      0,       0  },
     { L_, "w28720,0,28720,0", 2,     1,      0,        0,      0,       0  },
     { L_, "w13720,0,13720,0", 2,     1,      1,        1,      0,       0  },
     { L_, "w220,0,0,-1",      2,     1,      2,        1,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     0,      2,        1,      0,       0  },
     { L_, "R22729",           0,     0,      2,        1,      0,       0  },
     { L_, "R12729",           0,     0,      2,        1,      0,       0  },
     { L_, "dw1",              0,     0,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     1,      1,        1,      0,       0  },

     { L_, "cw",               1,     1,      0,        0,      0,       0  },
     { L_, "w28720,0,28720,0", 1,     1,      0,        0,      0,       0  },
     { L_, "w220,0,0,-1",      1,     1,      1,        1,      0,       0  },
     { L_, "ir",               1,     1,      1,        1,      1,       0  },
     { L_, "r3,1,0,-1",        1,     1,      1,        1,      1,       0  },
     { L_, "w5,1,0,-1",        1,     1,      2,        1,      1,       0  },

     { L_, "iw",               1,     1,      2,        1,      1,       1  },
     { L_, "r5,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_, "w9,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   #elif defined(BSLS_PLATFORM_OS_AIX)       // ibm test data
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        1,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        1,      0,       0  },
     { L_, "w1020,0,1020,0",   2,     1,      0,        1,      0,       0  },
     { L_, "w50,0,50,0",       2,     2,      1,        2,      0,       0  },
     { L_, "w220,0,0,-1",      2,     2,      2,        2,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     1,      2,        1,      0,       0  },
     { L_, "R500",             0,     1,      2,        1,      0,       0  },
     { L_, "dw1",              0,     1,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     2,      1,        2,      0,       0  },

     { L_, "cw",               1,     1,      0,        1,      0,       0  },
     { L_, "w1020,0,0,-1",     1,     2,      1,        2,      0,       0  },
     { L_, "ir",               1,     2,      1,        2,      1,       0  },
     { L_, "r3,1,0,-1",        1,     2,      1,        2,      1,       0  },
     { L_, "w5,1,0,-1",        1,     2,      2,        2,      1,       0  },

     { L_, "iw",               1,     2,      2,        2,      1,       1  },
     { L_, "r5,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_, "w9,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        0,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        0,      0,       0  },
     { L_, "w1020,0,1020,0",   2,     1,      0,        0,      0,       0  },
     { L_, "w50,0,50,0",       2,     1,      1,        1,      0,       0  },
     { L_, "w220,0,0,-1",      2,     1,      2,        1,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     0,      2,        1,      0,       0  },
     { L_, "R500",             0,     0,      2,        1,      0,       0  },
     { L_, "dw1",              0,     0,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     1,      1,        1,      0,       0  },

     { L_, "cw",               1,     1,      0,        0,      0,       0  },
     { L_, "w1020,0,0,-1",     1,     1,      1,        1,      0,       0  },
     { L_, "ir",               1,     1,      1,        1,      1,       0  },
     { L_, "r3,1,0,-1",        1,     1,      1,        1,      1,       0  },
     { L_, "w5,1,0,-1",        1,     1,      2,        1,      1,       0  },

     { L_, "iw",               1,     1,      2,        1,      1,       1  },
     { L_, "r5,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_, "w9,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },

   #elif BSLS_PLATFORM_OS_LINUX
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        1,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        1,      0,       0  },
     { L_, "w16000,0,16000,0", 2,     1,      0,        1,      0,       0  },
     { L_, "w30000,0,30000,0", 2,     2,      1,        2,      0,       0  },
     { L_, "w220,0,0,-1",      2,     2,      2,        2,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     1,      2,        1,      0,       0  },
     { L_, "R32729",           0,     1,      2,        1,      0,       0  },
     { L_, "R22729",           0,     1,      2,        1,      0,       0  },
     { L_, "dw1",              0,     1,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     2,      1,        2,      0,       0  },

     { L_, "cw",               1,     1,      0,        1,      0,       0  },
     { L_, "w16000,0,0,-1",    1,     2,      1,        2,      0,       0  },
     { L_, "ir",               1,     2,      1,        2,      1,       0  },
     { L_, "r3,1,0,-1",        1,     2,      1,        2,      1,       0  },
     { L_, "w5,1,0,-1",        1,     2,      2,        2,      1,       0  },

     { L_, "iw",               1,     2,      2,        2,      1,       1  },
     { L_, "r5,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_, "w9,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        0,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        0,      0,       0  },
     { L_, "w16000,0,16000,0", 2,     1,      0,        0,      0,       0  },
     { L_, "w30000,0,30000,0", 2,     1,      1,        1,      0,       0  },
     { L_, "w220,0,0,-1",      2,     1,      2,        1,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     0,      2,        1,      0,       0  },
     { L_, "R32729",           0,     0,      2,        1,      0,       0  },
     { L_, "R22729",           0,     0,      2,        1,      0,       0  },
     { L_, "dw1",              0,     0,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     1,      1,        1,      0,       0  },

     { L_, "cw",               1,     1,      0,        0,      0,       0  },
     { L_, "w16000,0,0,-1",    1,     1,      1,        1,      0,       0  },
     { L_, "ir",               1,     1,      1,        1,      1,       0  },
     { L_, "r3,1,0,-1",        1,     1,      1,        1,      1,       0  },
     { L_, "w5,1,0,-1",        1,     1,      2,        1,      1,       0  },

     { L_, "iw",               1,     1,      2,        1,      1,       1  },
     { L_, "r5,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_, "w9,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   #else                                      // sun test data
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        1,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        1,      0,       0  },
     { L_, "w73720,0,73720,0", 2,     1,      0,        1,      0,       0  },
     { L_, "w33720,0,33720,0", 2,     2,      1,        2,      0,       0  },
     { L_, "w220,0,0,-1",      2,     2,      2,        2,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     1,      2,        1,      0,       0  },
     { L_, "R32729",           0,     1,      2,        1,      0,       0  },
     { L_, "R22729",           0,     1,      2,        1,      0,       0  },
     { L_, "dw1",              0,     1,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     2,      1,        2,      0,       0  },

     { L_, "cw",               1,     1,      0,        1,      0,       0  },
     { L_, "w73720,0,0,-1",    1,     2,      1,        2,      0,       0  },
     { L_, "ir",               1,     2,      1,        2,      1,       0  },
     { L_, "r3,1,0,-1",        1,     2,      1,        2,      1,       0  },
     { L_, "w5,1,0,-1",        1,     2,      2,        2,      1,       0  },

     { L_, "iw",               1,     2,      2,        2,      1,       1  },
     { L_, "r5,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_, "w9,1,0,-1",        1,     2,      2,        2,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
   },
   {
     { L_, "rb1,0,0,-1",       1,     1,      0,        0,      0,       0  },
     { L_, "rb4,1,0,-1",       2,     1,      0,        0,      0,       0  },
     { L_, "w73720,0,73720,0", 2,     1,      0,        0,      0,       0  },
     { L_, "w33720,0,33720,0", 2,     1,      1,        1,      0,       0  },
     { L_, "w220,0,0,-1",      2,     1,      2,        1,      0,       0  },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
     { L_, "cr",               0,     0,      2,        1,      0,       0  },
     { L_, "R32729",           0,     0,      2,        1,      0,       0  },
     { L_, "R22729",           0,     0,      2,        1,      0,       0  },
     { L_, "dw1",              0,     0,      1,        1,      0,       0  },
     { L_, "r15,1,0,-1",       1,     1,      1,        1,      0,       0  },

     { L_, "cw",               1,     1,      0,        0,      0,       0  },
     { L_, "w73720,0,0,-1",    1,     1,      1,        1,      0,       0  },
     { L_, "ir",               1,     1,      1,        1,      1,       0  },
     { L_, "r3,1,0,-1",        1,     1,      1,        1,      1,       0  },
     { L_, "w5,1,0,-1",        1,     1,      2,        1,      1,       0  },

     { L_, "iw",               1,     1,      2,        1,      1,       1  },
     { L_, "r5,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_, "w9,1,0,-1",        1,     1,      2,        1,      1,       1  },
     { L_,  0,                 0,     0,      0,        0,      0,       0  }
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

                btlsos::TcpCbChannel channel(sSocket,
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

                char readBuf[MAX_BUF], sendBuf[MAX_BUF];
                memset(readBuf, '\0', sizeof readBuf);
                memset(sendBuf, '\0', sizeof sendBuf);
                Buffer buffer = {readBuf, sendBuf, 0, 0, 0};

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
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(channel.readEventManager()->numEvents());
                        P_(channel.numPendingWriteOperations());
                        P(channel.writeEventManager()->numEvents());
                        cout << endl;
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(OBJECTS[i].d_handle[0]);
            btlso::SocketImpUtil::close(OBJECTS[i].d_handle[1]);
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
#ifndef BSLS_PLATFORM_OS_SOLARIS
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The main concern is to make sure: 1.  Every constructor can
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
        //   btlsos::TcpCbChannel(eventManager);
        //   ~btlsos::TcpCbChannel();
        //   void cancelRead();
        //   void cancelWrite();
        //   void invalidateRead();
        //   void invalidateWrite();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        btlso::SocketHandle::Handle sock0[2], sock1[2], sock2[2], sock3[2];
        btlso::TcpTimerEventManager rManager0(&testAllocator),
                                   rManager1(&testAllocator),
                                   rManager2(&testAllocator),
                                   rManager3(&testAllocator);

        struct {
            int                               d_line;
            btlso::SocketHandle::Handle       *d_handle;
            btlso::TcpTimerEventManager       *d_rManager;
            btlso::TcpTimerEventManager       *d_wManager;
            const btlso::TcpTimerEventManager *d_exprManager;
            const btlso::TcpTimerEventManager *d_expwManager;

        } OBJECTS[] =
        { //line   d_handle   rManager  wManager  exprManager   expwManager
          //----------------------------------------------------------------
           {L_,     sock0,    &rManager0,   0,     &rManager0,   &rManager0 },
           {L_,     sock1,    &rManager1,   0,     &rManager1,   &rManager1 },
           {L_,     sock2,    &rManager2,   0,     &rManager2,   &rManager2 },
           {L_,     sock3,    &rManager3,   0,     &rManager3,   &rManager3 },
        };

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read-event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write-event
                                            // manager
            int         d_operationType;    // if it's a read with vector

            const char *d_expData;          // expected data read from the
                                            // channel
        } SCRIPTS[MAX_CMDS] =
        // First test channel read and write buffer (entry 1); then test
        // cancelRead() while some write requests on the queue; next test
        // cancelWrite() while some read requests on the queue; lastly test
        // invalidateRead() and invalidateWrite().  Note that it's important to
        // make sure read and write event will not interfere with each other,
        // e.g., cancelRead() shouldn't affect any write request.
   // line      cmd       PendingR  ReadE  PendingW  WriteE  Type  d_expData
   //-----      ---       --------  -----  --------  ------  ----  ---------
   #if defined(BSLS_PLATFORM_OS_WINDOWS)   // windows test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w13720,0,13720,0", 2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R12729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w28720,0,28720,0", 1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "ir",               1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },

    { L_, "iw",               1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };

   #elif defined(BSLS_PLATFORM_OS_AIX)     // ibm test data

   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w1020,0,1020,0",   2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w50,0,50,0",       2,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      2,     2,      2,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "R500",             0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      1,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      1,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w1020,0,0,-1",     1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "ir",               1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },

    { L_, "iw",               1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };
   #elif BSLS_PLATFORM_OS_LINUX
   {
   { L_, "rb1,0,0,-1",       1,     1,      0,        1,  e_NON_VEC,   ""    },
   { L_, "rb4,1,0,-1",       2,     1,      0,        1,  e_NON_VEC,   ""    },
   { L_, "rb8,1,0,-1",       3,     1,      0,        1,  e_NON_VEC,   ""    },
   { L_, "w16383,0,16383,0", 3,     1,      0,        1,  e_NON_VEC,   ""    },
   { L_, "w30000,0,30000,0", 3,     2,      1,        2,  e_NON_VEC,   ""    },
   { L_, "w220,0,0,-1",      3,     2,      2,        2,  e_NON_VEC,   ""    },
   { L_, "w120,0,0,-1",      3,     2,      3,        2,  e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
   { L_, "cr",               0,     1,      3,        1,  e_NON_VEC,   ""    },
   { L_, "R32729",           0,     1,      3,        1,  e_NON_VEC,   ""    },
   { L_, "R22729",           0,     1,      3,        1,  e_NON_VEC,   ""    },
   { L_, "dw1",              0,     1,      2,        1,  e_NON_VEC,   ""    },
   { L_, "r15,1,0,-1",       1,     2,      2,        2,  e_NON_VEC,   ""    },

   { L_, "cw",               1,     1,      0,        1,  e_NON_VEC,   ""    },
   { L_, "w30000,0,0,-1",    1,     2,      1,        2,  e_NON_VEC,   ""    },
   { L_, "ir",               1,     2,      1,        2,  e_NON_VEC,   ""    },
   { L_, "r3,1,0,-1",        1,     2,      1,        2,  e_NON_VEC,   ""    },
   { L_, "w5,1,0,-1",        1,     2,      2,        2,  e_NON_VEC,   ""    },

   { L_, "iw",               1,     2,      2,        2,  e_NON_VEC,   ""    },
   { L_, "r5,1,0,-1",        1,     2,      2,        2,  e_NON_VEC,   ""    },
   { L_, "w9,1,0,-1",        1,     2,      2,        2,  e_NON_VEC,   ""    },
    { L_,  0,                 0,     0,      0,        0,  e_NON_VEC,   ""    }
   };
   #else                                    // sun test data
   {
    { L_, "rb1,0,0,-1",       1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb4,1,0,-1",       2,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "rb8,1,0,-1",       3,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w73720,0,73720,0", 3,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w33720,0,33720,0", 3,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w220,0,0,-1",      3,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "w120,0,0,-1",      3,     2,      3,        2, e_NON_VEC,   ""    },
            // Make sure the write is not affected by cancelRead() by
            // dispatching a write request.
    { L_, "cr",               0,     1,      3,        1, e_NON_VEC,   ""    },
    { L_, "R32729",           0,     1,      3,        1, e_NON_VEC,   ""    },
    { L_, "R22729",           0,     1,      3,        1, e_NON_VEC,   ""    },
    { L_, "dw1",              0,     1,      2,        1, e_NON_VEC,   ""    },
    { L_, "r15,1,0,-1",       1,     2,      2,        2, e_NON_VEC,   ""    },

    { L_, "cw",               1,     1,      0,        1, e_NON_VEC,   ""    },
    { L_, "w73720,0,0,-1",    1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "ir",               1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "r3,1,0,-1",        1,     2,      1,        2, e_NON_VEC,   ""    },
    { L_, "w5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },

    { L_, "iw",               1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "r5,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
    { L_, "w9,1,0,-1",        1,     2,      2,        2, e_NON_VEC,   ""    },
     { L_,  0,                 0,     0,      0,        0, e_NON_VEC,   ""    }
   };
   #endif

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

                btlsos::TcpCbChannel channel(sSocket,
                                            OBJECTS[i].d_rManager,
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
                LOOP_ASSERT(i, 0 == channel.readEventManager()->numEvents());
                LOOP_ASSERT(i, 0 == channel.writeEventManager()->numEvents());

                char readBuf[MAX_BUF], sendBuf[MAX_BUF];
                memset(readBuf, '\0', sizeof readBuf);
                memset(sendBuf, '\0', sizeof sendBuf);
                Buffer buffer = {readBuf, sendBuf, 0, 0, 0};

                for (int j = 0; j < MAX_CMDS; ++j) {
                    const char *command = SCRIPTS[j].d_cmd;
                    if (!command) {
                        break;
                    }
                    const int LINE = SCRIPTS[j].d_line;

                    // Buffer is a struct type where declares different
                    // pointers pointing to different I/O buffers, e.g.,
                    // readBuffer, writeBuffer, iovecBuffer, ovecBuffer.  This
                    // struct is declared at the beginning of this test driver.

                    int length = gg(&channel, &buffer, OBJECTS[i].d_rManager,
                                              OBJECTS[i].d_rManager, command);

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
                    LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP_ASSERT(LINE, SCRIPTS[j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(channel.readEventManager()->numEvents());
                        P_(channel.numPendingWriteOperations());
                        P(channel.writeEventManager()->numEvents());
                        cout << endl;
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(OBJECTS[i].d_handle[0]);
            btlso::SocketImpUtil::close(OBJECTS[i].d_handle[1]);
        }
#endif
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST:
        //   We need to exercise basic functionality.  In particular, make sure
        //   that functions for I/O requests such as 'read()', 'readv',
        //   'readRaw', 'readvRaw', 'bufferedRead', 'bufferedReadRaw', 'write',
        //   'writeRaw' etc. will work fine.
        //
        // Plan:
        //   Create two channels, one has separate event managers for read and
        //   write requests, while another one only has one event manager for
        //   both read and write requests.  For each channel, issue a list of
        //   I/O requests that will be enqueued and later executed upon
        //   dispatch, verify the state value of each channel after each
        //   operation.
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------

        if (verbose) cout << "\nBreathing testing."
                          << "\n==================" << endl;

        btlso::SocketHandle::Handle sock0[2];
        btlso::TcpTimerEventManager rManager0(&testAllocator),
                                   rManager1(&testAllocator),
                                   rManager2(&testAllocator),
                                   rManager3(&testAllocator);

        struct {
            int                               d_line;
            btlso::SocketHandle::Handle       *d_handle;
            btlso::TcpTimerEventManager       *d_rManager;
            btlso::TcpTimerEventManager       *d_wManager;
        } CHANNELS[] =
        { //line  d_handle   rManager    wManager
          //----  --------   --------    --------
           {L_,   sock0,     &rManager0, &rManager1 },
        };

        enum { e_NON_VEC = 0, e_OVECTOR, e_IOVECTOR };

        struct {
            int         d_line;    // line number
            const char *d_cmd;     // the command to be executed on a channel

            int         d_numPendingRead;   // expected number of pending read
                                            // operations on a channel
            int         d_numReadEvent;     // expected number of callbacks
                                            // registered with a read-event
                                            // manager
            int         d_numPendingWrite;  // expected number of pending write
                                            // operations on a channel
            int         d_numWriteEvent;    // expected number of callbacks
                                            // registered with a write-event
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
 // ----^
   // line cmd PendingR ReadE PendingW WriteE Type d_expData
   //-----      ---       -------- ----- -------- ------ ---- ---------
 {
     {
    { L_, "rbr3,0,3,0",      1,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "r4,1,4,0",        2,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rbr3,0,3,0",      3,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "W10",             3,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "dr1",             0,    0,     0,      0,  e_NON_VEC,   "890"     },

#if 0
    { L_, "r4,1,4,0",        1,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rb5,1,5,0",       2,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rr7,1,7,0",       3,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rvr2,1,3,0",      4,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rb8,1,8,0",       5,    1,     0,      0,  e_NON_VEC,   ""        },

    { L_, "rv4,1,13,0",      6,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "rr5,1,5,0",       7,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "W5",              7,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "dr1",             6,    1,     0,      0,  e_NON_VEC,   "1234"    },
    { L_, "dr1",             6,    1,     0,      0,  e_NON_VEC,   ""        },


    { L_, "W8",              6,    1,     0,      0,  e_NON_VEC,   ""        },
     // the data will be mixed b/w "rb5,1,5,0" (should be "51234") and
     // "rr7,1,7,0" if the same readBuf is used, the first "5678" is read for
     // "rr7,1,7,0"..
    { L_, "dr1",             5,    1,     0,      0,  e_NON_VEC,   "56784"   },
    { L_, "W7",              5,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "dr1",             4,    1,     0,      0,  e_NON_VEC,   "5678123" },
    { L_, "dr1",             3,    1,     0,      0,  e_IOVECTOR,  "456"     },

#if !defined(BSLS_PLATFORM_OS_SOLARIS) || BSLS_PLATFORM_OS_VER_MAJOR < 10
    { L_, "W20",             3,    1,     0,      0,  e_NON_VEC,   ""        },
    { L_, "dr1",             1,    1,     0,      0,  e_NON_VEC,   "71234567"},
#endif

//     { L_, "W5",              1,    1,     0,      0,  e_NON_VEC,   ""        },
//     { L_, "dr1",             0,    0,     0,      0,  e_NON_VEC,   "12345"   },

#endif

    { L_,  0,                0,    0,     0,      0,  e_NON_VEC,   ""        }
   },
 };
 // ----v The normal alignment should be as the following statement:

        // the number of channel objects to be created for test
        const int NUM_CHANNELS = sizeof CHANNELS / sizeof *CHANNELS;

        for (int i = 0; i < NUM_CHANNELS; ++i) {
            if (veryVerbose) cout << "\tCreating channel " << i << ".\n";

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

                btlsos::TcpCbChannel channel(sSocket,
                                            CHANNELS[i].d_rManager,
                                            CHANNELS[i].d_wManager,
                                            &testAllocator);
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

                char buf[MAX_BUF] = "\0", helpReadBuf[MAX_BUF] = "\0",
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
                    if (veryVeryVerbose)
                        cout << "\t\tExecuting script line " << LINE << ".\n";

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
                            memset(helpReadBuf, '\0', sizeof helpReadBuf);
                            ret = cSocket->read(helpReadBuf, length);
                        }
                    }

//                     bslmt::ThreadUtil::microSleep(0, 1);
                    LOOP_ASSERT(LINE, 0 <= length);
                    LOOP3_ASSERT(LINE,
                                 SCRIPTS[i][j].d_numPendingRead,
                                 channel.numPendingReadOperations(),
                                 SCRIPTS[i][j].d_numPendingRead ==
                                          channel.numPendingReadOperations());

                    LOOP3_ASSERT(LINE,
                                SCRIPTS[i][j].d_numPendingWrite,
                                channel.numPendingWriteOperations(),
                                SCRIPTS[i][j].d_numPendingWrite ==
                                          channel.numPendingWriteOperations());

                    if (channel.readEventManager()) {
                        LOOP3_ASSERT(LINE,
                                     SCRIPTS[i][j].d_numReadEvent,
                                     channel.readEventManager()->numEvents(),
                                     SCRIPTS[i][j].d_numReadEvent ==
                                     channel.readEventManager()->numEvents());
                    }
                    if (channel.writeEventManager()) {
                        LOOP_ASSERT(LINE, SCRIPTS[i][j].d_numWriteEvent ==
                                     channel.writeEventManager()->numEvents());
                    }
                    if (SCRIPTS[i][j].d_operationType == e_IOVECTOR) {
                        helpAssertVecData(i, j, SCRIPTS[i][j].d_operationType,
                                          buffer.d_readIovecBuf,
                                          SCRIPTS[i][j].d_expData);
                    }
                    else if (SCRIPTS[i][j].d_operationType == e_NON_VEC) {
                        LOOP_ASSERT(LINE, 0 == strncmp(buffer.d_readBuf,
                                             SCRIPTS[i][j].d_expData,
                                             strlen(SCRIPTS[i][j].d_expData)));
                    }
                    else {
                        LOOP_ASSERT(LINE, "Wrong operation type." && 0);
                    }

                    if (veryVerbose) {
                        P_(LINE);
                        P_(channel.numPendingReadOperations());
                        P(channel.readEventManager()->numEvents());
                        P_(channel.numPendingWriteOperations());
                        P(channel.writeEventManager()->numEvents());
                        P_(buffer.d_readBuf); P_(SCRIPTS[i][j].d_expData);
                        cout << endl;
                    }
                }
            }
            factory.deallocate(sSocket);
            factory.deallocate(cSocket);
            btlso::SocketImpUtil::close(CHANNELS[i].d_handle[0]);
            btlso::SocketImpUtil::close(CHANNELS[i].d_handle[1]);
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
