// ball_severityutil.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_severityutil.h>

#include <ball_severity.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>
#include <bsl_new.h>          // placement 'new' syntax

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
// This component currently has two procedures: The 'fromAsciiCaseless'
// procedure is an inverse operation on 'ball::Severity::toAscii'.  The
// 'isValidNameCaseless' procedure is a helper to 'fromAsciiCaseless'.  The
// test strategy is to first validate that 'fromAsciiCaseless' correctly
// interprets the small set of C-style strings that represent the enumerated
// values of 'ball::Severity::Level' and then confirm that it rejects as
// invalid strings that are small perturbations from the members of the valid
// set.  Once some confidence in 'fromAsciiCaseless' is established, it is
// checked for consistency with 'isValidNameCaseless' and then checked for
// consistency with 'ball::Severity::toAscii'.
//-----------------------------------------------------------------------------
// [ 1] static int fromAsciiCaseless(ball::Severity::Level *, const char *);
// [ 5] static int fromAscii(ball::Severity::Level *, const char *);
// [ 2] static bool isValidNameCaseless(const char *);
// [ 4] static bool isValidName(const char *);
//-----------------------------------------------------------------------------
// [ 3] TESTING: consistency with 'ball::Severity::toAscii'
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
// GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::Severity     Class;
typedef Class::Level       Enum;
typedef ball::SeverityUtil Util;

const Enum initialValue = Class::e_OFF;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

        if (verbose) cout << "USAGE EXAMPLE"
                          << "=============" << endl;

///Usage
///-----
// In this example, we show how to validate that a set of C-style strings
// correspond to 'ball::Severity::Level' enumerators, and then use those
// strings to generate enumerator values that, in turn, may be used to
// administer a logger manager.  Here, for convenience, we define our strings
// in an array, much as how we might receive them from a command line:
//..
      const char *argv[] = {
          "INFO",   // record
          "WARN",   // pass
          "ERROR",  // trigger
          "FATAL"   // trigger-all
      };
//
      ASSERT(ball::SeverityUtil::isValidNameCaseless(argv[0]));
      ASSERT(ball::SeverityUtil::isValidNameCaseless(argv[1]));
      ASSERT(ball::SeverityUtil::isValidNameCaseless(argv[2]));
      ASSERT(ball::SeverityUtil::isValidNameCaseless(argv[3]));
//
      ball::Severity::Level record;
      ball::Severity::Level pass;
      ball::Severity::Level trigger;
      ball::Severity::Level triggerAll;
//
      ASSERT(0 == ball::SeverityUtil::fromAsciiCaseless(&record,     argv[0]));
      ASSERT(0 == ball::SeverityUtil::fromAsciiCaseless(&pass,       argv[1]));
      ASSERT(0 == ball::SeverityUtil::fromAsciiCaseless(&trigger,    argv[2]));
      ASSERT(0 == ball::SeverityUtil::fromAsciiCaseless(&triggerAll, argv[3]));
