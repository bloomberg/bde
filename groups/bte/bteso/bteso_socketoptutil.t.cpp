// bteso_socketoptutil.t.cpp                                          -*-C++-*-

#include <bteso_socketoptutil.h>

#include <bteso_socketimputil.h>
#include <bteso_socketoptions.h>
#include <bteso_lingeroptions.h>
#include <bteso_ipv4address.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_c_sys_time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#else
#include <windows.h>
#endif

#include <bsl_strstream.h>
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
// MANIPULATORS
// template<class T>
// static int setOption(bteso_SocketHandle::Handle handle, int level,
//                      int option, const T& value);
// template<class T>
// static int setOption(bteso_SocketHandle::Handle handle, int level,
//                      int option, const T& Value,
//                      int *errorCode);
// [ 2] Test this function with protocol levels and options that are both
//      listed in this component.
// [ 3] Test this function with protocol levels which are listed in this
//      component and options which are not listed.
// [ 4] Test this function with protocol levels and options which are both not
//      listed in this component.
// ACCESSORS
// template<class T>
// static int getOption(T *result, bteso_SocketHandle::Handle handle,
//                      int level, int option);
// template<class T>
// static int getOption(T *result, bteso_SocketHandle::Handle handle,
//                      int level, int option,  int *errorCode);
// [ 1] Test this function by using "read-only" options such as TYPE.
// ----------------------------------------------------------------------------
// [ 5] USAGE example
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

#define SO_LEVEL  bteso_SocketOptUtil::BTESO_SOCKETLEVEL
#define TCP_LEVEL bteso_SocketOptUtil::BTESO_TCPLEVEL

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

typedef bteso_SocketOptUtil             SockOptUtil;
typedef bteso_SocketOptUtil::LingerData LingerData;
typedef bteso_SocketHandle::Handle      SocketHandle;
typedef bteso_SocketOptions             SocketOptions;
typedef bteso_LingerOptions             LingerOptions;

const unsigned short DUMMY_PORT = 5000;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure a 'bteso_SocketOptions' according to a custom language.
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
//                  <LEAVE_OUT_OF_BAND_DATA_INLINE> | LINGER
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
//
// <VALUE>      ::= '0' | '1' | '2'
// <BOOLEAN>    ::= 'Y' | 'N'
// <LINGER>     ::= <BOOLEAN><VALUE>
//
//-----------------------------------------------------------------------------

