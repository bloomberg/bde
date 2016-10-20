// bslmt_entrypointfunctoradapter.t.cpp
#include <bslmt_entrypointfunctoradapter.h>

#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bslma_testallocator.h>
#include <bslma_default.h>

#include <bsl_string.h>

#include <stdio.h>   // printf

using namespace BloombergLP;
using namespace std;

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
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Wrapping a C++ Invokable Type
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have an existing interface for invoking a C-linkage function and
// passing a void* argument to it.  This situation may arise when starting
// threads or in general when registering a C-style callback.  A simplistic
// example of such a function is:
//..
    extern "C" {
       typedef void *(*CallbackFunction)(void*);
    }

    void *executeWithArgument(CallbackFunction funcPtr, void *argument)
    {
       return funcPtr(argument);
    }
//..
// In this example, we want to use this interface to invoke a C++-style
// functor.  Our approach will be to use
// 'bslmt_EntryPointFunctorAdapter_invoker' as the C-linkage callback function,
// and a dynamically allocated value of 'EntryPointFunctorAdapter' as the
// 'void*' argument.
//
// First, we define a C++ functor type.  This type implements the job of
// counting the number of words in a string held by value.
//..
    class WordCountJob {
        // DATA
        bsl::string  d_message;
        int         *d_result_p; // held, not owned

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(WordCountJob,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        WordCountJob(const bslstl::StringRef&  message,
                     int                      *result,
                     bslma::Allocator         *basicAllocator = 0);
            // Create a new functor that, upon execution, counts the number of
            // words (contiguous sequences of non-space characters) in the
            // specified 'message' and stores the count in the specified
            // 'result' address.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        WordCountJob(const WordCountJob&  original,
                     bslma::Allocator    *basicAllocator = 0);
            // Create a new functor that performs the same calculation as the
            // specified 'other' functor.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // MANIPULATORS
        void operator()();
            // Count the number of words in the message and store the count in
            // the address specified on construction.
    };

    inline
    WordCountJob::WordCountJob(const bslstl::StringRef&  message,
                               int                      *result,
                               bslma::Allocator         *basicAllocator)
    : d_message(message, basicAllocator)
    , d_result_p(result)
    {}

    inline
    WordCountJob::WordCountJob(const WordCountJob&  original,
                               bslma::Allocator    *basicAllocator)
    : d_message(original.d_message, basicAllocator)
    , d_result_p(original.d_result_p)
    {}

    void WordCountJob::operator()()
    {
        bool inWord = false;
        *d_result_p = 0;
        for (unsigned i = 0; i < d_message.length(); ++i) {
            if (isspace(d_message[i])) {
                inWord = false;
            } else if (!inWord) {
                inWord = true;
                ++(*d_result_p);
            }
        }
    }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int             test = argc > 1 ? atoi(argv[1]) : 0;
//  const bool         verbose = argc > 2;    // unused
//  const bool     veryVerbose = argc > 3;    // unused
    const bool veryVeryVerbose = argc > 4;

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //: The usage example must compile and run as shown.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------
        {
//  <<<<<<<<
//..
// Next, we dynamically allocate an 'EntryPointFunctorAdapter' wrapping an
// instance of this functor:
//..
    int result = 0;
    WordCountJob job("The quick brown fox jumped over the lazy dog.",
                     &result);

    bslma::ManagedPtr<
        bslmt::EntryPointFunctorAdapter<WordCountJob> > threadData;
    bslmt::EntryPointFunctorAdapterUtil::allocateAdapter(&threadData,
                                                         job,
                                                         "");
//..
// Finally, we use 'bslmt_EntryPointFunctorAdapter_invoker' to invoke the job
// in the context of a C-linkage function.  Note that
// 'bslmt_EntryPointFunctorAdapter_invoker' will deallocate the adapter object
// and the contained invokable job after executing it, so we must release the
// adapter from memory management via 'ManagedPtr'.  (In general, system APIs
// that register callbacks may fail; newly allocated adapters are loaded into
// 'ManagedPtr' to aid in proper error and exception handling, outside the
// scope of this example.)
//..
    executeWithArgument(bslmt_EntryPointFunctorAdapter_invoker,
                        threadData.ptr());
    threadData.release();
    ASSERT(9 == result);
//..
//  >>>>>>>>
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 != defaultAllocator.numBlocksTotal());
      }  break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // No memory should be ever used by the global allocator in this test
    // driver.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
