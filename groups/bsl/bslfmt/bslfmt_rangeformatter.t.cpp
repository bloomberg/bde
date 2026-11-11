// bslfmt_rangeformatter.t.cpp                                        -*-C++-*-
#include <bslfmt_rangeformatter.h>

#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterpair.h>
#include <bslfmt_formatterstring.h>
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
#include <bslstl_vector.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bslfmt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides three helper class templates for
// implementing `bsl::formatter` specializations for range types:
// * `range_formatter`  : default format `[v1, v2]`.
// * `SetLikeFormatter` : set-like format `{v1, v2}`.
// * `MapLikeFormatter` : map-like format `{k1: v1, k2: v2}`.
//
// The bulk of the format-spec parsing is delegated to
// `RangeFormatSpecification` (which is tested separately).  This test driver
// focuses on the behaviors specific to this component:
// * the default separator and bracket strings selected by each constructor
// * the `set_separator`, `set_brackets`, and `underlying` manipulators
// * the reaction of `parse` to the `n` flag and to the `m`, `s` and `?s`
//   range types (in particular `m` is admitted for any range whose element is
//   a pair, and rejected otherwise, while `s` and `?s` require the element to
//   be the character type)
// * the output of the `format` method for various ranges
// * the application of fill, alignment and width, which is measured in
//   display columns rather than in code units
// * the handing of the element specification to the element formatter,
//   including the case in which that specification contains a nested
//   replacement field and so depends on the arguments of the format context
// ----------------------------------------------------------------------------
// CLASS `range_formatter`
// CREATORS
// [ 2] range_formatter();
//
// MANIPULATORS
// [ 3] void set_separator(string_view separator);
// [ 3] void set_brackets(string_view opening, string_view closing);
// [ 3] UnderlyingFormatter& underlying();
// [ 4] typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx);
//
// ACCESSORS
// [ 5] typename t_FORMAT_CONTEXT::iterator format(t_RANGE&, t_FORMAT_CTX&);
// [ 3] const UnderlyingFormatter& underlying() const;
//
// CLASS `SetLikeFormatter`
// [ 6] SetLikeFormatter();
//
// CLASS `MapLikeFormatter`
// [ 7] MapLikeFormatter();
// ----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
// [ 9] RANGE TYPES AND WIDTH
// [ 8] ELEMENT FORMAT SPECIFICATION
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

// The `range_formatter` and friends are only defined when the standard
// library does not already provide C++23 `std::format` range support.
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

typedef range_formatter<int, char>                     IntRangeC;
typedef range_formatter<int, wchar_t>                  IntRangeW;

typedef range_formatter<char, char>                    CharRangeC;
typedef range_formatter<char, wchar_t>                 CharRangeW;

typedef SetLikeFormatter<int, char>                    IntSetC;
typedef SetLikeFormatter<int, wchar_t>                 IntSetW;

typedef bsl::pair<int, int>                            IntPair;
typedef MapLikeFormatter<IntPair, char>                IntMapC;
typedef MapLikeFormatter<IntPair, wchar_t>             IntMapW;

//=============================================================================
//                            HELPER CLASSES
//-----------------------------------------------------------------------------

/// A single-digit value whose formatter follows the standard `parse`
/// contract: `parse` returns the end of the specification it consumed and
/// does not advance the context.
struct ReturnOnlyType {
    // DATA
    int d_value;
};

namespace bsl {

/// Formatter for `ReturnOnlyType`.  Accepts an empty specification or `x`.
/// Renders the digit, prefixed with `x` if `x` was parsed.
template <class t_CHAR>
struct formatter<ReturnOnlyType, t_CHAR> {
    // DATA
    bool d_prefix;

    // CREATORS
    formatter()
    : d_prefix(false)
    {
    }

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx)
    {
        typename t_PARSE_CONTEXT::iterator current = ctx.begin();
        if (current != ctx.end() && t_CHAR('x') == *current) {
            d_prefix = true;
            ++current;
        }
        return current;
    }

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        const ReturnOnlyType& value,
                                        t_FORMAT_CONTEXT&     ctx) const
    {
        typename t_FORMAT_CONTEXT::iterator out = ctx.out();
        if (d_prefix) {
            *out++ = t_CHAR('x');
        }
        *out++ = static_cast<t_CHAR>('0' + value.d_value);
        return out;
    }
};

}  // close namespace bsl

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

/// Parse the specified `format` into the specified `*formatter` using a
/// `MockParseContext` that makes the specified `numArgs` arguments available
/// to the nested replacement fields of the specification.  The `format` must
/// include the terminating `}`.
template <class t_FORMATTER>
void parseFormat(t_FORMATTER             *formatter,
                 const bsl::string_view&  format,
                 size_t                   numArgs)
{
    bslfmt::MockParseContext<char> context(format, numArgs);
    formatter->parse(context);
}

