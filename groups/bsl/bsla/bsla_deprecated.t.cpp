// bsla_deprecated.t.cpp                                              -*-C++-*-
#include <bsla_deprecated.h>

#include <bsls_bsltestutil.h>

#include <limits.h>  // 'INT_MIN', 'INT_MAX'
#include <stdio.h>
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'memset'

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
//  BSLA_DEPRECATED                       Warning
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
//                                   USAGE
// ----------------------------------------------------------------------------

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Deprecating a type, a function, and a variable
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a deprecated type 'UsageType':
//..
    struct BSLA_DEPRECATED UsageType {
        int d_int;
    };
//..
// Then, we define a deprecated function 'usageFunc':
//..
    BSLA_DEPRECATED
    void usageFunc();
    void usageFunc()
    {
        printf("Don't call me.\n");
    }
//..
// Next, we define a deprecated variable 'usageVar':
//..
    extern int usageVar BSLA_DEPRECATED;
    int usageVar = 5;
//..
// Then, we define a deprecated typedef 'usageTypedef:
//..
    BSLA_DEPRECATED typedef int UsageTypedef;
//..
// Next, we define a 'struct' with a deprecated member 'd_usageMember':
//..
    struct UsageStruct {
        double                 d_x;
        BSLA_DEPRECATED double d_y;
    };
//..
// Then, we define a deprecated enum 'UsageEnum':
//..
    enum BSLA_DEPRECATED UsageEnum { e_FALSE, e_TRUE };
//..
// Next, we define a template where it's only deprecated if 'TYPE' == 'int':
//..
    template <class TYPE>
    TYPE usageAbs(TYPE x)
    {
        return x < 0 ? -x : x;
    }
//
    template <>
    BSLA_DEPRECATED_MESSAGE("'int' specialization not allowed")
    int usageAbs<int>(int x)
    {
        int ret = x < 0 ? -x : x;
        return ret < 0 ? ~ret : ret;
    }
//..
// Then, as long as we don't use them, no warnings will be issued.
//

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

BSLA_DEPRECATED
void test_DEPRECATED_function();
    // Provide a test function which, if called, will result in a deprecated
    // compiler warning.

void test_DEPRECATED_function()
{
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

int test_DEPRECATED_variable BSLA_DEPRECATED;
    // Provide a test variable which, if used, will result in a deprecated
    // compiler warning.

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

struct BSLA_DEPRECATED Test_DEPRECATED_type {
    // This 'struct' is a test type which, if used, will result in a deprecated
    // compiler warning.

    int d_d;
};

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_DEPRECATED_function()
    // Call 'test_DEPRECATED_function', provoking a compiler warning.
{
    test_DEPRECATED_function();
}

int use_with_warning_message_DEPRECATED_type()
    // Use 'Test_DEPRECATED_type', provoking a compiler warning.
{
    Test_DEPRECATED_type instance_of_DEPRECATED_TYPE;
    instance_of_DEPRECATED_TYPE.d_d = 0;
    return instance_of_DEPRECATED_TYPE.d_d;
}

void use_with_warning_message_DEPRECATED_variable()
    // Use 'test_DEPRECATED_variable', provoking a compiler warning.
{
    (void) test_DEPRECATED_variable;
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

    printf("\nBSLA_DEPRECATED: ");
#ifdef BSLA_DEPRECATED
    printf("%s\n", STRINGIFY(BSLA_DEPRECATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_DEPRECATED_IS_ACTIVE);

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
#if U_TRIGGER_WARNINGS

// Next, we use 'UsageType':
//..
    UsageType ut;
    ut.d_int = 5;
    (void) ut.d_int;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:287:5: warning: 'UsageType' is deprecated
//  [-Wdeprecated-declarations]
//      UsageType ut;
//      ^
//  .../bsla/bsla_deprecated.t.cpp:113:7: note: 'UsageType' has been explicitly
//   marked deprecated here
//      } BSLA_DEPRECATED;
//        ^
//..
// Then, we call 'usageFunc':
//..
    usageFunc();
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:309:5: warning: 'usageFunc' is deprecated
//  [-Wdeprecated-declarations]
//      usageFunc();
//      ^
//  .../bsla_deprecated.t.cpp:117:22: note: 'usageFunc' has been explicitly
//  marked deprecated here
//      void usageFunc() BSLA_DEPRECATED;
//                       ^
//..
// Next, we access 'usageVar':
//..
    printf("%d\n", usageVar);
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:326:20: warning: 'usageVar' is deprecated
//  [-Wdeprecated-declarations]
//      printf("%d\n", usageVar);
//                     ^
//  .../bsla_deprecated.t.cpp:134:25: note: 'usageVar' has been explicitly
//  marked deprecated here
//      extern int usageVar BSLA_DEPRECATED;
//                          ^
//  .../bsla_deprecated.h:119:32: note: expanded from macro 'BSLA_DEPRECATED'
//  #     define BSLA_DEPRECATED [[deprecated]]
//..
// Then, we use 'UsageTypedef':
//..
    UsageTypedef jjj = 32;
    (void) jjj;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:379:5: warning: 'UsageTypedef' is deprecated
//  [-Wdeprecated-declarations]
//      UsageTypedef jjj = 32;
//      ^
//  .../bsla_deprecated.t.cpp:140:5: note: 'UsageTypedef' has been explicitly
//  marked deprecated here
//      BSLA_DEPRECATED typedef int UsageTypedef;
//      ^
//..
// Next, we access the deprecated member of 'UsageStruct':
//..
    UsageStruct us;
    ::memset(&us, 0, sizeof(us));
    ASSERT(0 == us.d_x);    // no warning
    ASSERT(0 == us.d_y);    // 'd_y' is deprecated -- issues warning.
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:387:20: warning: 'd_y' is deprecated
//  [-Wdeprecated-declarations]
//      assert(0 == us.d_y);    // 'd_y' is deprecated -- issues warning.
//                     ^
//  .../bsla_deprecated.t.cpp:146:9: note: 'd_y' has been explicitly marked
//  deprecated here
//          BSLA_DEPRECATED double d_y;
//          ^
//..
// Now, we use the deprecated 'UsageEnum':
//..
    UsageEnum ue;
    ue = e_TRUE;
    (void) ue;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:411:15: warning: 'UsageEnum' is deprecated
//  [-Wdeprecated-declarations]
//       UsageEnum ue;
//                 ^
//  .../bsla_deprecated.t.cpp:152:26: note: declared here
//       enum BSLA_DEPRECATED UsageEnum { e_FALSE, e_TRUE };
//                            ^
//..
// Finally, we access the deprecated specialization of 'usageAbs':
//..
    ASSERT(2.0 == usageAbs(-2.0));            // no warning, 'usageAbs<double>'
                                              // not deprecated
    ASSERT(INT_MAX == usageAbs(INT_MIN));     // warning, 'usageAbs<int>' is
                                              // deprecated
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:441:39: warning: 'TYPE usageAbs(TYPE) [with TYPE
//  = int]' is deprecated: 'int' specialization not allowed
//  [-Wdeprecated-declarations]
//       assert(INT_MAX == usageAbs(INT_MIN));     // warning, 'usageAbs<int>'
//                                         ^
//  .../bsla_deprecated.t.cpp:168:9: note: declared here
//       int usageAbs<int>(int x)
//           ^~~~~~~~~~~~~
//..

#endif
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
