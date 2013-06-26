// bsls_bslexceptionutil.t.cpp                                        -*-C++-*-
#include <bsls_bslexceptionutil.h>

#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
#if defined(BDE_BUILD_TARGET_EXC)
// First we declare a function template that wants to throw a standard
// exception.  Note that the 'exception' header is not included at this point.
//..
    #include <bsls_bslexceptionutil.h>

    template<typename T>
    void testFunction(int selector)
        //  Throw a standard exception according to the specified 'selector'.
    {
      switch (selector) {
        case 1: bsls::BslExceptionUtil::throwBadAlloc();
        case 2: bsls::BslExceptionUtil::throwBadCast();
        default: bsls::BslExceptionUtil::throwException();
      }
    }
//..
// However, if client code wishes to catch the exception, the '.cpp' file must
// '#include' the appropriate header.
//..
    #include <exception>
    #include <new>
    #include <typeinfo>

    void callTestFunction()
    {
        try {
            testFunction<int>(1);
            ASSERT(0 == "Should throw before reaching here.");
        }
        catch (const std::bad_alloc& ex) {
        }

        try {
            testFunction<double>(2);
            ASSERT(0 == "Should throw before reaching here.");
        }
        catch (const std::bad_cast& ex) {
        }
    }
//..
#endif // defined BDE_BUILD_TARGET_EXC

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    // bool veryVerbose = argc > 3;
    // bool veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Simply invoke the functions 'addSecurity' and 'removeSecurity' to
        //   ensure the code compiles.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============");

#if !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf(
                "\nThis case is not run as it relies on exception support.\n");
#else
        callTestFunction();
#endif // defined BDE_BUILD_TARGET_EXC
     } break;
     case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

#if !defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf(
                "\nThis case is not run as it relies on exception support.\n");
#else
        try {
            if (verbose) printf("\nThrowing a 'bad_alloc' exception");
            bsls::BslExceptionUtil::throwBadAlloc();
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const std::bad_alloc& ex) {
            if (verbose) {
                P(ex.what());
            }
        }

        try {
            if (verbose) printf("\nThrowing a 'bad_cast' exception");
            bsls::BslExceptionUtil::throwBadCast();
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const std::bad_cast& ex) {
            if (verbose) {
                P(ex.what());
            }
        }

        try {
            if(verbose) printf("\nThrowing a 'bad_exception' exception");
            bsls::BslExceptionUtil::throwBadException();
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const std::bad_exception& ex) {
            if (verbose) {
                P(ex.what());
            }
        }

        try {
            if (verbose) printf("\nThrowing a 'bad_typeid' exception");
            bsls::BslExceptionUtil::throwBadTypeid();
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const std::bad_typeid& ex) {
            if (verbose) {
                P(ex.what());
            }
        }

        try {
            if (verbose) printf("\nThrowing an 'exception' exception");
            bsls::BslExceptionUtil::throwException();
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const std::exception& ex) {
            if (verbose) {
                P(ex.what());
            }
        }
#endif // defined BDE_BUILD_TARGET_EXC
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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
