// bsls_fuzztestpreconditionexception.t.cpp                           -*-C++-*-
#include <bsls_fuzztestpreconditionexception.h>

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
// 'bsls::FuzzTestPreconditionException' is a simple mechanism that
// communicates a 'bsls::AssertViolation' from the point where it is created to
// the place where it is consumed.  The intended use-case is for this
// CopyConstructible mechanism to be thrown as an exception, so that the
// attributes of 'AssertViolation' can be queried in an exception handler.  We
// must validate that such an exception object can be created with the desired
// 'AssertViolation'; that the 'AssertViolation' can be queried and have the
// correct values; that the object can be copied, such as by a 'throw'
// expression, and that such a copy will have an 'AssertViolation' having the
// same values as the original; that the objects can safely be destroyed
// without any other effect on the system.
//
//-----------------------------------------------------------------------------
// [2] bsls::FuzzTestPreconditionException(const AssertViolation& av);
// [3] bsls::FuzzTestPreconditionException(const bsls::FTPE& other);
// [2] ~bsls::FuzzTestPreconditionException();
// [2] const AssertViolation& assertViolation() const;
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

    switch (test) {
      case 0:
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

// First we write a macro to act as a precondition testing 'assert' facility
// that will throw an exception of type 'bsls::FuzzTestPreconditionException'
// if the asserted expression fails.  The thrown exception will capture the
// source-code of the expression, the filename and line number of the failing
// expression.
//..
    #define TEST_PRECONDITION(EXPRESSION)                                     \
        if (!(EXPRESSION)) {                                                  \
            bsls::AssertViolation violation(#EXPRESSION,                      \
                                            __FILE__,                         \
                                            __LINE__,                         \
                                            "LEVEL");                         \
            throw bsls::FuzzTestPreconditionException(violation);             \
        }
//..
// Next we use the macro inside a try-block, so that we can catch the exception
// thrown if the tested expression fails.
//..
        try {
            void *p = NULL;
            TEST_PRECONDITION(0 != p);
        }
//..
// If the assertion fails, catch the exception and confirm that it correctly
// recorded the context of where the assertion failed.
//..
        catch (const bsls::FuzzTestPreconditionException& exception) {
            ASSERT(0  == strcmp("0 != p",
                                exception.assertViolation().comment()));
            ASSERT(0  == strcmp(__FILE__,
                                exception.assertViolation().fileName()));
            ASSERT(11 == __LINE__ - exception.assertViolation().lineNumber());
            ASSERT(0  == strcmp("LEVEL",
                                exception.assertViolation().assertLevel()));
        }
//..
#else
        if (verbose)
            printf("\nUsage example disabled without exception support.\n");
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 That a copy of a 'bsls::FuzzTestPreconditionException' object
        //:   might not have the same value as the original object.  That the
        //:   source object might be altered by the act of making a copy.
        //
        // Plan:
        //: 1 Create a test object 'x' with known attribute values.  Then
        //:   create 'y', a copy of 'x', and verify that each corresponding
        //:   attribute of 'x' and 'y' have the same value.  Finally, confirm
        //:   that the attributes of 'x' still have the initially supplied
        //:   values.
        //
        // Testing:
        //  bsls::FuzzTestPreconditionException(const bsls::FTPE& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST COPY CONSTRUCTOR"
                            "\n=====================\n");

        if (verbose)
            printf("\nCreate test strings for constructing x.\n");

        const char *expression = "expression string";
        const char *filename = "filename string";
        const char *level = "level string";
        const int   lineNumber = 42;

        if (verbose) printf("\nCreate test object 'x'.\n");
        const bsls::AssertViolation               av(expression,
                                                     filename,
                                                     lineNumber,
                                                     level);
        const bsls::FuzzTestPreconditionException x(av);

        if (verbose) printf("\nCreate test object 'y', a copy of 'x'.\n");
        const bsls::FuzzTestPreconditionException y = x;

        if (veryVerbose)
            printf("\nConfirm 'y' has the same attribute values as 'x'.\n");

        ASSERTV(x.assertViolation().comment(),
                y.assertViolation().comment(),
                x.assertViolation().comment() ==
                    y.assertViolation().comment());
        ASSERTV(x.assertViolation().fileName(),
                y.assertViolation().fileName(),
                x.assertViolation().fileName() ==
                    y.assertViolation().fileName());
        ASSERTV(x.assertViolation().lineNumber(),
                y.assertViolation().lineNumber(),
                x.assertViolation().lineNumber() ==
                    y.assertViolation().lineNumber());
        ASSERTV(x.assertViolation().assertLevel(),
                y.assertViolation().assertLevel(),
                x.assertViolation().assertLevel() ==
                    y.assertViolation().assertLevel());

        if (verbose) printf("\nConfirm that 'x' has not changed.\n");
        ASSERTV(expression,
                x.assertViolation().comment(),
                expression == x.assertViolation().comment());
        ASSERTV(filename,
                x.assertViolation().fileName(),
                filename == x.assertViolation().fileName());
        ASSERTV(x.assertViolation().lineNumber(),
                lineNumber == x.assertViolation().lineNumber());
        ASSERTV(level,
                x.assertViolation().assertLevel(),
                level == x.assertViolation().assertLevel());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST VALUE CONSTRUCTOR AND PRIMARY INSPECTOR
        //
        // Concerns:
        //: 1 That a 'bsls::FuzzTestPreconditionException' object can be
        //:   created with a data member of type 'bsls::AssertViolation' with
        //:   attributes having the same values contained in the object of type
        //:   'bsls::AssertViolation' passed to the constructor.  That the
        //:   object's data member can be queried, regardless of whether the
        //:   object is 'const' or not.  That the queried data member has the
        //:   same values as those contained in the object supplied to the
        //:   constructor.  That the object can be cleanly destroyed without
        //:   affecting the strings referenced by pointer.
        //
        // Plan:
        //: 1 Construct an object 'x' of type
        //:   'bsls::FuzzTestPreconditionException' from a
        //:   'bsls::AssertViolation', and confirm each attribute of the member
        //:   'AssertViolation' has the respective value passed to the
        //:   constructor.  Allow 'x' to be destroyed, and confirm the strings
        //:   passed to the constructor can still be reached and have the same
        //:   value.  Then construct a second object 'y' with different values
        //:   for each attribute than were used constructing 'x'.  Confirm that
        //:   the attributes for 'y' have the respective values passed to the
        //:   constructor, and so distinctly different values than when
        //:   constructing 'x'.
        //
        // Testing:
        //   bsls::FuzzTestPreconditionException(const AssertViolation& av);
        //   ~bsls::FuzzTestPreconditionException();
        //   const AssertViolation& assertViolation() const;
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nTEST VALUE CONSTRUCTOR AND PRIMARY INSPECTORS"
                         "\n=============================================\n" );

        const char *exprX       = "first expression";
        const char *fileX       = "first file";
        const char *levelX      = "LEVEL";
        const int   lineNumberX = 13;

        {
            if (verbose)
                printf("\nCreate test object 'x'.\n");
            bsls::AssertViolation av(exprX, fileX, lineNumberX, levelX);

            bsls::FuzzTestPreconditionException x(av);

            if (verbose)
                printf("\nVerify attributes of 'x'.\n");
            ASSERTV(exprX,
                    x.assertViolation().comment(),
                    exprX == x.assertViolation().comment());
            ASSERTV(fileX,
                    x.assertViolation().fileName(),
                    fileX == x.assertViolation().fileName());
            ASSERTV(x.assertViolation().lineNumber(),
                    lineNumberX == x.assertViolation().lineNumber());
            ASSERTV(levelX,
                    x.assertViolation().assertLevel(),
                    levelX == x.assertViolation().assertLevel());
        }

        if (verbose)
            printf("\nVerify string literals unchanged.\n");
        ASSERT(0 == strcmp("first expression", exprX));
        ASSERT(0 == strcmp("first file", fileX));

        const char *exprY       = "second expression";
        const char *fileY       = "second file";
        const char *levelY      = "second level";
        const int   lineNumberY = 8;

        {
            if (verbose)
                printf("\nCreate second test object 'y'.\n");
            bsls::AssertViolation av(exprY, fileY, lineNumberY, levelY);

            bsls::FuzzTestPreconditionException y(av);

            if (verbose)
                printf("\nVerify attributes of 'y'.\n");
            ASSERTV(exprY,
                    y.assertViolation().comment(),
                    exprY == y.assertViolation().comment());
            ASSERTV(fileY,
                    y.assertViolation().fileName(),
                    fileY == y.assertViolation().fileName());
            ASSERTV(y.assertViolation().lineNumber(),
                    lineNumberY == y.assertViolation().lineNumber());
            ASSERTV(levelY,
                    y.assertViolation().assertLevel(),
                    levelY == y.assertViolation().assertLevel());

            ASSERTV(exprX,
                    y.assertViolation().comment(),
                    exprX != y.assertViolation().comment());
            ASSERTV(fileX,
                    y.assertViolation().fileName(),
                    fileX != y.assertViolation().fileName());
            ASSERTV(lineNumberX,
                    y.assertViolation().lineNumber(),
                    lineNumberX != y.assertViolation().lineNumber());
            ASSERTV(levelX,
                    y.assertViolation().assertLevel(),
                    levelX != y.assertViolation().assertLevel());
        }

        if (verbose)
            printf("\nVerify string literals unchanged.\n");
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
        const char *filename   = "filename string";
        const char *level      = "level string";
        const int   lineNumber  = 42;

        if (verbose) printf("\nCreate test object x.\n");

        const bsls::AssertViolation         av(expression,
                                               filename,
                                               lineNumber,
                                               level);
        bsls::FuzzTestPreconditionException x(av);

        ASSERTV(expression,
                x.assertViolation().comment(),
                expression == x.assertViolation().comment());
        ASSERTV(filename,
                x.assertViolation().fileName(),
                filename == x.assertViolation().fileName());
        ASSERTV(x.assertViolation().lineNumber(),
                lineNumber == x.assertViolation().lineNumber());
        ASSERTV(level,
                x.assertViolation().assertLevel(),
                level == x.assertViolation().assertLevel());

