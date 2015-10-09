// ball_transmission.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_transmission.h>

#include <bsl_cstdlib.h>                       // atoi()
#include <bsl_cstring.h>                       // strcmp(), memcmp(), memcpy()

#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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

typedef ball::Transmission  Class;
typedef Class::Cause       Enum;

const int NUM_ENUMS = Class::e_LENGTH;

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
using namespace bsl;  // automatically added by script

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

        char buf[1024];  memset(buf, 0xff, sizeof buf);  // Scribble on buf.
        ostrstream out(buf, sizeof buf);

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

        const int DATA_LENGTH = sizeof DATA / sizeof *DATA;

        const char *const UNKNOWN_FMT = "(* UNKNOWN *)";

        int i; // loop index variable -- keeps MS compiler from complaining

        if (verbose) cout << "\nVerify table length is correct." << endl;

        ASSERT(DATA_LENGTH == NUM_ENUMS);

        if (verbose)
            cout << "\nVerify enumerator values are sequential." << endl;

        for (i = 0; i < DATA_LENGTH; ++i) {
            LOOP_ASSERT(i, DATA[i].d_enum == i);
        }

        if (verbose) cout << "\nVerify the toAscii function." << endl;

        for (i = -1; i < DATA_LENGTH + 1; ++i) {  // also check UNKNOWN_FMT
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
