// bsla_fallthrough.t.cpp                                             -*-C++-*-
#include <bsla_fallthrough.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // 'calloc', 'realloc', 'atoi'
#include <string.h>  // 'strcmp'

// Set this preprocessor macro to 1 to enable compile warnings being generated,
// 0 to disable them.

#define U_TRIGGER_WARNINGS 0

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver serves as a framework for manually checking the annotations
// (macros) defined in this component.  The tester must repeatedly rebuild this
// test driver using a compliant compiler, each time defining different values
// of the boolean 'U_TRIGGER_WARNINGS' preprocessor macro.  In each case, the
// concerns are:
//
//: o Did the build succeed or not?
//:
//: o Was the expected warning observed or not?
//:
//: o Was the expected suppression of some warning suppressed or not?
//:
//: o For annotations taking arguments, do the results show if the arguments
//:   were properly passed to the underlying compiler directives?
//
// The single run-time "test" provided by this test driver, the BREATHING TEST,
// does nothing other than print out the values of the macros in verbose mode.
//
// The controlling preprocessor macro is 'U_TRIGGER_WARNINGS', which, if set to
// 1, provokes all the compiler warnings caused by the macros under test.  If
// set to 0, prevents any warnings from happening.
//
// The table below classifies each of the annotations provided by this
// component by the entities to which it can be applied (i.e., function,
// variable, and type) and the expected result (optimization, error, warning,
// conditional warning, absence of warning).  The tag(s) found in the
// right-most column appear as comments throughout this test driver.  They can
// be used as an aid to navigation to the test code for each annotation, and an
// aid to assuring test coverage.
//..
//  Annotation                            Result
//  ------------------------------------  --------
//  BSLA_FALLTHROUGH                      Warning
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
//                                GLOBALS
// ----------------------------------------------------------------------------

int             test;
bool         verbose;
bool     veryVerbose;
bool veryVeryVerbose;

// ============================================================================
//                                     USAGE
// ----------------------------------------------------------------------------

#if BSLA_FALLTHROUGH_IS_ACTIVE

///Usage
///-----
//
///Example 1: Suppressing Fall-Through Warnings in a 'switch' Statement
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function:
//..
    int usageFunction(int jj)
        // Demonstrate the usage of 'BSLA_FALLTHROUGH', read the specified
        // 'jj'.
    {
        for (int ii = 0; ii < 5; ++ii) {
//..
// Then, we have a 'switch' in the function:
//..
            switch (ii) {
              case 0: {
                printf("%d\n", jj - 3);
//..
// Next, we see that 'BSLA_FALLTHROUGH;' as the last statement in a 'case'
// block before falling through silences the fall-through warning from the
// compiler:
//..
                 BSLA_FALLTHROUGH;
              }
              case 1:
//..
// Then, we see this also works on 'case's that don't have a '{}' block:
//..
                jj -= 6;
                printf("%d\n", jj);
                BSLA_FALLTHROUGH;
              case 2: {
                if (jj > 4) {
                    printf("%d\n", jj + 10);
//..
// Next, we see that a 'BSLA_FALLTHROUGH;' works within an 'if' block, provided
// that it's in the last statement in the flow of control before falling
// through:
//..
                    BSLA_FALLTHROUGH;
                }
                else {
                    return 0;                                         // RETURN
                }
              }
              case 3: {
                if (jj > 4) {
                    continue;
                }
                else {
                    printf("%d\n", ++jj);
//..
// Now, we see that a 'BSLA_FALLTHROUGH;' can also occur as the last statement
// in an 'else' block:
//..
                    BSLA_FALLTHROUGH;
                }
              }
              default: {
                return 1;                                             // RETURN
              } break;
            }
        }
//
        return -7;
    }
//..
// Finally, we see that if we compile when 'BSLA_FALLTHROUGH_IS_ACTIVE' is set,
// the above compiles with no warnings.

#endif

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

int test_FALLTHROUGH_function(int i)
    // Test the 'BSLA_FALLTHROUGH' macro.
{
    switch (i) {
      case 0: {
        if (verbose) {
#if BSLA_FALLTHROUGH_IS_ACTIVE
            {{{{ BSLA_FALLTHROUGH; }}}}
#else
            return 3;                                                 // RETURN
#endif
        }
        else {
#if BSLA_FALLTHROUGH_IS_ACTIVE
            if (veryVerbose)
                return 8;                                             // RETURN
            else {
                BSLA_FALLTHROUGH;
            }
#else
            return 7;                                                 // RETURN
#endif
        }
      }
      case 1: {
        return 0;                                                     // RETURN
      }
      default: {
        return 1;                                                     // RETURN
      }
    }
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

int use_with_warning_message_FALLTHROUGH(int i)
{
    switch (i)
    {
      case 0: {
        return 0;                                                     // RETURN
      }
#if U_TRIGGER_WARNINGS
      case 1: {
        printf("%d\n", i * i + 1);
      }
      case 2: {
        if (verbose)) {
            printf("%d\n", i * i + 2);
        }
        else {
            return 7;                                                 // RETURN
        }
      }
      case 3: {
        if (verbose) {
            return 7;                                                 // RETURN
        }
        else {
            printf("%d\n", i * i + 3);
        }
      }
#endif
      default: {
        return 1;                                                     // RETURN
      }
    }
}

#endif

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

    printf("\nBSLA_FALLTHROUGH: ");
#ifdef BSLA_FALLTHROUGH
    printf("%s\n", STRINGIFY(BSLA_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif
    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_FALLTHROUGH_IS_ACTIVE);

    printf("\n\n---------------------------------------------\n");
    printf(    "printFlags: bsls_annotation Referenced Macros\n");

    printf("\nBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
    printf("%s\n",
              STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

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

    printf("\nBSLS_PLATFORM_CMP_VERSION: ");
#ifdef BSLS_PLATFORM_CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n__has_warning: ");
#ifdef __has_warning
    printf("DEFINED\n");
#else
    printf("UNDEFINED\n");
#endif

    printf("\n__has_warning: ");
#ifdef __has_warning
    printf("DEFINED\n");
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
        //: 1 This test driver builds with all expected compiler warning
        //:   messages and no unexpected warnings when the 'U_TRIGGER_WARNINGS'
        //:   preprocessor variable is defined to 1.
        //:
        //: 2 When 'U_TRIGGER_WARNINGS' is defined to 0, the compile is
        //:   successful and with no warnings.
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
