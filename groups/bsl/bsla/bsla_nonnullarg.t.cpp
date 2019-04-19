// bsla_nonnullarg.t.cpp                                              -*-C++-*-
#include <bsla_nonnullarg.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // 'atoi'
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
//  BSLA_NONNULLARG(...)                  warning
//  BSLA_NONNULLARGS                      warning
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
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates indended use of this component.
//
///Example 1: Passing Null to Arguments Annotated as Non-Null
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function, 'usagePrint1', annotated such that a compiler
// warning will occur if the first argument of the annotated function is passed
// 0, 'NULL', 'nullptr', or (on clang) a null pointer constant expression:
//..
    void usagePrint1(const char *string, int repetition) BSLA_NONNULLARG(1);
        // Print the specified 'string' the specified 'repetition' times.
//
    void usagePrint1(const char *string, int repetition)
    {
        for (int ii = 0; ii < repetition; ++ii) {
            printf("%s\n", string);
        }
    }
//..
// Then, we define a nearly identical function annotated with
// 'BSLA_NONNULLARGS' instead.  Note that only pointer arguments are affected
// by this annotation -- 'repetition' is not affected and may be passed 0
// without a warning being emitted:
//..
    void usagePrint2(const char *string, int repetition) BSLA_NONNULLARGS;
        // Print the specified 'string' the specified 'repetition' times.
//
    void usagePrint2(const char *string, int repetition)
    {
        for (int ii = 0; ii < repetition; ++ii) {
            printf("%s\n", string);
        }
    }
//..
// So the two different annotations on these functions have an identical
// effect -- affecting the 'string' argument but not the 'repetition' argument.
//

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

char test_NONNULLARG_1(const void *p, const void *q, const void *r)
                                                            BSLA_NONNULLARG(1);
    // Dereference the specified 'p' and return the result, ignoring the
    // specified 'q' and 'r'.
char test_NONNULLARG_1(const void *p, const void *q, const void *r)
{
    char c = *reinterpret_cast<const char *>(p);
    (void) q;
    (void) r;

    return c;
}

int test_NONNULLARG_2_3(const void *p, const void *q, const void *r)
                                                         BSLA_NONNULLARG(2, 3);
    // Ignore the specified 'p' and return the sum of the chars pointed at by
    // the specified 'q' and 'r'.

int test_NONNULLARG_2_3(const void *p, const void *q, const void *r)
{
    (void) p;
    return *static_cast<const char *>(q) + *static_cast<const char *>(r);
}

int test_NONNULLARGS(void *p, void *q) BSLA_NONNULLARGS;
    // Return the sum of the characters pointed at by the specified 'p' and
    // 'q'.

