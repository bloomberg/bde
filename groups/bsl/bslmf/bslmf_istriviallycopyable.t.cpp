// bslmf_istriviallycopyable.t.cpp                                    -*-C++-*-

#include "bslmf_istriviallycopyable.h"

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function, 'bsl::is_trivially_copyable', that
// determines whether a template parameter type is trivially copyable.  The
// meta-function by defualt support a set of type categories and can be
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
// [ 2] bsl::is_trivially_copyable
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

class MyTriviallyCopyableType {
};

struct MyNonTriviallyCopyableType {
    //...
};

}  // close unnamed namespace

namespace bsl {

template <>
struct is_trivially_copyable<MyTriviallyCopyableType> : bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates
    // that 'MyTriviallyCopyableType' is a trivially copyable.
};

}  // close namespace bsl

namespace {

struct UserDefinedTcTestType {
    // This user-defined type that is marked to be trivially copyable using
    // specialization is used for testing.
};

struct UserDefinedTcTestType2 {
    // This user-defined type that is marked to be trivially copyable using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedTcTestType2,
                                   bsl::is_trivially_copyable);
};

struct UserDefinedNonTcTestType {
    // This user-defined type that is not marked to be trivially copyable is
    // used for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

typedef int * PointerTestType;
    // This pointer type is used for testing.

typedef int& ReferenceTestType;
    // This reference type is used for testing.

typedef int (UserDefinedNonTcTestType::*MethodPtrTestType) ();
    // This non-static function member type is used for testing.

}  // close unnamed namespace


namespace bsl {

template <>
struct is_trivially_copyable<UserDefinedTcTestType> : bsl::true_type {
};

}  // close namespace bsl


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
///Example 1: Verify Whether Types are Trivially Copyable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially copyable.
//
// First, we define a set types to evaluate:
//..
          typedef int MyFundamentalType;
          typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyCopyableType {
//  };
//
//  struct MyNonTriviallyCopyableType {
//      //...
//  };
//..
// Then, since user-defined types can not be automatically evaluated by
// 'is_trivially_copyable', we define a template specialization to specify that
// 'MyTriviallyCopyableType' is trivially copyable:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_copyable<MyTriviallyCopyableType> : bsl::true_type {
//      // This template specialization for 'is_trivially_copyable' indicates
//      // that 'MyTriviallyCopyableType' is a trivially copyable.
//  };
//
//  }  // close namespace bsl
//..
// Now, we verify whether each type is trivially copyable using
// 'bsl::is_trivially_copyable':
//..
          ASSERT(true == bsl::is_trivially_copyable<MyFundamentalType>::value);
          ASSERT(false == bsl::is_trivially_copyable<
                                           MyFundamentalTypeReference>::value);
          ASSERT(true == bsl::is_trivially_copyable<
                                              MyTriviallyCopyableType>::value);
          ASSERT(false == bsl::is_trivially_copyable<
                                           MyNonTriviallyCopyableType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_copyable' manual
        //   Ensure the 'bsl::is_trivially_copyable' meta-function returns the
        //   correct value for types explicitly specified to be trivially
        //   copyable.
        //
        // Concerns:
        //: 1 The meta-function returns false for normal user-defined types.
        //:
        //: 2 The meta-function returns true for a user-defined type, if a
        //:   specialization for 'bsl::is_trivially_copyable' on that type is
        //:   defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns true for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_copyable' returns the correct value
        //   for each type listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_copyable
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_copyable' manual\n"
                   "\n===================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_copyable<UserDefinedNonTcTestType>::value);

        // C-2
        ASSERT(bsl::is_trivially_copyable<UserDefinedTcTestType>::value);

        // C-3
        ASSERT(bsl::is_trivially_copyable<UserDefinedTcTestType2>::value);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_copyable' intrinsic
        //   Ensure the 'bsl::is_trivially_copyable' meta-function
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
        //   Verify that 'bsl::is_trivially_copyable' returns the correct
        //   value for each type listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_copyable
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_copyable' intrinsic\n"
                   "\n======================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_copyable<ReferenceTestType>::value);

        // C-2
        ASSERT(bsl::is_trivially_copyable<int>::value);
        ASSERT(bsl::is_trivially_copyable<char>::value);
        // ASSERT(bsl::is_trivially_copyable<void>::value);

        // C-3
        ASSERT(bsl::is_trivially_copyable<EnumTestType>::value);

        // C-4
        ASSERT(bsl::is_trivially_copyable<PointerTestType>::value);

        // C-5
        ASSERT(bsl::is_trivially_copyable<MethodPtrTestType>::value);

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
