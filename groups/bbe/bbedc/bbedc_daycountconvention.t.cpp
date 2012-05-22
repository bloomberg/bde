// bbedc_daycountconvention.t.cpp        -*-C++-*-

#include <bbedc_daycountconvention.h>

#include <bdet_date.h>

#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;

// =========================================================================
//                              TEST PLAN
// -------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.
//--------------------------------------------------------------------------
// [ 1] enum Type { ... };
// [ 2] bdexStreamIn(STREAM&, bbedc_DayCount::Type, int version)
// [ 2] bdexStreamOut(STREAM&, bbedc_DayCount::Type, int version)
// [ 2] maxSupportedBdexVersion();
// [ 1] char *toAscii(bbedc_DayCount::Type value);
// [ 1] ostream& operator<<(ostream& output, bbedc_DayCount::Type type);
// [ 2] operator>>(bdex_InStream&, bbedc_DayCount::Type& rhs);
// [ 2] operator<<(bdex_OutStream&, bbedc_DayCount::Type rhs);
//--------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
//--------------------------------------------------------------------------

// =========================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//--------------------------------------------------------------------------
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_    cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bbedc_DayCountConvention Class;
typedef Class::Type              Enum;
typedef bdex_TestInStream        In;
typedef bdex_TestOutStream       Out;

const int NUM_ENUMS = Class::BBEDC_SIA_30_360_NEOM + 1;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
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

///Usage
///-----
// The following snippets of code provide a simple illustration of ordinary
// 'bbedc_DayCountConvention' operation.
//
// First create a variable 'convention' of type
// 'bbedc_DayCountConvention::Type' and initialize it to the value
// 'bbedc_DayCountConvention::BBEDC_ISMA_30_360':
//..
    bbedc_DayCountConvention::Type convention =
                                   bbedc_DayCountConvention::BBEDC_ISMA_30_360;
//..
// Next, store a pointer to its ASCII representation in a variable 'ascii' of
// type 'const char *':
//..
    const char *ascii = bbedc_DayCountConvention::toAscii(convention);
    ASSERT(0 == strcmp(ascii, "ISMA_30_360"));
