// bslmf_conditional.t.cpp                                            -*-C++-*-
#include <bslmf_conditional.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

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
// transforms to one of the two template parameter types based on its 'bool'
// template parameter value.  Thus we need to ensure that the value returned by
// this meta-functions is correct for each possible pair of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
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
///Example 1: Conditional Type Transformation Based on Boolean Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types, 'int' and 'char', based on
// 'bool' value.  If the 'bool' value is 'true', the 'int' type is returned;
// otherwise the 'char' type is returned.
//
// Now, we instantiate the 'bsl::conditional' template using 'int', 'char', and
// each of the two 'bool' values.  We use the 'bsl::is_same' meta-function to
// assert the 'type' static data member of each instantiation:
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
        //   Ensure that the static data member 'type' of 'bsl::conditional'
        //   instantiations has the correct value.
        //
        // Concerns:
        //: 1 'conditional::type' correctly transforms to one of the two
        //:   template parameter types when a 'true' or 'false' is set as its
        //:   'bool' template parameter type value.
        //
        // Plan:
        //   Instantiate 'bsl::conditional' with various types and verify that
        //   the 'type' member is initialized properly.  (C-1)
        //
        // Testing:
        //   bsl::conditional::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::conditional::type\n"
                            "======================\n");
        // C-1

        ASSERT_SAME_CV(   int ,  char);
        ASSERT_SAME_CV(  void*,  Enum);
        ASSERT_SAME_CV(  Enum&, Class);
        ASSERT_SAME_CV( Class*, Union);
        ASSERT_SAME_CV(     F ,    RF);
        ASSERT_SAME_CV(    RF ,    PF);
        ASSERT_SAME_CV(    PF ,   RPF);
        ASSERT_SAME_CV(   RPF ,    Fi);
        ASSERT_SAME_CV(    Fi ,   RFi);
        ASSERT_SAME_CV(   RFi ,   FRi);
        ASSERT_SAME_CV(   FRi ,  RFRi);
        ASSERT_SAME_CV(  RFRi ,     A);
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
