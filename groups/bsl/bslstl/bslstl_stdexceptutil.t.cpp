// bslstl_stdexceptutil.t.cpp                                         -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_exceptionutil.h>
#include <bsls_bsltestutil.h>

#include <stdexcept>    //  yes, we want the native std here


#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
#if defined(BDE_BUILD_TARGET_EXC)
///Usage
///-----
// First we declare a function template that wants to throw a standard
// exception.  Note that the 'stdexcept' header is not included at this
// point.
//..
  # include <bslstl_stdexceptutil.h>

  template<typename T>
  void testFunction(int selector)
      //  Throw a standard exception according to the specified 'selector'.
  {
    switch(selector) {
      case 1: bslstl::StdExceptUtil::throwRuntimeError("sample message 1");
      case 2: bslstl::StdExceptUtil::throwLogicError("sample message 2");
      default : bslstl::StdExceptUtil::throwInvalidArgument("ERROR");
    }
  }
//..
// However, if client code wishes to catch the exception, the .cpp file
// must #include the appropriate header.
//..
  #include <stdexcept>

  void callTestFunction()
  {
    try {
      testFunction<int>(1);
      ASSERT(0 == "Should throw before reaching here");
    }
    catch(const runtime_error& ex) {
      ASSERT(0 == std::strcmp(ex.what(), "sample message 1"));
    }

    try {
      testFunction<double>(2);
      ASSERT(0 == "Should throw before reaching here");
    }
    catch(const logic_error& ex) {
      ASSERT(0 == std::strcmp(ex.what(), "sample message 2"));
    }
  }
//..
#endif // defined BDE_BUILD_TARGET_EXC
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

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
            if(verbose) printf("\nThrowing a runtime_error exception");
            bslstl::StdExceptUtil::throwRuntimeError("one");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const runtime_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "one"));
        }
        try {
            if(verbose) printf("\nThrowing a logic_error exception");
            bslstl::StdExceptUtil::throwLogicError("two");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const logic_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "two"));
        }
        try {
            if(verbose) printf("\nThrowing a domain_error exception");
            bslstl::StdExceptUtil::throwDomainError("three");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const domain_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "three"));
        }
        try {
            if(verbose) printf("\nThrowing an invalid_argument exception");
            bslstl::StdExceptUtil::throwInvalidArgument("four");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const invalid_argument& ex) {
            ASSERT(0 == strcmp(ex.what(), "four"));
        }
        try {
            if(verbose) printf("\nThrowing a length_error exception");
            bslstl::StdExceptUtil::throwLengthError("five");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const length_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "five"));
        }
        try {
            if(verbose) printf("\nThrowing an out_of_range exception");
            bslstl::StdExceptUtil::throwOutOfRange("six");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const out_of_range& ex) {
            ASSERT(0 == strcmp(ex.what(), "six"));
        }
        try {
            if(verbose) printf("\nThrowing a range_error exception");
            bslstl::StdExceptUtil::throwRangeError("seven");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const range_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "seven"));
        }
        try {
            if(verbose) printf("\nThrowing an overflow_error exception");
            bslstl::StdExceptUtil::throwOverflowError("eight");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const overflow_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "eight"));
        }
        try {
            if(verbose) printf("\nThrowing an underflow_error exception");
            bslstl::StdExceptUtil::throwUnderflowError("nine");
            ASSERT( false ); // execution should jump to catch block
        }
        catch(const underflow_error& ex) {
            ASSERT(0 == strcmp(ex.what(), "nine"));
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
// Copyright (C) 2012 Bloomberg L.P.
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
