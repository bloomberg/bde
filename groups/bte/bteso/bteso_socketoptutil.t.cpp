// bteso_socketoptutil.t.cpp       -*-C++-*-

#include <bteso_socketoptutil.h>

#include <bteso_socketimputil.h>
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

typedef bteso_SocketOptUtil::LingerData LingerData;
typedef bteso_SocketHandle::Handle SocketHandle;
const unsigned short DUMMY_PORT = 5000;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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
        case 5: {
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