//
      ASSERT(ball::Severity::e_INFO  == record);
      ASSERT(ball::Severity::e_WARN  == pass);
      ASSERT(ball::Severity::e_ERROR == trigger);
      ASSERT(ball::Severity::e_FATAL == triggerAll);
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BASIC TEST 'isValidName'
        //   Test that 'isValidName' correctly classifies input
        //   strings.
        //
        // Concerns:
        //   - Are 'fromAscii' and 'isvalidName' in agreement
        //     in their interpretation of input?
        //
        // Plan:
        //   - Replicate the basic test of 'fromAscii' to test
        //     'isValidName'.  Where the return status of
        //     'fromAscii' is 0 the return value of
        //     'isValidName' should be 'true', and 'false' otherwise.
        //
        // Testing:
        //   bool isValidName(const char *severityName);
        // --------------------------------------------------------------------

        // This function and test case are deprecated and maintained in the
        // internal code base only.

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC TEST 'fromAscii'
        //    Test that 'fromAscii' correctly returns the enumerated
        //    integer value that is named by its argument.
        //
        // Concerns:
        //   - Does 'fromAscii' work correctly for each of the valid
        //     inputs?
        //   - Does 'fromAscii' correctly return an error condition
        //     when given incorrect input?
        //
        // Plan:
        //   - Confirm that 'fromAscii' returns success (0), and loads
        //     the correct severity value for each each of the valid
        //     input strings.
        //   - Confirm that 'fromAscii' correctly returns invalid
        //     status (non-zero) and leaves unchanged the severity value at
        //     the load address when given invalid input.  A set of invalid
        //     input is created by "ad hoc" perturbations on the set of
        //     valid inputs.
        //
        // Testing:
        //   int fromAscii(ball::Severity::Level *result,
        //                 const char           *severityName);
        // --------------------------------------------------------------------

        // This function and test case are deprecated and maintained in the
        // internal code base only.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: consistency with 'ball::Severity::toAscii'
        //    The 'fromAsciiCaseless' procedure should be a functional
        //    inverse to 'ball::Severity::ToAscii'.
        //
        // Concerns:
        //:  1 The implementations of 'ball::Severity' and 'ball::SeverityUtil'
        //     may become out-of-sync.  This test is provided to aid
        //     maintenance.
        //
        //  Plan:
        //    - Test the conversion from severity level to ascii and
        //      from ascii back to severity level
        //      for the entire set of enumerated severity values.
        //
        // Testing:
        //    int fromAsciiCaseless(ball::Severity::Level *result,
        //                  const char           *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "CONSISTENCY WITH 'ToAscii'" << endl
                          << "========================="  << endl;
            Enum VALUES[] = {
               Class::e_OFF,
               Class::e_FATAL,
               Class::e_ERROR,
               Class::e_WARN,
               Class::e_INFO,
               Class::e_DEBUG,
               Class::e_TRACE,
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Enum  SEVERITY = VALUES[i];
                Enum result = initialValue;
                int  status = Util::fromAsciiCaseless(&result,
                                                     Class::toAscii(SEVERITY));
                LOOP_ASSERT(i, 0 == status && SEVERITY == result);
            }
        } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST 'isValidNameCaseless'
        //   Test that 'isValidNameCaseless' correctly classifies input
        //   strings.
        //
        // Concerns:
        //   - Are 'fromAsciiCaseless' and 'isvalidName' in agreement
        //     in their interpretation of input?
        //
        // Plan:
        //   - Replicate the basic test of 'fromAsciiCaseless' to test
        //     'isValidNameCaseless'.  Where the return status of
        //     'fromAsciiCaseless' is 0 the return value of
        //     'isValidNameCaseless' should be 'true', and 'false' otherwise.
        //
        // Testing:
        //   bool isValidNameCaseless(const char *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'isValidNameCaseless'" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            bool        d_result;       // expected return value
         } DATA[] = {
           ///d_linenum  d_severityName   d_result
           ///---------  --------------   --------
            { L_,        "OFF",           true,    },
            { L_,        "FATAL",         true,    },
            { L_,        "ERROR",         true,    },
            { L_,        "WARN",          true,    },
            { L_,        "INFO",          true,    },
            { L_,        "DEBUG",         true,    },
            { L_,        "TRACE",         true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },

            { L_,        "OF",            false,   },
            { L_,        "FATA",          false,   },
            { L_,        "ERRO",          false,   },
            { L_,        "WAR",           false,   },
            { L_,        "INF",           false,   },
            { L_,        "DEBU",          false,   },
            { L_,        "TRAC",          false,   },
            { L_,        "NON",           false,   },

            { L_,        "OFFa",          false,   },
            { L_,        "FATALa",        false,   },
            { L_,        "ERRORa",        false,   },
            { L_,        "WARNa",         false,   },
            { L_,        "INFOa",         false,   },
            { L_,        "DEBUGa",        false,   },
            { L_,        "TRACEa",        false,   },
            { L_,        "NONEa",         false,   },

            { L_,        "xxyyzz",        false,   },
            { L_,        "xFATAL",        false,   },
            { L_,        "FATALy",        false,   },
         };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *SEVERITYNAME = DATA[i].d_severityName;
                const bool  RESULT       = DATA[i].d_result;
                bool result = Util::isValidNameCaseless(SEVERITYNAME);
                LOOP_ASSERT(LINE, result == RESULT);
            }
        } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST 'fromAsciiCaseless'
        //    Test that 'fromAsciiCaseless' correctly returns the enumerated
        //    integer value that is named by its argument.
        //
        // Concerns:
        //   - Does 'fromAsciiCaseless' work correctly for each of the valid
        //     inputs?
        //   - Does 'fromAsciiCaseless' correctly return an error condition
        //     when given incorrect input?
        //
        // Plan:
        //   - Confirm that 'fromAsciiCaseless' returns success (0), and loads
        //     the correct severity value for each each of the valid
        //     input strings.
        //   - Confirm that 'fromAsciiCaseless' correctly returns invalid
        //     status (non-zero) and leaves unchanged the severity value at
        //     the load address when given invalid input.  A set of invalid
        //     input is created by "ad hoc" perturbations on the set of
        //     valid inputs.
        //
        // Testing:
        //   int fromAsciiCaseless(ball::Severity::Level *result,
        //                 const char           *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'fromAsciiCaseless'" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            int         d_status;       // expected status
            Enum        d_severity;     // expected severity
         } DATA[] = {
           ///d_linenum  d_severityName   d_status   d_severity
           ///---------  --------------   ---------  ----------
            { L_,        "OFF",             0,       Class::e_OFF   },
            { L_,        "FATAL",           0,       Class::e_FATAL },
            { L_,        "ERROR",           0,       Class::e_ERROR },
            { L_,        "WARN",            0,       Class::e_WARN  },
            { L_,        "INFO",            0,       Class::e_INFO  },
            { L_,        "DEBUG",           0,       Class::e_DEBUG },
            { L_,        "TRACE",           0,       Class::e_TRACE },
            { L_,        "oFf",             0,       Class::e_OFF   },
            { L_,        "fAtAl",           0,       Class::e_FATAL },
            { L_,        "eRrOr",           0,       Class::e_ERROR },
            { L_,        "wArN",            0,       Class::e_WARN  },
            { L_,        "iNfO",            0,       Class::e_INFO  },
            { L_,        "dEbUg",           0,       Class::e_DEBUG },
            { L_,        "tRaCe",           0,       Class::e_TRACE },

            { L_,        "OfF",             0,       Class::e_OFF   },
            { L_,        "FaTaL",           0,       Class::e_FATAL },
            { L_,        "ErRoR",           0,       Class::e_ERROR },
            { L_,        "WaRn",            0,       Class::e_WARN  },
            { L_,        "InFo",            0,       Class::e_INFO  },
            { L_,        "DeBuG",           0,       Class::e_DEBUG },
            { L_,        "TrAcE",           0,       Class::e_TRACE },

            { L_,        "OF",             -1,       initialValue },
            { L_,        "FATA",           -1,       initialValue },
            { L_,        "ERRO",           -1,       initialValue },
            { L_,        "WAR",            -1,       initialValue },
            { L_,        "INF",            -1,       initialValue },
            { L_,        "DEBU",           -1,       initialValue },
            { L_,        "TRAC",           -1,       initialValue },
            { L_,        "NON",            -1,       initialValue },

            { L_,        "OFFa",           -1,       initialValue },
            { L_,        "FATALa",         -1,       initialValue },
            { L_,        "ERRORa",         -1,       initialValue },
            { L_,        "WARNa",          -1,       initialValue },
            { L_,        "INFOa",          -1,       initialValue },
            { L_,        "DEBUGa",         -1,       initialValue },
            { L_,        "TRACEa",         -1,       initialValue },
            { L_,        "NONEa",          -1,       initialValue },

            { L_,        "xxyyzz",         -1,       initialValue },
            { L_,        "xFATAL",         -1,       initialValue },
            { L_,        "FATALy",         -1,       initialValue },
         };

         const int NUM_DATA = sizeof DATA / sizeof *DATA;

         for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE         = DATA[i].d_lineNum;
            const char *SEVERITYNAME = DATA[i].d_severityName;
            const int   STATUS       = DATA[i].d_status;
            const Enum  RESULT       = DATA[i].d_severity;
            Enum        result = initialValue;
            const int   status = Util::fromAsciiCaseless(&result,
                                                         SEVERITYNAME);
            LOOP_ASSERT(LINE, status == STATUS && result == RESULT);
         }
        } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } }

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
