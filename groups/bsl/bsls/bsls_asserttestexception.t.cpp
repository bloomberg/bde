// bsls_asserttestexception.t.cpp                                     -*-C++-*-
#include <bsls_asserttestexception.h>

#include <bsls_bsltestutil.h>

#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bsls::AssertTestException' is a simple mechanism that communicates a set of
// values from the point where is created to the place where it is consumed.
// The intended use-case is for this CopyConstructible mechanism to be thrown
// as an exception, so that the attributes can be queried in an exception-
// handler.  We must validate that such an exception object can be created with
// the desired set of attributes; that the attributes can be queried and have
// the correct values; that the object can be copied, such as by a 'throw'
// expression, and that such a copy will have attributes having the same values
// as the original; that the objects can safely be destroyed without any other
// effect on the system.
//
//-----------------------------------------------------------------------------
// [2] bsls::AssertTestException(const char *, const char *, int);
// [3] bsls::AssertTestException(const bsls::AssertTestException& other);
// [2] ~bsls::AssertTestException();
// [2] const char *expression() const;
// [2] const char *filename() const;
// [2] int lineNumber() const;
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [4] USAGE EXAMPLE

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

//=============================================================================
//              GLOBAL HELPER FUNCTIONS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 4: {
#if defined BDE_BUILD_TARGET_EXC
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST USAGE EXAMPLE"
                            "\n==================\n");

