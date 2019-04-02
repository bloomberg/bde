// bsla_scanf.t.cpp                                                   -*-C++-*-
#include <bsla_scanf.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <stdarg.h>  // varargs
#include <stdio.h>
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'strcmp'

// Set this preprocessor variable to 1 to enable compile warnings being
// generated, 0 to disable them.

#define U_TRIGGER_WARNINGS 0

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This test driver serves as a framework for manually checking the annotations
// (macros) defined in this component.  The tester must repeatedly rebuild this
// task using a compliant compiler, each time defining different values of the
// boolean 'U_TRIGGER_WARNINGS' preprocessor variable.  In each case, the
// concerns are:
//
//: o Did the build succeed or not?
//:
//: o Was the expected warning observed, or not?
//:
//: o Was the expected suppression of some warning, suppressed or not?
//:
//: o For annotations taking arguments, do the results show if the arguments
//:   were properly passed to the underlying compiler directives?
//
// The single run-time "test" provided by this test driver, the BREATHING TEST,
// does nothing other than print out the values of the macros in verbose mode.
//
// The controlling preprocessor variable is 'U_TRIGGER_WARNINGS' which, if set
// to 1, provoke all the compiler warnings caused by the macros under test.  If
// set to 0, prevent any warnings from happening.
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
//  BSLA_SCANF(FMTIDX, STARTIDX)          Warning
//..
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
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
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

//
///Usage
///-----
//
///Example 1: Rand Populator:
///- - - - - - - - - - - - -
// Suppose we want to have a function that will populate a list of 'int's and
// 'float' will random numbers in the range '[ 0 .. 100 )'.
//
// First, we define our function:
//..
    int populateValues(const char *format, ...) BSLA_SCANF(1, 2);
        // Use the specified 'randGen' to populate 'int's and 'float's, passed
        // by pointer after the specified 'format', which will specify the
        // types of the variables passed.  Return the number of variables
        // populated, or -1 if the format string is invalid.
//
    int populateValues(const char *format, ...)
    {
        int ret = 0;
//
        va_list ap;
        va_start(ap, format);
//
        for (const char *pc = format; *pc; ++pc) {
            if ('%' != *pc) {
                continue;
            }
            const char c = *++pc;
            if ('%' == c) {
                continue;
            }
            else if ('d' == c) {
                * va_arg(ap, int *)   = static_cast<unsigned>(rand()) % 100;
            }
            else if ('f' == c || 'e' == c || 'g' == c) {
                const int characteristic = static_cast<unsigned>(rand()) % 100;
                const int mantissa = static_cast<unsigned>(rand()) % 1000;

                * va_arg(ap, float *) = static_cast<float>(characteristic +
                                                            mantissa / 1000.0);
            }
            else {
                // Unrecognized character.  Return a negative value.

                ret = -1;
                break;
            }
//
            ++ret;
        }
//
        va_end(ap);
//
        return ret;
    }
//..

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

void test_SCANF(const char *, ...) BSLA_SCANF(1, 2);
void test_SCANF(const char *, ...)
{
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

void use_without_diagnostic_message_SCANF()
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s",  buffer);
    test_SCANF("%d",  &i);
    test_SCANF("%lf", &d);
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_SCANF()
{
    char   buffer[20];
    int    i;

    test_SCANF("%s", &i);
    test_SCANF("%d", buffer);
    test_SCANF("%f", buffer);
}

#endif

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

    printf("\nBSLA_SCANF(fmt, arg): ");
#ifdef BSLA_SCANF
    printf("%s\n", STRINGIFY(BSLA_SCANF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_SCANF_IS_ACTIVE);

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

    printf("\nBSLS_PLATFORM_CMP_HP: ");
#ifdef BSLS_PLATFORM_CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 That the usage example builds and performs as expected.
        //
        // Plan:
        //: 1 Build and test the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Tnen, in 'main', we call 'populateValues' properly:
//..
    float ff[3] = { 0, 0, 0 };
    int   ii[3] = { 0, 0, 0 };
//
    int numVars = populateValues("%d %g %g %d %d %g",
                               &ii[0], &ff[0], &ff[1], &ii[1], &ii[2], &ff[2]);
    ASSERT(6 == numVars);
    for (int jj = 0; jj < 3; ++jj) {
        ASSERT(0 <= ii[jj]);
        ASSERT(0 <= ff[jj]);
        ASSERT(     ii[jj] < 100);
        ASSERT(     ff[jj] < 100);
    }
if (verbose) {
    printf("%d %g %g %d %d %g\n", ii[0], ff[0], ff[1], ii[1], ii[2], ff[2]);
}
//..
// Next, we observe that there are no compiler warnings and a reasonable set of
// random numbers are output:
//..
//  83 86.777 15.793 35 86 92.649
//..
// Now, we make a call where the arguments don't match the format string:
//..
#if U_TRIGGER_WARNINGS
    numVars = populateValues("%d %g", &ff[0], &ii[0]);
#endif
//..
// Finally, we observe the following compiler warnings on clang:
//..
//  .../bsla_scanf.t.cpp:351:39: warning: format specifies type 'int *' but the
//   argument has type 'float *' [-Wformat]
//      numVars = populateValues("%d %g", &ff[0], &ii[0]);
//                                ~~      ^~~~~~
//                                %f
//  .../bsla_scanf.t.cpp:351:47: warning: format specifies type 'float *' but t
//  he argument has type 'int *' [-Wformat]
//      numVars = populateValues("%d %g", &ff[0], &ii[0]);
//                                   ~~           ^~~~~~
//                                   %d
//..

    (void) numVars;
      } break;
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
        //: 1 Build with 'U_TRIGGER_WARNINGS' defined to 1 and externally
        //:   examine compiler output for expected warnings and the absence of
        //:   warnings expected to be suppressed.  (C-1)
        //:
        //: 2 Build with 'U_TRIGGER_WARNINGS' defined to 0 and observe that the
        //:   compile is successful with no warnings.  (C-2)
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
