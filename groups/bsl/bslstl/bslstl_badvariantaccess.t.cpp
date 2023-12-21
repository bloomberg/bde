// bslstl_badvariantaccess.t.cpp                                      -*-C++-*-
#include <bslstl_badvariantaccess.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The type under test is 'bsl::bad_variant_access', an exception type whose
// interface and contract are dictated by the C++ standard.  If the
// 'std::bad_variant_access' exception is available, we need to check that
// 'bsl::bad_variant_access' is a typedef to the standard's exception type.  If
// the 'std::bad_variant_access' exception isn't available, we need to check
// that 'bsl::bad_variant_access' satisfies the interface and contract of
// 'std::bad_variant_access'.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] bad_variant_access();
//
// ACCESSORS
// [ 3] const char *what() const;
//
// TYPES
// [ 4] bsl::bad_variant_access
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: Methods qualified 'noexcept' in standard are so.

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

    switch (test) {
      case 0:
#ifdef BDE_BUILD_TARGET_EXC
      case 4: {
        // --------------------------------------------------------------------
        // 'bsl::bad_variant_access' TYPEDEF
        //
        // Concerns:
        //: 1 The 'bsl::bad_variant_access' is a typedef for
        //:   'std::bad_variant_access' if 'std::bad_variant_access' is
        //:    available.
        //
        // Plan:
        //: 1 If we're using CPP17 library, check that
        //: 'bsl::bad_variant_access' is the same type as
        //: 'std::bad_variant_access' using 'bsl::is_same'.  (C-1)
        //
        // Testing:
        //   bsl::bad_variant_access
        // --------------------------------------------------------------------
        if (verbose)
            printf("\n'bsl::bad_variant_access' TYPEDEF"
                   "\n=================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        BSLMF_ASSERT((bsl::is_same<bsl::bad_variant_access,
                                   std::bad_variant_access>::value));

#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTION AND 'what' METHOD
        //
        // Concerns:
        //: 1 A 'bad_variant_access' object can be default constructed.
        //: 2 Invoking a 'what' method on a 'bad_variant_access' object
        //:   returns a "bad_variant_access" string.
        //: 3 'what' method can be invoked on a const 'bad_variant_access'
        //:   object.
        //
        // Plan:
        //: 1 Default construct a 'const' 'bad_variant_access' object, then
        //:   invoke the 'what' method on it and check that the returned string
        //:   is "bad_variant_access".  (C-1..3)
        //
        // Testing:
        //   bad_variant_access();
        //   const char *what() const;
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nDEFAULT CONSTRUCTION AND 'what' METHOD"
                   "\n======================================\n");
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // string returned by 'what()' method is implementation specific so we
        // can only check our own implementation
        const bsl::bad_variant_access b;
        ASSERT(0 == strcmp("bad_variant_access", b.what()));
#endif  // #ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        //
        // Concerns:
        //: 1 The default constructor of 'bad_variant_access' is 'noexcept' as
        //:   required by the standard.
        //: 2 'bad_variant_access::what' is 'noexcept' as required by the
        //:   standard.
        //
        // Plan:
        //: 1 Apply the unary 'noexcept' operator to an expression that default
        //:   constructs a 'bad_variant_access' object and confirm that the
        //:   calculated boolean value matches the expected value.  (C-1)
        //: 2 Apply the unary 'noexcept' operator to a call to
        //:   'bad_variant_access::what' and confirm that the calculated
        //:   boolean value matches the expected value.  (C-2)
        //
        // Testing:
        //   CONCERN: Methods qualified 'noexcept' in standard are so.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'noexcept' SPECIFICATION"
                   "\n========================\n");

            // N4835: 20.6.5 Class 'bad_variant_access' [variant.bad.access]
            //..
            //     namespace std {
            //       class bad_variant_access: public std::exception {
            //       public:
            //         const char* what() const noexcept override;
            //       };
            //     } // namespace std
            //..
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // While noexcept specification is a standard requirement, at least one
        // library implementation does not apply the noexcept specification
        // correctly.
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::bad_variant_access()));

        bsl::bad_variant_access b;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(b.what()));
#endif
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

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");
        bsl::bad_variant_access  b;
        const std::exception    *ptr = &b;

        ASSERT(0 != b.what());
        ASSERT(0 != ptr->what());
      } break;
#endif  //BDE_BUILD_TARGET_EXC
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
