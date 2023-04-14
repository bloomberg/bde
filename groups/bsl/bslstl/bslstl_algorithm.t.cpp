// bslstl_algorithm.t.cpp                                             -*-C++-*-
#include <bslstl_algorithm.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
#include <bslstl_compare.h>
#endif

#include <bsltf_testvaluesarray.h>

#include <functional>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>      // atoi
#include <string.h>      // strlen

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// The component under test provides implementations for algorithms not
// provided by the underlying standard library implementation.
// ----------------------------------------------------------------------------
//
// [ 1] bool all_of (InputIter first, InputIter last, PREDICATE pred);
// [ 1] bool any_of (InputIter first, InputIter last, PREDICATE pred);
// [ 1] bool none_of(InputIter first, InputIter last, PREDICATE pred);
// [ 2] BREATHING TEST
// [ 3] bsl::clamp();
// [ 4] bsl::lexicographical_compare_three_way()
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                       GLOBAL TEST ALIASES
// ----------------------------------------------------------------------------

using namespace BloombergLP;

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                              TEST FUNCTIONS
// ----------------------------------------------------------------------------

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

struct IsOdd {
    // A standard compliant C++03 unary predicate functor that returns 'true'
    // if an 'int' value is odd.

    // PUBLIC TYPES
    typedef char argument_type;
    typedef bool result_type;

    // ACCESSORS
    result_type operator()(argument_type value) const
        // Return 'true' if the specified 'value' is odd, and 'false'
        // otherwise.
    {
        return (value % 2) != 0;
    }
};

// A function (as opposed to a functor)
bool isEven(char value)
    // Return 'true' if the specified 'value' is even, and 'false' otherwise.
{
    return (value % 2) == 0;
}

template <class VALUE>
void runTestAllOf()
    // Test driver for 'all_of'.
{
    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_EVEN[] = {
        { "",    true  },
        { "0",   true  },
        { "1",   false },
        { "00",  true  },
        { "01",  false },
        { "10",  false },
        { "11",  false },
        { "000", true  },
        { "001", false },
        { "010", false },
        { "100", false },
        { "101", false },
        { "111", false }
    };
    const size_t NUM_DATA_EVEN = sizeof DATA_EVEN / sizeof *DATA_EVEN;

    for (size_t i = 0; i < NUM_DATA_EVEN; ++i) {
        const char *const SPEC = DATA_EVEN[i].d_spec_p;
        const bool        EXP  = DATA_EVEN[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::all_of(values.begin(), values.end(), isEven)));
    }

    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_ODD[] = {
        { "",    true  },
        { "0",   false },
        { "1",   true  },
        { "00",  false },
        { "01",  false },
        { "10",  false },
        { "11",  true  },
        { "000", false },
        { "001", false },
        { "010", false },
        { "100", false },
        { "101", false },
        { "111", true  }
    };
    const size_t NUM_DATA_ODD = sizeof DATA_ODD / sizeof *DATA_ODD;

    for (size_t i = 0; i < NUM_DATA_ODD; ++i) {
        const char *const SPEC = DATA_ODD[i].d_spec_p;
        const bool        EXP  = DATA_ODD[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::all_of(values.begin(), values.end(), IsOdd())));
    }
}

template <class VALUE>
void runTestAnyOf()
    // Test driver for 'any_of'.
{
    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_EVEN[] = {
        { "",    false },
        { "0",   true  },
        { "1",   false },
        { "00",  true  },
        { "01",  true  },
        { "10",  true  },
        { "11",  false },
        { "000", true  },
        { "001", true  },
        { "010", true  },
        { "100", true  },
        { "101", true  },
        { "111", false }
    };
    const size_t NUM_DATA_EVEN = sizeof DATA_EVEN / sizeof *DATA_EVEN;

    for (size_t i = 0; i < NUM_DATA_EVEN; ++i) {
        const char *const SPEC = DATA_EVEN[i].d_spec_p;
        const bool        EXP  = DATA_EVEN[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::any_of(values.begin(), values.end(), isEven)));
    }

    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_ODD[] = {
        { "",    false },
        { "0",   false },
        { "1",   true  },
        { "00",  false },
        { "01",  true  },
        { "10",  true  },
        { "11",  true  },
        { "000", false },
        { "001", true  },
        { "010", true  },
        { "100", true  },
        { "101", true  },
        { "111", true  }
    };
    const size_t NUM_DATA_ODD = sizeof DATA_ODD / sizeof *DATA_ODD;

    for (size_t i = 0; i < NUM_DATA_ODD; ++i) {
        const char *const SPEC = DATA_ODD[i].d_spec_p;
        const bool        EXP  = DATA_ODD[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::any_of(values.begin(), values.end(), IsOdd())));
    }
}

template <class VALUE>
void runTestNoneOf()
    // Test driver for 'none_of'.
{
    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_EVEN[] = {
        { "",    true  },
        { "0",   false },
        { "1",   true  },
        { "00",  false },
        { "01",  false },
        { "10",  false },
        { "11",  true  },
        { "000", false },
        { "001", false },
        { "010", false },
        { "100", false },
        { "101", false },
        { "111", true  }
    };
    const size_t NUM_DATA_EVEN = sizeof DATA_EVEN / sizeof *DATA_EVEN;

    for (size_t i = 0; i < NUM_DATA_EVEN; ++i) {
        const char *const SPEC = DATA_EVEN[i].d_spec_p;
        const bool        EXP  = DATA_EVEN[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::none_of(values.begin(), values.end(), isEven)));
    }

    const struct {
        const char *d_spec_p;
        bool        d_result;
    } DATA_ODD[] = {
        { "",    true  },
        { "0",   true  },
        { "1",   false },
        { "00",  true  },
        { "01",  false },
        { "10",  false },
        { "11",  false },
        { "000", true  },
        { "001", false },
        { "010", false },
        { "100", false },
        { "101", false },
        { "111", false }
    };
    const size_t NUM_DATA_ODD = sizeof DATA_ODD / sizeof *DATA_ODD;

    for (size_t i = 0; i < NUM_DATA_ODD; ++i) {
        const char *const SPEC = DATA_ODD[i].d_spec_p;
        const bool        EXP  = DATA_ODD[i].d_result;

        bsltf::TestValuesArray<VALUE> values(SPEC);
        ASSERT((EXP == bsl::none_of(values.begin(), values.end(), IsOdd())));
    }
}