template <class t_FORMATTER>
void parseFormat(t_FORMATTER              *formatter,
                 const bsl::wstring_view&  format,
                 size_t                    numArgs)
{
    bslfmt::MockParseContext<wchar_t> context(format, numArgs);
    formatter->parse(context);
}

/// Format the specified `range` using the specified (already parsed)
/// `formatter` and return the result as a `bsl::string`.
template <class t_FORMATTER, class t_RANGE>
bsl::string formatToString(const t_FORMATTER& formatter, t_RANGE& range)
{
    bslfmt::MockFormatContext<char> mfc(0);
    mfc.advance_to(formatter.format(range, mfc));
    return bsl::string(mfc.finalString());
}

/// Format the specified `range` using the specified (already parsed)
/// `formatter` and return the result as a `bsl::wstring`.
template <class t_FORMATTER, class t_RANGE>
bsl::wstring formatToWString(const t_FORMATTER& formatter, t_RANGE& range)
{
    bslfmt::MockFormatContext<wchar_t> mfc(0);
    mfc.advance_to(formatter.format(range, mfc));
    return bsl::wstring(mfc.finalString());
}

/// Format the specified `range` using the specified (already parsed)
/// `formatter` and a format context holding the specified `arg0` as its only
/// argument, and return the result as a `bsl::string`.  Note that a
/// specification containing a single nested replacement field, such as
/// `:{}`, refers to `arg0`.
template <class t_FORMATTER, class t_RANGE, class t_ARG0>
bsl::string formatToString(const t_FORMATTER& formatter,
                           t_RANGE&           range,
                           const t_ARG0&      arg0)
{
    bslfmt::MockFormatContext<char> mfc(arg0);
    mfc.advance_to(formatter.format(range, mfc));
    return bsl::string(mfc.finalString());
}

/// Format the specified `range` using the specified (already parsed)
/// `formatter` and a format context holding the specified `arg0` and `arg1`
/// as its arguments, and return the result as a `bsl::string`.
template <class t_FORMATTER, class t_RANGE, class t_ARG0, class t_ARG1>
bsl::string formatToString(const t_FORMATTER& formatter,
                           t_RANGE&           range,
                           const t_ARG0&      arg0,
                           const t_ARG1&      arg1)
{
    bslfmt::MockFormatContext<char> mfc(arg0, arg1);
    mfc.advance_to(formatter.format(range, mfc));
    return bsl::string(mfc.finalString());
}

