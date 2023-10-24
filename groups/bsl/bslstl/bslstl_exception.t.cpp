// bslstl_exception.t.cpp                                             -*-C++-*-
#include <bslstl_exception.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The component under test defines single function 'bsl::uncaught_exceptions'
// that returns the number of uncaught exceptions on the current thread.
//
// ----------------------------------------------------------------------------
//
// [ 3] USAGE EXAMPLE
// [ 2] NESTED EXCEPTIONS
// [ 1] BREATHING TEST

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

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators


//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

bool unwinding_in_progress;

struct ExceptionAware {
    ExceptionAware() : d_exception_count(bsl::uncaught_exceptions()) {}
    ~ExceptionAware();
    int d_exception_count;
};

ExceptionAware::~ExceptionAware () {
    if (bsl::uncaught_exceptions() > d_exception_count) {
        // The stack is being unwound
        ASSERT(unwinding_in_progress);
    }
    else {
        // The object is being destroyed normally
        ASSERT(!unwinding_in_progress);
    }
}


//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct ExceptionCounter {
    ExceptionCounter() : d_expected(bsl::uncaught_exceptions()) {}
    ExceptionCounter(int expected_count) : d_expected(expected_count) {}
    ~ExceptionCounter() {
        ASSERT(d_expected == bsl::uncaught_exceptions());
        }
    int d_expected;
};


struct ThrowsWithinDestructor {
    ThrowsWithinDestructor () : d_counter(0) {}
    ThrowsWithinDestructor(int expected_count) : d_counter(expected_count) {}
    ~ThrowsWithinDestructor() {
        ASSERT(d_counter == bsl::uncaught_exceptions());
        try {
            ExceptionCounter ec (d_counter + 1);
            throw 23;
            }
        catch (const int &) {}
        }
    int d_counter;
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3; (void)veryVerbose;
    veryVeryVerbose     = argc > 4; (void)veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        // No exception handling
        {
            unwinding_in_progress = false;
            ExceptionAware exa;
            (void) exa;
        }

        // Exception in flight
        try {
            unwinding_in_progress = true;
            ExceptionAware exa;
            (void) exa;
            throw 23;
        }
        catch (const int &) {}

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // NESTED EXCEPTIONS
        //
        // Concerns:
        //: 1 While an exception is "in-flight", throwing and catching another
        //:   exception does not call terminate, and the fact that two
        //:   exceptions are active is reflected in the return value of
        //:   'bsl::uncaught_exceptions'.
        //
        // Plan:
        //: 1 Throw and catch an exception in the destructor of a class.  Throw
        //:   an exception so that the class will be destroyed during stack
        //:   unwinding.  Verify the value of 'bsl::uncaught_exceptions' (C-1)
        //
        // Testing:
        //   NESTED EXCEPTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nNESTED EXCEPTIONS"
                            "\n=================\n");

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) ||  \
    defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) ||                  \
    defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC) ||                 \
    defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)

        try {
            ThrowsWithinDestructor td(1);
            throw "Twentythree";
            }
        catch (const char *) {}
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 'bsl::uncaught_exceptions' returns an 'int'.
        //: 2 'bsl::uncaught_exceptions' does not throw exceptions.
        //: 3 The 'bsl::uncaught_exceptions' returns 0 when no exceptions are
        //:   active.
        //: 4 The 'bsl::uncaught_exceptions' returns 1 when a single exception
        //    is active.
        //
        // Plan:
        //: 1 Check the return type and noexcept status of
        //:   'bsl::uncaught_exceptions' if possible.  (C-1,2)
        //:
        //: 2 Verify the return value of 'bsl::uncaught_exceptions' during
        //:   normal operation and while processing an exception.  (C-3,4)
        //:
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        ASSERT((
              bsl::is_same<int, decltype(bsl::uncaught_exceptions())>::value));
#endif
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        ASSERT(noexcept(bsl::uncaught_exceptions()));
#endif

        // not unwinding
        ASSERT(0 == bsl::uncaught_exceptions());

        // while unwinding
        try {
            ExceptionCounter ctr(1);
            throw 23;
        }
        catch(const int &) {}

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
// Copyright 2023 Bloomberg Finance L.P.
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
