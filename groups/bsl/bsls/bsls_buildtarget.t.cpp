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

#include <bsls_buildtarget.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // `printf`, `fprintf`, `setbuf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The component under test defines one type name, `bsls::BuildTargetExc`,
// that is used to enforce uniform settings for exception support across all
// translation units in a program.  In each translation unit (TU),
// `bsls::BuildTargetExc` is associated with a certain symbol having external
// linkage according to whether the TU was built with at most one of
// `-DBDE_BUILD_TARGET_EXC` and `-DBDE_BUILD_TARGET_NO_EXC` (perhaps neither).
// The various `DBDE_BUILD_TARGET*` macros operate by sabotaging the link phase
// of the build process unless the macros are defined identically for the
// compile phase of all translation units.
//
// In order to test this component properly, we would have to arrange to link
// the test driver from two object files, each with a different setting for one
// of the macros.  Then we would have to observe that the linker fails.
// Neither action is in the scope of a test driver; it requires a
// meta-test-driver test facility that can observe the compilation process.
//
// The solution (a la `bslstl_map` by Chen He) is to build a test driver that
// *does* link properly if the two macros match the settings with which
// `bsls_buildtarget.cpp` was built.  Then ask the user to manually modify
// the code so that the test driver is compiled with different settings from
// `bsls_buildtarget.cpp`, and observe that the test driver fails to link.
//
// This mechanism can be enforced by creating above-the-line test cases for
// each macro which always fail if the corresponding `BDE_BUILDTARGET_TEST_*`
// macro is defined.  Therefore, there are two possible states if one of the
// macros is defined: the test fails to build (good), or the test builds and
// fails (bad).
//-----------------------------------------------------------------------------
// [ 3] BDE_BUILD_TARGET_MT
// [ 2] BDE_BUILD_TARGET_EXC
// [ 1] bsls::BuildTargetExc::s_isBuildTargetExc
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        fflush(stdout);
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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Print a diagnostic message to standard output about a macro (flag) of
/// the specified `name`.  The specified `value` is a null pointer in case
/// the macro with `name` has not been defined, and a pointer to a C-string
/// in case the macro is defined.  Not that the `value` may be an empty
/// string in case it has been defined with no value.
static void printMacroFlag(const char *name, const char *value)
{
    const char *pv = value ? value[0] ? value : "<:none:>" : "<:UNDEFINED:>";
    printf("\n%s: %s\n", name, pv);
}

/// Print a diagnostic message to standard output if any of the preprocessor
/// flags of interest are defined, and their value if a value had been set.
/// An "Enter" and "Leave" message is printed unconditionally so there is
/// some report even if all of the flags are undefined.
static void printFlags()
{
#define u_STR2(...) "" #__VA_ARGS__
#define u_STR(...) u_STR2(__VA_ARGS__)

#define u_PRINT_UNDEF(a_NAME) printMacroFlag(#a_NAME, 0)
#define u_PRINT_VALUE(a_NAME) printMacroFlag(#a_NAME, u_STR(a_NAME))

    puts("printFlags: Enter");

    puts("\n==printFlags: bsls_buildtarget Macros==");

#ifdef BDE_BUILD_TARGET_EXC
    u_PRINT_VALUE(BDE_BUILD_TARGET_EXC);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_EXC);
#endif

#ifdef BDE_BUILD_TARGET_NO_EXC
    u_PRINT_VALUE(BDE_BUILD_TARGET_NO_EXC);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_NO_EXC);
#endif

#ifdef BDE_BUILD_TARGET_MT
    u_PRINT_VALUE(BDE_BUILD_TARGET_MT);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_MT);
#endif

#ifdef BDE_BUILD_SKIP_VERSION_CHECKS
    u_PRINT_VALUE(BDE_BUILD_SKIP_VERSION_CHECKS);
#else
    u_PRINT_UNDEF(BDE_BUILD_SKIP_VERSION_CHECKS);
#endif

#ifdef BDE_OMIT_DEPRECATED
    u_PRINT_VALUE(BDE_OMIT_DEPRECATED);
#else
    u_PRINT_UNDEF(BDE_OMIT_DEPRECATED);
#endif

#ifdef BDE_OMIT_INTERNAL_DEPRECATED
    u_PRINT_VALUE(BDE_OMIT_INTERNAL_DEPRECATED);
#else
    u_PRINT_UNDEF(BDE_OMIT_INTERNAL_DEPRECATED);
#endif

#ifdef BDE_OPENSOURCE_PUBLICATION
    u_PRINT_VALUE(BDE_OPENSOURCE_PUBLICATION);
#else
    u_PRINT_UNDEF(BDE_OPENSOURCE_PUBLICATION);
#endif

