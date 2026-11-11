// bslfmt_displaywidthutil.t.cpp                                      -*-C++-*-
#include <bslfmt_displaywidthutil.h>

#include <bslfmt_formaterror.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility that computes the display width of
// Unicode text.  The class methods of the utility are tested against tables
// of text of known width, for both `char` (UTF-8) and `wchar_t`, including
// text that is not well-formed Unicode.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] int displayWidth(const string_view& text);
// [ 2] int displayWidth(const wstring_view& text);
// [ 3] void findWidthLimitedPrefix(size_t *, int *, string_view, int);
// [ 3] void findWidthLimitedPrefix(size_t *, int *, wstring_view, int);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
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
//                         OTHER MACROS
// ----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define VERIFY_EXCEPTION_IS_THROWN(line, action)                              \
do {                                                                          \
    bool formatErrorWasCaught = false;                                        \
    try {                                                                     \
        action;                                                               \
    }                                                                         \
    catch(const bslfmt::format_error&) {                                      \
        formatErrorWasCaught = true;                                          \
    }                                                                         \
    ASSERTV(line, formatErrorWasCaught);                                      \
} while (false)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Measuring Text
///- - - - - - - - - - - - -
// Suppose we need to know how many columns a piece of text occupies, and how
// much of it fits within a given number of columns.
//
// First, we measure a string consisting of `a` followed by the CJK character
// U+4E2D, which is two columns wide:
// ```
    void run()
    {
        typedef bslfmt::DisplayWidthUtil Util;

        const char *text = "a\xe4\xb8\xad";

        ASSERT(3 == Util::displayWidth(text));
// ```
// Then, we find the longest prefix of the string that fits within two columns:
// ```
        std::size_t numCharacters = 0;
        int         width         = 0;

        Util::findWidthLimitedPrefix(&numCharacters, &width, text, 2);
// ```
// Finally, we observe that only `a` fits, because the CJK character would
// need a third column:
// ```
        ASSERT(1 == numCharacters);
        ASSERT(1 == width);
    }
