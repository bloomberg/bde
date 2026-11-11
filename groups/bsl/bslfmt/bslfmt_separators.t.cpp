// bslfmt_separators.t.cpp                                            -*-C++-*-
#include <bslfmt_separators.h>

#include <bslstl_stringview.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`
#include <string.h>  // `strlen`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a class template that is a namespace for the
// bracket and separator strings used by the range, pair and tuple formatters.
// Each class method is tested against its expected text for both `char` and
// `wchar_t`.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] basic_string_view<t_CHAR> openingParen();
// [ 2] basic_string_view<t_CHAR> closingParen();
// [ 2] basic_string_view<t_CHAR> openingBrace();
// [ 2] basic_string_view<t_CHAR> closingBrace();
// [ 2] basic_string_view<t_CHAR> openingSquareBracket();
// [ 2] basic_string_view<t_CHAR> closingSquareBracket();
// [ 2] basic_string_view<t_CHAR> commaSeparator();
// [ 2] basic_string_view<t_CHAR> colonSeparator();
// ----------------------------------------------------------------------------
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace u {

/// Return `true` if the specified `actual` consists of the characters of the
/// specified null-terminated `expected`, each converted to `t_CHAR`, and
/// `false` otherwise.
template <class t_CHAR>
bool equals(const bsl::basic_string_view<t_CHAR>& actual,
            const char                           *expected)
{
    if (actual.size() != strlen(expected)) {
        return false;                                                 // RETURN
    }
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != static_cast<t_CHAR>(expected[i])) {
            return false;                                             // RETURN
        }
    }
    return true;
}

/// Verify each class method of `bslfmt::Separators<t_CHAR>` against its
/// expected text, and verify that two calls refer to the same characters.
template <class t_CHAR>
void testClassMethods()
{
    typedef bslfmt::Separators<t_CHAR>         Obj;
    typedef bsl::basic_string_view<t_CHAR> (*Method)();

    static const struct {
        int         d_line;
        Method      d_method;
        const char *d_expected_p;
    } DATA[] = {
        //LINE  METHOD                       EXPECTED
        //----  ---------------------------  --------
        { L_,   &Obj::openingParen,          "("      },
        { L_,   &Obj::closingParen,          ")"      },
        { L_,   &Obj::openingBrace,          "{"      },
        { L_,   &Obj::closingBrace,          "}"      },
        { L_,   &Obj::openingSquareBracket,  "["      },
        { L_,   &Obj::closingSquareBracket,  "]"      },
        { L_,   &Obj::commaSeparator,        ", "     },
        { L_,   &Obj::colonSeparator,        ": "     },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE     = DATA[ti].d_line;
        const Method      METHOD   = DATA[ti].d_method;
        const char *const EXPECTED = DATA[ti].d_expected_p;

        const bsl::basic_string_view<t_CHAR> first  = METHOD();
        const bsl::basic_string_view<t_CHAR> second = METHOD();

        ASSERTV(LINE, EXPECTED, equals(first, EXPECTED));
        ASSERTV(LINE, EXPECTED, first.data() == second.data());
    }
}

}  // close namespace u

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS
        //
        // Concerns:
        // 1. Each class method returns a string view of its expected text,
        //    for both `char` and `wchar_t`.
        //
        // 2. The returned characters have static storage duration: every
        //    call refers to the same characters.
        //
        // Plan:
        // 1. Using the table-driven technique, call each class method twice,
        //    compare the result with the expected text, and verify that both
        //    results refer to the same address.  Do this for both `char` and
        //    `wchar_t`.  (C-1..2)
        //
        // Testing:
        //   basic_string_view<t_CHAR> openingParen();
        //   basic_string_view<t_CHAR> closingParen();
        //   basic_string_view<t_CHAR> openingBrace();
        //   basic_string_view<t_CHAR> closingBrace();
        //   basic_string_view<t_CHAR> openingSquareBracket();
        //   basic_string_view<t_CHAR> closingSquareBracket();
        //   basic_string_view<t_CHAR> commaSeparator();
        //   basic_string_view<t_CHAR> colonSeparator();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CLASS METHODS"
                            "\n=====================\n");

        if (verbose) printf("\t`char`\n");
        u::testClassMethods<char>();

        if (verbose) printf("\t`wchar_t`\n");
        u::testClassMethods<wchar_t>();
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
        // 1. Verify the text returned by two of the class methods.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bslfmt::Separators<char> Obj;

        ASSERT(bsl::string_view("(")  == Obj::openingParen());
        ASSERT(bsl::string_view(", ") == Obj::commaSeparator());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
