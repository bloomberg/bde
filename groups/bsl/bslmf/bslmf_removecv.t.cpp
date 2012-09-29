// bslmf_removecv.t.cpp                                               -*-C++-*-
#include <bslmf_removecv.h>

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
// The object under test is a meta-functions, 'bsl::remove_cv', that removes
// the top-level cv-qualifier from a template parameter type.  Thus, we need to
// ensure that the values returned by the meta-function is correct for each
// possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_cv
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
///Example 1: Removing The CV-Qualifier of A Type
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to strip the cv-qualifier from a particular type.
//
// First, we create two 'typedef's -- a cv-qualified type ('MyCvType') and the
// same type without the cv-qualifier ('MyType'):
//..
        typedef int                MyType;
        typedef const volatile int MyCvType;
//..
// Now, we strip the the cv-qualifier from 'MyCvType' using 'bsl::remove_cv'
// and verify that the resulting type is the same as 'MyType':
//..
        ASSERT(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
                                                              MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_cv'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_cv'
        //   instantiations has the same type as the template parameter type
        //   except has any top-level cv-qualifier removed.
        //
        // Concerns:
        //: 1 'bsl::remove_cv::type' is 'T' when 'TYPE' is 'T'.
        //:
        //: 2 'bsl::remove_cv::type' is 'T' when 'TYPE' is 'T const'.
        //:
        //: 3 'bsl::remove_cv::type' is 'T' when 'TYPE' is 'T volatile'.
        //:
        //: 4 'bsl::remove_cv::type' is 'T' when 'TYPE' is 'T const volatile'.
        //:
        //: 5 'bsl::remove_cv::type' is 'T const volatile *' when 'TYPE' is
        //:   'T const volatile *'.
        //
        // Plan:
        //   Verify that 'bsl::remove_cv' returns the correct value for each
        //   concern.
        //
        // Testing:
        //   bsl::remove_cv
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_const'\n"
                            "\n===================\n");

        // C-1
        ASSERT((is_same<remove_cv<int>::type, int>::value));
        ASSERT((is_same<remove_cv<int *>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType>::type, TestType>::value));

        // C-2
        ASSERT((is_same<remove_cv<int const>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const>::type, TestType>::value));

        // C-3
        ASSERT((is_same<remove_cv<int volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType volatile>::type, TestType>::value));

        // C-4
        ASSERT((is_same<remove_cv<int const volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<TestType const volatile>::type,
                                                            TestType>::value));

        // C-5
        ASSERT((is_same<remove_volatile<int const volatile *>::type,
                                                int const volatile *>::value));
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
