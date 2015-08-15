// bslstl_stdexceptutil.t.cpp                                         -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_exceptionutil.h>

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

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

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

  template<class T>
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
