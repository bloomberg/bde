// bdljsn_jsonliterals.t.cpp                                          -*-C++-*-
#include <bdljsn_jsonliterals.h>

#include <bdljsn_error.h>
#include <bdljsn_json.h>
#include <bdljsn_jsonutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>         // to verify that we do not
#include <bslma_testallocatormonitor.h>  // allocate any memory

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::ends;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component provides a namespace containing a user-defined literal
// operator '_json' for 'bdljsn::Json' objects.  This facility is a simple
// wrapper around already tested facilities in 'bdljsn_json' and
// 'bdljsn_jsonutil'.
// ----------------------------------------------------------------------------
// [ 1] bdljsn::Json operator "" _json(const char *, bsl::size_t );
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

#define WS "   \t       \n      \v       \f       \r       "

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR)   BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR)   BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)        BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)        BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_INVOKE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW(EXPR)


// ============================================================================
//                   MACROS FOR TESTING WORKAROUNDS
// ----------------------------------------------------------------------------

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonUtil Util;
typedef bdljsn::Json     Json;
typedef bdljsn::Error    Error;

// These otherwise standard test driver scaffolding variables are global in
// this test driver so they can be accessed in helper functions.
bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;



// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    // These otherwise standard test driver scaffolding variables are global in
    // this test driver so they can be accessed in helper functions.
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose; (void) veryVeryVerbose; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);


    switch (test) {
      case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   First usage example extracted from component header file.
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

        if (verbose)
            cout << "\nUSAGE EXAMPLE"
                 << "\n=============" << endl;

        if (verbose)
            cout << "Testing component doc example" << endl;
        {
#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
    using namespace bdljsn::JsonLiterals;
    bdljsn::Json json = R"({"price": 2.1})"_json;

            if (veryVerbose) {
                P(json);
            }
#endif

        }

        if (verbose)
            cout << "Testing usage example 1" << endl;
        {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Creating a 'bdljsn::Json' Object with a User Defined Literal
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides a namespace in which user-defined literal operations
// for 'bdljsn::Json' are defined.
//
// First, we use the appropriate namespace:
//..
    using namespace bdljsn::JsonLiterals;
//..
// Then we create a 'bdljsn::Json' object:
//..
    bdljsn::Json json = R"({ "number": 4, "array": [0, 2, null] })"_json;

    ASSERT(bdljsn::JsonType::e_NUMBER == json["number"].type());
    ASSERT(bdljsn::JsonType::e_ARRAY  == json["array"].type());
//..
// Notice that the user-defined literal operators will unconditionally invoke
// the 'bsls::Assert' handler if the supplied text is not valid JSON.
//..
        if (veryVerbose) {
            P(json);
        }
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING
        //   This case exercises the user-defined literal operator.
        //
        //   This operator is built on top of existing well tested components
        //   'bdljsn_json' and 'bdljsn_jsonutil', so we need only verify
        //   that the arguments are forward appropriately.
        //
        // Concerns:
        //: 1 That the user defined literal operator is exposed from the
        //:    appropriate namespace ('bdljsn::JsonLiterals')
        //:
        //: 2 That a user defined literal text containing valid JSON will
        //:   result in a corresponding 'bdljsn::Json' object.
        //:
        //: 3 That memory for the user defined literal JSON is allocated
        //:   from the global allocator.
        //:
        //: 4 That a user defined literal that does not contain valid JSON
        //:   invokes the 'bsls::Assert' failure handler.
        //
        // Plan:
        //: 1 Define a using declaration for 'bdljsn::JsonLiterals' and then
        //:   define a couple JSON literals.  Verify the 'bdljsn::Json'
        //:   resulting 'bdljsn::Json' objects meet expectations.
        //:
        //: 2 Define a using declaration for 'bdljsn::JsonLiterals' and
        //:   the define an invalid JSON literal.  Use 'bsls_asserttest'
        //:   machinery to verify that the 'bsls::Assert' failure handler
        //:   is invoked.
        //
        // Testing:
        //   bdljsn::Json operator "" _json(const char *, bsl::size_t );
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nBREATHING TEST"
                 << "\n==============" << endl;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
        if (verbose) {
            cout << "\tTest with valid JSON text" << endl;
        }
        {
            {
                using namespace bdljsn::JsonLiterals;
                bdljsn::Json value = "false"_json;

                ASSERTV(bdljsn::JsonType::e_BOOLEAN == value.type());
                ASSERTV(false == value.theBoolean());
            }
            {
                using namespace bdljsn::JsonLiterals;
                bdljsn::Json value =
                    R"({ "number": 4, "array": [0, 2, null] })"_json;

                ASSERTV(bdljsn::JsonType::e_OBJECT == value.type());
                ASSERTV(4.0 == value["number"].asDouble());
                ASSERTV(bdljsn::JsonType::e_ARRAY == value["array"].type());
            }
            {
                bslma::Allocator *oldGa = bslma::Default::globalAllocator();
                bslma::TestAllocator ga, da;

                bslma::TestAllocatorMonitor dam(&da), gam(&ga);

                {
                    bslma::DefaultAllocatorGuard dag(&da);
                    bslma::Default::setGlobalAllocator(&ga);

                    ASSERT(dam.isInUseSame());
                    ASSERT(gam.isInUseSame());

                    using namespace bdljsn::JsonLiterals;
                    bdljsn::Json result =
                        R"({ "number": 4, "array": [0, 2, null] })"_json;

                    ASSERT(dam.isInUseSame());
                    ASSERT(gam.isInUseUp());

                    if (veryVerbose) {
                        P_(result);
                        P_(da.numBlocksInUse());
                        P(ga.numBytesInUse());
                    }
                }
                ASSERT(dam.isInUseSame());
                ASSERT(gam.isInUseSame());
                bslma::Default::setGlobalAllocator(oldGa);
            }
        }

        if (verbose) {
            cout << "\tTest with invalid JSON text" << endl;
        }
        {
            bsls::AssertTestHandlerGuard hG;


            using namespace bdljsn::JsonLiterals;
            ASSERT_PASS(bdljsn::Json a = "true"_json);
            ASSERT_INVOKE_FAIL(bdljsn::Json a = "bad"_json);
            ASSERT_INVOKE_FAIL(bdljsn::Json a = "{]"_json);
        }
#else
        if (verbose) cout << "User defined literals are not supported in this"
                             " build.\n";
        (void)aSsErT;  // Avoids unused local function warning
#endif
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

