// ball_severity.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_severity.h>

#include <bdlb_string.h>
#include <bslim_testutil.h>

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
// 'ball::Severity' private methods (tested indirectly):
// [ 1] print(bsl::ostream& stream, ball::Severity::Level value);
//
// 'ball::Severity' public interface:
// [ 1] enum Level { ... };
// [ 1] enum { e_LENGTH = ... };
// [ 1] int fromAscii(ball::Severity::Level *value, const char *string, int);
// [ 1] const char *toAscii(ball::Severity::Level value);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::Severity      Class;
typedef Class::Level       Enum;


const int NUM_ENUMS = Class::e_LENGTH;

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
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
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
        ball::Severity::Level level = ball::Severity::e_ERROR;
        const char *asciiLevel = ball::Severity::toAscii(level);
        ASSERT(0 == strcmp(asciiLevel, "ERROR"));
        out << level << ends;
        ASSERT(0 == strcmp(buf, "ERROR"));
        if (veryVerbose) cout << buf << endl;
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        //
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
        //   print(bsl::ostream& stream, ball::Severity::Level value);
        //   enum Level { ... };
        //   enum { e_LENGTH = ... };
        //   int fromAscii(ball::Severity::Level *value,
        //                 const char *s,
        //                 int len);
        //   const char *toAscii(ball::Severity::Level value);
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
            {  Class::e_OFF,    "OFF",                   0               },
            {  Class::e_FATAL,  "FATAL",                 1 << 5          },
            {  Class::e_ERROR,  "ERROR",                 2 << 5          },
            {  Class::e_WARN,   "WARN",                  3 << 5          },
            {  Class::e_INFO,   "INFO",                  4 << 5          },
            {  Class::e_DEBUG,  "DEBUG",                 5 << 5          },
            {  Class::e_TRACE,  "TRACE",                 6 << 5          },
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
            ball::Severity::Level level;
            LOOP_ASSERT(i, 0 == Class::fromAscii(&level, STRING, STRINGLEN));
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << level
                                  << endl <<endl;
            LOOP_ASSERT(i, VALUE == level);

            // Test case-insensitivity.

            bsl::string LSTRING = STRING;  bdlb::String::toLower(&LSTRING);
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
