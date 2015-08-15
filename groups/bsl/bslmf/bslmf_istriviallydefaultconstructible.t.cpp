// bslmf_istriviallydefaultconstructible.t.cpp                        -*-C++-*-
#include <bslmf_istriviallydefaultconstructible.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bsl::is_trivially_default_constructible', that determines whether a
// template parameter type is trivially default-constructible.  By default, the
// meta-function supports a restricted set of type categories, but can be
// extended to support other types through either template specialization or
// use of the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively-supported types are correctly
// identified by the meta-function by testing the meta-function with each of
// the type categories.  We also need to verify that the meta-function can be
// correctly extended to support other types through either of the two
// supported mechanisms.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_trivially_default_constructible::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] EXTENDING bsl::is_trivially_default_constructible

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class MyTriviallyDefaultConstructibleType {
};

struct MyNonTriviallyDefaultConstructibleType {

    int d_data;

    MyNonTriviallyDefaultConstructibleType()
    : d_data(1)
    {
    }
};

}  // close unnamed namespace

namespace bsl {

template <>
struct is_trivially_default_constructible<
    MyTriviallyDefaultConstructibleType> : bsl::true_type {
        // This template specialization for
        // 'is_trivially_default_constructible' indicates that
        // 'MyTriviallyDefaultConstructibleType' is a trivially
        // default-constructible type.
};

}  // close namespace bsl

namespace {

struct UserDefinedTdcTestType {
    // This user-defined type, which is marked to be trivially
    // default-constructible using template specialization (below), is used for
    // testing.
};

struct UserDefinedTdcTestType2 {
    // This user-defined type, which is marked to be trivially
    // default-constructible using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro,
    // is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedTdcTestType2,
                                   bsl::is_trivially_default_constructible);
};

struct UserDefinedNonTdcTestType {
    // This user-defined type, which is not marked to be trivially
    // default-constructible, is used for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

typedef int *PointerTestType;
    // This pointer type is used for testing.

typedef int& ReferenceTestType;
    // This reference type is used for testing.

typedef int (UserDefinedNonTdcTestType::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

}  // close unnamed namespace

namespace bsl {

template <>
struct is_trivially_default_constructible<
                                     UserDefinedTdcTestType> : bsl::true_type {
};

}  // close namespace bsl

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3;
    int veryVeryVerbose     = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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
///Example 1: Verify Whether Types are Trivially Default-Constructible
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially
// default-constructible.
//
// First, we define a set of types to evaluate:
//..
    typedef int MyFundamentalType;
    typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyDefaultConstructibleType {
//  };
//
//  struct MyNonTriviallyDefaultConstructibleType {
//
//      int d_data;
//
//      MyNonTriviallyDefaultConstructibleType()
//      : d_data(1)
//      {
//      }
//  };
//..
// Then, since user-defined types cannot be automatically evaluated by
// 'is_trivially_default_constructible', we define a template specialization to
// specify that 'MyTriviallyDefaultConstructibleType' is trivially
// default-constructible:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_default_constructible<
//                      MyTriviallyDefaultConstructibleType> : bsl::true_type {
//      // This template specialization for
//      // 'is_trivially_default_constructible' indicates that
//      // 'MyTriviallyDefaultConstructibleType' is a trivially
//      // default-constructible type.
//  };
//
//  }  // close namespace bsl
//..
// Now, we verify whether each type is trivially default-constructible using
// 'bsl::is_trivially_default_constructible':
//..
    ASSERT(true  ==
            bsl::is_trivially_default_constructible<MyFundamentalType>::value);
    ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                     MyFundamentalTypeReference>::value);
    ASSERT(true  ==
                 bsl::is_trivially_default_constructible<
                     MyTriviallyDefaultConstructibleType>::value);
    ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                     MyNonTriviallyDefaultConstructibleType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Extending 'bsl::is_trivially_default_constructible'
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for types explicitly specified to be
        //   trivially default-constructible.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for general user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type if a
        //:   specialization for 'bsl::is_trivially_default_constructible' on
        //:   that type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_default_constructible' returns
        //   the correct value for each type listed in the concerns.
        //
        // Testing:
        //   Extending bsl::is_trivially_default_constructible
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nExtending 'bsl::is_trivially_default_constructible'\n"
                   "\n===================================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_default_constructible<
                                            UserDefinedNonTdcTestType>::value);

        // C-2
        ASSERT( bsl::is_trivially_default_constructible<
                                               UserDefinedTdcTestType>::value);

        // C-3
        ASSERT( bsl::is_trivially_default_constructible<
                                              UserDefinedTdcTestType2>::value);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_default_constructible::value'
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for reference types.
        //:
        //: 2 The meta-function returns 'true' for fundamental types.
        //:
        //: 3 The meta-function returns 'true' for enum types.
        //:
        //: 4 The meta-function returns 'true' for pointer types.
        //:
        //: 5 The meta-function returns 'true' for pointer to member types.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_default_constructible' returns the
        //   correct value for each type category listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_default_constructible::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_default_constructible::value'\n"
                   "\n================================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_default_constructible<
                                                    ReferenceTestType>::value);

        // C-2
        ASSERT( bsl::is_trivially_default_constructible<int>::value);
        ASSERT( bsl::is_trivially_default_constructible<char>::value);
        ASSERT( bsl::is_trivially_default_constructible<void>::value);

        // C-3
        ASSERT( bsl::is_trivially_default_constructible<EnumTestType>::value);

        // C-4
        ASSERT( bsl::is_trivially_default_constructible<
                                                      PointerTestType>::value);

        // C-5
        ASSERT( bsl::is_trivially_default_constructible<
                                                    MethodPtrTestType>::value);

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
