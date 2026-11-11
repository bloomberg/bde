// bslfmt_escapeutil.t.cpp                                            -*-C++-*-
#include <bslfmt_escapeutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a namespace for utility functions that
// render characters and strings in the escaped ("debug") form specified by
// [format.string.escaped].  The two rendering functions are tested against
// tables of inputs and expected results; the two Unicode property predicates
// they rely on are tested independently, since a defect in either would
// otherwise be visible only for exotic inputs.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] bool isPrintable(unsigned long int codePoint);
// [ 2] bool isGraphemeExtend(unsigned long int codePoint);
// [ 3] t_ITERATOR escapeString(t_ITERATOR out, const string_view& value);
// [ 4] t_ITERATOR escapeCharacter(t_ITERATOR out, t_CHAR value);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                          HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace u {

/// Return the escaped rendering of the specified string `input`.
bsl::string escapeStr(const bsl::string_view& input)
{
    bsl::string result;
    EscapeUtil<char>::escapeString(bsl::back_inserter(result), input);
    return result;
}

/// Return the escaped rendering of the specified character `input`.
bsl::string escapeChar(char input)
{
    bsl::string result;
    EscapeUtil<char>::escapeCharacter(bsl::back_inserter(result), input);
    return result;
}

/// Return the escaped rendering of the specified wide string `input`.
bsl::wstring escapeWStr(const bsl::wstring_view& input)
{
    bsl::wstring result;
    EscapeUtil<wchar_t>::escapeString(bsl::back_inserter(result), input);
    return result;
}

/// Return the escaped rendering of the specified wide character `input`.
bsl::wstring escapeWChar(wchar_t input)
{
    bsl::wstring result;
    EscapeUtil<wchar_t>::escapeCharacter(bsl::back_inserter(result), input);
    return result;
}

}  // close namespace u

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Escaping A String Containing A Newline
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wish to render a string in a form that shows its control
// characters rather than acting on them.
//
// First, we escape a string containing a newline into a `bsl::string`:
// ```
    void run()
    {
        bsl::string result;
        bslfmt::EscapeUtil<char>::escapeString(bsl::back_inserter(result),
                                               "a\nb");
// ```
// Then, we verify that the newline was replaced by its escape sequence and
// that the result is surrounded by double quotes:
// ```
        ASSERT("\"a\\nb\"" == result);
    }