#ifdef BDE_BUILD_TARGET_32
    u_PRINT_VALUE(BDE_BUILD_TARGET_32);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_32);
#endif

#ifdef BDE_BUILD_TARGET_64
    u_PRINT_VALUE(BDE_BUILD_TARGET_64);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_64);
#endif

#ifdef BDE_BUILD_TARGET_OPT
    u_PRINT_VALUE(BDE_BUILD_TARGET_OPT);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_OPT);
#endif

#ifdef BDE_BUILD_TARGET_SAFE
    u_PRINT_VALUE(BDE_BUILD_TARGET_SAFE);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_SAFE);
#endif

#ifdef BDE_BUILD_TARGET_SAFE_2
    u_PRINT_VALUE(BDE_BUILD_TARGET_SAFE_2);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_SAFE_2);
#endif

#ifdef BDE_BUILD_TARGET_ASAN
    u_PRINT_VALUE(BDE_BUILD_TARGET_ASAN);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_ASAN);
#endif

#ifdef BDE_BUILD_TARGET_MSAN
    u_PRINT_VALUE(BDE_BUILD_TARGET_MSAN);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_MSAN);
#endif

#ifdef BDE_BUILD_TARGET_TSAN
    u_PRINT_VALUE(BDE_BUILD_TARGET_TSAN);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_TSAN);
#endif

#ifdef BDE_BUILD_TARGET_UBSAN
    u_PRINT_VALUE(BDE_BUILD_TARGET_UBSAN);
#else
    u_PRINT_UNDEF(BDE_BUILD_TARGET_UBSAN);
#endif

    puts("\nprintFlags: Leave\n\n");
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;  (void)veryVerbose;
    bool veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output.

    printf("TEST " __FILE__ " CASE %d\n", test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

// There is no usage example for this component since it is not meant for
// direct client use.
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MACRO BDE_BUILD_TARGET_MT
        //
        // Concerns:
        // 1. The macro `BDE_BUILD_TARGET_MT` is always defined.
        //
        // Plan:
        // 1. Confirm that the macro is defined during preprocessing with the
        //    `defined` preprocessor operator.
        //
        // Testing:
        //   BDE_BUILD_TARGET_MT
        // --------------------------------------------------------------------

        if (verbose) puts("\nMACRO BDE_BUILD_TARGET_MT"
                          "\n=========================");

#if !defined(BDE_BUILD_TARGET_MT)
        ASSERT(0 == "`BDE_BUILD_TARGET_MT` is not defined");
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MACRO BDE_BUILD_TARGET_EXC
        //
        // Concerns:
        // 1. A program should not link when `BDE_BUILD_TARGET_EXC` is
        //    defined in one translation unit and not defined in another.
        //
        // 2. A program should link when `BDE_BUILD_TARGET_EXC` is defined in
        //    all translation units or not defined in all translation units.
        //
        // Plan:
        // 1. Define a macro, `BDE_BUILDTARGET_TEST_EXC`, that when defined,
        //    will invert the definitions of `BDE_BUILD_TARGET_EXC` and
        //    `BDE_BUILD_TARGET_NO_EXC` in this translation unit.  Build with
        //    `-D BDE_BUILD_TARGET_NO_EXC` specified on the command line and
        //    observe that `bsls_buildtarget.t.cpp` fails to link.  Perform a
        //    negative test of this condition by forcing an assertion failure
        //    if `BDE_BUILD_TARGET_NO_EXC` is defined, and allowing the test to
        //    pass otherwise. (C-1..2)
        //
        // Testing:
        //   BDE_BUILD_TARGET_EXC
        // --------------------------------------------------------------------

        if (verbose) puts("\nMACRO BDE_BUILD_TARGET_EXC"
                          "\n==========================");

#ifdef BDE_BUILDTARGET_TEST_EXC
        ASSERT(0 == "bsls_buildtarget.t.cpp should not build "
                    "with BDE_BUILDTARGET_TEST_EXC");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // `BuildTargetExc::s_isBuildTargetExc` CONSTANT
        //
        // Concerns:
        // 1. `bsls::BuildTargetExc::s_isBuildTargetExc` should be defined.
        //
        // 2. `bsls::BuildTargetExc::s_isBuildTargetExc == 1` if and only if
        //    `BDE_BUILD_TARGET_EXC` is defined.
        //
        // Plan:
        // 1. Manually test the value of
        //    `bsls::BuildTargetExc::s_isBuildTargetExc`. (C-1..2)
        //
        // Testing:
        //   bsls::BuildTargetExc::s_isBuildTargetExc
        // --------------------------------------------------------------------

        if (verbose) puts("\n'BuildTargetExc::s_isBuildTargetExc' CONSTANT"
                          "\n=============================================");

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
        printFlags();
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
