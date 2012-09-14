// bslmf_istriviallydefaultconstructible.t.cpp                        -*-C++-*-

#include <bslmf_istriviallydefaultconstructible.h>

#include <new>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;
using namespace std;

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

namespace {

class MyTriviallyDefaultConstructibleType {};
class MyTriviallyDefaultConstructibleType2 {};

} // close unnamed namespace

namespace bsl {

template <>
struct is_trivially_default_constructible<MyTriviallyDefaultConstructibleType2>
    : bsl::true_type {
    // This template specialization for 'is_trivially_default_constructible'
    // indicates that 'MyTriviallyDefaultConstructibleType2' is a
    // trivially-default-constructible type.
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
      case 2: {
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify A Supported Trivially-Default-Constructible Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert wheter a type is
// trivially-default-constructible.
//
// First, we define a set of types to be evaluated:
//..
          typedef int MyFundamentalType;
          typedef int& MyFundamentalTypeReference;
// class MyTriviallyDefaultConstructibleType {};
//..
// Now, we verify whether each type is default-constructible using
// 'bsl::is_trivially_default_constructible':
//..
          ASSERT(true ==
                 bsl::is_trivially_default_constructible<
                                                    MyFundamentalType>::value);
          ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                                           MyFundamentalTypeReference>::value);
          ASSERT(false ==
                 bsl::is_trivially_default_constructible<
                                  MyTriviallyDefaultConstructibleType>::value);
//..
// Notice that 'bsl::is_trivially_default_constructible' incorrectly found
// 'MyTriviallyDefaultConstructibleType' to be
// non-trivially-default-constructible, because the type can not be
// automatically evaluated by the meta-function and must be explicitly
// specified as such a type (See Example 2).
//
///Example 2: Explicitly Specify The Value For A User-Defined Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to explicitly specify that a user-defined type is
// trivially-default-constructible.
//
// First, we define a trivially-default-constructible user-defined type,
// 'MyTriviallyDefaultConstructibleType2':
//..
//  class MyTriviallyDefaultConstructibleType2 {};
//..
// Now, we define template specialization for
// 'bsl::is_trivially_default_constructible' to specify that
// 'MyTriviallyDefaultConstructibleType2' is trivially-default-constructible:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_default_constructible<
//                     MyTriviallyDefaultConstructibleType2> : bsl::true_type {
//      // This template specialization for
//      // 'is_trivially_default_constructible' indicates that
//      // 'MyTriviallyDefaultConstructibleType2' is a
//      // trivially-default-constructible type.
//  };
//
//  }  // close namespace bsl
//..
// Finally, we verify that 'bsl::is_trivially_default_constructible' correctly
// evaluate the value of 'MyTriviallyDefaultConstructibleType2':
//..
          ASSERT(true ==
                 bsl::is_trivially_default_constructible<
                                 MyTriviallyDefaultConstructibleType2>::value);
//..
      } break;
      case 1: {
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
