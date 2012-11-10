// bsltf_testvaluesarray.t.cpp                                        -*-C++-*-

#include <bsltf_testvaluesarray.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

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
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // end anonymous namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

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
//                      USAGE EXAMPLE
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

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

// Finally, we invoke the test function to verify our function is implemented
// correctly.  The test function to run without triggering the 'assert'
// statement:
//..
    runTest<char>();
//..
      }
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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
