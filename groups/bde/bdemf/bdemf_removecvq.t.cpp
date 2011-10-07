// bdemf_removecvq.t.cpp                                              -*-C++-*-

#include <bdemf_removecvq.h>

#include <bsls_platformutil.h>
#include <bslmf_issame.h>   // for testing only
#include <bslmf_assert.h>   // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bdemf_RemoveCvq
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

#define ASSERT_REFTYPE_ISSAME(T)  \
    BSLMF_ASSERT((bslmf_IsSame<T, bdemf_RemoveCvq<T>::Type>::VALUE));

// from component doc

struct MyType {};


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
// TBD: TEST 'bdemf_RemovePtrCvq' was added as a work-around for the Sun
// compiler, but is useful in its own right and should be tested and
// documented.
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bdemf_RemoveCvq' with various types and verify
        //   that their 'Type' member is initialized properly.
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "bdemf_RemoveCvq" << endl
                          << "===============" << endl;

        // from component doc

        bdemf_RemoveCvq<int              >::Type i1; // int
        bdemf_RemoveCvq<const int        >::Type i2; // int
        bdemf_RemoveCvq<volatile int     >::Type i3; // int
        bdemf_RemoveCvq<int *            >::Type i4; // int *
        bdemf_RemoveCvq<int **           >::Type i5; // int **
        bdemf_RemoveCvq<int *const       >::Type i6; // int *
        bdemf_RemoveCvq<int *const *     >::Type i7; // int *const *
        bdemf_RemoveCvq<int *const *const>::Type i8; // int *const *
        bdemf_RemoveCvq<MyType           >::Type m1; // MyType
        bdemf_RemoveCvq<MyType const     >::Type m2; // MyType

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

        bdemf_RemoveCvq<const int *            >::Type a1; // const int *
        bdemf_RemoveCvq<const int **           >::Type a2; // const int **
        bdemf_RemoveCvq<const int *const       >::Type a3; // const int *
        bdemf_RemoveCvq<const int *const *     >::Type a4; // const int *const*
        bdemf_RemoveCvq<const int *const *const>::Type a5; // const int *const*

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

        bdemf_RemoveCvq<const volatile int     >::Type b1; // int
        bdemf_RemoveCvq<const volatile int *   >::Type b2; // cv int *
        bdemf_RemoveCvq<int *const volatile    >::Type b3; // int *

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
