// bslmf_addcv.t.cpp                                                  -*-C++-*-
#include <bslmf_addcv.h>

#include <bslmf_issame.h>

#include <cstdlib>
#include <cstdio>

using namespace bsl;
using namespace BloombergLP;

using std::printf;
using std::fprintf;
using std::atoi;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-functions, 'bsl::add_const', that adds the
// top-level 'const'-qualifier to a template parameter type.  Thus, we need to
// ensure that the values returned by the meta-function is correct for each
// possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::add_const
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
   // argument for the template parameter 'TYPE' of 'bsl::add_cv'.
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

    (void) verbose;
    (void) veryVerbose;

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
///Example 1: Adding The 'const'-qualifier and 'volatile'-qualifier to A Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add the 'const'-qualifier and 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
        typedef int                MyType;
        typedef const volatile int MyCvType;
//..
// Now, we add the the 'const'-qualifier and the 'volatile'-qualifier to
// 'MyType' using 'bsl::add_cv' and verify that the resulting type is the
// same as 'MyCvType':
//..
        ASSERT(true ==
                   (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_cv'
        //   Ensure that the 'typedef' 'type' of 'bsl::add_cv' has the correct
        //   type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::add_cv' only adds the top-level 'const'-qualifier and
        //:   'volatile'-qualifier to regular types (test primitive, pointers,
        //:   and userdefined types).
        //:
        //: 2 'bsl::add_cv' doesn't add cv-qualifiers to reference types,
        //:   function types, and already both 'const'-qualified and
        //:   'volatile'-qualified types.
        //
        // Plan:
        //   Verify that 'bsl::add_cv::type' has the correct type for each
        //   concern.
        //
        // Testing:
        //   bsl::add_const
        // --------------------------------------------------------------------

        // C-1
        ASSERT((is_same<add_cv<int>::type, int const volatile>::value));
        ASSERT((is_same<add_cv<int const>::type, int const volatile>::value));
        ASSERT((is_same<add_cv<int volatile>::type, int const volatile>::value));

        ASSERT((is_same<add_cv<int *>::type, int * const volatile>::value));
        ASSERT((is_same<add_cv<int const volatile *>::type,
                                 int const volatile * const volatile>::value));
        ASSERT((is_same<add_cv<TestType>::type,
                                             TestType const volatile>::value));

        // C-2
        ASSERT((is_same<add_cv<int &>::type, int &>::value));
        ASSERT((is_same<add_cv<TestType &>::type, TestType &>::value));
        ASSERT((is_same<add_cv<int (int)>::type, int (int)>::value));
        ASSERT((is_same<add_cv<int const volatile>::type,
                                                  int const volatile>::value));

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
