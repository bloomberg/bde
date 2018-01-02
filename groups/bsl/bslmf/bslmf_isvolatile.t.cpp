// bslmf_isvolatile.t.cpp                                             -*-C++-*-
#include <bslmf_isvolatile.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_volatile', that
// determines whether a template parameter type is a 'volatile'-qualified type.
// Thus, we need to ensure that the value returned by the meta-function is
// correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_volatile<TYPE>
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] Function-overload consistency

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

//# define BSLMF_ISVOLATILE_SHOW_COMPILER_ERRORS 1
#if !defined(BSLMF_ISVOLATILE_SHOW_COMPILER_ERRORS)

# if defined(BSLS_PLATFORM_CMP_IBM)                                           \
  || defined(BSLS_PLATFORM_CMP_SUN)                                           \
  ||(defined(BSLS_PLATFORM_CMP_GNU)  && BSLS_PLATFORM_CMP_VERSION <= 40400)   \
  ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1900)
// The xlC and Sun CC compilers mistakenly detect function types with trailing
// cv-qualifiers as being cv-qualified themselves.  However, in such cases the
// cv-qualifier applies to the (hidden) 'this' pointer, as these function types
// exist only to be the result-type of a pointer-to-member type.  By definition
// no function type can ever be cv-qualified.  The Microsoft compiler cannot
// parse such types at all.
//
// Note that we could obtain the correct answer by deriving 'is_volatile' from
// (the negation of) 'is_function', but that simply exposes that our current
// implementation of 'is_function' does not detect such types either.
#   define BSLMF_ISVOLATILE_COMPILER_MISREPORTS_ABOMINABLE_FUNCTION_TYPES
# endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1800
// The Microsoft Visual C++ compiler. prior to VC2015, will correctly match an
// array of cv-qualified elements to a function template overload for a
// compatible cv-reference type, but it also retains the full cv-qualifier on
// the deduced type.  The trait is manually tested to confirm that it gives the
// correct result, so we define a macro allowing us to disable the affected
// tests on this platform.
#   define BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_CV_QUAL_FOR_ARRAYS
#endif

# if defined(BSLS_PLATFORM_CMP_IBM)                                           \
  || defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_CV_QUAL_FOR_ARRAYS)
// The IBM xlC compiler correctly matches an array of 'const volatile' elements
// to a function template taking 'const T&', but incorrectly deduces 'T' to be
// 'const volatile X[N]' rather than simply 'volatile X[N]'.  The trait is
// manually tested to confirm that it gives the correct result, so we define
// a macro allowing us to disable the affected tests on this platform.
#   define BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_TYPE_FOR_CV_ARRAY
# endif

#endif // BSLMF_ISVOLATILE_SHOW_COMPILER_ERRORS

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consquently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_CMP_GNU)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180) // cv-qualifiers meaningless on function types
#endif

//=============================================================================
//                      TYPES TO SUPPORT TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
    // This user-defined type is intended to be used for testing.
};

}  // close unnamed namespace

//=============================================================================
//                      FUNCTIONS TO SUPPORT TESTING
//-----------------------------------------------------------------------------

template <class TRAIT>
bool eval_dispatch(TRAIT, bsl::true_type)
    // Return 'TRAIT::value', and 'ASSERT' that the deduced type 'TRAIT' has
    // the same 'value', 'VALUE' and 'type' as the 'bsl::true_type' trait.
{
    ASSERT((bsl::is_same<typename TRAIT::type, bsl::true_type>::value));
    ASSERT(true == TRAIT::value);
    ASSERT(true == TRAIT::VALUE);
    return TRAIT::value;
}

template <class TRAIT>
bool eval_dispatch(TRAIT, bsl::false_type)
    // Return 'TRAIT::value', and 'ASSERT' that the deduced type 'TRAIT' has
    // the same 'value', 'VALUE' and 'type' as the 'bsl::false_type' trait.
{
    ASSERT((bsl::is_same<typename TRAIT::type, bsl::false_type>::value));
    ASSERT(false == TRAIT::value);
    ASSERT(false == TRAIT::VALUE);
    return TRAIT::value;
}

template <class TRAIT>
bool eval(const TRAIT& value)
    // Return 'TRAIT::value', and confirm that the deduced type 'TRAIT' has the
    // base-characteristics of either 'bsl::true_type' or 'bsl::false_type'.
{
    return eval_dispatch(value, value);
}

