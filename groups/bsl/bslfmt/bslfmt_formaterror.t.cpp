// bslstl_formaterror.t.cpp                                           -*-C++-*-
#include <bslfmt_formaterror.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements `format_error`, a standard-compliant
// implementation of 'std::format_error'.  The type has no Primary Manipulators
// and has a single Basic Accessor that is implemented by its base class
// therefore the testing of this type while similar to a value-semantic type
// will not include all tests a real value-semantic type would have.
//
// Note that the "Accessor" `what` is a virtual function, hence this class is
// only *similar* to a value-semantic type with a string member, but also very
// different as it serves in a type-hierarchy.  Most notably there is no
// equality operator, which signifies the fact that while we do have what looks
// like an attribute-class with an unconstrained string as its sole content it
// is in fact not to be treated as a value.
//
// Also note that the Accessor testing is present for consistency, literally
// every other test case also tests the Accessor.
//
// Notice that the type is not allocator-aware so allocator related testing is
// also not applicable.
//
// Basic Accessor:
//  - `what() const`
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] format_error();
// [ 2] format_error(const std:string&);
// [ 2] format_error(const bsl::string&);
// [ 2] ~format_error();
// [ 3] format_error(const format_error &);
//
// MANIPULATORS
// [ 5] operator=(const format_error &);
//
// ACCESSORS
// [ 4] const char *what() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int          test    = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

///Example: Throw an exception
/// - - - - - - - - - - - - -
//
// Typically a `format_error` exception this would be thrown from the `format`
// or `vformat` functions.  However, as this is at the very bottom of the
// dependency hierarchy the usage example cannot accurately reflect that case
// in our usage example.
//
// ```
     bool formatErrorCaught = false;
     try {
         throw bsl::format_error("Error message");
     }
     catch (const bsl::format_error &exc) {
         formatErrorCaught = true;
         ASSERT(0 == strcmp(exc.what(), "Error message"));
     }
     ASSERT(formatErrorCaught);
