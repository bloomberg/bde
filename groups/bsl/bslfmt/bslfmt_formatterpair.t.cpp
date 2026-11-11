// bslfmt_formatterpair.t.cpp                                         -*-C++-*-
#include <bslfmt_formatterpair.h>

#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterstring.h>
#include <bslfmt_formattertestutil.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_utility.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a partial specialization of
// `bsl::formatter` for `bsl::pair`.  Format-spec parsing is delegated to
// `TupleFormatSpecification` (tested separately); this test driver focuses on
// the behaviors owned by this component:
// * the default separator and bracket strings selected by the constructor
// * the `set_separator` and `set_brackets` manipulators (used, e.g., by
//   `MapLikeFormatter` to reconfigure the underlying pair formatter)
// * the reaction of `parse` to the `n` and `m` format-spec letters, and the
//   subsequent forwarding of the remaining spec to the two underlying
//   element formatters
// * the output of `format` for pairs of various element types, including
//   verification that both elements' bytes are correctly emitted (which
//   requires that the underlying element formats are invoked with an
//   up-to-date `ctx.out()`)
// ----------------------------------------------------------------------------
// CLASS `bsl::formatter<bsl::pair<T1, T2>, t_CHAR>`
// CREATORS
// [ 2] formatter();
//
// MANIPULATORS
// [ 3] void set_separator(string_view separator);
// [ 3] void set_brackets(string_view opening, string_view clos);
// [ 4] typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx);
//
// ACCESSORS
// [ 5] typename t_FORMAT_CONTEXT::iterator format(const bsl::pair<T1, T2>&,
//                                                 t_FORMAT_CONTEXT&) const;
// ----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE
// [ 6] WIDTH IN DISPLAY COLUMNS
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

// The `bsl::formatter<bsl::pair<...>, t_CHAR>` partial specialization is
// only defined when the standard library does not already provide C++23
// `std::format` range/pair support.
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

typedef bsl::pair<int, int>                                IntIntPair;
typedef bsl::pair<int, char>                               IntCharPair;

typedef bsl::formatter<IntIntPair,  char>                  IntIntFormatterC;
typedef bsl::formatter<IntIntPair,  wchar_t>               IntIntFormatterW;
typedef bsl::formatter<IntCharPair, char>                  IntCharFormatterC;

typedef bslfmt::FormatterTestUtil<char>                    TestUtilChar;
typedef bslfmt::FormatterTestUtil<wchar_t>                 TestUtilWchar;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Parse the specified `format` into the specified `*formatter` using a
/// `MockParseContext`.  The `format` must include the terminating `}`.
template <class t_FORMATTER>
void parseFormat(t_FORMATTER *formatter, const bsl::string_view& format)
{
    bslfmt::MockParseContext<char> context(format, 0);
    formatter->parse(context);
}

template <class t_FORMATTER>
void parseFormat(t_FORMATTER *formatter, const bsl::wstring_view& format)
{
    bslfmt::MockParseContext<wchar_t> context(format, 0);
    formatter->parse(context);
}

/// Format the specified `value` using the specified (already parsed)
/// `formatter` and return the result as a `bsl::string`.
template <class t_FORMATTER, class t_VALUE>
bsl::string formatToString(const t_FORMATTER& formatter, const t_VALUE& value)
{
    bslfmt::MockFormatContext<char> mfc(0);
    mfc.advance_to(formatter.format(value, mfc));
    return bsl::string(mfc.finalString());
}