/// Format the specified `range` using the specified (already parsed)
/// `formatter` and a format context holding the specified `arg0` as its only
/// argument, and return the result as a `bsl::wstring`.
template <class t_FORMATTER, class t_RANGE, class t_ARG0>
bsl::wstring formatToWString(const t_FORMATTER& formatter,
                             t_RANGE&           range,
                             const t_ARG0&      arg0)
{
    bslfmt::MockFormatContext<wchar_t> mfc(arg0);
    mfc.advance_to(formatter.format(range, mfc));
    return bsl::wstring(mfc.finalString());
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Formatting A Range
///- - - - - - - - - - - - - - -
// Suppose we want to render a sequence of integers without its brackets and
// with each element in hexadecimal.
//
// First, we create a formatter for a range of `int` and parse a specification
// into it.  The `n` flag asks for no brackets, and everything after the `:`
// describes an individual element:
// ```
    void run()
    {
        bslfmt::MockParseContext<char> parseContext("n:#x}", 0);

        bslfmt::range_formatter<int, char> formatter;
        formatter.parse(parseContext);
// ```
// Now, we format a range with it:
// ```
        bsl::vector<int> value;
        value.push_back(10);
        value.push_back(11);

        bslfmt::MockFormatContext<char> formatContext(0);
        formatContext.advance_to(formatter.format(value, formatContext));
// ```
// Finally, we observe that the brackets are gone and that each element carries
// the `0x` prefix that `#x` requested:
// ```
        ASSERT("0xa, 0xb" == formatContext.finalString());
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
                        "provides C++23 `std::format` ranges support.\n");
#else
    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
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
      case 9: {
        // --------------------------------------------------------------------
        // RANGE TYPES AND WIDTH
        //
        // Concerns:
        // 1. The `m` range type is accepted for any range whose element is a
        //    pair -- not only for the map-like formatters -- and renders the
        //    range as a map.  It is rejected for any other element type.
        //
        // 2. The `s` and `?s` range types render a range of characters as a
        //    string, escaping it in the `?s` case, and honor fill, alignment
        //    and width.  They are rejected for a non-character element type,
        //    and cannot be combined with the `n` option or an element
        //    sub-specification.
        //
        // 3. The width is applied in display columns, not in code units, so a
        //    body containing a multi-byte character is padded to the same
        //    column count as an equivalent ASCII body.
        //
        // 4. A width with a leading zero is rejected: `width` is a
        //    `positive-integer`, and a range specification has no zero-pad
        //    flag to absorb the `0`.
        //
        // Plan:
        // 1. Parse `m` into a `range_formatter` over a pair and verify the
        //    rendered output; verify that a `range_formatter` over `int`
        //    throws instead.  (C-1)
        //
        // 2. Parse `s` and `?s`, with and without fill/align/width, into a
        //    `range_formatter` over `char` and verify the rendered output;
        //    verify the rejections.  (C-2)
        //
        // 3. Format a range whose rendering contains a two-byte, one-column
        //    character to a fixed width and verify that the result occupies
        //    the requested number of columns.  (C-3)
        //
        // 4. Verify that a leading zero in the width throws.  (C-4)
        //
        // Testing:
        //   RANGE TYPES AND WIDTH
        // --------------------------------------------------------------------

        if (verbose) printf("\nRANGE TYPES AND WIDTH"
                            "\n=====================\n");

        if (verbose) printf("\t`m` is gated on the element type.\n");
        {
            IntPair range[] = {IntPair(1, 2), IntPair(3, 4)};

            range_formatter<IntPair, char> formatter;
            parseFormat(&formatter, "m}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "{1: 2, 3: 4}" == formatToString(formatter, range));

            range_formatter<IntPair, char> noBrackets;
            parseFormat(&noBrackets, "nm}");
            ASSERTV(formatToString(noBrackets, range).c_str(),
                    "1: 2, 3: 4" == formatToString(noBrackets, range));

            // A range of `int` has no pair element, so `m` must be rejected.
#ifdef BDE_BUILD_TARGET_EXC
            IntRangeC rejecter;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&rejecter, "m}"));
#endif
        }

        if (verbose) printf("\t`s` and `?s` render the range as a string.\n");
        {
            char range[] = {'a', '\t', 'b'};

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC          EXPECTED
                //---- ------------  ------------------
                { L_,  "s}",         "a\tb"            },
                { L_,  "?s}",        "\"a\\tb\""       },
                { L_,  "*<7s}",      "a\tb****"        },
                { L_,  "*>7s}",      "****a\tb"        },
                { L_,  "*^9s}",      "***a\tb***"      },
                { L_,  "7s}",        "a\tb    "        },
                { L_,  "*<9?s}",     "\"a\\tb\"***"    },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                CharRangeC formatter;
                parseFormat(&formatter, SPEC);
                ASSERTV(LINE, SPEC, formatToString(formatter, range).c_str(),
                        EXPECTED == formatToString(formatter, range));
            }

            // A non-character element type cannot be rendered as a string,
            // and an element sub-specification leaves nothing for `s` to do.
#ifdef BDE_BUILD_TARGET_EXC
            IntRangeC  nonCharacter;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&nonCharacter, "s}"));
            IntRangeC  nonCharacterEscaped;
            VERIFY_EXCEPTION_IS_THROWN(
                                  L_,
                                  parseFormat(&nonCharacterEscaped, "?s}"));
            CharRangeC withElementSpec;
            VERIFY_EXCEPTION_IS_THROWN(
                                      L_,
                                      parseFormat(&withElementSpec, "s:d}"));
            CharRangeC withNoBrackets;
            VERIFY_EXCEPTION_IS_THROWN(L_,
                                       parseFormat(&withNoBrackets, "ns}"));
            CharRangeC escapedWithNoBrackets;
            VERIFY_EXCEPTION_IS_THROWN(
                                  L_,
                                  parseFormat(&escapedWithNoBrackets, "n?s}"));
#endif
        }

        if (verbose) printf("\tWidth is counted in display columns.\n");
        {
            // Each element is one column wide but two bytes long, so a body
            // of `[X, X]` is 6 columns and 8 bytes.  Padding to 10 columns
            // must therefore add 4 fill characters, not 2.

            bsl::string twoByteOneColumn("\xc3\xa9");
            bsl::string range[] = {twoByteOneColumn, twoByteOneColumn};

            range_formatter<bsl::string, char> formatter;
            parseFormat(&formatter, "*<10:}");

            const bsl::string result = formatToString(formatter, range);
            const bsl::string expected =
                                 "[" + twoByteOneColumn + ", " +
                                 twoByteOneColumn + "]" + "****";

            ASSERTV(result.c_str(), expected.c_str(), expected == result);
        }

        if (verbose) printf("\tA leading zero in the width is rejected.\n");
        {
#ifdef BDE_BUILD_TARGET_EXC
            IntRangeC zero;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&zero, "0}"));
            IntRangeC leadingZero;
            VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&leadingZero, "05}"));
            IntRangeC leadingZeroTwoDigit;
            VERIFY_EXCEPTION_IS_THROWN(
                                    L_,
                                    parseFormat(&leadingZeroTwoDigit, "010}"));
