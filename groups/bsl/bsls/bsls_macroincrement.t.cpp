// bsls_macroincrement.t.cpp                                          -*-C++-*-

#include <bsls_macroincrement.h>

#include <cstdlib>     // atoi()
#include <iostream>

// using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test defines a macro that increments given argument in
// preprocessors.  There is no primary manipulator or basic accessor of this
// component.
//-----------------------------------------------------------------------------
// [ 1] BSLS_MACROINCREMENT(MACRO_VALUE)
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define TAB cout << '\t';                     // output the tab character.
#define L_ __LINE__                           // current Line number

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define LINENO(x)  x

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use the macro defined
        //   in this component.
        //
        // Concerns:
        //:  The usage example provided in the component header file must
        //:  compile, link, and run on all platforms as shown.
        //
        // Plan:
        //:  Incorporate usage example from header into driver, remove leading
        //:  comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        cout << "__LINE__, __FILE__: " << __LINE__ << ", " __FILE__ << "\n";
        #line BSLS_MACROINCREMENT(__LINE__) "some_other_filename.cpp"
        cout << "__LINE__, __FILE__: " << __LINE__ << ", " __FILE__ << "\n";
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST BSLS_MACROINCREMENT
        //   This test verifies correctness of 'BSLS_MACROINCREMENT' macro.
        //
        // Concerns:
        //:  1 The 'BSLS_MACROINCREMENT' correctly increments the given
        //:    argument (either an integer literal or a macro that ultimately
        //:    expands to an integer literal) in preprocessors.
        //
        // Plan:
        //:  1 For integer literal V in the range '[0, 19999]', set
        //:    'BSLS_MACROINCREMENT(V)' as the first argument following '#line'
        //:    directive.  Verify the line number is correctly incremented.
        //:    (C-1)
        //:
        //:  2 For integer literal V in the range '[0, 19999]', set
        //:    'BSLS_MACROINCREMENT(LINENO(V))' as the first argument following
        //:    '#line' directive where 'LINENO' is a macro eventually expanding
        //:     to an integer literal in the range '[0, 19999']'.  Verify the
        //:     line number is correctly incremented.  (C-1)
        //
        // Testing:
        //   BSLS_MACROINCREMENT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'BSLS_MACROINCREMENT' on integer literal"
                          << endl
                          << "================================================"
                          << endl;
        {
            #define TESTVALUE_1 BSLS_MACROINCREMENT(0)
            ASSERT(1 == TESTVALUE_1);

            #define TESTVALUE_2 BSLS_MACROINCREMENT(1)
            ASSERT(2 == TESTVALUE_2);

            #define TESTVALUE_3 BSLS_MACROINCREMENT(2)
            ASSERT(3 == TESTVALUE_3);

            #define TESTVALUE_4 BSLS_MACROINCREMENT(3)
            ASSERT(4 == TESTVALUE_4);

            #define TESTVALUE_5 BSLS_MACROINCREMENT(4)
            ASSERT(5 == TESTVALUE_5);

            #define TESTVALUE_6 BSLS_MACROINCREMENT(5)
            ASSERT(6 == TESTVALUE_6);

            #define TESTVALUE_101 BSLS_MACROINCREMENT(100)
            ASSERT(101 == TESTVALUE_101);

            #define TESTVALUE_1001 BSLS_MACROINCREMENT(1000)
            ASSERT(1001 == TESTVALUE_1001);

            #define TESTVALUE_5770 BSLS_MACROINCREMENT(5769)
            ASSERT(5770 == TESTVALUE_5770);

            #define TESTVALUE_10001 BSLS_MACROINCREMENT(10000)
            ASSERT(10001 == TESTVALUE_10001);

            #define TESTVALUE_12346 BSLS_MACROINCREMENT(12345)
            ASSERT(12346 == TESTVALUE_12346);

            #define TESTVALUE_20000 BSLS_MACROINCREMENT(19999)
            ASSERT(20000 == TESTVALUE_20000);
        }

        if (verbose) cout << endl
                          << "Testing 'BSLS_MACROINCREMENT' on macro" << endl
                          << "======================================" << endl;
        {
            #define TESTMACRO_1 BSLS_MACROINCREMENT(LINENO(0))
            ASSERT(1 == TESTVALUE_1);

            #define TESTMACRO_2 BSLS_MACROINCREMENT(LINENO(1))
            ASSERT(2 == TESTMACRO_2);

            #define TESTMACRO_3 BSLS_MACROINCREMENT(LINENO(2))
            ASSERT(3 == TESTMACRO_3);

            #define TESTMACRO_4 BSLS_MACROINCREMENT(LINENO(3))
            ASSERT(4 == TESTMACRO_4);

            #define TESTMACRO_5 BSLS_MACROINCREMENT(LINENO(4))
            ASSERT(5 == TESTMACRO_5);

            #define TESTMACRO_6 BSLS_MACROINCREMENT(LINENO(5))
            ASSERT(6 == TESTMACRO_6);

            #define TESTMACRO_101 BSLS_MACROINCREMENT(LINENO(100))
            ASSERT(101 == TESTMACRO_101);

            #define TESTMACRO_1001 BSLS_MACROINCREMENT(LINENO(1000))
            ASSERT(1001 == TESTMACRO_1001);

            #define TESTMACRO_5770 BSLS_MACROINCREMENT(LINENO(5769))
            ASSERT(5770 == TESTMACRO_5770);

            #define TESTMACRO_10001 BSLS_MACROINCREMENT(LINENO(10000))
            ASSERT(10001 == TESTMACRO_10001);

            #define TESTMACRO_12346 BSLS_MACROINCREMENT(LINENO(12345))
            ASSERT(12346 == TESTMACRO_12346);

            #define TESTMACRO_20000 BSLS_MACROINCREMENT(LINENO(19999))
            ASSERT(20000 == TESTMACRO_20000);
        }
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
