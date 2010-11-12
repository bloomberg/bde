// bslmf_removecvq.t.cpp            -*-C++-*-

#include <bslmf_removecvq.h>

#include <cstdlib>     // atoi()
#include <cstring>     // strcmp()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf_RemoveCvq
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// from component doc

struct MyType {};

template <typename TYPE>
bool isSame(TYPE& a, TYPE& b) { return true; }
template <typename TYPEA, typename TYPEB>
bool isSame(TYPEA& a, TYPEB& b) { return false; }

template <typename TYPEA, typename TYPEB>
bool isSortaSame(TYPEA& a, TYPEB& b)
{
    typename bslmf_RemoveCvq<TYPEA>::Type aa = a;
    typename bslmf_RemoveCvq<TYPEB>::Type bb = b;

    return isSame(aa, bb);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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
        //   Instantiate 'bslmf_RemoveCvq' with various types and verify
        //   that their 'Type' member is initialized properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_RemoveCvq" << endl
                          << "===============" << endl;

        // from component doc

        bslmf_RemoveCvq<int              >::Type i1; // int
        bslmf_RemoveCvq<const int        >::Type i2; // int
        bslmf_RemoveCvq<volatile int     >::Type i3; // int
        bslmf_RemoveCvq<int *            >::Type i4; // int *
        bslmf_RemoveCvq<int **           >::Type i5; // int **
        bslmf_RemoveCvq<int *const       >::Type i6; // int *
        bslmf_RemoveCvq<int *const *     >::Type i7; // int *const *
        bslmf_RemoveCvq<int *const *const>::Type i8; // int *const *
        bslmf_RemoveCvq<MyType           >::Type m1; // MyType
        bslmf_RemoveCvq<MyType const     >::Type m2; // MyType

        int i = 3;
        int *const J = &i;

        i1 = i;
        i2 = i;
        i3 = i;
        i4 = &i;
        i5 = &i4;
        i6 = &i;
        i7 = &J;
        i8 = &J;

        int *pi;
        int **ppi;
        int ***pppi;
        int *const *pcpi;
        int *const **ppcpi;
        MyType *pm;

        pi    = &i1;
        pi    = &i2;
        pi    = &i3;
        ppi   = &i4;
        pppi  = &i5;
        ppi   = &i6;
        ppcpi = &i7;
        ppcpi = &i8;
        pm    = &m1;
        pm    = &m2;

        ASSERT(pi && ppi && pppi && ppcpi && pm);

        const int ci = 0;
        volatile int vi = 1;
        int *const cpi = &i;
        int *const *const cpcpi = &cpi;
        const MyType cm = MyType();

        // References should remain unchanged.
        bslmf_RemoveCvq<int              &>::Type ri1 = i;
        bslmf_RemoveCvq<const int        &>::Type ri2 = ci;
        bslmf_RemoveCvq<volatile int     &>::Type ri3 = vi;
        bslmf_RemoveCvq<int *            &>::Type ri4 = pi;
        bslmf_RemoveCvq<int **           &>::Type ri5 = ppi;
        bslmf_RemoveCvq<int *const       &>::Type ri6 = cpi;
        bslmf_RemoveCvq<int *const *     &>::Type ri7 = pcpi;
        bslmf_RemoveCvq<int *const *const&>::Type ri8 = cpcpi;
        bslmf_RemoveCvq<MyType           &>::Type rm1 = m1;
        bslmf_RemoveCvq<MyType const     &>::Type rm2 = cm;

        ASSERT(&ri1 == &i);
        ASSERT(&ri2 == &ci);
        ASSERT(&ri3 == &vi);
        ASSERT(&ri4 == &pi);
        ASSERT(&ri5 == &ppi);
        ASSERT(&ri6 == &cpi);
        ASSERT(&ri7 == &pcpi);
        ASSERT(&ri8 == &cpcpi);
        ASSERT(&rm1 == &m1);
        ASSERT(&rm2 == &cm);

        int              & rri1 = ri1;
        const int        & rri2 = ri2;
        volatile int     & rri3 = ri3;
        int *            & rri4 = ri4;
        int **           & rri5 = ri5;
        int *const       & rri6 = ri6;
        int *const *     & rri7 = ri7;
        int *const *const& rri8 = ri8;
        MyType           & rrm1 = rm1;
        MyType const     & rrm2 = rm2;

        (void) rri1;
        (void) rri2;
        rri3 = 0;
        (void) rri4;
        (void) rri5;
        (void) rri6;
        (void) rri7;
        (void) rri8;
        (void) rrm1;
        (void) rrm2;

        bslmf_RemoveCvq<const int *            >::Type a1; // const int *
        bslmf_RemoveCvq<const int **           >::Type a2; // const int **
        bslmf_RemoveCvq<const int *const       >::Type a3; // const int *
        bslmf_RemoveCvq<const int *const *     >::Type a4; // const int *const*
        bslmf_RemoveCvq<const int *const *const>::Type a5; // const int *const*

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

        bslmf_RemoveCvq<const volatile int     >::Type b1; // int
        bslmf_RemoveCvq<const volatile int *   >::Type b2; // cv int *
        bslmf_RemoveCvq<int *const volatile    >::Type b3; // int *

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
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