#endif
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ELEMENT FORMAT SPECIFICATION
        //
        // Concerns:
        // 1. When the range format spec is followed by a `:` sub-specifier,
        //    the portion of the spec after the `:` (up to the matching `}`)
        //    is delegated to the underlying element formatter and controls
        //    how each element is rendered.
        //
        // 2. The element sub-specifier interoperates correctly with the
        //    `n` letter (no brackets) preceding the `:`.
        //
        // 3. The element sub-specifier works for a variety of element types
        //    -- e.g., `char` rendered as an integer via `d`, `int` rendered
        //    in hexadecimal via `x` / `#x`, zero-padded integers via `0Nd`.
        //
        // 4. The behavior is the same for `char` and `wchar_t` character
        //    types.
        //
        // 5. A spec that contains `:` alone (no letters after it) parses the
        //    underlying formatter with an empty spec (i.e., yields the
        //    same output as omitting the `:` altogether).
        //
        // 6. The element sub-specifier may itself contain a nested
        //    replacement field -- as in `bsl::format("{::{}}", range, 10)` --
        //    in which case the width of each element is taken from a
        //    `bsl::format` argument.  This holds whether the argument index
        //    is implicit or explicit, and whether or not the range itself
        //    carries a width (which sends the elements through the padding
        //    path rather than straight to the caller's output).
        //
        // 7. The element sub-specifier works with an element formatter whose
        //    `parse` returns the end of what it consumed and does not advance
        //    the context itself.
        //
        // Plan:
        // 1. Construct a `range_formatter<char, char>`, parse the spec
        //    `":d}"`, and format `vector<char>{'H','e','l','l','o'}`.
        //    Verify the output is `"[72, 101, 108, 108, 111]"`.  (C-1)
        //
        // 2. Repeat with spec `"n:d}"` and verify the brackets are
        //    suppressed.  (C-2)
        //
        // 3. Construct a `range_formatter<int, char>`, parse the specs
        //    `":x}"`, `":#x}"`, and `":03d}"`, and format representative
        //    integer ranges.  Verify the expected hexadecimal / padded
        //    outputs.  (C-3)
        //
        // 4. Repeat step 1 for `wchar_t`.  (C-4)
        //
        // 5. Parse spec `":}"` and verify the output equals the default
        //    (`}`) spec's output.  (C-5)
        //
        // 6. Parse the specs `":{}}"`, `":{0}}"`, `"*<20:{}}"` and
        //    `"{}:{}}"` in a parse context having the required number of
        //    arguments, format an integer range in a format context whose
        //    arguments supply the widths, and verify the rendered output.
        //    Repeat the first of these for `wchar_t`.  (C-6)
        //
        // 7. Parse `":x}"` and `":}"` into a `range_formatter` over a type
        //    whose `parse` only returns its end iterator, and verify the
        //    rendered output.  (C-7)
        //
        // Testing:
        //   ELEMENT FORMAT SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nELEMENT FORMAT SPECIFICATION"
                            "\n============================\n");

        if (verbose) printf(
                    "\t`vector<char>` formatted with `d` element spec.\n");
        {
            const char       arr[] = {'H', 'e', 'l', 'l', 'o'};
            bsl::vector<char> value(arr, arr + sizeof(arr));

            {
                CharRangeC formatter;
                parseFormat(&formatter, ":d}");
                ASSERTV(formatToString(formatter, value).c_str(),
                        "[72, 101, 108, 108, 111]" ==
                                        formatToString(formatter, value));
            }

            {
                CharRangeC formatter;
                parseFormat(&formatter, "n:d}");
                ASSERTV(formatToString(formatter, value).c_str(),
                        "72, 101, 108, 108, 111" ==
                                        formatToString(formatter, value));
            }
        }

        if (verbose) printf(
                    "\t`vector<int>` formatted with `x` / `#x` / `03d`.\n");
        {
            const int   arr[] = {10, 255, 4096};
            bsl::vector<int> value(arr, arr + sizeof(arr) / sizeof(*arr));

            {
                IntRangeC formatter;
                parseFormat(&formatter, ":x}");
                ASSERTV(formatToString(formatter, value).c_str(),
                        "[a, ff, 1000]" == formatToString(formatter, value));
            }

            {
                IntRangeC formatter;
                parseFormat(&formatter, ":#x}");
                ASSERTV(formatToString(formatter, value).c_str(),
                        "[0xa, 0xff, 0x1000]" ==
                                        formatToString(formatter, value));
            }

            {
                const int   small[] = {1, 2, 3};
                bsl::vector<int> v(small, small + 3);
                IntRangeC formatter;
                parseFormat(&formatter, ":03d}");
                ASSERTV(formatToString(formatter, v).c_str(),
                        "[001, 002, 003]" == formatToString(formatter, v));

                parseFormat(&formatter, ":-^3}");
                ASSERTV(formatToString(formatter, v).c_str(),
                        "[-1-, -2-, -3-]" == formatToString(formatter, v));

                parseFormat(&formatter, "*<20:-^3}");
                ASSERTV(formatToString(formatter, v).c_str(),
                       "[-1-, -2-, -3-]*****" == formatToString(formatter, v));
            }
        }

        if (verbose) printf("\t`vector<char>` `wchar_t` character type.\n");
        {
            const char        arr[] = {'H', 'e', 'l', 'l', 'o'};
            bsl::vector<char> value(arr, arr + sizeof(arr));

            CharRangeW formatter;
            parseFormat(&formatter, L":d}");
            ASSERT(L"[72, 101, 108, 108, 111]" ==
                                        formatToWString(formatter, value));
        }

        if (verbose) printf("\tEmpty element spec `\":}\"`.\n");
        {
            const int        arr[] = {1, 2, 3};
            bsl::vector<int> value(arr, arr + 3);

            IntRangeC withColon;
            parseFormat(&withColon, ":}");

            IntRangeC noColon;
            parseFormat(&noColon, "}");

            ASSERTV(formatToString(withColon, value).c_str(),
                    formatToString(withColon, value) ==
                                        formatToString(noColon, value));
        }

        if (verbose) printf("\tElement spec referring to an argument.\n");
        {
            // This is the `bsl::format("{::{}}", range, 5)` case: everything
            // after the `:` describes an element, and the `{}` in it takes
            // the element width from an argument.

            const int        arr[] = {1, 2};
            bsl::vector<int> value(arr, arr + 2);

            if (verbose) printf("\t\tImplicit argument index.\n");
            {
                IntRangeC formatter;
                parseFormat(&formatter, ":{}}", 1);
                ASSERTV(formatToString(formatter, value, 5).c_str(),
                        "[    1,     2]" ==
                                        formatToString(formatter, value, 5));
            }

            if (verbose) printf("\t\tExplicit argument index.\n");
            {
                IntRangeC formatter;
                parseFormat(&formatter, ":{0}}", 1);
                ASSERTV(formatToString(formatter, value, 5).c_str(),
                        "[    1,     2]" ==
                                        formatToString(formatter, value, 5));
            }

            if (verbose) printf("\t\t`wchar_t` character type.\n");
            {
                IntRangeW formatter;
                parseFormat(&formatter, L":{}}", 1);
                ASSERT(L"[    1,     2]" ==
                                       formatToWString(formatter, value, 5));
            }

            if (verbose) printf("\t\tWith a width on the range itself.\n");
            {
                // The range body is 14 columns wide, so four fill characters
                // are needed to reach the requested 18.

                IntRangeC formatter;
                parseFormat(&formatter, "*<18:{}}", 1);
                ASSERTV(formatToString(formatter, value, 5).c_str(),
                        "[    1,     2]****" ==
                                        formatToString(formatter, value, 5));
            }

            if (verbose) printf("\t\tBoth widths from arguments.\n");
            {
                // The first nested field is the width of the range, the
                // second that of an element.

                IntRangeC formatter;
                parseFormat(&formatter, "{}:{}}", 2);
                ASSERTV(formatToString(formatter, value, 18, 5).c_str(),
                        "[    1,     2]    " ==
                                    formatToString(formatter, value, 18, 5));
            }
        }

        if (verbose) printf("\tElement formatter that does not advance.\n");
        {
            // The element formatter reports the end of its specification only
            // through its return value.  The range must advance past it.

            ReturnOnlyType range[] = {{1}, {2}};

            range_formatter<ReturnOnlyType, char> withSpec;
            parseFormat(&withSpec, ":x}");
            ASSERTV(formatToString(withSpec, range).c_str(),
                    "[x1, x2]" == formatToString(withSpec, range));

            range_formatter<ReturnOnlyType, char> emptySpec;
            parseFormat(&emptySpec, ":}");
            ASSERTV(formatToString(emptySpec, range).c_str(),
                    "[1, 2]" == formatToString(emptySpec, range));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // `MapLikeFormatter`
        //
        // Concerns:
        // 1. The `MapLikeFormatter` default constructor sets the outer
        //    brackets to `{` and `}` and, unlike `range_formatter`, also
        //    configures the underlying pair formatter to have empty brackets
        //    and a `: ` separator (matching map-like output).
        //
        // 2. The overridden `parse` method allows the `m` letter (map format
        //    specifier) without throwing, while still setting `noBrackets`
        //    when `n` is present.
        //
        // 3. The `format` method produces the map-like `{k: v, k: v}`
        //    representation.
        //
        // Plan:
        // 1. Default-construct a `MapLikeFormatter<pair<int,int>, char>`, use
        //    an empty format string, and format an array of pairs.  Verify
        //    that the output matches `{1: 2, 3: 4}`.  (C-1, 3)
        //
        // 2. Verify that parsing a spec containing `m` does not throw and
        //    produces the same output.  (C-2)
        //
        // 3. Verify that parsing a spec containing `n` removes the outer
        //    brackets from the output.  (C-2)
        //
        // 4. Repeat step 1 for the `wchar_t` variant.  (C-1..3)
        //
        // Testing:
        //   MapLikeFormatter();
        //   typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) printf("\n`MapLikeFormatter`"
                            "\n==================\n");

        {
            IntPair range[] = {IntPair(1, 2), IntPair(3, 4)};

            {
                IntMapC formatter;
                parseFormat(&formatter, "}");
                ASSERTV(formatToString(formatter, range).c_str(),
                        "{1: 2, 3: 4}" == formatToString(formatter, range));
            }
            {
                // `m` specifier is accepted by MapLikeFormatter.
                IntMapC formatter;
                parseFormat(&formatter, "m}");
                ASSERTV(formatToString(formatter, range).c_str(),
                        "{1: 2, 3: 4}" == formatToString(formatter, range));
            }
            {
                // `n` specifier removes the outer brackets.
                IntMapC formatter;
                parseFormat(&formatter, "n}");
                ASSERTV(formatToString(formatter, range).c_str(),
                        "1: 2, 3: 4" == formatToString(formatter, range));
            }
            {
                // `nm` specifier is also accepted.
                IntMapC formatter;
                parseFormat(&formatter, "nm}");
                ASSERTV(formatToString(formatter, range).c_str(),
                        "1: 2, 3: 4" == formatToString(formatter, range));
            }
#ifdef BDE_BUILD_TARGET_EXC
            {
                // But an invalid specifier still throws.
                IntMapC formatter;
                VERIFY_EXCEPTION_IS_THROWN(L_, parseFormat(&formatter, "x}"));
            }
#endif
        }

        {
            IntPair range[] = {IntPair(1, 2), IntPair(3, 4)};

            IntMapW formatter;
            parseFormat(&formatter, L"}");
            ASSERT(L"{1: 2, 3: 4}" == formatToWString(formatter, range));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // `SetLikeFormatter`
        //
        // Concerns:
        // 1. The `SetLikeFormatter` default constructor sets the outer
        //    brackets to `{` and `}`, while inheriting the default separator
        //    from `range_formatter`.
        //
        // 2. The `format` method produces the set-like `{v1, v2}`
        //    representation.
        //
        // Plan:
        // 1. Default-construct a `SetLikeFormatter<int, char>`, use an empty
        //    format string, and format an integer array.  Verify that the
        //    output matches `{1, 2, 3}`.  (C-1..2)
        //
        // 2. Repeat step 1 for the `wchar_t` variant.  (C-1..2)
        //
        // Testing:
        //   SetLikeFormatter();
        // --------------------------------------------------------------------

        if (verbose) printf("\n`SetLikeFormatter`"
                            "\n==================\n");

        {
            int range[] = {1, 2, 3};

            IntSetC formatter;
            parseFormat(&formatter, "}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "{1, 2, 3}" == formatToString(formatter, range));
        }

        {
            int range[] = {1, 2, 3};

            IntSetW formatter;
            parseFormat(&formatter, L"}");
            ASSERT(L"{1, 2, 3}" == formatToWString(formatter, range));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `format`
        //
        // Concerns:
        // 1. `range_formatter::format` produces the expected representation
        //    for a variety of ranges (arrays, `bsl::pair` sequences, etc.).
        //
        // 2. The output respects the currently configured brackets and
        //    separator (as set by the constructor, `set_brackets`, and
        //    `set_separator`, or via a `parse`d format spec).
        //
        // 3. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Format an integer array using a default-constructed
        //    `range_formatter<int, char>` (after a `parse` with an empty
        //    spec) and verify the output is `[1, 2, 3]`.  Repeat with
        //    customized brackets and separator.  (C-1..2)
        //
        // 2. Verify that formatting an empty range yields just the brackets
        //    with no separator.  (C-1)
        //
        // 3. Repeat step 1 for `wchar_t`.  (C-3)
        //
        // Testing:
        //   typename t_FORMAT_CONTEXT::iterator format(t_RANGE&,
        //                                             t_FORMAT_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `format`"
                            "\n================\n");

        if (verbose) printf("\tFormatting a `char` integer range.\n");
        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "[1, 2, 3]" == formatToString(formatter, range));
        }

        if (verbose) printf("\tFormatting a single-element range.\n");
        {
            int range[] = {42};

            IntRangeC formatter;
            parseFormat(&formatter, "}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "[42]" == formatToString(formatter, range));
        }

        if (verbose) printf("\tFormatting an empty range.\n");
        {
            int range[1] = {0};

            IntRangeC formatter;
            parseFormat(&formatter, "}");

            bslfmt::MockFormatContext<char>          mfc(0);
            typedef bslfmt::MockFormatContext<char>::iterator OutIt;
            // Use a zero-length subrange by providing begin==end.
            OutIt out = mfc.out();
            (void) out;
            mfc.advance_to(formatter.format(range, mfc));
            // For an array of 1 element the output is "[0]".
            ASSERT("[0]" == bsl::string(mfc.finalString()));
        }

        if (verbose) printf("\tFormatting with the 'n' spec (no brackets).\n");
        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "n}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "1, 2, 3" == formatToString(formatter, range));
        }

        if (verbose) printf("\tFormatting with custom brackets/separator.\n");
        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "}");

            const bsl::string_view open("<");
            const bsl::string_view close(">");
            const bsl::string_view sep(" | ");
            formatter.set_brackets(open, close);
            formatter.set_separator(sep);

            ASSERTV(formatToString(formatter, range).c_str(),
                    "<1 | 2 | 3>" == formatToString(formatter, range));
        }

        if (verbose) printf("\tFormatting a `wchar_t` integer range.\n");
        {
            int range[] = {1, 2, 3};

            IntRangeW formatter;
            parseFormat(&formatter, L"}");
            ASSERT(L"[1, 2, 3]" == formatToWString(formatter, range));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `parse`
        //
        // Concerns:
        // 1. `range_formatter::parse` succeeds for a variety of valid format
        //    strings including empty, `n`, and width/fill specifications, so
        //    long as the input ends in `}`.
        //
        // 2. `range_formatter::parse` throws a `format_error` when the input
        //    contains the `m` specifier (which is reserved for map-like
        //    formatting).
        //
        // 3. `range_formatter::parse` throws a `format_error` when the input
        //    does not end with a `}` character (or the input is exhausted
        //    without a `}`).
        //
        // 4. `parse` returns an iterator pointing to the terminating `}` of
        //    the input.
        //
        // 5. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Using a table-based approach, parse a set of valid specs.
        //    Verify that `parse` does not throw and returns an iterator
        //    pointing to `}`.  (C-1, 4, 5)
        //
        // 2. Using a table-based approach, parse a set of invalid specs.
        //    Verify that `parse` throws `bslfmt::format_error`.  (C-2..3, 5)
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
                const char *d_spec_p;  // format spec (including trailing '}')
            } DATA[] = {
                //LINE SPEC
                //---- --------
                { L_,  "}"       },
                { L_,  "n}"      },
                { L_,  "10}"     },
                { L_,  "10n}"    },
                { L_,  "*<10}"   },
                { L_,  "*<10n}"  },
                { L_,  "*<n}"    },
                { L_,  "*^n}"    },
                { L_,  "*>10n}"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                IntRangeC                      formatter;
                bslfmt::MockParseContext<char> context(SPEC, 0);
                bslfmt::MockParseContext<char>::iterator it =
                                                     formatter.parse(context);
                ASSERTV(LINE, SPEC, it != context.end());
                ASSERTV(LINE, SPEC, '}' == *it);
            }
        }

        if (verbose) printf("\tValid `wchar_t` specifications.\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_spec_p;
            } DATA[] = {
                { L_,  L"}"       },
                { L_,  L"n}"      },
                { L_,  L"*<10n}"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const wchar_t *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                IntRangeW                         formatter;
                bslfmt::MockParseContext<wchar_t> context(SPEC, 0);
                bslfmt::MockParseContext<wchar_t>::iterator it =
                                                     formatter.parse(context);
                ASSERTV(LINE, it != context.end());
                ASSERTV(LINE, L'}' == *it);
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tInvalid `char` specifications.\n");
        {
            static const struct {
                int         d_line;
                const char *d_spec_p;
            } DATA[] = {
                //LINE SPEC
                //---- ------
                { L_,  "m}"    },   // 'm' is not allowed by range_formatter
                { L_,  "nm}"   },   // 'm' is not allowed by range_formatter
                { L_,  ""      },   // missing '}' - empty input
                { L_,  "n"     },   // missing '}'
                { L_,  "x}"    },   // invalid character
                { L_,  "nx}"   },   // invalid trailing character
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_line;
                const char *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                IntRangeC formatter;
                VERIFY_EXCEPTION_IS_THROWN(LINE,
                                           parseFormat(&formatter, SPEC));
            }
        }

        if (verbose) printf("\tInvalid `wchar_t` specifications.\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_spec_p;
            } DATA[] = {
                { L_,  L"m}"  },
                { L_,  L""    },
                { L_,  L"x}"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int            LINE = DATA[i].d_line;
                const wchar_t *const SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                IntRangeW formatter;
                VERIFY_EXCEPTION_IS_THROWN(LINE,
                                           parseFormat(&formatter, SPEC));
            }
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MANIPULATORS AND ACCESSORS
        //
        // Concerns:
        // 1. `set_separator` replaces the separator used between elements.
        //
        // 2. `set_brackets` replaces the opening and closing brackets.
        //
        // 3. `underlying()` returns a reference to the underlying formatter,
        //    and the modifiable overload allows callers to configure the
        //    underlying formatter for their element type.
        //
        // Plan:
        // 1. Construct a `range_formatter<int, char>`, parse an empty spec,
        //    and configure separator/brackets via `set_separator` and
        //    `set_brackets`.  Format an integer range and verify the output
        //    uses the new tokens.  (C-1..2)
        //
        // 2. Verify that the `underlying()` accessor returns the same
        //    reference from both const and non-const contexts.  (C-3)
        //
        // Testing:
        //   void set_separator(string_view);
        //   void set_brackets(string_view, string_view);
        //   UnderlyingFormatter&       underlying();
        //   const UnderlyingFormatter& underlying() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATORS AND ACCESSORS"
                            "\n==========================\n");

        if (verbose) printf("\tTesting `set_separator` / `set_brackets`.\n");
        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "}");

            const bsl::string_view sep(" | ");
            formatter.set_separator(sep);
            ASSERTV(formatToString(formatter, range).c_str(),
                    "[1 | 2 | 3]" == formatToString(formatter, range));

            const bsl::string_view open("<<");
            const bsl::string_view close(">>");
            formatter.set_brackets(open, close);
            ASSERTV(formatToString(formatter, range).c_str(),
                    "<<1 | 2 | 3>>" == formatToString(formatter, range));

            const bsl::string_view empty;
            formatter.set_brackets(empty, empty);
            ASSERTV(formatToString(formatter, range).c_str(),
                    "1 | 2 | 3" == formatToString(formatter, range));
        }

        if (verbose) printf("\tTesting `underlying()` accessor.\n");
        {
            IntRangeC        mX;
            const IntRangeC& X = mX;
            ASSERT(&mX.underlying() == &X.underlying());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS
        //
        // Concerns:
        // 1. The `range_formatter` default constructor uses `[`, `]` as the
        //    opening/closing brackets and `, ` (comma+space) as the element
        //    separator.
        //
        // Plan:
        // 1. Default-construct a `range_formatter<int, char>`, parse an
        //    empty spec, and format a small integer array.  Verify that the
        //    output matches `[1, 2, 3]`.  (C-1)
        //
        // 2. Repeat for `wchar_t`.  (C-1)
        //
        // Testing:
        //   range_formatter();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCREATORS"
                            "\n========\n");

        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "}");
            ASSERTV(formatToString(formatter, range).c_str(),
                    "[1, 2, 3]" == formatToString(formatter, range));
        }

        {
            int range[] = {1, 2, 3};

            IntRangeW formatter;
            parseFormat(&formatter, L"}");
            ASSERT(L"[1, 2, 3]" == formatToWString(formatter, range));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //    This case exercises (but does not fully test) basic
        //    functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Default-construct a `range_formatter`, `SetLikeFormatter`, and
        //    `MapLikeFormatter`, parse an empty spec, and format a small
        //    range for each.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            int range[] = {1, 2, 3};

            IntRangeC formatter;
            parseFormat(&formatter, "}");
            ASSERT("[1, 2, 3]" == formatToString(formatter, range));
        }

        {
            int range[] = {1, 2, 3};

            IntSetC formatter;
            parseFormat(&formatter, "}");
            ASSERT("{1, 2, 3}" == formatToString(formatter, range));
        }

        {
            IntPair range[] = {IntPair(1, 2), IntPair(3, 4)};

            IntMapC formatter;
            parseFormat(&formatter, "}");
            ASSERT("{1: 2, 3: 4}" == formatToString(formatter, range));
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
