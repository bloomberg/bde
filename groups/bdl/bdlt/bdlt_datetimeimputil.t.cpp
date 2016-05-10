// bdlt_datetimeimputil.t.cpp                                         -*-C++-*-
#include <bdlt_datetimeimputil.h>

#include <bdlt_date.h>
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a single utility 'struct', that defines
// a set of constants and methods to obtain pointers to 'bdlt::Datetime'
// suitable for use during static initialization.  The component can be tested
// in a test case that simply checks the values defined in the 'struct' and one
// that verifies the returned pointers are correct.
// ----------------------------------------------------------------------------
// [ 1] static const bsls::Types::Uint64 k_0001_01_01_VALUE;
// [ 1] static const bsls::Types::Uint64 k_1970_01_01_VALUE;
// [ 1] static const bsls::Types::Uint64 k_MAX_VALUE;
// [ 2] static const bdlt::Datetime *epoch_0001_01_01();
// [ 2] static const bdlt::Datetime *epoch_1970_01_01();
// [ 2] static const bdlt::Datetime *epoch_max();
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// To obtain the 'bdlt::Datetime' internal value for 0001/01/01 at
// static-initialization time:
//..
    static const bdlt::Datetime *firstDatetime =
                                     bdlt::DatetimeImpUtil::epoch_0001_01_01();

    ASSERT(reinterpret_cast<const bdlt::Datetime *>(
                 &bdlt::DatetimeImpUtil::k_0001_01_01_VALUE) == firstDatetime);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'DatetimeImpUtil' METHODS
        //
        // Concerns:
        //: 1 That each method defined in 'DatetimeImpUtil' is correct.
        //
        // Plan:
        //: 1 Using brute force, verify every returned pointer.  (C-1)
        //
        // Testing:
        //   static const bdlt::Datetime *epoch_0001_01_01();
        //   static const bdlt::Datetime *epoch_1970_01_01();
        //   static const bdlt::Datetime *epoch_max();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'DatetimeImpUtil' METHODS" << endl
                          << "=================================" << endl;

        ASSERT(reinterpret_cast<const bdlt::Datetime *>(
                                 &bdlt::DatetimeImpUtil::k_0001_01_01_VALUE) ==
                                    bdlt::DatetimeImpUtil::epoch_0001_01_01());

        ASSERT(reinterpret_cast<const bdlt::Datetime *>(
                                 &bdlt::DatetimeImpUtil::k_1970_01_01_VALUE) ==
                                    bdlt::DatetimeImpUtil::epoch_1970_01_01());

        ASSERT(reinterpret_cast<const bdlt::Datetime *>(
                                        &bdlt::DatetimeImpUtil::k_MAX_VALUE) ==
                                           bdlt::DatetimeImpUtil::epoch_max());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'DatetimeImpUtil' VALUES
        //
        // Concerns:
        //: 1 That each value defined in 'DatetimeImpUtil' is correct.
        //
        // Plan:
        //: 1 Using brute force, compare each explicit value defined in
        //:   'DatetimeImpUtil' to a value calculated using 'Date' and
        //:   'TimeUnitRatio'.  (C-1)
        //
        // Testing:
        //   static const bsls::Types::Uint64 k_0001_01_01_VALUE;
        //   static const bsls::Types::Uint64 k_1970_01_01_VALUE;
        //   static const bsls::Types::Uint64 k_MAX_VALUE;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'DatetimeImpUtil' VALUES" << endl
                          << "================================" << endl;

        {
            bsls::Types::Uint64 value;

            value = static_cast<bsls::Types::Uint64>(
                                      bdlt::Date(1, 1, 1) - bdlt::Date()) << 37
                                    | 1ULL << 63;

            ASSERTV(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE,
                    value,
                    bdlt::DatetimeImpUtil::k_0001_01_01_VALUE == value);
        }

        {
            bsls::Types::Uint64 value;

            value = static_cast<bsls::Types::Uint64>(
                                   bdlt::Date(1970, 1, 1) - bdlt::Date()) << 37
                                 | 1ULL << 63;

            ASSERTV(bdlt::DatetimeImpUtil::k_1970_01_01_VALUE,
                    value,
                    bdlt::DatetimeImpUtil::k_1970_01_01_VALUE == value);
        }

        {
            bsls::Types::Uint64 value;

            value = static_cast<bsls::Types::Uint64>(
                                 bdlt::Date(9999, 12, 31) - bdlt::Date()) << 37
                               | 1ULL << 63;
            value = value + bdlt::TimeUnitRatio::k_US_PER_D;

            ASSERTV(bdlt::DatetimeImpUtil::k_MAX_VALUE,
                    value,
                    bdlt::DatetimeImpUtil::k_MAX_VALUE == value);
        }

        if (veryVerbose) {
            P(bdlt::DatetimeImpUtil::k_0001_01_01_VALUE);
            P(bdlt::DatetimeImpUtil::k_1970_01_01_VALUE);
            P(bdlt::DatetimeImpUtil::k_MAX_VALUE);
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
// Copyright 2016 Bloomberg Finance L.P.
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
