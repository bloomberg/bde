// bsltf_testvaluesarray.t.cpp                                        -*-C++-*-

#include <bsltf_testvaluesarray.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ...
//
//-----------------------------------------------------------------------------
// [ ] ....
//-----------------------------------------------------------------------------
// [1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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
//                              USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 1: Testing a Simple Template Function
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we have a function that we would like to test.  This function
// take in a range defined by two input iterators and returns the largest value
// in that range.
//
// First, we define the function we would like to test:
//..
    template <class VALUE, class INPUT_ITERATOR>
    VALUE myMaxValue(INPUT_ITERATOR first, INPUT_ITERATOR last)
        // Return the largest value referred to by the iterators in the range
        // beginning at the specified 'first' and up to, but not including, the
        // specified 'last'.  The behavior is undefined unless [first, last)
        // specifies a valid range and 'first != last'.
    {
        ASSERT(first != last);

        VALUE largestValue(*first);
        ++first;
        for(;first != last; ++first) {
            // Store in temporary variable to avoid dereferencing twice.
            const VALUE& temp = *first;
            if (largestValue < temp) {
                largestValue = temp;
            }
        }
        return largestValue;
    }
//..
// Next, we implement a test function 'runTest' that allows the function to be
// tested with different types:
//..
    template <class VALUE>
    void runTest()
    {
//..
//  Then, we define a set of test values and expected results:
//..
        struct {
            const char *d_spec;
            const char  d_result;
        } DATA[] = {
            { "A",     'A' },
            { "ABC",   'C' },
            { "ADCB",  'D' },
            { "EDCBA", 'E' }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
//..
//  Now, for each set of test values, verify that the function return the
//  expected result.
//..
        for (int i = 0; i < NUM_DATA; ++i) {
            const char *const SPEC = DATA[i].d_spec;
            const VALUE       EXP  =
                  bsltf::TemplateTestFacility::create<VALUE>(DATA[i].d_result);

            bsltf::TestValuesArray<VALUE> values(SPEC);
            ASSERT(EXP == myMaxValue<VALUE>(values.begin(), values.end()));
        }
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns
        //: 1 The usage example compiles and runs correctly.
        //
        // Plan:
        //: 1 Compile and run the usage example (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we invoke the test function to verify our function is implemented
// correctly.  The test function to run without triggering the 'assert'
// statement:
//..
    runTest<char>();
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bsltf::TestValuesArray<char> X;
        bsltf::TestValuesArray<char> Y("ABC");

        ASSERTV(X.size(), 52 == X.size());

        bsltf::TestValuesArray<char>::iterator IT = Y.begin();
        ASSERTV(IT != Y.end());
        ASSERTV('A' == *IT);
        ++IT;
        ASSERTV(IT != Y.end());
        ASSERTV('B' == *IT);
        ++IT;
        ASSERTV(IT != Y.end());
        ASSERTV('C' == *IT);
        ++IT;
        ASSERTV(IT == Y.end());

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
