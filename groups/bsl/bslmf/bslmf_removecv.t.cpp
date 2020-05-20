// bslmf_removecv.t.cpp                                               -*-C++-*-
#include <bslmf_removecv.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions, 'bsl::remove_cv' and
// 'bsl::remove_cv_t', that remove any top-level cv-qualifiers from a template
// parameter type.  Thus, we need to ensure that the values returned by the
// meta-function are correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_cv::type
// [ 1] bsl::remove_cv_t
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
//              PLATFORM DETECTION MACROS TO SUPPORT TESTING
//-----------------------------------------------------------------------------

//# define BSLMF_REMOVECV_SHOW_COMPILER_ERRORS 1
#if !defined(BSLMF_REMOVECV_SHOW_COMPILER_ERRORS)

# if defined(BSLS_PLATFORM_CMP_IBM)                                           \
  ||(defined(BSLS_PLATFORM_CMP_GNU)  && BSLS_PLATFORM_CMP_VERSION <= 40400)   \
  ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1900)
// The xlC compiler matches function types with trailing cv-qualifiers as being
// cv-qualified themselves.  However, in such cases the cv-qualifier applies to
// the (hidden) 'this' pointer, as these function types exist only to be the
// result-type of a pointer-to-member type.  By definition no function type can
// ever be cv-qualified.  The Microsoft compiler cannot parse such types at
// all.
//
// Note that we could obtain the correct answer by testing 'is_function', and
// simply returning the original type in such cases.  However, that simply
// exposes that our current implementation of 'is_function' does not detect
// such types either.
#   define BSLMF_REMOVECV_COMPILER_MISMATCHES_ABOMINABLE_FUNCTION_TYPES 1
# endif

# if defined(BSLS_PLATFORM_CMP_IBM)
#   define BSLMF_REMOVECV_DO_NOT_TEST_CV_REF_TO_FUNCTION_TYPES 1
// The IBM compiler cannot handle references to cv-qualified types, where such
// referenced types are typedefs to regular (non-abominable) functions.  A
// conforming compiler should silently drop the cv-qualifier, although some may
// be noisy and issue a warning.  Last tested with xlC 12.2
# endif

#endif // BSLMF_REMOVEVOLATILE_SHOW_COMPILER_ERRORS

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consequently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#elif defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::remove_cv'.
};

typedef int TestType::* Pm;
typedef int (TestType::*Pmf)();
typedef int (TestType::*Pmq)() const;

}  // close unnamed namespace

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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers of a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
        typedef int                MyType;
        typedef const volatile int MyCvType;
//..
// Now, we remove the cv-qualifiers from 'MyCvType' using 'bsl::remove_cv' and
// verify that the resulting type is the same as 'MyType':
//..
        ASSERT(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
                                                              MyType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a 'const'-qualified and 'volatile'-qualifier from 'MyCvType' using
//'bsl::remove_cv_t' and verify that the resulting type is the same as
// 'MyType':
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        ASSERT(true ==
                    (bsl::is_same<bsl::remove_cv_t<MyCvType>, MyType>::value));
#endif
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_cv::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_cv'
        //   instantiations has the same type as the template parameter type
        //   except that it has any top-level cv-qualifiers removed.
        //
        // Concerns:
        //: 1 'bsl::remove_cv' leaves types that are not 'const'-qualified nor
        //:   'volatile'-qualified as-is.
        //:
        //: 2 'bsl::remove_cv' removes any top-level cv-qualifiers.
        //:
        //: 3 'bsl::remove_cv' removes top-level cv-qualifiers from a
        //:   pointer-to-member object type, and not from the qualifier in the
        //:   pointed-to member.
        //:
        //: 5 'bsl::remove_cv_t' represents the return type of 'bsl::remove_cv'
        //:   meta-function for a variety of template parameter types.
        //
        // Plan:
        //  1 Verify that 'bsl::remove_cv::type' has the correct type for each
        //    concern. (C1-4)
        //
        //  2 Verify that 'bsl::remove_cv_t' has the same type as the return
        //    type of 'bsl::remove_cv' for a variety of template parameter
        //    types. (C-5)
        //
        // Testing:
        //   bsl::remove_cv::type
        //   bsl::remove_cv_t
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_cv::type'\n"
                            "\n======================\n");

        // C-1
        ASSERT((is_same<remove_cv<int>::type, int>::value));
        ASSERT((is_same<remove_cv<int *>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType>::type, TestType>::value));
        ASSERT((is_same<remove_cv<int const volatile *>::type,
                                                int const volatile *>::value));

        ASSERT((is_same<remove_cv<int const *>::type, int const *>::value));
        ASSERT((is_same<remove_cv<int const &>::type, int const &>::value));
        ASSERT((is_same<remove_cv<int const()>::type, int const()>::value));
        ASSERT((is_same<remove_cv<int[5]>::type, int[5]>::value));
        ASSERT((is_same<remove_cv<int[5][2]>::type, int[5][2]>::value));
        ASSERT((is_same<remove_cv<int[5][2][3]>::type,
                                  int[5][2][3]>::value));
        ASSERT((is_same<remove_cv<int[]>::type, int[]>::value));
        ASSERT((is_same<remove_cv<int[][2]>::type, int[][2]>::value));
        ASSERT((is_same<remove_cv<int[][2][3]>::type, int[][2][3]>::value));

        ASSERT((is_same<remove_cv<void>::type, void>::value));

        ASSERT((is_same<remove_cv<const int TestType::*>::type,
                                  const int TestType::*>::value));

        ASSERT((is_same<remove_cv<const int (TestType::*)() const>::type,
                                  const int (TestType::*)() const>::value));

