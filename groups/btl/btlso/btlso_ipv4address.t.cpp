// btlso_ipv4address.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_ipv4address.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_byteoutstream.h>
#include <bslx_byteinstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iomanip.h>     // setw()
#include <bsl_iostream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [12] static int isValidAddress(const char *address);
//
// CREATORS
// [ 2] btlso::IPv4Address();
// [11] btlso::IPv4Address(const char *address, int portNumber);
// [11] btlso::IPv4Address(int address, int portNumber);
// [ 7] btlso::IPv4Address(const btlso::IPv4Address& original);
// [ 2] ~btlso::IPv4Address();
//
// MANIPULATORS
// [ 9] btlso::IPv4Address& operator=(const btlso::IPv4Address& rhs);
// [ 2] int setIpAddress(const char *address);
// [ 2] void setIpAddress(int address);
// [ 2] void setPortNumber(int portNumber);
// [10] bslx::InStream& bdexStreamIn(bslx::InStream& stream);
// [10] bslx::InStream& bdexStreamIn(bslx::InStream& stream, int version);
//
// ACCESSORS
// [ 4] int loadIpAddress(const char result[16]) const;
// [ 4] int formatIpAddress(const char result[22]) const;
// [ 4] int ipAddress() const;
// [ 4] int portNumber() const;
// [10] int maxSupportedBdexVersion(int) const;
// [10] bslx::OutStream& bdexStreamOut(OutStream& stream) const;
// [10] bslx::OutStream& bdexStreamOut(OutStream& stream, int version) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const IPv4Addr& lhs, const IPv4Addr& rhs);
// [ 6] bool operator!=(const IPv4Addr& lhr, const IPv4Addr& rhs);
// [ 5] ostream& operator<<(ostream& output, const IPv4Address& addr);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLE
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

typedef btlso::IPv4Address  Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#if defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_CLANG)
    // clang can use up all the memory on Linux compiling this test driver,
    // crashing the machine.
    return -1;                                                        // RETURN
