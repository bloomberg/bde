// bslstl_stdexceptutil.t.cpp                                         -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_log.h>

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

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

namespace BSLSTL_STDEXCEPTUTIL_TEST_CASE_2 {

namespace TC = BSLSTL_STDEXCEPTUTIL_TEST_CASE_2;

typedef bslstl::StdExceptUtil Util;

bsls::LogSeverity::Enum  severity;
char                     sourceFileNameBuf[8192];
int                      lineNumber;
char                     outBuf[8192];

bool                     caught;

void logMessageHandler(bsls::LogSeverity::Enum  severity,
                       const char              *file,
                       int                      line,
                       const char              *message)
{
    namespace TC = BSLSTL_STDEXCEPTUTIL_TEST_CASE_2;

    TC::severity = severity;
    strcpy(TC::sourceFileNameBuf, file);
    TC::lineNumber = line;
    strcpy(TC::outBuf, message);

    if (veryVerbose) {
        P_(severity);    P_(file);    P_(line);    P(message);
    }
}

void clear()
{
    Util::PreThrowHook segfault;    // calling this function will segfault
    memset(&segfault, 0xa5, sizeof(segfault));
    Util::setRuntimeErrorHook(segfault);
    Util::setLogicErrorHook(segfault);
    Util::setDomainErrorHook(segfault);
    Util::setInvalidArgumentHook(segfault);
    Util::setLengthErrorHook(segfault);
    Util::setOutOfRangeHook(segfault);
    Util::setRangeErrorHook(segfault);
    Util::setOverflowErrorHook(segfault);
    Util::setUnderflowErrorHook(segfault);

    memset(&TC::severity, 0xa5, sizeof(TC::severity));
    memset(TC::sourceFileNameBuf, 0xa5, sizeof(TC::sourceFileNameBuf));
    TC::lineNumber = -1;
    memset(TC::outBuf, 0xa5, sizeof(TC::outBuf));

    TC::caught = false;
}

void check(const char *exceptionName, const char *message)
{
    ASSERT(bsls::LogSeverity::e_FATAL == TC::severity);
    char *pc = strstr(TC::sourceFileNameBuf, ".cpp");
    ASSERT(pc);
    strcpy(pc, ".t.cpp");
    ASSERT(!strcmp(TC::sourceFileNameBuf, __FILE__));
    ASSERT(0 < TC::lineNumber);
    char aboutBuf[256];
    sprintf(aboutBuf, "About to throw %s", exceptionName);
    ASSERTV(TC::outBuf, strstr(TC::outBuf, aboutBuf));
    ASSERTV(TC::outBuf, exceptionName, strstr(TC::outBuf, exceptionName));
    ASSERT(strstr(TC::outBuf, message));
    ASSERTV(TC::outBuf, strstr(TC::outBuf, "/bb/bin/showfunc.tsk "));

    // count blanks after showfunc

    unsigned count = 0;
    pc = strstr(outBuf, ".tsk ");
    ASSERT(pc);
    pc += 4;
    while ((pc = strchr(pc, ' '))) {
        ++count;
        ++pc;
    }
    ASSERT(6 <= count);
}

int top(const char *message)
{
    {
        try {
            TC::clear();
            Util::setRuntimeErrorHook(&Util::logCheapStackTrace);
            Util::throwRuntimeError(message);
            ASSERT(0);
        }
        catch (const std::runtime_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::runtime_error", message);
    }

    {
        try {
            TC::clear();
            Util::setLogicErrorHook(&Util::logCheapStackTrace);
            Util::throwLogicError(message);
            ASSERT(0);
        }
        catch (const std::logic_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::logic_error", message);
    }

    {
        try {
            TC::clear();
            Util::setDomainErrorHook(&Util::logCheapStackTrace);
            Util::throwDomainError(message);
            ASSERT(0);
        }
        catch (const std::domain_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::domain_error", message);
    }

    {
        try {
            TC::clear();
            Util::setInvalidArgumentHook(&Util::logCheapStackTrace);
            Util::throwInvalidArgument(message);
            ASSERT(0);
        }
        catch (const std::invalid_argument& exc)
        {
            TC::caught = true;
        }
        TC::check("std::invalid_argument", message);
    }

    {
        try {
            TC::clear();
            Util::setLengthErrorHook(&Util::logCheapStackTrace);
            Util::throwLengthError(message);
            ASSERT(0);
        }
        catch (const std::length_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::length_error", message);
    }

    {
        try {
            TC::clear();
            Util::setOutOfRangeHook(&Util::logCheapStackTrace);
            Util::throwOutOfRange(message);
            ASSERT(0);
        }
        catch (const std::out_of_range& exc)
        {
            TC::caught = true;
        }
        TC::check("std::out_of_range", message);
    }

    {
        try {
            TC::clear();
            Util::setRangeErrorHook(&Util::logCheapStackTrace);
            Util::throwRangeError(message);
            ASSERT(0);
        }
        catch (const std::range_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::range_error", message);
    }

    {
        try {
            TC::clear();
            Util::setOverflowErrorHook(&Util::logCheapStackTrace);
            Util::throwOverflowError(message);
            ASSERT(0);
        }
        catch (const std::overflow_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::overflow_error", message);
    }

    {
        try {
            TC::clear();
            Util::setUnderflowErrorHook(&Util::logCheapStackTrace);
            Util::throwUnderflowError(message);
            ASSERT(0);
        }
        catch (const std::underflow_error& exc)
        {
            TC::caught = true;
        }
        TC::check("std::underflow_error", message);
    }

    return 1;
}

template <class FUNC_PTR>
int recurser(int *depth, FUNC_PTR func, const char *message)
    // Recurse to the specified 'depth' and then call
    // 'testThrowingWithCheapStackTrace'.
{
    const int depthIn = *depth;

    int rc = --*depth <= 0
           ? (*bsls::BslTestUtil::makeFunctionCallNonInline(func))(message)
           : (*bsls::BslTestUtil::makeFunctionCallNonInline(
                                   &recurser<FUNC_PTR>))(depth, func, message);
    *depth += rc;

    ASSERT(depthIn == *depth);

    return 1;
}

}  // close namespace BSLSTL_STDEXCEPTUTIL_TEST_CASE_2

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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
     case 2: {
        // --------------------------------------------------------------------
        // HOOK / STACKTRACE TEST
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
        namespace TC = BSLSTL_STDEXCEPTUTIL_TEST_CASE_2;

        bsls::Log::setLogMessageHandler(TC::logMessageHandler);

        int depth = 5;
        TC::recurser(&depth, &TC::top, "test case 2");
        ASSERT(5 == depth);
#endif
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
