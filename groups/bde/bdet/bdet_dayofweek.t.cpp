// bdet_dayofweek.t.cpp                                               -*-C++-*-

#include <bdet_dayofweek.h>

#include <bdex_instreamfunctions.h>
#include <bdex_outstreamfunctions.h>
#include <bdex_testinstream.h>
#include <bdex_testinstreamexception.h>
#include <bdex_testoutstream.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// Standard enumeration test plan.
// ----------------------------------------------------------------------------
// [ 1] enum Day { ... };
// [ 1] static const char *toAscii(Day value);
//
// [ 1] operator<<(ostream&, bdet_DayOfWeek::Day rhs);
// [ 2]
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

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
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

//==========================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//--------------------------------------------------------------------------

typedef bdet_DayOfWeek     Class;
typedef Class::Day         Enum;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

//==========================================================================
//                              MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following snippets of code provide a simple illustration of
// 'bdet_DayOfWeek' operation.  Given the low-level nature of an enumeration
// component, an extended example is not provided.
//
// First, create a variable, 'day', of type 'bdet_DayOfWeek::Day', and
// initialize it to the value 'bdet_DayOfWeek::BDET_MONDAY':
//..
    bdet_DayOfWeek::Day day = bdet_DayOfWeek::BDET_MONDAY;
//..
// Next, store its representation in a variable, 'rep', of type 'const char *':
//..
    const char *rep = bdet_DayOfWeek::toAscii(day);
    ASSERT(0 == bsl::strcmp(rep, "MON"));
//..
// Finally, we print the value of 'day' to 'stdout':
//..

