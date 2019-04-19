// bsla_format.t.cpp                                                  -*-C++-*-
#include <bsla_format.h>

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
//  BSLA_FORMAT                           Warning
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
//                              USAVE EXAMPLE
// ----------------------------------------------------------------------------

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Language Translator Function
///- - - - - - - - - - - - - - - - - - - - -
// First, we define an 'enum', 'Language', to indicate the choice of languages:
//..
    enum Language {
        e_ENGLISH,
        e_SPANISH,
        e_DUTCH,
        e_FRENCH };
//..
// Then, we define a function, 'prefixName', which will take a format string
// and prefix it with the word 'name' in the selected language.  The
// 'BSLA_FORMAT' annotation indicates that the result will be a pointer to a
// 'printf'-style format string equivalent to the format string passed to the
// third argument:
//..
    const char *prefixName(char *buf, Language lang, const char *format)
                                                                BSLA_FORMAT(3);
    const char *prefixName(char *buf, Language lang, const char *format)
        // Create a buffer beginning with the word 'name' translated to the
        // specified 'lang', followed by the specified format string 'format',
        // using the specified 'buf' for memory.
    {
        const char *name = "";
        switch (lang) {
          case e_ENGLISH: name = "Name";   break;
          case e_SPANISH: name = "Nombre"; break;
          case e_DUTCH:   name = "Naam";   break;
          case e_FRENCH:  name = "Nom";    break;
        }
        ::strcpy(buf, name);
        ::strcat(buf, ": ");
        ::strcat(buf, format);

        return buf;
    }
//..

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

const char *test_FORMAT(const char *locale, const char *format) BSLA_FORMAT(2);
const char *test_FORMAT(const char *locale, const char *format)
    // Return a string literal with the word 'name' translated according to
    // the specified 'locale', similar to the specified 'format'.
{
    if (0 == strcmp(locale, "FR") && 0 == strcmp(format, "Name: %s")) {
        return "Nom: %s";                                             // RETURN
    }

    return "translateFormat: bad locale or format argument - no translation";
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

void use_without_diagnostic_message_FORMAT()
{
    printf(test_FORMAT("FR", "Name: %s"), "Michael Bloomberg");
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_FORMAT()
{
    printf(test_FORMAT("FR", "Name: %s"), 3);
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

    printf("\nBSLA_FORMAT(arg): ");
#ifdef BSLA_FORMAT
    printf("%s\n", STRINGIFY(BSLA_FORMAT(arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_FORMAT_IS_ACTIVE);

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

// Next, in 'main', we call 'printf' and 'scanf' using the return value of
// 'prefixName'.  No warnings occur when correct arguments are passed:
//..
    char buffer[1000];
    ::printf(prefixName(buffer, e_SPANISH, "%s\n"), "Michael Bloomberg");
//
    char name[100];
    ::scanf(prefixName(buffer, e_FRENCH, "%so"), name);
//..
// Now, we call 'printf' and 'scanf' passing arguments that won't match the
// resulting format string:
//..
#if U_TRIGGER_WARNINGS
    ::printf(prefixName(buffer, e_ENGLISH, "%s\n"), 2.7);
    int x;
    ::scanf(prefixName(buffer, e_DUTCH, "%s"), &x);
#endif
//..
// Finally, we observe the following warning messages with clang:
//..
//  .../bsla_format.t.cpp:300:53: warning: format specifies type 'char *' but
//  the argument has type 'double' [-Wformat]
//      ::printf(prefixName(buffer, e_ENGLISH, "%s\n"), 2.7);
//                                              ~~      ^~~
//                                              %f
//  .../bsla_format.t.cpp:302:48: warning: format specifies type 'char *' but
//  the argument has type 'int *' [-Wformat]
//      ::scanf(prefixName(buffer, e_DUTCH, "%s"), &x);
//                                           ~~    ^~
//                                           %d
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