#endif

template <class Type, size_t Size>
inline Type *array_end(Type (&arr)[Size])
    // Return past-the-end poiner for the specified 'arr' array.
{
    return arr + Size;
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING C++20 'lexicographical_compare_three_way'
        //
        // Concerns:
        //: 1 'bsl::lexicographical_compare_three_way' function is available
        //:   when 'BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON' macro
        //:   is defined.
        //
        // Plan:
        //: 1 Create two 'int' arrays: 'a' and 'b'.  'a' is lexicographically
        //:   less than 'b'.
        //:
        //: 2 Verify the 4-argument version of the function.
        //:
        //: 3 Verify that 5-argument version of the function returns the same
        //:   value if the last argument is 'bsl::compare_three_way()'.
        //
        // Testing:
        //   bsl::lexicographical_compare_three_way()
        // --------------------------------------------------------------------

        if (verbose) printf(
                      "\nTESTING C++20 'lexicographical_compare_three_way'"
                      "\n=================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        const int a[] = { 1, 2 },
                  b[] = { 1, 3 };

        ASSERT(bsl::lexicographical_compare_three_way(a, array_end(a),
                                                      b, array_end(b)) < 0);
        ASSERT(bsl::lexicographical_compare_three_way(b, array_end(b),
                                                      a, array_end(a)) > 0);

        ASSERT(bsl::lexicographical_compare_three_way(
                                                a, array_end(a),
                                                b, array_end(b),
                                                bsl::compare_three_way()) < 0);
        ASSERT(bsl::lexicographical_compare_three_way(
                                                b, array_end(b),
                                                a, array_end(a),
                                                bsl::compare_three_way()) > 0);
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING C++17 <BSL_ALGORITHM.H> ADDITIONS
        //
        // Concerns:
        //: 1 The call 'bsl::clamp' exists and return expected values for
        //:   simple cases.
        //
        // Plan:
        //: 1 Call each version of the algorithm with simple inputs and verify
        //:   that the result is correct.
        //
        // Testing:
        //   bsl::clamp();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 <BSL_ALGORITHM.H> ADDITIONS"
                            "\n=========================================\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        ASSERT(3 == bsl::clamp(3, 1, 10));
        ASSERT(3 == bsl::clamp(1, 3, 10));
        ASSERT(3 == bsl::clamp(3, 10, 1, std::greater<int>()));
        ASSERT(3 == bsl::clamp(1, 10, 3, std::greater<int>()));
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLSTL_ALGORITHMWORKAROUND_IMPLEMENTS_COPY_IF
        const int INPUT_ARRAY[] = { 0, 1, 2, 3, 4, 5, 6, 7};
        const int * const INPUT_BEGIN = INPUT_ARRAY;
        const size_t INPUT_SIZE = sizeof(INPUT_ARRAY) / sizeof(*INPUT_ARRAY);
        const int * const INPUT_END = INPUT_BEGIN + INPUT_SIZE;
        int OUTPUT_ARRAY[INPUT_SIZE] = {};

        const int * const OUTPUT_END =
        bsl::copy_if(INPUT_BEGIN, INPUT_END, OUTPUT_ARRAY, IsOdd());

        const size_t OUTPUT_EFFECTIVE_SIZE = OUTPUT_END - OUTPUT_ARRAY;

        ASSERTV(OUTPUT_EFFECTIVE_SIZE, 4 == OUTPUT_EFFECTIVE_SIZE);

        if (OUTPUT_EFFECTIVE_SIZE > 0) {
            ASSERTV(OUTPUT_ARRAY[0], 1 == OUTPUT_ARRAY[0]);
        }
        if (OUTPUT_EFFECTIVE_SIZE > 1) {
            ASSERTV(OUTPUT_ARRAY[1], 3 == OUTPUT_ARRAY[1]);
        }
        if (OUTPUT_EFFECTIVE_SIZE > 2) {
            ASSERTV(OUTPUT_ARRAY[2], 5 == OUTPUT_ARRAY[2]);
        }
        if (OUTPUT_EFFECTIVE_SIZE > 3) {
            ASSERTV(OUTPUT_ARRAY[3], 7 == OUTPUT_ARRAY[3]);
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //
        // Concerns:
        //: 1 The algorithms are sufficiently functional to enable
        //:   comprehensive testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each algorithm with an empty input range and verify that the
        //:   behavior is as expected.
        //: 2 Run each algorithm with a single-element input range and verify
        //:   that the behavior is as expected.
        //: 3 Run each algorithm with multiple-element input range and verify
        //:   that the behavior is as expected.
        //
        // Testing:
        //   bool all_of (InputIter first, InputIter last, PREDICATE pred);
        //   bool any_of (InputIter first, InputIter last, PREDICATE pred);
        //   bool none_of(InputIter first, InputIter last, PREDICATE pred);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTIONALITY TEST"
                            "\n==================\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        runTestAllOf<char>();
        runTestAnyOf<char>();
        runTestNoneOf<char>();
#endif

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
// Copyright 2020 Bloomberg Finance L.P.
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
