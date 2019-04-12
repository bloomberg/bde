// bsla_printf.t.cpp                                                  -*-C++-*-
#include <bsla_printf.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <string>

#include <stdarg.h>
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
//  BSLA_PRINTF(FMTIDX, STARTIDX)         Warning
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
///Example 1: 'printf'-line Function That Returns a 'bsl::string' by Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function 'strPrintf' that takes a variable number of
// arguments.  The second argument is the format string, and we annotate it
// with 'BSLA_PRINTF':
//..
    std::string strPrintf(size_t *numChars, const char *format, ...)
                                                             BSLA_PRINTF(2, 3);
    std::string strPrintf(size_t *numChars, const char *format, ...)
        // Do an 'sprintf' write to a 'std::string' and return the string by
        // value.  Ensure that the write can't overflow unless memory or
        // address space is exhausted.  The specified '*numChars' is the number
        // of characters written, the specified 'format' is the 'printf'-style
        // format string, and the specified '...' is the variable-length list
        // of arguments to be formatted.
    {
        std::string ret = " ";
//
        va_list ap;
        va_start(ap, format);

        // 'vnsprintf' returns the number of characters that WOULD have been
        // written (not including the terminating '\0') had the buffer been
        // long enough.

        *numChars = ::vsnprintf(&ret[0], 1, format, ap);
        va_end(ap);
//
        ret.resize(*numChars + 1);
//
        va_start(ap, format);
        *numChars = ::vsnprintf(&ret[0], *numChars + 1, format, ap);
        va_end(ap);
//
        BSLS_ASSERT(::strlen(ret.c_str()) == *numChars);
//
        ret.resize(*numChars);
        return ret;
    }
//..

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

void test_PRINTF(const char *pattern, ...) BSLA_PRINTF(1, 2);
void test_PRINTF(const char *pattern, ...)
    // Do nothing with the specified 'pattern'.
{
    (void) pattern;
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

void use_without_diagnostic_message_PRINTF()
    // Call 'test_PRINTF' a few times with correct arguments.
{
    test_PRINTF("%s", "string");
    test_PRINTF("%d", 1);
    test_PRINTF("%f", 3.14159);
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_PRINTF()
{
    test_PRINTF("%s", 3.14159);
    test_PRINTF("%d", "string");
    test_PRINTF("%f", "other string");
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

    printf("\nBSLA_PRINTF(fmt, arg): ");
#ifdef BSLA_PRINTF
    printf("%s\n", STRINGIFY(BSLA_PRINTF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_PRINTF_IS_ACTIVE);

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

// Then, in 'main', we call the function correctly a couple of times:
//..
        size_t len;
        std::string s;
//
        s = strPrintf(&len, "%s %s %s %g\n", "woof", "meow", "arf", 23.5);
        ASSERT("woof meow arf 23.5\n" == s);
        ASSERT(19 == len);
        ASSERT(len == s.length());
//
        s = strPrintf(&len, "%s %s %s %s %s %s %s %s %s\n",
                               "The", "rain", "in", "Spain", "falls", "mainly",
                                                         "in", "the", "plain");
        ASSERT("The rain in Spain falls mainly in the plain\n" == s);
        ASSERT(44 == len);
        ASSERT(len == s.length());
//..
// Now, we call it with too many arguments and of the wrong type:
//..
#if U_TRIGGER_WARNINGS
        s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
#endif
//..
// Finally, we observe the compiler warnings with clang:
//..
//  .../bsla_printf.t.cpp:328:41: warning: format specifies type 'int' but the
//  argument has type 'const char *' [-Wformat]
//      s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//                           ~~         ^~~~~
//                           %s
//  .../bsla_printf.t.cpp:328:48: warning: format specifies type 'double' but
//  the argument has type 'int' [-Wformat]
//      s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//                              ~~             ^~
//                              %d
//  .../bsla_printf.t.cpp:328:52: warning: format specifies type 'double' but
//  the argument has type 'int' [-Wformat]
//      s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//                                 ~~              ^~
//                                 %d
//  .../bsla_printf.t.cpp:328:56: warning: data argument not used by format
//  string [-Wformat-extra-args]
//      s = strPrintf(&len, "%c %f %g", "arf", 27, 32, 65, 27);
//                          ~~~~~~~~~~                 ^
//..
        (void) s;
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
