// ball_transmission.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_transmission.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>  // atoi()
#include <bsl_cstring.h>  // strcmp(), memcmp(), memcpy()

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Standard enumeration test plan.
//
// 'ball::Trans' is sometimes used in the documentation as an abbreviation for
// 'ball::Transmission'.
//-----------------------------------------------------------------------------
// 'ball::Transmission' private methods (tested indirectly):
// [ 1] print(bsl::ostream& stream, ball::Transmission::Cause value);
//
// 'ball::Transmission' public interface:
// [ 1] enum Cause { ... };
// [ 1] enum { LENGTH = ... };
// [ 1] char *toAscii(ball::Transmission::Cause value);
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

typedef ball::Transmission Class;
typedef Class::Cause       Enum;

enum { NUM_ENUMS = Class::e_LENGTH };

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_logger.h

class my_Logger {
  // ...
  ostream& d_os;
  public:
    my_Logger(ostream& stream);
    ~my_Logger();
    void publish(const char *message, ball::Transmission::Cause cause);

  // ...
};

// my_logger.cpp

my_Logger::my_Logger(ostream& stream) : d_os(stream) { }

my_Logger::~my_Logger() { }

void my_Logger::publish(const char                *message,
                        ball::Transmission::Cause  cause)
{
    switch (cause) {
      case ball::Transmission::e_PASSTHROUGH: {
        d_os << ball::Transmission::toAscii(cause) << ":\t" << message << endl;
      } break;
      case ball::Transmission::e_TRIGGER: {
        d_os << ball::Transmission::toAscii(cause) << ":\t" << message << endl;
        d_os << "\t[ dump all messages archived for current thread ]" << endl;
      } break;
      case ball::Transmission::e_TRIGGER_ALL: {
        d_os << ball::Transmission::toAscii(cause) << ":\t" << message << endl;
        d_os << "\t[ dump all messages archived for *all* threads ]"  << endl;
      } break;
      default: {
        d_os << "***ERROR*** Unsupported Message Cause: "  << message << endl;
      } break;
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;

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

        char buf[1024];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
        bsl::ostream out(&obuf);

        if (verbose) cout << "\nUsage example 1" << endl;

        ball::Transmission::Cause cause = ball::Transmission::e_TRIGGER_ALL;
        const char *asciiCause = ball::Transmission::toAscii(cause);

        ASSERT(0 == strcmp(asciiCause, "TRIGGER_ALL"));

        out << cause;

        ASSERT(0 == strncmp(buf, "TRIGGER_ALL", 11));

        if (veryVerbose) { out << ends; cout << buf << endl; }
        out.seekp(0);

        if (verbose) cout << "\nUsage example 2" << endl;

        my_Logger logger(out);

        const char *MSG_BALL_PASSTHROUGH = "report relatively minor problem";
        const char *MSG_BALL_TRIGGER     =
                                      "report serious thread-specific problem";
        const char *MSG_BALL_TRIGGER_ALL = "report process-impacting problem";

        logger.publish(MSG_BALL_PASSTHROUGH,
                       ball::Transmission::e_PASSTHROUGH);
        logger.publish(MSG_BALL_TRIGGER,
                       ball::Transmission::e_TRIGGER);
        logger.publish(MSG_BALL_TRIGGER_ALL,
                       ball::Transmission::e_TRIGGER_ALL);

        if (veryVerbose) { out << ends; cout << buf << endl; }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        //
        // Concerns:
        //   The following must hold for the enumeration under test:
        //     - The enumerator values must be consecutive integers in the
        //       range [0 .. LENGTH - 1].
        //     - The string representation of the enumerator values must be
        //       correct (and, hence, unique).
        //
        // Plan:
        //   Ensure enumerator values are consecutive integers in the range
        //   [0 .. LENGTH - 1] and that all names are unique.  Verify that the
        //   'toAscii' function produces strings that are identical to their
        //   respective enumerator symbols.  Verify that the output operator
        //   produces the same respective string values that would be produced
        //   by 'toAscii' (note that this is testing streams convertible to
        //   standard 'ostream' streams and the 'print' method).  Also verify
        //   the ascii representation and 'ostream' output for invalid
        //   enumerator values.
        //
        // Testing:
        //   print(bsl::ostream& stream, ball::Transmission::Cause value);
        //   enum Cause { ... };
        //   enum { LENGTH = ... };
        //   char *toAscii(ball::Transmission::Cause value);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        static const struct {
            Enum         d_enum;   // Enumerated Value
            const char  *d_ascii;  // String Representation
        } DATA[] = {
            // Enumerated Value                 String Representation
            // ----------------                 ---------------------
            { Class::e_PASSTHROUGH,          "PASSTHROUGH"         },
            { Class::e_TRIGGER,              "TRIGGER"             },
            { Class::e_TRIGGER_ALL,          "TRIGGER_ALL"         },
            { Class::e_MANUAL_PUBLISH,       "MANUAL_PUBLISH"      },
            { Class::e_MANUAL_PUBLISH_ALL,   "MANUAL_PUBLISH_ALL"  },
        };

        enum { DATA_LENGTH = sizeof DATA / sizeof *DATA };

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT((int)DATA_LENGTH == (int)NUM_ENUMS);

        if (verbose)
            cout << "\nVerify enumerator values are sequential." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            ASSERTV(i, DATA[i].d_enum == i);
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
            const char *const FMT = 0 <= i && i < DATA_LENGTH
                                    ? DATA[i].d_ascii : UNKNOWN_FMT;

            if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;
            const char *const ACT = Class::toAscii(Enum(i));
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << ACT << endl <<endl;

            ASSERTV(i, 0 == strcmp(FMT, ACT));
            for (int j = 0; j < i; ++j) {  // Make sure ALL strings are unique.
                ASSERTV(i, j, 0 != strcmp(DATA[j].d_ascii, FMT));
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
            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream out(&obuf);
            out << Enum(i) << ends;
            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl <<endl;

            const int SZ = strlen(FMT) + 1;
            ASSERTV(i, SZ < SIZE);            // Check buffer is large enough.
            ASSERTV(i, XX == buf[SIZE - 1]);  // Check for overrun.
            ASSERTV(i, 0 == memcmp(buf, FMT, SZ));
            ASSERTV(i, 0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE - SZ));
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
