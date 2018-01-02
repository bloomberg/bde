// bslmf_removecvq.t.cpp                                              -*-C++-*-

#include <bslmf_removecvq.h>

#include <bslmf_assert.h>   // for testing only
#include <bslmf_issame.h>   // for testing only

#include <bsls_bsltestutil.h>

#include <stdio.h>      // print, fprintf
#include <stdlib.h>     // atoi

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf::RemoveCvq

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#define ASSERT_REFTYPE_ISSAME(T)  \
    BSLMF_ASSERT((bsl::is_same<T, bslmf::RemoveCvq<T>::Type>::value));

// from component doc

struct MyType {};

template <class TYPE>
bool isSame(TYPE& /* a */, TYPE& /* b */) { return true; }

template <class TYPEA, class TYPEB>
bool isSame(TYPEA& /* a */, TYPEB& /* b */) { return false; }

template <class TYPEA, class TYPEB>
bool isSortaSame(TYPEA& a, TYPEB& b)
{
    typename bslmf::RemoveCvq<TYPEA>::Type aa = a;
    typename bslmf::RemoveCvq<TYPEB>::Type bb = b;

    return isSame(aa, bb);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //
        // Plan:
        //   Demonstrate a simple use of this template.
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

        int i = 0, j = 0;
        const int ci = 0, cj = 0;
        volatile int vi = 0, vj = 0;
        const volatile int cvi = 0, cvj = 0;

        double x = 0, y = 0;

        ASSERT(  isSame(i, j));
        ASSERT(  isSame(ci, cj));
        ASSERT(  isSame(vi, vj));
        ASSERT(  isSame(cvi, cvj));
        ASSERT(  isSame(x, y));

        ASSERT(! isSame(i, x));
        ASSERT(! isSame(i, ci));
        ASSERT(! isSame(i, vi));
        ASSERT(! isSame(i, cvi));
        ASSERT(! isSame(ci, vi));
        ASSERT(! isSame(ci, cvi));
        ASSERT(! isSame(vi, cvi));

        ASSERT(! isSortaSame(i, x));
        ASSERT(  isSortaSame(i, ci));
        ASSERT(  isSortaSame(i, vi));
        ASSERT(  isSortaSame(i, cvi));
        ASSERT(  isSortaSame(ci, vi));
        ASSERT(  isSortaSame(ci, vi));
        ASSERT(  isSortaSame(ci, cvi));
        ASSERT(  isSortaSame(vi, cvi));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::RemoveCvq' with various types and verify
        //   that their 'Type' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::RemoveCvq'\n"
                            "\n==========================\n");

        // from component doc

        bslmf::RemoveCvq<int              >::Type i1; // int
        bslmf::RemoveCvq<const int        >::Type i2; // int
        bslmf::RemoveCvq<volatile int     >::Type i3; // int
        bslmf::RemoveCvq<int *            >::Type i4; // int *
        bslmf::RemoveCvq<int **           >::Type i5; // int **
        bslmf::RemoveCvq<int *const       >::Type i6; // int *
        bslmf::RemoveCvq<int *const *     >::Type i7; // int *const *
        bslmf::RemoveCvq<MyType           >::Type m1; // MyType
        bslmf::RemoveCvq<MyType const     >::Type m2; // MyType

        int i = 3;
        int *const J = &i;

        i1 = i;
        i2 = i;
        i3 = i;
        i4 = &i;
        i5 = &i4;
        i6 = &i;
        i7 = &J;

        int *pi;
        int **ppi;
        int ***pppi;
        int *const **ppcpi;
        MyType *pm;

        pi      = &i1;
        pi      = &i2;
        pi      = &i3;
        ppi     = &i4;
        pppi    = &i5;
        ppi     = &i6;
        ppcpi   = &i7;
        pm      = &m1;
        pm      = &m2;

        ASSERT(pi && ppi && pppi && ppcpi && pm);

        // References should remain unchanged.

        ASSERT_REFTYPE_ISSAME(int              &);
        ASSERT_REFTYPE_ISSAME(const int        &);
        ASSERT_REFTYPE_ISSAME(volatile int     &);
        ASSERT_REFTYPE_ISSAME(int *            &);
        ASSERT_REFTYPE_ISSAME(int **           &);
        ASSERT_REFTYPE_ISSAME(int *const       &);
        ASSERT_REFTYPE_ISSAME(int *const *     &);
        ASSERT_REFTYPE_ISSAME(int *const *const&);
        ASSERT_REFTYPE_ISSAME(MyType           &);
        ASSERT_REFTYPE_ISSAME(MyType const     &);

        bslmf::RemoveCvq<const int *            >::Type a1; //const int *
        bslmf::RemoveCvq<const int **           >::Type a2; //const int **
        bslmf::RemoveCvq<const int *const       >::Type a3; //const int *
        bslmf::RemoveCvq<const int *const *     >::Type a4; //const int *const*
        bslmf::RemoveCvq<const int *const *const>::Type a5; //const int *const*

        const int M = 4;
        const int *const N = &M;

        a1 = &M;
        a2 = &a1;
        a3 = &M;
        a4 = &N;
        a5 = &N;

        const int **ppci;
        const int ***pppci;
        const int *const **ppcpci;

        ppci   = &a1;
        pppci  = &a2;
        ppci   = &a3;
        ppcpci = &a4;
        ppcpci = &a5;

        ASSERT(ppci && pppci && ppcpci);

        bslmf::RemoveCvq<const volatile int     >::Type b1; // int
        bslmf::RemoveCvq<const volatile int *   >::Type b2; // cv int *
        bslmf::RemoveCvq<int *const volatile    >::Type b3; // int *

        b1 = i;
        b2 = &i;
        b3 = &i;

        const volatile int **ppcvi;

        pi    = &b1;
        ppcvi = &b2;
        ppi   = &b3;

        ASSERT(pi && ppcvi && ppi);

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
// Copyright 2013 Bloomberg Finance L.P.
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
