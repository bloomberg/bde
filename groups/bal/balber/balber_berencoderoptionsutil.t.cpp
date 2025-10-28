// balber_berencoderoptionsutil.t.cpp                                 -*-C++-*-
#include <balber_berencoderoptionsutil.h>

#include <balber_berencoderoptions.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cstdlib.h> // `bsl::atoi`
#include <bsl_ostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The `struct` under test, `balber::BerEncoderOptionsUtil`, implements a
// utility for setting `balber::BerEncoderOptions` object to particular
// configuration.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] static void setMode(BerEncoderOptions *options, Mode mode);
// ----------------------------------------------------------------------------
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balber::BerEncoderOptionsUtil Util;
typedef balber::BerEncoderOptions     Opts;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout  << endl
                           << "USAGE EXAMPLE" << endl
                           << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting `balber::EncoderOptions` for Efficiency
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Every call to one of the `encode` functions of `balber::BerEncoder` requires
// the user to provide a `balber::BerEncoderOptions` object.  The `setMode`
// function of this utility provides a convenient way to set the option
// attributes to a combination that is deemed efficient for future decoding of
// the message.
//
// First, create a `balber::BerEncoderOptions` object:
// ```
   balber::BerEncoderOptions options;
// ```
// Now, set the option values for efficiency:
// ```
   balber::BerEncoderOptionsUtil::setMode(
                               &options,
                               balber::BerEncoderOptionsUtil::e_FAST_20250615);
// ```
// Finally, `options` can be adjusted to the set of default attributes:
// ```
   balber::BerEncoderOptionsUtil::setMode(
                                     &options,
                                     balber::BerEncoderOptionsUtil::e_DEFAULT);
// ```
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST `setMode`
        //
        // Concerns:
        // 1. When setting mode all attributes are set to their expected
        //    values.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. A sequence of ad-hoc tests.  (C-1)
        //
        // 2. Use `BSLS_ASSERTTEST_*` macros to confirm pre-condition tests.
        //    (C-2)
        //
        // Testing:
        //   static void setMode(EncoderOptions *options, Mode mode);
        // --------------------------------------------------------------------

        if (verbose) cout  << endl
                           << "TEST `setMode`" << endl
                           << "==============" << endl;

        const Opts D;

        Opts mX; const Opts& X = mX;

        {  // verify `e_DEFAULT`
            mX.setTraceLevel(17);
            mX.bdeVersionConformance() = 10;
            mX.setEncodeEmptyArrays(false);
            mX.setEncodeDateAndTimeTypesAsBinary(true);
            mX.setDatetimeFractionalSecondPrecision(4);
            mX.setDisableUnselectedChoiceEncoding(true);
            mX.setPreserveSignOfNegativeZero(true);
            mX.setEncodeArrayLengthHints(true);

            ASSERT(D.traceLevel()             != X.traceLevel());
            ASSERT(D.bdeVersionConformance()  != X.bdeVersionConformance());
            ASSERT(D.encodeEmptyArrays()      != X.encodeEmptyArrays());
            ASSERT(D.encodeDateAndTimeTypesAsBinary()
                                        != X.encodeDateAndTimeTypesAsBinary());
            ASSERT(D.datetimeFractionalSecondPrecision()
                                     != X.datetimeFractionalSecondPrecision());
            ASSERT(D.disableUnselectedChoiceEncoding()
                                       != X.disableUnselectedChoiceEncoding());
            ASSERT(D.preserveSignOfNegativeZero()
                                            != X.preserveSignOfNegativeZero());
            ASSERT(D.encodeArrayLengthHints() != X.encodeArrayLengthHints());

            Util::setMode(&mX, Util::e_DEFAULT);

            ASSERT(D.traceLevel()             == X.traceLevel());
            ASSERT(D.bdeVersionConformance()  == X.bdeVersionConformance());
            ASSERT(D.encodeEmptyArrays()      == X.encodeEmptyArrays());
            ASSERT(D.encodeDateAndTimeTypesAsBinary()
                                        == X.encodeDateAndTimeTypesAsBinary());
            ASSERT(D.datetimeFractionalSecondPrecision()
                                     == X.datetimeFractionalSecondPrecision());
            ASSERT(D.disableUnselectedChoiceEncoding()
                                       == X.disableUnselectedChoiceEncoding());
            ASSERT(D.preserveSignOfNegativeZero()
                                            == X.preserveSignOfNegativeZero());
            ASSERT(D.encodeArrayLengthHints() == X.encodeArrayLengthHints());
        }
        {  // verify `e_FAST_20250615`
            mX.setTraceLevel(17);
            mX.bdeVersionConformance() = 10;
            mX.setEncodeEmptyArrays(false);
            mX.setEncodeDateAndTimeTypesAsBinary(false);
            mX.setDatetimeFractionalSecondPrecision(4);
            mX.setDisableUnselectedChoiceEncoding(true);
            mX.setPreserveSignOfNegativeZero(true);
            mX.setEncodeArrayLengthHints(false);

            ASSERT(D.traceLevel()            != X.traceLevel());
            ASSERT(D.bdeVersionConformance() != X.bdeVersionConformance());
            ASSERT(D.encodeEmptyArrays()     != X.encodeEmptyArrays());
            ASSERT(true                 != X.encodeDateAndTimeTypesAsBinary());
            ASSERT(D.datetimeFractionalSecondPrecision()
                                     != X.datetimeFractionalSecondPrecision());
            ASSERT(D.disableUnselectedChoiceEncoding()
                                       != X.disableUnselectedChoiceEncoding());
            ASSERT(D.preserveSignOfNegativeZero()
                                            != X.preserveSignOfNegativeZero());
            ASSERT(true                      != X.encodeArrayLengthHints());

            Util::setMode(&mX, Util::e_FAST_20250615);

            ASSERT(D.traceLevel()            == X.traceLevel());
            ASSERT(D.bdeVersionConformance() == X.bdeVersionConformance());
            ASSERT(D.encodeEmptyArrays()     == X.encodeEmptyArrays());
            ASSERT(true                 == X.encodeDateAndTimeTypesAsBinary());
            ASSERT(D.datetimeFractionalSecondPrecision()
                                     == X.datetimeFractionalSecondPrecision());
            ASSERT(D.disableUnselectedChoiceEncoding()
                                       == X.disableUnselectedChoiceEncoding());
            ASSERT(D.preserveSignOfNegativeZero()
                                            == X.preserveSignOfNegativeZero());
            ASSERT(true                       == X.encodeArrayLengthHints());
        }

        if (veryVerbose) cout << "Negative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Opts opts;
            ASSERT_PASS((Util::setMode(&opts, Util::e_DEFAULT)));
            ASSERT_FAIL((Util::setMode(    0, Util::e_DEFAULT)));
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

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