/// Format the specified `value` using the specified (already parsed)
/// `formatter` and return the result as a `bsl::wstring`.
template <class t_FORMATTER, class t_VALUE>
bsl::wstring formatToWString(const t_FORMATTER& formatter,
                             const t_VALUE&     value)
{
    bslfmt::MockFormatContext<wchar_t> mfc(0);
    mfc.advance_to(formatter.format(value, mfc));
    return bsl::wstring(mfc.finalString());
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Formatting A Pair
/// - - - - - - - - - - - - - -
// Suppose we wish to render a key/value pair as it would appear inside a map.
//
// First, we parse the map-like presentation, `m`, into a formatter for the
// pair:
// ```
    void run()
    {
        bslfmt::MockParseContext<char> parseContext("m}", 0);

        bsl::formatter<bsl::pair<int, bsl::string>, char> formatter;
        formatter.parse(parseContext);
// ```
// Now, we format a pair with it:
// ```
        bsl::pair<int, bsl::string> value(1, "one");

        bslfmt::MockFormatContext<char> formatContext(0);
        formatContext.advance_to(
                      bsl::as_const(formatter).format(value, formatContext));
// ```
// Finally, we observe that the parentheses are gone, that the two values are
// separated by `: `, and that the string is quoted:
// ```
        ASSERT("1: \"one\"" == formatContext.finalString());
    }
// ```

}  // close namespace usage_example_1

#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

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

    // CONCERN: No global memory is allocated after `main` starts.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
    (void) test;
    if (verbose) printf("Component is not defined when the standard library "
                        "provides C++23 `std::format` range/pair support.\n");
#else
    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
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
      case 6: {
        // --------------------------------------------------------------------
        // WIDTH IN DISPLAY COLUMNS
        //
        // Concerns:
        // 1. The width is applied in display columns, not in code units, so a
        //    pair whose rendering contains a multi-byte character is padded to
        //    the same column count as an equivalent ASCII rendering.
        //
        // 2. A width with a leading zero is rejected: `width` is a
        //    `positive-integer`, and a tuple specification has no zero-pad
        //    flag to absorb the `0`.
        //
        // Plan:
        // 1. Format a pair holding a two-byte, one-column character to a
        //    fixed width and verify that the result occupies the requested
        //    number of columns rather than the requested number of bytes.
        //    (C-1)
        //
        // 2. Verify that a leading zero in the width throws.  (C-2)
        //
        // Testing:
        //   WIDTH IN DISPLAY COLUMNS
        // --------------------------------------------------------------------

        if (verbose) printf("\nWIDTH IN DISPLAY COLUMNS"
                            "\n========================\n");

        {
            // `("X", 1)` is 8 columns but 9 bytes, so padding to 12 columns
            // must add 4 fill characters, not 3.

            const bsl::string twoByteOneColumn("\xc3\xa9");
            const bsl::pair<bsl::string, int> value(twoByteOneColumn, 1);

            bsl::formatter<bsl::pair<bsl::string, int>, char> formatter;
            parseFormat(&formatter, "*<12}");

            const bsl::string result = formatToString(formatter, value);
            const bsl::string expected =
                          "(\"" + twoByteOneColumn + "\", 1)" + "****";

            ASSERTV(result.c_str(), expected.c_str(), expected == result);
        }

        if (verbose) printf("\tA leading zero in the width is rejected.\n");
        {
#ifdef BDE_BUILD_TARGET_EXC
            IntIntFormatterC zero;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&zero, "0}"));
            IntIntFormatterC leadingZero;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&leadingZero, "05}"));
            IntIntFormatterC leadingZeroTwoDigit;
            VERIFY_EXCEPTION_IS_THROWN(
                                    L_,
                                    parseFormat(&leadingZeroTwoDigit, "010}"));
