// btlmt_listenoptions.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_listenoptions.h>

#include <btlso_inetstreamsocket.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, unconstrained (value-semantic)
// attribute class.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btlmt::ListenOptions();
// [ 4] btlmt::ListenOptions(const btlmt::ListenOptions& original);
// [ 2] ~btlmt::ListenOptions();
//
// MANIPULATORS
// [ 4] ListenOptions& operator=(const btlmt::ListenOptions& rhs);
// [ 4] void reset();
// [ 3] void setServerAddress(const btlso::IPv4Address& value);
// [ 3] void setBacklog(int value);
// [ 3] void setTimeout(const bsls::TimeInterval& value);
// [ 3] void setEnableRead(bool value);
// [ 3] void setAllowHalfOpenConnections(bool value);
// [ 3] void setSocketOptions(const btlso::SocketOptions& value);
//
// ACCESSORS
// [ 5] ostream& print(ostream& s, int l, int spl);
// [ 3] const btlso::IPv4Address& serverAddress() const;
// [ 3] int backlog() const;
// [ 3] const bdlb::NullableValue<bsls::TimeInterval>& timeout() const;
// [ 3] bool enableRead() const;
// [ 3] bool allowHalfOpenConnections() const;
// [ 3] const btlso::SocketOptions& socketOptions() const;
//
// FREE OPERATORS
// [ 4] bool operator==(const btlmt::ListenOptions& lhs, rhs);
// [ 4] bool operator!=(const btlmt::ListenOptions& lhs, rhs);
// [ 5] operator<<(ostream& s, const btlmt::ListenOptions& d);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btlmt::ListenOptions                      Obj;
typedef btlso::IPv4Address                        IPAddr;
typedef bsls::TimeInterval                        TimeInterval;
typedef btlso::SocketOptions                      SockOpts;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

  void acceptCallback(int status, int acceptFd)
      // Process the requests on the connection identified by the specified
      // 'acceptFd' socket fd if 'status' is 0 and log an error otherwise.
  {
      // . . .
  }

  int listen(const btlmt::ListenOptions&   options,
             bsl::function<void(int, int)> callback)
      // Create a listening socket based on the specified listen 'options'
      // and wait for incoming connections.  Invoke the specified 'callback'
      // with a callback status and the socket fd of the accepted socket.
      // The callback status is 0 if a new socket was accepted and non-zero
      // otherwise.  If status is non-zero the socket fd passed to 'callback'
      // is -1.  Return 0 on success and a non-zero value otherewise.  The
      // 'callback' parameter is invoked as follows:
      //..
      //  void callback(int status, int acceptFd);
      //..
  {
      return 0;
  }

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
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

        if (verbose) bsl::cout << bsl::endl
                               << "USAGE EXAMPLE TEST" << bsl::endl
                               << "==================" << bsl::endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a listening server socket
/// - - - - - - - - - - - - - - - - - - - - - -
// This component allows specifying configuration options for creating
// listening sockets.  Consider that we want to set up a server application on
// the local machine that responds to incoming client requests.  The following
// example will show how to provide configuration options for doing so.
//
// We will assume that we have two functions: 1) a 'listen' method that accepts
// a 'btlmt::ListenOptions' object as an argument, creates a listening socket,
// and invokes a specified callback on incoming connections (or timeout), and
// 2) a 'acceptCallback' method that will be supplied to 'listen' to invoke
// when new sockets are accepted.  The signature of both functions is provided
// below but for brevity the implementation is elided (note that in practice
// clients would likely use a higher level component like 'btlmt_sessionpool'
// or 'btlmt_channelpool'):
//..
//..
// Next, we will default-construct the listen options object:
//..
    btlmt::ListenOptions options;
    ASSERT(btlso::IPv4Address()   == options.serverAddress());
    ASSERT(1                      == options.backlog());
    ASSERT(true                   == options.enableRead());
    ASSERT(false                  == options.allowHalfOpenConnections());
    ASSERT(btlso::SocketOptions() == options.socketOptions());
    ASSERT(options.timeout().isNull());
//..
// Next, we specify the server address that FTP server will listen on:
//..
    btlso::IPv4Address address;
    address.setPortNumber(12345);
//
    options.setServerAddress(address);
    ASSERT(address == options.serverAddress());
//..
// Then, we will specify the number of connections that can be waiting to be
// accepted:
//..
    const int BACKLOG = 10;
    options.setBacklog(BACKLOG);
    ASSERT(BACKLOG == options.backlog());
//..
// Next, the time to wait for an incoming connection is specified.  If an
// incoming connection does not arrive within that time frame the 'listen'
// method can invoke the user callback with a timeout status.
//..
    const bsls::TimeInterval TIMEOUT(2);
    options.setTimeout(TIMEOUT);
    ASSERT(TIMEOUT == options.timeout().value());
