// btlmt_connectoptions.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_connectoptions.h>

#include <btlso_inetstreamsocket.h>
#include <btlso_sockethandle.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

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
// [ 2] btlmt::ConnectOptions();
// [ 4] btlmt::ConnectOptions(const btlmt::ConnectOptions& original);
// [ 2] ~btlmt::ConnectOptions();
//
// MANIPULATORS
// [ 4] ConnectOptions& operator=(const btlmt::ConnectOptions& rhs);
// [ 4] void reset();
// [ 3] void setServerEndpoint(const btlso::IPv4Address& value);
// [ 3] void setServerEndpoint(const btlso::Endpoint& value);
// [ 3] void setNumAttempts(int value);
// [ 3] void setTimeout(const bsls::TimeInterval& value);
// [ 3] void setEnableRead(bool value);
// [ 3] void setAllowHalfOpenConnections(bool value);
// [ 3] void setResolutionMode(btlmt::ResolutionMode::Enum value);
// [ 3] void setSocketOptions(const btlso::SocketOptions& value);
// [ 3] void setLocalAddress(const btlso::IPv4Address& value);
// [ 3] void setSocketPtr(ManagedPtr<StreamSocket<IPv4Address> > *value);
// [ 6] void swap(ConnectOptions& other);
//
// ACCESSORS
// [ 5] ostream& print(ostream& s, int l, int spl);
// [ 3] const ServerEndpoint& serverEndpoint() const;
// [ 3] int numAttempts() const;
// [ 3] const bsls::TimeInterval& timeout() const;
// [ 3] bool enableRead() const;
// [ 3] bool allowHalfOpenConnections() const;
// [ 3] btlmt::ResolutionMode::Enum resolutionMode() const;
// [ 3] const bdlb::NullableValue<SocketOptions>& socketOptions() const;
// [ 3] const bdlb::NullableValue<IPv4Address>& localAddress() const;
// [ 3] const ManagedPtr<StreamSocket<IPv4Address> > *socketPtr() const;
//
// FREE OPERATORS
// [ 4] bool operator==(const btlmt::ConnectOptions& lhs, rhs);
// [ 4] bool operator!=(const btlmt::ConnectOptions& lhs, rhs);
// [ 5] operator<<(ostream& s, const btlmt::ConnectOptions& d);
// [ 6] void swap(ConnectOptions& a, ConnectOptions& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
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

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btlmt::ConnectOptions                     Obj;
typedef btlso::Endpoint                           Endpoint;
typedef btlso::IPv4Address                        IPAddr;
typedef bsls::TimeInterval                        TimeInterval;
typedef btlmt::ConnectOptions::ServerEndpoint     ServerEndpoint;
typedef btlmt::ResolutionMode                     ResMode;
typedef btlso::SocketOptions                      SockOpts;
typedef bdlb::NullableValue<SockOpts>             NullableSockOpts;
typedef bdlb::NullableValue<IPAddr>               NullableIPAddr;
typedef btlso::InetStreamSocket<IPAddr>           InetStreamSock;
typedef btlso::StreamSocket<IPAddr>               StreamSock;
typedef bslma::ManagedPtr<StreamSock>             StreamSockMP;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

