// bsla_nodiscard.t.cpp                                               -*-C++-*-
#include <bsla_nodiscard.h>

#include <bsls_assert.h>
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
//  BSLA_NODISCARD                        Warning
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
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage Examples
///--------------
// This section illustrates intended use of this component.
//
///Example 1: Square Root Function
///- - - - - - - - - - - - - - - -
// First, we define a function, 'newtonsSqrt', which uses Newton's method for
// calculating a square root.  Since the function has no side effects, it
// doesn't make sense to call it and ignore its result, so we annotate it with
// 'BSLA_NODISCARD':
//..
    BSLA_NODISCARD
    double newtonsSqrt(double x);
    double newtonsSqrt(double x)
        // Take the square root of the specified 'x'.  The behavior is
        // undefined unless 'x' is positive.
    {
        BSLS_ASSERT(x > 0);
//
        double guess = 1.0;
        for (int ii = 0; ii < 100; ++ii) {
            guess = (guess + x / guess) / 2;
        }
//
        return guess;
    }
//..

///Example 2: No-discard Type
/// - - - - - - - - - - - - -
// Suppose we create a guard type that is capable of closing delimiters that we
// have opened while operating on an output stream.  The example uses C I/O and
// string literals for brevity.
//
// First, we define a guard type 'DelimiterGuard0':
//..
    class DelimGuard0 {
      private:
        // DATA
        const char *d_closingText_p;  // Held, not owned

      public:
        // CREATORS
        explicit DelimGuard0(const char *closingText);
            // Create a delimiter guard that upon its destruction prints the
            // specified 'closingText' to 'stdout'.  The behavior is undefined
            // unless 'closingText' outlives the created object.

        ~DelimGuard0();
            // Print the closing text to the output file, then destroy this
            // object.
    };
//..
// Then we can write code that uses the guard properly:
//..
    void guard0ProperUse()
    {
        printf("\n```ruby\n");  DelimGuard0 closeCode("```\n");

        // Suppose long and complicated code with early returns writing some
        // source code between the delimiters.  Instead we write something
        // trivial for brevity:
        printf("puts 'Hello World'\n");
    }
//..
// Next, we demonstrate that the guard works as intended (see in 'main'):
//..
//  guard0ProperUse();  // prints: [\n]```ruby[\n]puts 'Hello World'[\n]```[\n]
//..
// Then, we write code missing the variable name for the guard.  By not giving
// a variable name we turn what should be an automatic (local) variable
// definition into a so-called expression statement: '<expression>;'.
// Expression statements execute an expression for its side effects, then
// destroy all temporaries created in the expression "at the semicolon".  All
// the 'printf' function calls below are expression statements, they just don't
// have any temporaries to destroy.
//..
    void discardedGuard0()
    {
        printf("(");  DelimGuard0(")\n");
        printf("in-parens");
    }
//..
// Next, we demonstrate the bug cause by the guard variable name missing:
//..
//  discardedGuard0();  // prints: ()[\n]in-parens
//..
// Then, we add the no-discard annotation to our guard type directly:
//..
    class BSLA_NODISCARD_CPP17 DelimGuardCpp17 {
      private:
        // DATA
        const char *d_closingText_p;  // Held, not owned

      public:
        // CREATORS
        explicit
        DelimGuardCpp17(const char *closingText);
            // Create a delimiter guard that upon its destruction prints the
            // specified 'closingText' to 'stdout'.  The behavior is undefined
            // unless 'closingText' outlives the created object.

        ~DelimGuardCpp17();
            // Print the closing text to the output file, then destroy this
            // object.
    };
//..
// The rest is omitted for brevity.
//
// Next, we can write the buggy code again using the annotated type:
//..
#if U_TRIGGER_WARNINGS
    void discardedGuardCpp17()
    {
        printf("[");  DelimGuardCpp17("]");
        printf("in-brackets");
    }
#endif
//..
// Finally, we can demonstrate using a C++17 compiler that we get a warning for
// the buggy code:
//..
//  .../bsla_nodiscard.t.cpp:LLL:CC: warning: ignoring return value of 'double
//  newtonsSqrt(double)', declared with attribute warn_unused_result
//  [-Wunused-result]
//       DelimGuardCpp17("]");
//
// ...\bsla_nodiscard.t.cpp(227,36): warning C4834: discarding return value of
//                                          function with 'nodiscard' attribute
//..

                           // -----------------
                           // class DelimGuard0
                           // -----------------

