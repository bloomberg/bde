// baljsn_decoderoptionsutil.t.cpp                                    -*-C++-*-
#include <baljsn_decoderoptionsutil.h>

#include <baljsn_decoderoptions.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cstdlib.h> // 'bsl::atoi'
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
// The component under test, 'baljsn_decoderoptionutils, implements a utility
// for setting 'baljsn::DecoderOptions' object to particular configuration.
// Currently there a single function, 'setMode', is provided, and two modes,
// "default" and "strict" are defined.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] static void setMode(DecoderOptions *options, Mode mode);
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

typedef baljsn::DecoderOptionsUtil Util;
typedef baljsn::DecoderOptions     Opts;

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
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
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
///Example 1: Setting 'baljsn::DecoderOptions' for Strictness
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Every call to one of the (non-deprecated) 'decode' functions of
// 'baljsn::Decoder' requires the user to provide a 'baljsn::DecoderOptions'
// object that allows the user to fine-tune the rules used when decoding the
// JSON document.  The 'setMode' function of this utility provides a convenient
// way to set the option attributes to a combination that is deemed "strict"
// (i.e., strictly complying with the rules of the JSON grammar).
//
// First, create a 'baljsn::DecoderOptions' object:
//..
    baljsn::DecoderOptions options;
//..
// Now, set the option values for strict compliance:
//..
    baljsn::DecoderOptionsUtil::setMode(
                                &options,
                                baljsn::DecoderOptionsUtil::e_STRICT_20240423);
//..
// Finally, should there be a need, 'options' can be adjusted to a laxer set of
// rules by adjusting individual attributes or, if the original set of default
// attributes is needed, by using 'setMode':
//..
    baljsn::DecoderOptionsUtil::setMode(&options,
                                        baljsn::DecoderOptionsUtil::e_DEFAULT);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST 'setMode'
        //
        // Concerns:
        //: 1 When setting mode 'e_STRICT_20240423' all
        //:   'baljsn::DecoderOptions' attributes relevant to strict are set to
        //:   their expected values *and* no other attributes are changed.
        //:
        //: 2 When setting mode 'e_DEFAULT' all attributes (relevant to
        //:   strictness or not) are set to their default values.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 A sequence of ad-hoc tests.  (C-1,2)
        //:
        //: 2 Use 'BSLS_ASSERTTEST_*' macros to confirm pre-condition tests.
        //:  (C-3)
        //
        // Testing:
        //   static void setMode(DecoderOptions *options, Mode mode);
        // --------------------------------------------------------------------

        if (verbose) cout  << endl
                           << "TEST 'setMode'" << endl
                           << "==============" << endl;

        if (veryVerbose) cout << "Set to Strict  Mode" << endl;

        Opts mX; const Opts& X = mX;

        Util::setMode(&mX, Util::e_STRICT_20240423);

        // Set the two non-"strictness" attributes to non-default  values.
        const int  MAX_DEPTH_A             =  Opts().maxDepth() + 1;
        const bool SKIP_UNKNOWN_ELEMENTS_A = !Opts().skipUnknownElements();

        mX.setMaxDepth(MAX_DEPTH_A);
        mX.setSkipUnknownElements(SKIP_UNKNOWN_ELEMENTS_A);

        // Confirm that the "strictness" attributes have the expected value.
        ASSERT(X.validateInputIsUtf8()             == true );
        ASSERT(X.allowConsecutiveSeparators()      == false);
        ASSERT(X.allowFormFeedAsWhitespace()       == false);
        ASSERT(X.allowUnescapedControlCharacters() == false);

        // Confirm that the "strictness" attributes were indeed changed.
        ASSERT(     X.validateInputIsUtf8()
            != Opts().validateInputIsUtf8());
        ASSERT(     X.allowConsecutiveSeparators()
            != Opts().allowConsecutiveSeparators());
        ASSERT(     X.allowFormFeedAsWhitespace()
            != Opts().allowFormFeedAsWhitespace());
        ASSERT(     X.allowUnescapedControlCharacters()
            != Opts().allowUnescapedControlCharacters());

        // Confirm that the non-"strictness" attributes were *not* changed.
        ASSERT(X.maxDepth()            ==             MAX_DEPTH_A);
        ASSERT(X.skipUnknownElements() == SKIP_UNKNOWN_ELEMENTS_A);

        if (veryVerbose) cout << "Set to Default" << endl;

        Util::setMode(&mX, Util::e_DEFAULT);
        ASSERT(X == Opts());

        if (veryVerbose) cout << "Negative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Opts opts;
            ASSERT_PASS((Util::setMode(&opts, Util::e_DEFAULT)));
            ASSERT_FAIL((Util::setMode(   0, Util::e_DEFAULT)));
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
