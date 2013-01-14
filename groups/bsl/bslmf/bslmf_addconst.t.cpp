// bslmf_addconst.t.cpp                                               -*-C++-*-
#include <bslmf_addconst.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <cstdio>
#include <cstdlib>

using namespace bsl;
using namespace BloombergLP;

using std::atoi;
using std::fprintf;
using std::printf;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-functions, 'bsl::add_const', that
// adds a top-level 'const'-qualifier to a template parameter type.  Thus, we
// need to ensure that the values returned by the meta-function are correct for
// each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bsl::add_const::type
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
   // argument for the template parameter 'TYPE' of 'bsl::add_const'.
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

    (void)veryVerbose;

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
///Example 1: Adding the 'const'-qualifier to A Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
        typedef int       MyType;
        typedef const int MyConstType;
//..
// Now, we add a 'const'-qualifier to 'MyType' using 'bsl::add_const' and
// verify that the resulting type is the same as 'MyConstType':
//..
        ASSERT(true ==
               (bsl::is_same<bsl::add_const<MyType>::type, MyConstType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_const::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::add_const' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::add_const' adds a top-level 'const'-qualifier only to
        //:   primitive types, pointer types, and user-defined types.
        //:
        //: 2 'bsl::add_const' does not add a 'const'-qualifier to reference
        //:   types, function types, or types that are already
        //:   'const'-qualified.
        //
        // Plan:
        //   Verify that 'bsl::add_const::type' has the correct type for each
        //   concern.
        //
        // Testing:
        //   bsl::add_const::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::add_const::type'\n"
                            "\n======================\n");

        // C-1
        ASSERT((is_same<add_const<int>::type, int const>::value));
        ASSERT((is_same<add_const<int *>::type, int * const>::value));
        ASSERT((is_same<add_const<int const *>::type,
                                                   int const * const>::value));
        ASSERT((is_same<add_const<TestType>::type, TestType const>::value));

        // C-2
        ASSERT((is_same<add_const<int &>::type, int &>::value));
        ASSERT((is_same<add_const<TestType &>::type, TestType &>::value));
        ASSERT((is_same<add_const<int (int)>::type, int (int)>::value));
        ASSERT((is_same<add_const<int const>::type, int const>::value));
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
