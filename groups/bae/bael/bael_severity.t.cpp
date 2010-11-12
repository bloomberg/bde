// bael_severity.t.cpp          -*-C++-*-

#include <bael_severity.h>

#include <bsls_platformutil.h>           // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only

#include <bdeu_string.h>

#include <bsl_cstdlib.h>                      // atoi()
#include <bsl_cstring.h>                      // strcmp(), memcmp(), memcpy()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The enumeration under test is non-standard in that the enumeration values
// are non-consecutive.  Therefore, the standard enumeration test plan is
// modified where necessary to accommodate the non-consecutive nature of this
// enumeration.  Note that the set of enumerator values under test are defined
// by { n << 5 : 1 <= n <= 7 }, so expressions similar to "n << 5" occur
// regularly in the test cases.
//-----------------------------------------------------------------------------
// 'bael_Severity' private methods (tested indirectly):
// [ 3] streamOut(bdex_OutStream&, bael_Severity::Level);
// [ 1] streamOut(bdex_OutStream&, bael_Severity::Level);
// [ 1] print(bsl::ostream& stream, bael_Severity::Level value);
//
// 'bael_Severity' public interface:
// [ 1] enum Level { ... };
// [ 1] enum { BAEL_LENGTH = ... };
// [ 2] int maxSupportedBdexVersion();
// [ 1] int fromAscii(bael_Severity::Level *value, const char *string, int);
// [ 1] const char *toAscii(bael_Severity::Level value);
// [ 3] streamIn(bdex_InStream& s, bael_Severity::Level& value);
// [ 3] streamIn(bdex_InStream& s, bael_Severity::Level& value, int ver);
// [ 3] streamOut(bdex_OutStream& s, bael_Severity::Level value);
// [ 3] streamOut(bdex_OutStream& s, bael_Severity::Level value, int ver);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_Severity      Class;
typedef Class::Level       Enum;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