#endif
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) {
      case 0:
      case 14: {
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;
        {
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'btlso::IPv4Address' object.  First create a default object 'ip1'.
//..
            btlso::IPv4Address ip1;
//..
// Next, set the value of 'ip1' to have IP address "127.0.0.1" and port number
// 8142.
//..

            ip1.setIpAddress("127.0.0.1");
            ip1.setPortNumber(8142);
            char ip[16];
            ip1.loadIpAddress(ip);  ASSERT( 0 == strcmp("127.0.0.1", ip) );
                                    ASSERT( 8142 == ip1.portNumber() );
//..
// Then create 'ip2' as a copy of 'ip1'.
//..

            btlso::IPv4Address ip2(ip1);
            ip2.loadIpAddress(ip);  ASSERT( 0 == strcmp("127.0.0.1", ip) );
                                    ASSERT( 8142 == ip2.portNumber() );
//..
// Now confirm that 'ip2' has the same value as that of 'ip1'.
//..
            ASSERT( ip2 == ip1 );
//..
// Alternately, set 'ip2' to have a 32-bit IP address in network byte order
// specified as an 'int' on the local platform.
//..

            const int IP = htonl(0x7f000001UL);
            ip2.setIpAddress(IP);   ASSERT( IP == ip2.ipAddress() );
//..
// Confirm that 'ip2' still has the same IP address as 'ip1', as the 32-bit IP
// address 0x7f000001 has the dotted decimal notation "127.0.0.1".
//..

            ASSERT( ip2.ipAddress() == ip1.ipAddress() );
//..
// Finally, write the value of 'ip2' to 'stdout'.
//..

            if (verbose) cout << ip2 << endl;
//..
// The output operator produces the following (single-line) format:
//..
//    127.0.0.1:8142
//..
        }
      } break;
      case 13: {
        // ----------------------------------------------------------------
        // TESTING STATIC isLocalBroadcastAddress FUNCTION:
        //   'isLocalBroadcastAddress' returns nonzero if the input IP
        //   address in dotted decimal notation refers to the address -1,
        //   and 0 otherwise.
        //
        // Plan:
        //   For a sequence of various IP addresses in various format,
        //   for both -1 addresses and other addresses.
        //
        // Testing
        //   static int isLocalBroadcastAddress(const char *address);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isLocalBraodcastAddress'"
                          << "\n================================="
                          << endl;
        {
            static const struct {
                int         d_lineNum;    // line number
                const char *d_ips;        // IP string
                bool        expect;       // Expected value
            } VALUES[] = {
                //line         ips            expected
                //----  --------------------  --------

                // format a.b.c.d

                { L_,  "255.255.255.255",       true  },

                // format a.b.c

                { L_,  "255.255.65535",         true  },
                { L_,  "0xff.0377.65535",       true  },

                // format a.b

                { L_,  "255.16777215",          true  },
                { L_,  "0377.077777777",        true  },

                // format a

                { L_,  "4294967295",            true  },
                { L_,  "0xFFFFFFFF",            true  },
                { L_,  "037777777777",          true  },

                // Invalid address

                { L_,  "325.3.5.7",            false  },
                { L_,  "5.7.0x10000",          false  },
                { L_,  "5.0x1000000",          false  },
                { L_,  "akjfa;kdfjask",        false  },
                { L_,  "",                     false  },
                { L_,  ".",                    false  },
                { L_,  "...",                  false  },
                { L_,  "...........",          false  },
                { L_,  "1.1.1.1.1.1",          false  },
                { L_,  "11111.1.1.1",          false  },
                { L_,  "255.255.255.255.255",  false  },

                // Not -1

                { L_,  "255.255.255.255.",     false  },
                { L_,  "0xff.377.65535",       false  },
                { L_,  "0377..077777777",      false  },
                { L_,  "0.0.0.0",              false  },
                { L_,  "255.5127776",          false  },
                { L_,  "-1",                   false  },
                { L_,  "255.255.0.255",        false  }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE   = VALUES[i].d_lineNum;
                const char *IPS    = VALUES[i].d_ips;
                const bool  EXPECT = VALUES[i].expect;

                if (veryVerbose) {
                    T_(); P(IPS);
                }

                LOOP2_ASSERT(i, LINE,
                    EXPECT == (
                    btlso::IPv4Address::isLocalBroadcastAddress(IPS) != 0));
            }
        }
      } break;
      case 12: {
        // ----------------------------------------------------------------
        // TESTING STATIC 'isValid' and 'isValidAddress' FUNCTIONS:
        //   'isValid' returns 0 if the input IP address in dotted decimal
        //   notation is valid, and -1 otherwise.
        //   'isValidAddress' returns 'true' if the input IP address in
        //   dotted decimal notation is valid, and 'false' otherwise.
        // Plan:
        //   For a sequence of various IP addresses in various format,
        //   verify their validity.
        //
        // Testing
        //   static bool isValidAddress(const char *address);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isValidAddress'"
                          << "\n========================"
                          << endl;
        {
            static const struct {
                int         d_lineNum;    // line number
                const char *d_ips;        // IP string
                bool        d_valid;      // Expected return value
            } VALUES[] = {
                //line         ips          expected
                //----  ------------------  --------

                // format a.b.c.d

                { L_,  "0.0.0.0"          ,   true  },
                { L_,  "127.191.0xdF.0357",   true  },
                { L_,  "127.191.0337.0xEf",   true  },
                { L_,  "239.0x7f.0277.223",   true  },
                { L_,  "239.0177.0xbf.223",   true  },
                { L_,  "0xdf.0357.127.191",   true  },
                { L_,  "0337.0xEf.127.191",   true  },
                { L_,  "0277.223.239.0x7f",   true  },
                { L_,  "0xBf.223.239.0177",   true  },
                { L_,  "199.172.169.20"   ,   true  },
                { L_,  "255.255.255.255"  ,   true  },

                // format a.b.c

                { L_,  "199.172.43284"    ,   true  },
                { L_,  "0xc7.0xaC.0124424",   true  },
                { L_,  "0307.0254.0xa914" ,   true  },

                // format a.b

                { L_,  "199.11315476"     ,   true  },
                { L_,  "0xc7.053124424"   ,   true  },
                { L_,  "0307.0xAca914"    ,   true  },

                // format a

                { L_,  "3349981460"       ,   true  },
                { L_,  "030753124424"     ,   true  },
                { L_,  "0xc7acA914"       ,   true  },
                { L_,  "0xffffffff"       ,   true  },

                // Invalid address

                { L_,  "325.3.5.7"        ,  false  },
                { L_,  "5.7.0x10000"      ,  false  },
                { L_,  "5.0x1000000"      ,  false  },
                { L_,  "akjfa;kdfjask"    ,  false  }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE   = VALUES[i].d_lineNum;
                const char *IPS    = VALUES[i].d_ips;
                const bool  EXPECT = VALUES[i].d_valid;

                if (veryVerbose) {
                    T_(); P(IPS);
                }

                LOOP2_ASSERT(i, LINE, EXPECT ==
                                  btlso::IPv4Address::isValidAddress(IPS));
            }
        }
      } break;
      case 11: {
        // ----------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTOR:
        //   The constructors takes a logical IP address and a port number
        //   as inputs.  The logical IP can be specified as either a
        //   32-bit 'int' in network byte order or a null-terminated
        //   string in the dotted decimal notation.  Each segment in the
        //   dotted decimal notation of an IP can be represented in
        //   decimal, octal, or (upper or lower case) hexadecimal format.
        //   The port number must be in the range [0, 65535].
        //
        // Plan:
        //   Specify a set S of IPv4 addresses as (ip, port) pairs having
        //   widely varying values, where ip is either a 32-bit 'int' or a
        //   null-terminated string.  For each (ip, port) in S,  construct
        //   an IPv4 address object x and verify that x has the expected
        //   value.
        //
        // Testing
        //   btlso::IPv4Address(const char *address, int portNumber);
        //   btlso::IPv4Address(int address, int portNumber);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n==================================="
                          << endl;
        {
            static const struct {
                int         d_lineNum;    // line number
                int         d_ip;         // 32-bit IP
                const char *d_ips;        // IP string
                int         d_port;       // port number
            } VALUES[] = {
                //line        ip                   ips           port
                //---- -------------------  ------------------  ------

                // format a.b.c.d

                { L_,  htonl(0x00000000UL), "0.0.0.0"          ,     0 },
                { L_,  static_cast<int>(htonl(0x7fbfdfefUL)),
                                            "127.191.0xdF.0357",    58 },
                { L_,  static_cast<int>(htonl(0x7fbfdfefUL)),
                                            "127.191.0337.0xEf",    58 },
                { L_,  static_cast<int>(htonl(0xef7fbfdfUL)),
                                            "239.0x7f.0277.223",   976 },
                { L_,  static_cast<int>(htonl(0xef7fbfdfUL)),
                                            "239.0177.0xbf.223",   976 },
                { L_,  static_cast<int>(htonl(0xdfef7fbfUL)),
                                            "0xdf.0357.127.191",  8142 },
                { L_,  static_cast<int>(htonl(0xdfef7fbfUL)),
                                            "0337.0xEf.127.191",  8142 },
                { L_,  htonl(0xbfdfef7fUL), "0277.223.239.0x7f", 10364 },
                { L_,  htonl(0xbfdfef7fUL), "0xBf.223.239.0177", 10364 },
                { L_,  static_cast<int>(htonl(0xffffffffUL)),
                                            "255.255.255.255"  , 30000 },
                { L_,  htonl(0xc7aca914UL), "199.172.169.20"   , 65535 },

                // format a.b.c

                { L_,  htonl(0xc7aca914UL), "199.172.43284"    , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7.0xaC.0124424", 65535 },
                { L_,  htonl(0xc7aca914UL), "0307.0254.0xa914" , 65535 },

                // format a.b

                { L_,  htonl(0xc7aca914UL), "199.11315476"     , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7.053124424"   , 65535 },
                { L_,  htonl(0xc7aca914UL), "0307.0xAca914"    , 65535 },

                // format a

                { L_,  htonl(0xc7aca914UL), "3349981460"       , 65535 },
                { L_,  htonl(0xc7aca914UL), "030753124424"     , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7acA914"       , 65535 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int          LINE = VALUES[i].d_lineNum;
                const int          IP   = VALUES[i].d_ip;
                const char        *IPS  = VALUES[i].d_ips;
                const int          PORT = VALUES[i].d_port;

                {
                    Obj x(IPS, PORT);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(IPS); P(PORT);
                    }
                    LOOP2_ASSERT(i, LINE,   IP == X.ipAddress());
                    LOOP2_ASSERT(i, LINE, PORT == X.portNumber());
                }

                {
                    Obj x(IP, PORT);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(IP); P(PORT);
                    }
                    LOOP2_ASSERT(i, LINE,   IP == X.ipAddress());
                    LOOP2_ASSERT(i, LINE, PORT == X.portNumber());
                }
            }
        }
      } break;
      case 10: {
        // ----------------------------------------------------------------
        // TESTING BDEX STREAMING FUNCTIONALITY:
        //   The BDEX streaming concerns for this component are
        //   absolutely standard.  We first probe the member functions
        //   'outStream' and 'inStream' in the manner of a "breathing test"
        //   to verify basic functionality, then we thoroughly test that
        //   functionality using the available BDEX functions which
        //   forward appropriate calls to the member functions.  We next
        //   step through the sequence of possible stream states (valid,
        //   empty, invalid, incomplete, and corrupted), appropriately
        //   selecting data sets as described below.  In all cases,
        //   exception neutrality is confirmed using the specially
        //   instrumented 'bslx::TestInStream' and a pair of standard
        //   macros, 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //   'bslx::TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'outStream' and
        //     'inStream' methods (the rest of the testing will use the
        //     stream operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each
        //     try that the object is unchanged and that the stream is
        //     invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by assigning it
        //     a distinct new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of
        //     a typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each data field in the
        //     stream (beginning with the version number), provide one or
        //     more similar tests with that data field corrupted.  After
        //     each test, verify that the object is in some valid state
        //     after streaming, and that the input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   bslx::InStream& streamIn(bslx::InStream& stream);
        //   bslx::InStream& streamIn(bslx::InStream& stream, int version);
        //   bslx::OutStream& streamOut(bslx::OutStream& stream) const;
        //   bslx::OutStream& streamOut(bslx::OutStream& stream, int) const;
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests

        const int IPA = htonl(0x00000000UL);
        const int PA  = 0;                       // VA
        const int IPB = htonl(0x7fbfdfefUL);
        const int PB  = 58;                      // VB
        const int IPC = htonl(0xbfdfef7fUL);
        const int PC  = 976;                     // VC
        const int IPD = htonl(0xdfef7fbfUL);
        const int PD  = 8142;                    // VD
        const int IPE = htonl(0xef7fbfdfUL);
        const int PE  = 30000;                   // VE
        const int IPF = htonl(0xffffffffUL);
        const int PF  = 65535;                   // VF

        Obj VA, VB, VC, VD, VE, VF;
        VA.setIpAddress(IPA); VA.setPortNumber(PA);
        VB.setIpAddress(IPB); VB.setPortNumber(PB);
        VC.setIpAddress(IPC); VC.setPortNumber(PC);
        VD.setIpAddress(IPD); VD.setPortNumber(PD);
        VE.setIpAddress(IPE); VE.setPortNumber(PE);
        VF.setIpAddress(IPF); VF.setPortNumber(PF);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
        "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        const int VERSION = Obj::maxSupportedBdexVersion(0);
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                       " (valid) 'streamIn' functionality." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;
            using bslx::OutStreamFunctions::bdexStreamOut;

            const Obj X(VC);
            Out out(VERSION_SELECTOR);
            bdexStreamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            ASSERT(in);                             ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;           ASSERT(X != T);
            bdexStreamIn(in,  t, VERSION);
            ASSERT(X == T);
            ASSERT(in);                             ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
        "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;
            using bslx::OutStreamFunctions::bdexStreamOut;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out(VERSION_SELECTOR);
                bdexStreamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                  BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    bdexStreamIn(in, t, VERSION);
                  } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
        "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(VERSION_SELECTOR);
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input
                // stream leaves the stream invalid and the target object
                // unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) { in.reset();
                bdexStreamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdexStreamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        if (verbose) cout <<
        "\tOn incomplete (but otherwise valid) data." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;
            using bslx::OutStreamFunctions::bdexStreamOut;

            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out(VERSION_SELECTOR);
            bdexStreamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdexStreamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdexStreamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);
              BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i)    LOOP_ASSERT(i, W1 == t1);
                    bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                   LOOP_ASSERT(i, W2 == t2);
                    bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                   LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);
                                   LOOP_ASSERT(i, X1 == t1);
                    bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                   LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);
                                   LOOP_ASSERT(i, X1 == t1);
                    bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);
                                   LOOP_ASSERT(i, X2 == t2);
                    bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                   LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;           LOOP_ASSERT(i, Y1 == t1);

                                   LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;           LOOP_ASSERT(i, Y2 == t2);

                                   LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;           LOOP_ASSERT(i, Y3 == t3);
              } BSLX_TESTINSTREAM_EXCEPTION_TEST_END;
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(VERSION_SELECTOR);
            const unsigned char *ip = (const unsigned char *) &IPB;
            out.putUint8(ip[0]);
            out.putUint8(ip[1]);
            out.putUint8(ip[2]);
            out.putUint8(ip[3]);
            out.putUint16(PB);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            bdexStreamIn(in,  t, VERSION);
            ASSERT(in);
                            ASSERT(X != t); ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\tVersion too small." <<endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;

            const char version = 0; // too small ('version' >= 1)

            Out out(VERSION_SELECTOR);
            const unsigned char *ip = (const unsigned char *) &IPB;
            out.putUint8(ip[0]);
            out.putUint8(ip[1]);
            out.putUint8(ip[2]);
            out.putUint8(ip[3]);
            out.putUint16(PB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            bdexStreamIn(in,  t, version);
            ASSERT(!in);
                            ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\t\t\tVersion too big." << endl;
        {
            using bslx::InStreamFunctions::bdexStreamIn;

            const char version = 5; // too large (current versions = 1)

            Out out(VERSION_SELECTOR);
            const unsigned char *ip = (const unsigned char *) &IPB;
            out.putUint8(ip[0]);
            out.putUint8(ip[1]);
            out.putUint8(ip[2]);
            out.putUint8(ip[3]);
            out.putUint16(PB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);       ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD); ASSERT(in);
            in.setQuiet(!veryVerbose);
            bdexStreamIn(in,  t, version);
            ASSERT(!in);
                            ASSERT(X == t); ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion'."
                          << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(0));
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int          d_lineNum;     // source line number
                int          d_ip;          // IP
                int          d_port;        // port number
                int          d_version;     // version to stream with
                int          d_length;      // expect output length
                const char  *d_fmt_p;       // expected output format
            } DATA[] = {
                //line     ip      port ver len format
                //---- ----------  ---- --- --- -------------------------
                { L_,  0x00000000, 10,  0,  0, ""                        },
                { L_,  static_cast<int>(0xffffffff),
                                    8,  0,  0, ""                        },
                { L_,  0x00000000, 10,  1,  6, "\x00\x00\x00\x00\x00\x0a"},
                { L_,  static_cast<int>(0xffffffff),
                                    8,  1,  6, "\xff\xff\xff\xff\x00\x08"}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_lineNum;
                const int IP          = DATA[i].d_ip;
                const int PORT        = DATA[i].d_port;
                const int VERSION     = DATA[i].d_version;
                const int LEN         = DATA[i].d_length;
                const char *const FMT = DATA[i].d_fmt_p;

                Obj mX;  const Obj& X = mX;
                mX.setIpAddress(IP);  mX.setPortNumber(PORT);
                bslx::ByteOutStream out(VERSION_SELECTOR);
                X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                if (verbose && memcmp(out.data(),
                                      FMT,
                                      LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                      (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                   (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY;  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bslx::ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bslx::ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // ----------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial
        //   value without affecting the rhs operand value.  Also, any
        //   object must be assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and
        //   varied differences in value.  Construct and initialize all
        //   combinations (u, v) in the cross product S x S, copy
        //   construct a control w from v, assign v to u, and assert that
        //   w == u and w == v.  Then test aliasing by copy constructing a
        //   control w from each u in S, assigning u to itself, and
        //   verifying that w == u.
        //
        // Testing:
        //   btlso::IPv4Address& operator=(const btlso::IPv4Address& rhs);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;
        {
            static const struct {
                int         d_lineNum;   // Line number
                const char *d_ips;       // IP string
                int         d_port;      // Port number
            } VALUES[] = {
                //line        ips          port
                //---- -----------------  ------
                { L_,  "0.0.0.0"        ,     0 },
                { L_,  "127.191.223.239",    58 },
                { L_,  "239.127.191.223",   976 },
                { L_,  "223.239.127.191",  8142 },
                { L_,  "191.223.239.127", 10364 },
                { L_,  "255.255.255.255", 30000 },
                { L_,  "199.172.169.20" , 65535 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINEI = VALUES[i].d_lineNum;

                Obj v;  const Obj& V = v;
                v.setIpAddress(VALUES[i].d_ips);
                v.setPortNumber(VALUES[i].d_port);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int LINEJ = VALUES[j].d_lineNum;

                    Obj u;  const Obj& U = u;
                    u.setIpAddress(VALUES[i].d_ips);
                    u.setPortNumber(VALUES[i].d_port);

                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);

                    LOOP4_ASSERT(i, j, LINEI, LINEJ, W == U);
                    LOOP4_ASSERT(i, j, LINEI, LINEJ, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;

                Obj u;  const Obj& U = u;
                u.setIpAddress(VALUES[i].d_ips);
                u.setPortNumber(VALUES[i].d_port);

                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_();  P_(U);  P(W); }

                LOOP2_ASSERT(i, LINE, W == U);
            }
        }
      } break;
      case 8: {
        // ----------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g':
        //   Not Implemented.
        // ----------------------------------------------------------------
      } break;
      case 7: {
        // ----------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without
        //   affecting its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same
        //   value as w.
        //
        // Testing:
        //   btlso::IPv4Address(const btlso::IPv4Address& original);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;
        {
            static const struct {
                int         d_lineNum;   // Line number
                const char *d_ips;       // IP string
                int         d_port;      // Port number
            } VALUES[] = {
                //line        ips          port
                //---- -----------------  ------
                { L_,  "0.0.0.0"        ,     0 },
                { L_,  "127.191.223.239",    58 },
                { L_,  "239.127.191.223",   976 },
                { L_,  "223.239.127.191",  8142 },
                { L_,  "191.223.239.127", 10364 },
                { L_,  "255.255.255.255", 30000 },
                { L_,  "199.172.169.20" , 65535 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE = VALUES[i].d_lineNum;
                Obj w;  const Obj& W = w;           // control
                w.setIpAddress(VALUES[i].d_ips);
                w.setPortNumber(VALUES[i].d_port);

                Obj x;  const Obj& X = x;
                x.setIpAddress(VALUES[i].d_ips);
                x.setPortNumber(VALUES[i].d_port);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
                LOOP2_ASSERT(i, LINE, X == W);
                LOOP2_ASSERT(i, LINE, Y == W);
            }
        }
      } break;
      case 6: {
        // ----------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in parameters for
        //   logical IP and port number.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor
        //   or subtle differences.  Each segment in the dotted decimal
        //   notation of a "base" IP is perturbed to create new IPs.  All
        //   four segments are also rotated to form other new IPs.  Verify
        //   the correctness of 'operator==' and 'operator!=' using all
        //   elements (u, v) of the cross product S X S.
        //
        // Testing:
        //   operator==(const IPv4Addr&, const IPv4Addr&);
        //   operator!=(const IPv4Addr&, const IPv4Addr&);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
        "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int         d_lineNum;   // Line number
                const char *d_ips;       // IP string
                int         d_port;      // Port number
            } VALUES[] = {
                //line        ips          port
                //---- -----------------  ------
                { L_,  "0.0.0.0"        ,     0 },
                { L_,  "127.191.223.239",     0 },
                { L_,  "127.191.223.239",     1 },
                { L_,  "127.191.223.239",    58 },
                { L_,  "128.191.223.239",    58 },
                { L_,  "127.192.223.239",    58 },
                { L_,  "127.191.224.239",    58 },
                { L_,  "127.191.223.240",    58 },
                { L_,  "239.127.191.223",    58 },
                { L_,  "223.239.127.191",    58 },
                { L_,  "191.223.239.127",    58 },
                { L_,  "255.255.255.255",    58 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINEI = VALUES[i].d_lineNum;

                Obj u;  const Obj& U = u;
                u.setIpAddress(VALUES[i].d_ips);
                u.setPortNumber(VALUES[i].d_port);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int LINEJ = VALUES[j].d_lineNum;

                    Obj v;  const Obj& V = v;
                    v.setIpAddress(VALUES[j].d_ips);
                    v.setPortNumber(VALUES[j].d_port);
                    int isSame = i == j;
                    if (veryVerbose) { P_(i); P_(j); P_(U); P(V); }

                    LOOP4_ASSERT(i, j, LINEI, LINEJ,  isSame == (U == V));
                    LOOP4_ASSERT(i, j, LINEI, LINEJ, !isSame == (U != V));
                    LOOP4_ASSERT(i, j, LINEI, LINEJ,  isSame == (V == U));
                    LOOP4_ASSERT(i, j, LINEI, LINEJ, !isSame == (V != U));
                }
            }
        }
      } break;
      case 5: {
        // ----------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator prints out the value of an IPv4 address
        //   object in a single-line format.
        //
        // Plan:
        //   For each of a set of object values, use 'ostrstream' to write
        //   that object's value to a character buffer and then compare
        //   the contents of that buffer with the expected output format.
        //
        // Testing:
        //   operator<<(ostream&, const btlso::IPv4Address&);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator"
                          << "\n============================" << endl;
        {
            static const struct {
                int         d_lineNum;   // Line number
                const char *d_ips;       // IP string
                int         d_port;      // Port number
                const char *d_fmt;       // Expected format
            } DATA[] = {
                //line        ips          port     expected format
                //---- -----------------  ------ -----------------------
                { L_,  "0.0.0.0"        ,     0, "0.0.0.0:0"             },
                { L_,  "3.5.7.9"        ,     8, "3.5.7.9:8"             },
                { L_,  "127.191.223.239",    58, "127.191.223.239:58"    },
                { L_,  "239.127.191.223",   976, "239.127.191.223:976"   },
                { L_,  "223.239.127.191",  8142, "223.239.127.191:8142"  },
                { L_,  "191.223.239.127", 10364, "191.223.239.127:10364" },
                { L_,  "255.255.255.255", 30000, "255.255.255.255:30000" },
                { L_,  "199.172.169.20" , 65535, "199.172.169.20:65535"  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SIZE = 1000;        // output string buffer size
            const char XX = (char) 0xFF;  // represents unset 'char'
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra char check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int   LINE = DATA[di].d_lineNum;
                const char *IPS  = DATA[di].d_ips;
                const int   PORT = DATA[di].d_port;
                const char *FMT  = DATA[di].d_fmt;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE);

                Obj x;  const Obj& X = x;
                x.setIpAddress(IPS);
                x.setPortNumber(PORT);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: "
                                      << FMT << endl;
                ostrstream out(buf, SIZE);  out << X << ends;
                if (veryVerbose) cout << "\tACTUAL FORMAT:   "
                                      << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP2_ASSERT(di, LINE, SZ  < SIZE);
                LOOP2_ASSERT(di, LINE, XX == buf[SIZE - 1]);
                LOOP2_ASSERT(di, LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP2_ASSERT(di, LINE,  0 == memcmp(buf + SZ,
                                                    CTRL_BUF + SZ,
                                                    SIZE-SZ));

                // Check to make sure setw applies to the whole object.

                char buf2[SIZE];
                ostrstream out_setw(buf2, SIZE);
                out_setw << setw(100) << X << ends;

                string str(out.str());
                string setw_str(out_setw.str());

                str = string(100 - str.length(), ' ') + str;

                if (veryVerbose) cout << "\tEXPECTED SETW OUTPUT: "
                                      << str << endl;

                if (veryVerbose) cout << "\tACTUAL SETW OUTPUT:   "
                                      << setw_str << endl;

                LOOP2_ASSERT(di, LINE, str == setw_str);
            }
        }
      } break;
      case 4: {
        // ----------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //   'loadIpAddress' converts the internal 32-bit IP address into
        //   its dotted decimal notation and stores it as a null-terminated
        //   string then returns the number of bytes used (including the
        //   null termination char).
        //   'formatIpAddress' converts the internal 32-bit IP address into
        //   its dotted decimal notation, appends a colon followed by the
        //   port number and stores it as a null-terminated string then
        //   returns the number of bytes used (including the null
        //   termination char).
        //   The other accessors simply return the internal representations
        //   of IP and port number.
        //
        // Plan:
        //   For a sequence of logical IPs represented in the dotted
        //   decimal notation as null-terminated strings, use the fully
        //   tested manipulator to set the logical IP of a pre-existing
        //   IPv4 address object x.  Call 'loadIpAddress' to obtain the
        //   IP's dotted decimal notation and compare with the original
        //   representation, and verify the num bytes used are correct.
        //   Call 'formatIpAddress' to obtain the IP's dotted decimal
        //   notation and compare with the original representation with
        //   port, and verify the num bytes used are correct.  Call other
        //   direct accessors and verify that they return the expected
        //   values.
        //
        // Testing:
        //   void loadIpAddress(const char result[16]) const;
        //   void formatIpAddress(const char result[22]) const;
        //   int ipAddress() const;
        //   int portNumber() const;
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTESTING DIRECT ACCESSORS"
                          << "\n========================"
                          << endl;

        {
            static const struct {
                int         d_lineNum;    // line number
                int         d_ip;         // 32-bit IP
                const char *d_ips;        // IP string
                int         d_port;       // port number
                const char *d_ipsp;       // IP string + port
            } VALUES[] = {
              //line        ip                 ips/ipsp       port
              //---- -------------------  ------------------ -----

                { L_,  htonl(0x00000000UL), "0.0.0.0"        ,     0 ,
                                            "0.0.0.0:0"               },
                { L_,  static_cast<int>(htonl(0x7fbfdfefUL)),
                                            "127.191.223.239",    58 ,
                                            "127.191.223.239:58"      },
                { L_,  static_cast<int>(htonl(0xef7fbfdfUL)),
                                            "239.127.191.223",   976 ,
                                            "239.127.191.223:976"     },
                { L_,  static_cast<int>(htonl(0xdfef7fbfUL)),
                                            "223.239.127.191",  8142 ,
                                            "223.239.127.191:8142"    },
                { L_,  htonl(0xbfdfef7fUL), "191.223.239.127", 10364 ,
                                            "191.223.239.127:10364"   },
                { L_,  static_cast<int>(htonl(0xffffffffUL)),
                                            "255.255.255.255", 30000 ,
                                            "255.255.255.255:30000"   },
                { L_,  htonl(0xc7aca914UL), "199.172.169.20" , 65535 ,
                                            "199.172.169.20:65535"    }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            char IPSTRING[16];
            char IPSTRINGPORT[22];

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE = VALUES[i].d_lineNum;
                const int   IP   = VALUES[i].d_ip;
                const char *IPS  = VALUES[i].d_ips;
                const char *IPSP = VALUES[i].d_ipsp;
                const int   PORT = VALUES[i].d_port;

                int rLen;
                int rLongLen;

                Obj x;  const Obj& X = x;
                x.setIpAddress(IPS);
                x.setPortNumber(PORT);
                rLen = x.loadIpAddress(IPSTRING);
                rLongLen = x.formatIpAddress(IPSTRINGPORT);

                if (veryVerbose) {
                    T_(); P_(i); P(X);
                    T_(); T_(); P_(IPS); P_(IPSTRING); P(PORT);
                    T_(); T_(); P_(IPSTRINGPORT); P_(rLen); P(rLongLen);
                }

                LOOP2_ASSERT(i, LINE,    0  == strcmp(IPSTRING, IPS));
                LOOP2_ASSERT(i, LINE,    0  == strcmp(IPSTRINGPORT, IPSP));
                LOOP2_ASSERT(i, LINE,   IP  == X.ipAddress());
                LOOP2_ASSERT(i, LINE, PORT  == X.portNumber());
                LOOP2_ASSERT(i, LINE,
                             static_cast<int>(bsl::strlen(IPS)) +1  == rLen);
                LOOP2_ASSERT(i, LINE,
                           static_cast<int>(bsl::strlen(IPSP)) +1 == rLongLen);

                // Must + 1 for the null char (not picked up by strlen).

            }
        }
      } break;
      case 3: {
        // ----------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Not Implemented.
        // ----------------------------------------------------------------
      } break;
      case 2: {
        // ----------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   An IPv4 address consists of a logical IP address and a port
        //   number.  The 'set' manipulators for logical IP address take
        //   either a 32-bit 'int' on the local platform in
        //   network byte order or a null-terminated string using the
        //   dotted decimal notation.  Each segment in the dotted decimal
        //   notation can be represented in decimal, octal, or hexadecimal
        //   format.  The 'set' manipulator for port number takes an
        //   integer in the range [0, 65535].
        //
        // Plan:
        //   First, create two IPv4 address objects x1 and x2 using the
        //   default constructor.
        //
        //   Next, for a sequence of distinct logical IPs, each of which
        //   represented in both 32-bit 'int' and null-terminated string,
        //   use the 'set' manipulator taking 32-bit 'int' to set X1's
        //   logical IP, and use the 'set' manipulator taking
        //   null-terminated string to set X2's logical IP.  Verify the IP
        //   value using the trivial direct accessor for logical IP.
        //
        //   Also, for a sequence of distinct port numbers, set x1 and
        //   x2's port number.  Verify the port number using the trivial
        //   direct accessor for port number.
        //
        //   Note that the destructor is exercised on each configuration
        //   as the object being tested leaves scope.  Also note that the
        //   trivial direct accessors for logical IP and port number
        //   simply return the internal representations of the
        //   corresponding value, and therefore can safely be relied upon.
        //
        // Testing:
        //   btlso::IPv4Address();
        //   ~btlso::IPv4Address();
        //   int  setIpAddress(const char *address);
        //   void setIpAddress(int address);
        //   void setPortNumber(int portNumber);
        // ----------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n===========================" << endl;

        {
            static const struct {
                int         d_lineNum;    // line number
                int         d_ip;         // 32-bit IP
                const char *d_ips;        // IP string
                int         d_port;       // port number
            } VALUES[] = {
                //line        ip                   ips           port
                //---- -------------------  ------------------  ------

                // format a.b.c.d

                { L_,  htonl(0x00000000UL), "0.0.0.0"          ,     0 },
                { L_,  static_cast<int>(htonl(0x7fbfdfefUL)),
                                            "127.191.0xdF.0357",    58 },
                { L_,  static_cast<int>(htonl(0x7fbfdfefUL)),
                                            "127.191.0337.0xEf",    58 },
                { L_,  static_cast<int>(htonl(0xef7fbfdfUL)),
                                            "239.0x7f.0277.223",   976 },
                { L_,  static_cast<int>(htonl(0xef7fbfdfUL)),
                                            "239.0177.0xbf.223",   976 },
                { L_,  static_cast<int>(htonl(0xdfef7fbfUL)),
                                            "0xdf.0357.127.191",  8142 },
                { L_,  static_cast<int>(htonl(0xdfef7fbfUL)),
                                            "0337.0xEf.127.191",  8142 },
                { L_,  htonl(0xbfdfef7fUL), "0277.223.239.0x7f", 10364 },
                { L_,  htonl(0xbfdfef7fUL), "0xBf.223.239.0177", 10364 },
                { L_,  htonl(0xc7aca914UL), "199.172.169.20"   , 65535 },

                // format a.b.c

                { L_,  htonl(0xc7aca914UL), "199.172.43284"    , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7.0xaC.0124424", 65535 },
                { L_,  htonl(0xc7aca914UL), "0307.0254.0xa914" , 65535 },

                // format a.b

                { L_,  htonl(0xc7aca914UL), "199.11315476"     , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7.053124424"   , 65535 },
                { L_,  htonl(0xc7aca914UL), "0307.0xAca914"    , 65535 },

                // format a

                { L_,  htonl(0xc7aca914UL), "3349981460"       , 65535 },
                { L_,  htonl(0xc7aca914UL), "030753124424"     , 65535 },
                { L_,  htonl(0xc7aca914UL), "0xc7acA914"       , 65535 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE = VALUES[i].d_lineNum;
                const int   IP   = VALUES[i].d_ip;
                const char *IPS  = VALUES[i].d_ips;
                const int   PORT = VALUES[i].d_port;

                Obj x1;  const Obj& X1 = x1;
                Obj x2;  const Obj& X2 = x2;

                x1.setIpAddress(IP);
                LOOP2_ASSERT(i, LINE,    0 == x2.setIpAddress(IPS));
                x1.setPortNumber(PORT);
                x2.setPortNumber(PORT);

                if (veryVerbose) {
                    T_(); P_(X1); P(X2); T_(); T_(); P_(IPS); P(PORT);
                }

                LOOP2_ASSERT(i, LINE,   IP == X1.ipAddress());
                LOOP2_ASSERT(i, LINE,   IP == X2.ipAddress());
                LOOP2_ASSERT(i, LINE, PORT == X1.portNumber());
                LOOP2_ASSERT(i, LINE, PORT == X2.portNumber());
            }
        }

        {
            static const struct {
                int         d_lineNum;    // line number
                const char* d_ips;        // IP string
            } VALUES[] = {
                //line         ips
                //----  ------------------
                // Invalid address

                { L_,  "325.3.5.7"         },
                { L_,  "5.7.0x10000"       },
                { L_,  "5.0x1000000"       },
                { L_,  "akjfa;kdfjask"     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            if (veryVerbose) {
                cout << "\tInvalid IP address:" << endl;
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE = VALUES[i].d_lineNum;
                const char *IPS  = VALUES[i].d_ips;

                if (veryVerbose) {
                    T_(); P(IPS);
                }

                Obj x;
                LOOP2_ASSERT(i, LINE, -1 == x.setIpAddress(IPS));
            }
        }
      } break;
      case 1: {
        // ----------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing,
        //   and copy constructors.  Exercise the basic value-semantic
        //   methods and the equality operators using these test objects.
        //   Invoke the primary manipulators [2, 3], copy constructor [5],
        //   and assignment operator without [6] and with [7] aliasing.
        //   Use the direct accessors to verify the expected results.
        //   Display object values frequently in verbose mode.  Note that
        //   'VA', 'VB', denote the same object value in different
        //   formats; and 'VC' denotes another unique value.  '0' denotes
        //   the default object value.
        //
        // 1.  Create default objects x1, x2.    { x1: 0 x2: 0 }
        // 2.  Set x1 to VA.                     { x1:VA x2: 0 }
        // 3.  Set x2 to VB.                     { x1:VA x2:VB }
        // 4.  Create object x3 (with value VC). { x1:VA x2:VB x3:VC }
        // 5.  Create object x4 (copy from x1).  { x1:VA x2:VB x3:VC x4:VA}
        // 6.  Assign x2 = x3.                   { x1:VA x2:VC x3:VC x4:VA}
        // 7.  Assign x1 = x1 (aliasing).        { x1:VA x2:VC x3:VC x4:VA}
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // ----------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const char *IPA = "127.0.0.1";
        const int   PA  = 5;                    // VA
        const int   IPB = htonl(0x7f000001UL);
        const int   PB  = 5;                    // VB
        const char *IPC = "178.55.3.8";
        const int   PC  = 10;                   // VC

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create default objects x1, x2."
                         "\t\t{ x1: 0 x2: 0 }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        Obj mX2;  const Obj& X2 = mX2;

        ASSERT(0 == X1.ipAddress());
        ASSERT(0 == X1.portNumber());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Set x1 to value VA."
                         "\t\t\t\t{ x1:VA x2: 0 }" << endl;
        char ip[16];

        mX1.setIpAddress(IPA);
        mX1.setPortNumber(PA);
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        X1.loadIpAddress(ip);
        ASSERT( 0 == strcmp(IPA, ip));
        ASSERT(PA == X1.portNumber());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x2 to value VB."
                         "\t\t\t\t{ x1:VA x2:VB }" << endl;
        mX2.setIpAddress(IPB);
        mX2.setPortNumber(PB);
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(IPB == X2.ipAddress());
        ASSERT( PB == X2.portNumber());

        if (verbose) cout <<
        "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x3 with value VC."
                         "\t\t{ x1:VA x2:VB x3:VC }" << endl;

        Obj mX3(IPC, PC);  const Obj& X3 = mX3;
        if (verbose) { T_();  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        X3.loadIpAddress(ip);
        ASSERT( 0 == strcmp(IPC, ip));
        ASSERT(PC == X3.portNumber());

        if (verbose) cout <<
        "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x1)."
                         "\t\t{ x1:VA x2:VB x3:VC x4:VA }" << endl;

        Obj mX4(X1);  const Obj& X4 = mX4;
        if (verbose) { T_();  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        X4.loadIpAddress(ip);
        ASSERT( 0 == strcmp(IPA, ip));
        ASSERT(PA == X4.portNumber());

        if (verbose) cout <<
        "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(1 == (X4 == X2));        ASSERT(0 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Assign x2 = x3."
                         "\t\t\t\t{ x1:VA x2:VC x3:VC x4:VA }" << endl;

        mX2 = X3;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        X2.loadIpAddress(ip);
        ASSERT( 0 == strcmp(IPC, ip));
        ASSERT(PC == X2.portNumber());

        if (verbose) cout <<
        "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x1 = x1 (aliasing)."
                         "\t\t\t{ x1:VA x2:VC x3:VC x4:VA }" << endl;

        mX1 = X1;
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        X1.loadIpAddress(ip);
        ASSERT( 0 == strcmp(IPA, ip));
        ASSERT(PA == X1.portNumber());

        if (verbose) cout <<
        "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

      } break;
      default: {
            cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
            testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

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