#if !defined(BSLMF_REMOVECV_COMPILER_MISMATCHES_ABOMINABLE_FUNCTION_TYPES)
        ASSERT((is_same<remove_cv<int const() const>::type,
                                  int const() const>::value));

        ASSERT((is_same<remove_cv<int const() const volatile>::type,
                                  int const() const volatile>::value));
#endif


        // C-2
        ASSERT((is_same<remove_cv<int const>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const>::type, TestType>::value));

        ASSERT((is_same<remove_cv<int volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType volatile>::type, TestType>::value));

        ASSERT((is_same<remove_cv<int const volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const volatile>::type,
                                                            TestType>::value));

        ASSERT((is_same<remove_cv<const int[5]>::type,
                                        int[5]>::value));
        ASSERT((is_same<remove_cv<const int[5][2]>::type,
                                        int[5][2]>::value));
        ASSERT((is_same<remove_cv<const int[5][2][3]>::type,
                                        int[5][2][3]>::value));

        ASSERT((is_same<remove_cv<const int[]>::type,
                                        int[]>::value));
        ASSERT((is_same<remove_cv<const int[][2]>::type,
                                        int[][2]>::value));
        ASSERT((is_same<remove_cv<const int[][2][3]>::type,
                                        int[][2][3]>::value));

        ASSERT((is_same<remove_cv<volatile int[5]>::type,
                                           int[5]>::value));
        ASSERT((is_same<remove_cv<volatile int[5][2]>::type,
                                           int[5][2]>::value));
        ASSERT((is_same<remove_cv<volatile int[5][2][3]>::type,
                                           int[5][2][3]>::value));

        ASSERT((is_same<remove_cv<volatile int[]>::type,
                                           int[]>::value));
        ASSERT((is_same<remove_cv<volatile int[][2]>::type,
                                           int[][2]>::value));
        ASSERT((is_same<remove_cv<volatile int[][2][3]>::type,
                                           int[][2][3]>::value));

        ASSERT((is_same<remove_cv<const volatile int[5]>::type,
                                                 int[5]>::value));
        ASSERT((is_same<remove_cv<const volatile int[5][2]>::type,
                                                 int[5][2]>::value));
        ASSERT((is_same<remove_cv<const volatile int[5][2][3]>::type,
                                                 int[5][2][3]>::value));

        ASSERT((is_same<remove_cv<const volatile int[]>::type,
                                                 int[]>::value));
        ASSERT((is_same<remove_cv<const volatile int[][2]>::type,
                                                 int[][2]>::value));
        ASSERT((is_same<remove_cv<const volatile int[][2][3]>::type,
                                                 int[][2][3]>::value));

        ASSERT((is_same<remove_cv<const          void>::type,
                                                 void>::value));
        ASSERT((is_same<remove_cv<      volatile void>::type,
                                                 void>::value));
        ASSERT((is_same<remove_cv<const volatile void>::type,
                                                 void>::value));

        // C -3
        ASSERT((is_same<remove_cv<               Pm>::type,
                                                 Pm>::value));
        ASSERT((is_same<remove_cv<const          Pm>::type,
                                                 Pm>::value));
        ASSERT((is_same<remove_cv<      volatile Pm>::type,
                                                 Pm>::value));
        ASSERT((is_same<remove_cv<const volatile Pm>::type,
                                                 Pm>::value));

        ASSERT((is_same<remove_cv<               Pmf>::type,
                                                 Pmf>::value));
        ASSERT((is_same<remove_cv<const          Pmf>::type,
                                                 Pmf>::value));
        ASSERT((is_same<remove_cv<      volatile Pmf>::type,
                                                 Pmf>::value));
        ASSERT((is_same<remove_cv<const volatile Pmf>::type,
                                                 Pmf>::value));

        ASSERT((is_same<remove_cv<               Pmq>::type,
                                                 Pmq>::value));
        ASSERT((is_same<remove_cv<const          Pmq>::type,
                                                 Pmq>::value));
        ASSERT((is_same<remove_cv<      volatile Pmq>::type,
                                                 Pmq>::value));
        ASSERT((is_same<remove_cv<const volatile Pmq>::type,
                                                 Pmq>::value));

        // C-4
        ASSERT((is_same<remove_cv<int const(&)()>::type,
                                  int const(&)()>::value));