#if defined(BDE_BUILD_TARGET_EXC)
        // This class is created for the express purpose of throwing as an
        // exception, but this cannot be tested unless exceptions are enabled.
        if (verbose)
            printf("\nThrow and catch a copy of x.\n");
        try {
            throw x;
        }
        catch (bsls::FuzzTestPreconditionException y) {  // catch by value, use
                                                         // copy constructor
            if (veryVerbose)
                printf("\n\tCaught y, a copy of x.\n");
            ASSERTV(x.assertViolation().comment(),
                    y.assertViolation().comment(),
                    x.assertViolation().comment() ==
                        y.assertViolation().comment());
            ASSERTV(x.assertViolation().fileName(),
                    y.assertViolation().fileName(),
                    x.assertViolation().fileName() ==
                        y.assertViolation().fileName());
            ASSERTV(x.assertViolation().lineNumber(),
                    y.assertViolation().lineNumber(),
                    x.assertViolation().lineNumber() ==
                        y.assertViolation().lineNumber());
            ASSERTV(x.assertViolation().assertLevel(),
                    y.assertViolation().assertLevel(),
                    x.assertViolation().assertLevel() ==
                        y.assertViolation().assertLevel());
        }
#endif

        if (verbose)
            printf("\nConfirm that x has not changed.\n");
        ASSERTV(expression,
                x.assertViolation().comment(),
                expression == x.assertViolation().comment());
        ASSERTV(filename,
                x.assertViolation().fileName(),
                filename == x.assertViolation().fileName());
        ASSERTV(x.assertViolation().lineNumber(),
                lineNumber == x.assertViolation().lineNumber());
        ASSERTV(level,
                x.assertViolation().assertLevel(),
                level == x.assertViolation().assertLevel());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
