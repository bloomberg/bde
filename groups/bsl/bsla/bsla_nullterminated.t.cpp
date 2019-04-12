// bsla_nullterminated.t.cpp                                          -*-C++-*-
#include <bsla_nullterminated.h>

#include <bsls_bsltestutil.h>

#include <stdarg.h>
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
//  ------------------------------------  -------
//  BSLA_NULLTERMINATED                   Warning
//  BSLA_NULLTERMINATEDAT(ARG_IDX)        Warning
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
//                                    GLOBAL
// ----------------------------------------------------------------------------

int                 test;
bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

//
///Usage
///-----
//
///Example 1: 'catStrings' function
/// - - - - - - - - - - - - - - - -
// Suppose we want to have a function that, passed a variable length argument
// list of 'const char *' strings terminated by 'NULL', concatenates the
// strings, separated by spaces, into a buffer.
//
// First, we declare and define the function, annotated with
// 'BSLA_NULL_TERMINATED':
//..
    void catStrings(char *outputBuffer, ...) BSLA_NULLTERMINATED;
    void catStrings(char *outputBuffer, ...)
        // The specified 'outputBuffer' is a buffer where the output of this
        // function is placed.  The specified '...' is a 'NULL'-terminated list
        // of 'const char *' strings, which are to be copied into
        // 'outputBuffer', concatenated together and separated by spaces.  The
        // behavior is undefined unless the '...' is a 'NULL'-terminated list
        // of 'const char *' arguments.
    {
        *outputBuffer = 0;

        va_list ap;
        va_start(ap, outputBuffer);
        const char *next;
        for (bool first = 1; (next = va_arg(ap, const char *)); first = 0) {
            ::strcat(outputBuffer, first ? "" : " ");
            ::strcat(outputBuffer, next);
        }
        va_end(ap);
    }
//..

//
///Example 2: 'catVerdict' function
/// - - - - - - - - - - - - - - - -
// Suppose we want to have a function that, passed a variable length argument
// list of 'const char *' strings terminated by 'NULL', concatenates the
// strings, separated by spaces, into a buffer, and then there's an additional
// integer argument, interpreted as a boolean, that determines what is to be
// appended to the end of the buffer.
//
// First, we declare and define the function, annotated with
// 'BSLA_NULL_TERMINATEDAT(1)':
//..
    void catVerdict(char *outputBuffer, ...) BSLA_NULLTERMINATEDAT(1);
    void catVerdict(char *outputBuffer, ...)
        // The specified 'outputBuffer' is a buffer where output is to be
        // placed.  All but the last 2 of the specified '...' arguments are
        // 'const char *' strings to be concatenated together into
        // 'outputBuffer', separated by spaces.  The second-to-last argument is
        // to be 'NULL', and the last argument is an 'int' interpreted as a
        // boolean to determine whether the buffer is to end with a verdict of
        // "guilty" or "not guilty".  The behavior is undefined unless the
        // types of all the arguments are correct and the second to last
        // argument is 'NULL'.
    {
        *outputBuffer = 0;

        va_list ap;
        va_start(ap, outputBuffer);
        const char *next;
        for (bool first = 1; (next = va_arg(ap, const char *)); first = 0) {
            ::strcat(outputBuffer, first ? "" : " ");
            ::strcat(outputBuffer, next);
        }

        const bool guilty = va_arg(ap, int);
        ::strcat(outputBuffer, guilty ? ": guilty" : ": not guilty");
        va_end(ap);
    }
//..

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

void test_NULL_TERMINATED(void *, ...) BSLA_NULLTERMINATED;
void test_NULL_TERMINATED(void *, ...)
    // Do things.
{
}

void test_NULL_TERMINATED_AT2(void *, ...) BSLA_NULLTERMINATEDAT(2);
void test_NULL_TERMINATED_AT2(void *, ...)
    // Do things.
{
}

void test_NULL_TERMINATED_AT3(void *, ...) BSLA_NULLTERMINATEDAT(3);
void test_NULL_TERMINATED_AT3(void *, ...)
    // Do things.
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

void use_without_diagnostic_message_NULL_TERMINATED()
    // Call 'test_NULL_TERMINATED' with a properly null-terminated list of
    // pointers.
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    test_NULL_TERMINATED(buffer1, buffer2, buffer3, buffer4, NULL);
}