int TIMEOUTS[] = { 0, 1, 2 };
int DATA[] = { 16, 64, 128 };
int SIZES[] = { 1024, 2048, 4096 };

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
        linger.setUseLingeringFlag('Y' == value ? true : false);
        const char nextValue = *(specString + 2);
        LOOP_ASSERT(nextValue, '0' == nextValue
                    || '1' == nextValue || '2' == nextValue);
        linger.setTimeout(DATA[nextValue - '0']);
        options->setLinger(linger);
        ++numRead;
      } break;
      default: {
        LOOP2_ASSERT(spec, value, 0);
        return 0;
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
            return numRead;
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

int verify(bteso_SocketHandle::Handle handle,
           const bteso_SocketOptions& options)
    // Verify that the socket options on the specified 'handle' match the
    // specified 'options'.  Return 0 on success and a non-zero value
    // otherwise.
{
    if (!options.debugFlag().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_DEBUGINFO);
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
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_BROADCAST);
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
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_REUSEADDRESS);
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
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_KEEPALIVE);
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
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_DONTROUTE);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT((bool) result, options.bypassNormalRouting().value(),
                     (bool) result == options.bypassNormalRouting().value());
    }

    if (!options.linger().isNull()) {
        bteso_SocketOptUtil::LingerData lingerData;
        const int rc = SockOptUtil::getOption(
                                        &lingerData,
                                        handle,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_LINGER);
        if (rc) {
            return rc;                                                // RETURN
        }

        LOOP2_ASSERT((bool) lingerData.l_onoff,
                     options.linger().value().useLingeringFlag(),
                     (bool) lingerData.l_onoff
                               == options.linger().value().useLingeringFlag());
        LOOP2_ASSERT(lingerData.l_linger, options.linger().value().timeout(),
                    lingerData.l_linger == options.linger().value().timeout());
    }

    if (!options.leaveOutOfBandDataInline().isNull()) {
        int result;
        const int rc = SockOptUtil::getOption(
                                        &result,
                                        handle,
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_OOBINLINE);
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
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_SENDBUFFER);
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
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                     bteso_SocketOptUtil::BTESO_RECEIVEBUFFER);
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
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_SENDLOWATER);
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
                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                    bteso_SocketOptUtil::BTESO_RECEIVELOWATER);
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
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_SENDTIMEOUT);
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
                                    bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                    bteso_SocketOptUtil::BTESO_RECEIVETIMEOUT);
        if (rc) {
            return rc;                                                // RETURN
        }
        LOOP2_ASSERT(result, options.receiveTimeout().value(),
                     result == options.receiveTimeout().value());
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
    bteso_SocketImpUtil::startup(&errorcode);
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
            bteso_SocketHandle::Handle sockethandle;

            bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                      &sockethandle,
                                      bteso_SocketImpUtil::BTESO_SOCKET_STREAM,
                                      &errorcode);
            ASSERT(0 == errorcode);

            result = bteso_SocketOptUtil::setOption(
                                       sockethandle,
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                       addropt);

            ASSERT(0 == result);

            addropt = 0;
            result = bteso_SocketOptUtil::getOption(
                                      &addropt, sockethandle,
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_REUSEADDRESS);

            ASSERT(0 == result);
            ASSERT(0 != addropt);

            int sockettype = 0;
            result = bteso_SocketOptUtil::getOption(
                          &sockettype,
                          sockethandle, bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                          bteso_SocketOptUtil::BTESO_TYPE);

            ASSERT(0 == result);
            ASSERT(SOCK_STREAM == sockettype);

            // usage 2:
            bteso_SocketOptUtil::LingerData ld;
            ld.l_onoff  = 1;   // Enable lingering for
            ld.l_linger = 2;   // 2 seconds
            result = bteso_SocketOptUtil::setOption(sockethandle,
                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                      bteso_SocketOptUtil::BTESO_LINGER,
                                      ld, &errorcode);

            ASSERT(0 == result);
            ASSERT(0 == errorcode);

            #ifdef BSLS_PLATFORM__OS_WINDOWS
            /*
            WSAPROTOCOL_INFO protoInfo;
            memset(&ProtoInfo,0,sizeof(ProtoInfo));
            result = bteso_SocketOptUtil::getOption(&protoInfo, sockethandle,
                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL, SO_PROTOCOL_INFO,
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
            //   static int setSocketOptions(bteso_SocketHandle::Handle  h,
            //                               const SocketOptions        *opts);
            //   static int getSocketOptions(SocketOptions              *opts,
            //                               bteso_SocketHandle::Handle  h);
            // ----------------------------------------------------------------

            if (verbose) cout << endl <<
                        "'setSocketOptions and getSocketOptions' TEST" << endl
                     << "============================================" << endl;

            // Boolean options
            {
                const struct {
                    int         d_line;
                    const char *d_spec_p;
                    int         d_exp;
                } DATA[] = {
                  // Line   Spec  Exp
                  // ----   ----  ---
                  {   L_,   "GN",         0 },

#ifdef BSLS_PLATFORM__OS_LINUX
                  {   L_,   "GY",        -1 },
#else
                  {   L_,   "GY",         0 },
#endif

                  {   L_,   "HN",         0 },
                  {   L_,   "HY",         0 },
                  {   L_,   "IN",         0 },
                  {   L_,   "IY",         0 },

#ifndef BSLS_PLATFORM__OS_AIX
// TBD on AIX setting this option succeeds for BTESO_SOCKET_DATAGRAM
//                   {   L_,   "JN",        -1 },
//                   {   L_,   "JY",        -1 },
// #else
                  {   L_,   "JN",         0 },
                  {   L_,   "JY",         0 },
#endif

                  {   L_,   "KN",         0 },
                  {   L_,   "KY",         0 },

#ifndef BSLS_PLATFORM__OS_HPUX
// TBD on HPUX setting this option succeeds for BTESO_SOCKET_DATAGRAM
//                   {   L_,   "LN",        -1 },
//                   {   L_,   "LY",        -1 },
// #else
                  {   L_,   "LN",         0 },
                  {   L_,   "LY",         0 },
#endif

                  {   L_,   "GNHN",       0 },
                  {   L_,   "GNHYIN",     0 },
                  {   L_,   "GNHYIYKY",   0 },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                bteso_SocketHandle::Handle handles[NUM_DATA];
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE = DATA[i].d_line;
                    const char *SPEC = DATA[i].d_spec_p;
                    const int   EXP  = DATA[i].d_exp;

                    SocketOptions mX = g(SPEC); const SocketOptions& X = mX;
                    for (int k = 0; k < 2; ++k) {
                        bteso_SocketImpUtil::Type type =
                            k
                            ? bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM
                            : bteso_SocketImpUtil::BTESO_SOCKET_STREAM;

                        if (veryVerbose) {
                            P_(LINE) P_(SPEC) P_(type) P(X)
                        }

                        int err = 0;
                        bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                                                   &handles[i],
                                                                   type,
                                                                   &err);
                        LOOP_ASSERT(i, 0 == err);
                        int rc = bteso_SocketOptUtil::setSocketOptions(
                                                                    handles[i],
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
                    int         d_exp;
                } DATA[] = {
              // Line   Spec
              // ----   ----
              {   L_,   "A0",         0 },
              {   L_,   "A1",         0 },
              {   L_,   "A2",         0 },

              {   L_,   "B0",         0 },
              {   L_,   "B1",         0 },
              {   L_,   "B2",         0 },

#if !defined(BSLS_PLATFORM__OS_SOLARIS) && !defined(BSLS_PLATFORM__OS_LINUX)
              // Cannot be changed on Linux and not specified on Sun

              {   L_,   "C0",         0 },
              {   L_,   "C1",         0 },
              {   L_,   "C2",         0 },
#else
              {   L_,   "C0",        -1 },
              {   L_,   "C1",        -1 },
              {   L_,   "C2",        -1 },
#endif

#ifdef BSLS_PLATFORM__OS_SOLARIS
              {   L_,   "D0",        -1 },
              {   L_,   "D1",        -1 },
              {   L_,   "D2",        -1 },
#else
              {   L_,   "D0",         0 },
              {   L_,   "D1",         0 },
              {   L_,   "D2",         0 },
#endif

#ifndef BSLS_PLATFORM__OS_HPUX
// TBD on HPUX setting this option succeeds but the timeout value is not what
// was specified.
//               {   L_,   "E0",         0 },
//               {   L_,   "E1",         0 },
//               {   L_,   "E2",         0 },

//               {   L_,   "F0",         0 },
//               {   L_,   "F1",         0 },
//               {   L_,   "F2",         0 },
// #else
              {   L_,   "E0",        -1 },
              {   L_,   "E1",        -1 },
              {   L_,   "E2",        -1 },

              {   L_,   "F0",        -1 },
              {   L_,   "F1",        -1 },
              {   L_,   "F2",        -1 },
#endif

#if defined(BSLS_PLATFORM__OS_AIX)
              // Works only on IBM.  On other platforms although the return
              // code is 0, the timeout is not set correctly.

              {   L_,   "MN1",       0 },
#endif

              {   L_,   "MY2",       0 },

              {   L_,   "A1B2MY2",   0 },

                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                bteso_SocketHandle::Handle handles[NUM_DATA];
                for (int i = 0; i < NUM_DATA; ++i) {
                    const int   LINE = DATA[i].d_line;
                    const char *SPEC = DATA[i].d_spec_p;
                    const int   EXP  = DATA[i].d_exp;

                    SocketOptions mX = g(SPEC); const SocketOptions& X = mX;
                    for (int k = 0; k < 2; ++k) {
                        bteso_SocketImpUtil::Type type =
                            k
                            ? bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM
                            : bteso_SocketImpUtil::BTESO_SOCKET_STREAM;

                        if (veryVerbose) {
                            P_(LINE) P_(SPEC) P_(type) P(X)
                        }

                        int err = 0;
                        bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                                                   &handles[i],
                                                                   type,
                                                                   &err);
                        LOOP_ASSERT(i, 0 == err);
                        int rc = bteso_SocketOptUtil::setSocketOptions(
                                                                    handles[i],
                                                                    X);
                        LOOP4_ASSERT(LINE, k, EXP, rc, EXP == rc);
                        if (!EXP) {
                            LOOP3_ASSERT(LINE, k, X, !verify(handles[i], X));
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
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& value);
            //   template<class T>
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& Value,
            //                        int *errorCode);
            // ---------------------------------------------------------------

            static int result;
            int errorcode = 0;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nNon-listed protocol level and options"
                              << endl;

            #ifdef BSLS_PLATFORM__OS_UNIX
            bteso_SocketImpUtil::Type udp;
            udp = bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM;

            struct {
              int                       d_lineNum;
              bteso_SocketImpUtil::Type type;
              int                       opt;          // socket options.
              int                       onVal;        // enable option
              int                       offVal;       // disable option
              int                       sockLevel;
            } SOCK_OPTS[] =
              //line   type        opt       onVal  offVal   sockLevel
              //----   ----        ---       -----  ------   ---------
            {
              #if defined(BSLS_PLATFORM__OS_LINUX)
              // IP_RECVDSTADDR is not supported on Linux
              // IP_PKTINFO does the job
              { L_,     udp,  IP_PKTINFO,       12,      0, IPPROTO_IP }
              #elif defined(BSLS_PLATFORM__OS_CYGWIN) || \
                    defined(BSLS_PLATFORM__OS_HPUX)
              { L_,     udp,  IP_TTL,           12,      0, IPPROTO_IP }
              #else
              { L_,     udp,  IP_RECVDSTADDR,   12,      0, IPPROTO_IP },
              #endif
            };

            #elif defined BSLS_PLATFORM__OS_WINDOWS
            bteso_SocketImpUtil::Type udp, tcp;
            udp = bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM;
            tcp = bteso_SocketImpUtil::BTESO_SOCKET_STREAM;

            struct {
              int                       d_lineNum;
              bteso_SocketImpUtil::Type type;
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
            bteso_SocketHandle::Handle serverSocket[NUM_OPTS];

            for (int i = 0; i < NUM_OPTS; i++) {
                int optResult = 0;
                bteso_SocketImpUtil::open<bteso_IPv4Address>(
                           &serverSocket[i],
                           SOCK_OPTS[i].type,
                           &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);

                result = bteso_SocketOptUtil::setOption(
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

                    #ifdef BSLS_PLATFORM__OS_WINDOWS
                    int optsize = sizeof(optResult);
                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (char*)&optResult, &optsize);
                    #else
                    #ifdef BSLS_PLATFORM__OS_AIX
                      #ifdef BSLS_PLATFORM__CPU_32_BIT
                           unsigned long optsize = sizeof(optResult);
                      #else
                           unsigned int optsize = sizeof(optResult);
                      #endif
                    #endif
                    #if defined(BSLS_PLATFORM__OS_HPUX)
                        int optsize = sizeof(optResult);
                    #endif
                    #if defined(BSLS_PLATFORM__OS_SOLARIS)
                        unsigned int optsize = sizeof(optResult);
                    #endif
                   #if    defined(BSLS_PLATFORM__OS_LINUX) \
                       || defined(BSLS_PLATFORM__OS_CYGWIN) \
                       || defined(BDES_PLATFORM__OS_FREEBSD)
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

                result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP2_ASSERT(i, SOCK_OPTS[i].d_lineNum, 0 != optResult);

                // Set the option off.
                result = bteso_SocketOptUtil::setOption(
                                             serverSocket[i],
                                             SOCK_OPTS[i].sockLevel,
                                             SOCK_OPTS[i].opt,
                                             SOCK_OPTS[i].offVal,
                                             &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                result = bteso_SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 == optResult);

                bteso_SocketImpUtil::close(serverSocket[i],
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
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& value);
            //   template<class T>
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& Value,
            //                        int *errorCode);
            // ---------------------------------------------------------------
            static int result;
            int errorcode = 0;
            bteso_SocketImpUtil::Type tcp, udp;
            tcp = bteso_SocketImpUtil::BTESO_SOCKET_STREAM;
            udp = bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nSOL_SOCKET & TCPLEVEL OPTIONS Not Listed"
                              << endl;

            #ifdef BSLS_PLATFORM__OS_UNIX
            struct {
              int                       d_lineNum;
              bteso_SocketImpUtil::Type type;
              int                       opt;          // socket options.
              int                       onVal;        // enable option
              int                       offVal;       // disable option
              int                       sockLevel;
            } SOCK_OPTS[] =
              //line  type       opt        onVal   offVal   sockLevel
              //----  ----       ---        -----   ------   ---------
            {
              #ifdef BSLS_PLATFORM__OS_LINUX
              // SO_USELOOPBACK does not exist on Linux
              { L_,    tcp,  SO_KEEPALIVE,     64,       0,   SO_LEVEL },
              { L_,    udp,  SO_KEEPALIVE,     64,       0,   SO_LEVEL },
              #elif defined(BSLS_PLATFORM__OS_CYGWIN)
              // SO_USELOOPBACK does not exist on Cygwin
              { L_,    tcp,  SO_DEBUG,          1,       0,   SO_LEVEL },
              { L_,    udp,  SO_DEBUG,          1,       0,   SO_LEVEL },
              #else
              { L_,    tcp,  SO_USELOOPBACK,   64,       0,   SO_LEVEL },
              { L_,    udp,  SO_USELOOPBACK,   64,       0,   SO_LEVEL },
              #endif
              //{ L_,    udp, SO_DGRAM_ERRIND,   64,       0,   SO_LEVEL }
            };

            #elif defined BSLS_PLATFORM__OS_WINDOWS
               struct {
                 int                       d_lineNum;
                 bteso_SocketImpUtil::Type type;
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
            bteso_SocketHandle::Handle serverSocket[NUM_OPTS];

            for (int i = 0; i < NUM_OPTS; i++) {
                int optResult = 0;
                bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                 &serverSocket[i],
                                 SOCK_OPTS[i].type,
                                 &errorcode);
                LOOP_ASSERT(i, 0 == errorcode);

                result = bteso_SocketOptUtil::setOption(
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
                    #ifdef BSLS_PLATFORM__OS_WINDOWS
                    int optsize = sizeof(optResult);
                    result = getsockopt(serverSocket[i],
                                  SOCK_OPTS[i].sockLevel,
                                  SOCK_OPTS[i].opt,
                                  (char*)&optResult, &optsize);
                    #else
                    #ifdef BSLS_PLATFORM__OS_AIX
                      #ifdef BSLS_PLATFORM__CPU_32_BIT
                           unsigned long optsize = sizeof(optResult);
                      #else
                           unsigned int optsize = sizeof(optResult);
                      #endif
                    #endif
                    #if defined(BSLS_PLATFORM__OS_HPUX)
                        int optsize = sizeof(optResult);
                    #endif
                    #if defined(BSLS_PLATFORM__OS_SOLARIS)
                      unsigned int optsize = sizeof(optResult);
                    #endif
                    #if   defined(BSLS_PLATFORM__OS_LINUX) \
                       || defined(BSLS_PLATFORM__OS_CYGWIN) \
                       || defined(BDES_PLATFORM__OS_FREEBSD)
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

                result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 != optResult);

                // Set the option off

                result = bteso_SocketOptUtil::setOption(
                                             serverSocket[i],
                                             SOCK_OPTS[i].sockLevel,
                                             SOCK_OPTS[i].opt,
                                             SOCK_OPTS[i].offVal,
                                             &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                result = bteso_SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        SOCK_OPTS[i].sockLevel,
                                        SOCK_OPTS[i].opt,
                                        &errorcode);
                LOOP_ASSERT(i, 0 == result);
                LOOP_ASSERT(i, 0 == errorcode);
                LOOP_ASSERT(i, 0 == optResult);

                bteso_SocketImpUtil::close(serverSocket[i],
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
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& value);
            //   template<class T>
            //   static int setOption(bteso_SocketHandle::Handle handle,
            //                        int level, int option, const T& Value,
            //                        int *errorCode);
            // ---------------------------------------------------------------
            static int result;
            int errorcode = 0, *errNull = 0;
            bteso_SocketImpUtil::Type tcp, udp;
            tcp = bteso_SocketImpUtil::BTESO_SOCKET_STREAM;
            udp = bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'setOption' FUNCTION"
                              << "\n============================"
                              << "\nTesting SOL_SOCKET Listed Options"
                              << endl;
            {
                struct {
                  int                        d_lineNum;
                  bteso_SocketImpUtil::Type  type;      // socket type.
                  int                        expType;
                                   // expected type of the new created socket.
                } SOCK_TYPES[] =
                //line     type         expType
                //----     ----         -------
                {
                  { L_,     tcp,     SOCK_STREAM },
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
                  #ifdef BSLS_PLATFORM__OS_LINUX
                  { L_,  bteso_SocketOptUtil::BTESO_BROADCAST,     1,  0 },
                  #else
                  { L_,  bteso_SocketOptUtil::BTESO_DEBUGINFO,     1,  0 },
                  #endif
                  { L_,  bteso_SocketOptUtil::BTESO_REUSEADDRESS,  1,  0 },
                  { L_,  bteso_SocketOptUtil::BTESO_DONTROUTE,     1,  0 },
                  { L_,  bteso_SocketOptUtil::BTESO_SENDBUFFER,   64,  32 },
                  { L_,  bteso_SocketOptUtil::BTESO_RECEIVEBUFFER,64,  32 }

                };
                const int NUM_TYPES =
                              sizeof SOCK_TYPES / sizeof *SOCK_TYPES;
                const int NUM_OPTS =
                              sizeof SOCK_OPTS / sizeof *SOCK_OPTS;
                bteso_SocketHandle::Handle serverSocket[NUM_TYPES];

                for (int i = 0; i < NUM_TYPES; i++) {
                    errorcode = 0;
                    bteso_SocketImpUtil::open<bteso_IPv4Address>(
                               &serverSocket[i],
                               SOCK_TYPES[i].type,
                               &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);

                    for (int j = 0; j < NUM_OPTS; j++) {
                        int optResult=0;
                        errorcode = 0;
                        result = bteso_SocketOptUtil::setOption(
                                     serverSocket[i],
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                        SOCK_OPTS[j].opt,
                        SOCK_OPTS[j].enableVal, &errorcode);

                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);

                        if(veryVerbose)
                            cout << "Use system getsockopt() to get opt info."
                                 << endl;
                        {
                            #ifdef BSLS_PLATFORM__OS_WINDOWS
                            int optsize = sizeof(optResult);
                            result = getsockopt(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        (char*)&optResult,
                                        &optsize);
                            #else
                            #ifdef BSLS_PLATFORM__OS_AIX
                              #ifdef BSLS_PLATFORM__CPU_32_BIT
                                unsigned long optsize = sizeof(optResult);
                              #else
                                unsigned int optsize = sizeof(optResult);
                              #endif
                            #endif
                            #if defined(BSLS_PLATFORM__OS_HPUX)
                                int optsize = sizeof(optResult);
                            #endif
                            #if defined(BSLS_PLATFORM__OS_SOLARIS)
                              unsigned int optsize = sizeof(optResult);
                            #endif
                            #if   defined(BSLS_PLATFORM__OS_LINUX) \
                               || defined(BSLS_PLATFORM__OS_CYGWIN) \
                               || defined(BDES_PLATFORM__OS_FREEBSD)
                              socklen_t optsize = sizeof(optResult);
                            #endif
                            result = getsockopt(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        (void*)&optResult,
                                        &optsize);
                            #endif
                            LOOP2_ASSERT(i, j, 0 == result);
                            LOOP2_ASSERT(i, j, 0 != optResult);

                        }
                        result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);
                        // SNDBUF and RCVBUF don't go through the follow block.
                        if(j != 3 && j != 4) {
                            result = bteso_SocketOptUtil::setOption(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        SOCK_OPTS[j].disableVal,
                                        &errorcode);
                            LOOP2_ASSERT(i, j, 0 == result);
                            LOOP2_ASSERT(i, j, 0 == errorcode);
                            result = bteso_SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                                        LOOP2_ASSERT(i, j, 0 == result);
                                        LOOP2_ASSERT(i, j, 0 == errorcode);
                                        LOOP2_ASSERT(i, j, 0 == optResult);
                        }
                        // without passing 'errorcode'
                        result = bteso_SocketOptUtil::setOption(
                                     serverSocket[i],
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                     SOCK_OPTS[j].opt,
                                     SOCK_OPTS[j].enableVal);

                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);

                        result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);
                        // Pass 'errNull', a "NULL" pointer.
                        result = bteso_SocketOptUtil::setOption(
                                     serverSocket[i],
                                     bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                     SOCK_OPTS[j].opt,
                                     SOCK_OPTS[j].enableVal,
                                     errNull);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 == errNull);

                        result = bteso_SocketOptUtil::getOption(
                                        &optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        SOCK_OPTS[j].opt,
                                        &errorcode);
                        LOOP2_ASSERT(i, j, 0 == result);
                        LOOP2_ASSERT(i, j, 0 == errorcode);
                        LOOP2_ASSERT(i, j, 0 != optResult);

                    }

                    // Set linger option: windows doesn't support udp linger.
                    #if   defined(BSLS_PLATFORM__OS_WINDOWS) \
                       || defined(BSLS_PLATFORM__OS_CYGWIN)
                        if (i != 0) continue;
                    #endif
                    LingerData ld = {1, 2}, ld2;

                    LOOP_ASSERT(i, 0 == errorcode);
                    result = bteso_SocketOptUtil::setOption(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_LINGER,
                                        ld, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    ASSERT(0 == errorcode);

                    result = bteso_SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                bteso_SocketOptUtil::BTESO_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != ld2.l_onoff);
                    LOOP_ASSERT(i, 2 == ld2.l_linger);

                    // without passing 'errorcode'
                    ld.l_onoff = 0;     ld.l_linger = 5;
                    result = bteso_SocketOptUtil::setOption(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_LINGER,
                                        ld);
                    LOOP_ASSERT(i, 0 == result);
                    ASSERT(0 == errorcode);

                    result = bteso_SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                bteso_SocketOptUtil::BTESO_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 == ld2.l_onoff);

                    // pass 'errNull', which is 'NULL'.
                    ld.l_onoff = 2;     ld.l_linger = 5;
                    result = bteso_SocketOptUtil::setOption(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_LINGER,
                                        ld, errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);

                    result = bteso_SocketOptUtil::getOption(
                                &ld2, serverSocket[i],
                                bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                bteso_SocketOptUtil::BTESO_LINGER, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != ld2.l_onoff);
                    LOOP_ASSERT(i, 5 == ld2.l_linger);

                    bteso_SocketImpUtil::close(serverSocket[i],
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
                  bteso_SocketImpUtil::Type  type;   // socket type.
                             // expected type of the new created socket.
                  int                        opt;

                } VALUES[] =
                //line     type                  opt
                //----     ----                  ---
                {
                  { L_,     tcp,     bteso_SocketOptUtil::BTESO_TCPNODELAY },
                };
                const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
                bteso_SocketHandle::Handle serverSocket[NUM_VALUES];

                for (int i = 0; i < NUM_VALUES; i++) {
                    errorcode = 0;
                    bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                 &serverSocket[i],
                                 VALUES[i].type,
                                 &errorcode);
                    LOOP_ASSERT(i, 0 == errorcode);
                    int optValue=1, optResult=0;
                    result = bteso_SocketOptUtil::setOption(
                                     serverSocket[i],
                                     bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                     VALUES[i].opt,
                                     optValue, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);

                    result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                        VALUES[i].opt,
                                        &errorcode);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 != optResult);

                    optValue=0;
                    result = bteso_SocketOptUtil::setOption(
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_TCPLEVEL,
                                        VALUES[i].opt,
                                        optValue, &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);

                    result = bteso_SocketOptUtil::getOption(&optResult,
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_TCPLEVEL,
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
            //                        bteso_SocketHandle::Handle  handle,
            //                        int level, int option);
            //   template<class T>
            //   static int getOption(T                         *result,
            //                        bteso_SocketHandle::Handle handle,
            //                        int level, int option, int *errorCode);
            // ----------------------------------------------------------------
            static int result;
            int errorcode = 0, *errNull = 0;
            bteso_SocketImpUtil::Type tcp, udp;

            tcp = bteso_SocketImpUtil::BTESO_SOCKET_STREAM;
            udp = bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM;

            if (verbose) cout << "\nTesting 'getOption' FUNCTION "
                              << "\n============================"
                              << endl;

            {
                if(verbose) cout << "\nTesting Socket TYPE, SOCKETERROR Option"
                                 << "\n======================================="
                                 << endl;

                struct {
                  int                        d_lineNum;
                  bteso_SocketImpUtil::Type  type;   // socket type.
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
                bteso_SocketHandle::Handle serverSocket[NUM_VALUES];
                bteso_SocketImpUtil::Type sockettype[NUM_VALUES];
                int erroropt[NUM_VALUES];
                for (int i = 0; i < NUM_VALUES; i++) {
                    errorcode = 0;
                    bteso_SocketImpUtil::open<bteso_IPv4Address>(
                               &serverSocket[i],
                               VALUES[i].type,
                               &errorcode);
                    ASSERT(0 == errorcode);
                    result = bteso_SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_TYPE,
                                        &errorcode);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);
                    // without the 'errorCode' parameter.
                    result = bteso_SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_TYPE);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);

                    // Pass a 'errNull' with 'NULL' value.
                    result = bteso_SocketOptUtil::getOption(
                                        &sockettype[i],
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_TYPE,
                                        errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);
                    LOOP_ASSERT(i, VALUES[i].expType == sockettype[i]);

                    result = bteso_SocketOptUtil::getOption(
                                        &erroropt[i],
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_SOCKETERROR,
                                        &errorcode);

                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errorcode);
                    LOOP_ASSERT(i, 0 == erroropt[i]);
                    // without the 'errorCode' parameter.
                    result = bteso_SocketOptUtil::getOption(
                                       &erroropt[i],
                                       serverSocket[i],
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_SOCKETERROR);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == erroropt[i]);

                    // Pass a 'errNull' with 'NULL' value.
                    result = bteso_SocketOptUtil::getOption(
                                        &erroropt[i],
                                        serverSocket[i],
                                        bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                        bteso_SocketOptUtil::BTESO_SOCKETERROR,
                                        errNull);
                    LOOP_ASSERT(i, 0 == result);
                    LOOP_ASSERT(i, 0 == errNull);

                    bteso_SocketImpUtil::close(serverSocket[i], &errorcode);
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
    bteso_SocketImpUtil::cleanup(&errorcode);
    ASSERT(0 == errorcode);
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
