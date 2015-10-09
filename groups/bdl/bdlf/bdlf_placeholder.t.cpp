// bdlf_placeholder.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlf_placeholder.h>

#include <bsls_bsltestutil.h>

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
// [ 1] template <int I> struct bdlf::PlaceHolder
// [ 2] _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template <int I>
int indexOf(const bdlf::PlaceHolder<I>&)
{
    return I;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

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

        using namespace bdlf::PlaceHolders;

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
        //   template <int I> struct bdlf::PlaceHolder
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting `bdlf::PlaceHolder<I>'"
                            "\n=============================\n");

        ASSERT(-2 == bdlf::PlaceHolder<-2>::VALUE);
        ASSERT(-1 == bdlf::PlaceHolder<-1>::VALUE);
        ASSERT( 0 == bdlf::PlaceHolder< 0>::VALUE);
        ASSERT( 1 == bdlf::PlaceHolder< 1>::VALUE);
        ASSERT( 2 == bdlf::PlaceHolder< 2>::VALUE);

        ASSERT(bslmf::IsBitwiseMoveable<bdlf::PlaceHolder<-2> >::VALUE);
        ASSERT(bslmf::IsBitwiseMoveable<bdlf::PlaceHolder<-1> >::VALUE);
        ASSERT(bslmf::IsBitwiseMoveable<bdlf::PlaceHolder< 0> >::VALUE);
        ASSERT(bslmf::IsBitwiseMoveable<bdlf::PlaceHolder< 1> >::VALUE);
        ASSERT(bslmf::IsBitwiseMoveable<bdlf::PlaceHolder< 2> >::VALUE);

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
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
