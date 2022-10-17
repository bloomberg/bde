// bdlt_fuzzutil.t.cpp                                                -*-C++-*-
#include <bdlt_fuzzutil.h>

#include <bslim_fuzzdataview.h>
#include <bslim_fuzzutil.h>
#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlt::FuzzUtil' provides a suite of functions for producing dates from fuzz
// data.  This test driver tests each implemented utility function
// independently.
//
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 3] Date consumeDate(FuzzDataView *);
// [ 2] Date consumeDateInRange(FuzzDataView *, begin, end);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef bdlt::FuzzUtil Util;

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test)  { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //:
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
///Example 1: Provide 'bdlt::Datetime' within a Range
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The provided fuzz data is here represented by an array of bytes:
//..
   const uint8_t data[] = {0x8A, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                           0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//..
// First, we default construct a 'bslim::FuzzDataView' object, 'fdv':
//..
   bslim::FuzzDataView fdv(data, sizeof(data));
//
   ASSERT(12 == fdv.length());
//..
// Next, we construct 'Date' objects to represent the 'begin' and 'end' of the
// time interval in which we wish to construct our new 'Date' from the fuzz
// data:
//..
   bdlt::Date begin(1833, 5, 7);
   bdlt::Date end(1897, 4, 3);
//..
// Finally, we crate a 'Date' object, 'within', by employing 'bdlt_fuzzutil':
//..
   bdlt::Date within = Util::consumeDateInRange(&fdv, begin, end);
//
   ASSERT(begin  <= within);
   ASSERT(within <= end);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'consumeDate'
        //
        // Concerns:
        //: 1 The function returns valid 'Date' values.
        //:
        //: 2 The function uses the expected number of bytes (i.e., 3) of fuzz
        //:   data to produce the 'Date'.
        //
        // Plan:
        //: 1 Create a data array of size 30 bytes to simulate fuzz data.
        //:
        //: 2 Create a 'FuzzDataView' from the fuzz data array.
        //:
        //: 3 Repeat the following steps 10 times.  (C-1..2)
        //:
        //:   1 Create a 'Date' object using the 'FuzzDataView'.
        //:
        //:   2 Verify that the function returns valid 'Date' values.  (C-1)
        //:
        //:   3 Verify that the number of bytes consumed is what was expected.
        //:     (C-2)
        //
        // Testing:
        //   Date consumeDate(FuzzDataView *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'consumeDate'" << endl
                 << "====================" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                          0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                          0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                          0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                          0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};

        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
        const int           NUM_TESTS = 10;

        for (int ti = 0; ti < NUM_TESTS; ++ti) {

            bsl::size_t beforeBytes = fdv.length();
            bdlt::Date  d = Util::consumeDate(&fdv);
            bsl::size_t afterBytes = fdv.length();

            ASSERT(
                bdlt::Date::isValidYearMonthDay(d.year(), d.month(), d.day()));
            ASSERT(3 == beforeBytes - afterBytes);

            if (veryVerbose) {
                T_ P_(ti) P(d);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'consumeDateInRange'
        //
        // Concerns:
        //: 1 The function returns valid 'Date' values within the specified
        //:   range.
        //:
        //: 2 The function uses the expected number of bytes of fuzz data to
        //:    produce the 'Date'.
        //
        // Plan:
        //: 1 Create a data array of size 30 bytes to simulate fuzz data.
        //:
        //: 2 Create a 'FuzzDataView' from the fuzz data array.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid date ranges in columns 'BEGIN' and
        //:     'END'.
        //:
        //:   2 Additionally, provide a column, 'NUM_BYTES', for the expected
        //:     number of consumed bytes.
        //:
        //: 4 For each row in the table described in P-3:  (C-1..2)
        //:
        //:   1 Create a 'Date' object by invoking 'consumeDateInRange' with
        //:     the 'FuzzDataView' and the date range from the row.
        //:
        //:   2 Verify that the function returns dates within the specified
        //:     date range.  (C-1)
        //:
        //:   3 Verify that the number of bytes consumed is what was specified
        //:     in the table.  (C-2)
        //
        // Testing:
        //   Date consumeDateInRange(FuzzDataView *, begin, end);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'consumeDateInRange'" << endl
                 << "===========================" << endl;

        const bsl::uint8_t  FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                           0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                           0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                           0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                           0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};
        bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));

        static const struct {
            int              d_line;     // source line number
            bdlt::Date       d_begin;    // expected result
            bdlt::Date       d_end;      // expected result
            bsl::size_t      d_numBytes; // expected number of consumed bytes
        } DATA[] = {
            //LN          BEGIN                     END              NUM_BYTES
            //--  -----------------------  ------------------------  ---------
            { L_, bdlt::Date(1900, 2, 28), bdlt::Date(1900, 3,  28),    1 },
            { L_, bdlt::Date(1999, 1, 31), bdlt::Date(2010, 1,  11),    2 },
            { L_, bdlt::Date(2000, 1, 31), bdlt::Date(2000, 12, 31),    2 },
            { L_, bdlt::Date(2000, 2, 29), bdlt::Date(2001, 7,  28),    2 },
            { L_, bdlt::Date(2001, 4, 30), bdlt::Date(2010, 8,  28),    2 },
            { L_, bdlt::Date(2003, 2, 28), bdlt::Date(2030, 9,  22),    2 },
            { L_, bdlt::Date(2004, 2, 29), bdlt::Date(2044, 2,  28),    2 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const bdlt::Date  BEGIN     = DATA[ti].d_begin;
            const bdlt::Date  END       = DATA[ti].d_end;
            const bsl::size_t NUM_BYTES = DATA[ti].d_numBytes;

            bsl::size_t beforeBytes = fdv.length();
            bdlt::Date  d = Util::consumeDateInRange(&fdv, BEGIN, END);
            bsl::size_t afterBytes = fdv.length();

            ASSERT(
                bdlt::Date::isValidYearMonthDay(d.year(), d.month(), d.day()));
            ASSERT(BEGIN     <= d);
            ASSERT(d         <= END);
            ASSERT(NUM_BYTES == beforeBytes - afterBytes);

            if (veryVerbose) {
                T_ P_(LINE) P_(BEGIN) P_(END) P(d);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform a test of the primary utilities.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bsl::uint8_t FUZZ_DATA[] = {0x43, 0x19, 0x0D, 0x44, 0x37, 0x0D,
                                          0x28, 0xDB, 0x01, 0xB4, 0x52, 0x41,
                                          0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA,
                                          0x20, 0x24, 0xF8, 0xD9, 0x1F, 0x14,
                                          0x7C, 0x5C, 0xDD, 0x6F, 0xB3, 0xD7};
        int                testIters   = 10;
        {
            bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
            ASSERTV(30 == fdv.length());

            if (veryVerbose) {
                cout << "Testing 'consumeDate'" << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                bdlt::Date date = Util::consumeDate(&fdv);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(date)
                }
                int y, m, d;
                date.getYearMonthDay(&y, &m, &d);
                ASSERT(bdlt::Date::isValidYearMonthDay(y, m, d));
            }
        }
        {
            bslim::FuzzDataView fdv(FUZZ_DATA, sizeof(FUZZ_DATA));
            ASSERTV(30 == fdv.length());

            bdlt::Date begin(1685, 3, 21);
            bdlt::Date end(1750, 7, 28);

            if (veryVerbose) {
                cout << "Testing 'consumeDateInRange'" << endl;
            }
            for (int j = 0; j < testIters; ++j) {
                bdlt::Date d = Util::consumeDateInRange(&fdv, begin, end);
                if (veryVeryVerbose) {
                    T_ T_ P_(fdv.length()) P(d)
                }
                ASSERT(begin <= d);
                ASSERT(d     <= end);
            }
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