#if !defined(BSLMF_REMOVECV_DO_NOT_TEST_CV_REF_TO_FUNCTION_TYPES)
        typedef int const FnType();

        ASSERT((is_same<remove_cv<const volatile FnType&>::type,
                                  const volatile FnType&>::value));
#endif

        ASSERT((is_same<remove_cv<const int(*               )()>::type,
                                  const int(*               )()>::value));
        ASSERT((is_same<remove_cv<const int(* const         )()>::type,
                                  const int(*               )()>::value));
        ASSERT((is_same<remove_cv<const int(*       volatile)()>::type,
                                  const int(*               )()>::value));
        ASSERT((is_same<remove_cv<const int(* const volatile)()>::type,
                                  const int(*               )()>::value));
        ASSERT((is_same<remove_cv<const int(* volatile &)()>::type,
                                  const int(* volatile &)()>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        if (verbose) printf("\n'bsl::remove_cv_t'\n"
                            "\n=================\n");

        ASSERT((is_same<remove_cv  <int     >::type,
                        remove_cv_t<int     >>::value));
        ASSERT((is_same<remove_cv  <int *   >::type,
                        remove_cv_t<int *   >>::value));
        ASSERT((is_same<remove_cv  <TestType>::type,
                        remove_cv_t<TestType>>::value));

        ASSERT((is_same<remove_cv  <int const volatile *>::type,
                        remove_cv_t<int const volatile *>>::value));
        ASSERT((is_same<remove_cv  <int const *         >::type,
                        remove_cv_t<int const *         >>::value));
        ASSERT((is_same<remove_cv  <int const &         >::type,
                        remove_cv_t<int const &         >>::value));
        ASSERT((is_same<remove_cv  <int const()         >::type,
                        remove_cv_t<int const()         >>::value));
        ASSERT((is_same<remove_cv  <int[5]              >::type,
                        remove_cv_t<int[5]              >>::value));
        ASSERT((is_same<remove_cv  <void                >::type,
                        remove_cv_t<void                >>::value));

        ASSERT((is_same<remove_cv  <const int  TestType::*         >::type,
                        remove_cv_t<const int  TestType::*         >>::value));
        ASSERT((is_same<remove_cv  <const int (TestType::*)() const>::type,
                        remove_cv_t<const int (TestType::*)() const>>::value));

        ASSERT((is_same<remove_cv  <int      const         >::type,
                        remove_cv_t<int      const         >>::value));
        ASSERT((is_same<remove_cv  <int *    const         >::type,
                        remove_cv_t<int *    const         >>::value));
        ASSERT((is_same<remove_cv  <TestType const         >::type,
                        remove_cv_t<TestType const         >>::value));
        ASSERT((is_same<remove_cv  <int            volatile>::type,
                        remove_cv_t<int            volatile>>::value));
        ASSERT((is_same<remove_cv  <int *          volatile>::type,
                        remove_cv_t<int *          volatile>>::value));
        ASSERT((is_same<remove_cv  <TestType       volatile>::type,
                        remove_cv_t<TestType       volatile>>::value));
        ASSERT((is_same<remove_cv  <int      const volatile>::type,
                        remove_cv_t<int      const volatile>>::value));
        ASSERT((is_same<remove_cv  <int *    const volatile>::type,
                        remove_cv_t<int *    const volatile>>::value));
        ASSERT((is_same<remove_cv  <TestType const volatile>::type,
                        remove_cv_t<TestType const volatile>>::value));

        ASSERT((is_same<remove_cv  <const int[5]>::type,
                        remove_cv_t<const int[5]>>::value));

        ASSERT((is_same<remove_cv  <               Pm>::type,
                        remove_cv_t<               Pm>>::value));
        ASSERT((is_same<remove_cv  <const          Pm>::type,
                        remove_cv_t<const          Pm>>::value));
        ASSERT((is_same<remove_cv  <      volatile Pm>::type,
                        remove_cv_t<      volatile Pm>>::value));
        ASSERT((is_same<remove_cv  <const volatile Pm>::type,
                        remove_cv_t<const volatile Pm>>::value));
        ASSERT((is_same<remove_cv  <               Pmf>::type,
                        remove_cv_t<               Pmf>>::value));
        ASSERT((is_same<remove_cv  <const          Pmf>::type,
                        remove_cv_t<const          Pmf>>::value));
        ASSERT((is_same<remove_cv  <      volatile Pmf>::type,
                        remove_cv_t<      volatile Pmf>>::value));
        ASSERT((is_same<remove_cv  <const volatile Pmf>::type,
                        remove_cv_t<const volatile Pmf>>::value));
        ASSERT((is_same<remove_cv  <               Pmq>::type,
                        remove_cv_t<               Pmq>>::value));
        ASSERT((is_same<remove_cv  <const          Pmq>::type,
                        remove_cv_t<const          Pmq>>::value));
        ASSERT((is_same<remove_cv  <      volatile Pmq>::type,
                        remove_cv_t<      volatile Pmq>>::value));
        ASSERT((is_same<remove_cv  <const volatile Pmq>::type,
                        remove_cv_t<const volatile Pmq>>::value));
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