const int NUM_ENUMS = Class::BAEL_LENGTH;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        char buf[256];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);
        bael_Severity::Level level = bael_Severity::BAEL_ERROR;
        const char *asciiLevel = bael_Severity::toAscii(level);
        ASSERT(0 == strcmp(asciiLevel, "ERROR"));
        out << level << ends;
        ASSERT(0 == strcmp(buf, "ERROR"));
        if (veryVerbose) cout << buf << endl;
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING STREAMING OPERATORS
        // Concerns:
        //   Our concerns here are to test the standard operations on streams
        //   that are convertible to 'bdex' streams.  We thoroughly test
        //   "normal" functionality using the 'streamOut' and 'streamIn'
        //   methods.  We next step through the sequence of possible "abnormal"
        //   stream states (empty, invalid, incomplete, and corrupted),
        //   appropriately selecting data sets as described below.  In all
        //   cases, exception neutrality is confirmed using the specially
        //   instrumented 'bdex_TestInStream' and a pair of standard macros,
        //   'BEGIN_BDEX_EXCEPTION_TEST' and 'END_BDEX_EXCEPTION_TEST', which
        //   configure the 'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   Let S represent the set of enumerator values in Enum.
        //   Let T represent the set { -1, 0 } union S.
        //
        //   VALID STREAMS
        //     Verify that each valid enumerator value in S can be written to
        //     and successfully read from a valid 'bdex' data stream into an
        //     instance of the enumeration with any initial value in T leaving
        //     the stream in a valid state.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each valid and invalid initial enumerator value in T,
        //     create an instance of the enumeration and attempt to stream
        //     into it from an empty and then invalid stream.  Verify that the
        //     instance has its initial value, and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct valid enumerator values to an output stream
        //     buffer, which will then be of total length N.  For each partial
        //     stream length from 0 to N - 1, construct an input stream and
        //     attempt to read into enumerator instances initialized with 3
        //     other distinct values.  Verify values of instances that are
        //     successfully modified, partially modified (and therefore reset
        //     to the default value), or left entirely unmodified.  Also verify
        //     that the stream becomes invalid immediately after the first
        //     incomplete read.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate an instance of a
        //     typical valid (control) stream and verify that it can be
        //     streamed in successfully.  Then for each of the two data fields
        //     in the stream (beginning with the version number), provide two
        //     similar tests with the data field corrupted ("too small" and
        //     "too large").  After each test, verify the instance has the
        //     default value, and that the input stream has gone invalid.
        //
        //   WHITE-BOX CONSIDERATIONS
        //   ------------------------
        //   Of the streaming methods being tested here, we know from looking
        //   at the implementation that the 'streamOut' and 'streamIn' methods
        //   which *require* a 'version' parameter are the methods which do the
        //   main bulk of processing (the remaining methods are lightweight
        //   wrappers).  Therefore, only these two are significantly tested -
        //   for the remaining ones we only perform some basic tests with
        //   valid data.
        //
        // Testing:
        //  ^streamOut(bdex_OutStream&,bael_Severity::Level);
        //   streamIn(bdex_InStream& s, bael_Severity::Level& value);
        //   streamIn(bdex_InStream& s, bael_Severity::Level& value, int ver);
        //   streamOut(bdex_OutStream& s, bael_Severity::Level value);
        //   streamOut(bdex_OutStream& s, bael_Severity::Level value, int ver);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming" << endl
                                  << "=================" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCORE FUNCTIONS." << endl << endl;

        // --------------------------------------------------------------------

        const int VERSION = Class::maxSupportedBdexVersion();
        if (verbose) cout << "\tOk data." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                Out out;
                const Enum X = i == 0 ? Enum(0) : Enum(i << 5);
                if (veryVerbose) { P_(i);  P(X); }
                Class::bdexStreamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_ENUMS; ++j) {
                    char ver;
                    In in(OD, LOD);
                    in.setSuppressVersionCheck(1);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());

                    Enum t = j == 0 ? Enum(0) : Enum(j << 5);

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      Class::bdexStreamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST

                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = -1; i <= NUM_ENUMS; ++i) {
                In in(OD, LOD);
                in.setSuppressVersionCheck(1);
                In &testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged if
                // it was initially valid.
                const Enum X = Enum((i + 1) << 5);
                Enum t(X);
                LOOP_ASSERT(i, X == t);

                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  Class::bdexStreamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);
                  LOOP_ASSERT(i, X == t ||
                              ((i == -1 || i == NUM_ENUMS) && 0 == t));
                  Class::bdexStreamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);
                  LOOP_ASSERT(i, X == t ||
                              ((i == -1 || i == NUM_ENUMS) && 0 == t));
                } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose)
            cout << "\t\tOn incomplete, but otherwise valid, data." << endl;
        {
            const Enum W1 = Enum(32),  X1 = Enum(64),  Y1 = Enum(96);
            const Enum W2 = Enum(128), X2 = Enum(160), Y2 = Enum(192);
            const Enum W3 = Enum(224), X3 = Enum(32),  Y3 = Enum(64);

            Out out;
            Class::bdexStreamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            Class::bdexStreamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            Class::bdexStreamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const     OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
              In in(OD, i);  In &testInStream = in;
              in.setSuppressVersionCheck(1);

              BEGIN_BDEX_EXCEPTION_TEST {
                in.reset();
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !i == in.isEmpty());
                Enum t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    Class::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    Class::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                    Class::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    Class::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);   LOOP_ASSERT(i, X1 == t1);
                    Class::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    Class::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    Class::bdexStreamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X1 == t1);
                    Class::bdexStreamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X2 == t2);
                    Class::bdexStreamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                    if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;
                LOOP_ASSERT(i, Y1 == t1);    LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;
                LOOP_ASSERT(i, Y2 == t2);    LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;
                LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Enum W = Enum(32), X = Enum(64), Y = Enum(96);

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            Class::bdexStreamOut(out, Y, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            Class::bdexStreamIn(in, t, VERSION);
            ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);    ASSERT(Y == t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const int version     = 0;          // BAD: too small
            const Enum enumerator = Enum(Y);    // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const int version     = 5;           // BAD: too large
            const Enum enumerator = Enum(Y);     // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const Enum enumerator = Enum(-1);             // BAD: too small
            Out out;
            Class::bdexStreamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const Enum enumerator = Enum(NUM_ENUMS);      // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);
            Class::bdexStreamIn(in, t, VERSION);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\n\nWRAPPER FUNCTIONS." << endl << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting streamIn/Out." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                Out out;
                const Enum X = i == 0 ? Enum(0) : Enum(i << 5);
                if (veryVerbose) { P_(i);  P(X); }
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_ENUMS; ++j) {
                    In in(OD, LOD);
                    in.setSuppressVersionCheck(1);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = j == 0 ? Enum(0) : Enum(j << 5);

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST

                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting bdex functions." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                const Enum X = i == 0 ? Enum(0) : Enum(i << 5);
                if (veryVerbose) { P_(i);  P(X); }
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                for (int j = 0; j < NUM_ENUMS; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = j == 0 ? Enum(0) : Enum(j << 5);
                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING MAXSUPPORTEDBDEXVERSION
        //
        // Concerns:
        //   Our concern is that the correct value is returned.
        //
        // Plan:
        //   Retrieve the version number and ensure that it is the correct
        //   value.  Note that this version number is hard-coded into the .h,
        //   and therefore, this test case needs to be updated whenever that
        //   number changes.
        //
        // Testing:
        //   int maxSupportedBdexVersion();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting maxSupportedBdexVersion"
                          << "\n===========================" << endl;

        const int EXPECTED_VERSION = 1;
        ASSERT(EXPECTED_VERSION == Class::maxSupportedBdexVersion());
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        // Concerns:
        //   The following must hold for the enumeration under test:
        //     - The enumerator values must be [ 1 << 5, 2 << 5, 3 << 5,
        //       4 << 5, 5 << 5, 6 << 5, 7 << 5 ].
        //     - The string representation of the enumerator values must be
        //       correct (and, hence, unique).
        //
        // Plan:
        //   Ensure enumerator values are [ 1 << 5, 2 << 5, 3 << 5, 4 << 5,
        //   5 << 5, 6 << 5, 7 << 5 ] and that all names are unique.  Verify
        //   that the 'toAscii' function produces strings that are identical to
        //   their respective enumerator symbols.  Verify that the 'fromAscii'
        //   function produces values that are identical to the enumerators.
        //   Verify that the output operator produces the same respective
        //   string values that would be produced by 'toAscii' (note that this
        //   is testing streams convertible to standard 'ostream' streams and
        //   the 'print' method).  Also verify the ascii representation and
        //   'ostream' output for invalid enumerator values.
        //
        // Testing:
        //  ^streamOut(bdex_OutStream&,bael_Severity::Level);
        //  ^print(bsl::ostream& stream, bael_Severity::Level value);
        //   enum Level { ... };
        //   enum { BAEL_LENGTH = ... };
        //   int fromAscii(bael_Severity::Level *value,
        //                 const char *s,
        //                 int len);
        //   const char *toAscii(bael_Severity::Level value);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        static const struct {
            Enum         d_enum;    // enumerated value
            const char  *d_ascii;   // string representation
            int          d_value;   // expected value
        } DATA[] = {
            // Enumerated Value    String Representation    expected value
            // ----------------    ---------------------    --------------
            {  Class::BAEL_OFF,    "OFF",                   0               },
            {  Class::BAEL_FATAL,  "FATAL",                 1 << 5          },
            {  Class::BAEL_ERROR,  "ERROR",                 2 << 5          },
            {  Class::BAEL_WARN,   "WARN",                  3 << 5          },
            {  Class::BAEL_INFO,   "INFO",                  4 << 5          },
            {  Class::BAEL_DEBUG,  "DEBUG",                 5 << 5          },
            {  Class::BAEL_TRACE,  "TRACE",                 6 << 5          },
            {  Class::BAEL_NONE,   "NONE",                  7 << 5          },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH == NUM_ENUMS);

        if (verbose)
            cout << "\nVerify enumerator values are sequential." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            LOOP_ASSERT(i, DATA[i].d_enum == DATA[i].d_value);
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;
            const int       VALUE = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_value : -1;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Class::toAscii(Enum(VALUE));
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

            LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));
            for (int j = 0; j < i; ++j) {  // Make sure ALL strings are unique.
                LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii, FMT));
            }
        }

        if (verbose) cout << "\nVerify the fromAscii function." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {  // also check UNKNOWN_FMT
            const char *const STRING    = DATA[i].d_ascii;
            const int         STRINGLEN = bsl::strlen(DATA[i].d_ascii);
            const int         VALUE     = DATA[i].d_value;

            if (veryVerbose) cout << "EXPECTED VALUE: " << VALUE << endl;
            bael_Severity::Level level;
            LOOP_ASSERT(i, 0 == Class::fromAscii(&level, STRING, STRINGLEN));
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << level
                                  << endl <<endl;
            LOOP_ASSERT(i, VALUE == level);

            // Test case-insensitivity.

            bsl::string LSTRING = STRING;  bdeu_String::toLower(&LSTRING);
            LOOP_ASSERT(i, 0 == Class::fromAscii(&level, LSTRING.c_str(),
                                                 STRINGLEN));
            LOOP_ASSERT(i, VALUE == level);

            // Test with smaller lengths.

            for (int j = STRINGLEN - 1; j > 0; --j) {
                LOOP2_ASSERT(i, j, 0 != Class::fromAscii(&level, STRING, j));
            }

            // Test with padding.
            LSTRING += ' ';
            LOOP_ASSERT(i, 0 != Class::fromAscii(&level, LSTRING.c_str(),
                                                 STRINGLEN + 1));
        }

        if (verbose) cout << "\nVerify the output (<<) operator." << endl;

        const int SIZE = 1000;   // Must be big enough to hold output string.
        const char XX = (char) 0xff;  // Value used to represent an unset char.
        char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
        const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
            char buf[SIZE];
            memcpy(buf, CTRL_BUF, SIZE);  // Preset buf to 'unset' char values.

            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;
            const int       VALUE = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_value : -1;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            ostrstream out(buf, sizeof buf); out << Enum(VALUE) << ends;
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl <<endl;

            const int SZ = strlen(FMT) + 1;
            LOOP_ASSERT(i, SZ < SIZE);  // Check buffer is large enough.
            LOOP_ASSERT(i, XX == buf[SIZE - 1]);  // Check for overrun.
            LOOP_ASSERT(i, 0 == memcmp(buf, FMT, SZ));
            LOOP_ASSERT(i, 0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
