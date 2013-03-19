// bslmf_conditional.t.cpp                                            -*-C++-*-
#include <bslmf_conditional.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <cstdio>     // 'printf'
#include <cstdlib>    // 'atoi'

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::conditional', that
// conditionally selects to one of its two template parameter types based on a
// 'bool' (template parameter) value.  Thus, we need to ensure that the values
// returned by this meta-function are correct for each possible set of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bsl::conditional::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef int INT;

typedef void        F ();
typedef void (  &  RF)();
typedef void (*    PF)();
typedef void (*&  RPF)();

typedef void      Fi  (int);
typedef void ( & RFi )(int);
typedef void      FRi (int&);
typedef void ( & RFRi)(int&);

typedef char      A [5];
typedef char ( & RA)[5];

#define ASSERT_SAME_CV2(TYPE1, TYPE2)                                         \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
            TYPE1>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          const TYPE2>::type,                 \
            TYPE1>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          volatile TYPE2>::type,              \
            TYPE1>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          const volatile TYPE2>::type,        \
            TYPE1>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
            TYPE2>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          const TYPE2>::type,                 \
            const TYPE2>::value));                                            \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          volatile TYPE2>::type,              \
            volatile TYPE2>::value));                                         \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          const volatile TYPE2>::type,        \
            const volatile TYPE2>::value));

#define ASSERT_SAME_CV(TYPE1, TYPE2)                                          \
    ASSERT_SAME_CV2(TYPE1, TYPE2)                                             \
    ASSERT_SAME_CV2(const TYPE1, TYPE2)                                       \
    ASSERT_SAME_CV2(volatile TYPE1, TYPE2)                                    \
    ASSERT_SAME_CV2(const volatile TYPE1, TYPE2)

#define ASSERT_SAME_FN_TYPE(TYPE1, TYPE2)                                     \
    ASSERT((bsl::is_same<bsl::conditional<true,                               \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
            TYPE1>::value));                                                  \
    ASSERT((bsl::is_same<bsl::conditional<false,                              \
                                          TYPE1,                              \
                                          TYPE2>::type,                       \
            TYPE2>::value));                                                  \

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    //int veryVerbose = argc > 3;

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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditionally Select From Two Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types based on a 'bool' value.
//
// Now, we use 'bsl::conditional' to select between two types, 'int' and
// 'char', with a 'bool' value.  When the 'bool' is 'true', we select 'int';
// otherwise, we select 'char'.  We verify that our code behaves correctly by
// asserting the result of 'bsl::conditional' with the expected type using
// 'bsl::is_same':
//..
    ASSERT(true ==
        (bsl::is_same<bsl::conditional<true,  int, char>::type, int >::value));
    ASSERT(true ==
        (bsl::is_same<bsl::conditional<false, int, char>::type, char>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::conditional::type'
        //   Ensure that each 'typedef' 'type' of 'bsl::conditional'
        //   instantiations has the correct return value.
        //
        // Concerns:
        //: 1 'bsl::conditional' selects the first of its two (template
        //:   parameter) types when the (template parameter) value 'COND' is
        //:   'true'.
        //:
        //: 2 'bsl::conditional' selects the second of its two (template
        //:   parameter) types when the (template parameter) value 'COND' is
        //:   'false'.
        //
        // Plan:
        //   Instantiate 'bsl::conditional' with various types and verify that
        //   the 'type' member is initialized properly.  (C-1..2)
        //
        // Testing:
        //   bsl::conditional::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::conditional::type\n"
                            "======================\n");

        ASSERT_SAME_CV(   int ,  char);
        ASSERT_SAME_CV(  void*,  Enum);
        ASSERT_SAME_CV(  Enum&, Class);
        ASSERT_SAME_CV( Class*, Union);

        ASSERT_SAME_FN_TYPE(     F ,    RF);
        ASSERT_SAME_FN_TYPE(    RF ,    PF);
        ASSERT_SAME_FN_TYPE(    PF ,   RPF);
        ASSERT_SAME_FN_TYPE(   RPF ,    Fi);
        ASSERT_SAME_FN_TYPE(    Fi ,   RFi);
        ASSERT_SAME_FN_TYPE(   RFi ,   FRi);
        ASSERT_SAME_FN_TYPE(   FRi ,  RFRi);
        ASSERT_SAME_FN_TYPE(  RFRi ,     A);

        ASSERT_SAME_CV(     A ,    RA);
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
// Copyright (C) 2013 Bloomberg L.P.
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
