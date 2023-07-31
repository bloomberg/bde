// bslmf_isfloatingpoint.t.cpp                                        -*-C++-*-
#include <bslmf_isfloatingpoint.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                        // 'std::is_base_of',
                        // 'std::is_floating_point', and
                        // 'std::is_floating_point_v' (C++17)
#endif

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_floating_point'
// and a template variable 'bsl::is_floating_point_v' that determine whether a
// template parameter type is a floating-point type.  Thus, we need to ensure
// that the values returned by the meta-function are correct for each possible
// category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_floating_point::value
// [ 1] bsl::is_floating_point_v
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Conforms to implementation constraints.

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

namespace {

struct TestType {
    // This user-defined type is used for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
    ENUM_TEST_VALUE0 = 0,
    ENUM_TEST_VALUE1
};

typedef float (TestType::*RetFloatingPointMethodPtrType) ();
    // This pointer to function member type that returns an integral type is
    // used for testing.

typedef double (*RetFloatingPointFunctionPtrType) ();
    // This pointer to function type that returns a floating-point type is used
    // for testing.

}  // close unnamed namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#define ASSERT_V_SAME(TYPE)                                                   \
    ASSERT(bsl::is_floating_point  <TYPE>::value ==                           \
           bsl::is_floating_point_v<TYPE>)
    // 'ASSERT' that 'is_floating_point_v' has the same value as
    // 'is_floating_point::value'.
#else
#define ASSERT_V_SAME(TYPE)
#endif

#define TYPE_ASSERT_CVQ(metaFunc, member, type, result)                       \
    ASSERT(result == metaFunc<               type>::member);                  \
    ASSERT(result == metaFunc<const          type>::member);                  \
    ASSERT(result == metaFunc<      volatile type>::member);                  \
    ASSERT(result == metaFunc<const volatile type>::member);                  \
    ASSERT_V_SAME(               type);                                       \
    ASSERT_V_SAME(const          type);                                       \
    ASSERT_V_SAME(      volatile type);                                       \
    ASSERT_V_SAME(const volatile type);
    // Test all cv-qualified combinations on the specified 'type' and confirm
    // that the result value of the 'metaFunc' and the expected 'result' value
    // are the same.  Also test that all cv-qualified combinations on the
    // 'type' has the same value as the 'metaFunc_v' template variable
    // instantiated with the same types.

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
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.z
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
///Example 1: Verify Floating-Point Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a floating-point
// type.
//
// First, we create two 'typedef's -- a floating-point type and a
// non-floating-point type:
//..
    typedef void  MyType;
    typedef float MyFloatingPointType;
//..
// Now, we instantiate the 'bsl::is_floating_point' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_floating_point<MyType>::value);
    ASSERT(true  == bsl::is_floating_point<MyFloatingPointType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above using the
// 'bsl::is_floating_point_v<T> as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(false == bsl::is_floating_point_v<MyType>);
    ASSERT(true  == bsl::is_floating_point_v<MyFloatingPointType>);
#endif
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLEMENTATION
        //
        // Concerns:
        //: 1 The 'bsl::is_floating_point' meta function is *never* implemented
        //:   as an alias to the 'std::is_floating_point' Standard meta
        //:   function.
        //:
        //: 2 The 'bsl::is_floating_point' meta function is *always* based on
        //:   either 'bsl::true_type' or 'bsl::false_type'.
        //:
        //: 3 The 'bsl::is_floating_point_v' variable template *is* implemented
        //:   using the 'std::is_floating_point_v' Standard variable template.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is
        //:   defined, use 'bsl::is_same' to compare 'bsl::is_floating_point'
        //:   to 'std::is_floating_point' using a representative type.  (C-1)
        //:
        //: 2 When 'BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY' is
        //:   defined, use 'std::is_base_of' to confirm that the
        //:   'bsl::is_floating_point' meta function has 'bsl::true_type' or
        //:   'bsl::false_type', as appropriate, as a base class.  (C-2)
        //:
        //:   o For all versions, explicitly access the 'value' member to
        //:     confirm that the inheritance is neither 'private', 'protected',
        //:     nor ambiguous.
        //:
        //: 3 When 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' and (for
        //:   'std::is_floating_point_v')
        //:   'BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY' are defined,
        //:   compare for equality the addresses of 'bsl::is_floating_point_v'
        //:   and 'std::is_floating_point_v' using a representative type.
        //:   (C-3)
        //
        // Testing:
        //   CONCERN: Conforms to implementation constraints.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING IMPLEMENTATION"
                            "\n======================\n");

        if (veryVerbose) printf(
                            "\nTesting 'is_floating_point' using 'double'.\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                                                // for 'std::is_floating_point'
        ASSERT((false == bsl::is_same<
                                     bsl::is_floating_point<double>,
                                     std::is_floating_point<double> >::value));

        ASSERT((std::is_base_of<bsl::true_type,
                                bsl::is_floating_point<double>>::value));

        ASSERT((std::is_base_of<bsl::false_type,
                                bsl::is_floating_point<int>>::value));
#endif

        ASSERT(true  == bsl::is_floating_point<double>::value);
        ASSERT(false == bsl::is_floating_point<int   >::value);

#if defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES                  \
 && defined BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        if (veryVerbose) printf(
                          "\nTesting 'is_floating_point_v' using 'double'.\n");

        typedef double T;

        ASSERT((&bsl::is_floating_point_v<T> == &std::is_floating_point_v<T>));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_floating_point::value'
        //   Ensure that 'bsl::is_floating_point' returns the correct values
        //   for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'is_floating_point::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) non-integral primitive type.
        //:
        //: 2 'is_floating_point::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_floating_point::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) enum type.
        //:
        //: 4 'is_floating_point::value' is 'false' when 'TYPE' is a pointer to
        //:   (possibly cv-qualified) floating-point type.
        //:
        //: 5 'is_floating_point::value' is 'false' when 'TYPE' is a reference
        //:   to (possibly cv-qualified) floating-point type.
        //:
        //: 6 'is_integral::value' is 'false' when 'TYPE' is a function pointer
        //:   type returning an integral type.
        //:
        //: 7 'is_integral::value' is 'false' when 'TYPE' is a function member
        //:   pointer type returning an integral type.
        //:
        //: 8 'is_floating_point::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) floating-point type.
        //:
        //: 10 That 'is_floating_point<T>::value' has the same value as
        //:    'is_floating_point_v<T>' for a variety of template parameter
        //:    types.
        //
        // Plan:
        //: 1 Verify that 'bsl::is_floating_point::value' has the correct value
        //:   for each concern.
        //
        // Testing:
        //   bsl::is_floating_point::value
        //   bsl::is_floating_point_v
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_floating_point::value'"
                            "\n===============================\n");

        // C-1
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, int, false);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, long int, false);

        // C-2
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, TestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, EnumTestType, false);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, float *, false);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, double *, false);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, long double *, false);

        // C-5
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, float&, false);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, double&, false);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, long double&, false);

        // C-6
        TYPE_ASSERT_CVQ(bsl::is_floating_point,
                        value,
                        RetFloatingPointFunctionPtrType,
                        false);

        // C-7
        TYPE_ASSERT_CVQ(bsl::is_floating_point,
                        value,
                        RetFloatingPointMethodPtrType,
                        false);

        // C-8
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, float, true);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, double, true);
        TYPE_ASSERT_CVQ(bsl::is_floating_point, value, long double, true);

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
