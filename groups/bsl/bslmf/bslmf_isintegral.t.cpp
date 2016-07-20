// bslmf_isintegral.t.cpp                                             -*-C++-*-
#include <bslmf_isintegral.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test implements a meta-function, 'bsl::is_integral',
// that determines whether a template parameter type is an integral type.
// Thus, we need to ensure that the value returned by the meta-function is
// correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_integral::value
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

namespace {

struct TestType {
    // This user-defined type is for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
    ENUM_TEST_VALUE0 = 0,
    ENUM_TEST_VALUE1
};

typedef int (*RetIntegralFunctionPtrType) ();
    // This pointer to function type that returns an integral type is
    // used for testing.

}  // close unnamed namespace

#define TYPE_ASSERT_CVQ(metaFunc, member, type, result)                       \
    ASSERT(result == metaFunc<type>::member);                                 \
    ASSERT(result == metaFunc<const type>::member);                           \
    ASSERT(result == metaFunc<volatile type>::member);                        \
    ASSERT(result == metaFunc<const volatile type>::member);

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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_integral::value'
        //   Ensure that 'bsl::is_integral' returns the correct values for a
        //   variety of template parameter types.
        //
        // Concerns:
        //: 1 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) non-integral primitive type.
        //
        //: 2 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_integral::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) enum type.
        //:
        //: 4 'is_integral::value' is 'false' when 'TYPE' is a pointer to
        //:   (possibly cv-qualified) integral type.
        //:
        //: 5 'is_integral::value' is 'false' when 'TYPE' is a reference to
        //:   (possibly cv-qualified) integral type.
        //:
        //: 6 'is_integral::value' is 'false' when 'TYPE' is a function type
        //:   returning an integral type.
        //:
        //: 7 'is_integral::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) integral type.
        //:
        //
        // Plan:
        //   Verify that 'bsl::is_integral::value' has the correct value for
        //   each concern.
        //
        // Testing:
        //   bsl::is_integral::value
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::is_integral::value'\n"
                            "\n=========================\n");

        // C-1
        TYPE_ASSERT_CVQ(bsl::is_integral, value, double, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long double, false);

        // C-2
        TYPE_ASSERT_CVQ(bsl::is_integral, value, TestType, false);

        // C-3
        TYPE_ASSERT_CVQ(bsl::is_integral, value, EnumTestType, false);

        // C-4
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool *, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int *, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int *, false);

        // C-5
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool&, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int&, false);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int&, false);

        // C-6
        TYPE_ASSERT_CVQ(bsl::is_integral,
                        value,
                        RetIntegralFunctionPtrType,
                        false);

        // C-7
        TYPE_ASSERT_CVQ(bsl::is_integral, value, bool, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, wchar_t, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, signed char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, long int, true);

        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned char, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned short, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned int, true);
        TYPE_ASSERT_CVQ(bsl::is_integral, value, unsigned long int, true);

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
