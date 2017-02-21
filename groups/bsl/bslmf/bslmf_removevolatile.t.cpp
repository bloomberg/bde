// bslmf_removevolatile.t.cpp                                         -*-C++-*-
#include <bslmf_removevolatile.h>

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
// The component under test defines a meta-function, 'bsl::remove_volatile',
// that removes any top-level 'volatile'-qualifier from a template parameter
// type.  Thus, we need to ensure that the values returned by the meta-function
// are correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_volatile::type
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

//# define BSLMF_REMOVEVOLATILE_SHOW_COMPILER_ERRORS 1
#if !defined(BSLMF_REMOVEVOLATILE_SHOW_COMPILER_ERRORS)

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
# define BSLMF_REMOVEVOLATILE_COMPILER_MISMATCHES_ABOMINABLE_FUNCTION_TYPES 1
#endif

# if defined(BSLS_PLATFORM_CMP_IBM)
#   define BSLMF_REMOVEVOLATILE_DO_NOT_TEST_CV_REF_TO_FUNCTION_TYPES 1
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

#if defined(BSLS_PLATFORM_CMP_GNU)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180) // cv-qualifiers meaningless on function types
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::remove_volatile'.
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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the 'volatile'-qualifier of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any 'volatile'-qualifier from a particular
// type.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
        typedef int          MyType;
        typedef volatile int MyVolatileType;
//..
// Now, we remove the 'volatile'-qualifier from 'MyVolatileType' using
// 'bsl::remove_volatile' and verify that the resulting type is the same as
// 'MyType':
//..
        ASSERT(true ==
               (bsl::is_same<bsl::remove_volatile<MyVolatileType>::type,
                                                              MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::remove_volatile<T>::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_volatile' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::remove_volatile' leaves types that are not
        //:   'volatile'-qualified at the top-level as-is.
        //:
        //: 2 'bsl::remove_volatile' removes any top-level
        //:   'volatile'-qualifier.
        //:
        //: 3 'bsl::remove_volatile' removes any top-level 'volatile'-qualifier
        //:   from a pointer-to-member object type, and not from the qualifier
        //:   in the pointed-to member.
        //
        // Plan:
        //   Verify that 'bsl::remove_volatile::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::remove_volatile::type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::remove_volatile<T>::type'"
                            "\n=======================================\n");

        // C-1
        ASSERT((is_same<remove_volatile<int>::type, int>::value));
        ASSERT((is_same<remove_volatile<int *>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType>::type, TestType>::value));
        ASSERT((is_same<remove_volatile<int volatile *>::type,
                                        int volatile *>::value));
        ASSERT((is_same<remove_volatile<int volatile &>::type,
                                        int volatile &>::value));
        ASSERT((is_same<remove_volatile<int volatile()>::type,
                                        int volatile()>::value));
        ASSERT((is_same<remove_volatile<int[5]>::type,
                                        int[5]>::value));
        ASSERT((is_same<remove_volatile<int[5][2]>::type,
                                        int[5][2]>::value));
        ASSERT((is_same<remove_volatile<int[5][2][3]>::type,
                                        int[5][2][3]>::value));
        ASSERT((is_same<remove_volatile<int[]>::type,
                                        int[]>::value));
        ASSERT((is_same<remove_volatile<int[][2]>::type,
                                        int[][2]>::value));
        ASSERT((is_same<remove_volatile<int[][2][3]>::type,
                                        int[][2][3]>::value));

        ASSERT((is_same<remove_volatile<void>::type, void>::value));
        ASSERT((is_same<remove_volatile<const void>::type,
                                        const void>::value));

        ASSERT((is_same<remove_volatile<
                               volatile int (TestType::*)() volatile>::type,
                               volatile int (TestType::*)() volatile>::value));

        ASSERT((is_same<remove_volatile<volatile int TestType::*>::type,
                                        volatile int TestType::*>::value));

#ifndef BSLMF_REMOVEVOLATILE_COMPILER_MISMATCHES_ABOMINABLE_FUNCTION_TYPES
        ASSERT((is_same<remove_volatile<volatile int() volatile>::type,
                                        volatile int() volatile>::value));

        ASSERT((is_same<
                       remove_volatile<volatile int() const volatile>::type,
                                       volatile int() const volatile>::value));
#endif

        // C-2
        ASSERT((is_same<remove_volatile<int volatile>::type, int>::value));
        ASSERT((is_same<remove_volatile<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType volatile>::type,
                                                            TestType>::value));

        ASSERT((is_same<remove_volatile<int const volatile>::type,
                                                           int const>::value));
        ASSERT((is_same<remove_volatile<int * const volatile>::type,
                                                         int * const>::value));
        ASSERT((is_same<remove_volatile<TestType const volatile>::type,
                                                      TestType const>::value));

        ASSERT((is_same<remove_volatile<volatile int[5]>::type,
                                                 int[5]>::value));
        ASSERT((is_same<remove_volatile<volatile int[5][2]>::type,
                                                 int[5][2]>::value));
        ASSERT((is_same<remove_volatile<volatile int[5][2][3]>::type,
                                                 int[5][2][3]>::value));

        ASSERT((is_same<remove_volatile<volatile int[]>::type,
                                                 int[]>::value));
        ASSERT((is_same<remove_volatile<volatile int[][2]>::type,
                                                 int[][2]>::value));
        ASSERT((is_same<remove_volatile<volatile int[][2][3]>::type,
                                                 int[][2][3]>::value));

        ASSERT((is_same<remove_volatile<volatile void>::type,
                                                 void>::value));
        ASSERT((is_same<remove_volatile<const volatile void>::type,
                                        const          void>::value));

        // C-3
        ASSERT((is_same<remove_volatile<               Pm>::type,
                                                       Pm>::value));
        ASSERT((is_same<remove_volatile<const          Pm>::type,
                                        const          Pm>::value));
        ASSERT((is_same<remove_volatile<      volatile Pm>::type,
                                                       Pm>::value));
        ASSERT((is_same<remove_volatile<const volatile Pm>::type,
                                        const          Pm>::value));

        ASSERT((is_same<remove_volatile<               Pmf>::type,
                                                       Pmf>::value));
        ASSERT((is_same<remove_volatile<const          Pmf>::type,
                                        const          Pmf>::value));
        ASSERT((is_same<remove_volatile<      volatile Pmf>::type,
                                                       Pmf>::value));
        ASSERT((is_same<remove_volatile<const volatile Pmf>::type,
                                        const          Pmf>::value));

        ASSERT((is_same<remove_volatile<               Pmq>::type,
                                                       Pmq>::value));
        ASSERT((is_same<remove_volatile<const          Pmq>::type,
                                        const          Pmq>::value));
        ASSERT((is_same<remove_volatile<      volatile Pmq>::type,
                                                       Pmq>::value));
        ASSERT((is_same<remove_volatile<const volatile Pmq>::type,
                                        const          Pmq>::value));


        // C-4
        ASSERT((is_same<remove_volatile<int volatile(&)()>::type,
                                        int volatile(&)()>::value));

# if!defined(BSLMF_REMOVEVOLATILE_DO_NOT_TEST_CV_REF_TO_FUNCTION_TYPES)
        typedef int const FnType();

        ASSERT((is_same<remove_volatile<volatile FnType&>::type,
                                        volatile FnType&>::value));
#endif

        ASSERT((is_same<remove_volatile<int volatile(* volatile  )()>::type,
                                        int volatile(*           )()>::value));
        ASSERT((is_same<remove_volatile<int volatile(* volatile &)()>::type,
                                        int volatile(* volatile &)()>::value));
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
