// btlso_socketoptutil.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_socketoptutil.h>

#include <btlso_socketimputil.h>
#include <btlso_socketoptions.h>
#include <btlso_lingeroptions.h>
#include <btlso_ipv4address.h>

#ifdef BSLS_PLATFORM_OS_UNIX
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_c_sys_time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#else
#include <windows.h>
#endif

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//
// The component under test consists of a set of static member functions and
// enumeration types that are used to provide platform independence for
// socket-based programs.  The basic plan for testing is to ensure that
// all methods in the component pass correct parameters through to the
// system level calls.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [  ] template <T> int setOption(handle, level, option, value);
// [  ] template <T> int setOption(handle, level, option, value, *error);
// [  ] template <T> int getOption(T *result, handle, level, int option);
// [  ] template <T> int getOption(T *result, handle, level, option, *error);
// [ 5] int setSocketOptions(SocketHandle::Handle handle, options);
// ----------------------------------------------------------------------------
// [ 6] USAGE example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t"                             \
                    << #J << ": " << J << "\t" << #K << ": " << K << "\t"  \
                    << #L << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define SO_LEVEL  btlso::SocketOptUtil::k_SOCKETLEVEL
#define TCP_LEVEL btlso::SocketOptUtil::k_TCPLEVEL

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btlso::SocketOptUtil             SockOptUtil;
typedef btlso::SocketOptUtil::LingerData LingerData;
typedef btlso::SocketHandle::Handle      SocketHandle;
typedef btlso::SocketOptions             SocketOptions;
typedef btlso::LingerOptions             LingerOptions;

const unsigned short DUMMY_PORT = 5000;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure a 'btlso::SocketOptions' according to a custom language.
// A tilde ('~') indicates that the logical (but not necessarily
// physical) state of the object is to be set to its initial, empty state (via
// the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ELEMENT> | <ELEMENT><LIST>
//
// <ELEMENT>    ::= <SEND_BUF_SIZE> | <RECV_BUF_SIZE> | <MIN_SEND_BUF_SIZE>
//                  <MIN_RECV_BUF_SIZE> | <SEND_TIMEOUT> | <RECV_TIMEOUT>
//                  <DEBUG> | <ALLOW_BROADCASTING> | <REUSE_ADDR> |
//                  <KEEP_ALIVE> | <BYPASS_ROUTING> |
//                  <LEAVE_OUT_OF_BAND_DATA_INLINE> | <LINGER> | <TCP_NODELAY>
//
// <ELEMENT>    ::= <SPEC><VALUE> | <SPEC><BOOLEAN> | <SPEC><LINGER>
//
// <SPEC>       ::= 'A'              // <SEND_BUF_SIZE>
//                | 'B'              // <RECV_BUF_SIZE>
//                | 'C'              // <MIN_SEND_BUF_SIZE>
//                | 'D'              // <MIN_RECV_BUF_SIZE>
//                | 'E'              // <SEND_TIMEOUT>
//                | 'F'              // <RECV_TIMEOUT>
//                | 'G'              // <DEBUG>
//                | 'H'              // <ALLOW_BROADCASTING>
//                | 'I'              // <REUSE_ADDR>
//                | 'J'              // <KEEP_ALIVE>
//                | 'K'              // <BYPASS_ROUTING>
//                | 'L'              // <LEAVE_OUT_OF_BAND_INLINE>
//                | 'M'              // <LINGER>
//                | 'N'              // <TCP_NODELAY>
//
// <VALUE>      ::= '0' | '1' | '2'
// <BOOLEAN>    ::= 'Y' | 'N'
// <LINGER>     ::= <BOOLEAN><VALUE>
//
//-----------------------------------------------------------------------------

int TIMEOUTS[] = { 0, 1, 2 };
int DATA[]     = { 16, 64, 128 };
int SIZES[]    = { 1024, 2048, 4096 };

int setOption(SocketOptions *options, const char *specString)
    // Set the spec.  Return the number of characters read.
{
    const char spec  = *specString;
    const char value = *(specString + 1);
    int        numRead = 2;
    switch (spec) {
      case 'A': {  // SEND_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setSendBufferSize(SIZES[value - '0']);
      } break;
      case 'B': {  // RECV_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setReceiveBufferSize(SIZES[value - '0']);
      } break;
      case 'C': {  // MIN_SEND_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setMinimumSendBufferSize(SIZES[value - '0']);
      } break;
      case 'D': {  // MIN_RECV_BUF_SIZE
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setMinimumReceiveBufferSize(SIZES[value - '0']);
      } break;
      case 'E': {  // SEND_TIMEOUT
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setSendTimeout(TIMEOUTS[value - '0']);
      } break;
      case 'F': {  // RECEIVE_TIMEOUT
        LOOP_ASSERT(value, '0' == value || '1' == value || '2' == value);
        options->setReceiveTimeout(TIMEOUTS[value - '0']);
      } break;
      case 'G': {  // DEBUG
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setDebugFlag('Y' == value ? true : false);
      } break;
      case 'H': {  // ALLOW BROADCASTING
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setAllowBroadcasting('Y' == value ? true : false);
      } break;
      case 'I': {  // REUSE ADDR
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setReuseAddress('Y' == value ? true : false);
      } break;
      case 'J': {  // KEEP ALIVE
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setKeepAlive('Y' == value ? true : false);
      } break;
      case 'K': {  // BYPASS ROUTING
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setBypassNormalRouting('Y' == value ? true : false);
      } break;
      case 'L': {  // LEAVE_OUT_OF_BAND_DATA_INLINE
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setLeaveOutOfBandDataInline('Y' == value ? true : false);
      } break;
      case 'M': {  // LINGER
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        LingerOptions linger;
        linger.setLingerFlag('Y' == value ? true : false);
        const char nextValue = *(specString + 2);
        LOOP_ASSERT(nextValue, '0' == nextValue
                    || '1' == nextValue || '2' == nextValue);
        linger.setTimeout(DATA[nextValue - '0']);
        options->setLinger(linger);
        ++numRead;
      } break;
      case 'N': {  // TCP_NODELAY
        LOOP_ASSERT(value, 'Y' == value || 'N' == value);
        options->setTcpNoDelay('Y' == value);
      } break;
      default: {
        LOOP2_ASSERT(spec, value, 0);
        return 0;                                                     // RETURN
      } break;
    }
    return numRead;
}