if (verbose) {  // added in test driver

    bsl::cout << day << bsl::endl;

}               // added in test driver
//..
// This statement produces the following output on 'stdout':
//..
//  MON
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING STREAMING OPERATORS:
        //   The 'bdex' streaming concerns for this 'enum' component are
        //   standard.  We thoroughly test "normal" functionality using the
        //   available bdex functions.  We next step through the sequence of
        //   possible "abnormal" stream states (empty, invalid, incomplete,
        //   and corrupted), appropriately selecting data sets as described
        //   below.  In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   Let L represents the number of valid enumerator values.
        //   Let S represent the set of consecutive integers { 1 .. L }
        //   Let T represent the set of consecutive integers { 0 .. L + 1 }
        //
        //   VALID STREAMS
        //     Verify that each valid enumerator value in S can be written to
        //     and successfully read from a valid 'bdex' data stream into an
        //     variable of the enumeration type with any initial value in T
        //     leaving the stream in a valid state.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each valid and invalid initial enumerator value in T,
        //     create a variable of the enumeration type and attempt to stream
        //     into it from an empty and then invalid stream.  Verify that the
        //     variable has its initial value, and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 identical valid enumerator values to an output stream
        //     buffer, which will then be of total length N.  For each partial
        //     stream length from 0 to N - 1, construct an input stream and
        //     attempt to read into variables of the enumerator type
        //     initialized with 3 other identical values.  Verify that values
        //     of variables are successfully modified, partially modified (and
        //     therefore reset to the default value), or left entirely
        //     unmodified.  Also verify that the stream becomes invalid
        //     immediately after the first incomplete read.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate a typical valid
        //     (control) stream and verify that it can be streamed into a
        //     variable of the enumeration type successfully.  Then for each of
        //     the two data fields in the stream (beginning with the version
        //     number), provide two similar tests with the data field corrupted
        //     ("too small" and "too large").  After each test, verify the
        //     variable has the default value, and that the input stream has
        //     gone invalid.
        //
        // Testing:
        //   template <class STREAM>
        //   static STREAM& bdexStreamOut(STREAM&             stream,
        //                                bdet_DayOfWeek::Day value,
        //                                int                 version);
        //   template <class STREAM>
        //   static STREAM& bdexStreamIn(STREAM&              stream,
        //                               bdet_DayOfWeek::Day& value,
        //                               int                  version);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Streaming Operators" << endl
                                  << "===========================" << endl;

        // --------------------------------------------------------------------

        const int VERSION = Class::maxSupportedBdexVersion();

        if (verbose) cout <<
            "\nTesting ('<<') and ('>>') on valid streams and data." << endl;
        if (verbose) cout << "\tFor normal (correct) conditions." << endl;
        {
            BSLMF_ASSERT(Class::BDET_SUN < Class::BDET_SAT);

            for (int i = Class::BDET_SUN; i <= Class::BDET_SAT; ++i) {
                const Enum X = Enum(i);  if (veryVerbose) { P_(i);  P(X); }
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = Class::BDET_SUN; j <= Class::BDET_SAT; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Enum t = Enum(j);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, (X == t) == (i == j));
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

            for (int i = Class::BDET_SUN - 1; i <= Class::BDET_SAT + 1; ++i) {
                In in(OD, LOD);      In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);  LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Enum X = Enum(i);  Enum t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
             "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Enum W1 = Enum(0), X1 = Enum(1), Y1 = Enum(2);
            const Enum W2 = Enum(3), X2 = Enum(4), Y2 = Enum(5);
            const Enum W3 = Enum(6), X3 = Enum(7), Y3 = Enum(8);

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const     OD   = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Enum t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Enum W = Enum(0), X = Enum(1), Y = Enum(2);
        ASSERT(Class::BDET_SAT > Y);
        // If only two enumerators, use Y = X = 1 and remove "ASSERT(Y != t)"s.

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tGood stream (for control)." << endl;

        {
            const char version    = 1;
            const char enumerator = char(Y);

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);   ASSERT(in);
                                ASSERT(W != t); ASSERT(X != t); ASSERT(Y == t);
         }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad version number." << endl;

        {
            const char version    = -1;          // BAD: too small
            const char enumerator = char(Y);

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD);     ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);   ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char version    = 5;           // BAD: too large
            const char enumerator = char(Y);

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD);     ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);   ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

        // -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

        if (verbose) cout << "\t\tBad enumerator value." << endl;

        {
            const char enumerator = char(-1);     // BAD: too small

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD =  out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);   ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }
        {
            const char enumerator = char(Class::BDET_SAT + 1);// BAD: too large

            Out out;
            bdex_OutStreamFunctions::streamOut(out, enumerator, VERSION);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Enum t(X);          ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
            In in(OD, LOD);     ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);   ASSERT(!in);
                                ASSERT(W != t); ASSERT(X == t); ASSERT(Y != t);
        }

      } break;
      case 1: {
        // --------------------------------------------------------
        // Testing:
        //   static const char *toAscii(Day dayOfWeek);
        //   operator<<(ostream&, bdet_DayOfWeek::Day rhs);
        // --------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        static const struct {
            int         d_line;    // line number
            int         d_intDay;  // enumerated Value as int
            const char *d_exp;     // expected String Rep.
        } DATA[] = {
            // line    Enumerated Value         Expected output
            // ----    ----------------         ---------------
            {  L_,     Class::BDET_SUNDAY,        "SUN"                      },
            {  L_,     Class::BDET_SUN,           "SUN"                      },
            {  L_,     Class::BDET_MONDAY,        "MON"                      },
            {  L_,     Class::BDET_MON,           "MON"                      },
            {  L_,     Class::BDET_TUESDAY,       "TUE"                      },
            {  L_,     Class::BDET_TUE,           "TUE"                      },
            {  L_,     Class::BDET_WEDNESDAY,     "WED"                      },
            {  L_,     Class::BDET_WED,           "WED"                      },
            {  L_,     Class::BDET_THURSDAY,      "THU"                      },
            {  L_,     Class::BDET_THU,           "THU"                      },
            {  L_,     Class::BDET_FRIDAY,        "FRI"                      },
            {  L_,     Class::BDET_FRI,           "FRI"                      },
            {  L_,     Class::BDET_SATURDAY,      "SAT"                      },
            {  L_,     Class::BDET_SAT,           "SAT"                      },
#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
            {  L_,     0,                         "(* Unknown Enumerator *)" },
            {  L_,     Class::BDET_SAT + 1,       "(* Unknown Enumerator *)" },
            {  L_,     19,                        "(* Unknown Enumerator *)" },
#endif
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Testing 'toAscii'." << endl;
        {
            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const Enum  DAY  = (Enum) DATA[i].d_intDay;
                const char *EXP  = DATA[i].d_exp;

                const char *res = Class::toAscii(DAY);
                if (veryVerbose) { cout << '\t'; P_(i); P_(DAY); P(res); }
                LOOP2_ASSERT(LINE, i, 0 == strcmp(EXP, res));
            }
        }

        if (verbose) cout << "Testing 'operator<<'." << endl;
        {
            const int   SIZE = 100;
            char        buf[SIZE];
            const char  XX = (char) 0xFF;   // Value for an unset char.
            char        mCtrl[SIZE];           memset(mCtrl, XX, SIZE);
            const char *CTRL = mCtrl;

            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int   LINE = DATA[i].d_line;
                const Enum  DAY  = (Enum) DATA[i].d_intDay;
                const char *EXP  = DATA[i].d_exp;

                memset(buf, XX, SIZE);
                ostrstream out(buf, SIZE);
                out << DAY << ends;

                const int SZ = strlen(EXP) + 1;
                if (veryVerbose) { cout << '\t'; P_(i); P(buf); }
                LOOP2_ASSERT(LINE, i, XX == buf[SIZE - 1]);
                LOOP2_ASSERT(LINE, i,  0 == memcmp(buf, EXP, SZ));
                LOOP2_ASSERT(LINE, i,
                             0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));
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
