// bslmf_isvoid.t.cpp                                                 -*-C++-*-
#include <bslmf_isvoid.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // sprintf()
#include <stdlib.h>     // atoi()

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 2] class bslmf::IsVoid
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=========================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-------------------------------------------------------------------------
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
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------
// These standard aliases will be defined below as part of the usage example.

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
namespace
{
struct Empty {
};

template<class TYPE>
struct Identity {
    typedef TYPE Type;
};

struct Incomplete;

template<class TYPE>
bool typeDependentTest() {
    return bslmf::IsVoid<typename Identity<TYPE>::Type >::VALUE;
}

}  // close anonymous namespace
//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    setbuf(stdout, 0);    // Use unbuffered output

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

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING bslmf::IsVoid metafunction:
        // Concerns:
        //: 1 The metafunction returns 'true' for any 'void' type, regardless
        //:   of its cv-qualification.
        //: 2 The metafunction returns 'false' for every other type
        //
        // Plan:
        //
        // Testing:
        //   bslmf::IsVoid
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING bslmf::IsVoid metafunction"
                            "\n==================================\n");

        // Basic test dataset
        ASSERT(bslmf::IsVoid<void>::VALUE);
        ASSERT(bslmf::IsVoid<const void>::VALUE);
        ASSERT(bslmf::IsVoid<volatile void>::VALUE);
        ASSERT(bslmf::IsVoid<const volatile void>::VALUE);
        ASSERT(!bslmf::IsVoid<void *>::VALUE);
        ASSERT(!bslmf::IsVoid<void *&>::VALUE);
        ASSERT(!bslmf::IsVoid<void()>::VALUE);
        ASSERT(!bslmf::IsVoid<void(*)()>::VALUE);
        ASSERT(!bslmf::IsVoid<void *Empty::*>::VALUE);
        ASSERT(!bslmf::IsVoid<bslmf::IsVoid<void> >::VALUE);
        ASSERT(!bslmf::IsVoid<Incomplete>::VALUE);

        // Test nested template typenames with the same dataset
        ASSERT(bslmf::IsVoid<Identity<void>::Type>::VALUE);
        ASSERT(bslmf::IsVoid<Identity<const void>::Type>::VALUE);
        ASSERT(bslmf::IsVoid<Identity<volatile void>::Type>::VALUE);
        ASSERT(bslmf::IsVoid<Identity<const volatile void>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<void *>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<void *&>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<void()>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<void(*)()>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<void *Empty::*>::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<bslmf::IsVoid<void> >::Type>::VALUE);
        ASSERT(!bslmf::IsVoid<Identity<Incomplete>::Type>::VALUE);

        // Test type-dependant typenames with the same dataset
        ASSERT(typeDependentTest<void>());
        ASSERT(typeDependentTest<const void>());
        ASSERT(typeDependentTest<volatile void>());
        ASSERT(typeDependentTest<const volatile void>());
        ASSERT(!typeDependentTest<void *>());
        ASSERT(!typeDependentTest<void *&>());
        ASSERT(!typeDependentTest<void()>());
        ASSERT(!typeDependentTest<void(*)()>());
        ASSERT(!typeDependentTest<void *Empty::*>());
        ASSERT(!typeDependentTest<bslmf::IsVoid<void> >());
        ASSERT(!typeDependentTest<Incomplete>());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT(bslmf::IsVoid<void>::VALUE);
        ASSERT(!bslmf::IsVoid<int>::VALUE);

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
