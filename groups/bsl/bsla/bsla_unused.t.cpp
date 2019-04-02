// bsla_unused.t.cpp                                                  -*-C++-*-
#include <bsla_unused.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <math.h>
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
//  ------------------------------------  -------
//  BSLA_UNUSED                           Warning
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
///Example 1: Unused Warnings:
/// - - - - - - - - - - - - -
// First, we define a namespace 'warn' within the unnamed namespace with a
// type, a function, and a function in it.  They are unused:
//..
    namespace {
    namespace warn {

    struct ResultRec {
        double d_x;
        double d_y;
    };

    double x;

    int quadratic(double *zeroA,
                  double *zeroB,
                  double a,
                  double b,
                  double c)
        // Solve the quadratic function for the specified 'a', 'b', and 'c',
        // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
        // return a non-zero value, and set the specified '*zeroA' and '*zeroB'
        // to those solutions and return 0 otherwise.
    {
        const double discriminant = b * b - 4 * a * c;
        if (discriminant < 0 || 0.0 == a) {
            *zeroA = *zeroB = 0.0;
            return -1;                                                // RETURN
        }

        const double root = ::sqrt(discriminant);
        *zeroA = (-b + root) / (2 * a);
        *zeroB = (-b - root) / (2 * a);

        return 0;
    }

    }  // close namespace warn
    }  // close unnamed namespace
//..
// Then, we observe the warnings:
//..
//  .../bsla_unused.t.cpp:130:12: warning: '{anonymous}::warn::x' defined but n
//  ot used [-Wunused-variable]
//       double x;
//              ^
//  .../bsla_unused.t.cpp:132:9: warning: 'int {anonymous}::warn::quadratic(dou
//  ble*, double*, double, double, double)' defined but not used [-Wunused-func
//  tion]
//       int quadratic(double *zeroA,
//           ^
//..
// Note that none of the compilers we are currently using issue a warning on
// the unused 'warn::ResultRec', but some in the future might.
//
// Next, we define a namespace 'nowarn' within the unused namespace with
// exactly the same unused entities, using the 'BSLA_UNUSED' annotation to
// silence the warnings:
//..
    namespace {
    namespace nowarn {

    struct ResultRec {
        double d_x;
        double d_y;
    } BSLA_UNUSED;

    double x BSLA_UNUSED;

    int quadratic(double *zeroA,
                  double *zeroB,
                  double a,
                  double b,
                  double c) BSLA_UNUSED;
        // Solve the quadratic function for the specified 'a', 'b', and 'c',
        // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
        // return a non-zero value, and set the specified '*zeroA' and '*zeroB'
        // to those solutions and return 0 otherwise.

    int quadratic(double *zeroA,
                  double *zeroB,
                  double a,
                  double b,
                  double c)
    {
        const double discriminant = b * b - 4 * a * c;
        if (discriminant < 0 || 0.0 == a) {
            *zeroA = *zeroB = 0.0;
            return -1;                                                // RETURN
        }

        const double root = ::sqrt(discriminant);
        *zeroA = (-b + root) / (2 * a);
        *zeroB = (-b - root) / (2 * a);

        return 0;
    }

    }  // close namespace nowarn
    }  // close unnamed namespace
//..
// Finally, we observe that the warnings for the 'nowarn' namespace are
// suppressed.

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

static
void test_UNUSED_function_no_warning() BSLA_UNUSED;
void test_UNUSED_function_no_warning()
{
}

static
void test_UNUSED_function_warning();
void test_UNUSED_function_warning()
{
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

static
int test_UNUSED_variable_no_warning     BSLA_UNUSED;

static
int test_UNUSED_variable_warning;

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

namespace {

struct Test_UNUSED_type_no_warning {
    int d_d;
} BSLA_UNUSED;

struct Test_UNUSED_type_warning {
    int d_d;
};

}  // close unnamed namespace

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if !U_TRIGGER_WARNINGS

void stifle_unused_warnings()
{
    (void) Test_UNUSED_type_warning();

    (void) test_UNUSED_variable_warning;
    (void) test_UNUSED_function_warning();

# if !BSLA_UNUSED_IS_ACTIVE

    (void) Test_UNUSED_type_no_warning();

    (void) test_UNUSED_variable_no_warning;
    (void) test_UNUSED_function_no_warning();

# endif

}

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

#if U_TRIGGER_ERRORS

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

    printf("\nBSLA_UNUSED: ");
#ifdef BSLA_UNUSED
    printf("%s\n", STRINGIFY(BSLA_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_UNUSED_IS_ACTIVE);

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

#if !U_TRIGGER_WARNINGS
        {
            warn::ResultRec rr;

            int rc = warn::quadratic(&rr.d_x, &rr.d_y, 1, 2, 1);
            warn::x = rr.d_y;
            P_(rc);    P_(rr.d_x);    P_(rr.d_y);    P(warn::x);
            rc = warn::quadratic(&rr.d_x, &rr.d_y, 2, 2, 2);
            P_(rc);    P_(rr.d_x);    P(rr.d_y);
            rc = warn::quadratic(&rr.d_x, &rr.d_y, 2, 8, 2);
            P_(rc);    P_(rr.d_x);    P(rr.d_y);
        }
# if !BSLS_UNUSED_IS_ACTIVE
        {
            nowarn::ResultRec rr;

            int rc = nowarn::quadratic(&rr.d_x, &rr.d_y, 1, 2, 1);
            nowarn::x = rr.d_y;
            P_(rc);    P_(rr.d_x);    P_(rr.d_y);    P(nowarn::x);
            rc = nowarn::quadratic(&rr.d_x, &rr.d_y, 2, 2, 2);
            P_(rc);    P_(rr.d_x);    P(rr.d_y);
            rc = nowarn::quadratic(&rr.d_x, &rr.d_y, 2, 8, 2);
            P_(rc);    P_(rr.d_x);    P(rr.d_y);
        }
# endif
#endif
      } break;
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
