// bslstl_format_string.t.cpp                                         -*-C++-*-
#include <bslfmt_format_string.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'basic_format_string` is a standard-compliant implementation of
// 'std::basic_format_string'.
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::basic_format_string'
//
// CREATORS
// [ 3] basic_format_string();
// [ 3] ~basic_format_string();
// [ 7] basic_format_string(const basic_format_string &);
// [12] TBD
//
// MANIPULATORS
// [ 6] operator==(const basic_format_string &);
// [ 9] operator=(const basic_format_string &);
//
// ACCESSORS
// [ 4] TBD
//
// FREE FUNCTIONS
// [ 8] swap(basic_format_string &, basic_format_string &);
// [11] TBD
// [11] TBD
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS: Not Applicable
// [ 5] TESTING OUTPUT:               Not Applicable
// [10] STREAMING FUNCTIONALITY:      Not Applicable
// [13] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)




// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------





// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------




// ============================================================================
//                    GLOBAL TYPES AND FUNCTIONS FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&              \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
  template <class t_ARG>
  void myFormatLikeFunction(bslfmt::format_string<t_ARG> fmtstr,
                            const t_ARG&)
  {
    ASSERT(fmtstr.get() == "{:}");
  }
#else
  template <class t_ARG>
  void myFormatLikeFunction(bslfmt::format_string        fmtstr,
                            const t_ARG&)
  {
    ASSERT(fmtstr.get() == "{:}");
  }
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------


int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Construct an instance and verify it holds no value.
        //:
        //: 2 Construct an instance with a single value and verify contents.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Own format function
/// - - - - - - - - - - - - - -
//
// This usage example reflects how `bslfmt::format` typically uses this type.
//
// Suppose we have a function that takes a format string and requires that the
// string be constant evaluated under C++20:
//
//..
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&
//     defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
//   template <class t_ARG>
//   void myFormatLikeFunction(bslfmt::format_string<t_ARG> fmtstr,
//                             const t_ARG&)
//   {
//     assert(fmtstr.get() == "{:}");
//   }
// #else
//   template <class t_ARG>
//   void myFormatLikeFunction(bslfmt::format_string        fmtstr,
//                             const t_ARG&)
//   {
//     assert(fmtstr.get() == "{:}");
//   }
// #endif
//..
//
// We can then invoke our function:
//
//..
        int value = 5;
        myFormatLikeFunction("{:}", value);
//..
//

      } break;
      case 12: {
        // --------------------------------------------
        // TESTING TBD - WORK IN PROGRESS
        //
        // Testing:
        // --------------------------------------------
        if (verbose)
            printf("\nCONSTRUCTION FROM ARG STORE"
                   "\n= =========================\n");

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS - WORK IN PROGRESS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nFREE FUNCTIONS"
                   "\n==============\n");

      } break;
      case 10: {
        // -----------------------------------------------
        // TESTING STREAMING FUNCTIONALITY: Not Applicable
        //
        // Testing:
        //   STREAMING FUNCTIONALITY: Not Applicable
        // -----------------------------------------------
        if (verbose)
            printf("\nSTREAMING FUNCTIONALITY: Not Applicable"
                   "\n=======================================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   operator=(const basic_format_args &);
        //   operator=(basic_format_args &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ASSIGNMENT OPERATOR"
                   "\n===========================\n");

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   swap(formatter &, formatter &)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY/MOVE CONSTRUCTOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   formatter(const formatter &);
        //   formatter(formatter &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

      } break;
      case 6: {
        // --------------------------------------------
        // TESTING EQUALITY OPERATOR: Not Applicable
        //
        // Testing:
        //   EQUALITY OPERATOR: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nEQUALITY OPERATOR: Not Applicable"
                   "\n= ===============================\n");

      } break;
      case 5: {
        // --------------------------------------------
        // TESTING OUTPUT: Not Applicable
        //
        // Testing:
        //   OUTPUT: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nOUTPUT: Not Applicable"
                   "\n======================\n");
      } break;
      case 4: {
        // --------------------------------------------
        // TESTING BASIC ACCESSORS
        // 
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   TBD
        // --------------------------------------------
        if (verbose)
            printf("\nTESTING BASIC ACCESSORS"
                   "\n=======================\n");
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //
        // Concerns:
        //: 1 We can default construct a `bslfmt::basic_format_arg` type.
        //:
        //: 2 We can construct a `bslfmt::basic_format_arg` type containing a
        //:   value.
        //
        // Plan:
        //: 1 Construct a default `bslfmt::basic_format_arg` type. (C-1)
        //:
        //: 2 Construct a non-default `bslfmt::basic_format_arg` type. This
        //:   will need to be performed indirectly as the constructors are
        //:   private (C-2)
        //
        // Testing:
        //   basic_format_args();
        //   ~basic_format_args()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");
      } break;
      case 2: {
        // --------------------------------------------
        // TESTING PRIMARY MANIPULATORS: Not Applicable
        //
        // Testing:
        //   PRIMARY MANIPULATORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nPRIMARY MANIPULATORS: Not Applicable"
                   "\n====================================\n");
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
        bslfmt::format_string<int> fmtstr("{}");
        ASSERT(fmtstr.get() == "{}");

        bslfmt::wformat_string<int> wfmtstr(L"{}");
        ASSERT(wfmtstr.get() == L"{}");
#else
        bslfmt::format_string fmtstr("{}");
        ASSERT(fmtstr.get() == "{}");

        bslfmt::wformat_string wfmtstr(L"{}");
        ASSERT(wfmtstr.get() == L"{}");
#endif


      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
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
