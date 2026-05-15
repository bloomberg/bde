// bslmf_containercompatiblerange.t.cpp                               -*-C++-*-
#include <bslmf_containercompatiblerange.h>

#include <bsla_maybeunused.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#endif

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] CONCERN: ALL RELEVANT PROPERTIES ARE TESTED

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

BSLA_MAYBE_UNUSED void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

namespace usageExample {
namespace bsl {
    // Replicate code from `bslstl_ranges.h` here to avoid a cycle with that
    // higher level package.

struct from_range_t {

    // CREATORS
    explicit from_range_t() = default;
};

static from_range_t from_range;

}  // close namespace bsl

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Declaring a Range Compatible Interface
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a container class that we want to be able to accept content
// from a range.  We can use `bslmf::ContainerCompatibleRange` concept to
// constrain the relevant templates, removing them from the overload set unless
// the (template argument) `t_RANGE` meets the required constraints.
// ```
                        // =================
                        // class MyContainer
                        // =================

    template <class t_TYPE>
    class MyContainer {

      public:
        // CREATORS

        // ...

        template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
        MyContainer(bsl::from_range_t, t_RANGE&& range);

        // ...

        // MANIPULATORS

        // ...
        template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
        MyContainer& append_range(t_RANGE&& range);

        // ...

    };
// ```
// Notice that the (singular) tag value `bsl::from_range` is used to clearly
// direct overload resolution to the range constructor when that is the intent.
// For other methods, we have the option to incorporate "range" into the method
// name to avoid ambiguous overloads -- i.e., `append_range` is clearly
// different from assorted overloads of `append` (not shown).
//
// Finally, notice that the constraint on `t_RANGE` and `t_TYPE` must be
// repeated in the method definitions:
// ```
                        // -----------------
                        // class MyContainer
                        // -----------------

    // CREATORS

    // ...

    template <class t_TYPE>
    template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
    MyContainer<t_TYPE>::MyContainer(bsl::from_range_t, t_RANGE&& range)
    {
        (void) range;  // elide
        // ...
    }

    // MANIPULATORS

    // ...

    template <class t_TYPE>
    template <bslmf::ContainerCompatibleRange<t_TYPE> t_RANGE>
    MyContainer<t_TYPE>& MyContainer<t_TYPE>::append_range(t_RANGE&& range)
    {
        (void) range;  // elide
        // ...
        return *this;
    }

    int main()
    {
        int              intArray[] = {  7,  13 };
        MyContainer<int> mc(bsl::from_range, intArray);
                                      // OK, `intArray` is a range.

        std::vector<int> iVector    = { 42, 666 };
        mc.append_range(iVector);     // OK, `iVector` is also a range.

        std::vector<double> dVector = { 1.0, 32.2, 211.95 };
        mc.append_range(dVector);     // OK, `dVector` is also a range *and*
                                      // `double` is convertible to `int`.

#if 0   // elide
        std::vector<std::string> sVector = { "Tom", "Dick", "Harry" };
        mc.append_range(sVector);     // Error, `sVector` is a range but
                                      // `bsl::string` is *not* implicitly
                                      // convertible to `int`.

        int intValue = -1;
        mc.append_range(intValue);    // Error, `intValue` has expected type
                                      // *but* is not a range.
#endif  // elide

        return 0;
    }
// ```
}  // close namespace usageExample
#endif

// ============================================================================
//                              HELPER CLASSES
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

struct TestUtil {
    enum { e_isOther
         , e_isInputRange
         , e_isRangeConvertible
         , e_isBoth
         };

    static int function(...) {
        return e_isOther;
    }

    template <class TYPE, class RANGE> requires std::ranges::input_range<RANGE>
    static int function(TYPE value, RANGE range) {
        (void) value;
        (void) range;
        return e_isInputRange;
    }

    template <class TYPE, class RANGE>
                                      requires std::convertible_to<RANGE, TYPE>
    static int function(TYPE value, RANGE range) {
        (void) value;
        (void) range;
        return e_isRangeConvertible;
    }

    template <class TYPE, class RANGE>
             requires BloombergLP::bslmf::ContainerCompatibleRange<RANGE, TYPE>
    static int function(TYPE value, RANGE range) {
        (void) value;
        (void) range;
        return e_isBoth;
    }
};
#endif

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate the usage example from header into test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

        usageExample::MyContainer<int> myIntContainer(
                                            usageExample::bsl::from_range_t{},
                                            std::string_view("Hello, world!"));
#else
        if (veryVerbose)
                  printf("SKIP: the test requires both concepts and ranges\n");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING COVERAGE
        //
        // Concerns:
        // 1. The constraint `bslmf::ContainerCompatbileRange` tests *both*
        //    standard concepts for the `TYPE` and `RANGE` template arguments.
        //
        // Plan:
        // 1. Write ad hoc examples using `bslmf::ContainerCompatibleRange`
        //    in concert with `TestUtil` that provides overloads of `function`
        //    that categorize different levels of compliance with the
        //    constraint requirements.
        //
        // Testing:
        //   CONCERN: ALL RELEVANT PROPERTIES ARE TESTED
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COVERAGE"
                            "\n================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS) \
 && defined(BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES)

        using TU = TestUtil;

        char             c = 'c';
        std::string_view sv;
        ASSERT(TU::e_isBoth == TU::function(c, sv));

        int              i = 42;
        std::vector<int> vi;
        ASSERT(TU::e_isBoth == TU::function(i, vi));

        double           d = 1.0;  // 'double` is compatible to `int`.
        ASSERTV(                TU::function(d, vi),
                TU::e_isBoth == TU::function(d, vi));

        std::string      s;  // 'std::string` not compatible to `int`.
        ASSERTV(                      TU::function(&s, vi),
                TU::e_isInputRange == TU::function(&s, vi));

        ASSERTV(                 TU::function(&s, d),  // 'double' not a range
                TU::e_isOther == TU::function(&s, d));
#else
        if (veryVerbose)
                  printf("SKIP: the test requires both concepts and ranges\n");
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
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