int ggg(SocketOptions *object, const char *spec)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator functions 'setXXX'.  Return the index
    // of the first invalid character, and a negative value otherwise.  Note
    // that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    enum { SUCCESS = -1 };
    while (*spec) {
        const int numRead = setOption(object, spec);
        if (!numRead) {
            return numRead;                                           // RETURN
        }
        spec += numRead;
    }
    return SUCCESS;
}

SocketOptions& gg(SocketOptions *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

SocketOptions g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    SocketOptions object;
    return gg(&object, spec);
}

int verify(btlso::SocketHandle::Handle handle,
           const btlso::SocketOptions& options)
    // Verify that the socket options on the specified 'handle' match the
    // specified 'options'.  Return 0 on success and a non-zero value
    // otherwise.
{
    if (!options.debugFlag().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_DEBUGINFO);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.debugFlag().value(),
                     (bool) result == options.debugFlag().value());
    }

    if (!options.allowBroadcasting().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_BROADCAST);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.allowBroadcasting().value(),
                     (bool) result == options.allowBroadcasting().value());
    }

    if (!options.reuseAddress().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                      &result,
                                      handle,
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_REUSEADDRESS);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) result, options.reuseAddress().value(),
                     (bool) result == options.reuseAddress().value());
    }

    if (!options.keepAlive().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_KEEPALIVE);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.keepAlive().value(),
                     (bool) result == options.keepAlive().value());
    }

    if (!options.bypassNormalRouting().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_DONTROUTE);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.bypassNormalRouting().value(),
                     (bool) result == options.bypassNormalRouting().value());
    }

    if (!options.linger().isNull()) {
        btlso::SocketOptUtil::LingerData lingerData;
        const int rc = SockOptUtil::getOption(
                                        &lingerData,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_LINGER);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) lingerData.l_onoff,
                     options.linger().value().lingerFlag(),
                     (bool) lingerData.l_onoff
                               == options.linger().value().lingerFlag());
        LOOP2_ASSERT(lingerData.l_linger, options.linger().value().timeout(),
                    lingerData.l_linger == options.linger().value().timeout());
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_OOBINLINE);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.leaveOutOfBandDataInline().value(),
                  (bool) result == options.leaveOutOfBandDataInline().value());
    }

    if (!options.sendBufferSize().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_SENDBUFFER);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.sendBufferSize().value(),
                     result >= options.sendBufferSize().value());
    }

    if (!options.receiveBufferSize().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                     &result,
                                     handle,
                                     btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     btlso::SocketOptUtil::k_RECEIVEBUFFER);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveBufferSize().value(),
                     result >= options.receiveBufferSize().value());
    }

    if (!options.minimumSendBufferSize().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                       &result,
                                       handle,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       btlso::SocketOptUtil::k_SENDLOWATER);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.minimumSendBufferSize().value(),
                     result == options.minimumSendBufferSize().value());
    }

    if (!options.minimumReceiveBufferSize().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                    &result,
                                    handle,
                                    btlso::SocketOptUtil::k_SOCKETLEVEL,
                                    btlso::SocketOptUtil::k_RECEIVELOWATER);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.minimumReceiveBufferSize().value(),
                     result == options.minimumReceiveBufferSize().value());
    }

    if (!options.sendTimeout().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                       &result,
                                       handle,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       btlso::SocketOptUtil::k_SENDTIMEOUT);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.sendTimeout().value(),
                     result == options.sendTimeout().value());
    }

    if (!options.receiveTimeout().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                    &result,
                                    handle,
                                    btlso::SocketOptUtil::k_SOCKETLEVEL,
                                    btlso::SocketOptUtil::k_RECEIVETIMEOUT);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveTimeout().value(),
                     result == options.receiveTimeout().value());
    }

    if (!options.tcpNoDelay().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                    &result,
                                    handle,
                                    btlso::SocketOptUtil::k_TCPLEVEL,
                                    btlso::SocketOptUtil::k_TCPNODELAY);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.tcpNoDelay().value(),
                     (bool) result == options.tcpNoDelay().value());
    }

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    int errorcode = 0;
    btlso::SocketImpUtil::startup(&errorcode);
    ASSERT(0 == errorcode);

    switch (test) { case 0:  // always the leading case.
      case 6: {
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

        int result, errorcode=0;
        int addropt = 1;
        if (verbose) cout << "Testing Usage Example" << endl
                          << "=====================" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting 'k_REUSEADDRESS' option on a socket
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to set 'k_REUSEADDRESS' flag
// on a socket.  Note that we assume that a socket of type 'SOCK_STREAM' is
// already created and socket allocation 'btlso_socketimputil' can be used.
//..
        btlso::SocketHandle::Handle sockethandle;

        btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                      &sockethandle,
                                      btlso::SocketImpUtil::k_SOCKET_STREAM,
                                      &errorcode);
        ASSERT(0 == errorcode);

        result = btlso::SocketOptUtil::setOption(
                                       sockethandle,
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       btlso::SocketOptUtil::k_REUSEADDRESS,
                                       addropt);

        ASSERT(0 == result);

        addropt = 0;
        result = btlso::SocketOptUtil::getOption(
                                      &addropt, sockethandle,
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_REUSEADDRESS);

        ASSERT(0 == result);
        ASSERT(0 != addropt);

        int sockettype = 0;
        result = btlso::SocketOptUtil::getOption(
                          &sockettype,
                          sockethandle, btlso::SocketOptUtil::k_SOCKETLEVEL,
                          btlso::SocketOptUtil::k_TYPE);

        ASSERT(0 == result);
        ASSERT(SOCK_STREAM == sockettype);

//..
///Example 2: Setting linger options
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to set and get linger data
// option values for a socket.  First set the linger interval to 2 seconds:
//..
        btlso::SocketOptUtil::LingerData ld;
        ld.l_onoff  = 1;   // Enable lingering for
        ld.l_linger = 2;   // 2 seconds
        result = btlso::SocketOptUtil::setOption(sockethandle,
                                      btlso::SocketOptUtil::k_SOCKETLEVEL,
                                      btlso::SocketOptUtil::k_LINGER,
                                      ld, &errorcode);

        ASSERT(0 == result);
        ASSERT(0 == errorcode);

//..
///Example 3: Illustrating pass-through ability
///- - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the pass-through ability.
//..
#ifdef BSLS_PLATFORM_OS_WINDOWS
            /*
            WSAPROTOCOL_INFO protoInfo;
            memset(&ProtoInfo,0,sizeof(ProtoInfo));
            result = btlso::SocketOptUtil::getOption(&protoInfo, sockethandle,
                     btlso::SocketOptUtil::k_SOCKETLEVEL, SO_PROTOCOL_INFO,
                     &errorcode);

            ASSERT(0 == result);
            ASSERT(0 == errorcode);

            ASSERT(SOCK_STREAM == ProtoInfo.iAddressFamily);
            ASSERT(ProtocolInformation == ProtoInfo.iProtocol);
            // ASSERT(ProtocolVersion == ProtoInfo.iVersion);
            //            ASSERT(SocketType == ProtoInfo.iSocketType);
            */
#endif

      } break;
      case 5: {
        // ----------------------------------------------------------------
        // TESTING int 'setSocketOptions' FUNCTION:
        //
        // Plan:
        //
        // Testing
        //   int setSocketOptions(btlso::SocketHandle::Handle h, options);
        // ----------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setSocketOptions'" << endl
                          << "==========================" << endl;

        // Boolean options
        {
            const struct {
                    int         d_line;
                    const char *d_spec_p;
                    int         d_exp0;    // EXP when k == 0
                    int         d_exp1;    // EXP when k == 1
            } DATA[] = {
                  // Line   Spec  Exp
                  // ----   ----  ---
                  {   L_,   "GN",         0, 0 },

#ifdef BSLS_PLATFORM_OS_LINUX
                  {   L_,   "GY",        -1, -1 },
#else
                  {   L_,   "GY",         0,  0 },
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
                  {   L_,   "HN",        -1, 0 },
                  {   L_,   "HY",        -1, 0 },
#else
                  {   L_,   "HN",         0, 0 },
                  {   L_,   "HY",         0, 0 },
#endif
                  {   L_,   "IN",         0, 0 },
                  {   L_,   "IY",         0, 0 },

#if defined(BSLS_PLATFORM_OS_WINDOWS)                           \
 || defined(BSLS_PLATFORM_OS_SOLARIS)                           \
 || defined(BSLS_PLATFORM_OS_AIX)
                  {   L_,   "JN",         0, -1 },
                  {   L_,   "JY",         0, -1 },
#else
                  {   L_,   "JN",         0, 0 },
                  {   L_,   "JY",         0, 0 },
#endif
                  {   L_,   "KN",         0, 0 },
                  {   L_,   "KY",         0, 0 },

#if defined(BSLS_PLATFORM_OS_WINDOWS)                           \
 || defined(BSLS_PLATFORM_OS_HPUX)                              \
 || defined(BSLS_PLATFORM_OS_SOLARIS)
                  {   L_,   "LN",         0, -1 },
                  {   L_,   "LY",         0, -1 },
#else
                  {   L_,   "LN",         0, 0 },
                  {   L_,   "LY",         0, 0 },
#endif

#ifndef BSLS_PLATFORM_OS_WINDOWS
                  {   L_,   "NN",         0, -1 },
#endif
                  {   L_,   "NY",         0, -1 },

#ifdef BSLS_PLATFORM_OS_WINDOWS
                  {   L_,   "GNHN",      -1, 0 },
                  {   L_,   "GNHYIN",    -1, 0 },
                  {   L_,   "GNHYIYKY",  -1, 0 },
#else
                  {   L_,   "GNHN",       0, 0 },
                  {   L_,   "GNHYIN",     0, 0 },
                  {   L_,   "GNHYIYKY",   0, 0 },
#endif
                };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            btlso::SocketHandle::Handle handles[NUM_DATA];
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;

                SocketOptions mX = g(SPEC); const SocketOptions& X = mX;
                for (int k = 0; k < 2; ++k) {
                    const int EXP = k ? DATA[i].d_exp1 : DATA[i].d_exp0;

                    btlso::SocketImpUtil::Type type =
                            k
                            ? btlso::SocketImpUtil::k_SOCKET_DATAGRAM
                            : btlso::SocketImpUtil::k_SOCKET_STREAM;

                    if (veryVerbose) {
                        P_(LINE) P_(k) P_(SPEC) P(EXP)
                    }

                    int err = 0;
                    btlso::SocketImpUtil::open<btlso::IPv4Address>(&handles[i],
                                                                   type,
                                                                   &err);
                    LOOP_ASSERT(i, 0 == err);
                    int rc = btlso::SocketOptUtil::setSocketOptions(handles[i],
                                                                    X);
                    LOOP4_ASSERT(LINE, k, EXP, rc, EXP == rc);
                    if (!EXP) {
                        LOOP3_ASSERT(LINE, k, X, !verify(handles[i], X));
                    }
                }
            }
        }

        // Numerical options
        {
            const struct {
                    int         d_line;
                    const char *d_spec_p;
                    int         d_exp0;
                    int         d_exp1;
            } DATA[] = {
             // Line   Spec
             // ----   ----
              {   L_,   "A0",         0, 0 },
              {   L_,   "A1",         0, 0 },
              {   L_,   "A2",         0, 0 },

              {   L_,   "B0",         0, 0 },
              {   L_,   "B1",         0, 0 },
              {   L_,   "B2",         0, 0 },

#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_LINUX) &&\
    !defined(BSLS_PLATFORM_OS_WINDOWS)
              // Cannot be changed on Linux and not specified on Sun

              {   L_,   "C0",         0, 0 },
              {   L_,   "C1",         0, 0 },
              {   L_,   "C2",         0, 0 },
#else
              {   L_,   "C0",        -1, -1 },
              {   L_,   "C1",        -1, -1 },
              {   L_,   "C2",        -1, -1 },
#endif

#if !defined(BSLS_PLATFORM_OS_SOLARIS) && !defined(BSLS_PLATFORM_OS_WINDOWS)
              {   L_,   "D0",         0, 0 },
              {   L_,   "D1",         0, 0 },
              {   L_,   "D2",         0, 0 },
#else
              {   L_,   "D0",        -1, -1 },
              {   L_,   "D1",        -1, -1 },
              {   L_,   "D2",        -1, -1 },
#endif

# ifdef BSLS_PLATFORM_OS_WINDOWS
              {   L_,   "E0",        0, 0 },
              {   L_,   "E1",        0, 0 },
              {   L_,   "E2",        0, 0 },

              {   L_,   "F0",        0, 0 },
              {   L_,   "F1",        0, 0 },
              {   L_,   "F2",        0, 0 },
# else
              {   L_,   "E0",        -1, -1 },
              {   L_,   "E1",        -1, -1 },
              {   L_,   "E2",        -1, -1 },

              {   L_,   "F0",        -1, -1 },
              {   L_,   "F1",        -1, -1 },
              {   L_,   "F2",        -1, -1 },
# endif

#if defined(BSLS_PLATFORM_OS_AIX)
              // Works only on IBM.  On other platforms although the return
              // code is 0, the timeout is not set correctly.

              {   L_,   "MN1",       0, 0 },
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_SOLARIS)
              {   L_,   "MY2",       0, -1 },
              {   L_,   "A1B2MY2",   0, -1 },
#else
              {   L_,   "MY2",       0, 0 },
              {   L_,   "A1B2MY2",   0, 0 },
#endif
                };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            btlso::SocketHandle::Handle handles[NUM_DATA];
            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const char *SPEC = DATA[i].d_spec_p;

                SocketOptions mX = g(SPEC); const SocketOptions& X = mX;
                for (int k = 0; k < 2; ++k) {
                    const int EXP = k ? DATA[i].d_exp1 : DATA[i].d_exp0;

                    btlso::SocketImpUtil::Type type =
                            k
                            ? btlso::SocketImpUtil::k_SOCKET_DATAGRAM
                            : btlso::SocketImpUtil::k_SOCKET_STREAM;

                    if (veryVerbose) {
                        P_(LINE) P_(k) P_(SPEC) P_(type) P(EXP)
                    }

                    int err = 0;
                    btlso::SocketImpUtil::open<btlso::IPv4Address>(&handles[i],
                                                                   type,
                                                                   &err);
                    LOOP_ASSERT(i, 0 == err);
                    int rc = btlso::SocketOptUtil::setSocketOptions(handles[i],
                                                                    X);
                    LOOP5_ASSERT(LINE, k, SPEC, EXP, rc, EXP == rc);
                    if (!rc) {
                        LOOP3_ASSERT(LINE, k, SPEC,
                                     !verify(handles[i], X));
                    }
                }
            }
        }
      } break;
        case 4: {
          // ----------------------------------------------------------------
          // TESTING int setsockopt FUNCTION:
          //   The function returns zero on success and does not modify
          //   'errorCode'.  Otherwise it returns -1 on error and return
          //   the optional native platform error code in the specified
          //   'errorCode'.
          //
          // Plan:
          //   Test protocol levels and options which are not listed in this
          //   component.  For a sequence of various protocol levels and
          //   options, try setting option values and verify their validity.
          //
          // Testing
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& value);
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& Value,
          //                        int *errorCode);
          // ---------------------------------------------------------------

            static int result;
            int errorcode = 0;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nNon-listed protocol level and options"
                              << endl;

            #ifdef BSLS_PLATFORM_OS_UNIX
            btlso::SocketImpUtil::Type udp;
            udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

            struct {
              int                       d_lineNum;
              btlso::SocketImpUtil::Type type;
              int                       opt;          // socket options.
              int                       onVal;        // enable option
              int                       offVal;       // disable option
              int                       sockLevel;
            } SOCK_OPTS[] =
              //line   type        opt       onVal  offVal   sockLevel
              //----   ----        ---       -----  ------   ---------
            {
              #if defined(BSLS_PLATFORM_OS_LINUX)
              // IP_RECVDSTADDR is not supported on Linux
              // IP_PKTINFO does the job
              { L_,     udp,  IP_PKTINFO,       12,      0, IPPROTO_IP }
              #elif defined(BSLS_PLATFORM_OS_CYGWIN) || \
                    defined(BSLS_PLATFORM_OS_HPUX)
              { L_,     udp,  IP_TTL,           12,      0, IPPROTO_IP }
              #else
              { L_,     udp,  IP_RECVDSTADDR,   12,      0, IPPROTO_IP },
              #endif
            };

            #elif defined BSLS_PLATFORM_OS_WINDOWS
            btlso::SocketImpUtil::Type udp, tcp;
            udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;
            tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;

            struct {
              int                       d_lineNum;
              btlso::SocketImpUtil::Type type;
              int                       opt;       // socket options.
              int                       onVal;     // enable option
              int                       offVal;    // disable option
              int                       sockLevel;
            } SOCK_OPTS[] =
             //line  type       opt             onVal   offVal   sockLevel
             //----  ----       ---             -----   ------   ---------
            {
              { L_,   tcp,  SO_CONDITIONAL_ACCEPT,  1,       0,  SO_LEVEL },
              { L_,   udp,  SO_CONDITIONAL_ACCEPT,  1,       0,  SO_LEVEL },
              { L_,   tcp,  SO_EXCLUSIVEADDRUSE,   64,       0,  SO_LEVEL },
              { L_,   udp,  SO_EXCLUSIVEADDRUSE,   64,       0,  SO_LEVEL }
            };
            #endif

            const int NUM_OPTS = sizeof SOCK_OPTS / sizeof *SOCK_OPTS;
            btlso::SocketHandle::Handle serverSocket[NUM_OPTS];

            for (int i = 0; i < NUM_OPTS; i++) {
                int optResult = 0;
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                           &serverSocket[i],
                           SOCK_OPTS[i].type,
                           &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);

                result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     SOCK_OPTS[i].sockLevel,
                                     SOCK_OPTS[i].opt,
                                     SOCK_OPTS[i].onVal, &errorcode);

                LOOP2_ASSERT(SOCK_OPTS[i].d_lineNum, errno, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                if (veryVerbose)
                    cout << "Use system getsockopt() to get opt info."
                         << endl;
                {

                    #ifdef BSLS_PLATFORM_OS_WINDOWS
                    int optsize = sizeof(optResult);
                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (char*)&optResult, &optsize);
                    #else
                    #ifdef BSLS_PLATFORM_OS_AIX
                      #ifdef BSLS_PLATFORM_CPU_32_BIT
                           unsigned long optsize = sizeof(optResult);
                      #else
                           unsigned int optsize = sizeof(optResult);
                      #endif
                    #endif
                    #if defined(BSLS_PLATFORM_OS_HPUX)
                        int optsize = sizeof(optResult);
                    #endif
                    #if defined(BSLS_PLATFORM_OS_SOLARIS)
                        unsigned int optsize = sizeof(optResult);
                    #endif
                   #if    defined(BSLS_PLATFORM_OS_LINUX) \
                       || defined(BSLS_PLATFORM_OS_CYGWIN) \
                       || defined(BSLS_PLATFORM_OS_DARWIN) \
                       || defined(BSLS_PLATFORM_OS_FREEBSD)
                        socklen_t optsize = sizeof(optResult);
                   #endif

                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (void*)&optResult, &optsize);
                    #endif

                    LOOP_ASSERT(i, 0 == result);
                    LOOP2_ASSERT(i, SOCK_OPTS[i].d_lineNum, 0 != optResult);
                }

                result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP2_ASSERT(i, SOCK_OPTS[i].d_lineNum, 0 != optResult);

                // Set the option off.
                result = btlso::SocketOptUtil::setOption(
                                             serverSocket[i],
                                             SOCK_OPTS[i].sockLevel,
                                             SOCK_OPTS[i].opt,
                                             SOCK_OPTS[i].offVal,
                                             &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                result = btlso::SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 == optResult);

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);
            }
        } break;
        case 3: {
          // ----------------------------------------------------------------
          // TESTING 'setOption' FUNCTION:
          //   The function returns zero on success and does not modify
          //   'errorCode'.  Otherwise it returns -1 on error and return
          //   the optional native platform error code in the specified
          //   'errorCode'.
          // Plan:
          //   Test listed protocol levels and non-listed options.
          //   For a sequence of various protocol levels and options,
          //   try setting option values and verify their validity.
          // Testing
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& value);
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& Value,
          //                        int *errorCode);
          // ---------------------------------------------------------------
            static int result;
            int errorcode = 0;
            btlso::SocketImpUtil::Type tcp, udp;
            tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
            udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nSOL_SOCKET & TCPLEVEL OPTIONS Not Listed"
                              << endl;

            #ifdef BSLS_PLATFORM_OS_UNIX
            struct {
              int                       d_lineNum;
              btlso::SocketImpUtil::Type type;
              int                       opt;          // socket options.
              int                       onVal;        // enable option
              int                       offVal;       // disable option
              int                       sockLevel;
            } SOCK_OPTS[] =
              //line  type       opt        onVal   offVal   sockLevel
              //----  ----       ---        -----   ------   ---------
            {
              #ifdef BSLS_PLATFORM_OS_LINUX
              // SO_USELOOPBACK does not exist on Linux
              { L_,    tcp,  SO_KEEPALIVE,     64,       0,   SO_LEVEL },
              { L_,    udp,  SO_KEEPALIVE,     64,       0,   SO_LEVEL },
              #elif defined(BSLS_PLATFORM_OS_CYGWIN)
              // SO_USELOOPBACK does not exist on Cygwin
              { L_,    tcp,  SO_DEBUG,          1,       0,   SO_LEVEL },
              { L_,    udp,  SO_DEBUG,          1,       0,   SO_LEVEL },
              #else
              { L_,    tcp,  SO_USELOOPBACK,   64,       0,   SO_LEVEL },
              { L_,    udp,  SO_USELOOPBACK,   64,       0,   SO_LEVEL },
              #endif
              //{ L_,    udp, SO_DGRAM_ERRIND,   64,       0,   SO_LEVEL }
            };

            #elif defined BSLS_PLATFORM_OS_WINDOWS
               struct {
                 int                       d_lineNum;
                 btlso::SocketImpUtil::Type type;
                 int                       opt;        // socket options.
                 int                       onVal;      // enable option
                 int                       offVal;     // disable option
                 int                       sockLevel;
               } SOCK_OPTS[] =
               //line  type        opt            onVal   offVal   optLevel
               //----  ----        ---            -----   ------   --------
               {
                 { L_,  tcp,  SO_CONDITIONAL_ACCEPT,  1,       0,  SO_LEVEL },
                 { L_,  udp,  SO_CONDITIONAL_ACCEPT,  1,       0,  SO_LEVEL },
                 { L_,  tcp,  SO_EXCLUSIVEADDRUSE,   64,       0,  SO_LEVEL },
                 { L_,  udp,  SO_EXCLUSIVEADDRUSE,   64,       0,  SO_LEVEL }
               };
            #endif

            const int NUM_OPTS =
                              sizeof SOCK_OPTS / sizeof *SOCK_OPTS;
            btlso::SocketHandle::Handle serverSocket[NUM_OPTS];

            for (int i = 0; i < NUM_OPTS; i++) {
                int optResult = 0;
                btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 SOCK_OPTS[i].type,
                                 &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);

                result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     SOCK_OPTS[i].sockLevel,
                                     SOCK_OPTS[i].opt,
                                     SOCK_OPTS[i].onVal, &errorcode);

                LOOP_ASSERT(i, 0 == result);

                LOOP_ASSERT(i, 0 == errorcode);
                if(veryVerbose)
                    cout << "Use system getsockopt() to get opt info."
                         << endl;
                {
                    #ifdef BSLS_PLATFORM_OS_WINDOWS
                    int optsize = sizeof(optResult);
                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (char*)&optResult, &optsize);
                    #else
                    #ifdef BSLS_PLATFORM_OS_AIX
                      #ifdef BSLS_PLATFORM_CPU_32_BIT
                           unsigned long optsize = sizeof(optResult);
                      #else
                           unsigned int optsize = sizeof(optResult);
                      #endif
                    #endif
                    #if defined(BSLS_PLATFORM_OS_HPUX)
                        int optsize = sizeof(optResult);
                    #endif
                    #if defined(BSLS_PLATFORM_OS_SOLARIS)
                      unsigned int optsize = sizeof(optResult);
                    #endif
                    #if   defined(BSLS_PLATFORM_OS_LINUX) \
                       || defined(BSLS_PLATFORM_OS_CYGWIN) \
                       || defined(BSLS_PLATFORM_OS_DARWIN) \
                       || defined(BSLS_PLATFORM_OS_FREEBSD)
                        socklen_t optsize = sizeof(optResult);
                    #endif
                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (void*)&optResult, &optsize);
                    #endif
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 != optResult);
                }

                result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 != optResult);

                // Set the option off

                result = btlso::SocketOptUtil::setOption(
                                             serverSocket[i],
                                             SOCK_OPTS[i].sockLevel,
                                             SOCK_OPTS[i].opt,
                                             SOCK_OPTS[i].offVal,
                                             &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                result = btlso::SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 == optResult);

                btlso::SocketImpUtil::close(serverSocket[i],
                                           &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);
            }
        } break;
        case 2: {
          // ----------------------------------------------------------------
          // TESTING 'setOption' FUNCTION:
          //   The function returns zero on success and does not modify
          //   'errorCode'.  Otherwise it returns -1 on error and return
          //   the optional native platform error code in the specified
          //   'errorCode'.
          //
          // Plan:
          //   Test listed protocol levels and options.  For a sequence of
          //   various protocol levels and options, create a socket, set
          //   different options for the socket and verify their validity.
          //   Then enable 'linger' for the socket with two seconds time out
          //   value and next disable linger (SO_DONTLINGER).  Close the
          //   socket at the end.
          //   1. Test listed options for SOL_LEVEL level.
          //   2. Test listed options for IPPROTO_TCP level.
          //
          // Testing
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& value);
          //   template<class T>
          //   static int setOption(btlso::SocketHandle::Handle handle,
          //                        int level, int option, const T& Value,
          //                        int *errorCode);
          // ---------------------------------------------------------------
            static int result;
            int errorcode = 0, *errNull = 0;
            btlso::SocketImpUtil::Type tcp, udp;
            tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
            udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nTesting SOL_SOCKET Listed Options"
                              << endl;
            {
                struct {
                  int                        d_lineNum;
                  btlso::SocketImpUtil::Type  type;      // socket type.
                  int                        expType;
                                   // expected type of the new created socket.
                } SOCK_TYPES[] =
                //line     type         expType
                //----     ----         -------
                {
                  { L_,     tcp,      SOCK_STREAM },
                  { L_,     udp,      SOCK_DGRAM  }
                };

                struct {
                  int d_lineNum;
                  int opt;              // socket options.
                  int enableVal;        // enable the options
                  int disableVal;       // disable the options
                } SOCK_OPTS[] =
                //line              opt              enableVal   disableVal
                //----              ---              ---------   ----------
                {
                  #ifdef BSLS_PLATFORM_OS_LINUX
                  { L_,  btlso::SocketOptUtil::k_BROADCAST,     1,  0 },
                  #else
                  { L_,  btlso::SocketOptUtil::k_DEBUGINFO,     1,  0 },
                  #endif
                  { L_,  btlso::SocketOptUtil::k_REUSEADDRESS,  1,  0 },
                  { L_,  btlso::SocketOptUtil::k_DONTROUTE,     1,  0 },
                  { L_,  btlso::SocketOptUtil::k_SENDBUFFER,   64,  32 },
                  { L_,  btlso::SocketOptUtil::k_RECEIVEBUFFER,64,  32 }

                };
                const int NUM_TYPES =
                              sizeof SOCK_TYPES / sizeof *SOCK_TYPES;
                const int NUM_OPTS =
                              sizeof SOCK_OPTS / sizeof *SOCK_OPTS;
                btlso::SocketHandle::Handle serverSocket[NUM_TYPES];

                for (int i = 0; i < NUM_TYPES; i++) {
                    errorcode = 0;
                    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                               &serverSocket[i],
                               SOCK_TYPES[i].type,
                               &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);

                    for (int j = 0; j < NUM_OPTS; j++) {
                        int optResult=0;
                        errorcode = 0;
                        result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     btlso::SocketOptUtil::k_SOCKETLEVEL,
                        SOCK_OPTS[j].opt,
                        SOCK_OPTS[j].enableVal, &errorcode);

                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);

                        if(veryVerbose)
                            cout << "Use system getsockopt() to get opt info."
                                 << endl;
                        {
                            #ifdef BSLS_PLATFORM_OS_WINDOWS
                            int optsize = sizeof(optResult);
                            result = getsockopt(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        (char*)&optResult,
                                        &optsize);
                            #else
                            #ifdef BSLS_PLATFORM_OS_AIX
                              #ifdef BSLS_PLATFORM_CPU_32_BIT
                                unsigned long optsize = sizeof(optResult);
                              #else
                                unsigned int optsize = sizeof(optResult);
                              #endif
                            #endif
                            #if defined(BSLS_PLATFORM_OS_HPUX)
                                int optsize = sizeof(optResult);
                            #endif
                            #if   defined(BSLS_PLATFORM_OS_LINUX) \
                               || defined(BSLS_PLATFORM_OS_CYGWIN) \
                               || defined(BSLS_PLATFORM_OS_DARWIN) \
                               || defined(BSLS_PLATFORM_OS_SOLARIS) \
                               || defined(BSLS_PLATFORM_OS_FREEBSD)
                              socklen_t optsize = sizeof(optResult);
                            #endif
                            result = getsockopt(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        (void*)&optResult,
                                        &optsize);
                            #endif
                            LOOP2_ASSERT(i, j, 0 == result);
                            LOOP2_ASSERT(i, j, 0 != optResult);

                        }
                        result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);
                        // SNDBUF and RCVBUF don't go through the follow block.
                        if(j != 3 && j != 4) {
                            result = btlso::SocketOptUtil::setOption(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        SOCK_OPTS[j].disableVal,
                                        &errorcode);
                            LOOP2_ASSERT(i, j, 0 == result);
                            LOOP2_ASSERT(i, j, 0 == errorcode);
                            result = btlso::SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                                        LOOP2_ASSERT(i, j, 0 == result);
                                        LOOP2_ASSERT(i, j, 0 == errorcode);
                                        LOOP2_ASSERT(i, j, 0 == optResult);
                        }
                        // without passing 'errorcode'
                        result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     SOCK_OPTS[j].opt,
                                     SOCK_OPTS[j].enableVal);

                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);

                        result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);
                        // Pass 'errNull', a "NULL" pointer.
                        result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     btlso::SocketOptUtil::k_SOCKETLEVEL,
                                     SOCK_OPTS[j].opt,
                                     SOCK_OPTS[j].enableVal,
                                     errNull);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 == errNull);

                        result = btlso::SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);

                    }

                    // Set linger option: windows doesn't support udp linger.
                    #if   defined(BSLS_PLATFORM_OS_WINDOWS) \
                       || defined(BSLS_PLATFORM_OS_SOLARIS) \
                       || defined(BSLS_PLATFORM_OS_CYGWIN)
                        if (i != 0) continue;
                    #endif
                    LingerData ld = {1, 2}, ld2;

                    LOOP_ASSERT(i, 0 == errorcode);
                    result = btlso::SocketOptUtil::setOption(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_LINGER,
                                        ld, &errorcode);

                    LOOP3_ASSERT(i, result, errorcode, 0 == result);
                    ASSERT(0 == errorcode);

                    result = btlso::SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                btlso::SocketOptUtil::k_SOCKETLEVEL,
                                btlso::SocketOptUtil::k_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != ld2.l_onoff);
                    LOOP_ASSERT(i, 2 == ld2.l_linger);

                    // without passing 'errorcode'
                    ld.l_onoff = 0;     ld.l_linger = 5;
                    result = btlso::SocketOptUtil::setOption(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_LINGER,
                                        ld);
                    LOOP_ASSERT(i, 0 == result);
                    ASSERT(0 == errorcode);

                    result = btlso::SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                btlso::SocketOptUtil::k_SOCKETLEVEL,
                                btlso::SocketOptUtil::k_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 == ld2.l_onoff);

                    // pass 'errNull', which is 'NULL'.
                    ld.l_onoff = 2;     ld.l_linger = 5;
                    result = btlso::SocketOptUtil::setOption(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_LINGER,
                                        ld, errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);

                    result = btlso::SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                btlso::SocketOptUtil::k_SOCKETLEVEL,
                                btlso::SocketOptUtil::k_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != ld2.l_onoff);
                    LOOP_ASSERT(i, 5 == ld2.l_linger);

                    btlso::SocketImpUtil::close(serverSocket[i],
                                               &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);
                }
            }
            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nTesting TCPLEVEL Listed Options"
                              << endl;
            {
                struct {
                  int                        d_lineNum;
                  btlso::SocketImpUtil::Type  type;   // socket type.
                             // expected type of the new created socket.
                  int                        opt;

                } VALUES[] =
                //line     type                  opt
                //----     ----                  ---
                {
                  { L_,     tcp,     btlso::SocketOptUtil::k_TCPNODELAY },
                };
                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                btlso::SocketHandle::Handle serverSocket[NUM_VALUES];

                for (int i = 0; i < NUM_VALUES; i++) {
                    errorcode = 0;
                    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].type,
                                 &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);
                    int optValue=1, optResult=0;
                    result = btlso::SocketOptUtil::setOption(
                                     serverSocket[i],
                                     btlso::SocketOptUtil::k_TCPLEVEL,
                                     VALUES[i].opt,
                                     optValue, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);

                    result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_TCPLEVEL,
                                        VALUES[i].opt,
                                        &errorcode);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != optResult);

                    optValue=0;
                    result = btlso::SocketOptUtil::setOption(
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_TCPLEVEL,
                                        VALUES[i].opt,
                                        optValue, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);

                    result = btlso::SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_TCPLEVEL,
                                        VALUES[i].opt,
                                        &errorcode);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 == optResult);
                }
            }
        } break;
        case 1: {
          // ----------------------------------------------------------------
          // TESTING 'getOption' FUNCTION:
          //   Get the socket option specified by 'result' for the
          //   specified 'level' and store in the specified 'optionValue'.
          //   The function returns zero on success and does not modify
          //   'errorCode'.  Otherwise it returns -1 on error and return the
          //   optional native platform error code in the specified
          //   'errorCode'.
          //
          // Plan:
          //   Options "TYPE", "SOCKETERROR" can be retrieved because both
          //   options are set when a socket is created.  Verify the result
          //   retrieved from the function being tested by comparing it with
          //   the expected value.
          //
          // Testing
          //   template<class T>
          //   static int getOption(T                          *result,
          //                        btlso::SocketHandle::Handle  handle,
          //                        int level, int option);
          //   template<class T>
          //   static int getOption(T                         *result,
          //                        btlso::SocketHandle::Handle handle,
          //                        int level, int option, int *errorCode);
          // ----------------------------------------------------------------
            static int result;
            int errorcode = 0, *errNull = 0;
            btlso::SocketImpUtil::Type tcp, udp;

            tcp = btlso::SocketImpUtil::k_SOCKET_STREAM;
            udp = btlso::SocketImpUtil::k_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'getOption' FUNCTION "
                              << "\n============================"
                              << endl;

            {
                if(verbose) cout << "\nTesting Socket TYPE, SOCKETERROR Option"
                                 << "\n======================================="
                                 << endl;

                struct {
                  int                        d_lineNum;
                  btlso::SocketImpUtil::Type  type;   // socket type.
                  int                        expType;
                                   // expected type of the new created socket.
                } VALUES[] =
                 //line     type          expType
                 //----     ----          -------
                {
                   { L_,     tcp,     SOCK_STREAM },
                   { L_,     udp,      SOCK_DGRAM }
                };

                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                btlso::SocketHandle::Handle serverSocket[NUM_VALUES];
                btlso::SocketImpUtil::Type sockettype[NUM_VALUES];
                int erroropt[NUM_VALUES];
                for (int i = 0; i < NUM_VALUES; i++) {
                    errorcode = 0;
                    btlso::SocketImpUtil::open<btlso::IPv4Address>(
                               &serverSocket[i],
                               VALUES[i].type,
                               &errorcode);
                    ASSERT(0 == errorcode);
                    result = btlso::SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_TYPE,
                                        &errorcode);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);
                    // without the 'errorCode' parameter.
                    result = btlso::SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_TYPE);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);

                    // Pass a 'errNull' with 'NULL' value.
                    result = btlso::SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_TYPE,
                                        errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);

                    result = btlso::SocketOptUtil::getOption(
                                        &erroropt[i],
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_SOCKETERROR,
                                        &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 == erroropt[i]);
                    // without the 'errorCode' parameter.
                    result = btlso::SocketOptUtil::getOption(
                                       &erroropt[i],
                                       serverSocket[i],
                                       btlso::SocketOptUtil::k_SOCKETLEVEL,
                                       btlso::SocketOptUtil::k_SOCKETERROR);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == erroropt[i]);

                    // Pass a 'errNull' with 'NULL' value.
                    result = btlso::SocketOptUtil::getOption(
                                        &erroropt[i],
                                        serverSocket[i],
                                        btlso::SocketOptUtil::k_SOCKETLEVEL,
                                        btlso::SocketOptUtil::k_SOCKETERROR,
                                        errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);

                    btlso::SocketImpUtil::close(serverSocket[i], &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);
                }
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
    errorcode = 0;
    btlso::SocketImpUtil::cleanup(&errorcode);
    ASSERT(0 == errorcode);
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
