// bsls_buildtarget.t.cpp                                             -*-C++-*-

// N.B. This test driver must manipulate the definitions of the macros
// referenced by the component *before* the component header has a chance to
// act on the macros.
#ifdef BDE_BUILDTARGET_TEST_EXC
#  if defined(BDE_BUILD_TARGET_EXC)
#    undef BDE_BUILD_TARGET_EXC
#    define BDE_BUILD_TARGET_NO_EXC
#  else
#    define BDE_BUILD_TARGET_EXC
#    if defined(BDE_BUILD_TARGET_NO_EXC)
#      undef BDE_BUILD_TARGET_NO_EXC
#    endif
#  endif
#endif

#ifdef BDE_BUILDTARGET_TEST_MT
#  if defined(BDE_BUILD_TARGET_MT)
#    undef BDE_BUILD_TARGET_MT
#    define BDE_BUILD_TARGET_NO_MT
#  else
#    define BDE_BUILD_TARGET_MT
#    if defined(BDE_BUILD_TARGET_NO_MT)
#      undef BDE_BUILD_TARGET_NO_MT
#    endif
#  endif
#endif

#include <bsls_buildtarget.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The component under test defines two macros, 'BDE_BUILD_TARGET_EXC' and
// 'BDE_BUILD_TARGET_MT', that enforce uniform settings for exception support
// and multi-threading support across all translation units in a program.
// These macros operate by sabotaging the link phase of the build process
// unless the macros are defined identically in all translation units.
//
// In order to test this component properly, we would have to arrange to link
// the test driver from two object files, each with a different setting for one
// of the macros.  Then we would have to observe that the linker fails.
// Neither action is in the scope of a test driver...it requires a
// meta-test-driver test facility that can observe the compilation process.
//
// The solution (a la 'bslstl_map' by Chen He) is to build a test driver that
// *does* link properly if the two macros match the settings with which
// 'bsls_buildtarget.cpp' were built and then ask the user to manually modify
// the code so that the test driver compiles with different settings from
// 'bsls_buildtarget.cpp', and observe that the test driver fails to link.
//
// This mechanism can be enforced by creating above-the-line test cases for
// each macro which always fail if the corresponding BDE_BUILDTARGET_TEST_*
// macro is defined.  Therefore there are two possible states if one of the
// macros is defined: the test fails to build (good), or the test builds and
// fails (bad).
//-----------------------------------------------------------------------------
// [ 4] BDE_BUILD_TARGET_MT
// [ 3] BDE_BUILD_TARGET_EXC
// [ 2] bsls::BuildTargetMt::s_isBuildTargetMt
// [ 1] bsls::BuildTargetExc::s_isBuildTargetExc
//-----------------------------------------------------------------------------

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MACRO BDE_BUILD_TARGET_MT
        //
        // Concerns:
        //  1 A program should not link when 'BDE_BUILD_TARGET_MT' is defined
        //    in one translation unit and not defined in another.
        //
        //  2 A program should link when 'BDE_BUILD_TARGET_MT' is defined in
        //    all translation units or not defined in all translation units.
        //
        // Plan:
        //  1 Define a macro, 'BDE_BUILDTARGET_TEST_MT' that, when defined,
        //    will invert the definitions of 'BDE_BUILD_TARGET_MT' and
        //    'BDE_BUILD_TARGET_NO_MT' in this translation unit.  Build with
        //    '-D BDE_BUILD_TARGET_NO_MT' specified on the command line and
        //    observe that 'bsls_buildtarget.t.cpp' fails to link.  Perform a
        //    negative test of this condition by forcing an assertion failure
        //    if 'BDE_BUILD_TARGET_NO_MT' is defined, and allowing the test to
        //    pass otherwise. (C-1, C-2)
        //
        // Testing:
        //   BDE_BUILDTARGET_TEST_MT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting BDE_BUILD_TARGET_MT"
                            "\n===========================\n");

#ifdef BDE_BUILDTARGET_TEST_MT
        ASSERT(0 == "bsls_buildtarget.t.cpp should not build "
                    "with BDE_BUILDTARGET_TEST_MT");
#endif

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MACRO BDE_BUILD_TARGET_EXC
        //
        // Concerns:
        //  1 A program should not link when 'BDE_BUILD_TARGET_EXC' is
        //    defined in one translation unit and not defined in another.
        //
        //  2 A program should link when 'BDE_BUILD_TARGET_EXC' is defined in
        //    all translation units or not defined in all translation units.
        //
        // Plan:
        //  1 Define a macro, 'BDE_BUILDTARGET_TEST_EXC' that, when defined,
        //    will invert the definitions of 'BDE_BUILD_TARGET_EXC' and
        //    'BDE_BUILD_TARGET_NO_EXC' in this translation unit.  Build with
        //    '-D BDE_BUILD_TARGET_NO_EXC' specified on the command line and
        //    observe that 'bsls_buildtarget.t.cpp' fails to link.  Perform a
        //    negative test of this condition by forcing an assertion failure
        //    if 'BDE_BUILD_TARGET_NO_EXC' is defined, and allowing the test to
        //    pass otherwise. (C-1, C-2)
        //
        // Testing:
        //   BDE_BUILDTARGET_TEST_EXC
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting BDE_BUILD_TARGET_EXC"
                            "\n============================\n");

#ifdef BDE_BUILDTARGET_TEST_EXC
        ASSERT(0 == "bsls_buildtarget.t.cpp should not build "
                    "with BDE_BUILDTARGET_TEST_EXC");
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DATA MEMBER BuildTargetMt::s_isBuildTargetMt
        //
        // Concerns:
        //  1 'bsls::BuildTargetMt::s_isBuildTargetMt' should be defined.
        //
        //  2 'bsls::BuildTargetMt::s_isBuildTargetMt == 1' if and only if
        //    'BDE_BUILD_TARGET_MT' is defined.
        //
        // Plan:
        //  1 Manually test the value of
        //    'bsls::BuildTargetMt::s_isBuildTargetMt'. (C-1, C-2)
        //
        // Testing:
        //   bsls::BuildTargetMt::s_isBuildTargetMt
        // --------------------------------------------------------------------

        if (verbose)
                  printf("\nTesting bsls::BuildTargetMt::s_isBuildTargetMt"
                         "\n==============================================\n");

#ifdef BDE_BUILD_TARGET_MT
        ASSERT(1 == bsls::BuildTargetMt::s_isBuildTargetMt);
#else
        ASSERT(0 == bsls::BuildTargetMt::s_isBuildTargetMt);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // DATA MEMBER BuildTargetMt::s_isBuildTargetExc
        //
        // Concerns:
        //  1 'bsls::BuildTargetMt::s_isBuildTargetExc' should be defined.
        //
        //  2 'bsls::BuildTargetMt::s_isBuildTargetExc == 1' if and only if
        //    'BDE_BUILD_TARGET_EXC' is defined.
        //
        // Plan:
        //  1 Manually test the value of
        //    'bsls::BuildTargetMt::s_isBuildTargetExc'. (C-1, C-2)
        //
        // Testing:
        //   bsls::BuildTargetExc::s_isBuildTargetExc
        // --------------------------------------------------------------------

        if (verbose)
                 printf("\nTesting bsls::BuildTargetMt::s_isBuildTargetExc"
                        "\n===============================================\n");

#ifdef BDE_BUILD_TARGET_EXC
        ASSERT(1 == bsls::BuildTargetExc::s_isBuildTargetExc);
#else
        ASSERT(0 == bsls::BuildTargetExc::s_isBuildTargetExc);
#endif

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
// Copyright (C) 2013 Bloomberg L.P.
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