// ```

}  // close namespace usage_example_1

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
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file
        //    compiles, links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate the usage example from the header into the test
        //    driver, remove leading comment characters, and replace `assert`
        //    with `ASSERT`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usage_example_1::run();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `findWidthLimitedPrefix`
        //
        // Concerns:
        // 1. The longest prefix whose width does not exceed the limit is
        //    found, and its number of characters and width are loaded.
        //
        // 2. A grapheme cluster is never split, and a cluster that would
        //    exceed the limit is excluded entirely.
        //
        // 3. A limit of zero, or empty text, gives an empty prefix.
        //
        // 4. Ill-formed text that is reached while searching for the prefix
        //    causes `bsl::format_error` to be thrown, while ill-formed text
        //    that is not reached does not.
        //
        // 5. The above holds for `wchar_t` as well as `char`.
        //
        // Plan:
        // 1. Using the table-driven technique, call `findWidthLimitedPrefix`
        //    on UTF-8 text with a variety of limits, and verify the loaded
        //    values.  (C-1..3)
        //
        // 2. Call it on ill-formed text with limits that do and do not reach
        //    the ill-formed part, and verify the result or the exception.
        //    (C-4)
        //
        // 3. Repeat a representative subset with `wchar_t` text.  (C-5)
        //
        // Testing:
        //   void findWidthLimitedPrefix(size_t *, int *, StringView, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `findWidthLimitedPrefix`"
                            "\n================================\n");

        typedef bslfmt::DisplayWidthUtil Util;

        if (veryVerbose) printf("\t`char`\n");
        {
            static const struct {
                int         d_line;
                const char *d_text_p;
                int         d_maxWidth;
                int         d_expNumCharacters;
                int         d_expWidth;
            } DATA[] = {
                //LINE  TEXT                    MAX  NUM  WIDTH
                //----  ----------------------  ---  ---  -----
                { L_,   "",                       5,   0,   0 },
                { L_,   "abc",                    0,   0,   0 },
                { L_,   "abc",                    2,   2,   2 },
                { L_,   "abc",                    3,   3,   3 },
                { L_,   "abc",                    9,   3,   3 },

                // `a` followed by U+4E2D (two columns wide).

                { L_,   "a\xe4\xb8\xad",          1,   1,   1 },
                { L_,   "a\xe4\xb8\xad",          2,   1,   1 },
                { L_,   "a\xe4\xb8\xad",          3,   4,   3 },

                // `g` followed by U+0308 (combining), then `h`.

                { L_,   "g\xcc\x88h",             1,   3,   1 },
                { L_,   "g\xcc\x88h",             2,   4,   2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *TEXT      = DATA[ti].d_text_p;
                const int   MAX       = DATA[ti].d_maxWidth;
                const int   EXP_NUM   = DATA[ti].d_expNumCharacters;
                const int   EXP_WIDTH = DATA[ti].d_expWidth;

                std::size_t numCharacters = 99;
                int         width         = 99;

                Util::findWidthLimitedPrefix(&numCharacters,
                                             &width,
                                             TEXT,
                                             MAX);

                ASSERTV(LINE, numCharacters,
                        static_cast<std::size_t>(EXP_NUM) == numCharacters);
                ASSERTV(LINE, width, EXP_WIDTH == width);
            }
        }

        if (veryVerbose) printf("\tIll-formed text\n");
        {
            std::size_t numCharacters = 0;
            int         width         = 0;

            // The search stops before reaching the ill-formed byte.

            Util::findWidthLimitedPrefix(&numCharacters,
                                         &width,
                                         "abc\xff",
                                         1);

            ASSERTV(numCharacters, 1u == numCharacters);
            ASSERTV(width,         1  == width);

#ifdef BDE_BUILD_TARGET_EXC
            VERIFY_EXCEPTION_IS_THROWN(
                L_,
                Util::findWidthLimitedPrefix(&numCharacters,
                                             &width,
                                             "abc\xff",
                                             3));
#endif
        }

        if (veryVerbose) printf("\t`wchar_t`\n");
        {
            // `a` followed by U+1F408 (two columns wide, outside the BMP),
            // which occupies one `wchar_t` in UTF-32, and two in UTF-16.

            const bsl::wstring_view TEXT(L"a\U0001F408");

            std::size_t numCharacters = 0;
            int         width         = 0;

            Util::findWidthLimitedPrefix(&numCharacters, &width, TEXT, 2);

            ASSERTV(numCharacters, 1u == numCharacters);
            ASSERTV(width,         1 == width);

            Util::findWidthLimitedPrefix(&numCharacters, &width, TEXT, 3);

            ASSERTV(numCharacters, TEXT.size() == numCharacters);
            ASSERTV(width,         3           == width);

            // An emoji ZWJ sequence is a single cluster, which is never split.

            const bsl::wstring_view EMOJI(L"\U0001F407\x200d\U0001F7E7");
            const bsl::wstring_view EMOJI_HELLO(
                                         L"\U0001F407\x200d\U0001F7E7hello");

            Util::findWidthLimitedPrefix(&numCharacters,
                                         &width,
                                         EMOJI_HELLO,
                                         1);

            ASSERTV(numCharacters, 0u == numCharacters);
            ASSERTV(width,         0  == width);

            Util::findWidthLimitedPrefix(&numCharacters,
                                         &width,
                                         EMOJI_HELLO,
                                         3);

            ASSERTV(numCharacters, EMOJI.size() + 1 == numCharacters);
            ASSERTV(width,         3                == width);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `displayWidth`
        //
        // Concerns:
        // 1. The width of text is the sum of the widths of its grapheme
        //    clusters, each as wide as its first code point.
        //
        // 2. Wide (e.g., CJK and emoji) code points count as two columns, and
        //    combining code points and joined emoji sequences add nothing.
        //
        // 3. Empty text has a width of zero.
        //
        // 4. Text that is not well-formed Unicode causes `bsl::format_error`
        //    to be thrown.
        //
        // 5. The above holds for `wchar_t` as well as `char`.
        //
        // Plan:
        // 1. Using the table-driven technique, call `displayWidth` on UTF-8
        //    text of known width, and verify the result.  (C-1..3)
        //
        // 2. Call `displayWidth` on ill-formed UTF-8 and verify that
        //    `bsl::format_error` is thrown.  (C-4)
        //
        // 3. Using the table-driven technique, repeat P-1 with `wchar_t`
        //    text built from the same code points.  (C-5)
        //
        // Testing:
        //   int displayWidth(const StringView& text);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `displayWidth`"
                            "\n======================\n");

        typedef bslfmt::DisplayWidthUtil Util;

        if (veryVerbose) printf("\t`char`\n");
        {
            static const struct {
                int         d_line;
                const char *d_text_p;
                int         d_expWidth;
            } DATA[] = {
                //LINE  TEXT                                          WIDTH
                //----  --------------------------------------------  -----
                { L_,   "",                                               0 },
                { L_,   "a",                                              1 },
                { L_,   "hello",                                          5 },

                // U+4E2D: CJK, two columns wide.

                { L_,   "\xe4\xb8\xad",                                   2 },
                { L_,   "a\xe4\xb8\xad" "b",                              4 },

                // `g` followed by U+0308 (combining): one cluster.

                { L_,   "g\xcc\x88",                                      1 },

                // U+1F408 ZWJ U+1F7E7: one emoji cluster.

                { L_,   "\xf0\x9f\x90\x88\xe2\x80\x8d\xf0\x9f\x9f\xa7",   2 },

                // `a` ZWJ U+1F408: `a` is not pictographic, so the ZWJ does
                // not join the emoji to it, giving two clusters.

                { L_,   "a\xe2\x80\x8d\xf0\x9f\x90\x88",                   3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *TEXT      = DATA[ti].d_text_p;
                const int   EXP_WIDTH = DATA[ti].d_expWidth;

                const int width = Util::displayWidth(TEXT);

                ASSERTV(LINE, width, EXP_WIDTH == width);
            }
        }

        if (veryVerbose) printf("\tIll-formed text\n");
        {
#ifdef BDE_BUILD_TARGET_EXC
            VERIFY_EXCEPTION_IS_THROWN(L_, Util::displayWidth("\xff"));
            VERIFY_EXCEPTION_IS_THROWN(L_, Util::displayWidth("ab\xe4\xb8"));
#endif
        }

        if (veryVerbose) printf("\t`wchar_t`\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_text_p;
                int            d_expWidth;
            } DATA[] = {
                //LINE  TEXT                                     WIDTH
                //----  ---------------------------------------  -----
                { L_,   L"",                                         0 },
                { L_,   L"a",                                        1 },
                { L_,   L"\x4e2d" L"b",                              3 },
                { L_,   L"g\x308",                                   1 },
                { L_,   L"\U0001F408\x200d\U0001F7E7",               2 },
                { L_,   L"a\x200d\U0001F408",                        3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE      = DATA[ti].d_line;
                const wchar_t *TEXT      = DATA[ti].d_text_p;
                const int      EXP_WIDTH = DATA[ti].d_expWidth;

                const int width = Util::displayWidth(TEXT);

                ASSERTV(LINE, width, EXP_WIDTH == width);
            }
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
        // 1. Measure some ASCII text, and find a prefix of it.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bslfmt::DisplayWidthUtil Util;

        ASSERT(5 == Util::displayWidth("hello"));

        std::size_t numCharacters = 0;
        int         width         = 0;

        Util::findWidthLimitedPrefix(&numCharacters, &width, "hello", 3);

        ASSERT(3 == numCharacters);
        ASSERT(3 == width);
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
