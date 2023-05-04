// balxml_encodingstyle.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_encodingstyle.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

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
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------


static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // -------------------------------------------------------------------
        // TESTING 'toString' and 'fromString'
        //
        // Concerns:
        //: 1 The 'toString' method returns the expected string representation
        //:   for each enumerator.
        //: 2 The 'fromString' method returns the right enumeration value.
        //
        // Plan:
        //: 1 Test 'toString' with different enumerator values and checking
        //:   against the expected string representation.
        //: 2 Test 'fromString' with different string values and checking
        //:   against the expected enumerator value.
        //
        // Testing:
        //   const char *toString(Value val);
        //   int fromString(Value *result, const char *str, int len);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'toString' and 'fromString'"
                          << endl << "==================================="
                          << endl;

        typedef balxml::EncodingStyle::Value Enum;
        typedef balxml::EncodingStyle        Obj;

        if (verbose) cout << "\nTesting 'toString'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                Enum        d_value;    // enumerator value
                const char *d_exp;      // expected result
            } DATA[] = {
                // line         enumerator value        expected result
                // ----    -----------------------      -----------------
                {  L_,     Obj::COMPACT,           "COMPACT"         },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::e_COMPACT,                "COMPACT"         },
#endif // BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::PRETTY,            "PRETTY"          },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::e_PRETTY,                 "PRETTY"          },
#endif // BDE_OMIT_INTERNAL_DEPRECATED
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const Enum  VALUE = DATA[ti].d_value;
                const char *EXP   = DATA[ti].d_exp;

                const char *result = Obj::toString(VALUE);

                if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

                LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
                LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
            }
        }

        if (verbose) cout << "\nTesting 'fromString'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                Enum        d_expValue; // expected enumerator value
                const char *d_inputStr; // input string
            } DATA[] = {
                // line         enumerator value        expected result
                // ----    -----------------------      -----------------
                {  L_,     Obj::COMPACT,                "COMPACT"         },
                {  L_,     Obj::COMPACT,                "compact"         },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::COMPACT,                "BAEXML_COMPACT"  },
                {  L_,     Obj::COMPACT,                "baexml_compact"  },
#endif
                {  L_,     Obj::PRETTY,                 "PRETTY"          },
                {  L_,     Obj::PRETTY,                 "pretty"          },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::PRETTY,                 "BAEXML_PRETTY"   },
                {  L_,     Obj::PRETTY,                 "baexml_pretty"   },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_lineNum;
                const Enum  EXP_VALUE = DATA[ti].d_expValue;
                const char *STR       = DATA[ti].d_inputStr;

                Enum value;
                int rc = Obj::fromString(&value,
                                         STR,
                                         static_cast<int>(strlen(STR)));

                if (veryVerbose) { T_; P_(ti); P_(value); P_(STR); P(rc); }

                LOOP_ASSERT(LINE, !rc);
                LOOP_ASSERT(LINE, EXP_VALUE == value);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "Basic Attribute Test" << bsl::endl
                      << "====================" << bsl::endl;
        }

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