void use_without_diagnostic_message_NULL_TERMINATED_AT2()
    // Call 'test_NULL_TERMINATED_AT2' with a properly null-terminated list of
    // pointers.
{
    char buffer1[2];
    char buffer2[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT2(buffer1, buffer2, NULL, buffer4, buffer5);
}

void use_without_diagnostic_message_NULL_TERMINATED_AT3()
    // Call 'test_NULL_TERMINATED_AT3' with a properly null-terminated list of
    // pointers.
{
    char buffer1[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT3(buffer1, NULL, buffer3, buffer4, buffer5);
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_NULL_TERMINATED()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_NULL_TERMINATED_AT2()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT2(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_NULL_TERMINATED_AT3()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT3(buffer1, buffer2, buffer3, buffer4, buffer5);
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

    printf("\nBSLA_NULLTERMINATED: ");
#ifdef BSLA_NULLTERMINATED
    printf("%s\n", STRINGIFY(BSLA_NULLTERMINATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NULLTERMINATEDAT(x): ");
#ifdef BSLA_NULLTERMINATEDAT
    printf("%s\n", STRINGIFY(BSLA_NULLTERMINATEDAT(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_NULLTERMINATED_IS_ACTIVE);
    P(BSLA_NULLTERMINATEDAT_IS_ACTIVE);

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
                   test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

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


{
// Then, in 'main', we call 'catStrings' correctly:
//..
        char buf[1000];
        catStrings(buf, "Now", "you", "see", "it.", NULL);
        printf("%s\n", buf);
//..
// which compiles without a warning and produces the output:
//..
//  Now you see it.
//..
// Now, we call 'catStrings" again and forget to add the terminating 'NULL':
//..
#if U_TRIGGER_WARNINGS
if (veryVeryVeryVerbose) {
        catStrings(buf, "Now", "you", "don't.");
        printf("%s\n", buf);
}
#endif
//..
// Finally, we get the compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:412:47: warning: missing sentinel in function
//  call [-Wsentinel]
//      catStrings(buf, "Now", "you", "don't.");
//                                            ^
//                                            , nullptr
//  .../bsla_nullterminated.t.cpp:137:10: note: function has been explicitly
//  marked sentinel here
//  void catStrings(char *outputBuffer, ...)
//       ^
//..
}

{
// Then, in 'main', we call 'catVerdict' correctly:
//..
        char buf[1000];
        catVerdict(buf, "We find the", "defendant,", "Bugs Bunny", NULL, 0);
        printf("%s\n", buf);
//..
// which compiles without a warning and produces the output:
//..
//  We find the defendant, Bugs Bunny: not guilty
//..
// Next, we call 'catVerdict' with no 'NULL' passed, and get a warning (and
// probably a segfault if we ran it):
//..
#if U_TRIGGER_WARNINGS
if (veryVeryVeryVerbose) {
        catVerdict(buf, "We find the", "defendant,", "Wile E. Coyote", 1);
        printf("%s\n", buf);
}
#endif
//..
// And we get the following compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:447:70: warning: missing sentinel in function
//  call [-Wsentinel]
//      catVerdict(buf, "We find the", "defendant,", "Wile E. Coyote", 1);
//                                                                   ^
//                                                                   , nullptr
//  .../bsla_nullterminated.t.cpp:171:10: note: function has been explicitly
//  marked sentinel here
//  void catVerdict(char *outputBuffer, ...)
//       ^
//..
// Now, we call 'catVerdict' and forget to put the integer that indicates guilt
// or innocence after the 'NULL'.  This means that 'NULL' is happening at index
// 0, not index 1, which violates the requirement imposed by the annotation:
//..
#if U_TRIGGER_WARNINGS
if (veryVeryVeryVerbose) {
        catVerdict(buf, "We find the", "defendant,", "Road Runner", NULL);
        printf("%s\n", buf);
}
#endif
//..
// Finally, we get the compiler warning:
//..
//  .../bsla_nullterminated.t.cpp:471:67: warning: missing sentinel in function
//  call [-Wsentinel]
//      catVerdict(buf, "We find the", "defendant,", "Road Runner", NULL);
//                                                                ^
//                                                                , nullptr
//  .../bsla_nullterminated.t.cpp:171:10: note: function has been explicitly
//   marked sentinel here
//  void catVerdict(char *outputBuffer, ...)
//       ^
//..
}
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