template <class DEDUCED_TYPE>
bool testCVDeduction(DEDUCED_TYPE &)
{
    return bsl::is_volatile<DEDUCED_TYPE>::value;
}

template <class DEDUCED_TYPE>
bool testCVOverload(DEDUCED_TYPE &)
{
    ASSERT(false == bsl::is_volatile<DEDUCED_TYPE>::value);
    return bsl::is_volatile<DEDUCED_TYPE>::value;
}

template <class DEDUCED_TYPE>
bool testCVOverload(volatile DEDUCED_TYPE &)
{
    ASSERT(false == bsl::is_volatile<DEDUCED_TYPE>::value);
    return bsl::is_volatile<DEDUCED_TYPE>::value;
}

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

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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
///Example 1: Verify 'Volatile' Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is
// 'volatile'-qualified.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type and an
// unqualified type:
//..
    typedef int           MyType;
    typedef volatile int  MyVolatileType;
//..
// Now, we instantiate the 'bsl::is_volatile' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_volatile<MyType>::value);
    ASSERT(true  == bsl::is_volatile<MyVolatileType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSISTENCY OF TRAIT AND FUNCTION OVERLOADS
        //   Test that the value of the trait corresponds with the value that
        //   is expected for a type deduced inside a function template.  The
        //   main concern here is that compilers with buggy behavior, such as
        //   the implementation works around for arrays, may deduce types in a
        //   similar manner, and forcing the correct result on the trait would
        //   produce an inconsistent program.  The tests below should expose
        //   any inconsistency from our forced-correct trait value, with the
        //   types deduced by the compiler in generic code.
        //
        // Concerns:
        //: 1 'is_volatile<TYPE>::value' has the value expected for the type
        //:   deduced for a single function template, with no overloads, that
        //:   deduces the complete type, including cv-qualifiers, from its
        //:   argument passed by reference.
        //:
        //: 2 'is_volatile<TYPE>::value' is always 'false' for a type deduced
        //:   from a pair of function template overloads taking their arguments
        //:   by reference, and by const-reference.
        //:
        //: 3 Given the specific information that some platforms require a
        //:   special implementation for arrays, multidimensional arrays should
        //:   have the same result as this trait applied to an array of a
        //:   single dimension with the same (potentially cv-qualified) element
        //:   type.
        //
        // Plan:
        //: 1 Verify that 'bsl::is_volatile<TYPE>::value' has the correct value
        //:   for each concern.
        //
        // Testing:
        //   Function-overload consistency
        // --------------------------------------------------------------------

        if (verbose)
             printf("\nTESTING CONSISTENCY OF TRAIT AND FUNCTION OVERLOADS"
                    "\n===================================================\n");

                       int data                        =   0;
        const          int constData                   =   0;
              volatile int volatileData                =   0;
        const volatile int constVolatileData           =   0;

                       int array[2]                    = { 0, 1 };
        const          int constArray[2]               = { 0, 1 };
              volatile int volatileArray[2]            = { 0, 1 };
        const volatile int constVolatileArray[2]       = { 0, 1 };

                       int arrayUB[]                   = { 0, 1 };
        const          int constArrayUB[]              = { 0, 1 };
              volatile int volatileArrayUB[]           = { 0, 1 };
        const volatile int constVolatileArrayUB[]      = { 0, 1 };

                       int array2D[2][2]               = {{0, 1}, {2, 3}};
        const          int constArray2D[2][2]          = {{0, 1}, {2, 3}};
              volatile int volatileArray2D[2][2]       = {{0, 1}, {2, 3}};
        const volatile int constVolatileArray2D[2][2]  = {{0, 1}, {2, 3}};

                       int arrayUB2D[][2]              = {{0, 1}, {2, 3}};
        const          int constArrayUB2D[][2]         = {{0, 1}, {2, 3}};
              volatile int volatileArrayUB2D[][2]      = {{0, 1}, {2, 3}};
        const volatile int constVolatileArrayUB2D[][2] = {{0, 1}, {2, 3}};

        ASSERT(false == testCVDeduction(data));
        ASSERT(false == testCVDeduction(constData));
        ASSERT( true == testCVDeduction(volatileData));
        ASSERT( true == testCVDeduction(constVolatileData));

        ASSERT(false == testCVDeduction(array));
        ASSERT(false == testCVDeduction(constArray));
        ASSERT( true == testCVDeduction(volatileArray));
        ASSERT( true == testCVDeduction(constVolatileArray));

        ASSERT(false == testCVDeduction(arrayUB));
        ASSERT(false == testCVDeduction(constArrayUB));
        ASSERT( true == testCVDeduction(volatileArrayUB));
        ASSERT( true == testCVDeduction(constVolatileArrayUB));

        ASSERT(false == testCVDeduction(array2D));
        ASSERT(false == testCVDeduction(constArray2D));
        ASSERT (true == testCVDeduction(volatileArray2D));
        ASSERT( true == testCVDeduction(constVolatileArray2D));

        ASSERT(false == testCVDeduction(arrayUB2D));
        ASSERT(false == testCVDeduction(constArrayUB2D));
        ASSERT( true == testCVDeduction(volatileArrayUB2D));
        ASSERT( true == testCVDeduction(constVolatileArrayUB2D));

        // Overload match should implicitly strip off 'const', so the following
        // test functions should always return 'false'.

        ASSERT(false == testCVOverload(data));
        ASSERT(false == testCVOverload(constData));
        ASSERT(false == testCVOverload(volatileData));
        ASSERT(false == testCVOverload(constVolatileData));

        ASSERT(false == testCVOverload(array));
        ASSERT(false == testCVOverload(constArray));
#if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_CV_QUAL_FOR_ARRAYS)
        ASSERT(false == testCVOverload(volatileArray));
# if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_TYPE_FOR_CV_ARRAY)
        ASSERT(false == testCVOverload(constVolatileArray));
# endif
#endif

        ASSERT(false == testCVOverload(arrayUB));
        ASSERT(false == testCVOverload(constArrayUB));
#if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_CV_QUAL_FOR_ARRAYS)
        ASSERT(false == testCVOverload(volatileArrayUB));
# if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_TYPE_FOR_CV_ARRAY)
        ASSERT(false == testCVOverload(constVolatileArrayUB));
# endif
#endif

        ASSERT(false == testCVOverload(array2D));
        ASSERT(false == testCVOverload(constArray2D));
#if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_TYPE_FOR_CV_ARRAY)
        ASSERT(false == testCVOverload(volatileArray2D));
        ASSERT(false == testCVOverload(constVolatileArray2D));
#endif

        ASSERT(false == testCVOverload(arrayUB2D));
        ASSERT(false == testCVOverload(constArrayUB2D));
#if !defined(BSLMF_ISVOLATILE_COMPILER_DEDUCES_BAD_TYPE_FOR_CV_ARRAY)
        ASSERT(false == testCVOverload(volatileArrayUB2D));
        ASSERT(false == testCVOverload(constVolatileArrayUB2D));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_volatile<TYPE>'
        //   Ensure 'bsl::is_volatile' has the correct base-characteristics
        //   for a variety of template parameter types, and neither hides nor
        //   makes ambiguous the salient elements of the 'integral_constant'
        //   interface.
        //
        // Concerns:
        //: 1 'is_volatile<T>::value' is 'false' when 'T' is a (possibly
        //:   'volatile'-qualified) type.
        //:
        //: 2 'is_volatile<T>::value' is 'true' when 'T' is a 'const'-qualified
        //:   or cv-qualified type.
        //:
        //: 3 'is_volatile<T>::VALUE' has the same value as
        //:   'is_volatile<T>::value'.
        //:
        //: 4 'is_volatile<T>' is publicly and unambiguously derived from
        //:   either 'true_type' or 'false_type', according to concerns C-1 and
        //:   C-2.
        //:
        //: 5 Objects of type 'is_volatile<T>' can be default constructed and
        //:   copied, for use in tag-dispatch schemes.
        //
        // Plan:
        //: 1 Call a test function template with a value-initialized object of
        //:   type 'is_volatile<T>' for a 'T' of each possible value category
        //:
        //:  1a) That function shall dispatch to a further overload set that
        //:      tag-dispatches on 'true_type' and 'false_type'.
        //:  1b) Within the deeper dispatch, confirm that 'value' and 'VALUE'
        //:      have the same value as the corresponding tag type.
        //:  1c) Return 'value' to compare with the expected result for the
        //:      template argument 'T'.
        //
        // Testing:
        //   bsl::is_volatile<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::is_volatile<TYPE>'"
                            "\n================================\n");

        // C-1
        ASSERT(false == eval(bsl::is_volatile<int>()));
        ASSERT(false == eval(bsl::is_volatile<int const>()));

        ASSERT(false == eval(bsl::is_volatile<TestType>()));
        ASSERT(false == eval(bsl::is_volatile<TestType const>()));

        ASSERT(false == eval(bsl::is_volatile<int &>()));
        ASSERT(false == eval(bsl::is_volatile<const int &>()));
        ASSERT(false == eval(bsl::is_volatile<volatile int &>()));

        ASSERT(false == eval(bsl::is_volatile<volatile int *>()));
        ASSERT(false == eval(bsl::is_volatile<const volatile int *>()));
        ASSERT(false == eval(bsl::is_volatile<volatile int TestType::*>()));
        ASSERT(false == eval(
                          bsl::is_volatile<const volatile int TestType::*>()));

        ASSERT(false == eval(bsl::is_volatile<void>()));
        ASSERT(false == eval(bsl::is_volatile<void const>()));

        ASSERT(false == eval(bsl::is_volatile<volatile int()>()));
        ASSERT(false == eval(bsl::is_volatile<volatile int(&)()>()));
        ASSERT(false == eval(bsl::is_volatile<volatile int(*)()>()));