int connect(btlso::SocketHandle::Handle  *handle,
            const Obj&                    options)
    // Connect to the server based on the specified connect 'options'.  On
    // success, load into the specified 'handle' the handle for the socket
    // created for the newly established connection.  Return 0 on success
    // and a non-zero value otherewise.
{
    return 0;
}

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
      case 7: {
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
///Example 1: Establishing a connection to a server
/// - - - - - - - - - - - - - - - - - - - - - - - -
// This component allows specifying configuration options for creating
// sockets that connect to servers.  Consider that a client wants to establish
// a connection to a server on a remote machine.  The following example will
// show how to provide configuration options for doing so.
//
// We will assume that we have a 'connect' method that accepts a
// 'btlmt::ConnectOptions' object as an argument, connects to the server
// address in that object, and provides the socket handle of that socket.  The
// signature of that function is provided below but for brevity the
// implementation is elided (note that in practice clients would likely use a
// higher level component like 'btlmt_sessionpool' or 'btlmt_channelpool' for
// connecting to remote servers):
//..
//..
// First, we will default-construct the connect options object:
//..
    btlmt::ConnectOptions options;
//
    ASSERT(btlmt::ConnectOptions::ServerEndpoint() ==
                                               options.serverEndpoint());
    ASSERT(1                                == options.numAttempts());
    ASSERT(bsls::TimeInterval()             == options.timeout());
    ASSERT(true                             == options.enableRead());
    ASSERT(btlmt::ResolutionMode::e_RESOLVE_ONCE   ==
                                               options.resolutionMode());
    ASSERT(false                        == options.allowHalfOpenConnections());
    ASSERT(options.socketOptions().isNull());
    ASSERT(options.localAddress().isNull());
    ASSERT(!options.socketPtr());
//..
// Next, we specify the server address to connect to:
//..
    btlso::Endpoint address("www.bloomberg.com", 80);
//
    options.setServerEndpoint(address);
    ASSERT(address == options.serverEndpoint().the<btlso::Endpoint>());
//..
// Then, we will specify the number of attempts that the connecting method
// make before reporting an unsuccessful connection:
//..
    const int NUM_ATTEMPTS = 10;
    options.setNumAttempts(NUM_ATTEMPTS);
    ASSERT(NUM_ATTEMPTS == options.numAttempts());
//..
// Next, the time to wait for a connect attempt to succeed is specified.  If a
// connect attempt does not succeed within that time frame the 'connect'
// method can invoke the user callback with a timeout status.
//..
    const bsls::TimeInterval TIMEOUT(2);
    options.setTimeout(TIMEOUT);
    ASSERT(TIMEOUT == options.timeout());
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
// Now, we specify certain socket options for the connecting socket such as
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
    btlso::SocketHandle::Handle handle;
    const int rc = connect(&handle, options);
    ASSERT(0 == rc);
//
   // Start reading and writing to the connection using 'handle'
   // . . .
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address held by both objects is
        //:   unchanged.
        //:
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures and return types.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 5)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-5)
        //:
        //:     1 The value is unchanged.  (C-5)
        //:
        //:     2 The allocator address held by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', and having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator address held by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-6)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg_SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-6)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-6)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
        //
        // Testing:
        //   void swap(baltzo::LocalTimeDescriptor& other);
        //   void swap(baltzo::LocalTimeDescriptor& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        // 'D' values: These are the default-constructed values.

        const IPAddr           D1A;
        SockOpts               D7A;
        IPAddr                 D8A;

        const ServerEndpoint   D1(D1A);
        const NullableSockOpts D7(D7A);
        const NullableIPAddr   D8(D8A);
        StreamSockMP           D9;

        // 'A' values

        const IPAddr           A1A("127.0.0.1", 12345);
        SockOpts               A7A;  A7A.setReuseAddress(true);
        IPAddr                 A8A;  A8A.setPortNumber(12345);
        InetStreamSock         ss(0);

        const ServerEndpoint   A1(A1A);
        const int              A2 = 10;
        const TimeInterval     A3 = TimeInterval(5);
        const bool             A4 = false;
        const bool             A5 = true;
        const ResMode::Enum    A6 = ResMode::e_RESOLVE_EACH_TIME;
        NullableSockOpts       A7(A7A);
        const NullableIPAddr   A8(A8A);
        StreamSockMP           A9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        // 'B' values

        const Endpoint         B1A("www.bloomberg.com", 12345);
        SockOpts               B7A;
        B7A.setKeepAlive(true);
        IPAddr                 B8A;
        B8A.setPortNumber(98765);

        ServerEndpoint         B1(B1A);
        const int              B2 = 25;
        const TimeInterval     B3 = TimeInterval(10);
        const bool             B4 = true;
        const bool  B5 = false;
        const ResMode::Enum    B6 = ResMode::e_RESOLVE_ONCE;
        NullableSockOpts       B7(B7A);
        const NullableIPAddr   B8(B8A);
        StreamSockMP           B9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        Obj mD;  const Obj& D = mD;

        Obj mA;  const Obj& A = mA;
        mA.setServerEndpoint(A1A);
        mA.setNumAttempts(A2);
        mA.setTimeout(A3);
        mA.setEnableRead(A4);
        mA.setAllowHalfOpenConnections(A5);
        mA.setResolutionMode(A6);
        mA.setSocketOptions(A7A);
        mA.setLocalAddress(A8A);
        mA.setSocketPtr(&A9);

        Obj mB;  const Obj& B = mB;
        mB.setServerEndpoint(B1A);
        mB.setNumAttempts(B2);
        mB.setTimeout(B3);
        mB.setEnableRead(B4);
        mB.setAllowHalfOpenConnections(B5);
        mB.setResolutionMode(B6);
        mB.setSocketOptions(B7A);
        mB.setLocalAddress(B8A);
        mB.setSocketPtr(&B9);

        LOOP2_ASSERT(D, A, D != A);
        LOOP2_ASSERT(D, B, D != B);
        LOOP2_ASSERT(A, B, A != B);

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mW(A, &oa);  const Obj& W = mW;
            const Obj XX(W, &scratch);

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP2_ASSERT(XX, W, XX == W);
                ASSERT(oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP2_ASSERT(XX, W, XX == W);
                ASSERT(oam.isTotalSame());
            }

            {
                bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(B, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP2_ASSERT(YY, X, YY == X);
                    LOOP2_ASSERT(XX, Y, XX == Y);
                    ASSERT(oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP2_ASSERT(XX, X, XX == X);
                    LOOP2_ASSERT(YY, Y, YY == Y);
                    ASSERT(oam.isTotalSame());
                }
            }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mA.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mA, mZ));
            }
        }
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
        //   operator<<(ostream& s, const btlmt::ConnectOptions& d);
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
            int              d_numAttempts;
            int              d_timeout;
            bool             d_enableRead;
            bool             d_allowHalfOpenConnections;
            ResMode::Enum    d_resMode;
            bool             d_reuseAddr;
            IPAddr           d_localAddress;
            void            *d_socketPtr;
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                      NL
            "serverEndpoint = 1.2.3.4:5"             NL
            "numAttempts = 10"                       NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "resolutionMode = 0"                     NL
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  NL
            "localAddress = 1.1.1.1:1"               NL
            "socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                       NL
            " serverEndpoint = 1.2.3.4:5"             NL
            " numAttempts = 10"                       NL
            " timeout = (100, 0)"                     NL
            " enableRead = true"                      NL
            " allowHalfOpenConnections = false"       NL
            " resolutionMode = 0"                     NL
            " socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"   NL
            " localAddress = 1.1.1.1:1"               NL
            " socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                      SP
            "serverEndpoint = 1.2.3.4:5"             SP
            "numAttempts = 10"                       SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "resolutionMode = 0"                     SP
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  SP
            "localAddress = 1.1.1.1:1"               SP
            "socketPtr = 0x12345678"                 SP
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                          NL
            "    serverEndpoint = 1.2.3.4:5"             NL
            "    numAttempts = 10"                       NL
            "    timeout = (100, 0)"                     NL
            "    enableRead = true"                      NL
            "    allowHalfOpenConnections = false"       NL
            "    resolutionMode = 0"                     NL
            "    socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"      NL
            "    localAddress = 1.1.1.1:1"               NL
            "    socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                      NL
            "serverEndpoint = 1.2.3.4:5"             NL
            "numAttempts = 10"                       NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "resolutionMode = 0"                     NL
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  NL
            "localAddress = 1.1.1.1:1"               NL
            "socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "      ["                                        NL
            "        serverEndpoint = 1.2.3.4:5"             NL
            "        numAttempts = 10"                       NL
            "        timeout = (100, 0)"                     NL
            "        enableRead = true"                      NL
            "        allowHalfOpenConnections = false"       NL
            "        resolutionMode = 0"                     NL
            "        socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]"    NL
            "        localAddress = 1.1.1.1:1"               NL
            "        socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "      ["                                SP
            "serverEndpoint = 1.2.3.4:5"             SP
            "numAttempts = 10"                       SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "resolutionMode = 0"                     SP
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  SP
            "localAddress = 1.1.1.1:1"               SP
            "socketPtr = 0x12345678"                 SP
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "            ["                                          NL
            "                serverEndpoint = 1.2.3.4:5"             NL
            "                numAttempts = 10"                       NL
            "                timeout = (100, 0)"                     NL
            "                enableRead = true"                      NL
            "                allowHalfOpenConnections = false"       NL
            "                resolutionMode = 0"                     NL
            "                socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]"            NL
            "                localAddress = 1.1.1.1:1"               NL
            "                socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                      NL
            "serverEndpoint = 1.2.3.4:5"             NL
            "numAttempts = 10"                       NL
            "timeout = (100, 0)"                     NL
            "enableRead = true"                      NL
            "allowHalfOpenConnections = false"       NL
            "resolutionMode = 0"                     NL
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  NL
            "localAddress = 1.1.1.1:1"               NL
            "socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                              NL
            "        serverEndpoint = 1.2.3.4:5"             NL
            "        numAttempts = 10"                       NL
            "        timeout = (100, 0)"                     NL
            "        enableRead = true"                      NL
            "        allowHalfOpenConnections = false"       NL
            "        resolutionMode = 0"                     NL
            "        socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]"    NL
            "        localAddress = 1.1.1.1:1"               NL
            "        socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                      SP
            "serverEndpoint = 1.2.3.4:5"             SP
            "numAttempts = 10"                       SP
            "timeout = (100, 0)"                     SP
            "enableRead = true"                      SP
            "allowHalfOpenConnections = false"       SP
            "resolutionMode = 0"                     SP
            "socketOptions = [ DebugFlag = NULL AllowBroadcasting = NULL "
                              "ReuseAddress = 0 KeepAlive = NULL "
                              "BypassNormalRouting = NULL Linger = NULL "
                              "LeaveOutOfBandDataInline = NULL "
                              "SendBufferSize = NULL ReceiveBufferSize = NULL "
                              "MinimumSendBufferSize = NULL "
                              "MinimumReceiveBufferSize = NULL "
                              "SendTimeout = NULL ReceiveTimeout = NULL "
                              "TcpNoDelay = NULL ]"  SP
            "localAddress = 1.1.1.1:1"               SP
            "socketPtr = 0x12345678"                 SP
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                                      NL
            "                serverEndpoint = 1.2.3.4:5"             NL
            "                numAttempts = 10"                       NL
            "                timeout = (100, 0)"                     NL
            "                enableRead = true"                      NL
            "                allowHalfOpenConnections = false"       NL
            "                resolutionMode = 0"                     NL
            "                socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]"            NL
            "                localAddress = 1.1.1.1:1"               NL
            "                socketPtr = 0x12345678"                 NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "      ["                                         NL
            "         serverEndpoint = 1.2.3.4:5"             NL
            "         numAttempts = 10"                       NL
            "         timeout = (100, 0)"                     NL
            "         enableRead = true"                      NL
            "         allowHalfOpenConnections = false"       NL
            "         resolutionMode = 0"                     NL
            "         socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]"     NL
            "         localAddress = 1.1.1.1:1"               NL
            "         socketPtr = 0x12345678"                 NL
            "      ]"                                         NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "["                                           NL
            "    serverEndpoint = 1.2.3.4:5"              NL
            "    numAttempts = 10"                        NL
            "    timeout = (100, 0)"                      NL
            "    enableRead = true"                       NL
            "    allowHalfOpenConnections = false"        NL
            "    resolutionMode = 0"                      NL
            "    socketOptions = [ DebugFlag = NULL "
                                    "AllowBroadcasting = NULL "
                                    "ReuseAddress = 0 KeepAlive = NULL "
                                    "BypassNormalRouting = NULL Linger = NULL "
                                    "LeaveOutOfBandDataInline = NULL "
                                    "SendBufferSize = NULL "
                                    "ReceiveBufferSize = NULL "
                                    "MinimumSendBufferSize = NULL "
                                    "MinimumReceiveBufferSize = NULL "
                                    "SendTimeout = NULL ReceiveTimeout = NULL "
                                    "TcpNoDelay = NULL ]" NL
            "    localAddress = 1.1.1.1:1"                NL
            "    socketPtr = 0x12345678"                  NL
            "]"                                           NL
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
            ResMode::e_RESOLVE_ONCE,
            false,
            IPAddr("1.1.1.1", 1),
            (void *) 0x12345678,

            "[ 1.2.3.4:5 10 (100, 0) true false 0 "
            "[ DebugFlag = NULL AllowBroadcasting = NULL "
              "ReuseAddress = 0 KeepAlive = NULL "
              "BypassNormalRouting = NULL Linger = NULL "
              "LeaveOutOfBandDataInline = NULL "
              "SendBufferSize = NULL ReceiveBufferSize = NULL "
              "MinimumSendBufferSize = NULL "
              "MinimumReceiveBufferSize = NULL "
              "SendTimeout = NULL ReceiveTimeout = NULL "
              "TcpNoDelay = NULL ] "
            "1.1.1.1:1 0x12345678 ]"
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
                int               NA     = DATA[ti].d_numAttempts;
                int               T      = DATA[ti].d_timeout;
                bool              ER     = DATA[ti].d_enableRead;
                bool              CM     = DATA[ti].d_allowHalfOpenConnections;
                ResMode::Enum     RM     = DATA[ti].d_resMode;
                bool              RA     = DATA[ti].d_reuseAddr;
                IPAddr            LA     = DATA[ti].d_localAddress;
                void             *SP     = DATA[ti].d_socketPtr;

                SockOpts          SO;
                SO.setReuseAddress(RA);

                StreamSockMP      SSMP((StreamSock *) SP,
                                       0,
                                       &bslma::ManagedPtrUtil::noOpDeleter);

                bslma::TestAllocator sa("scratchAllocator", veryVeryVerbose);

                const bsl::string EXP(DATA[ti].d_expected_p, &sa);

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                mX.setServerEndpoint(ADDR);
                mX.setNumAttempts(NA);
                mX.setTimeout(TimeInterval(T, 0));
                mX.setEnableRead(ER);
                mX.setAllowHalfOpenConnections(CM);
                mX.setResolutionMode(RM);
                mX.setSocketOptions(SO);
                mX.setLocalAddress(LA);
                mX.setSocketPtr(&SSMP);

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
        //   bool operator==(const btlmt::ConnectOptions& lhs, rhs);
        //   bool operator!=(const btlmt::ConnectOptions& lhs, rhs);
        //   ConnectOptions& operator=(const btlmt::ConnectOptions& rhs);
        //   ConnectOptions(const btlmt::ConnectOptions& rhs);
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

        // 'D' values: These are the default-constructed values.

        const IPAddr           D1A;
        SockOpts               D7A;
        IPAddr                 D8A;

        const ServerEndpoint   D1(D1A);
        const NullableSockOpts D7(D7A);
        const NullableIPAddr   D8(D8A);
        StreamSockMP           D9;

        // 'A' values

        const IPAddr           A1A("127.0.0.1", 12345);
        SockOpts               A7A;  A7A.setReuseAddress(true);
        IPAddr                 A8A;  A8A.setPortNumber(12345);
        InetStreamSock         ss(0);

        const ServerEndpoint   A1(A1A);
        const int              A2 = 10;
        const TimeInterval     A3 = TimeInterval(5);
        const bool             A4 = false;
        const bool             A5 = true;
        const ResMode::Enum    A6 = ResMode::e_RESOLVE_EACH_TIME;
        NullableSockOpts       A7(A7A);
        const NullableIPAddr   A8(A8A);
        StreamSockMP           A9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        // 'B' values

        const Endpoint         B1A("www.bloomberg.com", 12345);
        SockOpts               B7A;
        B7A.setKeepAlive(true);
        IPAddr                 B8A;
        B8A.setPortNumber(98765);

        ServerEndpoint         B1(B1A);
        const int              B2 = 25;
        const TimeInterval     B3 = TimeInterval(10);
        const bool             B4 = true;
        const bool  B5 = false;
        const ResMode::Enum    B6 = ResMode::e_RESOLVE_ONCE;
        NullableSockOpts       B7(B7A);
        const NullableIPAddr   B8(B8A);
        StreamSockMP           B9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        {
            Obj mD;  const Obj& D = mD;

            Obj mA;  const Obj& A = mA;
            mA.setServerEndpoint(A1A);
            mA.setNumAttempts(A2);
            mA.setTimeout(A3);
            mA.setEnableRead(A4);
            mA.setAllowHalfOpenConnections(A5);
            mA.setResolutionMode(A6);
            mA.setSocketOptions(A7A);
            mA.setLocalAddress(A8A);
            mA.setSocketPtr(&A9);

            Obj mB;  const Obj& B = mB;
            mB.setServerEndpoint(B1A);
            mB.setNumAttempts(B2);
            mB.setTimeout(B3);
            mB.setEnableRead(B4);
            mB.setAllowHalfOpenConnections(B5);
            mB.setResolutionMode(B6);
            mB.setSocketOptions(B7A);
            mB.setLocalAddress(B8A);
            mB.setSocketPtr(&B9);

            LOOP2_ASSERT(D, A, D != A);
            LOOP2_ASSERT(D, B, D != B);
            LOOP2_ASSERT(A, B, A != B);

            {
                Obj mX(D);  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setServerEndpoint(A1A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setNumAttempts(A2);

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

                mX.setResolutionMode(A6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(A7A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLocalAddress(A8A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketPtr(&A9);

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

                mX.setServerEndpoint(B1A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setNumAttempts(B2);

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

                mX.setResolutionMode(B6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(B7A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLocalAddress(B8A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketPtr(&B9);

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

                mX.setServerEndpoint(A1A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setNumAttempts(A2);

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

                mX.setResolutionMode(A6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(A7A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLocalAddress(A8A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketPtr(&A9);

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

                mX.setServerEndpoint(B1A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setNumAttempts(B2);

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

                mX.setResolutionMode(B6);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketOptions(B7A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLocalAddress(B8A);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSocketPtr(&B9);

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
        //   void setServerEndpoint(const btlso::IPv4Address& value);
        //   void setServerEndpoint(const btlso::Endpoint& value);
        //   void setNumAttempts(int value);
        //   void setTimeout(const bsls::TimeInterval& value);
        //   void setEnableRead(bool value);
        //   void setAllowHalfOpenConnections(bool value);
        //   void setResolutionMode(btlmt::ResolutionMode::Enum value);
        //   void setSocketOptions(const btlso::SocketOptions& value);
        //   void setLocalAddress(const btlso::IPv4Address& value);
        //   void setSocketPtr(ManagedPtr<StreamSocket<IPv4Address> > *value);
        //   const ServerEndpoint& serverEndpoint() const;
        //   int numAttempts() const;
        //   const bsls::TimeInterval& timeout() const;
        //   bool enableRead() const;
        //   bool allowHalfOpenConnections() const;
        //   btlmt::ResolutionMode::Enum resolutionMode() const;
        //   const bdlb::NullableValue<SocketOptions>& socketOptions() const;
        //   const bdlb::NullableValue<IPv4Address>& localAddress() const;
        //   const ManagedPtr<StreamSocket<IPv4Address> > *socketPtr() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MANIPULATORS & ACCESSORS" << endl
                          << "========================" << endl;

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const IPAddr           D1A;
        SockOpts               D7A;
        IPAddr                 D8A;

        const ServerEndpoint   D1(D1A);
        const int              D2 = 1;
        const TimeInterval     D3 = TimeInterval();
        const bool             D4 = true;
        const bool  D5 = false;
        const ResMode::Enum    D6 = ResMode::e_RESOLVE_ONCE;
        const NullableSockOpts D7(D7A);
        const NullableIPAddr   D8(D8A);
        StreamSockMP           D9;

        // 'A' values

        const IPAddr           A1A("127.0.0.1", 12345);
        SockOpts               A7A;  A7A.setReuseAddress(true);
        IPAddr                 A8A;  A8A.setPortNumber(12345);
        InetStreamSock         ss(0);

        const ServerEndpoint   A1(A1A);
        const int              A2 = 10;
        const TimeInterval     A3 = TimeInterval(5);
        const bool             A4 = false;
        const bool             A5 = true;
        const ResMode::Enum    A6 = ResMode::e_RESOLVE_EACH_TIME;
        NullableSockOpts       A7(A7A);
        const NullableIPAddr   A8(A8A);
        StreamSockMP           A9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        // 'B' values

        const Endpoint         B1A("www.bloomberg.com", 12345);
        SockOpts               B7A;
        B7A.setKeepAlive(true);
        IPAddr                 B8A;
        B8A.setPortNumber(98765);

        ServerEndpoint         B1(B1A);
        const int              B2 = 25;
        const TimeInterval     B3 = TimeInterval(10);
        const bool             B4 = true;
        const bool  B5 = false;
        const ResMode::Enum    B6 = ResMode::e_RESOLVE_ONCE;
        NullableSockOpts       B7(B7A);
        const NullableIPAddr   B8(B8A);
        StreamSockMP           B9(&ss,
                                  0,
                                  &bslma::ManagedPtrUtil::noOpDeleter);

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // ----------------
        // 'serverEndpoint'
        // ----------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setServerEndpoint(D1A);
            ASSERT(D1 == X.serverEndpoint());

            mX.setServerEndpoint(A1A);
            ASSERT(A1 == X.serverEndpoint());

            mX.setServerEndpoint(B1A);
            ASSERT(B1 == X.serverEndpoint());

            mX.setServerEndpoint(D1A);
            ASSERT(D1 == X.serverEndpoint());
        }

        // -------------
        // 'numAttempts'
        // -------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setNumAttempts(D2);
            ASSERT(D2 == X.numAttempts());

            mX.setNumAttempts(A2);
            ASSERT(A2 == X.numAttempts());

            mX.setNumAttempts(B2);
            ASSERT(B2 == X.numAttempts());

            mX.setNumAttempts(D2);
            ASSERT(D2 == X.numAttempts());
        }

        // ---------
        // 'timeout'
        // ---------
        {
            Obj mX;  const Obj& X = mX;

            mX.setTimeout(D3);
            ASSERT(D3 == X.timeout());

            mX.setTimeout(A3);
            ASSERT(A3 == X.timeout());

            mX.setTimeout(B3);
            ASSERT(B3 == X.timeout());

            mX.setTimeout(D3);
            ASSERT(D3 == X.timeout());
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

        // ----------------
        // 'resolutionMode'
        // ----------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setResolutionMode(D6);
            ASSERT(D6 == X.resolutionMode());

            mX.setResolutionMode(A6);
            ASSERT(A6 == X.resolutionMode());

            mX.setResolutionMode(B6);
            ASSERT(B6 == X.resolutionMode());

            mX.setResolutionMode(D6);
            ASSERT(D6 == X.resolutionMode());
        }

        // ---------------
        // 'socketOptions'
        // ---------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setSocketOptions(D7A);
            ASSERT(D7 == X.socketOptions());

            mX.setSocketOptions(A7A);
            ASSERT(A7 == X.socketOptions());

            mX.setSocketOptions(B7A);
            ASSERT(B7 == X.socketOptions());

            mX.setSocketOptions(D7A);
            ASSERT(D7 == X.socketOptions());
        }

        // --------------
        // 'localAddress'
        // --------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setLocalAddress(D8A);
            ASSERT(D8 == X.localAddress());

            mX.setLocalAddress(A8A);
            ASSERT(A8 == X.localAddress());

            mX.setLocalAddress(B8A);
            ASSERT(B8 == X.localAddress());

            mX.setLocalAddress(D8A);
            ASSERT(D8 == X.localAddress());
        }

        // -----------
        // 'socketPtr'
        // -----------
        {
            Obj mX;  const Obj& X = mX;

            mX.setSocketPtr(&D9);
            ASSERT(&D9 == X.socketPtr());

            mX.setSocketPtr(&A9);
            ASSERT(&A9 == X.socketPtr());

            mX.setSocketPtr(&B9);
            ASSERT(&B9 == X.socketPtr());

            mX.setSocketPtr(&D9);
            ASSERT(&D9 == X.socketPtr());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            Obj mX;  const Obj& X = mX;

            mX.setServerEndpoint(A1A);
            mX.setNumAttempts(A2);
            mX.setTimeout(A3);
            mX.setEnableRead(A4);
            mX.setAllowHalfOpenConnections(A5);
            mX.setResolutionMode(A6);
            mX.setSocketOptions(A7A);
            mX.setLocalAddress(A8A);
            mX.setSocketPtr(&A9);

            ASSERT(A1 == X.serverEndpoint());
            ASSERT(A2 == X.numAttempts());
            ASSERT(A3 == X.timeout());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setServerEndpoint(B1A);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(A2 == X.numAttempts());
            ASSERT(A3 == X.timeout());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setNumAttempts(B2);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(A3 == X.timeout());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setTimeout(B3);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(A4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setEnableRead(B4);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(A5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setAllowHalfOpenConnections(B5);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(A6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setResolutionMode(B6);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(B6 == X.resolutionMode());
            ASSERT(A7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setSocketOptions(B7A);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(B6 == X.resolutionMode());
            ASSERT(B7 == X.socketOptions());
            ASSERT(A8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setLocalAddress(B8A);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(B6 == X.resolutionMode());
            ASSERT(B7 == X.socketOptions());
            ASSERT(B8 == X.localAddress());
            ASSERT(&A9 == X.socketPtr());

            mX.setSocketPtr(&B9);
            ASSERT(B1 == X.serverEndpoint());
            ASSERT(B2 == X.numAttempts());
            ASSERT(B3 == X.timeout());
            ASSERT(B4 == X.enableRead());
            ASSERT(B5 == X.allowHalfOpenConnections());
            ASSERT(B6 == X.resolutionMode());
            ASSERT(B7 == X.socketOptions());
            ASSERT(B8 == X.localAddress());
            ASSERT(&B9 == X.socketPtr());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tnumAttempts" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumAttempts(-1));
                ASSERT_SAFE_FAIL(obj.setNumAttempts(0));
                ASSERT_SAFE_PASS(obj.setNumAttempts(1));
                ASSERT_SAFE_PASS(obj.setNumAttempts(2));
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
        //   btlmt::ConnectOptions();
        //   ~btlmt::ConnectOptions();
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

            ASSERT(ServerEndpoint() == X.serverEndpoint());
            ASSERT(1                == X.numAttempts());
            ASSERT(TimeInterval()   == X.timeout());
            ASSERT(true             == X.enableRead());
            ASSERT(ResMode::e_RESOLVE_ONCE == X.resolutionMode());
            ASSERT(false == X.allowHalfOpenConnections());
            ASSERT(X.socketOptions().isNull());
            ASSERT(X.localAddress().isNull());
            ASSERT(!X.socketPtr());
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

        typedef int              T2;
        typedef TimeInterval     T3;
        typedef bool             T4;
        typedef bool  T5;
        typedef ResMode::Enum    T6;
        typedef NullableSockOpts T7;
        typedef NullableIPAddr   T8;
        typedef StreamSockMP     T9;

        // Attribute 1 Values: 'serverEndpoint'
        ServerEndpoint D1;                         // default value
        const IPAddr   A1A("127.0.0.1", 12345);
        ServerEndpoint A1(A1A);

        // Attribute 2 Values: 'numAttempts'

        const T2 D2 = 1;                           // default value
        const T2 A2 = 10;

        // Attribute 3 Values: 'timeout'

        const T3 D3 = TimeInterval();              // default value
        const T3 A3 = TimeInterval(5);

        // Attribute 4 Values: 'enableRead'

        const T4 D4 = true;                        // default value
        const T4 A4 = false;

        // Attribute 5 Values: 'allowHalfOpenConnections'

        const T5 D5 = false;     // default value
        const T5 A5 = true;

        // Attribute 6 Values: 'resolutionMode'

        const T6 D6 = ResMode::e_RESOLVE_ONCE;     // default value
        const T6 A6 = ResMode::e_RESOLVE_EACH_TIME;

        // Attribute 7 Values: 'socketOptions'

        SockOpts D7A;
        const T7 D7;                               // default value
        SockOpts A7A;
        A7A.setReuseAddress(true);
        T7 A7(A7A);

        // Attribute 8 Values: 'localAddress'

        IPAddr D8A;
        const T8 D8;                               // default value
        const IPAddr A8A("127.0.0.1", 12345);
        const T8 A8(A8A);

        // Attribute 9 Values: 'socketPtr'

        T9 D9;                                     // default value
        InetStreamSock ss(0);
        StreamSockMP   A9(&ss, 0, &bslma::ManagedPtrUtil::noOpDeleter);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.serverEndpoint());
        ASSERT(D2 == W.numAttempts());
        ASSERT(D3 == W.timeout());
        ASSERT(D4 == W.enableRead());
        ASSERT(D5 == W.allowHalfOpenConnections());
        ASSERT(D6 == W.resolutionMode());
        ASSERT(D7 == W.socketOptions());
        ASSERT(D8 == W.localAddress());
        ASSERT(0  == W.socketPtr());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.serverEndpoint());
        ASSERT(D2 == X.numAttempts());
        ASSERT(D3 == X.timeout());
        ASSERT(D4 == X.enableRead());
        ASSERT(D5 == X.allowHalfOpenConnections());
        ASSERT(D6 == X.resolutionMode());
        ASSERT(D7 == X.socketOptions());
        ASSERT(D8 == X.localAddress());
        ASSERT(0  == X.socketPtr());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setServerEndpoint(A1A);
        mX.setNumAttempts(A2);
        mX.setTimeout(A3);
        mX.setEnableRead(A4);
        mX.setAllowHalfOpenConnections(A5);
        mX.setResolutionMode(A6);
        mX.setSocketOptions(A7A);
        mX.setLocalAddress(A8A);
        mX.setSocketPtr(&A9);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.serverEndpoint());
        ASSERT(A2 == X.numAttempts());
        ASSERT(A3 == X.timeout());
        ASSERT(A4 == X.enableRead());
        ASSERT(A5 == X.allowHalfOpenConnections());
        ASSERT(A6 == X.resolutionMode());
        ASSERT(A7 == X.socketOptions());
        ASSERT(A8 == X.localAddress());
        ASSERT(&A9 == X.socketPtr());

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

        ASSERT(A1 == Z.serverEndpoint());
        ASSERT(A2 == Z.numAttempts());
        ASSERT(A3 == Z.timeout());
        ASSERT(A4 == Z.enableRead());
        ASSERT(A5 == Z.allowHalfOpenConnections());
        ASSERT(A6 == Z.resolutionMode());
        ASSERT(A7 == Z.socketOptions());
        ASSERT(A8 == Z.localAddress());
        ASSERT(&A9 == Z.socketPtr());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Reset 'z' to 'D' (the default value)."
                             "\t\t{ w:D x:A z:D }" << endl;

        mZ.reset();

        ASSERT(D1 == Z.serverEndpoint());
        ASSERT(D2 == Z.numAttempts());
        ASSERT(D3 == Z.timeout());
        ASSERT(D4 == Z.enableRead());
        ASSERT(D5 == Z.allowHalfOpenConnections());
        ASSERT(D6 == Z.resolutionMode());
        ASSERT(D7 == Z.socketOptions());
        ASSERT(D8 == Z.localAddress());
        ASSERT(0  == Z.socketPtr());

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

        ASSERT(A1 == W.serverEndpoint());
        ASSERT(A2 == W.numAttempts());
        ASSERT(A3 == W.timeout());
        ASSERT(A4 == W.enableRead());
        ASSERT(A5 == W.allowHalfOpenConnections());
        ASSERT(A6 == W.resolutionMode());
        ASSERT(A7 == W.socketOptions());
        ASSERT(A8 == W.localAddress());
        ASSERT(&A9 == W.socketPtr());

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

        ASSERT(A1 == X.serverEndpoint());
        ASSERT(A2 == X.numAttempts());
        ASSERT(A3 == X.timeout());
        ASSERT(A4 == X.enableRead());
        ASSERT(A5 == X.allowHalfOpenConnections());
        ASSERT(A6 == X.resolutionMode());
        ASSERT(A7 == X.socketOptions());
        ASSERT(A8 == X.localAddress());
        ASSERT(&A9 == X.socketPtr());

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
