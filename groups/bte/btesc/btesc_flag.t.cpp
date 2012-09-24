// btesc_flag.t.cpp          -*-C++-*-

#include <btesc_flag.h>

#include <bsls_platformutil.h>           // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only

#include <bsl_cstdlib.h>                      // atoi()
#include <bsl_cstring.h>                      // strcmp(), memcmp() memcpy()
#include <bsl_iostream.h>

#if defined(BDES_PLATFORMUTIL_NO_LONG_HEADER_NAMES)
#include <strstrea.h>
#else
#include <bsl_strstream.h>
#endif
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.
//-----------------------------------------------------------------------------
// [ 1] enum Flag { ... };
// [ 1] enum { LENGTH = ... };
// [ 1] static const char *toAscii(Flag value);
//
// [ 1] operator<<(ostream&, btesc_Flag::Flag rhs);
// [ 2] bdexStreamIn(bdex_InStream&, btesc_Flag::Flag& rhs);
// [ 2] bdexStreamOut(bdex_OutStream&, btesc_Flag::Flag rhs);
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

typedef btesc_Flag         Class;
typedef Class::Flag        Enum;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

//=============================================================================
//                  HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
inline int twoToTheN(int n)
    // Return the value of 2 raised to the power of the specified 'n' if
    // n >= 0, and return the (negative) value of 'n' otherwise.
{
    if (n < 0)
    {
        return n;
    }

    return 1 << n;
}

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

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING STREAMING OPERATORS:
        //   The 'bdex' streaming concerns for this 'enum' component are
        //   standard.  We thoroughly test "normal" functionality using the
        //   available bdex stream functions.  We next step through
        //   the sequence of possible "abnormal" stream states (empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   Let L represents the number of valid enumerator values.
        //   Let S represent the set of consecutive integers { 0 .. L - 1 }
        //   Let T represent the set of consecutive integers { -1 .. L }
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
        //     Write 3 identical valid enumerator values to an output stream
        //     buffer, which will then be of total length N.  For each partial
        //     stream length from 0 to N - 1, construct an input stream and
        //     attempt to read into enumerator instances initialized with 3
        //     other identical values.  Verify values of instances that are
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
        // Testing:
        //   bdexStreamIn(bdex_InStream&, btesc_Flag::Flag& rhs);
        //   bdexStreamOut(bdex_OutStream&, btesc_Flag::Flag rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming Operators" << endl
                                  << "===========================" << endl;

        // To corrupt the output stream of any 'bde' object for testing
        // purposes, replace the first byte of the stream with the value 'M1'
        // that follows:

        //char M1; {bdex_OutByteStream o; o<<char(-1); M1=o.data()[0];}

        const int NUM_ENUMS = Class::BTESC_LENGTH;

        // --------------------------------------------------------------------

        const int VERSION = Class::maxSupportedBdexVersion();
        if (verbose) cout <<
            "\nTesting ('<<') and ('>>') on valid streams and data." << endl;
        if (verbose) cout << "\tFor normal (correct) conditions." << endl;
        {
            for (int i = 0; i < NUM_ENUMS; ++i) {
                const Enum X = (Enum) twoToTheN(i);
                if (veryVerbose) { P_(i);  P_(twoToTheN(i));  P(X); }
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_ENUMS; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Enum t = (Enum) twoToTheN(j);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
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
                In in(OD, LOD);      In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged if
                // it was initially valid.

                const Enum X = (Enum) twoToTheN(i);  Enum t(X);
                LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == t ||
                                      ((i == -1 || i == NUM_ENUMS) && 0 == t));
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, X == t ||
                                      ((i == -1 || i == NUM_ENUMS) && 0 == t));
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
             "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Enum W1 = (Enum) 2, X1 = (Enum) 1;
            const Enum W2 = (Enum) 1, X2 = (Enum) 2;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD  = out.length();
            const char *const     OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Enum t1(W1), t2(W2);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                }
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Enum W = (Enum) 1, X = (Enum) 2;
        ASSERT(twoToTheN(NUM_ENUMS) > X);
        // If only two enumerators, use Y = X = 1 and remove "ASSERT(Y != t)"s.

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tGood stream (for control)." << endl;

        {
            const char enumerator = (char) W;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                                ASSERT(W == t); ASSERT(X != t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const int version    = 0;          // BAD: too small
            const char enumerator = (char) W;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t);
        }
        {
            const int version    = 5;           // BAD: too large
            const char enumerator = (char) W;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const char enumerator = char(-1);     // BAD: too small

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD =  out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t);
        }
        {
            const char enumerator = char(twoToTheN(NUM_ENUMS)); // BAD: too big

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST:
        //   The enumerators in this component are flag bits, i.e., powers of
        //   2, and *not* consecutive integers.  At present, they are
        //   consecutive powers of two, but note that that fact can change.
        //   Verify that
        //   the 'toAscii' function produces strings that are identical to
        //   their respective enumerator symbols.  Verify that the output
        //   operator produces the same respective string values that would
        //   be produced by 'toAscii'.  Also verify the ascii representation
        //   and 'ostream' output for invalid enumerator values.
        //
        // Testing:
        //   enum Flag { ... };
        //   enum { LENGTH = ... };
        //   static const char *toAscii(Flag value);
        //   operator<<(ostream&, btesc_Flag::Flag rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        static const struct {
            Enum d_enum;                        // Enumerated Value
            const char  *d_ascii;               // String Representation
        } DATA[] = {
            // Enumerated Value                 String Representation
            // --------------------------       --------------------------
            { Class::BTESC_ASYNC_INTERRUPT,     "ASYNC_INTERRUPT"      },
            { Class::BTESC_RAW,                 "RAW"                  },
        };

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH == Class::BTESC_LENGTH);

        if (verbose)
            cout << "\nVerify enumerator values are sequential "
                 << "powers of 2." << endl;

        {
            for (i = 0; i < DATA_LENGTH; ++i) {
                LOOP_ASSERT(i, DATA[i].d_enum == twoToTheN(i));
            }
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT

            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Class::toAscii((Enum) twoToTheN(i));
            if (veryVerbose)
                cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

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
            char buf[SIZE];
            memcpy(buf, CTRL_BUF, SIZE);  // Preset buf to 'unset' char values.

            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            ostrstream out(buf, sizeof buf);
            out << (Enum) twoToTheN(i) << ends;
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
