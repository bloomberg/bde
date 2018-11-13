// bslmf_addpointer.t.cpp                                             -*-C++-*-
#include <bslmf_addpointer.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions, 'bsl::add_pointer' and
// 'bsl::add_pointer_t', that transform a type to a pointer type to that type.
// We need to ensure that the values returned by the meta-function are correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPE
// [ 1] bsl::add_pointer::type
// [ 1] bsl::add_pointer_t
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
//                  COMPILER DEFECT MACROS TO GUIDE TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900)
# define BSLMF_ADDPOINTER_CREATES_ABOMINABLE_POINTERS 1
// Some compilers erroneously support pointers to "abominable" function types.
// This macro indicates that they are expected to produce the wrong result.

typedef int AbominableFunction() const volatile;
typedef AbominableFunction* BadFunctionPointer;

    // Test to confirm that this compiler supports the buggy syntax.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for a template parameter.
};

typedef void (TestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::add_pointer'.

typedef int TestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::add_pointer'.

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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform Type to Pointer Type to that Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a type to a pointer type to that type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType') and the type
// pointed to by the pointer type ('MyType'):
//..
    typedef int   MyType;
    typedef int * MyPtrType;
//..
// Now, we transform 'MyType' to a pointer type to 'MyType' using
// 'bsl::add_pointer' and verify that the resulting type is the same as
// 'MyPtrType':
//..
    ASSERT((bsl::is_same<bsl::add_pointer<MyType>::type,
                         MyPtrType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// transform 'MyType' to a pointer type using 'bsl::add_pointer_t' and verify
// that the resulting type is the same as 'MyPtrType':
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    ASSERT((bsl::is_same<bsl::add_pointer_t<MyType>, MyPtrType>::value));
#endif
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_pointer::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::add_pointer' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::add_pointer' transforms a non-reference type to a pointer
        //:   type pointing to the original type.
        //:
        //: 2 'bsl::add_pointer' transforms a reference type to a pointer
        //:   type pointing to the type referred to by the reference type.
        //:
        //: 3 'bsl::add_pointer' does not decay arrays.
        //:
        //: 4 'bsl::add_pointer' produces function-pointers from function types
        //:   and function-reference-types.
        //:
        //: 5 'bsl::add_pointer' does not change the type of an "abominable"
        //:   function type (with a trailing cv-qualifier) as there is no legal
        //:   pointer type to transform into.  Note that some compilers are
        //:   known to have bugs, and we should test that this trait produces
        //:   the corresponding (illegal) type on such compilers, rather than
        //:   failing to compile at all.
        //:
        //: 6 'bsl::add_pointer_t' represents the return type of
        //:   'bsl::add_pointer' meta-function for a variety of template
        //:   parameter types.
        //
        // Plan:
        //   Verify that 'bsl::add_pointer::type' has the correct type for
        //   each concern.
        //
        // Testing:
        //   bsl::add_pointer::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::add_pointer::type'\n"
                            "\n========================\n");

        // C-1
        ASSERT((is_same<add_pointer<void>::type, void *>::value));
        ASSERT((is_same<add_pointer<const void>::type, const void *>::value));
        ASSERT((is_same<add_pointer<int>::type, int *>::value));
        ASSERT((is_same<add_pointer<const int>::type, const int *>::value));
        ASSERT((is_same<add_pointer<int *>::type, int **>::value));
        ASSERT((is_same<add_pointer<TestType>::type, TestType *>::value));

        // C-2
        ASSERT((is_same<add_pointer<int &>::type, int *>::value));
        ASSERT((is_same<add_pointer<int const &>::type, int const *>::value));
        ASSERT((is_same<add_pointer<TestType &>::type, TestType *>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT((is_same<add_pointer<int &&>::type, int *>::value));
        ASSERT((is_same<add_pointer<int const &&>::type, int const *>::value));
#endif

        // C-3
        typedef int   IntArray    [];
        typedef int (*IntArrayPtr)[];
        typedef int (&IntArrayRef)[];

        typedef int   IntArray5    [5];
        typedef int (*IntArrayPtr5)[5];
        typedef int (&IntArrayRef5)[5];

        ASSERT((is_same<add_pointer<IntArray>::type,    IntArrayPtr >::value));
        ASSERT((is_same<add_pointer<IntArrayRef>::type, IntArrayPtr >::value));
        ASSERT((is_same<add_pointer<IntArrayPtr>::type, IntArrayPtr*>::value));

        ASSERT((is_same<add_pointer<IntArray5>::type, IntArrayPtr5>::value));
        ASSERT((is_same<add_pointer<IntArrayRef5>::type,
                        IntArrayPtr5>::value));
        ASSERT((is_same<add_pointer<IntArrayPtr5>::type,
                        IntArrayPtr5*>::value));

        // C-4
        typedef int   Function    (...);
        typedef int (&FunctionRef)(...);
        typedef int (*FunctionPtr)(...);

        ASSERT((is_same<add_pointer<Function>::type,    FunctionPtr >::value));
        ASSERT((is_same<add_pointer<FunctionRef>::type, FunctionPtr >::value));
        ASSERT((is_same<add_pointer<FunctionPtr>::type, FunctionPtr*>::value));

        // C-5
        typedef int AbominableFn() const volatile;

#if defined(BSLMF_ADDPOINTER_CREATES_ABOMINABLE_POINTERS)
        ASSERT((is_same<add_pointer<AbominableFn>::type,
                        AbominableFn *>::value));
#elif (!defined(BSLS_PLATFORM_CMP_GNU) || BSLS_PLATFORM_CMP_VERSION >= 40900)
        // gcc prior to v4.9 would produce the pointer-to-abominable type, but
        // reject parsing that pointer type explicitly, making the result type
        // impossible to test.

        ASSERT((is_same<add_pointer<AbominableFn>::type,
                        AbominableFn>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        if (verbose) printf("\n'bsl::add_pointer_t'\n"
                            "\n====================\n");

        // C-6
        ASSERT((is_same<add_pointer  <void       >::type,
                        add_pointer_t<void       >>::value));
        ASSERT((is_same<add_pointer  <const void >::type,
                        add_pointer_t<const void >>::value));
        ASSERT((is_same<add_pointer  <int        >::type,
                        add_pointer_t<int        >>::value));
        ASSERT((is_same<add_pointer  <const int  >::type,
                        add_pointer_t<const int  >>::value));
        ASSERT((is_same<add_pointer  <int *      >::type,
                        add_pointer_t<int *      >>::value));
        ASSERT((is_same<add_pointer  <TestType   >::type,
                        add_pointer_t<TestType   >>::value));
        ASSERT((is_same<add_pointer  <int &      >::type,
                        add_pointer_t<int &      >>::value));
        ASSERT((is_same<add_pointer  <int const &>::type,
                        add_pointer_t<int const &>>::value));
        ASSERT((is_same<add_pointer  <TestType & >::type,
                        add_pointer_t<TestType & >>::value));

       ASSERT((is_same<add_pointer  <MethodPtrTestType               >::type,
                       add_pointer_t<MethodPtrTestType               >>::value));
       ASSERT((is_same<add_pointer  <MethodPtrTestType &             >::type,
                       add_pointer_t<MethodPtrTestType &             >>::value));
       ASSERT((is_same<add_pointer  <MethodPtrTestType *             >::type,
                       add_pointer_t<MethodPtrTestType *             >>::value));
       ASSERT((is_same<add_pointer  <MethodPtrTestType const         >::type,
                       add_pointer_t<MethodPtrTestType const         >>::value));
       ASSERT((is_same<add_pointer  <MethodPtrTestType       volatile>::type,
                       add_pointer_t<MethodPtrTestType       volatile>>::value));
       ASSERT((is_same<add_pointer  <MethodPtrTestType const volatile>::type,
                       add_pointer_t<MethodPtrTestType const volatile>>::value));

       ASSERT((is_same<add_pointer  <PMD               >::type,
                       add_pointer_t<PMD               >>::value));
       ASSERT((is_same<add_pointer  <PMD &             >::type,
                       add_pointer_t<PMD &             >>::value));
       ASSERT((is_same<add_pointer  <PMD *             >::type,
                       add_pointer_t<PMD *             >>::value));
       ASSERT((is_same<add_pointer  <PMD const         >::type,
                       add_pointer_t<PMD const         >>::value));
       ASSERT((is_same<add_pointer  <PMD       volatile>::type,
                       add_pointer_t<PMD       volatile>>::value));
       ASSERT((is_same<add_pointer  <PMD const volatile>::type,
                       add_pointer_t<PMD const volatile>>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT((is_same<add_pointer  <int &&>::type,
                        add_pointer_t<int &&> >::value));
        ASSERT((is_same<add_pointer  <int const &&>::type,
                        add_pointer_t<int const &&> >::value));
#endif
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
// Copyright 2017 Bloomberg Finance L.P.
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