// ```

}  // close namespace usage_example_1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    (void)veryVerbose;

    printf("TEST %s CASE %d \n", __FILE__, test);

    bslma::TestAllocator globalAllocator("global", veryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE\n=============\n");

        usage_example_1::run();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `escapeCharacter`
        //
        // Concerns:
        // 1. A character is rendered surrounded by single quotes.
        //
        // 2. The simple escapes are applied, and `'` -- but not `"` -- is
        //    among them, the reverse of the string case.
        //
        // 3. A non-printable character is rendered as `\u{...}`.
        //
        // Plan:
        // 1. Render a table of characters and compare against the expected
        //    result, for both `char` and `wchar_t`.  (C-1..3)
        //
        // Testing:
        //   t_ITERATOR escapeCharacter(t_ITERATOR out, t_CHAR value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `escapeCharacter`"
                            "\n=========================\n");

        static const struct {
            int         d_line;
            char        d_input;
            const char *d_expected_p;
        } DATA[] = {
            //LINE  INPUT   EXPECTED
            //----  ------  -----------
            { L_,   'a',    "'a'"       },
            { L_,   'Z',    "'Z'"       },
            { L_,   '0',    "'0'"       },
            { L_,   ' ',    "' '"       },

            // `'` is escaped in a character, `"` is not
            { L_,   '\'',   "'\\''"     },
            { L_,   '"',    "'\"'"      },

            // Other simple escapes
            { L_,   '\t',   "'\\t'"     },
            { L_,   '\n',   "'\\n'"     },
            { L_,   '\r',   "'\\r'"     },
            { L_,   '\\',   "'\\\\'"    },

            // Non-printable
            { L_,   '\0',   "'\\u{0}'"  },
            { L_,   '\x01', "'\\u{1}'"  },
            { L_,   '\x1b', "'\\u{1b}'" },
            { L_,   '\x7f', "'\\u{7f}'" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char        INPUT    = DATA[ti].d_input;
            const char *const EXPECTED = DATA[ti].d_expected_p;

            const bsl::string result = u::escapeChar(INPUT);
            ASSERTV(LINE, EXPECTED, result.c_str(), EXPECTED == result);

            // The same input through the `wchar_t` specialization must give
            // the same rendering, widened.
            const bsl::wstring wResult =
                                 u::escapeWChar(static_cast<wchar_t>(INPUT));
            const bsl::wstring wExpected(bsl::from_range,
                                         bsl::string_view(EXPECTED));
            ASSERTV(LINE, EXPECTED, wExpected == wResult);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `escapeString`
        //
        // Concerns:
        // 1. A string is rendered surrounded by double quotes, and an empty
        //    string renders as just the two quotes.
        //
        // 2. The simple escapes are applied, and `"` -- but not `'` -- is
        //    among them.
        //
        // 3. A non-printable code point is rendered as `\u{...}` in lowercase
        //    with no leading zeros, and this holds for code points that are
        //    non-printable by Unicode category rather than by being a control
        //    character.
        //
        // 4. A printable code point outside ASCII, including one outside the
        //    basic multilingual plane, is copied through unchanged.
        //
        // 5. Each code unit of an ill-formed sequence is rendered as
        //    `\x{...}`, and rendering resynchronizes on the next unit.
        //
        // 6. A code point having the `Grapheme_Extend` property is escaped
        //    when it begins the string, and copied through otherwise.
        //
        // 7. An embedded null character is escaped rather than terminating
        //    the rendering.
        //
        // Plan:
        // 1. Render a table of strings and compare against the expected
        //    result.  (C-1..7)
        //
        // Testing:
        //   t_ITERATOR escapeString(t_ITERATOR o, const string_view& value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `escapeString`"
                            "\n======================\n");

        static const struct {
            int         d_line;
            const char *d_input_p;
            const char *d_expected_p;
        } DATA[] = {
            //LINE  INPUT               EXPECTED
            //----  ------------------  ---------------------------
            { L_,   "",                 "\"\""                      },
            { L_,   "abc",              "\"abc\""                   },
            { L_,   " ",                "\" \""                     },

            // Simple escapes; `"` escaped, `'` not
            { L_,   "a\tb",             "\"a\\tb\""                 },
            { L_,   "a\nb",             "\"a\\nb\""                 },
            { L_,   "a\rb",             "\"a\\rb\""                 },
            { L_,   "a\\b",             "\"a\\\\b\""                },
            { L_,   "a\"b",             "\"a\\\"b\""                },
            { L_,   "a'b",              "\"a'b\""                   },

            // Control characters
            { L_,   "\x01",             "\"\\u{1}\""                },
            { L_,   "\x1b",             "\"\\u{1b}\""               },
            { L_,   "\x7f",             "\"\\u{7f}\""               },

            // Printable non-ASCII passes through unchanged: U+00E9,
            // U+4E2D, U+1F600 (outside the BMP)
            { L_,   "\xc3\xa9",         "\"\xc3\xa9\""              },
            { L_,   "\xe4\xb8\xad",     "\"\xe4\xb8\xad\""          },
            { L_,   "\xf0\x9f\x98\x80", "\"\xf0\x9f\x98\x80\""      },

            // Non-printable by category, not by being a control character:
            // U+00A0 NO-BREAK SPACE and U+2028 LINE SEPARATOR are `Zs` and
            // `Zl`; U+200B ZERO WIDTH SPACE is `Cf`
            { L_,   "\xc2\xa0",         "\"\\u{a0}\""               },
            { L_,   "\xe2\x80\xa8",     "\"\\u{2028}\""             },
            { L_,   "\xe2\x80\x8b",     "\"\\u{200b}\""             },

            // Ill-formed UTF-8
            { L_,   "\xff",             "\"\\x{ff}\""               },
            { L_,   "a\xc3",            "\"a\\x{c3}\""              },
            { L_,   "\xc3\x28",         "\"\\x{c3}(\""              },

            // U+0300 COMBINING GRAVE ACCENT.  It is escaped unless the
            // character before it was copied through unchanged, so it is
            // escaped at the start, copied after a printable character, and
            // -- the case that distinguishes this rule from "is it first" --
            // escaped again after a character that was itself escaped.
            { L_,   "\xcc\x80",         "\"\\u{300}\""              },
            { L_,   "a\xcc\x80",        "\"a\xcc\x80\""             },
            { L_,   "\t\xcc\x80",       "\"\\t\\u{300}\""           },
            { L_,   "\x01\xcc\x80",     "\"\\u{1}\\u{300}\""        },
            { L_,   "\xff\xcc\x80",     "\"\\x{ff}\\u{300}\""       },

            // ... and a second mark after a copied-through one is itself
            // copied through.
            { L_,   "a\xcc\x80\xcc\x81",
                                        "\"a\xcc\x80\xcc\x81\""     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const INPUT    = DATA[ti].d_input_p;
            const char *const EXPECTED = DATA[ti].d_expected_p;

            const bsl::string result = u::escapeStr(INPUT);
            ASSERTV(LINE, EXPECTED, result.c_str(), EXPECTED == result);
        }

        if (verbose) printf("\tEmbedded null character\n");
        {
            const bsl::string result = u::escapeStr(
                                                bsl::string_view("a\0b", 3));
            ASSERTV(result.c_str(), "\"a\\u{0}b\"" == result);
        }

        if (verbose) printf("\tWide strings\n");
        {
            ASSERT(L"\"abc\""      == u::escapeWStr(L"abc"));
            ASSERT(L"\"a\\nb\""    == u::escapeWStr(L"a\nb"));
            ASSERT(L"\"a\\\"b\""   == u::escapeWStr(L"a\"b"));
            ASSERT(L"\"\\u{1b}\""  == u::escapeWStr(L"\x1b"));
            ASSERT(L"\"\\u{a0}\""  == u::escapeWStr(L"\xa0"));
            ASSERT(L"\"\\u{300}\"" == u::escapeWStr(L"\x300"));

            // An ill-formed wide code unit must be reported at its full
            // width, not truncated to its low byte: an unpaired surrogate is
            // `\x{d800}`, and `static_cast<unsigned char>(0xd800)` is 0.
            {
                const wchar_t surrogate[] = { static_cast<wchar_t>(0xd800),
                                              0 };
                ASSERT(L"\"\\x{d800}\"" == u::escapeWStr(surrogate));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PROPERTY PREDICATES
        //
        // Concerns:
        // 1. `isPrintable` agrees with the Unicode `General_Category` rule of
        //    [format.string.escaped]: a code point is printable unless its
        //    category is one of `Cc`, `Cf`, `Cs`, `Co`, `Cn`, `Zl`, `Zp`, and
        //    `Zs`, with U+0020 SPACE excepted.
        //
        // 2. `isGraphemeExtend` reports the derived `Grapheme_Extend`
        //    property.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of code points
        //    representative of each relevant category, together with the
        //    expected result of each predicate, and verify both predicates
        //    for each row.  (C-1..2)
        //
        // Testing:
        //   bool isPrintable(unsigned long int codePoint);
        //   bool isGraphemeExtend(unsigned long int codePoint);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PROPERTY PREDICATES"
                            "\n===========================\n");

        typedef EscapeUtil<char> Util;

        static const struct {
            int               d_line;
            unsigned long int d_codePoint;
            bool              d_isPrintable;
            bool              d_isGraphemeExtend;
        } DATA[] = {
            //LINE  CODE POINT  PRINT  EXTEND    DESCRIPTION
            //----  ----------  -----  ------    ---------------------------
            { L_,   0x00,       false, false },  // `Cc` NULL
            { L_,   0x1f,       false, false },  // `Cc` UNIT SEPARATOR
            { L_,   0x20,       true,  false },  // SPACE, excepted from `Zs`
            { L_,   0x61,       true,  false },  // LATIN SMALL LETTER A
            { L_,   0x7e,       true,  false },  // TILDE
            { L_,   0x7f,       false, false },  // `Cc` DELETE
            { L_,   0x85,       false, false },  // `Cc` NEXT LINE
            { L_,   0xa0,       false, false },  // `Zs` NO-BREAK SPACE
            { L_,   0xe9,       true,  false },  // LATIN SMALL LETTER E ACUTE
            { L_,   0x300,      true,  true  },  // COMBINING GRAVE ACCENT
            { L_,   0x36f,      true,  true  },  // COMBINING LATIN SMALL X
            { L_,   0x370,      true,  false },  // GREEK CAPITAL HETA
            { L_,   0x200b,     false, false },  // `Cf` ZERO WIDTH SPACE
            { L_,   0x2028,     false, false },  // `Zl` LINE SEPARATOR
            { L_,   0x2029,     false, false },  // `Zp` PARAGRAPH SEPARATOR
            { L_,   0x4e2d,     true,  false },  // CJK UNIFIED IDEOGRAPH
            { L_,   0xd800,     false, false },  // `Cs` surrogate
            { L_,   0xe000,     false, false },  // `Co` private use
            { L_,   0x1f600,    true,  false },  // GRINNING FACE
            { L_,   0x10ffff,   false, false },  // `Cn` unassigned
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int               LINE       = DATA[ti].d_line;
            const unsigned long int CODE_POINT = DATA[ti].d_codePoint;
            const bool              PRINTABLE  = DATA[ti].d_isPrintable;
            const bool              EXTEND     = DATA[ti].d_isGraphemeExtend;

            ASSERTV(LINE, CODE_POINT,
                    PRINTABLE == Util::isPrintable(CODE_POINT));
            ASSERTV(LINE, CODE_POINT,
                    EXTEND    == Util::isGraphemeExtend(CODE_POINT));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The component is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Escape a simple string and a simple character.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST\n==============\n");

        ASSERT("\"abc\"" == u::escapeStr("abc"));
        ASSERT("'a'"     == u::escapeChar('a'));
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