// CREATORS
DelimGuard0::DelimGuard0(const char *closingText)
: d_closingText_p(closingText)
{
    BSLS_ASSERT(closingText);
}

DelimGuard0::~DelimGuard0()
{
    fputs(d_closingText_p, stdout);
}

                        // ---------------------
                        // class DelimGuardCpp17
                        // ---------------------

// CREATORS
DelimGuardCpp17::DelimGuardCpp17(const char *closingText)
: d_closingText_p(closingText)
{
    BSLS_ASSERT(closingText);
}

DelimGuardCpp17::~DelimGuardCpp17()
{
    fputs(d_closingText_p, stdout);
}

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

BSLA_NODISCARD
int test_NODISCARD();

int test_NODISCARD()
    // Return 1.
{
    return 1;
}

BSLA_NODISCARD_CPP17
int test_NODISCARD_CPP17();

int test_NODISCARD_CPP17()
    // Return 1.
{
    return 1;
}

struct BSLA_NODISCARD_CPP17 Type_NODISCARD_CPP17;
struct Type_NODISCARD_CPP17 {
    // Any function call that creates or returns-by-copy an object of this type
    // should behave as if that function had been marked 'BSLA_NODISCARD' when
    // compiled on a C++17 compiler.

    explicit Type_NODISCARD_CPP17(const char *) {}
        // Just to avoid the most vexing parse without weird syntax.

    Type_NODISCARD_CPP17() {}
        // Must make a default constructor by hand due to having another one.
};