// ```
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        //
        // Concerns:
        // 1. The copy assignment copies the `what` value.
        //
        // 2. We have no complex because the copy assignment itself is
        //    implemented in the base class, which is a Standard Library class
        //    so we do not need to test for aliasing and other such concern.
        //
        // Plan:
        // 1. Construct const `format_error` objects, copy assign them to
        //    non-const objects, verify that the copy has the same `what`
        //    value.
        //
        // Testing:
        //   format_error& operator=(const format_error &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nASSIGNMENT OPERATOR"
                          "\n===================");

        struct {
            int         d_line;
            const char *d_msg;
        } k_DATA[] = {
            { L_, ""                   },
            { L_, "Error message"      },
            { L_, "Errorer messager"   },
            { L_, "Errorest messagest" },
        };
        size_t k_NUM_DATA = sizeof k_DATA / sizeof *k_DATA;

        for (size_t ti = 0; ti < k_NUM_DATA; ++ti) {
            const int          LINE = k_DATA[ti].d_line;
            const char * const MSG  = k_DATA[ti].d_msg;

            if (veryVerbose) { P_(LINE) P(MSG); }

            const bsl::format_error OBJ(MSG);
            bsl::format_error       copied("original value");

            copied = OBJ;

            ASSERTV(LINE, OBJ.what(), copied.what(),
                    0 == strcmp(OBJ.what(), copied.what()));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ACCESSORS
        //
        // Concerns:
        // 1. This test case is present for consistency only, since every other
        //    test case have already tested `what`, and `what` is implemented
        //    in a standard library class we inherit from we do not actually
        //    have any actual concerns.
        //
        // Plan:
        // 1. Just a simple table-based test with a few `what` messages that we
        //    verify get properly stored and retrieved via `what`.
        //
        // Testing:
        //   const char *what() const;
        // --------------------------------------------------------------------

        if (verbose) puts("\nACCESSORS"
                          "\n=========");

        struct {
            int         d_line;
            const char *d_msg;
        } k_DATA[] = {
            { L_, ""                   },
            { L_, "Error message"      },
            { L_, "Errorer messager"   },
            { L_, "Errorest messagest" },
        };
        size_t k_NUM_DATA = sizeof k_DATA / sizeof *k_DATA;

        for (size_t ti = 0; ti < k_NUM_DATA; ++ti) {
            const int          LINE = k_DATA[ti].d_line;
            const char * const MSG  = k_DATA[ti].d_msg;

            if (veryVerbose) { P_(LINE) P(MSG); }

            const bsl::format_error OBJ(MSG);
            ASSERTV(LINE, OBJ.what(), 0 == strcmp(OBJ.what(), MSG));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. The copy constructor copies the `what` value.
        //
        // 2. We have no complex because the copying itself is implemented in
        //    the base class, which is a Standard Library class so we do not
        //    need to test for aliasing and other such concern.
        //
        // Plan:
        // 1. Construct `format_error` objects, copy them from a `const` alias,
        //    verify that the copy has the same `what` value.
        //
        // 2. `throw` `bsl::format_error` objects, which action copies them,
        //    verify that the caught object has the same `what` value.
        //
        // Testing:
        //   format_error(const format_error &);
        // --------------------------------------------------------------------

        if (verbose) puts("\nCOPY CONSTRUCTOR"
                          "\n================");

        if (veryVerbose) puts("Direct copy");
        {
            bsl::format_error        mObj("Some message");
            const bsl::format_error& OBJ(mObj);
            const bsl::format_error  CPY(OBJ);

            ASSERTV(CPY.what(), OBJ.what(),
                    0 == strcmp(CPY.what(), OBJ.what()));
        }

        if (veryVerbose) puts("copied-by-`throw` verification");
        try {
            throw bsl::format_error("Error message");
        }
        catch (const bsl::format_error& exc) {
            ASSERT(bsl::string("Error message") == exc.what());
        }

        try {
            const bsl::format_error exc1("Error message");
            bsl::format_error exc2(exc1);
            throw exc2;
        }
        catch (const bsl::format_error& exc) {
            ASSERT(bsl::string("Error message") == exc.what());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTORS AND DESTRUCTOR
        //   Ensure that we can use the constructors to create an object and
        //   use the destructor to destroy it safely.
        //
        // Concerns:
        // 1. The error message constructor argument is copied into the object
        //    and may be retrieved using the `what` method.
        //
        // 2. The object destruction is not implemented by us, so we do not
        //    need to test (and cannot, due to the lack of allocator
        //    support) for the proper releasing of the resources.
        //
        // Plan:
        // 1. Create objects using different types of constructor arguments,
        //    verify that the `what` message is equal to the constructor
        //    argument.
        //
        // Testing:
        //   CONSTRUCTORS AND DESTRUCTORs
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONSTRUCTORS AND DESTRUCTOR"
                          "\n===========================");

        if (veryVerbose) puts("\tString literal");
        {
            bsl::format_error        mObj("Literal message");
            const bsl::format_error& OBJ(mObj);

            ASSERTV(OBJ.what(),
                    0 == strcmp(OBJ.what(), "Literal message"));
        }

        if (veryVerbose) puts("\tStandard string");
        {
            const std::string        msg = "std::message";
            bsl::format_error        mObj(msg);
            const bsl::format_error& OBJ(mObj);

            ASSERTV(OBJ.what(), msg.c_str(), OBJ.what() == msg);
        }

        if (veryVerbose) puts("\tbsl::string");
        {
            const bsl::string        msg = "bsl::message";
            bsl::format_error        mObj(msg);
            const bsl::format_error& OBJ(mObj);

            ASSERTV(OBJ.what(), msg.c_str(), OBJ.what() == msg);
        }

        if (veryVerbose) puts("\tEmpty string");
        {
            bsl::format_error        mObj("");
            const bsl::format_error& OBJ(mObj);

            ASSERTV(OBJ.what(), 0 == strcmp(OBJ.what(), ""));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create an object, throw it, and verify its `what` accessor.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        try {
            throw bsl::format_error("Error message");
        }
        catch (const bsl::format_error& exc) {
            ASSERT(bsl::string("Error message") == exc.what());
        }
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
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
