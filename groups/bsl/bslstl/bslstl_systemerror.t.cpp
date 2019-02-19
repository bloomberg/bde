// bslstl_systemerror.t.cpp                                           -*-C++-*-
#include <bslstl_systemerror.h>

#include <bsls_bsltestutil.h>

#include <bslstl_error.h>

#include <stdexcept>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The component under test defines a systematic way to create sets of error
// categories and error codes and conditions of those categories.  Error
// categories, in addition to serving as tags, are also responsible for
// providing string versions of error values.
//
// ----------------------------------------------------------------------------
//
// SYSTEM ERROR METHODS
// [ 2] system_error(error_code, const std::string&)
// [ 2] system_error(error_code, const char *)
// [ 2] system_error(error_code)
// [ 2] system_error(int, const error_category&, const std::string&)
// [ 2] system_error(int, const error_category&, const char *)
// [ 2] system_error(int, const error_category&)
// [ 2] const error_code& code() const
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//            GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {
namespace bsls {

template <>
void BslTestUtil::callDebugprint(const bsl::system_error&  error,
                                 const char               *leadingString,
                                 const char               *trailingString)
    // Print a descrriptive form of the specified 'error' bracketed by the
    // specified 'leadingString' and 'trailingString'.
{
    printf("%ssystem_error<%d, '%s', %s>%s",
           leadingString,
           error.code().value(),
           error.what(),
           error.code().category().name(),
           trailingString);
}

}  // close namespace bsls
}  // close enterprise namespace

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2;
    int veryVerbose         = argc > 3; (void)veryVerbose;
    int veryVeryVerbose     = argc > 4; (void)veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

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

///Example 1: Adding Annotation to an Error
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to add an informative message when a system error occurs and
// include that as part of an exception that we throw when reporting the error.
// We can use 'bsl::system_error' to do that.
//
// First, reset 'errno' to avoid detecting old problems.
//..
    errno = 0;
//..
// Then, do something that will fail and set 'errno'.
//..
    sqrt(-3.5);
//..
// Next, check that 'errno' was actually set.
//..
    ASSERT(EDOM == errno);
//..
//  Finally, throw an annotated exception and verify the annotaion and the
//  error code stored within it.
//..
    try {
        throw bsl::system_error(errno, generic_category(), "sqrt(-3.5)");
    }
    catch (std::runtime_error& e) {
        ASSERT(0 != strstr(e.what(), "sqrt(-3.5)"));
        try {
            throw;
        }
        catch (bsl::system_error& e) {
            ASSERT(static_cast<int>(bsl::errc::argument_out_of_domain) ==
                   e.code().value());
            ASSERT(&generic_category() == &e.code().category());
        }
    }
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SYSTEM ERROR METHODS
        //   Test the metthods of the 'bsl::system_error' class.
        //
        // Concerns:
        //: 1 The constructors build an object with the correct code.
        //: 2 The optional 'what' parameter can be observed.
        //
        // Plan:
        //: 1 Construct objects with each of the constructors.  (C-1)
        //: 2 Retrieve the code and message to verify preservation.  (C-2)
        //
        // Testing:
        //   system_error(error_code, const std::string&)
        //   system_error(error_code, const char *)
        //   system_error(error_code)
        //   system_error(int, const error_category&, const std::string&)
        //   system_error(int, const error_category&, const char *)
        //   system_error(int, const error_category&)
        //   const error_code& code() const
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SYSTEM ERROR METHODS"
                   "\n============================\n");

        error_code ec(static_cast<int>(errc::io_error), generic_category());
#ifdef BSLS_PLATFORM_OS_WINDOWS
        const char *io_error = "io error";
#else
        const char *io_error = strerror(EIO);
#endif

        {
            system_error        mX(ec, std::string("@@@"));
            const system_error& X = mX;
            ASSERT(ec == X.code());
            ASSERT(strstr(X.what(), io_error));
            ASSERT(strstr(X.what(), "@@@"));
        }

        {
            system_error        mX(ec, "@@@");
            const system_error& X = mX;
            ASSERT(ec == X.code());
            ASSERT(strstr(X.what(), io_error));
            ASSERT(strstr(X.what(), "@@@"));
        }

        {
            system_error        mX(ec);
            const system_error& X = mX;
            ASSERT(EIO == X.code().value());
            ASSERT(strstr(X.what(), io_error));
        }

        {
            system_error        mX(static_cast<int>(errc::io_error),
                                   generic_category(),
                                   std::string("@@@"));
            const system_error& X = mX;
            ASSERT(ec == X.code());
            ASSERT(strstr(X.what(), io_error));
            ASSERT(strstr(X.what(), "@@@"));
        }

        {
            system_error mX(
                static_cast<int>(errc::io_error), generic_category(), "@@@");
            const system_error& X = mX;
            ASSERT(ec == X.code());
            ASSERT(strstr(X.what(), io_error));
            ASSERT(strstr(X.what(), "@@@"));
        }

        {
            system_error mX(static_cast<int>(errc::io_error),
                            generic_category());
            const system_error& X = mX;
            ASSERT(ec == X.code());
            ASSERT(strstr(X.what(), io_error));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create four test objects by using the default, initializing, and
        //:   copy constructors.
        //:
        //: 2 Exercise the basic value-semantic methods and the equality
        //:   operators using these test objects.
        //:
        //: 3 Invoke the primary manipulator, copy constructor, and assignment
        //:   operator without and with aliasing.
        //:
        //: 4 Use the basic accessors to verify the expected results.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        const char *io_error = "io error";
#else
        const char *io_error = strerror(EIO);
#endif
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 1. Create an object x1. to VA).\n");
        const error_category &C1 = generic_category();

        error_code        mD1(static_cast<int>(errc::io_error), C1);
        const error_code& D1 = mD1;

        system_error        mS1(D1, "breathing");
        const system_error& S1 = mS1;
        if (verbose) { T_ P(S1) }

        if (verbose) printf("\ta. Check initial state of x1.\n");
        ASSERT(EIO == S1.code().value());
        ASSERT(&C1 == &S1.code().category());
        ASSERT(0 == strcmp("generic", S1.code().category().name()));
        ASSERT(strstr(S1.what(), "breathing"));
        ASSERT(strstr(S1.what(), io_error));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 2. Create an object x2 (copy from x1).\n");
        system_error        mS2(S1);
        const system_error& S2 = mS2;
        if (verbose) { T_ P(S2) }

        if (verbose) printf("\ta. Check the initial state of x2.\n");
        ASSERT(strstr(S2.what(), "breathing"));
        ASSERT(strstr(S2.what(), io_error));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 7. Assign x2 = x1.\n");

        mS2 = S1;
        if (verbose) { T_ P(S2) }

        if (verbose) printf("\ta. Check new state of x2.\n");
        ASSERT(strstr(S2.what(), "breathing"));
        ASSERT(strstr(S2.what(), io_error));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) printf("\n 9. Assign x1 = x1 (aliasing).\n");

        mS1 = S1;
        if (verbose) { T_ P(S1) }

        if (verbose) printf("\ta. Check new state of x1.\n");
        ASSERT(strstr(S1.what(), "breathing"));
        ASSERT(strstr(S1.what(), io_error));
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
// Copyright 2019 Bloomberg Finance L.P.
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