//..
// Finally, print the value of 'convention' to 'bsl::cout';
//..
//  bsl::cout << convention << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  ISMA_30_360
//..

        char buf[256];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);
        out << convention << ends;
        ASSERT(0 == strcmp(buf, "ISMA_30_360"));
        if (veryVerbose) cout << buf << endl;

      } break;
      case 2: {
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
        //   Let L represent the number of valid enumerator values.
        //   Let S represent the set of consecutive integers { 0 .. L - 1 }.
        //   Let T represent the set of consecutive integers { -1 .. L }.
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
        //   maxSupportedBdexVersion();
        //   bdexStreamIn(STREAM&, bbedc_DayCount::Type, int version)
        //   bdexStreamOut(STREAM&, bbedc_DayCount::Type, int version)
        //   operator>>(bdex_InStream&, bbedc_DayCount::Type& rhs);
        //   operator<<(bdex_OutStream&, bbedc_DayCount::Type rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming" << endl
                                  << "=================" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCORE FUNCTIONS." << endl << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\tmaxSupportedBdexVersion." << endl;
        {
            const int version = Class::maxSupportedBdexVersion();
            ASSERT(1 == version);
        }

        if (verbose) cout << "\tOk data." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                Out out;
                const Enum X = Enum(i);  if (veryVerbose) { P_(i);  P(X); }
                const int version = Class::maxSupportedBdexVersion();
                out.putVersion(version);
                Class::bdexStreamOut(out, X, version);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_ENUMS; ++j) {
                    int ver;
                    In in(OD, LOD);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);

                    BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      in.getVersion(ver);
                      ASSERT(version == ver);
                      Class::bdexStreamIn(in, t, ver);
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
            const int version = 1;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = -1; i <= NUM_ENUMS; ++i) {
                In in(OD, LOD);
                In &testInStream = in;
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged if
                // it was initially valid.
                const Enum X = Enum(i);
                Enum t(X);
                LOOP_ASSERT(i, X == t);

                BEGIN_BDEX_EXCEPTION_TEST {  in.reset();
                  Class::bdexStreamIn(in, t, version);
                  LOOP_ASSERT(i, !in);
                  LOOP_ASSERT(i, X == t ||
                              ((i == -1 || i == NUM_ENUMS) && 0 == t));
                  Class::bdexStreamIn(in, t, version);
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
            const Enum W1 = Enum(0), X1 = Enum(1), Y1 = Enum(2);
            const Enum W2 = Enum(1), X2 = Enum(2), Y2 = Enum(0);
            const Enum W3 = Enum(2), X3 = Enum(0), Y3 = Enum(1);

            Out out;
            const int version = 1;
            Class::bdexStreamOut(out, X1, version);
            const int LOD1 = out.length();
            Class::bdexStreamOut(out, X2, version);
            const int LOD2 = out.length();
            Class::bdexStreamOut(out, X3, version);
            const int LOD  = out.length();
            const char *const     OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
              In in(OD, i);  In &testInStream = in;

              BEGIN_BDEX_EXCEPTION_TEST {  in.reset();
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, !i == in.isEmpty());
                Enum t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    Class::bdexStreamIn(in, t1, version);
                    LOOP_ASSERT(i, !in);
                    if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    Class::bdexStreamIn(in, t2, version);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W2 == t2);
                    Class::bdexStreamIn(in, t3, version);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    Class::bdexStreamIn(in, t1, version);
                    LOOP_ASSERT(i,  in);   LOOP_ASSERT(i, X1 == t1);
                    Class::bdexStreamIn(in, t2, version);
                    LOOP_ASSERT(i, !in);
                    if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    Class::bdexStreamIn(in, t3, version);
                    LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    Class::bdexStreamIn(in, t1, version);
                    LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X1 == t1);
                    Class::bdexStreamIn(in, t2, version);
                    LOOP_ASSERT(i,  in);    LOOP_ASSERT(i, X2 == t2);
                    Class::bdexStreamIn(in, t3, version);
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

        const Enum W = Enum(0), X = Enum(1), Y = Enum(2);
        ASSERT(NUM_ENUMS > Y);

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const int version = 1;
            Out out;
            Class::bdexStreamOut(out, Y, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            Class::bdexStreamIn(in, t, version);
            ASSERT(in);
            ASSERT(W != t);    ASSERT(X != t);    ASSERT(Y == t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const int version     = 0;          // BAD: too small
            const Enum enumerator = Enum(Y);    // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const int version     = 5;           // BAD: too large
            const Enum enumerator = Enum(Y);     // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const int version     = 1;
            const Enum enumerator = Enum(-1);             // BAD: too small
            Out out;
            Class::bdexStreamOut(out, enumerator, version);
            const char *const OD =  out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);
            Class::bdexStreamIn(in, t, version);
            ASSERT(!in);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
        }
        {
            const int version     = 1;
            const Enum enumerator = Enum(NUM_ENUMS);      // BAD: too large
            Out out;
            Class::bdexStreamOut(out, enumerator, version);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            Enum t(X);
            ASSERT(W != t);    ASSERT(X == t);    ASSERT(Y != t);
            In in(OD, LOD);
            ASSERT(in);
            Class::bdexStreamIn(in, t, version);
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
                const Enum X = Enum(i);
                if (veryVerbose) { P_(i);  P(X); }
                Class::bdexStreamOut(out, X, 1);
                const char *const OD = out.data();
                const int LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.
                for (int j = 0; j < NUM_ENUMS; ++j) {
                    In in(OD, LOD);
                    In &testInStream = in;
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);

                    in.setSuppressVersionCheck(1);

                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, (X == t) == (i == j));
                      Class::bdexStreamIn(in, t, 1);
                    } END_BDEX_EXCEPTION_TEST

                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting '<<' and '>>'." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                const Enum X = Enum(i);
                if (veryVerbose) { P_(i);  P(X); }
                Out out;
                Class::bdexStreamOut(out, X, 1);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                for (int j = 0; j < NUM_ENUMS; ++j) {
                    In in(OD, LOD);  In &testInStream = in;

                    in.setSuppressVersionCheck(1);

                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, !in.isEmpty());
                    Enum t = Enum(j);
                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, (X == t) == (i == j));
                      Class::bdexStreamIn(in, t, 1);
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);
                    LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        // Concerns:
        //   The following must hold for the enumeration under test:
        //     - The enumerator values must be consecutive integers in the
        //       range [0 .. NUM_ENUMS - 1].
        //     - The string representation of the enumerator values must be
        //       correct (and, hence, unique).
        //
        // Plan:
        //   Ensure enumerator values are consecutive integers in the range
        //   [0 .. NUM_ENUMS - 1] and that all names are unique.  Verify that
        //   the 'toAscii' function produces strings that are identical to
        //   their respective enumerator symbols.  Verify that the output
        //   operator produces the same respective string values that would be
        //   produced by 'toAscii' (note that this is testing streams
        //   convertible to standard 'ostream' streams and the 'print' method).
        //   Also verify the ascii representation and 'ostream' output for
        //   invalid enumerator values.
        //
        // Testing:
        //   enum Type { ... };
        //   char *toAscii(bbedc_DayCount::Type value);
        //   ostream& operator<<(ostream& output, bbedc_DayCount::Type type);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        static const struct {
            Enum         d_enum;   // Enumerated Value
            const char  *d_ascii;  // String Representation
        } DATA[] = {
            // Enumerated Value                 String Representation
            // ----------------                 ---------------------
            { Class::BBEDC_ACTUAL_360,          "ACTUAL_360"         },
            { Class::BBEDC_ACTUAL_365_FIXED,    "ACTUAL_365_FIXED"   },
            { Class::BBEDC_ICMA_ACTUAL_ACTUAL,  "ICMA_ACTUAL_ACTUAL" },
            { Class::BBEDC_ISDA_ACTUAL_ACTUAL,  "ISDA_ACTUAL_ACTUAL" },
            { Class::BBEDC_ISMA_ACTUAL_ACTUAL,  "ISMA_ACTUAL_ACTUAL" },
            { Class::BBEDC_ISMA_30_360,         "ISMA_30_360"        },
            { Class::BBEDC_PSA_30_360_EOM,      "PSA_30_360_EOM"     },
            { Class::BBEDC_SIA_30_360_EOM,      "SIA_30_360_EOM"     },
            { Class::BBEDC_SIA_30_360_NEOM,     "SIA_30_360_NEOM"    },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        if (verbose)
            cout << "\nVerify enumerator values are sequential." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            LOOP_ASSERT(i, DATA[i].d_enum == i);
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
            if (Class::BBEDC_ISMA_ACTUAL_ACTUAL == DATA[i].d_enum) {
                continue;
            }

            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Class::toAscii(Enum(i));
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

            LOOP_ASSERT(i, 0 == strcmp(FMT, ACT));
            for (int j = 0; j < i; ++j) {  // Make sure ALL strings are unique.
                LOOP2_ASSERT(i, j, 0 != strcmp(DATA[j].d_ascii, FMT));
            }
        }

        if (verbose) cout << "\nVerify the output (<<) operator." << endl;

        const int SIZE = 1000;   // Must be big enough to hold output string.
        const char XX = (char) 0xff;  // Value used to represent an unset char.
        char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
        const char *CTRL_BUF = mCtrlBuf; // Used to check for extra characters.

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
            if (Class::BBEDC_ISMA_ACTUAL_ACTUAL == DATA[i].d_enum) {
                continue;
            }

            char buf[SIZE];
            memcpy(buf, CTRL_BUF, SIZE);  // Preset buf to 'unset' char values.

            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            ostrstream out(buf, sizeof buf); out << Enum(i) << ends;
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
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
