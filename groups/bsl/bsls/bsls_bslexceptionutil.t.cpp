// bsls_bslexceptionutil.t.cpp                                        -*-C++-*-
#include <bsls_bslexceptionutil.h>

#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This test driver tests utility functions provided by the
// 'bsls::BslExceptionUtil' namespace.  We need to verify that each function
// throws the documented exception.
//-----------------------------------------------------------------------------
// [ 1] void throwBadAlloc();
// [ 1] void throwBadCast();
// [ 1] void throwBadException();
// [ 1] void throwBadTypeid();
// [ 1] void throwException();
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE 1

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
///Example 1: Throwing a standard exception
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a class that must conform to the requirements of
// the C++ Standard.  There are several clauses that dictate throwing an
// exception of a standard type to indicate failure.  However, we do not want
// to expose the standard exception header to our clients, which would be
// typical when implementing function templates inline, and we want to have a
// consistent behavior when building with a compiler in a non-standard mode
// that does not support exceptions.
//
// First we declare a function template that wants to throw a standard
// exception.  Note that the 'exception' header is not included at this point.
//..
    #include <bsls_bslexceptionutil.h>

    template<class T>
    void testFunction(int selector)
        //  Throw a standard exception according to the specified 'selector'.
    {
        switch (selector) {
//..
//  Now we can use the utilities in this component to throw the desired
//  exception, even though the standard exception classes are not visible to
//  this code.
//..
          case  1: bsls::BslExceptionUtil::throwBadAlloc();
          case  2: bsls::BslExceptionUtil::throwBadCast();
          default: bsls::BslExceptionUtil::throwException();
        }
    }
//..
// Finally, we can write some client code that calls our function, and wishes
// to catch the thrown exception.  Observe that this file must #include the
// corresponding standard header in order to catch the exception.
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
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
//  bool     veryVerbose = argc > 3;
//  bool veryVeryVerbose = argc > 4;

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
        //: 1. Each function throws a standard exception that can be caught as
        //:    the documented type.
        //
        // Plan:
        //: 1. For each utility function in the class 'BslExceptionUtil', use a
        //:    try/catch block to demonstrate that the documented exception is
        //:    thrown.
        //: 2. Use an 'ASSERT(false)' after the throwing function to find any
        //:    cases where the exception is not thrown.
        //: 3. Use a 'catch(...)' block to observe if the wrong exception type
        //:    is thrown.
        //
        // Testing:
        //   void throwBadAlloc();
        //   void throwBadCast();
        //   void throwBadException();
        //   void throwBadTypeid();
        //   void throwException();
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
        catch(...) {
            ASSERT(!"Threw the wrong exception type, expected std::bad_alloc");
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
        catch(...) {
            ASSERT(!"Threw the wrong exception type, expected std::bad_cast");
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
        catch(...) {
            ASSERT(
               !"Threw the wrong exception type, expected std::bad_exception");
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
        catch(...) {
            ASSERT(
                  !"Threw the wrong exception type, expected std::bad_typeid");
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
        catch(...) {
            ASSERT(!"Threw the wrong exception type, expected std::exception");
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
