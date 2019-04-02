// bsla_used.t.cpp                                                    -*-C++-*-
#include <bsla_used.h>

#include <bsla_unused.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // 'calloc', 'realloc', 'atoi'
#include <string.h>  // 'strcmp'

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// Build and run test case '1' with 'verbose' to see if 'BSLA_USED_IS_ACTIVE'
// and 'BSLA_UNUSED_IS_ACTIVE' are set.  If both are set, build and run in the
// debugger and observe that the unused static variable marked 'BSLA_USED' is
// accessible from within 'main', and that a breakpoint can be put in the
// unused static function marked 'BSLA_USED'.
//
// At this time, g++ always emits unused static functions and variables whether
// they are marked 'BSLA_USED' or not.  Clang, however, will omit unused static
// variables and functions unless they are marked 'BSLA_USED'.
//..
//  Annotation                            Result
//  ------------------------------------  --------------------
//  BSLA_USED                             Emission of entities
//..
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// ----------------------------------------------------------------------------

namespace bsls = BloombergLP::bsls;

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

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if BSLA_UNUSED_IS_ACTIVE && BSLA_USED_IS_ACTIVE

//
///Usage
///-----
//
///Example 1: Unused variables:
///- - - - - - - - - - - - - -
// First, we declare two unused static variables, one marked 'BSLA_UNUSED'
// and the other marked 'BSLA_USED'.
//..
    static
    int usage_UNUSED_variable_no_warning BSLA_UNUSED;

    static
    int usage_USED_variable_no_warning BSLA_USED;
//..
// Finally, if we compile with clang and go into the debugger and stop in
// 'main' which is in the same file and from which both variables are visible,
// we observe that the variable marked 'BSLA_UNUSED' cannot be accessed, but
// the the variable marked 'BSLA_USED' can.
//..
//
///Example 2: Unused functions:
///- - - - - - - - - - - - - -
// First declare two unused static functions, one marked 'BSLA_UNUSED' and one
// marked 'BSLA_USED':
//..
    static
    void usage_UNUSED_function_no_warning(int woof) BSLA_UNUSED;
        // Print the specified 'woof'.
    static
    void usage_UNUSED_function_no_warning(int woof)
    {
        printf("%d\n", woof);
    }

    static
    void usage_USED_function_no_warning(int woof) BSLA_USED;
        // Print the specified 'woof'.
    static
    void usage_USED_function_no_warning(int woof)
    {
        printf("%d\n", woof);
    }
//..
// Finally, if we compile with clang and go into the debugger, we find that
// we can put a breakpoint in the function marked 'BSLA_USED', but not in
// the function marked 'BSLA_UNUSED'.

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\nprintFlags: bsls_annotation Macros\n");

    printf("\nBSLA_USED: ");
#ifdef BSLA_USED
    printf("%s\n", STRINGIFY(BSLA_USED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_UNUSED_IS_ACTIVE);
    P(BSLA_USED_IS_ACTIVE);

    printf("\n\n---------------------------------------------\n");
    printf(    "printFlags: bsls_annotation Referenced Macros\n");

    printf("\nBSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning

    printf( "TEST %s CASE %d\n", __FILE__, test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 This test driver does *not* build when the 'U_TRIGGER_ERRORS'
        //:   preprocessor variable is defined to 1 and all expected output
        //:   appears.
        //:
        //: 2 This test driver builds with all expected compiler warning
        //:   messages and no unexpected warnings when the 'U_TRIGGER_WARNINGS'
        //:   preprocessor variable is defined to 1.
        //:
        //: 3 When 'U_TRIGGER_WARNINGS' and 'U_TRIGGER_ERRORS' are both defined
        //:   to 0, the compile is successful and with no warnings.
        //
        // Plan:
        //: 1 Build with 'U_TRIGGER_ERRORS' defined to and externally confirm
        //:   that compilation of this task failed and the compiler output
        //:   shows the expected message.  (C-1)
        //:
        //: 2 Build with 'U_TRIGGER_WARNINGS' defined to and externally examine
        //:   compiler output for expected warnings and the absence of warnings
        //:   expected to be suppressed.  (C-2)
        //:
        //: 3 Build with 'U_TRIGGER_ERRORS' and 'U_TRIGGER_WARNINGS' both
        //:   defined to 0 and observe that the compile is successful with no
        //:   warnings.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
            printf("\nThere are no run-time tests for this component."
                   "\nManually run build-time tests using a conforming "
                   "compiler.");

            if (!veryVeryVerbose) printFlags();

            ASSERT(true); // remove unused warning for 'aSsErT'
        }

      } break;
      default: {
        fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
