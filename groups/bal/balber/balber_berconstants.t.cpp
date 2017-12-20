// balber_berconstants.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <balber_berconstants.h>

#include <bslim_testutil.h>

#include <bsl_sstream.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//

// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
            verbose = argc > 2;
        veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Output (<<) Operator"
                               << "\n============================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting TagClass." << bsl::endl;
        {
            const balber::BerConstants::TagClass X
                                           = balber::BerConstants::e_UNIVERSAL;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "UNIVERSAL" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                         = balber::BerConstants::e_APPLICATION;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "APPLICATION" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                    = balber::BerConstants::e_CONTEXT_SPECIFIC;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "CONTEXT_SPECIFIC" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                             = balber::BerConstants::e_PRIVATE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIVATE" == ss.str());
        }

        if (verbose) bsl::cout << "\nTesting TagType." << bsl::endl;
        {
            const balber::BerConstants::TagType X
                                           = balber::BerConstants::e_PRIMITIVE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIMITIVE" == ss.str());
        }
        {
            const balber::BerConstants::TagType X
                                         = balber::BerConstants::e_CONSTRUCTED;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "CONSTRUCTED" == ss.str());
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
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