//..
// Then, we will populate a flag that specifies whether incoming connections
// can be half-open.  A half-open connection has only its read-end or write-end
// open.  For a connection the server may decide to close the read end
// while continuing to enqueue write data.
//..
    const bool ALLOW_HALF_OPEN_CONNECTIONS = true;
    options.setAllowHalfOpenConnections(ALLOW_HALF_OPEN_CONNECTIONS);
    ASSERT(ALLOW_HALF_OPEN_CONNECTIONS == options.allowHalfOpenConnections());
//..
// Now, we specify certain socket options for the listening socket such as
// allowing address reuse and disabling Nagle's data coalescing algorithm:
//..
    btlso::SocketOptions socketOptions;
    socketOptions.setReuseAddress(true);
    socketOptions.setTcpNoDelay(true);
    options.setSocketOptions(socketOptions);
    ASSERT(socketOptions == options.socketOptions());
//..
// Finally, we call the 'listen' method with the constructed 'options':
//..
    const int rc = listen(options, acceptCallback);
    ASSERT(0 == rc);
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const btlmt::ListenOptions& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace btlmt;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int              d_line;           // source line number
            int              d_level;
            int              d_spacesPerLevel;

            IPAddr           d_serverAddress;
            int              d_backlog;
            int              d_timeout;
            bool             d_enableRead;
            bool             d_allowHalfOpenConnections;
            bool             d_reuseAddr;

            const char      *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        {
            L_,
            0,
            0,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                      NL
            "serverAddress = 1.2.3.4:5"              NL
            "backlog = 10"                           NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "socketOptions = ["                      NL
            "DebugFlag = NULL"                       NL
            "AllowBroadcasting = NULL"               NL
            "ReuseAddress = 0"                       NL
            "KeepAlive = NULL"                       NL
            "BypassNormalRouting = NULL"             NL
            "Linger = NULL"                          NL
            "LeaveOutOfBandDataInline = NULL"        NL
            "SendBufferSize = NULL"                  NL
            "ReceiveBufferSize = NULL"               NL
            "MinimumSendBufferSize = NULL"           NL
            "MinimumReceiveBufferSize = NULL"        NL
            "SendTimeout = NULL"                     NL
            "ReceiveTimeout = NULL"                  NL
            "TcpNoDelay = NULL"                      NL
            "]"                                      NL
            "]"                                      NL
        },

        {
            L_,
            0,
            1,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                       NL
            " serverAddress = 1.2.3.4:5"              NL
            " backlog = 10"                           NL
            " timeout = (100, 0)"                     NL
            " enableRead = true"                      NL
            " allowHalfOpenConnections = false"       NL
            " socketOptions = ["                      NL
            "  DebugFlag = NULL"                      NL
            "  AllowBroadcasting = NULL"              NL
            "  ReuseAddress = 0"                      NL
            "  KeepAlive = NULL"                      NL
            "  BypassNormalRouting = NULL"            NL
            "  Linger = NULL"                         NL
            "  LeaveOutOfBandDataInline = NULL"       NL
            "  SendBufferSize = NULL"                 NL
            "  ReceiveBufferSize = NULL"              NL
            "  MinimumSendBufferSize = NULL"          NL
            "  MinimumReceiveBufferSize = NULL"       NL
            "  SendTimeout = NULL"                    NL
            "  ReceiveTimeout = NULL"                 NL
            "  TcpNoDelay = NULL"                     NL
            " ]"                                      NL
            "]"                                       NL
        },

        {
            L_,
            0,
            -1,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                      SP
            "serverAddress = 1.2.3.4:5"              SP
            "backlog = 10"                           SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "socketOptions = ["                      SP
            "DebugFlag = NULL"                       SP
            "AllowBroadcasting = NULL"               SP
            "ReuseAddress = 0"                       SP
            "KeepAlive = NULL"                       SP
            "BypassNormalRouting = NULL"             SP
            "Linger = NULL"                          SP
            "LeaveOutOfBandDataInline = NULL"        SP
            "SendBufferSize = NULL"                  SP
            "ReceiveBufferSize = NULL"               SP
            "MinimumSendBufferSize = NULL"           SP
            "MinimumReceiveBufferSize = NULL"        SP
            "SendTimeout = NULL"                     SP
            "ReceiveTimeout = NULL"                  SP
            "TcpNoDelay = NULL"                      SP
            "]"                                      SP
            "]"
        },

        {
            L_,
            0,
            -8,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                          NL
            "    serverAddress = 1.2.3.4:5"              NL
            "    backlog = 10"                           NL
            "    timeout = (100, 0)"                     NL
            "    enableRead = true"                      NL
            "    allowHalfOpenConnections = false"       NL
            "    socketOptions = ["                      NL
            "        DebugFlag = NULL"                   NL
            "        AllowBroadcasting = NULL"           NL
            "        ReuseAddress = 0"                   NL
            "        KeepAlive = NULL"                   NL
            "        BypassNormalRouting = NULL"         NL
            "        Linger = NULL"                      NL
            "        LeaveOutOfBandDataInline = NULL"    NL
            "        SendBufferSize = NULL"              NL
            "        ReceiveBufferSize = NULL"           NL
            "        MinimumSendBufferSize = NULL"       NL
            "        MinimumReceiveBufferSize = NULL"    NL
            "        SendTimeout = NULL"                 NL
            "        ReceiveTimeout = NULL"              NL
            "        TcpNoDelay = NULL"                  NL
            "    ]"                                      NL
            "]"                                          NL
        },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        {
            L_,
            3,
            0,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                      NL
            "serverAddress = 1.2.3.4:5"              NL
            "backlog = 10"                           NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "socketOptions = ["                      NL
            "DebugFlag = NULL"                       NL
            "AllowBroadcasting = NULL"               NL
            "ReuseAddress = 0"                       NL
            "KeepAlive = NULL"                       NL
            "BypassNormalRouting = NULL"             NL
            "Linger = NULL"                          NL
            "LeaveOutOfBandDataInline = NULL"        NL
            "SendBufferSize = NULL"                  NL
            "ReceiveBufferSize = NULL"               NL
            "MinimumSendBufferSize = NULL"           NL
            "MinimumReceiveBufferSize = NULL"        NL
            "SendTimeout = NULL"                     NL
            "ReceiveTimeout = NULL"                  NL
            "TcpNoDelay = NULL"                      NL
            "]"                                      NL
            "]"                                      NL
        },

        {
            L_,
            3,
            2,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "      ["                                        NL
            "        serverAddress = 1.2.3.4:5"              NL
            "        backlog = 10"                           NL
            "        timeout = (100, 0)"                     NL
            "        enableRead = true"                      NL
            "        allowHalfOpenConnections = false"       NL
            "        socketOptions = ["                      NL
            "          DebugFlag = NULL"                     NL
            "          AllowBroadcasting = NULL"             NL
            "          ReuseAddress = 0"                     NL
            "          KeepAlive = NULL"                     NL
            "          BypassNormalRouting = NULL"           NL
            "          Linger = NULL"                        NL
            "          LeaveOutOfBandDataInline = NULL"      NL
            "          SendBufferSize = NULL"                NL
            "          ReceiveBufferSize = NULL"             NL
            "          MinimumSendBufferSize = NULL"         NL
            "          MinimumReceiveBufferSize = NULL"      NL
            "          SendTimeout = NULL"                   NL
            "          ReceiveTimeout = NULL"                NL
            "          TcpNoDelay = NULL"                    NL
            "        ]"                                      NL
            "      ]"                                        NL
        },

        {
            L_,
            3,
            -2,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "      ["                                SP
            "serverAddress = 1.2.3.4:5"              SP
            "backlog = 10"                           SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "socketOptions = ["                      SP
            "DebugFlag = NULL"                       SP
            "AllowBroadcasting = NULL"               SP
            "ReuseAddress = 0"                       SP
            "KeepAlive = NULL"                       SP
            "BypassNormalRouting = NULL"             SP
            "Linger = NULL"                          SP
            "LeaveOutOfBandDataInline = NULL"        SP
            "SendBufferSize = NULL"                  SP
            "ReceiveBufferSize = NULL"               SP
            "MinimumSendBufferSize = NULL"           SP
            "MinimumReceiveBufferSize = NULL"        SP
            "SendTimeout = NULL"                     SP
            "ReceiveTimeout = NULL"                  SP
            "TcpNoDelay = NULL"                      SP
            "]"                                      SP
            "]"
        },

        {
            L_,
            3,
            -8,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "            ["                                          NL
            "                serverAddress = 1.2.3.4:5"              NL
            "                backlog = 10"                           NL
            "                timeout = (100, 0)"                     NL
            "                enableRead = true"                      NL
            "                allowHalfOpenConnections = false"       NL
            "                socketOptions = ["                      NL
            "                    DebugFlag = NULL"                   NL
            "                    AllowBroadcasting = NULL"           NL
            "                    ReuseAddress = 0"                   NL
            "                    KeepAlive = NULL"                   NL
            "                    BypassNormalRouting = NULL"         NL
            "                    Linger = NULL"                      NL
            "                    LeaveOutOfBandDataInline = NULL"    NL
            "                    SendBufferSize = NULL"              NL
            "                    ReceiveBufferSize = NULL"           NL
            "                    MinimumSendBufferSize = NULL"       NL
            "                    MinimumReceiveBufferSize = NULL"    NL
            "                    SendTimeout = NULL"                 NL
            "                    ReceiveTimeout = NULL"              NL
            "                    TcpNoDelay = NULL"                  NL
            "                ]"                                      NL
            "            ]"                                          NL
        },

        {
            L_,
            -3,
            0,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                      NL
            "serverAddress = 1.2.3.4:5"              NL
            "backlog = 10"                           NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "socketOptions = ["                      NL
            "DebugFlag = NULL"                       NL
            "AllowBroadcasting = NULL"               NL
            "ReuseAddress = 0"                       NL
            "KeepAlive = NULL"                       NL
            "BypassNormalRouting = NULL"             NL
            "Linger = NULL"                          NL
            "LeaveOutOfBandDataInline = NULL"        NL
            "SendBufferSize = NULL"                  NL
            "ReceiveBufferSize = NULL"               NL
            "MinimumSendBufferSize = NULL"           NL
            "MinimumReceiveBufferSize = NULL"        NL
            "SendTimeout = NULL"                     NL
            "ReceiveTimeout = NULL"                  NL
            "TcpNoDelay = NULL"                      NL
            "]"                                      NL
            "]"                                      NL
        },

        {
            L_,
            -3,
            2,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                              NL
            "        serverAddress = 1.2.3.4:5"              NL
            "        backlog = 10"                           NL
            "        timeout = (100, 0)"                     NL
            "        enableRead = true"                      NL
            "        allowHalfOpenConnections = false"       NL
            "        socketOptions = ["                      NL
            "          DebugFlag = NULL"                     NL
            "          AllowBroadcasting = NULL"             NL
            "          ReuseAddress = 0"                     NL
            "          KeepAlive = NULL"                     NL
            "          BypassNormalRouting = NULL"           NL
            "          Linger = NULL"                        NL
            "          LeaveOutOfBandDataInline = NULL"      NL
            "          SendBufferSize = NULL"                NL
            "          ReceiveBufferSize = NULL"             NL
            "          MinimumSendBufferSize = NULL"         NL
            "          MinimumReceiveBufferSize = NULL"      NL
            "          SendTimeout = NULL"                   NL
            "          ReceiveTimeout = NULL"                NL
            "          TcpNoDelay = NULL"                    NL
            "        ]"                                      NL
            "      ]"                                        NL
        },

        {
            L_,
            -3,
            -2,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                      SP
            "serverAddress = 1.2.3.4:5"              SP
            "backlog = 10"                           SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "socketOptions = ["                      SP
            "DebugFlag = NULL"                       SP
            "AllowBroadcasting = NULL"               SP
            "ReuseAddress = 0"                       SP
            "KeepAlive = NULL"                       SP
            "BypassNormalRouting = NULL"             SP
            "Linger = NULL"                          SP
            "LeaveOutOfBandDataInline = NULL"        SP
            "SendBufferSize = NULL"                  SP
            "ReceiveBufferSize = NULL"               SP
            "MinimumSendBufferSize = NULL"           SP
            "MinimumReceiveBufferSize = NULL"        SP
            "SendTimeout = NULL"                     SP
            "ReceiveTimeout = NULL"                  SP
            "TcpNoDelay = NULL"                      SP
            "]"                                      SP
            "]"
        },

        {
            L_,
            -3,
            -8,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                                      NL
            "                serverAddress = 1.2.3.4:5"              NL
            "                backlog = 10"                           NL
            "                timeout = (100, 0)"                     NL
            "                enableRead = true"                      NL
            "                allowHalfOpenConnections = false"       NL
            "                socketOptions = ["                      NL
            "                    DebugFlag = NULL"                   NL
            "                    AllowBroadcasting = NULL"           NL
            "                    ReuseAddress = 0"                   NL
            "                    KeepAlive = NULL"                   NL
            "                    BypassNormalRouting = NULL"         NL
            "                    Linger = NULL"                      NL
            "                    LeaveOutOfBandDataInline = NULL"    NL
            "                    SendBufferSize = NULL"              NL
            "                    ReceiveBufferSize = NULL"           NL
            "                    MinimumSendBufferSize = NULL"       NL
            "                    MinimumReceiveBufferSize = NULL"    NL
            "                    SendTimeout = NULL"                 NL
            "                    ReceiveTimeout = NULL"              NL
            "                    TcpNoDelay = NULL"                  NL
            "                ]"                                      NL
            "            ]"                                          NL
        },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        {
            L_,
            2,
            3,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "      ["                                        NL
            "         serverAddress = 1.2.3.4:5"             NL
            "         backlog = 10"                          NL
            "         timeout = (100, 0)"                    NL
            "         enableRead = true"                     NL
            "         allowHalfOpenConnections = false"      NL
            "         socketOptions = ["                     NL
            "            DebugFlag = NULL"                   NL
            "            AllowBroadcasting = NULL"           NL
            "            ReuseAddress = 0"                   NL
            "            KeepAlive = NULL"                   NL
            "            BypassNormalRouting = NULL"         NL
            "            Linger = NULL"                      NL
            "            LeaveOutOfBandDataInline = NULL"    NL
            "            SendBufferSize = NULL"              NL
            "            ReceiveBufferSize = NULL"           NL
            "            MinimumSendBufferSize = NULL"       NL
            "            MinimumReceiveBufferSize = NULL"    NL
            "            SendTimeout = NULL"                 NL
            "            ReceiveTimeout = NULL"              NL
            "            TcpNoDelay = NULL"                  NL
            "         ]"                                     NL
            "      ]"                                        NL
        },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        {
            L_,
            -8,
            -8,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "["                                          NL
            "    serverAddress = 1.2.3.4:5"              NL
            "    backlog = 10"                           NL
            "    timeout = (100, 0)"                     NL
            "    enableRead = true"                      NL
            "    allowHalfOpenConnections = false"       NL
            "    socketOptions = ["                      NL
            "        DebugFlag = NULL"                   NL
            "        AllowBroadcasting = NULL"           NL
            "        ReuseAddress = 0"                   NL
            "        KeepAlive = NULL"                   NL
            "        BypassNormalRouting = NULL"         NL
            "        Linger = NULL"                      NL
            "        LeaveOutOfBandDataInline = NULL"    NL
            "        SendBufferSize = NULL"              NL
            "        ReceiveBufferSize = NULL"           NL
            "        MinimumSendBufferSize = NULL"       NL
            "        MinimumReceiveBufferSize = NULL"    NL
            "        SendTimeout = NULL"                 NL
            "        ReceiveTimeout = NULL"              NL
            "        TcpNoDelay = NULL"                  NL
            "    ]"                                      NL
            "]"                                          NL
        },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        {
            L_,
            -9,
            -9,

            IPAddr("1.2.3.4", 5),
            10,
            100,
            true,
            false,
            false,

            "[ 1.2.3.4:5 10 (100, 0) true false "
            "[ DebugFlag = NULL AllowBroadcasting = NULL "
              "ReuseAddress = 0 KeepAlive = NULL "
              "BypassNormalRouting = NULL Linger = NULL "
              "LeaveOutOfBandDataInline = NULL "
              "SendBufferSize = NULL ReceiveBufferSize = NULL "
              "MinimumSendBufferSize = NULL "
              "MinimumReceiveBufferSize = NULL "
              "SendTimeout = NULL ReceiveTimeout = NULL "
              "TcpNoDelay = NULL ] ]"
        },

