// bslmf_isintegral.t.cpp                                             -*-C++-*-
#include <bslmf_isintegral.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addvolatile.h>

#include <bsls_bsltestutil.h>
#include <bsls_nullptr.h>
#include <bsls_types.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test implements a meta-function, 'bsl::is_integral' and
// a template variable 'bsl::is_integral_v', that determines whether a template
// parameter type is an integral type.  Thus, we need to ensure that the value
// returned by the meta-function is correct for each possible category of
// types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_integral::value
// [ 1] bsl::is_integral_v
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
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#define ASSERT_V_SAME(TYPE)                                                   \
    ASSERT(bsl::is_integral<TYPE>::value == bsl::is_integral_v<TYPE>)
    // Test whether 'bsl::is_integral_v<TYPE>' has the same value as
    // 'bsl::is_integral<TYPE>::value'.
#else
#define ASSERT_V_SAME(TYPE)
#endif

#define TYPE_ASSERT_CVQ(METAFUNC, MEMBER, TYPE, RESULT)                       \
    ASSERT(RESULT == METAFUNC<                  TYPE>::MEMBER);               \
    ASSERT(RESULT == METAFUNC<   bsl::add_const<TYPE>::type>::MEMBER);        \
    ASSERT(RESULT == METAFUNC<bsl::add_volatile<TYPE>::type>::MEMBER);        \
    ASSERT(RESULT == METAFUNC<      bsl::add_cv<TYPE>::type>::MEMBER);        \
    ASSERT_V_SAME(                              TYPE);                        \
    ASSERT_V_SAME(               bsl::add_const<TYPE>::type);                 \
    ASSERT_V_SAME(            bsl::add_volatile<TYPE>::type);                 \
    ASSERT_V_SAME(                  bsl::add_cv<TYPE>::type);
    // This macro allows for efficient testing of all cv-qualified combination
    // of a type.  Note the 'typename' is NOT missing from the invocations of
    // the 'add_qualifier' traits, as 'TYPE' will be textually substituted as
    // the exact type, rather than a type parameter, so this is NOT a dependant
    // context.  Conforming C++03 compilers would be required to reject this
    // code if the 'typename' were present; C++11 compilers should quietly
    // ignore it.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
    // This 'struct' provides a user-defined type is for testing.

    operator int() const { return 0; }
        // Return 0.  This function provides an implicit conversion-to-integer
        // operator to confirm that the trait under test is not fooled.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
    ENUM_TEST_VALUE0 = 0,
    ENUM_TEST_VALUE1
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum class EnumClassType {
    // This 'enum' type is used for testing.
};
#endif

typedef int RetIntegralFunctionType();
    // This function type that returns an integral type is used for testing.

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
///Example 1: Verify Integral Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an integral
// type.
//
// First, we create two 'typedef's -- an integral type and a non-integral type:
//..
    typedef void MyType;
    typedef int  MyIntegralType;
//..
// Now, we instantiate the 'bsl::is_integral' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_integral<MyType>::value);
    ASSERT(true  == bsl::is_integral<MyIntegralType>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_function_v' variable as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(false == bsl::is_integral_v<MyType>);
    ASSERT(true  == bsl::is_integral_v<MyIntegralType>);
#endif
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_integral::value'
        //   Ensure that 'bsl::is_integral' returns the correct values for a
        //   variety of template parameter types.
        //
        // Concerns:
        //:  1 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:    cv-qualified) non-integral primitive type.
        //:
        //:  2 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:    cv-qualified) user-defined type.
        //:
        //:  3 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:    cv-qualified) enum type.
        //:
        //:  4 'is_integral::value' is 'false' when 'TYPE' is a pointer to
        //:    (possibly cv-qualified) integral type.
        //:
        //:  5 'is_integral::value' is 'false' when 'TYPE' is a reference to
        //:    (possibly cv-qualified) integral type.
        //:
        //:  6 'is_integral::value' is 'false' when 'TYPE' is an array of an
        //:    integral type.
        //:
        //:  7 'is_integral::value' is 'false' when 'TYPE' is a function type
        //:    returning an integral type.
        //:
        //:  8 'is_integral::value' is 'false' when 'TYPE' is a pointer-to-data-
        //:    member returning an integral type.
        //:
        //:  9 'is_integral::value' is 'true' when 'TYPE' is a (possibly
        //:    cv-qualified) integral type.
        //:
        //: 10 The type names defined by 'bsls::Types' alias existing integral
        //:    types.  (This test is deferred from 'bsls_types' to this
        //:    component, as we need the machinery at this level for the test.)
        //:
        //: 11 That 'is_integral<T>::value' has the same value as
        //:    'is_integral_v<T>' for a variety of template parameter types.
        //
        // Plan:
        //: 1 Verify that 'bsl::is_integral<TYPE>::value' has the correct value
        //:   for types representative of each negative concern.
        //:
        //: 2 Verify that 'bsl::is_integral<TYPE>::value' has the correct value
        //:   for all the types which would produce a 'true' result.
        //
        // Testing:
        //   bsl::is_integral::value
        //   bsl::is_integral_v
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_integral::value'\n"
                            "\n=========================\n");

        // C-1
        TYPE_ASSERT_CVQ(bsl::is_integral, value, float, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, double, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long double, false);

        // C-2
        TYPE_ASSERT_CVQ(bsl::is_integral, value, TestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_integral, value, EnumTestType, false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        TYPE_ASSERT_CVQ(bsl::is_integral, value, EnumClassType, false);
#endif

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool *, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int *, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int *, false);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, bsl::nullptr_t, false);

        // C-5
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool&, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int&, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int&, false);

        // C-6
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool[5], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int[5][4], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long[], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short[][12], false);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool(*)[5], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int(*)[5][4], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long(*)[], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short(*)[][12], false);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool(&)[5], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int(&)[5][4], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long(&)[], false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short(&)[][12], false);

        // C-7
        TYPE_ASSERT_CVQ(bsl::is_integral,
                        value,
                        RetIntegralFunctionType,
                        false);

        TYPE_ASSERT_CVQ(bsl::is_integral,
                        value,
                        RetIntegralFunctionType *,
                        false);

        TYPE_ASSERT_CVQ(bsl::is_integral,
                        value,
                        RetIntegralFunctionType &,
                        false);

        // C-8
        TYPE_ASSERT_CVQ(bsl::is_integral,
                        value,
                        int TestType::*,
                        false);

        // C-9
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, wchar_t, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char8_t, true);
#endif
#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char16_t, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char32_t, true);
#endif

        TYPE_ASSERT_CVQ(bsl::is_integral, value, signed char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long int, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, long long, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long long, true);

        // C-10
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bsls::Types::Int64, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bsls::Types::Uint64, true);

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
