// bslmf_tag.t.cpp                                                    -*-C++-*-

#include <bslmf_tag.h>

#include <cstdlib>      // atoi()
#include <cstring>      // strcmp()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf_MetaTag
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
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

// verify that the tag is evaluated at compile-time

template <int N>
bslmf_Tag<N> tag() {
    ASSERT(0);
    return bslmf_Tag<N>();
}

enum {
    C0  = 1 + BSLMF_TAG_TO_INT(tag<0>()),  // 1
    C1  = 1 + BSLMF_TAG_TO_INT(tag<1>()),  // 2
    C2  = 1 + BSLMF_TAG_TO_INT(tag<2>())   // 3
};

const unsigned U5 = -5;
const unsigned C5 = BSLMF_TAG_TO_UINT(tag<-5>());  // (unsigned)-5

const int CM5 = BSLMF_TAG_TO_INT(tag<-5>());  // -5

template <class T>
void doSomethingImp(T *t, bslmf_Tag<0>)
{
    // slow but generic implementation
}

template <class T>
void doSomethingImp(T *t, bslmf_Tag<1>)
{
    // fast implementation (works only for some T's)
}

template <class T, bool IsFast>
void doSomething(T *t)
{
    doSomethingImp(t, bslmf_Tag<IsFast>());
}

void f()
{
    int i;
    doSomething<int, true>(&i); // fast version selected for int

    double m;
    doSomething<double, false>(&m); // slow version selected for double
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
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;
        f();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_MetaTag' with various constant integral
        //   values and verify that their 'VALUE' member is initialized
        //   properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_Tag" << endl
                          << "=========" << endl;

        ASSERT(1  == C0);
        ASSERT(2  == C1);
        ASSERT(3  == C2);

        ASSERT((unsigned)-5 == C5);

        ASSERT(-5 == (int)CM5);

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