#undef NL
#undef SP

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;

                const IPAddr      ADDR   = DATA[ti].d_serverAddress;
                int               NA     = DATA[ti].d_backlog;
                int               T      = DATA[ti].d_timeout;
                bool              ER     = DATA[ti].d_enableRead;
                bool              CM     = DATA[ti].d_allowHalfOpenConnections;
                bool              RA     = DATA[ti].d_reuseAddr;

                SockOpts          SO;
                SO.setReuseAddress(RA);

                bslma::TestAllocator sa("scratchAllocator", veryVeryVerbose);

                const bsl::string EXP(DATA[ti].d_expected_p, &sa);

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                mX.setServerAddress(ADDR);
                mX.setBacklog(NA);
                mX.setTimeout(TimeInterval(T, 0));
                mX.setEnableRead(ER);
                mX.setAllowHalfOpenConnections(CM);
                mX.setSocketOptions(SO);

                ostringstream os(&sa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS, ASSIGNMENT, AND COPY CTOR
        //   Ensure that '==' and '!=' are the operational definition of value
        //   and the assignment operator and copy constructor copy correctly
        //   from the source object.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //:
        //:13 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //:14 The signature and return type of the assignment operator is
        //:   standard.
        //:
        //:15 The assignment operation returns a reference to the target
        //:   object (i.e., '*this').
        //:
        //:16 The value of the source object is not modified by either the
        //:   assignment operator or copy constructor.
        //:
        //:17 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:18 The copy constructor changes the value of any modifiable
        //:   target object to that of any source object.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:
        //: 2 Use the respective addresses of 'operator=' and the copy
        //:   constructor to initialize function pointers having the
        //:   appropriate signatures and return types for the two methods.
        //:
        //: 3 Create three objects, D, A, and B, having respectively the
        //:   default value (D) and two unique values.
        //:
        //: 4 Create a new object, mX, copy constructed from object D.
        //:
        //: 5 Confirm that X equals the value D and does not equal A or B.
        //:
        //: 6 Change one attribute in X at a time to its corresponding A value
        //:   and confirm that X is not equal to D or A or B at each step.
        //:   When all the attributes are done X should equal A.
        //:
        //: 7 Repeat step 4-6 by creating mX initialized to A and changing
        //:   each attribute to its B value.
        //:
        //: 8 Repeat steps 4-7 by creating mX using the assignment operator
        //:   instead of the copy constructor.
        //
        // Testing:
        //   bool operator==(const btlmt::ListenOptions& lhs, rhs);
        //   bool operator!=(const btlmt::ListenOptions& lhs, rhs);
        //   ListenOptions& operator=(const btlmt::ListenOptions& rhs);
        //   ListenOptions(const btlmt::ListenOptions& rhs);
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace btlmt;

            typedef bool (*operatorPtr)(const Obj&, const Obj&);
            typedef Obj& (Obj::*operatorAssignment)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr        operatorEq = operator==;
            operatorPtr        operatorNe = operator!=;
            operatorAssignment operatorAssign = &Obj::operator=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
            (void)operatorAssign;
        }

        // 'A' values

        const IPAddr           A1("127.0.0.1", 12345);
        const int              A2 = 10;
        const TimeInterval     A3 = TimeInterval(5);
        const bool             A4 = false;
        const bool             A5 = true;
        SockOpts               A6;  A6.setReuseAddress(true);

        // 'B' values

        const IPAddr           B1("1.1.1.1", 11765);
        const int              B2 = 25;
        const TimeInterval     B3 = TimeInterval(10);
        const bool             B4 = true;
        const bool             B5 = false;
        SockOpts               B6;  B6.setKeepAlive(true);

        {
            Obj mD;  const Obj& D = mD;

            Obj mA;  const Obj& A = mA;
            mA.setServerAddress(A1);
            mA.setBacklog(A2);
            mA.setTimeout(A3);
            mA.setEnableRead(A4);
            mA.setAllowHalfOpenConnections(A5);
            mA.setSocketOptions(A6);

            Obj mB;  const Obj& B = mB;
            mB.setServerAddress(B1);
            mB.setBacklog(B2);
            mB.setTimeout(B3);
            mB.setEnableRead(B4);
            mB.setAllowHalfOpenConnections(B5);
            mB.setSocketOptions(B6);

            LOOP2_ASSERT(D, A, D != A);
            LOOP2_ASSERT(D, B, D != B);
            LOOP2_ASSERT(A, B, A != B);

            {
                Obj mX(D);  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setServerAddress(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBacklog(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTimeout(A3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setEnableRead(A4);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowHalfOpenConnections(A5);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(A6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A == X);
                LOOP2_ASSERT(B, X, B != X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }

            {
                Obj mX(A);  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A == X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setServerAddress(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBacklog(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTimeout(B3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setEnableRead(B4);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowHalfOpenConnections(B5);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(B6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B == X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }

            {
                Obj mX = D;  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setServerAddress(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBacklog(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTimeout(A3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setEnableRead(A4);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowHalfOpenConnections(A5);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(A6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A == X);
                LOOP2_ASSERT(B, X, B != X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }

            {
                Obj mX = A;  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A == X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setServerAddress(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBacklog(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTimeout(B3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setEnableRead(B4);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowHalfOpenConnections(B5);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(B6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B == X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }
        }

        ASSERT(0 == da.numBlocksTotal());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MANIPULATORS & ACCESSORS
        //   Ensure that we can use the manipulators to put that object into
        //   any state relevant for testing and use the accessors to confirm
        //   the value set.
        //
        // Concerns:
        //: 1 Each attribute is modifiable independently.
        //:
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 3 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //
        // Plan:
        //: 1 Use the default constructor to create an object 'X'.
        //:
        //: 2 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-2, 4)
        //:
        //: 3 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-3)
        //
        // Testing:
        //   void setServerAddress(const btlso::IPv4Address& value);
        //   void setBacklog(int value);
        //   void setTimeout(const bsls::TimeInterval& value);
        //   void setEnableRead(bool value);
        //   void setAllowHalfOpenConnections(bool value);
        //   void setSocketOptions(const btlso::SocketOptions& value);
        //   const btlso::IPv4Address& serverAddress() const;
        //   int backlog() const;
        //   const bdlb::NullableValue<bsls::TimeInterval>& timeout() const;
        //   bool enableRead() const;
        //   bool AllowHalfOpenConnections() const;
        //   const btlso::SocketOptions& socketOptions() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MANIPULATORS & ACCESSORS" << endl
                          << "========================" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const IPAddr           D1;
        const int              D2 = 1;
        const TimeInterval     D3 = TimeInterval();
        const bool             D4 = true;
        const bool             D5 = false;
        const SockOpts         D6;

        // 'A' values

        const IPAddr           A1("127.0.0.1", 12345);
        const int              A2 = 10;
        const TimeInterval     A3 = TimeInterval(5);
        const bool             A4 = false;
        const bool             A5 = true;
        SockOpts               A6;  A6.setReuseAddress(true);

        // 'B' values

        const IPAddr           B1("1.1.1.1", 11765);
        const int              B2 = 25;
        const TimeInterval     B3 = TimeInterval(10);
        const bool             B4 = true;
        const bool  B5 = false;
        SockOpts               B6;  B6.setKeepAlive(true);

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // ---------------
        // 'serverAddress'
        // ---------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setServerAddress(D1);
            ASSERT(D1 == X.serverAddress());

            mX.setServerAddress(A1);
            ASSERT(A1 == X.serverAddress());

            mX.setServerAddress(B1);
            ASSERT(B1 == X.serverAddress());

            mX.setServerAddress(D1);
            ASSERT(D1 == X.serverAddress());
        }

        // ---------
        // 'backlog'
        // ---------
        {
            Obj mX;  const Obj& X = mX;

            mX.setBacklog(D2);
            ASSERT(D2 == X.backlog());

            mX.setBacklog(A2);
            ASSERT(A2 == X.backlog());

            mX.setBacklog(B2);
            ASSERT(B2 == X.backlog());

            mX.setBacklog(D2);
            ASSERT(D2 == X.backlog());
        }

        // ---------
        // 'timeout'
        // ---------
        {
            Obj mX;  const Obj& X = mX;

            mX.setTimeout(D3);
            ASSERT(D3 == X.timeout().value());

            mX.setTimeout(A3);
            ASSERT(A3 == X.timeout().value());

            mX.setTimeout(B3);
            ASSERT(B3 == X.timeout().value());

            mX.setTimeout(D3);
            ASSERT(D3 == X.timeout().value());
        }

        // ------------
        // 'enableRead'
        // ------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setEnableRead(D4);
            ASSERT(D4 == X.enableRead());

            mX.setEnableRead(A4);
            ASSERT(A4 == X.enableRead());

            mX.setEnableRead(B4);
            ASSERT(B4 == X.enableRead());

            mX.setEnableRead(D4);
            ASSERT(D4 == X.enableRead());
        }

        // --------------------------
        // 'allowHalfOpenConnections'
        // --------------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setAllowHalfOpenConnections(D5);
            ASSERT(D5 == X.allowHalfOpenConnections());

            mX.setAllowHalfOpenConnections(A5);
            ASSERT(A5 == X.allowHalfOpenConnections());

            mX.setAllowHalfOpenConnections(B5);
            ASSERT(B5 == X.allowHalfOpenConnections());

            mX.setAllowHalfOpenConnections(D5);
            ASSERT(D5 == X.allowHalfOpenConnections());
        }

        // ---------------
        // 'socketOptions'
        // ---------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setSocketOptions(D6);
            ASSERT(D6 == X.socketOptions());

            mX.setSocketOptions(A6);
            ASSERT(A6 == X.socketOptions());

            mX.setSocketOptions(B6);
            ASSERT(B6 == X.socketOptions());

            mX.setSocketOptions(D6);
            ASSERT(D6 == X.socketOptions());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            Obj mX;  const Obj& X = mX;

            mX.setServerAddress(A1);
            mX.setBacklog(A2);
            mX.setTimeout(A3);
            mX.setEnableRead(A4);
            mX.setAllowHalfOpenConnections(A5);
            mX.setSocketOptions(A6);

            ASSERT(A1 == X.serverAddress());
            ASSERT(A2 == X.backlog());
            ASSERT(A3 == X.timeout().value());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setServerAddress(B1);
            ASSERT(B1 == X.serverAddress());
            ASSERT(A2 == X.backlog());
            ASSERT(A3 == X.timeout().value());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            mX.setBacklog(B2);
            ASSERT(B1 == X.serverAddress());
            ASSERT(B2 == X.backlog());
            ASSERT(A3 == X.timeout().value());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            mX.setTimeout(B3);
            ASSERT(B1 == X.serverAddress());
            ASSERT(B2 == X.backlog());
            ASSERT(B3 == X.timeout().value());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            mX.setEnableRead(B4);
            ASSERT(B1 == X.serverAddress());
            ASSERT(B2 == X.backlog());
            ASSERT(B3 == X.timeout().value());
            ASSERT(B4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            mX.setAllowHalfOpenConnections(B5);
            ASSERT(B1 == X.serverAddress());
            ASSERT(B2 == X.backlog());
            ASSERT(B3 == X.timeout().value());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.socketOptions());

            mX.setSocketOptions(B6);
            ASSERT(B1 == X.serverAddress());
            ASSERT(B2 == X.backlog());
            ASSERT(B3 == X.timeout().value());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(B6 == X.socketOptions());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tbacklog" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setBacklog(-1));
                ASSERT_SAFE_FAIL(obj.setBacklog(0));
                ASSERT_SAFE_PASS(obj.setBacklog(1));
                ASSERT_SAFE_PASS(obj.setBacklog(2));
            }
        }

        ASSERT(0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value) and use the
        //   destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //
        // Plan:
        //: 1 Use the default constructor to create an object 'X'.
        //:
        //: 2 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //
        // Testing:
        //   btlmt::ListenOptions();
        //   ~btlmt::ListenOptions();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR & DTOR" << endl
                          << "===================" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

        {
            Obj mX;  const Obj& X = mX;

            if (verbose) cout << "Verify the object's attribute values."
                              << endl;

            // ---------------------------------------------
            // Verify the object's default attribute values.
            // ---------------------------------------------

            ASSERT(IPAddr()                == X.serverAddress());
            ASSERT(1                       == X.backlog());
            ASSERT(X.timeout().isNull());
            ASSERT(true                    == X.enableRead());
            ASSERT(false == X.allowHalfOpenConnections());
            ASSERT(SockOpts()              == X.socketOptions());
        }

        ASSERT(0 == da.numBlocksTotal());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).       { w:D         }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D     }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A     }
        //: 4 Create an object 'z' (copy from 'y').      { w:D x:A z:A }
        //: 5 Reset 'z' to 'D' (the default value).      { w:D x:A z:D }
        //: 6 Assign 'w' from 'x'.                       { w:A x:A z:D }
        //: 7 Assign 'x' from 'x' (aliasing).            { w:A x:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Attribute Types

        typedef IPAddr           T1;
        typedef int              T2;
        typedef TimeInterval     T3;
        typedef bool             T4;
        typedef bool             T5;
        typedef SockOpts         T6;

        // Attribute 1 Values: 'serverAddress'

        const T1 D1;                               // default value
        const T1 A1("127.0.0.1", 12345);

        // Attribute 2 Values: 'backlog'

        const T2 D2 = 1;                           // default value
        const T2 A2 = 10;

        // Attribute 3 Values: 'timeout'

        const T3 D3 = TimeInterval();              // default value
        const T3 A3 = TimeInterval(5);

        // Attribute 4 Values: 'enableRead'

        const T4 D4 = true;                        // default value
        const T4 A4 = false;

        // Attribute 5 Values: 'AllowHalfOpenConnections'

        const T5 D5 = false;     // default value
        const T5 A5 = true;

        // Attribute 6 Values: 'socketOptions'

        const T6 D6;                               // default value
        T6       A6;  A6.setReuseAddress(true);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.serverAddress());
        ASSERT(D2 == W.backlog());
        ASSERT(W.timeout().isNull());
        ASSERT(D4 == W.enableRead());
        ASSERT(D5 == W.allowHalfOpenConnections());
        ASSERT(D6 == W.socketOptions());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.serverAddress());
        ASSERT(D2 == X.backlog());
        ASSERT(X.timeout().isNull());
        ASSERT(D4 == X.enableRead());
        ASSERT(D5 == X.allowHalfOpenConnections());
        ASSERT(D6 == X.socketOptions());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setServerAddress(A1);
        mX.setBacklog(A2);
        mX.setTimeout(A3);
        mX.setEnableRead(A4);
        mX.setAllowHalfOpenConnections(A5);
        mX.setSocketOptions(A6);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.serverAddress());
        ASSERT(A2 == X.backlog());
        ASSERT(A3 == X.timeout().value());
        ASSERT(A4 == X.enableRead());
        ASSERT(A5 == X.allowHalfOpenConnections());
        ASSERT(A6 == X.socketOptions());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'z' (copy from 'x')."
                             "\t\t{ w:D x:A z:A }" << endl;

        Obj mZ(X);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.serverAddress());
        ASSERT(A2 == Z.backlog());
        ASSERT(A3 == Z.timeout().value());
        ASSERT(A4 == Z.enableRead());
        ASSERT(A5 == Z.allowHalfOpenConnections());
        ASSERT(A6 == Z.socketOptions());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Reset 'z' to 'D' (the default value)."
                             "\t\t{ w:D x:A z:D }" << endl;

        mZ.reset();

        ASSERT(D1 == Z.serverAddress());
        ASSERT(D2 == Z.backlog());
        ASSERT(Z.timeout().isNull());
        ASSERT(D4 == Z.enableRead());
        ASSERT(D5 == Z.allowHalfOpenConnections());
        ASSERT(D6 == Z.socketOptions());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.serverAddress());
        ASSERT(A2 == W.backlog());
        ASSERT(A3 == W.timeout().value());
        ASSERT(A4 == W.enableRead());
        ASSERT(A5 == W.allowHalfOpenConnections());
        ASSERT(A6 == W.socketOptions());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n \7. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:A x:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.serverAddress());
        ASSERT(A2 == X.backlog());
        ASSERT(A3 == X.timeout().value());
        ASSERT(A4 == X.enableRead());
        ASSERT(A5 == X.allowHalfOpenConnections());
        ASSERT(A6 == X.socketOptions());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
