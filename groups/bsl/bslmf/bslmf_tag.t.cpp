// bslmf_tag.t.cpp                                                    -*-C++-*-

#include <bslmf_tag.h>

#include <cstdlib>      // 'atoi'
#include <cstring>      // 'strcmp'
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] bslmf::Tag
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
bslmf::Tag<N> tag() {
    ASSERT(0);
    return bslmf::Tag<N>();
}

enum {
    C0  = 1 + BSLMF_TAG_TO_INT(tag<0>()),  // 1
    C1  = 1 + BSLMF_TAG_TO_INT(tag<1>()),  // 2
    C2  = 1 + BSLMF_TAG_TO_INT(tag<2>())   // 3
};

const unsigned U5 = -5;
const unsigned C5 = BSLMF_TAG_TO_UINT(tag<-5>());  // (unsigned)-5

const int CM5 = BSLMF_TAG_TO_INT(tag<-5>());  // -5

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The most common use of this structure is to perform static function
// dispatching based on a compile-time calculation.  Often the calculation is
// nothing more than a simple predicate, allowing us to select one of two
// functions.  The following function, 'doSomething', uses a fast
// implementation (e.g., 'memcpy') if the parameterized type allows for such
// operations; otherwise it will use a more generic and slower implementation
// (e.g., copy constructor).
//..
    template <class T>
    void doSomethingImp(T *t, bslmf::Tag<0> *)
    {
        // slow but generic implementation
    }

    template <class T>
    void doSomethingImp(T *t, bslmf::Tag<1> *)
    {
        // fast implementation (appropriate for bitwise-movable types)
    }

    template <class T, bool IsFast>
    void doSomething(T *t)
    {
        doSomethingImp(t, (bslmf::Tag<IsFast> *)0);
    }
//..
// For some parameter types, the fast version of 'doSomethingImp' is not legal.
// The power of this approach is that the compiler will compile just the
// implementation selected by the tag argument.
//..
    void f()
    {
        int i;
        doSomething<int, true>(&i);      // fast version selected for 'int'

        double m;
        doSomething<double, false>(&m);  // slow version selected for 'double'
    }
//..
// Note that an alternative design would be to use template partial
// specialization instead of standard function overloading to avoid the
// cost of passing a 'bslmf::Tag<N>' pointer.

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
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        f();  // call function defined in Usage

//
// The value of the integral parameter supplied to an instantiation of
// 'bslmf::Tag<N>' is "recoverable" by using the 'BSLMF_TAG_TO_INT' macro.
// For example:
//..
    bslmf::Tag<7> tag;
    ASSERT( 7 == BSLMF_TAG_TO_INT(tag));
    ASSERT(53 == BSLMF_TAG_TO_INT(bslmf::Tag<50 + 3>()));
//..
// The 'BSLMF_TAG_TO_BOOL' macro can be used to determine if the parameter is
// non-zero:
//..
    ASSERT( 1 == BSLMF_TAG_TO_BOOL(tag));
    ASSERT( 0 == BSLMF_TAG_TO_BOOL(bslmf::Tag<0>()));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::Tag' with various constant integral
        //   values and verify that their 'VALUE' member is initialized
        //   properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::Tag" << endl
                          << "==========" << endl;

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
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
