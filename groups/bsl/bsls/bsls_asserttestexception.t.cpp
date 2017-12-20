// bsls_asserttestexception.t.cpp                                     -*-C++-*-
#include <bsls_asserttestexception.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

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
// handler.  We must validate that such an exception object can be created
// with the desired set of attributes; that the attributes can be queried and
// have the correct values; that the object can be copied, such as by a 'throw'
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
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
#if defined BDE_BUILD_TARGET_EXC
        // --------------------------------------------------------------------
        // Test usage example
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
// First we write a macro to act as an 'assert' facility which will throw an
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
            cout << "\nUsage example disabled without exception support."
                 << endl;
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Test copy constructor
        //
        // Concerns:
        //   That a copy of a 'bsls::AssertTestException' object might not have
        //   the same value for each attribute as the original object.  That
        //   the source object might be altered by the act of making a copy.
        //
        // Plan:
        //   Create a test object 'x' with known attribute values.  Then create
        //   'y', a copy of 'x', and verify that each corresponding attribute
        //   of 'x' and 'y' have the same value.  Finally, confirm that the
        //   attributes of 'x' still have the initially supplied values.
        //
        // Testing:
        //   bsls::AssertTestException(const bsls::AssertTestException&);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nCreate test strings for constructing x." << endl;
        const char *expression = "expression string";
        const char *filename = "filename string";

        if (verbose) cout << "\nCreate test object 'x'." << endl;
        const bsls::AssertTestException x(expression, filename, 42);

        if (verbose) cout << "\nCreate test object 'y', a copy of 'x'."
                          << endl;
        const bsls::AssertTestException y = x;

        if (veryVerbose)
            cout << "\nConfirm 'y' has the same attribute values as 'x'."
                 << endl;
        LOOP2_ASSERT(x.expression(), y.expression(),
                     x.expression() == y.expression());
        LOOP2_ASSERT(x.filename(), y.filename(), x.filename() == y.filename());
        LOOP2_ASSERT(x.lineNumber(), y.lineNumber(),
                     x.lineNumber() == y.lineNumber());

        if (verbose) cout << "\nConfirm that 'x' has not changed." << endl;
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Value constructor and primary inspectors
        //
        // Concerns:
        //   That a 'bsls::AssertTestException' object can be created with
        //   attributes have in the same values passed to the constructor.
        //   That the object's attributes can be queried, whether the object
        //   is 'const' or not.  That the queried attributes have the values
        //   supplied to the constructor.  That the object can be cleanly
        //   destroyed without affecting the strings referenced by pointer.
        //
        // Plan:
        //   Construct an object 'x' of type 'bsls::AssertTestException', and
        //   confirm each attribute has the respective value passed to the
        //   constructor.  Allow 'x' to be destroyed, and confirm the strings
        //   passed to the constructor can still be reached and have the same
        //   value.  Then construct a second object 'y' with different values
        //   for each attribute than were used constructing 'x'.  Confirm that
        //   the attributes for 'y' have the respective values passed to the
        //   constructor, and so distinctly different values than when
        //   constructing 'x'.
        //
        // Testing:
        //   bsls::AssertTestException(const char *, const char *, int);
        //   ~bsls::AssertTestException();
        //   const char *expression() const;
        //   const char *filename() const;
        //   int lineNumber() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test Value constructor and primary inspectors\n"
                          << "============================================="
                          << endl;

        const char *exprX = "first expression";
        const char *fileX = "first file";

        {
            if (verbose) cout << "\nCreate test object 'x'." << endl;
            bsls::AssertTestException x(exprX, fileX, 13);

            if (verbose) cout << "\nVerify atttributes of 'x'." << endl;
            LOOP2_ASSERT(exprX, x.expression(), exprX == x.expression());
            LOOP2_ASSERT(fileX, x.filename(), fileX == x.filename());
            LOOP_ASSERT(x.lineNumber(), 13 == x.lineNumber());
        }

        if (verbose) cout << "\nVerify string literals unchanged." << endl;
        ASSERT(0 == strcmp("first expression", exprX));
        ASSERT(0 == strcmp("first file", fileX));

        const char *exprY = "second expression";
        const char *fileY = "second file";

        {
            if (verbose) cout << "\nCreate second test object 'y'." << endl;
            bsls::AssertTestException y(exprY, fileY, 8);

            if (verbose) cout << "\nVerify atttributes of 'y'." << endl;
            LOOP2_ASSERT(exprY, y.expression(), exprY == y.expression());
            LOOP2_ASSERT(fileY, y.filename(), fileY == y.filename());
            LOOP_ASSERT(y.lineNumber(), 8 == y.lineNumber());

            LOOP2_ASSERT(exprX, y.expression(), exprX != y.expression());
            LOOP2_ASSERT(fileX, y.filename(), fileX != y.filename());
        }

        if (verbose) cout << "\nVerify string literals unchanged." << endl;
        ASSERT(0 == strcmp("second expression", exprY));
        ASSERT(0 == strcmp("second file", fileY));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   beginning testing in earnest.  That an object of this type can be
        //   thrown as an exception and pass along the information from
        //   construction to the object caught in an exception-handler.
        //
        // Plan:
        //   Create a test objects 'x' and throw a copy of this as an
        //   exception.  Catch a copy of the exception as 'y' in the
        //   exception-handler, and compare each attribute with the original
        //   'x'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        if (verbose)
            cout << "\nCreate test strings for constructing x." << endl;
        const char *expression = "expression string";
        const char *filename = "filename string";

        if (verbose) cout << "\nCreate test object x." << endl;
        const bsls::AssertTestException x(expression, filename, 42);
        // Note that the first two tests are intentionally pointer-value
        // comparisons and not string comparisons.
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());

#if defined(BDE_BUILD_TARGET_EXC)
        // This class is created for the express purpose of throwing as an
        // exception, but this cannot be tested unless exceptions are enabled.
        if (verbose) cout << "\nThrow and catch a copy of x." << endl;
        try {
            throw x;
        }
        catch(bsls::AssertTestException y) {  // catch by value, worst case
                                              // scenario
            if (veryVerbose) cout << "\n\tCaught y, a copy of x." << endl;
            LOOP2_ASSERT(x.expression(), y.expression(),
                         x.expression() == y.expression());
            LOOP2_ASSERT(x.filename(), y.filename(),
                         x.filename() == y.filename());
            LOOP2_ASSERT(x.lineNumber(), y.lineNumber(),
                         x.lineNumber() == y.lineNumber());
        }
#endif

        if (verbose) cout << "\nConfirm that x has not changed." << endl;
        LOOP2_ASSERT(expression, x.expression(), expression == x.expression());
        LOOP2_ASSERT(filename, x.filename(), filename == x.filename());
        LOOP_ASSERT(x.lineNumber(), 42 == x.lineNumber());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
