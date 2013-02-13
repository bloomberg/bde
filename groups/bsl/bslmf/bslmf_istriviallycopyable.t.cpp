// bslmf_istriviallycopyable.t.cpp                                    -*-C++-*-

#include "bslmf_istriviallycopyable.h"

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;

using std::printf;
using std::fprintf;
using std::atoi;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bsl::is_trivially_copyable', that determines whether a template parameter
// type is trivially copyable.  By defualt, the meta-function supports a
// restricted set of type categories and can be extended to support other types
// through either template specialization or use of the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by the meta-function by testing the meta-function with each of
// the supported type categories.  We also need to verify that the
// meta-function can be correctly extended to support other types through
// either of the two supported mechanisms.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_trivially_copyable::value
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] EXTENDING bsl::is_trivially_copyable::value

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
    // This user-defined type, which is marked to be trivially copyable using
    // template specialization (below), is used for testing.
};

struct UserDefinedTcTestType2 {
    // This user-defined type, which is marked to be trivially copyable using
    // the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedTcTestType2,
                                   bsl::is_trivially_copyable);
};

struct UserDefinedNonTcTestType {
    // This user-defined type, which is not marked to be trivially copyable, is
    // used for testing.
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

typedef int *PointerTestType;
    // This pointer type is used for testing.

typedef int& ReferenceTestType;
    // This reference type is used for testing.

typedef int (UserDefinedNonTcTestType::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

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
///Example 1: Verify Whether Types are Trivially Copyable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially copyable.
//
// First, we define a set of types to evaluate:
//..
        typedef int  MyFundamentalType;
        typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyCopyableType {
//  };
//
//  struct MyNonTriviallyCopyableType {
//      //...
//  };
//..
// Then, since user-defined types cannot be automatically evaluated by
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
        ASSERT(true  == bsl::is_trivially_copyable<MyFundamentalType>::value);
        ASSERT(false == bsl::is_trivially_copyable<
                                           MyFundamentalTypeReference>::value);
        ASSERT(true  == bsl::is_trivially_copyable<
                                              MyTriviallyCopyableType>::value);
        ASSERT(false == bsl::is_trivially_copyable<
                                           MyNonTriviallyCopyableType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Extending 'bsl::is_trivially_copyable'
        //   Ensure the 'bsl::is_trivially_copyable' meta-function returns the
        //   correct value for types explicitly specified to be trivially
        //   copyable.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for normal user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bsl::is_trivially_copyable' on that type is
        //:   defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //
        // Plan:
        //   Verify that 'bsl::is_trivially_copyable' returns the correct value
        //   for each type listed in the concerns.
        //
        // Testing:
        //   Extending bsl::is_trivially_copyable
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nExtending 'bsl::is_trivially_copyable'\n"
                   "\n======================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_copyable<UserDefinedNonTcTestType>::value);

        // C-2
        ASSERT( bsl::is_trivially_copyable<UserDefinedTcTestType>::value);

        // C-3
        ASSERT( bsl::is_trivially_copyable<UserDefinedTcTestType2>::value);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_trivially_copyable::value'
        //   Ensure the 'bsl::is_trivially_copyable' meta-function
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
        //   Verify that 'bsl::is_trivially_copyable' returns the correct
        //   value for each type listed in the concerns.
        //
        // Testing:
        //   bsl::is_trivially_copyable::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bsl::is_trivially_copyable::value'\n"
                   "\n===================================\n");

        // C-1
        ASSERT(!bsl::is_trivially_copyable<ReferenceTestType>::value);

        // C-2
        ASSERT( bsl::is_trivially_copyable<int>::value);
        ASSERT( bsl::is_trivially_copyable<char>::value);
        ASSERT( bsl::is_trivially_copyable<void>::value);

        // C-3
        ASSERT( bsl::is_trivially_copyable<EnumTestType>::value);

        // C-4
        ASSERT( bsl::is_trivially_copyable<PointerTestType>::value);

        // C-5
        ASSERT( bsl::is_trivially_copyable<MethodPtrTestType>::value);

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
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
