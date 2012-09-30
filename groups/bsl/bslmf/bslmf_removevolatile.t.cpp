// bslmf_removevolatile.t.cpp                                         -*-C++-*-
#include <bslmf_removevolatile.h>

#include <bslmf_issame.h>

#include <cstdlib>
#include <cstdio>

using namespace bsl;
using namespace BloombergLP;

using std::atoi;
using std::printf;
using std::fprintf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-functions, 'bsl::remove_volatile', that
// removes the top-level 'volatile'-qualifier from a template parameter type.
// Thus, we need to ensure that the values returned by the meta-function is
// correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_volatile
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::remove_volatile'.
};

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

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
///Example 1: Removing The Volatile-Qualifier of A Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to strip the 'volatile'-qualifier from a particular
// type..
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
        typedef int       MyType;
        typedef volatile int MyVolatileType;
//..
// Now, we strip the the 'volatile'-qualifier from 'MyVolatileType' using
// 'bsl::remove_volatile' and verify that the resulting type is the same as
// 'MyType'.
//..
        ASSERT(true == (bsl::is_same<
                  bsl::remove_volatile<MyVolatileType>::type, MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_volatile'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_volatile'
        //   instantiations has the same type as the template parameter type
        //   except has any top-level 'volatile'-qualifier removed.
        //
        // Concerns:
        //: 1 'bsl::remove_volatile::type' is 'T' when 'TYPE' is 'T'.
        //:
        //: 2 'bsl::remove_volatile::type' is 'T' when 'TYPE' is 'T volatile'.
        //:
        //: 3 'bsl::remove_volatile::type' is 'T const' when 'TYPE' is
        //:   'T const volatile'.
        //:
        //: 4 'bsl::remove_volatile::type' is 'T volatile *' when 'TYPE' is
        //:   'T volatile *'.
        //
        // Plan:
        //   Verify that 'bsl::remove_volatile::type' has the correct type for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::remove_volatile
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_volatile'\n"
                            "\n======================\n");

        // C-1
        ASSERT((is_same<remove_volatile<int>::type, int>::value));
        ASSERT((is_same<remove_volatile<int *>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType>::type, TestType>::value));

        // C-2
        ASSERT((is_same<remove_volatile<int volatile>::type, int>::value));
        ASSERT((is_same<remove_volatile<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_volatile<TestType volatile>::type,
                                                            TestType>::value));

        // C-3
        ASSERT((is_same<remove_volatile<int const volatile>::type,
                                                           int const>::value));
        ASSERT((is_same<remove_volatile<int * const volatile>::type,
                                                         int * const>::value));
        ASSERT((is_same<remove_volatile<TestType const volatile>::type,
                                                      TestType const>::value));

        // C-4
        ASSERT((is_same<remove_volatile<int volatile *>::type,
                                                      int volatile *>::value));

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