int test_NONNULLARGS(void *p, void *q)
{
    return *static_cast<const char *>(p) + *static_cast<const char *>(q);
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

void use_without_diagnostic_message_NONNULLARG_1()
    // Call 'test_NONNULLARG_1' without provoking a warning.
{
    test_NONNULLARG_1("", NULL, NULL);
}

void use_without_diagnostic_message_ARG2_NONNULL()
    // Call 'test_NONNULLARG_2_3' without provoking a warning.
{
    char buffer2[2] = { 'a', 0 };
    int ret = test_NONNULLARG_2_3(NULL, buffer2, buffer2 + 1);
    ASSERT('a' == ret);
}

void use_without_diagnostic_message_NONNULLARGS()
    // Call 'test_NONNULLARGS' without provoking a warning.
{
    char buffer1[2];
    char buffer2[2];

    test_NONNULLARGS(buffer1, buffer2);
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------
                                                                              \
#if U_TRIGGER_WARNINGS

void use_with_warning_message_NONNULLARG_1()
    // Call 'test_NONNULLARG_1' several times, each time provoking a warning.
{
    test_NONNULLARG_1(0   , NULL, NULL);
    test_NONNULLARG_1(NULL, NULL, NULL);
    test_NONNULLARG_1(NULL, "", "");
    test_NONNULLARG_1(NULL, "", NULL);
    test_NONNULLARG_1(NULL, NULL, "");
}

void use_with_warning_message_NONNULLARG_2_3_NONNULL()
    // Call 'test_NONNULLARG_2_3' several times, each time provoking a warning.
{
    test_NONNULLARG_2_3("", "", NULL);
    test_NONNULLARG_2_3("", NULL,"");
    test_NONNULLARG_2_3("", NULL, NULL);
    test_NONNULLARG_2_3(NULL, NULL, NULL);
}

void use_with_warning_message_NONNULLARGS()
    // Call 'test_NONNULLARGS' several times, each time provoking a warning.
{
    char buffer1[2];
    char buffer2[2];

    test_NONNULLARGS(NULL, buffer2);
    test_NONNULLARGS(buffer1, NULL);
    test_NONNULLARGS(NULL, NULL);
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

    printf("\nBSLA_NONNULLARGS: ");
#ifdef BSLA_NONNULLARGS
    printf("%s\n", STRINGIFY(BSLA_NONNULLARGS) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NONNULLARG(...): ");
#ifdef BSLA_NONNULLARG
    printf("%s\n", STRINGIFY(BSLA_NONNULLARG(...)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_NONNULLARGS_IS_ACTIVE);
    P(BSLA_NONNULLARG_IS_ACTIVE);

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

// Next, in 'main', we call both functions with a non-null first argument, and
// observe that no warning occurs.  Note that even though 0 is passed to the
// integer argument to 'usagePrint2' and the 'BSLA_NONNULLARGS' annotation was
// used, non-pointer arguments are not affected by that annotation:
//..
        usagePrint1("woof", 0);
        usagePrint2("meow", 0);
//..
// Then, we call both functions passing the first argument a variable whose
// value is known by the compiler to be null, but since 'np1' is a non-'const'
// variable, no warning is issued:
//..
        char *np1 = NULL;
        usagePrint1(np1,    0);
        usagePrint2(np1,    0);
//..
// Now, we call both functions passing various forms of constant null pointer
// expressions to the first argument:
//..
#if U_TRIGGER_WARNINGS
        usagePrint1(   0, -10);
        usagePrint2(   0, -10);

        usagePrint1(NULL, -20);
        usagePrint2(NULL, -20);

        usagePrint1(static_cast<char *>(0), -30);
        usagePrint2(static_cast<char *>(0), -30);

        #if __cplusplus >= 201103L
            usagePrint1(nullptr, -40);
            usagePrint2(nullptr, -40);
        #endif

        char * const np2 = 0;   // 'np2', unlike 'np1' above, is 'const'.
        usagePrint1(np2, -50);    // Warning with clang, not g++
        usagePrint2(np2, -50);    // Warning with clang, not g++
#endif
//..
// Finally, we observe that the above calls result in the following warnings
// with clang w/C++11 support:
//..
//  .../bsla_nonnullarg.t.cpp:376:30: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint1(   0, -10);
//                     ~     ^
//  .../bsla_nonnullarg.t.cpp:377:30: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint2(   0, -10);
//                     ~     ^
//  .../bsla_nonnullarg.t.cpp:379:30: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint1(NULL, -20);
//                  ~~~~     ^
//  .../bsla_nonnullarg.t.cpp:380:30: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint2(NULL, -20);
//                  ~~~~     ^
//  .../bsla_nonnullarg.t.cpp:382:48: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint1(static_cast<char *>(0), -30);
//                  ~~~~~~~~~~~~~~~~~~~~~~     ^
//  .../bsla_nonnullarg.t.cpp:383:48: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint2(static_cast<char *>(0), -30);
//                  ~~~~~~~~~~~~~~~~~~~~~~     ^
//  .../bsla_nonnullarg.t.cpp:386:37: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//          usagePrint1(nullptr, -40);
//                      ~~~~~~~     ^
//  .../bsla_nonnullarg.t.cpp:387:37: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//          usagePrint2(nullptr, -40);
//                      ~~~~~~~     ^
//  .../bsla_nonnullarg.t.cpp:391:29: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint1(np2, -50);    // Warning with clang, not g++
//                  ~~~     ^
//  .../bsla_nonnullarg.t.cpp:392:29: warning: null passed to a callee that
//  requires a non-null argument [-Wnonnull]
//      usagePrint2(np2, -50);    // Warning with clang, not g++
//                  ~~~     ^
//..
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
        //: 1 Build with 'U_TRIGGER_WARNINGS' defined to and externally examine
        //:   compiler output for expected warnings and the absence of warnings
        //:   expected to be suppressed.  (C-1)
        //:
        //: 2 Build with 'U_TRIGGER_WARNINGS' defined to 0 and observe that the
        //:   compile is successful with no warnings.
        //:
        //: 3 Run with 'verbose' to get a listing of macro expansions.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        if (verbose) {
            printf("There are no run-time tests for this component.\n"
                   "Manually run build-time tests using a conforming "
                   "compiler.\n");

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
