// bslstl_function_test.t.cpp                                         -*-C++-*-

#include <bslstl_function_test.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;
using bsls::NameOf;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                             Overview
//                             --------
// The object under test is a 'bsl::function' object initialized with a lambda
// or with a callable object that captures a reference or pointer to a local
// variable in a specific way, as per '{DRQS 160155173}'.  When built with
// several different versions of 'g++' using optimization of '-O2' or higher,
// the compiler appears to believe that a flag set by the invoked function is
// not, in fact, altered, and elides a test, giving an incorrect result.  As of
// this writing, we do not yet know whether this is triggered by formally
// undefined behavior in our code or by an error in the compiler.  We do know
// that modifying 'bslma::ConstructorUtil::construct' to always use placement
// new rather than 'memcpy' appears to make the problem go away.
//
// This test is in a separate test driver because placing the same code in the
// main 'bslstl_function' test driver does not trigger the error!
//
// ----------------------------------------------------------------------------
// [ 1] STRANGE BEHAVIOR

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

struct SimulatedLambdaWithReference
    // This class holds a bool value and a bool reference and has a function
    // call operator that assigns the value to the reference.
{
    // DATA
    bool  d_s;  // the value
    bool& d_b;  // the reference

    // ACCESSORS
    void operator()() const;
        // Perform 'd_b = d_s'.
};

void SimulatedLambdaWithReference::operator()() const
{
    if (veryVerbose) { Q(Reference) }
    d_b = d_s;
}

struct SimulatedLambdaWithPointer
    // This class holds a bool value and a bool pointer and has a function call
    // operator that assigns the value through the pointer.
{
    // DATA
    bool  d_s;    // the value
    bool *d_b_p;  // the pointer

    void operator()() const;
        // Perform '*d_b_p = d_s'.
};

void SimulatedLambdaWithPointer::operator()() const
{
    if (veryVerbose) { Q(Pointer) }
    *d_b_p = d_s;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // STRANGE BEHAVIOR
        //
        // Concerns:
        //: 1 Certain uses of 'bsl::function' involving lambdas or simulated
        //:   versions thereof are either encountering undefined behavior or
        //:   a bug in 'g++'.  See '{DRQS 160155173}'.  We wish to prepare
        //:   tests that exercise the problem absent a fix elsewhere.
        //
        // Plan:
        //: 1 Implement several forms of the problematic cases.  They will fail
        //:   only if they are compiled incorrectly.
        //
        // Testing:
        //  STRANGE BEHAVIOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nSTRANGE BEHAVIOR"
                            "\n================\n");

        typedef bsl::function<void()> Obj;

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        if (veryVerbose) printf("\t...with lambdas\n");
        {

            bool s = true, b = false;

            Obj o;
            new (&o) Obj([s, &b] { b = s; });
            o();

            ASSERT(b);
        }
#endif

        if (veryVerbose) printf("\t...with lambda simulated by reference\n");
        {
            bool s = true, b = false;

            SimulatedLambdaWithReference sl = { s, b };

            Obj o;
            new (&o) Obj(sl);
            o();

            ASSERT(b);
        }

        if (veryVerbose) printf("\t...with lambda simulated by pointer\n");
        {
            bool s = true, b = false;

            SimulatedLambdaWithPointer sl = { s, &b };

            Obj o;
            new (&o) Obj(sl);
            o();

            ASSERT(b);
        }
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
// Copyright 2020 Bloomberg Finance L.P.
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
