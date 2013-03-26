// bdef_placeholder.t.cpp                                             -*-C++-*-

#include <bdef_placeholder.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] template <int I> struct bdef_PlaceHolder
// [ 2] _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <int I>
int indexOf(const bdef_PlaceHolder<I>&)
{
    return I;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;      // kill warnings about unused variable
    (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING _1, _2, ETC.
        //
        // Plan:
        //
        // Testing:
        //   _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting `_1', `_2', etc."
                            "\n========================\n");

        using namespace bdef_PlaceHolders;

        ASSERT( 1 == indexOf( _1));
        ASSERT( 2 == indexOf( _2));
        ASSERT( 3 == indexOf( _3));
        ASSERT( 4 == indexOf( _4));
        ASSERT( 5 == indexOf( _5));
        ASSERT( 6 == indexOf( _6));
        ASSERT( 7 == indexOf( _7));
        ASSERT( 8 == indexOf( _8));
        ASSERT( 9 == indexOf( _9));
        ASSERT(10 == indexOf(_10));
        ASSERT(11 == indexOf(_11));
        ASSERT(12 == indexOf(_12));
        ASSERT(13 == indexOf(_13));
        ASSERT(14 == indexOf(_14));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BDEF_PLACEHOLDER<I>
        //
        // Plan:
        //
        // Testing:
        //   template <int I> struct bdef_PlaceHolder
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting `bdef_PlaceHolder<I>'"
                            "\n=============================\n");

        ASSERT(-2 == bdef_PlaceHolder<-2>::VALUE);
        ASSERT(-1 == bdef_PlaceHolder<-1>::VALUE);
        ASSERT( 0 == bdef_PlaceHolder< 0>::VALUE);
        ASSERT( 1 == bdef_PlaceHolder< 1>::VALUE);
        ASSERT( 2 == bdef_PlaceHolder< 2>::VALUE);

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