Type_NODISCARD_CPP17 test_Type_NODISCARD_CPP17();
Type_NODISCARD_CPP17 test_Type_NODISCARD_CPP17()
    // Return a 'Type_NODISCARD_CPP17' object.  This function is deliberately
    // not marked with either of the 'BSLA_NODISCARD*' annotations.
{
    return Type_NODISCARD_CPP17();
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

int use_without_diagnostic_message_NODISCARD()
    // Call 'testNODISCARD' and use the result.
{
    return test_NODISCARD();
}

void use_without_diagnostic_message_NODISCARD_CPP17()
    // Call 'testNODISCARD' and use the result.
{
    (void)test_NODISCARD_CPP17();

    Type_NODISCARD_CPP17 varname("most vexing parse is ''fun''");

    (void)test_Type_NODISCARD_CPP17();
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_NODISCARD()
    // Call 'test_NODISCARD' and don't use the result.
{
    test_NODISCARD();
}

void use_with_warning_message_NODISCARD_CPP17()
    // Call 'test_NODISCARD_CPP17', 'test_Type_NODISCARD_CPP17' and don't use
    // the result.  Create a 'Type_NODISCARD_CPP17' object without a name and
    // not use it.
{
    test_NODISCARD_CPP17();

    Type_NODISCARD_CPP17();

    test_Type_NODISCARD_CPP17();
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

    printf("\n\tBSLA_NODISCARD: ");
#ifdef BSLA_NODISCARD
    printf("%s\n", STRINGIFY(BSLA_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\tBSLA_NODISCARD_CPP17: ");
#ifdef BSLA_NODISCARD_CPP17
    printf("%s\n", STRINGIFY(BSLA_NODISCARD_CPP17));
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    printf("\n\tBSLA_NODISCARD_IS_ACTIVE: ");
#ifdef BSLA_NODISCARD_IS_ACTIVE
    printf("%s\n", STRINGIFY(BSLA_NODISCARD_IS_ACTIVE) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\tBSLA_NODISCARD_CPP17_IS_ACTIVE: ");
#ifdef BSLA_NODISCARD_CPP17_IS_ACTIVE
    printf("%s\n", STRINGIFY(BSLA_NODISCARD_CPP17_IS_ACTIVE));
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n---------------------------------------------\n");
    printf(    "printFlags: bsls_annotation Referenced Macros\n");

    printf("\n\tBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\tBSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\tBSLS_PLATFORM_CMP_GNU: ");
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

// Then, in 'main', we call it normally a few times and observe that it works
// with no compiler warnings generated:
//..
if (verbose) {
    printf("Square root of 9.0 = %g\n",           newtonsSqrt(9.0));
    printf("Square root of 0.01 = %g\n",          newtonsSqrt(0.01));
    printf("Square root of 0.917 * 0.917 = %g\n", newtonsSqrt(0.917 * 0.917));
}
//..
// Next, we call it and do nothing with the result, which will generate a
// warning:
//..

#if U_TRIGGER_WARNINGS
    newtonsSqrt(36.0);
#endif
//..
// Now, we call it and explicitly void the result, which, with gcc, still won't
// suppress the "unused result" warning:
//..
    (void)newtonsSqrt(25.0);
//..
// Finally, we observe the compiler warnings from the last 2 calls:
//..
//  .../bsla_nodiscard.t.cpp:289:22: warning: ignoring return value of 'double
//  newtonsSqrt(double)', declared with attribute warn_unused_result
//  [-Wunused-result]
//       newtonsSqrt(36.0);
//                        ^
//  .../bsla_nodiscard.t.cpp:294:29: warning: ignoring return value of 'double
//  newtonsSqrt(double)', declared with attribute warn_unused_result
//  [-Wunused-result]
//       (void) newtonsSqrt(25.0);
//                               ^
//..

///Example 2: No-discard Type
/// - - - - - - - - - - - - -
// Suppose we create a guard type that is capable of closing delimiters that we
// have opened while operating on an output stream.  The example uses C I/O and
// string literals for brevity.
//
// First, we define a guard type 'DelimiterGuard0':
//..
//  class DelimGuard0 {
//    private:
//      // DATA
//      const char *d_closingText_p;  // Held, not owned
//
//    public:
//      // CREATORS
//      explicit DelimGuard0(const char *closingText);
//          // Create a delimiter guard that upon its destruct  The behavior is
//          // undefined unless both 'closingText' outlive the created object.
//
//      ~DelimGuard0();
//          // Print the closing text to the output file, then destroy this
//          // object.
//  };
//..
// Then we can write code that uses the guard properly:
//..
//  void guard0ProperUse()
//  {
//      printf("\n```ruby\n");  DelimGuard0 closeCode("```\n");
//
//      // Suppose long and complicated code with early returns writing some
//      // source code between the delimiters.  Instead we write something
//      // trivial for brevity:
//      printf("puts 'Hello World'\n");
//  }
//..
// Next, we demonstrate that the guard works as intended:
//..
    if (verbose) {
        guard0ProperUse();  // --> [\n]```ruby[\n]puts 'Hello World'[\n]```[\n]
    }
//..
// Then, we write code missing the variable name for the guard.  By not giving
// a variable name we turn what should be an automatic (local) variable
// definition into a so-called expression statement: '<expression>;'.
// Expression statements execute an expression for its side effects, then
// destroy all temporaries created in the expression "at the semicolon".  All
// the 'printf' function calls below are expression statements, they just don't
// have any temporaries to destroy.
//..
//  void discardedGuard0()
//  {
//      printf("(");  DelimGuard0(")\n");
//      printf("in-parens");
//  }
//..
// Next, we demonstrate the bug when the guard variable name is missing:
//..
    if (verbose) {
        discardedGuard0();  // prints: ()in-parens
        printf("\n");  // add the missing new-line to the end
    }
//..
// Then, we add the no-discard annotation to our guard type directly:
//..
//  class BSLA_NODISCARD_CPP17 DelimGuardCpp17 {
//..
// The rest is omitted for brevity.
//
// Next, we can write the buggy code again using the annotated type:
//..
//  void discardedGuardCpp17()
//  {
//      printf("[");  DelimGuardCpp17("]");
//      printf("in-brackets");
//  }
//..
// Finally, we can demonstrate using a C++17 compiler that we get a warning for
// the buggy code:
//..
// .../bsla_nodiscard.t.cpp:LLL:CC: warning: ignoring temporary created by a
//             constructor declared with 'nodiscard' attribute [-Wunused-value]
//     printf("[");  DelimGuardCpp17("]");
//                   ^~~~~~~~~~~~~~~~~~~~
//
// ...\bsla_nodiscard.t.cpp(227,36): warning C4834: discarding return value of
//                                          function with 'nodiscard' attribute
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

            if (verbose && !veryVeryVerbose) printFlags();

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
