// bslstl_badfunctioncall.t.cpp                                       -*-C++-*-
#include <bslstl_badfunctioncall.h>
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
// The type under test is 'bsl::bad_function_call', an exception type whose
// interface and contract is dictated by the C++ standard.  If
// 'std::bad_function_call' exception is available, we need to check that
// 'bsl::bad_function_call' is a typedef to the standard's exception type.  If
// 'std::bad_function_call' exception isn't available, we need to check that
// 'bsl::bad_function_call' satisfies the interface and contract of
// 'std::bad_function_call'.
//
// ----------------------------------------------------------------------------
// creators:
// [ 3] bad_function_call();
//
// accessors:
// [ 3] const char *what() const;
//
// typedef:
// [ 4] typedef bsl::bad_function_call
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
      case 5: {
        // --------------------------------------------------------------------
        // 'bsl::bad_function_call' IS THROWABLE
        //
        // Concerns:
        //: 1 'bad_function_call' can be used in a throw expression.
        //
        // Plan:
        //: 1 For concern 1, throw an object of bsl::'bad_function_call' and
        //:   check that it was caught by the correct handler.
        //
        // Testing:
        //   CONCERN: 'bsl::bad_function_call' can be thrown
        // --------------------------------------------------------------------
        if (verbose) printf("\n'bsl::bad_function_call' IS THROWABLE"
                            "\n======================================\n");

        bool correctCatch = false;
        try
        {
            throw bsl::bad_function_call();
        }
        catch(const bsl::bad_function_call&)
        {
            correctCatch = true;
        }
        catch (...)
        {}

        ASSERT(true == correctCatch);

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // 'bsl::bad_function_call' TYPEDEF
        //
        // Concerns:
        //: 1 The 'bsl::bad_function_call' is a typedef for
        //:   'std::bad_function_call' if 'std::bad_function_call' is
        //:    available.
        //
        // Plan:
        //: 1 For concern 1, if we're using CPP11 library, check that
        //:   'bsl::bad_function_call' is the same type as
        //:   'std::bad_function_call' using 'bsl::is_same'.
        //
        // Testing:
        //   typedef bsl::bad_function_call
        // --------------------------------------------------------------------
        if (verbose)
            printf("\n'bsl::bad_function_call' TYPEDEF"
                   "\n================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(
        (bsl::is_same<bsl::bad_function_call, std::bad_function_call>::value));

#endif  //BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTION AND 'what' METHOD
        //
        // Concerns:
        //: 1 A 'bad_function_call' object can be default constructed.
        //:
        //: 2 Invoking a 'what' method on a 'bad_function_call' object
        //:   returns a "bad_function_call" string.
        //:
        //: 3 'what' method can be invoked on a const 'bad_function_call'
        //:   object.
        //
        // Plan:
        //: 1 For concern 1, default construct a 'bad_function_call' object.
        //:
        //: 2 For concern 2, invoke the 'what' method on a
        //:   'bad_function_call' object and check that the returned string
        //:   is "bad_function_call".
        //:
        //: 3 For concern 3, in step 2, use a const qualified
        //:   'bad_function_call' object.
        //
        // Testing:
        //   bad_function_call();
        //   const char *what() const;
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nDEFAULT CONSTRUCTION AND 'what' METHOD"
                   "\n======================================\n");
        const bsl::bad_function_call b;
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // string returned by 'what()' method is implementation specific so we
        // can only check our own implementation
        ASSERT(0 == strcmp("bad_function_call", b.what()));
#else
        ASSERT(0 != b.what() && 0 != strlen(b.what()));
#endif  // #ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        //
        // Concerns:
        //: 1 The 'noexcept' specification has been applied to
        //:   'bad_function_call' constructor as required by the standard.
        //:
        //: 2 The 'noexcept' specification has been applied to
        //:   'bad_function_call' 'what' method as required by the standard.
        // Plan:
        //: 1 Apply the unary 'noexcept' operator to a 'bad_function_call'
        //:   constructor and, for concern 1, confirm that calculated boolean
        //:   value matches the expected value.
        //:
        //: 2 Apply the unary 'noexcept' operator to 'bad_function_call'
        //:   'what' method and, for concern 2, confirm that calculated boolean
        //:   value matches the expected value.
        //
        // Testing:
        //   CONCERN: Methods qualified 'noexcept' in standard are so.
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'noexcept' SPECIFICATION"
                   "\n========================\n");

            // N4835: 20.14.16.1 Class 'bad_function_call' [func.wrap.badcall]
            //..
            //     namespace std {
            //       class bad_function_call: public std::exception {
            //       public:
            //         const char* what() const noexcept override;
            //       };
            //     } // namespace std
            //..
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // While noexcept specification is a standard requirement, at least one
        // library implementation does not apply the noexcept specification
        // correctly.
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE ==
               BSLS_KEYWORD_NOEXCEPT_OPERATOR(bsl::bad_function_call()));

        bsl::bad_function_call b;
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
        bsl::bad_function_call  b;
        const std::exception   *ptr = &b;

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