// First we write a macro to act as an 'assert' facility that will throw an
// exception of type 'bsls::AssertTestException' if the asserted expression
// fails.  The thrown exception will capture the source-code of the expression,
// the filename and line number of the failing expression.
//..
#define TEST_ASSERT(EXPRESSION)                                               \
    if(!(EXPRESSION)) {                                                       \
        throw bsls::AssertTestException( #EXPRESSION, __FILE__, __LINE__ );   \
    }
//..
// Next we use the macro inside a try-block, so that we can catch the exception
// thrown if the tested expression fails.
//..
    try {
        void *p = NULL;
        TEST_ASSERT(0 != p);
    }
//..
// If the assertion fails, catch the exception and confirm that it correctly
// recorded the context of where the assertion failed.
//..
    catch(const bsls::AssertTestException& ex) {
        ASSERT( 0 == strcmp("0 != p", ex.expression()));
        ASSERT( 0 == strcmp(__FILE__, ex.filename()));
        ASSERT( 9 == __LINE__ - ex.lineNumber());
    }
//..
#else
        if (verbose)
            printf("\nUsage example disabled without exception support.\n");
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 That a copy of a 'bsls::AssertTestException' object might not
        //:   have the same value for each attribute as the original object.
        //:   That the source object might be altered by the act of making a
        //:   copy.
        //
        // Plan:
        //: 1 Create a test object 'x' with known attribute values.  Then
        //:   create 'y', a copy of 'x', and verify that each corresponding
        //:   attribute of 'x' and 'y' have the same value.  Finally, confirm
        //:   that the attributes of 'x' still have the initially supplied
        //:   values.
        //
        // Testing:
        //  bsls::AssertTestException(const bsls::AssertTestException& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST COPY CONSTRUCTOR"
                            "\n=====================\n");

        if (verbose)
            printf("\nCreate test strings for constructing x.\n");

        const char *expression = "expression string";
        const char *filename = "filename string";

        if (verbose) printf("\nCreate test object 'x'.\n");
        const bsls::AssertTestException x(expression, filename, 42);

        if (verbose) printf("\nCreate test object 'y', a copy of 'x'.\n");
        const bsls::AssertTestException y = x;

        if (veryVerbose)
            printf("\nConfirm 'y' has the same attribute values as 'x'.\n");

        LOOP2_ASSERT(x.expression(), y.expression(),
                     x.expression() == y.expression());
        LOOP2_ASSERT(x.filename(), y.filename(), x.filename() == y.filename());
        LOOP2_ASSERT(x.lineNumber(), y.lineNumber(),
                     x.lineNumber() == y.lineNumber());

        if (verbose) printf("\nConfirm that 'x' has not changed.\n");
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST VALUE CONSTRUCTOR AND PRIMARY INSPECTORS
        //
        // Concerns:
        //: 1 That a 'bsls::AssertTestException' object can be created with
        //:   attributes have in the same values passed to the constructor.
        //:   That the object's attributes can be queried, whether the object
        //:   is 'const' or not.  That the queried attributes have the values
        //:   supplied to the constructor.  That the object can be cleanly
        //:   destroyed without affecting the strings referenced by pointer.
        //
        // Plan:
        //: 1 Construct an object 'x' of type 'bsls::AssertTestException', and
        //:   confirm each attribute has the respective value passed to the
        //:   constructor.  Allow 'x' to be destroyed, and confirm the strings
        //:   passed to the constructor can still be reached and have the same
        //:   value.  Then construct a second object 'y' with different values
        //:   for each attribute than were used constructing 'x'.  Confirm that
        //:   the attributes for 'y' have the respective values passed to the
        //:   constructor, and so distinctly different values than when
        //:   constructing 'x'.
        //
        // Testing:
        //   bsls::AssertTestException(const char *, const char *, int);
        //   ~bsls::AssertTestException();
        //   const char *expression() const;
        //   const char *filename() const;
        //   int lineNumber() const;
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nTEST VALUE CONSTRUCTOR AND PRIMARY INSPECTORS"
                         "\n=============================================\n" );

        const char *exprX = "first expression";
        const char *fileX = "first file";

        {
            if (verbose) printf("\nCreate test object 'x'.\n");
            bsls::AssertTestException x(exprX, fileX, 13);

            if (verbose) printf("\nVerify attributes of 'x'.\n");
            LOOP2_ASSERT(exprX, x.expression(), exprX == x.expression());
            LOOP2_ASSERT(fileX, x.filename(), fileX == x.filename());
            LOOP_ASSERT(x.lineNumber(), 13 == x.lineNumber());
        }

        if (verbose) printf("\nVerify string literals unchanged.\n");
        ASSERT(0 == strcmp("first expression", exprX));
        ASSERT(0 == strcmp("first file", fileX));

        const char *exprY = "second expression";
        const char *fileY = "second file";

        {
            if (verbose) printf("\nCreate second test object 'y'.\n");
            bsls::AssertTestException y(exprY, fileY, 8);

            if (verbose) printf("\nVerify attributes of 'y'.\n");
            LOOP2_ASSERT(exprY, y.expression(), exprY == y.expression());
            LOOP2_ASSERT(fileY, y.filename(), fileY == y.filename());
            LOOP_ASSERT(y.lineNumber(), 8 == y.lineNumber());

            LOOP2_ASSERT(exprX, y.expression(), exprX != y.expression());
            LOOP2_ASSERT(fileX, y.filename(), fileX != y.filename());
        }

        if (verbose) printf("\nVerify string literals unchanged.\n");
        ASSERT(0 == strcmp("second expression", exprY));
        ASSERT(0 == strcmp("second file", fileY));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   beginning testing in earnest.  That an object of this type can be
        //:   thrown as an exception and pass along the information from
        //:   construction to the object caught in an exception-handler.
        //
        // Plan:
        //: 2 Create a test objects 'x' and throw a copy of this as an
        //:   exception.  Catch a copy of the exception as 'y' in the
        //:   exception-handler, and compare each attribute with the original
        //:   'x'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose)
            printf("\nCreate test strings for constructing x.\n");
        const char *expression = "expression string";
        const char *filename = "filename string";

        if (verbose) printf("\nCreate test object x.\n");
        const bsls::AssertTestException x(expression, filename, 42);
        // Note that the first two tests are intentionally pointer-value
        // comparisons and not string comparisons.
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());

#if defined(BDE_BUILD_TARGET_EXC)
        // This class is created for the express purpose of throwing as an
        // exception, but this cannot be tested unless exceptions are enabled.
        if (verbose) printf("\nThrow and catch a copy of x.\n");
        try {
            throw x;
        }
        catch(bsls::AssertTestException y) {  // catch by value, worst case
                                              // scenario
            if (veryVerbose) printf("\n\tCaught y, a copy of x.\n");
            LOOP2_ASSERT(x.expression(), y.expression(),
                         x.expression() == y.expression());
            LOOP2_ASSERT(x.filename(), y.filename(),
                         x.filename() == y.filename());
            LOOP2_ASSERT(x.lineNumber(), y.lineNumber(),
                         x.lineNumber() == y.lineNumber());
        }
#endif

        if (verbose) printf("\nConfirm that x has not changed.\n");
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());
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
// Copyright 2018 Bloomberg Finance L.P.
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