#endif
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `format`
        //
        // Concerns:
        // 1. `format` produces the expected representation for a variety of
        //    pairs and format specifications:
        //      - the default (empty) spec yields `(v1, v2)`
        //      - the `n` spec yields `v1, v2` (no brackets)
        //      - the `m` spec yields `v1: v2` (map-like, no brackets)
        //      - the `nm` spec has the same effect as `m`
        //
        // 2. `format` respects brackets and separators that were configured
        //    via `set_brackets` / `set_separator` when the parsed spec left
        //    them unchanged (i.e., default `e_TUPLE_DEFAULT`).
        //
        // 3. The behavior is the same for `char` and `wchar_t`.
        //
        // 4. The behavior is correct for pairs whose two element types
        //    differ (exercising both `d_underlying1` and `d_underlying2`
        //    with distinct formatter states).
        //
        // 5. Outer fill / align / width tokens in the spec cause `format` to
        //    pad the pair's rendered representation to the requested width
        //    using the requested fill character and alignment.
        //
        // Plan:
        // 1. Construct a `formatter<pair<int,int>, char>`, parse the spec
        //    `"}"`, and verify the output is `"(1, 2)"`.  Repeat for the
        //    specs `"n}"`, `"m}"`.  (C-1)
        //
        // 2. After parsing `"}"`, reconfigure the formatter via
        //    `set_brackets` and `set_separator` and verify the output
        //    reflects the new tokens.  (C-2)
        //
        // 3. Repeat step 1 for `wchar_t`.  (C-3)
        //
        // 4. Format a `pair<int, char>` and verify that both elements
        //    render correctly (i.e., they neither overwrite each other nor
        //    share underlying state).  (C-4)
        //
        // 5. Using a table-based approach, parse a variety of specs that
        //    include fill / align / width tokens and verify that the
        //    rendered output is padded to the requested width using the
        //    correct fill character and alignment.  (C-5)
        //
        // Testing:
        //   typename t_FORMAT_CONTEXT::iterator format(const bsl::pair&,
        //                                              t_FORMAT_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `format`"
                            "\n================\n");

        if (verbose) printf("\tDefault spec `char`.\n");
        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv = TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              "(1, 2)",
                                                              false,
                                                              "{:}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\t`n` spec removes brackets.\n");
        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv = TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              "1, 2",
                                                              false,
                                                              "{:n}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\t`m` spec yields map-like output.\n");
        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv = TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              "1: 2",
                                                              false,
                                                              "{:m}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\tCustom brackets / separator.\n");
        {
            // Direct-formatter test: `FormatterTestUtil` default-constructs
            // the formatter internally, so the `set_brackets`/`set_separator`
            // manipulators must be exercised via the hand-rolled path.
            IntIntPair       value(1, 2);
            IntIntFormatterC formatter;
            parseFormat(&formatter, "}");

            const bsl::string_view open("<<");
            const bsl::string_view close(">>");
            const bsl::string_view sep(" | ");
            formatter.set_brackets(open, close);
            formatter.set_separator(sep);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "<<1 | 2>>" == formatToString(formatter, value));

            const bsl::string_view empty;
            formatter.set_brackets(empty, empty);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "1 | 2" == formatToString(formatter, value));
        }

        if (verbose) printf("\tFill / align / width padding.\n");
        {
            // The pair formatter renders the pair body -- opening bracket,
            // first value, separator, second value, closing bracket -- and
            // then pads that rendering to the requested width using the
            // fill character and alignment captured by the tuple format
            // specification.  Default alignment is left.  This table pins
            // the observable behavior across a variety of fill / align /
            // width combinations.

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC            EXPECTED
                //---- --------------  ------------------
                { L_,  "{:10}",        "(1, 2)    "       },
                { L_,  "{:*<8}",       "(1, 2)**"         },
                { L_,  "{:*^10}",      "**(1, 2)**"       },
                { L_,  "{:*>12}",      "******(1, 2)"     },
                { L_,  "{:0<9}",       "(1, 2)000"        },
                { L_,  "{:-^15}",      "----(1, 2)-----"  },
                { L_,  "{:#>15}",      "#########(1, 2)"  },
                { L_,  "{:*<10n}",     "1, 2******"       },
                { L_,  "{:*^15n}",     "*****1, 2******"  },
                { L_,  "{:*>10m}",     "******1: 2"       },
                { L_,  "{:0<15m}",     "1: 200000000000"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                bsl::string      message;
                const IntIntPair value(1, 2);
                const bool       rv =
                              TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              EXPECTED,
                                                              false,
                                                              SPEC,
                                                              value);
                ASSERTV(LINE, SPEC, EXPECTED, message.c_str(), rv);
            }
        }

        if (verbose) printf("\tHeterogeneous element types.\n");
        {
            // The two underlying formatters must be independently initialized
            // and independently emit their respective values.  Per
            // [format.tuple], the elements are rendered in the escaped
            // ("debug") presentation, so the character appears quoted.
            bsl::string       message;
            const IntCharPair value(7, 'x');
            const bool        rv =
                              TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              "(7, 'x')",
                                                              false,
                                                              "{:}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\tDefault spec `wchar_t`.\n");
        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv =
                              TestUtilWchar::testEvaluateVFormat(
                                                              &message,
                                                              L"(1, 2)",
                                                              false,
                                                              L"{:}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\t`m` spec `wchar_t`.\n");
        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv =
                              TestUtilWchar::testEvaluateVFormat(
                                                              &message,
                                                              L"1: 2",
                                                              false,
                                                              L"{:m}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `parse`
        //
        // Concerns:
        // 1. `parse` succeeds for a variety of valid format strings including
        //    an empty spec (`}`), `n}`, `m}`, and width/fill specifications
        //    combined with one of those letters.
        //
        // 2. `parse` throws a `format_error` when the input contains an
        //    unrecognized specifier, when more than one letter follows the
        //    width, or when the input is not terminated with `}`.
        //
        // 3. On success, `parse` leaves the parse-context iterator pointing
        //    to the terminating `}`.
        //
        // 4. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Using a table-based approach, parse a set of valid specs.
        //    Verify that `parse` does not throw and returns an iterator
        //    pointing to `}`.  (C-1, 3, 4)
        //
        // 2. Using a table-based approach, parse a set of invalid specs.
        //    Verify that `parse` throws `bslfmt::format_error`.  (C-2, 4)
        //
        // 3. Using a table-based approach, parse a set of specs that
        //    exercise every combination of fill, alignment, and width
        //    (with and without a trailing pair-type letter).  Verify that
        //    `parse` accepts all of them.  (C-1, 3)
        //
        // Testing:
        //   typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `parse`"
                            "\n===============\n");

        if (verbose) printf("\tValid `char` specifications.\n");
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // format spec (full `{:...}` form)
            } DATA[] = {
                //LINE SPEC
                //---- ------------
                { L_,  "{:}"        },
                { L_,  "{:n}"       },
                { L_,  "{:m}"       },
                { L_,  "{:10}"      },
                { L_,  "{:10n}"     },
                { L_,  "{:10m}"     },
                { L_,  "{:*<10}"    },
                { L_,  "{:*<10n}"   },
                { L_,  "{:*<10m}"   },
                { L_,  "{:*<n}"     },
                { L_,  "{:*^m}"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                bsl::string message;
                const bool  rv = TestUtilChar::testParseVFormat<IntIntPair>(
                                                                    &message,
                                                                    false,
                                                                    SPEC);
                ASSERTV(LINE, SPEC, message.c_str(), rv);
            }
        }

        if (verbose) printf("\tValid `wchar_t` specifications.\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_spec_p;
            } DATA[] = {
                { L_,  L"{:}"       },
                { L_,  L"{:n}"      },
                { L_,  L"{:m}"      },
                { L_,  L"{:*<10}"   },
                { L_,  L"{:*<10m}"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const wchar_t *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                bsl::string message;
                const bool  rv = TestUtilWchar::testParseVFormat<IntIntPair>(
                                                                    &message,
                                                                    false,
                                                                    SPEC);
                ASSERTV(LINE, message.c_str(), rv);
            }
        }

        if (verbose) printf("\tInvalid `char` specifications.\n");
        {
            static const struct {
                int         d_line;
                const char *d_spec_p;
            } DATA[] = {
                //LINE SPEC             Reason
                //---- --------------- ----------------------------------------
                { L_,  "{:x}"       }, // unknown character
                { L_,  "{:nx}"      }, // unknown trailing character
                { L_,  "{:nm}"      }, // more than one letter after width
                { L_,  "{:mn}"      }, // more than one letter after width
                { L_,  "{:nn}"      }, // more than one letter after width
                { L_,  "{:mm}"      }, // more than one letter after width
                { L_,  "{:10nm}"    }, // more than one letter after width

                // A tuple-format-spec has no element sub-specification
                // ([format.tuple]): it may only end in `[n|m]`, so these
                // strings must be rejected by `parse`.  Note that `:` is
                // also not a legal tuple fill character, so `"{::^5}"` --
                // which would otherwise be interpreted as fill=`:`,
                // align=`^`, width=`5` -- must also be rejected.
                { L_,  "{::^5:^5}"  }, // per-element specs (key & value)
                { L_,  "{::^5}"     }, // `:` filler is disallowed
                { L_,  "{:10:^5}"   }, // width + trailing element spec
                { L_,  "{:n:^5}"    }, // n-letter + trailing element spec
                { L_,  "{:m:^5}"    }, // m-letter + trailing element spec
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                bsl::string message;
                const bool  rv = TestUtilChar::testParseFailure<IntIntPair>(
                                                                    &message,
                                                                    false,
                                                                    SPEC);
                ASSERTV(LINE, SPEC, message.c_str(), rv);
            }
        }

        if (verbose) printf("\tInvalid `wchar_t` specifications.\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_spec_p;
            } DATA[] = {
                { L_,  L"{:x}"   },
                { L_,  L"{:mn}"  },
                { L_,  L"{:nm}"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const wchar_t *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                bsl::string message;
                const bool  rv = TestUtilWchar::testParseFailure<IntIntPair>(
                                                                    &message,
                                                                    false,
                                                                    SPEC);
                ASSERTV(LINE, message.c_str(), rv);
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tMissing terminating `}` (direct parse).\n");
        {
            // These specs cannot be reached through the normal `bsl::format`
            // machinery (which always supplies a matching `}`); they are
            // driven directly through the formatter's `parse` to pin the
            // internal error-handling behavior.
            static const struct {
                int         d_line;
                const char *d_spec_p;
            } DATA[] = {
                //LINE SPEC
                //---- ------
                { L_,  ""     },  // empty input
                { L_,  "n"    },  // missing '}'
                { L_,  "m"    },  // missing '}'
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                IntIntFormatterC formatter;
                VERIFY_EXCEPTION_IS_THROWN(LINE,
                                           parseFormat(&formatter, SPEC));
            }
        }
#endif

        if (verbose) printf("\tFill / align / width combinations.\n");
        {
            // Verify that the fill-and-align + width prefix (parsed by the
            // underlying `TupleFormatSpecification` via its
            // `NestedFormatSpecificationParser`) is accepted for every
            // combination of alignment and width, both with and without a
            // custom filler character and both with and without a trailing
            // pair-type letter.

            static const struct {
                int         d_line;
                const char *d_spec_p;
            } DATA[] = {
                //LINE SPEC
                //---- ------------
                // Alignment only (no explicit filler, no width digits).
                { L_,  "{:<}"       },
                { L_,  "{:^}"       },
                { L_,  "{:>}"       },

                // Width only (no fill, no align).
                { L_,  "{:1}"       },
                { L_,  "{:20}"      },

                // Alignment + width, no explicit filler.
                { L_,  "{:<5}"      },
                { L_,  "{:^5}"      },
                { L_,  "{:>5}"      },
                { L_,  "{:<20}"     },
                { L_,  "{:^20}"     },
                { L_,  "{:>20}"     },

                // Explicit filler + alignment + width.
                { L_,  "{:*<5}"     },
                { L_,  "{:*^5}"     },
                { L_,  "{:*>5}"     },
                { L_,  "{:0<10}"    },
                { L_,  "{:-^10}"    },
                { L_,  "{:#>10}"    },

                // Fill / align / width followed by pair-type letter.
                { L_,  "{:*<10n}"   },
                { L_,  "{:*^10n}"   },
                { L_,  "{:*>10m}"   },
                { L_,  "{:0<5n}"    },
                { L_,  "{:#>15m}"   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                bsl::string message;
                const bool  rv = TestUtilChar::testParseVFormat<IntIntPair>(
                                                                    &message,
                                                                    false,
                                                                    SPEC);
                ASSERTV(LINE, SPEC, message.c_str(), rv);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MANIPULATORS `set_separator` / `set_brackets`
        //
        // Concerns:
        // 1. `set_separator` replaces the separator emitted between the two
        //    elements of a pair.
        //
        // 2. `set_brackets` replaces the opening and closing brackets
        //    emitted around the pair.
        //
        // 3. The above manipulators can be called both before and after
        //    `parse`, and the values they establish are used when `format`
        //    is subsequently invoked.
        //
        // Plan:
        // 1. Construct a `formatter<pair<int,int>, char>`, parse the empty
        //    spec `"}"`, then invoke `set_separator` and `set_brackets` to
        //    change the tokens.  Verify that a subsequent `format` call
        //    uses the new values.  (C-1..2)
        //
        // 2. Also verify that repeated invocations of the manipulators
        //    update the tokens each time.  (C-3)
        //
        // Testing:
        //   void set_separator(basic_string_view);
        //   void set_brackets(basic_string_view, basic_string_view);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATORS"
                            "\n============\n");

        {
            IntIntPair       value(1, 2);
            IntIntFormatterC formatter;
            parseFormat(&formatter, "}");

            // Change the separator only.
            const bsl::string_view sep1(" | ");
            formatter.set_separator(sep1);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "(1 | 2)" == formatToString(formatter, value));

            // Change the brackets only.
            const bsl::string_view open1("[");
            const bsl::string_view close1("]");
            formatter.set_brackets(open1, close1);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "[1 | 2]" == formatToString(formatter, value));

            // Change to empty brackets.
            const bsl::string_view empty;
            formatter.set_brackets(empty, empty);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "1 | 2" == formatToString(formatter, value));

            // Change the separator again.
            const bsl::string_view sep2(": ");
            formatter.set_separator(sep2);
            ASSERTV(formatToString(formatter, value).c_str(),
                    "1: 2" == formatToString(formatter, value));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //
        // Concerns:
        // 1. The default constructor initializes the pair formatter with
        //    `(` and `)` as the opening/closing brackets and `, ` (comma +
        //    space) as the separator.
        //
        // Plan:
        // 1. Default-construct a `formatter<pair<int,int>, char>`, parse
        //    the empty spec `"}"`, and format a simple pair.  Verify that
        //    the output matches `"(1, 2)"`.  (C-1)
        //
        // 2. Repeat step 1 for `wchar_t`.  (C-1)
        //
        // Testing:
        //   formatter();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCREATORS"
                            "\n========\n");

        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv = TestUtilChar::testEvaluateVFormat(
                                                              &message,
                                                              "(1, 2)",
                                                              false,
                                                              "{:}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }

        {
            bsl::string      message;
            const IntIntPair value(1, 2);
            const bool       rv =
                              TestUtilWchar::testEvaluateVFormat(
                                                              &message,
                                                              L"(1, 2)",
                                                              false,
                                                              L"{:}",
                                                              value);
            ASSERTV(message.c_str(), rv);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic
        //   functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Default-construct a `formatter<pair<int,int>, char>`, parse
        //    the empty spec, and format a simple pair.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            IntIntPair       value(1, 2);
            IntIntFormatterC formatter;
            parseFormat(&formatter, "}");
            ASSERT("(1, 2)" == formatToString(formatter, value));
        }

        {
            IntIntPair       value(1, 2);
            IntIntFormatterC formatter;
            parseFormat(&formatter, "m}");
            ASSERT("1: 2" == formatToString(formatter, value));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
