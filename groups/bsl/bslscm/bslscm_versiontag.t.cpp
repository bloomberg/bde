// bslscm_versiontag.t.cpp                                            -*-C++-*-

#include <bslscm_versiontag.h>

#include <cstdlib>     // atoi()
#include <cstring>
#include <cstdio>

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;
static bool verbose = false;
static bool veryVerbose = false;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { std::printf("%s: %d\n", #I, I); \
                aSsErT(1, #X, __LINE__); } }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) std::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ std::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  USAGE EXAMPLE HELPER FUNCTIONS
//-----------------------------------------------------------------------------

int newFunction()
    // Return 1
{
    return 1;
}

// int OldFunction()
// Not defined and never called due to conditional compilation

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);

    std::printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 3: {
        //--------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concern:
        //   That the usage example in the user documentation compiles and
        //   runs as expected.
        //
        // Plan:
        //   Use the exact text of the usage example from the user
        //   documentation, but change uses of 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) std::printf("\nTEST USAGE EXAMPLE"
                                 "\n==================\n");

// At compile time, the version of BSL can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if a function changed names (a rare
// occurrence, but disruptive when it does happen), disruption can be minimized
// by conditionally calling the old or new function name using conditional
// compilation.  The '#if' directive compares 'BSL_VERSION' to a specified
// major, minor, and patch version 4 composed using 'BSL_MAKE_VERSION':
//..
    #if BSL_VERSION > BSL_MAKE_VERSION(1, 2)
        // Call 'newFunction' for BSL version 1.2 and later:
        int result = newFunction();
    #else
        // Call 'oldFunction' for BSL older than version 1.2:
        int result = oldFunction();
    #endif

        ASSERT(result);
//..
      } break;

      case 2: {
        //--------------------------------------------------------------------
        // TEST BSL_MAKE_VERSION MACRO
        //
        // Concerns:
        //   That BSL_MAKE_VERSION create a compile-time constant if its
        //   arguments are all compile-time constants.
        //   That BSL_MAKE_VERSION correctly composes a major, minor, and
        //   patch version number into a single integer.  Each component can
        //   be in the range 0-99.
        //
        // Plan:
        //   Use the result of BSL_MAKE_VERSION as an array dimension to
        //   prove that it is a compile-time constant.
        //   Using ad-hoc data selection, create a number of version values
        //   using the 'BSL_MAKE_VERSION' macro and verify that the expected
        //   value matches the actual value.
        //
        // Testing:
        //   BSL_MAKE_VERSION(major, minor)
        //--------------------------------------------------------------------

        if (verbose) std::printf("\nTEST BSL_MAKE_VERSION MACRO"
                                 "\n===========================\n");
#if 0

        // Test that 'BSL_MAKE_VERSION(0,1,2)' is a compile-time constant.
        static const char COMPILE_ASSERT[BSL_MAKE_VERSION(0,1,2)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 102);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_patch;
            int d_version;
        } const DATA[] = {
            //line major minor patch  version
            //---- ----- ----- -----  -------
            {  L_,    0,    0,    0,        0 },
            {  L_,    0,    0,    1,        1 },
            {  L_,    0,    1,    0,      100 },
            {  L_,    0,    1,    1,      101 },
            {  L_,    1,    0,    0,    10000 },
            {  L_,    1,    0,    1,    10001 },
            {  L_,    1,    1,    0,    10100 },
            {  L_,   12,   34,   56,   123456 },
            {  L_,   99,   99,   99,   999999 },
            {  L_,    9,    9,    9,    90909 },
            {  L_,   10,   20,   30,   102030 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int PATCH = DATA[i].d_patch;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BSL_MAKE_VERSION(MAJOR, MINOR, PATCH));
        }
#endif
        static const char COMPILE_ASSERT[BSL_MAKE_VERSION(0,1)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 100);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_version;
        } const DATA[] = {
            //line major minor version
            //---- ----- ----- -------
            {  L_,    0,    0,       0 },
            {  L_,    0,    1,     100 },
            {  L_,    1,    0,   10000 },
            {  L_,    1,    1,   10100 },
            {  L_,   12,   34,  123400 },
            {  L_,   99,   99,  999900 },
            {  L_,    9,    9,   90900 },
            {  L_,   10,   20,  102000 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BSL_MAKE_VERSION(MAJOR, MINOR));
        }

      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST VERSION CONSISTENCY
        //
        // Concerns:
        //   That BSL_VERSION corresponds to the two components
        //   BSL_VERSION_MAJOR and BSL_VERSION_MINOR
        //
        // Plan:
        //   Decompose BSL_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BSL_VERSION
        //   BSL_VERSION_MAJOR
        //   BSL_VERSION_MINOR
        //--------------------------------------------------------------------

        if (verbose) std::printf("\nTEST VERSION CONSISTENCY"
                                 "\n========================\n");

        int major = (BSL_VERSION / 10000) % 100;
        int minor = (BSL_VERSION / 100) % 100;

        ASSERT(BSL_VERSION_MAJOR == major);
        ASSERT(BSL_VERSION_MINOR == minor);

      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
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
