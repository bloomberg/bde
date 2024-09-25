// bslstl_monostate.t.cpp                                             -*-C++-*-
#include <bslstl_monostate.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bslstl_hash.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The type under test is `bsl::monostate`, a default constructible type whose
// interface and contract are dictated by the C++ standard.  If the
// `std::monostate` type is available, we need to check that `bsl::monostate`
// is a typedef to the standard's type.  If the `std::monostate` type isn't
// available, we need to check that `bsl::monostate` satisfies the interface
// and contract of `std::monostate`.
//
// ----------------------------------------------------------------------------
//
// FREE FUNCTIONS
// [ 3] operator==(monostate, monostate)
// [ 3] operator!=(monostate, monostate)
// [ 3] operator<(monostate, monostate)
// [ 3] operator>(monostate, monostate)
// [ 3] operator<=(monostate, monostate)
// [ 3] operator>=(monostate, monostate)
//
// [ 5] void hashAppend(HASHALG& hashAlg, const monostate&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: `monostate` is default constructible
// [ 4] CONCERN: Methods qualified `noexcept` in standard are so.

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                         GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    //  bool veryVerbose         = argc > 3;
    //  bool veryVeryVerbose     = argc > 4;
    //  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `hashAppend`
        //
        // Concerns:
        // 1. `hashAppend` for an object of type `monostate` is usable and
        //    satisfies the requirements of a hash function.
        //
        // Plan:
        // 1. Create two `monostate` objects and verify that hashing both of
        //    them yields the same value.  (C-1)
        //
        // Testing:
        //   void hashAppend(HASHALG& hashAlg, const monostate&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `hashAppend`"
                   "\n====================\n");

        {
            const bsl::monostate obj1;
            const bsl::monostate obj2;

            const size_t hashValue1 = bslh::Hash<>()(obj1);
            const size_t hashValue2 = bslh::Hash<>()(obj2);
            ASSERTV(hashValue1, hashValue2, hashValue1 == hashValue2);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `noexcept` SPECIFICATION
        //
        // Concerns:
        // 1. The `noexcept` specification has been applied to all relational
        //    operators as required by the standard.
        //
        // Plan:
        // 1. Apply the unary `noexcept` operator to expressions using each
        //    relational operator and verify that the calculated boolean values
        //    match the expected value.  (C-1)
        //
        // Testing:
        //   CONCERN: Methods qualified `noexcept` in standard are so.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'noexcept' SPECIFICATION"
                   "\n========================\n");

        const bsl::monostate lhs;
        const bsl::monostate rhs;

        // suppress unused variable warnings
        (void)lhs;
        (void)rhs;

        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs == rhs));
        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs != rhs));
        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs < rhs));
        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs > rhs));
        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs <= rhs));
        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(lhs >= rhs));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RELATIONAL OPERATORS
        //
        // Concerns:
        // 1. Any two `monostate` objects compare equal
        //
        // Plan:
        // 1. create two `monostate` objects, and check that all comparison
        //    operators behave as expected.  (C-1)
        //
        // Testing:
        //   operator==(monostate, monostate)
        //   operator!=(monostate, monostate)
        //   operator<(monostate, monostate)
        //   operator>(monostate, monostate)
        //   operator<=(monostate, monostate)
        //   operator>=(monostate, monostate)
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nRELATIONAL OPERATORS"
                   "\n====================\n");

        const bsl::monostate lhs;
        const bsl::monostate rhs;

        ASSERT((lhs == rhs));
        ASSERT(!(lhs != rhs));
        ASSERT(!(lhs < rhs));
        ASSERT(!(lhs > rhs));
        ASSERT((lhs <= rhs));
        ASSERT((lhs >= rhs));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTION
        //
        // Concerns:
        // 1. `monostate` object is default constructible as required by the
        //     standard.
        // Plan:
        // 1. Default construct an object of `monostate` type.  (C-1)
        //
        // Testing:
        //   CONCERN: `monostate` is default constructible
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nDEFAULT CONSTRUCTION"
                   "\n====================\n");

        // N4861: 20.7.8   Class monostate [variant.monostate]
        //
        // The class monostate can serve as a first alternative type for a
        // variant to make the variant type default constructible.

        const bsl::monostate obj;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        const bsl::monostate obj2{};
#else
        const bsl::monostate obj2;
#endif // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

        // suppress unused variable warnings
        (void)obj;
        (void)obj2;

        BSLMF_ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::monostate()));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Perform an ad-hoc test of the primary modifiers and accessors.
        //    (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");
        bsl::monostate obj;
        bsl::monostate obj2 = obj;

        // suppress unused variable warning
        (void)obj2;
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
// Copyright 2023 Bloomberg Finance L.P.
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
