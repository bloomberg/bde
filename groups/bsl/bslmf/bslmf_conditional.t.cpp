// bslmf_conditional.t.cpp                                            -*-C++-*-
#include <bslmf_conditional.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

#if defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4181)  // 'const' applied to reference type
#endif

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions, 'bsl::conditional' and
// 'bsl::conditional_t', that conditionally select to one of its two template
// parameter types based on a 'bool' (template parameter) value.  Thus, we need
// to ensure that the values returned by this meta-function are correct for
// each possible set of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bsl::conditional::type
// [ 1] bsl::conditional_t
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef void (Class::*MethodPtrTestType)();
typedef int   Class::* PMD;

typedef int INT;

typedef void        F ();
typedef void (  &  RF)();
typedef void (*    PF)();
typedef void (*&  RPF)();

typedef void      Fi  (int);
typedef void ( & RFi )(int);
typedef void      FRi (int&);
typedef void ( & RFRi)(int&);

typedef char      A [5];
typedef char ( & RA)[5];

#define ASSERT_SAME_CV2(TYPE1, TYPE2)                                         \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
                                          TYPE1                               \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                                TYPE1,                        \
                                          const TYPE2>::type,                 \
                                                TYPE1                         \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                                   TYPE1,                     \
                                          volatile TYPE2>::type,              \
                                                   TYPE1                      \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                                         TYPE1,               \
                                          const volatile TYPE2>::type,        \
                                                         TYPE1                \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
                                          TYPE2                               \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                                TYPE1,                        \
                                          const TYPE2>::type,                 \
                                          const TYPE2>::value));              \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                                   TYPE1,                     \
                                          volatile TYPE2>::type,              \
                                          volatile TYPE2                      \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          const volatile TYPE2>::type,        \
                                          const volatile TYPE2                \
            >::value));
    // Test that the result type of 'bsl::conditional' meta-function is defined
    // as the specified 'TYPE1' if the first template parameter of the
    // meta-function is 'true', and one of cv-qualified combination on the
    // specified 'TYPE2' otherwise.

#define ASSERT_SAME_CV(TYPE1, TYPE2)                                          \
    ASSERT_SAME_CV2(               TYPE1, TYPE2)                              \
    ASSERT_SAME_CV2(const          TYPE1, TYPE2)                              \
    ASSERT_SAME_CV2(      volatile TYPE1, TYPE2)                              \
    ASSERT_SAME_CV2(const volatile TYPE1, TYPE2)
    // Test all cv-qualified combination on the specified 'TYPE1' and 'TYPE2'.

#define ASSERT_SAME_FN_TYPE(TYPE1, TYPE2)                                     \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
                                          TYPE1                               \
            >::value));                                                       \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
                                          TYPE2                               \
            >::value));
    // Test function types separately since function types cannot be
    // cv-qaulified.

#define ASSERT_CONDITIONAL_T(TYPE1, TYPE2)                                    \
    ASSERT((bsl::is_same<bsl::conditional  <true,                             \
                                            TYPE1,                            \
                                            TYPE2>::type,                     \
                         bsl::conditional_t<true,                             \
                                            TYPE1,                            \
                                            TYPE2> >::value));                \
    ASSERT((bsl::is_same<bsl::conditional  <false,                            \
                                            TYPE1,                            \
                                            TYPE2>::type,                     \
                         bsl::conditional_t<false,                            \
                                            TYPE1,                            \
                                            TYPE2> >::value));
    // Test that the result types of the 'bsl::conditional<COND, TYPE1, TYPE2>'
    // and 'bsl::conditional_t<COND, TYPE1, TYPE2' meta-functions are the same.

//=============================================================================
//                              MAIN PROGRAM
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

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditionally Select From Two Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types based on a 'bool' value.
//
// Now, we use 'bsl::conditional' to select between two types, 'int' and
// 'char', with a 'bool' value.  When the 'bool' is 'true', we select 'int';
// otherwise, we select 'char'.  We verify that our code behaves correctly by
// asserting the result of 'bsl::conditional' with the expected type using
// 'bsl::is_same':
//..
    ASSERT(true ==
        (bsl::is_same<bsl::conditional<true,  int, char>::type, int >::value));
    ASSERT(true ==
        (bsl::is_same<bsl::conditional<false, int, char>::type, char>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// select between two types using 'bsl::conditional_t' and verify that our code
// behaves correctly by asserting the result of 'bsl::conditional_t' with the
// expected type using 'bsl::is_same':
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    ASSERT(true ==
            (bsl::is_same<bsl::conditional_t<true,  int, char>, int >::value));
    ASSERT(true ==
            (bsl::is_same<bsl::conditional_t<false, int, char>, char>::value));
#endif
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::conditional::type'
        //   Ensure that each 'typedef' 'type' of 'bsl::conditional'
        //   instantiations has the correct return value.
        //
        // Concerns:
        //: 1 'bsl::conditional' selects the first of its two (template
        //:   parameter) types when the (template parameter) value 'COND' is
        //:   'true'.
        //:
        //: 2 'bsl::conditional' selects the second of its two (template
        //:   parameter) types when the (template parameter) value 'COND' is
        //:   'false'.
        //:
        //: 3 'bsl::conditional' represents the return type of
        //:   'bsl::conditional' meta-function for a variety of template
        //:   parameter types.
        //
        // Plan:
        //   Instantiate 'bsl::conditional' with various types and verify that
        //   the 'type' member is initialized properly.  (C-1..2)
        //
        // Testing:
        //   bsl::conditional::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::conditional::type\n"
                            "======================\n");

        ASSERT_SAME_CV(   int ,  char);
        ASSERT_SAME_CV(  void*,  Enum);
        ASSERT_SAME_CV(  Enum&, Class);
        ASSERT_SAME_CV( Class*, Union);

        ASSERT_SAME_FN_TYPE(     F ,    RF);
        ASSERT_SAME_FN_TYPE(    RF ,    PF);
        ASSERT_SAME_FN_TYPE(    PF ,   RPF);
        ASSERT_SAME_FN_TYPE(   RPF ,    Fi);
        ASSERT_SAME_FN_TYPE(    Fi ,   RFi);
        ASSERT_SAME_FN_TYPE(   RFi ,   FRi);
        ASSERT_SAME_FN_TYPE(   FRi ,  RFRi);
        ASSERT_SAME_FN_TYPE(  RFRi ,     A);

        ASSERT_SAME_CV(     A ,    RA);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        if (verbose) printf("bsl::conditional_t\n"
                            "==================\n");

        // C-3
        ASSERT_CONDITIONAL_T(int ,   char);
        ASSERT_CONDITIONAL_T(void*,  Enum);
        ASSERT_CONDITIONAL_T(Enum&,  Class);
        ASSERT_CONDITIONAL_T(Class*, Union);
        ASSERT_CONDITIONAL_T(PMD,    MethodPtrTestType);
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