#if !defined BSLMF_ISVOLATILE_COMPILER_MISREPORTS_ABOMINABLE_FUNCTION_TYPES
        // Additional tests for abominable function types
        ASSERT(false == eval(bsl::is_volatile<volatile int() volatile>()));
        ASSERT(
             false == eval(bsl::is_volatile<volatile int() const volatile>()));
#endif

        ASSERT(false == eval(bsl::is_volatile<int[4]>()));
        ASSERT(false == eval(bsl::is_volatile<const int[4]>()));

        ASSERT(false == eval(bsl::is_volatile<int[4][2]>()));
        ASSERT(false == eval(bsl::is_volatile<const int[4][2]>()));

        ASSERT(false == eval(bsl::is_volatile<int[]>()));
        ASSERT(false == eval(bsl::is_volatile<const int[]>()));

        ASSERT(false == eval(bsl::is_volatile<int[][2]>()));
        ASSERT(false == eval(bsl::is_volatile<const int[][2]>()));

        // C-2
        ASSERT(true == eval(bsl::is_volatile<int volatile>()));
        ASSERT(true == eval(bsl::is_volatile<int const volatile>()));

        ASSERT(true == eval(bsl::is_volatile<TestType volatile>()));
        ASSERT(true == eval(bsl::is_volatile<TestType const volatile>()));

        ASSERT(true == eval(bsl::is_volatile<void volatile>()));
        ASSERT(true == eval(bsl::is_volatile<void const volatile>()));

        ASSERT(true == eval(bsl::is_volatile<int * const volatile>()));
        ASSERT(true == eval(bsl::is_volatile<int * const volatile>()));
        ASSERT(true == eval(bsl::is_volatile<int(* volatile)()>()));
        ASSERT(true == eval(bsl::is_volatile<int(* const volatile)()>()));

        ASSERT(true == eval(bsl::is_volatile<int TestType::* volatile>()));
        ASSERT(true == eval(
                          bsl::is_volatile<int TestType::* const volatile>()));

        ASSERT(true == eval(bsl::is_volatile<volatile int[4]>()));
        ASSERT(true == eval(bsl::is_volatile<const volatile int[4]>()));

        ASSERT(true == eval(bsl::is_volatile<volatile int[4][2]>()));
        ASSERT(true == eval(bsl::is_volatile<const volatile int[4][2]>()));

        ASSERT(true == eval(bsl::is_volatile<volatile int[]>()));
        ASSERT(true == eval(bsl::is_volatile<const volatile int[]>()));

        ASSERT(true == eval(bsl::is_volatile<volatile int[][2]>()));
        ASSERT(true == eval(bsl::is_volatile<const volatile int[][2]>()));

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
