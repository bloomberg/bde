// bslmf_istriviallydefaultconstructible.t.cpp                        -*-C++-*-
#include <bslmf_istriviallydefaultconstructible.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addvolatile.h>
#include <bslmf_nestedtraitdeclaration.h>

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
// [ 2] EXTENDING 'bsl::is_trivially_default_constructible'

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

// Each of the macros below will test the
// 'bsl::is_trivially_default_constructible' trait with a set of variations on
// a type.  There are several layers of macros, as object types support the
// full range of variation, but function types cannot form an array, nor be
// cv-qualified.  Similarly, 'void' may be cv-qualified but still cannot form
// an array.  As macros are strictly text-substitution we must use the
// appropriate 'add_decoration' traits to transform types in a manner that is
// guaranteed to be syntactically valid.  Note that these are not
// type-dependent contexts, so there is no need to use 'typename' when fetching
// the result from any of the queried traits.

#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE(TYPE, RESULT)          \
    ASSERT( bsl::is_trivially_default_constructible<TYPE>::value == RESULT);  \
    ASSERT( bsl::is_trivially_default_constructible<                          \
                                       bsl::add_pointer<TYPE>::type>::value); \
    ASSERT(!bsl::is_trivially_default_constructible<                          \
                                bsl::add_lvalue_reference<TYPE>::type>::value);

#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)       \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE(TYPE, RESULT);             \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE(                           \
                                      bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE(                           \
                                      bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE(                           \
                                      bsl::add_cv<TYPE>::type, RESULT);

// Two additional macros will allow testing on old MSVC compilers when 'TYPE'
// is an array of unknown bound.

#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT)   \
    ASSERT( bsl::is_trivially_default_constructible<TYPE>::value == RESULT);  \
    ASSERT( bsl::is_trivially_default_constructible<                          \
                                       bsl::add_pointer<TYPE>::type>::value);

#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE, RESULT)\
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE_NO_REF(TYPE, RESULT);      \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE_NO_REF(                    \
                                      bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE_NO_REF(                    \
                                      bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_TYPE_NO_REF(                    \
                                      bsl::add_cv<TYPE>::type, RESULT);


#if defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.

#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)   \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)           \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)      \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)

#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
// Old microsoft compilers compilers do not support references to arrays of
// unknown bound.

# define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)  \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)           \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)      \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)    \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[], RESULT)  \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE_NO_REF(TYPE[][8], RESULT)

#else
#define ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(TYPE, RESULT)   \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE, RESULT)           \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[128], RESULT)      \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[12][8], RESULT)    \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[], RESULT)         \
    ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(TYPE[][8], RESULT)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class MyTriviallyDefaultConstructibleType {
};

struct MyNonTriviallyDefaultConstructibleType {

    int d_data;

    MyNonTriviallyDefaultConstructibleType()
        // Create a 'MyNonTriviallyDefaultConstructibleType' object.  This user
        // defined constructor ensures that this class is not trivially default
        // constructible, while it is default constructible.
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
        // EXTENDING 'bsl::is_trivially_default_constructible'
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for types explicitly specified to be
        //   trivially default constructible.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for normal user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bsl::is_trivially_default_constructible' on
        //:   that type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 For cv-qualified types, the meta-function returns 'true' if the
        //:   corresponding cv-unqualified type is trivially default
        //:   constructible, and 'false' otherwise.
        //:
        //: 5 For array types, the meta-function returns 'true' if the array
        //:   element is trivially default constructible, and 'false'
        //:   otherwise.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C4,5)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-3, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   EXTENDING 'bsl::is_trivially_default_constructible'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nEXTENDING 'bsl::is_trivially_default_constructible'"
                   "\n===================================================\n");

        // C-1
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(
                                                     UserDefinedNonTdcTestType,
                                                     false);

        // C-2
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(
                                                        UserDefinedTdcTestType,
                                                        true);

        // C-3
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(
                                                       UserDefinedTdcTestType2,
                                                       true);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_default_constructible::value'
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The meta-function returns 'true' for fundamental types.
        //:
        //:  2 The meta-function returns 'true' for enumerated types.
        //:
        //:  3 The meta-function returns 'true' for pointer to member types.
        //:
        //:  4 The meta-function returns 'false' for cv-qualified 'void' types.
        //:
        //:  5 The meta-function returns 'false' for function types.
        //
        //:  6 The meta-function returns 'true' for pointer types.
        //:
        //:  7 The meta-function returns 'false' for reference types.
        //:
        //:  8 The meta-function returns the same result for array types as it
        //:    would for the array's element type.
        //:
        //:  9 The meta-function returns the same result for cv-qualified
        //:    types that it would return 'true' for the corresponding
        //:    cv-unqualified type.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C6-9)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-5, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   bsl::is_trivially_default_constructible::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_default_constructible::value'"
                   "\n================================================\n");

        // C-1
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(int, true);
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(char, true);
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(double, true);

        // C-2
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(EnumTestType,
                                                              true);

        // C-3
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_OBJECT_TYPE(
                                                             MethodPtrTestType,
                                                             true);

        // C-4 : 'void' is not an object type, but can be cv-qualified.
        ASSERT_IS_TRIVIALLY_DEFAULT_CONSTRUCTIBLE_CV_TYPE(void, false);

        // C-5 : Function types are not object types, nor cv-qualifiable.
        // Note that this particular test stresses compilers handling of
        // function types, and function reference types, in the template type
        // system.  We incrementally disable tests for compilers known to have
        // bugs that we cannot easily work around/
        ASSERT( bsl::is_trivially_default_constructible<void(*)()>::value);
        ASSERT( bsl::is_trivially_default_constructible<
                                             int(*)(float, double...)>::value);
#if !defined(BSLS_PLATFORM_CMP_SUN) // last tested for v12.3
        ASSERT(!bsl::is_trivially_default_constructible<void()>::value);
        ASSERT(!bsl::is_trivially_default_constructible<
                                                int(float, double...)>::value);
#if !defined(BSLS_PLATFORM_CMP_IBM) // last tested for v12.1
        ASSERT(!bsl::is_trivially_default_constructible<void(&)()>::value);
        ASSERT(!bsl::is_trivially_default_constructible<
                                             int(&)(float, double...)>::value);
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
