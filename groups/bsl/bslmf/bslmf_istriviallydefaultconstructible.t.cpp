// bslmf_istriviallydefaultconstructible.t.cpp                        -*-C++-*-

#include <bslmf_istriviallydefaultconstructible.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function,
// 'bsl::is_trivially_default_constructible', that determines whether a
// template parameter type is trivially default-constructible.  The
// meta-function by default support a set of type categories and can be
// extended to support other type through either template specialization or a
// macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by the meta-function by testing the meta-function with each
// possible supported category of types.  We also need to verify that the
// meta-function can be correctly extended to support other types through
// either of the 2 supported mechanisms.
//
// ----------------------------------------------------------------------------
// class methods
// [ 2] bsl::is_trivially_default_constructible
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

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
    // This user-defined type that is marked to be trivially
    // default-constructible using specialization is used for testing.
};

struct UserDefinedTdcTestType2 {
    // This user-defined type that is marked to be trivially
    // default-constructible using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro
    // is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedTdcTestType2,
                                   bsl::is_trivially_default_constructible);
};

struct UserDefinedNonTdcTestType {
    // This user-defined type that is not marked to be trivially
    // default-constructible is used for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

typedef int * PointerTestType;
    // This pointer type is used for testing.

typedef int& ReferenceTestType;
    // This reference type is used for testing.

typedef int (UserDefinedNonTdcTestType::*MethodPtrTestType) ();
    // This non-static function member type is used for testing.

}  // close unnamed namespace


namespace bsl {

template <>
struct is_trivially_default_constructible<
                                     UserDefinedTdcTestType> : bsl::true_type {
};

} // close namespace bsl

int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3;
    int veryVeryVerbose     = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
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
// First, we define a set of trivially default-constructible types:
//..
          typedef int MyFundamentalType;
          typedef int& MyFundamentalTypeReference;

//..
// Then, since user-defined types can not be automatically determined by the
// 'is_trivially_default_constructible', we define a template specialization to
// specify that 'MyTriviallyDefaultConstructibleType' is trivially
// default-constructible:
//..
//..
// Now, we verify whether each type is default-constructible using
// 'bsl::is_trivially_default_constructible':
//..
          ASSERT(true ==
            bsl::is_trivially_default_constructible<MyFundamentalType>::value);
          ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                     MyFundamentalTypeReference>::value);
          ASSERT(true ==
                 bsl::is_trivially_default_constructible<
                     MyTriviallyDefaultConstructibleType>::value);
          ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                     MyNonTriviallyDefaultConstructibleType>::value);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_default_constructible' manual
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for types explicitly specified to be
        //   trivially default-constructible.
        //
        // Concerns:
        //: 1 The meta-function returns false for normal user-defined types.
        //:
        //: 2 The meta-function returns true for a user-defined type, if a
        //:   specialization for 'bsl::is_trivially_default_constructible' on
        //:   that type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns true for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_default_constructible' returns
        //   the correct value for each type listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_default_constructible
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_default_constructible' manual\n"
                   "\n================================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_default_constructible<
                                            UserDefinedNonTdcTestType>::value);

        // C-2
        ASSERT(bsl::is_trivially_default_constructible<
                                               UserDefinedTdcTestType>::value);

        // C-3
        ASSERT(bsl::is_trivially_default_constructible<
                                              UserDefinedTdcTestType2>::value);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_default_constructible' intrinsic
        //   Ensure the 'bsl::is_trivially_default_constructible' meta-function
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //: 1 The meta-function returns false for reference types.
        //:
        //: 2 The meta-function returns true for fundamental types.
        //:
        //: 3 The meta-function returns true for enum types.
        //:
        //: 4 The meta-function returns true for pointer types.
        //:
        //: 5 The meta-function returns true for pointer to member types.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_default_constructible' returns
        //   the correct value for each type listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_default_constructible
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_default_constructible' intrinsic\n"
                   "\n===================================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_default_constructible<
                                                    ReferenceTestType>::value);

        // C-2
        ASSERT(bsl::is_trivially_default_constructible<int>::value);
        ASSERT(bsl::is_trivially_default_constructible<char>::value);
        ASSERT(bsl::is_trivially_default_constructible<void>::value);

        // C-3
        ASSERT(bsl::is_trivially_default_constructible<EnumTestType>::value);

        // C-4
        ASSERT(bsl::is_trivially_default_constructible<
                                                      PointerTestType>::value);

        // C-5
        ASSERT(bsl::is_trivially_default_constructible<
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
