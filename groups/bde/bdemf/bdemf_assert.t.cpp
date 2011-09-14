// bdemf_assert.t.cpp              -*-C++-*-

#include <bdemf_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>

using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] BDEMF_ASSERT(expr)
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

// namespace scope

BDEMF_ASSERT(sizeof(int) >= sizeof(char));

// un-named namespace
namespace {
    BDEMF_ASSERT(1);
    BDEMF_ASSERT(1); BDEMF_ASSERT(1); // not class scope
}

namespace {
    BDEMF_ASSERT(1);
}

namespace Bar {
    BDEMF_ASSERT(1);
    BDEMF_ASSERT(1); BDEMF_ASSERT(1); // not class scope
}

class MyType {
    // class scope

    BDEMF_ASSERT(sizeof(int) >= sizeof(char));
    BDEMF_ASSERT(sizeof(int) >= sizeof(char));

  public:
    int d_data;
    void foo();
};

void MyType::foo()
{
    // function scope

    BDEMF_ASSERT(sizeof(int) >= sizeof(char));
    BDEMF_ASSERT(1);

    using namespace BloombergLP;
    BDEMF_ASSERT(1);
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
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BDEMF_ASSERT MACRO
        // We have the following concerns:
        //   1. Works for all non-zero integral values.
        //   2. Works in and out of class scope.
        //
        // Plan:
        //   Invoke the macro at namespace, class, and function scope (above)
        //   and verify that it does not cause a compiler error.  Please see
        //   the 'ttt' test package group for test cases where the macro
        //   should cause a compile-time error.
        //
        // Testing:
        //   BDEMF_ASSERT(expr)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEMF_ASSERT Macro" << endl
                          << "==================" << endl;

        BDEMF_ASSERT(sizeof(int) >= sizeof(char));
        BDEMF_ASSERT(sizeof(int) >= sizeof(char));
        BDEMF_ASSERT(1);  ASSERT(131 == __LINE__);
        BDEMF_ASSERT(1);  ASSERT(132 == __LINE__);
        BDEMF_ASSERT(1 > 0 && 1);

// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)
#if !defined(BSLS_PLATFORM__CMP_MSVC) &&                \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
        bslmf_Assert_130 t1; // test typedef name creation; matches above line
        bslmf_Assert_131 t2; // test typedef name creation; matches above line
        ASSERT(sizeof t1 == sizeof t2);  // use t1 and t2
#endif

        BDEMF_ASSERT(2);
        BDEMF_ASSERT(-1);

        BDEMF_ASSERT(1); BDEMF_ASSERT(1); // not class scope

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
