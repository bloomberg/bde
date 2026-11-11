// bslfmt_formatterrange.t.cpp                                        -*-C++-*-
#include <bslfmt_formatterrange.h>

#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterpair.h>
#include <bslfmt_formatterstring.h>
#include <bslfmt_formattertestutil.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_array.h>
#include <bslstl_deque.h>
#include <bslstl_list.h>
#include <bslstl_map.h>
#include <bslstl_multimap.h>
#include <bslstl_multiset.h>
#include <bslstl_pair.h>
#include <bslstl_set.h>
#include <bslstl_span.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_unorderedmap.h>
#include <bslstl_unorderedmultimap.h>
#include <bslstl_unorderedmultiset.h>
#include <bslstl_unorderedset.h>
#include <bslstl_vector.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides trivial partial specializations of
// `bsl::formatter` for a family of range and container types.  Each
// specialization simply inherits from one of three formatter bases (already
// tested in `bslfmt_rangeformatter`):
//
// * `range_formatter`   arrays, `bsl::array`, `bsl::deque`, `bsl::list`,
//                       `bsl::span`, `bsl::vector`;
// * `SetLikeFormatter`  `bsl::set`, `bsl::multiset`,
//                        `bsl::unordered_set`, `bsl::unordered_multiset`;
// * `MapLikeFormatter`  `bsl::map`, `bsl::multimap`,
//                        `bsl::unordered_map`, `bsl::unordered_multimap`.
//
// Because the parsing / formatting behavior is inherited, this driver focuses
// on verifying that each specialization is well-formed and correctly wired to
// its expected base by:
//
// * formatting a representative value using the default (empty) spec and
//   observing the correct bracket style ("[..]" / "{..}" / "{k: v, ..}");
// * formatting the same value using the `n` spec and observing that the
//   outer brackets are dropped.
//
// Unordered containers are exercised with single-element instances to keep
// the expected output deterministic.
// ----------------------------------------------------------------------------
// FORMATTER SPECIALIZATIONS
// [ 2] formatter<t_TYPE[k_SIZE], t_CHAR>
// [ 2] formatter<bsl::array<t_TYPE, k_SIZE>, t_CHAR>
// [ 2] formatter<bsl::deque<t_TYPE, t_ALLOC>, t_CHAR>
// [ 2] formatter<bsl::list<t_TYPE, t_ALLOC>, t_CHAR>
// [ 2] formatter<bsl::span<t_TYPE, k_EXTENT>, t_CHAR>
// [ 2] formatter<bsl::vector<t_TYPE, t_ALLOC>, t_CHAR>
// [ 3] formatter<bsl::set<...>, t_CHAR>
// [ 3] formatter<bsl::multiset<...>, t_CHAR>
// [ 3] formatter<bsl::unordered_set<...>, t_CHAR>
// [ 3] formatter<bsl::unordered_multiset<...>, t_CHAR>
// [ 4] formatter<bsl::map<...>, t_CHAR>
// [ 4] formatter<bsl::multimap<...>, t_CHAR>
// [ 4] formatter<bsl::unordered_map<...>, t_CHAR>
// [ 4] formatter<bsl::unordered_multimap<...>, t_CHAR>
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// [ 5] ELEMENT SPECIFICATION WITH A NESTED ARGUMENT
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

// The component under test is only defined when the standard library does not
// already provide C++23 `std::format` range/pair support.
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

typedef bslfmt::FormatterTestUtil<char>    TestUtilChar;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Parse the specified `format` (including the trailing `}`) into the
/// specified `*formatter` using a `MockParseContext`.
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

/// Parse the specified `spec` (including the trailing `}`) using a formatter
/// of type (template parameter) `t_FORMATTER`, format the specified `value`
/// with that formatter, and compare the result to the specified `expected`.
/// Return `true` if the strings compare equal.
template <class t_FORMATTER, class t_VALUE>
bool checkFormat(const char *spec, const char *expected, const t_VALUE& value)
{
    t_FORMATTER formatter;
    parseFormat(&formatter, spec);
    return bsl::string(expected) == formatToString(formatter, value);
}

template <class t_FORMATTER, class t_VALUE>
bool checkFormatW(const wchar_t  *spec,
                  const wchar_t  *expected,
                  const t_VALUE&  value)
{
    t_FORMATTER formatter;
    parseFormat(&formatter, spec);
    return bsl::wstring(expected) == formatToWString(formatter, value);
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Formatting A Container
///- - - - - - - - - - - - - - - - -
// Suppose we wish to log the contents of a container.
//
// First, we create a vector of strings:
// ```
    void run()
    {
        bsl::vector<bsl::string> tickers;
        tickers.push_back("IBM");
        tickers.push_back("MSFT");
// ```
// Now, we render it with its default format:
// ```
        bslfmt::MockParseContext<char> parseContext("}", 0);

        bsl::formatter<bsl::vector<bsl::string>, char> formatter;
        formatter.parse(parseContext);

        bslfmt::MockFormatContext<char> formatContext(0);
        formatContext.advance_to(formatter.format(tickers, formatContext));
// ```
// Finally, we observe that the container is bracketed and comma-separated, and
// that its elements are quoted:
// ```
        ASSERT("[\"IBM\", \"MSFT\"]" == formatContext.finalString());
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
      case 6: {
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
      case 5: {
        // --------------------------------------------------------------------
        // ELEMENT SPECIFICATION WITH A NESTED ARGUMENT
        //
        // Concerns:
        // 1. An element specification may itself contain a nested
        //    replacement field -- `bsl::format("{::{}}", range, 5)` -- in
        //    which case the argument it refers to supplies the width of each
        //    element.
        //
        // 2. This holds when the range carries a width of its own, in which
        //    case the elements are rendered into a temporary buffer before
        //    the rendering as a whole is padded, and it holds when that
        //    width is itself supplied by an argument.
        //
        // 3. The same holds for the map-like specializations, whose elements
        //    are rendered by a pair formatter.
        //
        // 4. All of the above hold both when the formatter is driven by the
        //    format context of this package and when it is driven by that of
        //    the standard library, which is what requesting the oracle
        //    exercises.
        //
        // Plan:
        // 1. Using a table-based approach, format a `bsl::vector<int>` with
        //    format strings that place a nested replacement field in the
        //    element specification, with and without a width on the range
        //    itself, and verify the rendered output.  (C-1, 2, 4)
        //
        // 2. Repeat for a `bsl::map<int, char>`.  (C-3, 4)
        //
        // Testing:
        //   ELEMENT SPECIFICATION WITH A NESTED ARGUMENT
        // --------------------------------------------------------------------

        if (verbose) printf("\nELEMENT SPECIFICATION WITH A NESTED ARGUMENT"
                            "\n============================================"
                            "\n");

        if (verbose) printf("\t`bsl::vector<int>`.\n");
        {
            bsl::vector<int> value;
            value.push_back(1);
            value.push_back(2);

            static const struct {
                int         d_line;
                const char *d_format_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE FORMAT         EXPECTED
                //---- -------------  ----------------------
                { L_,  "{::{}}",      "[    1,     2]"       },
                { L_,  "{::>{}}",     "[    1,     2]"       },
                { L_,  "{::^{}}",     "[  1  ,   2  ]"       },
                { L_,  "{::*^{}}",    "[**1**, **2**]"       },
                { L_,  "{::*<{}}",    "[1****, 2****]"       },
                { L_,  "{:*<18:{}}",  "[    1,     2]****"   },
                { L_,  "{:n:{}}",     "    1,     2"         },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const FORMAT   = DATA[i].d_format_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(FORMAT) P(EXPECTED) }

                bsl::string message;
                const bool  rv = TestUtilChar::testEvaluateVFormat(
                                                                 &message,
                                                                 EXPECTED,
                                                                 true,
                                                                 FORMAT,
                                                                 value,
                                                                 5);
                ASSERTV(LINE, FORMAT, EXPECTED, message.c_str(), rv);
            }

            // Both the width of the range and that of an element may come
            // from an argument.

            bsl::string message;
            const bool  rv = TestUtilChar::testEvaluateVFormat(
                                                        &message,
                                                        "[    1,     2]    ",
                                                        true,
                                                        "{:{}:{}}",
                                                        value,
                                                        18,
                                                        5);
            ASSERTV(message.c_str(), rv);
        }

        if (verbose) printf("\t`bsl::map<int, char>`.\n");
        {
            bsl::map<int, char> value;
            value[1] = 'a';
            value[2] = 'b';

            static const struct {
                int         d_line;
                const char *d_format_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE FORMAT         EXPECTED
                //---- -------------  ------------------------------
                { L_,  "{::{}}",      "{1: 'a'  , 2: 'b'  }"         },
                { L_,  "{:*<26:{}}",  "{1: 'a'  , 2: 'b'  }******"   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const FORMAT   = DATA[i].d_format_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(FORMAT) P(EXPECTED) }

                bsl::string message;
                const bool  rv = TestUtilChar::testEvaluateVFormat(
                                                                 &message,
                                                                 EXPECTED,
                                                                 true,
                                                                 FORMAT,
                                                                 value,
                                                                 8);
                ASSERTV(LINE, FORMAT, EXPECTED, message.c_str(), rv);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MAP-LIKE FORMATTER SPECIALIZATIONS
        //
        // Concerns:
        // 1. `bsl::formatter` is specialized for `bsl::map`, `bsl::multimap`,
        //    `bsl::unordered_map`, and `bsl::unordered_multimap` by
        //    deriving from `MapLikeFormatter`, and each specialization
        //    produces the expected map-like `{k: v, k: v}` output for the
        //    default (empty) spec.
        //
        // 2. The `n` spec drops the outer `{`/`}` brackets (proving that
        //    `parse` / `format` behavior is inherited from
        //    `MapLikeFormatter`).
        //
        // 3. Both `char` and `wchar_t` instantiations are available.
        //
        // 4. Outer fill / align / width tokens and inner element (pair)
        //    format specs (delivered via `:`) are forwarded correctly, and
        //    can be combined.
        //
        // Plan:
        // 1. For each ordered container type, populate a small instance and
        //    format it with `"}"` and `"n}"` specs, verifying the output is
        //    `"{1: a, 2: b}"` / `"1: a, 2: b"`.  (C-1, 2)
        //
        // 2. For each unordered container type, populate a single-element
        //    instance and format it with `"}"` and `"n}"` specs, verifying
        //    the output is `"{1: a}"` / `"1: a"`.  (C-1, 2)
        //
        // 3. Repeat one representative container (`bsl::map`) for `wchar_t`.
        //    (C-3)
        //
        // 4. Using a table-based approach on `bsl::map<int, char>`, verify
        //    that outer fill / align / width tokens pad the whole map, that
        //    the `:` handoff correctly forwards a spec to the underlying
        //    pair formatter (pair-level fill / align / width), and that
        //    outer width can be combined with an inner pair spec.  (C-4)
        //
        // Testing:
        //   formatter<bsl::map<...>, t_CHAR>
        //   formatter<bsl::multimap<...>, t_CHAR>
        //   formatter<bsl::unordered_map<...>, t_CHAR>
        //   formatter<bsl::unordered_multimap<...>, t_CHAR>
        // --------------------------------------------------------------------

        if (verbose) printf("\nMAP-LIKE FORMATTER SPECIALIZATIONS"
                            "\n==================================\n");

        if (verbose) printf("\t`bsl::map`.\n");
        {
            typedef bsl::map<int, char>                 Map;
            typedef bsl::formatter<Map, char>           Formatter;

            Map m;
            m[1] = 'a';
            m[2] = 'b';

            ASSERT(( checkFormat<Formatter>("}",   "{1: 'a', 2: 'b'}", m) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1: 'a', 2: 'b'",   m) ));
        }

        if (verbose) printf("\t`bsl::multimap`.\n");
        {
            typedef bsl::multimap<int, char>            Multimap;
            typedef bsl::formatter<Multimap, char>      Formatter;

            Multimap m;
            m.emplace(1, 'a');
            m.emplace(2, 'b');

            ASSERT(( checkFormat<Formatter>("}",   "{1: 'a', 2: 'b'}", m) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1: 'a', 2: 'b'",   m) ));
        }

        if (verbose) printf("\t`bsl::unordered_map` (single element).\n");
        {
            typedef bsl::unordered_map<int, char>       Umap;
            typedef bsl::formatter<Umap, char>          Formatter;

            Umap u;
            u[1] = 'a';

            ASSERT(( checkFormat<Formatter>("}",   "{1: 'a'}",  u) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1: 'a'",   u) ));
        }

        if (verbose) printf("\t`bsl::unordered_multimap` (single element).\n");
        {
            typedef bsl::unordered_multimap<int, char>  Umultimap;
            typedef bsl::formatter<Umultimap, char>     Formatter;

            Umultimap u;
            u.emplace(1, 'a');

            ASSERT(( checkFormat<Formatter>("}",   "{1: 'a'}",  u) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1: 'a'",   u) ));
        }

        if (verbose) printf("\t`bsl::map` (`wchar_t`).\n");
        {
            typedef bsl::map<int, char>                 Map;
            typedef bsl::formatter<Map, wchar_t>        Formatter;

            Map m;
            m[1] = 'a';
            m[2] = 'b';

            ASSERT(( checkFormatW<Formatter>(L"}",  L"{1: 'a', 2: 'b'}", m) ));
            ASSERT(( checkFormatW<Formatter>(L"n}", L"1: 'a', 2: 'b'",   m) ));
        }

        if (verbose) printf("\t`bsl::map<int, char>` (nested format specs)."
                            "\n");
        {
            typedef bsl::map<int, char>        Map;
            typedef bsl::formatter<Map, char>  Formatter;

            Map m;
            m[1] = 'a';
            m[2] = 'b';

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC              EXPECTED
                //---- ----------------  -----------------------------
                // Default & no-brackets (body = 16 / 14 chars).  The
                // values are characters, so they are rendered in the
                // escaped presentation and appear quoted.
                { L_,  "}",              "{1: 'a', 2: 'b'}"           },
                { L_,  "n}",             "1: 'a', 2: 'b'"             },

                // Outer fill / align / width.
                { L_,  "20}",            "{1: 'a', 2: 'b'}    "       },
                { L_,  "*<20}",          "{1: 'a', 2: 'b'}****"       },
                { L_,  "*>20}",          "****{1: 'a', 2: 'b'}"       },
                { L_,  "*^18}",          "*{1: 'a', 2: 'b'}*"         },
                { L_,  "-^16n}",         "-1: 'a', 2: 'b'-"           },

                // Inner (pair) element spec via `:`.  The pair formatter
                // is pre-configured by `MapLikeFormatter` to render as
                // `k: v` with no pair brackets, so parsing pair-level
                // fill / align / width just pads that rendering.  Note
                // that supplying an element spec suppresses the range's
                // own request for debug elements, but the *pair*
                // formatter still applies it to its own elements, so the
                // characters remain quoted.
                { L_,  ":*<8}",          "{1: 'a'**, 2: 'b'**}"       },
                { L_,  ":*>8}",          "{**1: 'a', **2: 'b'}"       },

                // Outer width combined with an inner pair spec.
                { L_,  "*<25:*<8}",      "{1: 'a'**, 2: 'b'**}*****"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                ASSERTV(LINE, SPEC, EXPECTED,
                       (checkFormat<Formatter>(SPEC, EXPECTED, m)));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // SET-LIKE FORMATTER SPECIALIZATIONS
        //
        // Concerns:
        // 1. `bsl::formatter` is specialized for `bsl::set`, `bsl::multiset`,
        //    `bsl::unordered_set`, and `bsl::unordered_multiset` by
        //    deriving from `SetLikeFormatter`, and each specialization
        //    produces the expected `{a, b, c}` output for the default
        //    (empty) spec.
        //
        // 2. The `n` spec drops the outer `{`/`}` brackets (proving that
        //    `parse` / `format` behavior is inherited from
        //    `SetLikeFormatter`).
        //
        // 3. Both `char` and `wchar_t` instantiations are available.
        //
        // 4. Outer fill / align / width tokens and inner element format
        //    specs (delivered via `:`) are forwarded correctly, and can
        //    be combined.
        //
        // Plan:
        // 1. For each ordered container type, populate a three-element
        //    instance and format it with `"}"` and `"n}"` specs, verifying
        //    the output is `"{1, 2, 3}"` / `"1, 2, 3"`.  (C-1, 2)
        //
        // 2. For each unordered container type, populate a single-element
        //    instance and format it with `"}"` and `"n}"` specs, verifying
        //    the output is `"{1}"` / `"1"`.  (C-1, 2)
        //
        // 3. Repeat one representative container (`bsl::set`) for `wchar_t`.
        //    (C-3)
        //
        // 4. Using a table-based approach on `bsl::set<int>`, verify that
        //    outer fill / align / width tokens pad the whole set, that
        //    the `:` handoff correctly forwards a spec to the underlying
        //    integer formatter (`03`, `#x`, and element-level
        //    fill / align / width), and that outer width can be combined
        //    with an inner element spec.  (C-4)
        //
        // Testing:
        //   formatter<bsl::set<...>, t_CHAR>
        //   formatter<bsl::multiset<...>, t_CHAR>
        //   formatter<bsl::unordered_set<...>, t_CHAR>
        //   formatter<bsl::unordered_multiset<...>, t_CHAR>
        // --------------------------------------------------------------------

        if (verbose) printf("\nSET-LIKE FORMATTER SPECIALIZATIONS"
                            "\n==================================\n");

        const int         values[] = {1, 2, 3};
        const std::size_t NUM      = sizeof(values) / sizeof(*values);

        if (verbose) printf("\t`bsl::set`.\n");
        {
            typedef bsl::set<int>                       Set;
            typedef bsl::formatter<Set, char>           Formatter;

            Set s(values, values + NUM);

            ASSERT(( checkFormat<Formatter>("}",   "{1, 2, 3}", s) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  s) ));
        }

        if (verbose) printf("\t`bsl::multiset`.\n");
        {
            typedef bsl::multiset<int>              Multiset;
            typedef bsl::formatter<Multiset, char>  Formatter;

            Multiset s(values, values + NUM);

            ASSERT(( checkFormat<Formatter>("}",   "{1, 2, 3}", s) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  s) ));
        }

        if (verbose) printf("\t`bsl::unordered_set` (single element).\n");
        {
            typedef bsl::unordered_set<int>     Uset;
            typedef bsl::formatter<Uset, char>  Formatter;

            Uset u(values, values + 1);

            ASSERT(( checkFormat<Formatter>("}",   "{1}", u) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1",  u) ));
        }

        if (verbose) printf("\t`bsl::unordered_multiset` (single element).\n");
        {
            typedef bsl::unordered_multiset<int>     Umultiset;
            typedef bsl::formatter<Umultiset, char>  Formatter;

            Umultiset u(values, values + 1);

            ASSERT(( checkFormat<Formatter>("}",   "{1}", u) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1",  u) ));
        }

        if (verbose) printf("\t`bsl::set` (`wchar_t`).\n");
        {
            typedef bsl::set<int>                 Set;
            typedef bsl::formatter<Set, wchar_t>  Formatter;

            Set s(values, values + NUM);

            ASSERT(( checkFormatW<Formatter>(L"}",   L"{1, 2, 3}", s) ));
            ASSERT(( checkFormatW<Formatter>(L"n}",  L"1, 2, 3",  s) ));
        }

        if (verbose) printf("\t`bsl::set<int>` (nested format specs).\n");
        {
            typedef bsl::set<int>              Set;
            typedef bsl::formatter<Set, char>  Formatter;

            const Set s(values, values + NUM);

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC          EXPECTED
                //---- ------------  ---------------------------
                // Default & no-brackets (body = 9 / 7 chars).
                { L_,  "}",          "{1, 2, 3}"              },
                { L_,  "n}",         "1, 2, 3"                },

                // Outer fill / align / width.
                { L_,  "15}",        "{1, 2, 3}      "        },
                { L_,  "*<15}",      "{1, 2, 3}******"        },
                { L_,  "*>15}",      "******{1, 2, 3}"        },
                { L_,  "*^13}",      "**{1, 2, 3}**"          },
                { L_,  "-^11n}",     "--1, 2, 3--"            },

                // Inner (integer) element spec via `:`.
                { L_,  ":03}",       "{001, 002, 003}"        },
                { L_,  ":#x}",       "{0x1, 0x2, 0x3}"        },
                { L_,  ":*<3}",      "{1**, 2**, 3**}"        },

                // Outer width combined with an inner element spec.
                { L_,  "*<20:03}",   "{001, 002, 003}*****"   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                ASSERTV(LINE, SPEC, EXPECTED,
                       (checkFormat<Formatter>(SPEC, EXPECTED, s)));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RANGE FORMATTER SPECIALIZATIONS
        //
        // Concerns:
        // 1. `bsl::formatter` is specialized for C-style arrays,
        //    `bsl::array`, `bsl::deque`, `bsl::list`, `bsl::span`, and
        //    `bsl::vector` by deriving from `range_formatter`, and each
        //    specialization produces the expected `[a, b, c]` output for
        //    the default (empty) spec.
        //
        // 2. The `n` spec drops the outer `[`/`]` brackets (proving that
        //    `parse` / `format` behavior is inherited from
        //    `range_formatter`).
        //
        // 3. Both `char` and `wchar_t` instantiations are available.
        //
        // 4. A range whose element type is `bsl::pair` is still handled by
        //    `range_formatter` (not `MapLikeFormatter`); the default spec
        //    renders each pair with its pair-formatter representation
        //    `(k, v)`, and the `m` specifier is rejected with
        //    `bsl::format_error` because `range_formatter` is not
        //    map-like.
        //
        // Plan:
        // 1. For each range type, populate an instance with `{1, 2, 3}` and
        //    format it with `"}"` and `"n}"` specs, verifying the output is
        //    `"[1, 2, 3]"` / `"1, 2, 3"`.  (C-1, 2)
        //
        // 2. Repeat one representative container (`bsl::vector`) for
        //    `wchar_t`.  (C-3)
        //
        // 3. Using a table-based approach on `bsl::vector<int>`, verify
        //    that outer fill / align / width tokens pad the whole range,
        //    that the `:` handoff correctly forwards a spec to the
        //    underlying integer formatter (`x`, `#x`, `04d`, and
        //    pair-level fill / align / width), and that outer width can be
        //    combined with an inner element spec.  (C-1, 2)
        //
        // 4. Using a table-based approach on
        //    `bsl::vector<bsl::pair<int, char> >`, verify:
        //      - the default and `n` range specs render each element with
        //        the pair formatter's default `(k, v)` form;
        //      - outer fill / align / width tokens pad the whole range;
        //      - the `:` handoff correctly forwards a spec to the
        //        underlying pair formatter (including the pair `m` and
        //        `n` letters and pair-level fill / align / width);
        //      - outer width and inner pair specs can be combined; and
        //      - the bare `m` specifier is rejected with
        //        `bsl::format_error`.  (C-4)
        //
        // Testing:
        //   formatter<t_TYPE[k_SIZE], t_CHAR>
        //   formatter<bsl::array<t_TYPE, k_SIZE>, t_CHAR>
        //   formatter<bsl::deque<t_TYPE, t_ALLOC>, t_CHAR>
        //   formatter<bsl::list<t_TYPE, t_ALLOC>, t_CHAR>
        //   formatter<bsl::span<t_TYPE, k_EXTENT>, t_CHAR>
        //   formatter<bsl::vector<t_TYPE, t_ALLOC>, t_CHAR>
        // --------------------------------------------------------------------

        if (verbose) printf("\nRANGE FORMATTER SPECIALIZATIONS"
                            "\n===============================\n");

        const int values[] = {1, 2, 3};

        if (verbose) printf("\tC-style array.\n");
        {
            typedef int                                 Arr[3];
            typedef bsl::formatter<Arr, char>           Formatter;

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", values) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  values) ));
        }

        if (verbose) printf("\t`bsl::array`.\n");
        {
            typedef bsl::array<int, 3>                  Array;
            typedef bsl::formatter<Array, char>         Formatter;

            const Array a = {1, 2, 3};

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", a) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  a) ));
        }

        if (verbose) printf("\t`bsl::deque`.\n");
        {
            typedef bsl::deque<int>                     Deque;
            typedef bsl::formatter<Deque, char>         Formatter;

            const Deque d(values, values + 3);

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", d) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  d) ));
        }

        if (verbose) printf("\t`bsl::list`.\n");
        {
            typedef bsl::list<int>                      List;
            typedef bsl::formatter<List, char>          Formatter;

            const List l(values, values + 3);

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", l) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  l) ));
        }

        if (verbose) printf("\t`bsl::span` (dynamic extent).\n");
        {
            typedef bsl::span<const int>                Span;
            typedef bsl::formatter<Span, char>          Formatter;

            const Span s(values, 3);

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", s) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  s) ));
        }

        if (verbose) printf("\t`bsl::span` (static extent).\n");
        {
            typedef bsl::span<const int, 3>             Span;
            typedef bsl::formatter<Span, char>          Formatter;

            const Span s(values, 3);

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", s) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  s) ));
        }

        if (verbose) printf("\t`bsl::vector`.\n");
        {
            typedef bsl::vector<int>                    Vector;
            typedef bsl::formatter<Vector, char>        Formatter;

            const Vector v(values, values + 3);

            ASSERT(( checkFormat<Formatter>("}",   "[1, 2, 3]", v) ));
            ASSERT(( checkFormat<Formatter>("n}",  "1, 2, 3",  v) ));
        }

        if (verbose) printf("\t`bsl::vector` (`wchar_t`).\n");
        {
            typedef bsl::vector<int>                    Vector;
            typedef bsl::formatter<Vector, wchar_t>     Formatter;

            const Vector v(values, values + 3);

            ASSERT(( checkFormatW<Formatter>(L"}",   L"[1, 2, 3]", v) ));
            ASSERT(( checkFormatW<Formatter>(L"n}",  L"1, 2, 3",  v) ));
        }

        if (verbose) printf("\t`bsl::vector<int>` (nested format specs).\n");
        {
            typedef bsl::vector<int>                    Vector;
            typedef bsl::formatter<Vector, char>        Formatter;

            const int  data[]  = {10, 15, 255};
            const Vector v(data, data + 3);

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC          EXPECTED
                //---- ------------  ---------------------------
                // Default & no-brackets (body = 13 / 11 chars).
                { L_,  "}",          "[10, 15, 255]"          },
                { L_,  "n}",         "10, 15, 255"            },

                // Outer fill / align / width.
                { L_,  "20}",        "[10, 15, 255]       "   },
                { L_,  "*<20}",      "[10, 15, 255]*******"   },
                { L_,  "*>20}",      "*******[10, 15, 255]"   },
                { L_,  "*^19}",      "***[10, 15, 255]***"    },
                { L_,  "-^17n}",     "---10, 15, 255---"      },

                // Inner (integer) element spec via `:`.
                { L_,  ":x}",        "[a, f, ff]"             },
                { L_,  ":#x}",       "[0xa, 0xf, 0xff]"       },
                { L_,  ":04d}",      "[0010, 0015, 0255]"     },
                { L_,  ":*<5}",      "[10***, 15***, 255**]"  },
                { L_,  ":*>5}",      "[***10, ***15, **255]"  },

                // Outer width combined with an inner element spec.
                { L_,  "*<25:x}",    "[a, f, ff]***************" },
                { L_,  "*^22:04d}",  "**[0010, 0015, 0255]**"  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                ASSERTV(LINE, SPEC, EXPECTED,
                       (checkFormat<Formatter>(SPEC, EXPECTED, v)));
            }
        }

        if (verbose) printf("\t`bsl::vector<bsl::pair<int, char> >`.\n");
        {
            typedef bsl::pair<int, char>                Pair;
            typedef bsl::vector<Pair>                   Vector;
            typedef bsl::formatter<Vector, char>        Formatter;

            Vector v;
            v.push_back(Pair(1, 'a'));
            v.push_back(Pair(2, 'b'));
            v.push_back(Pair(3, 'c'));

            // Table columns:
            //  * SPEC     -- range format specification (everything that
            //                would appear between `{:` and `}` in a full
            //                format string, including the trailing `}`).
            //  * EXPECTED -- expected rendering of `v`.
            //
            // Coverage:
            //  * default and `n` (no-bracket) range specs;
            //  * outer fill / align / width applied to the whole range;
            //  * `:` handing a spec to the underlying `pair` formatter,
            //    exercising the pair `m` and `n` letters and pair-level
            //    fill / align / width;
            //  * combinations of outer width with an inner pair spec.

            static const struct {
                int         d_line;
                const char *d_spec_p;
                const char *d_expected_p;
            } DATA[] = {
                //LINE SPEC            EXPECTED
                //---- --------------  --------------------------------------
                // Default & no-brackets.  The pair elements are rendered in
                // the escaped ("debug") presentation per [format.tuple], so
                // each character value appears quoted and each pair is
                // `(k, 'v')` = 8 chars.
                { L_,  "}",            "[(1, 'a'), (2, 'b'), (3, 'c')]"     },
                { L_,  "n}",           "(1, 'a'), (2, 'b'), (3, 'c')"       },

                // Outer fill / align / width on the range as a whole; the
                // rendered body `[(1, 'a'), (2, 'b'), (3, 'c')]` is 30 chars.
                { L_,  "36}",          "[(1, 'a'), (2, 'b'), (3, 'c')]      "
                                                                            },
                { L_,  "*<36}",        "[(1, 'a'), (2, 'b'), (3, 'c')]******"
                                                                            },
                { L_,  "*>36}",        "******[(1, 'a'), (2, 'b'), (3, 'c')]"
                                                                            },
                { L_,  "*^36}",        "***[(1, 'a'), (2, 'b'), (3, 'c')]***"
                                                                            },
                { L_,  "-^34}",        "--[(1, 'a'), (2, 'b'), (3, 'c')]--" },
                { L_,  "#>32}",        "##[(1, 'a'), (2, 'b'), (3, 'c')]"   },

                // Outer width combined with `n` (body without brackets is
                // 28 chars: `(1, 'a'), (2, 'b'), (3, 'c')`).
                { L_,  "*<32n}",       "(1, 'a'), (2, 'b'), (3, 'c')****"   },
                { L_,  "*>32n}",       "****(1, 'a'), (2, 'b'), (3, 'c')"   },

                // Inner (pair) formatter spec via `:`.  The pair `m` letter
                // strips the pair's parentheses and switches its separator
                // to `: `; the pair `n` letter just strips its parentheses.
                // Note that supplying an element spec suppresses the range's
                // own request for debug elements, but the pair formatter
                // still applies it to its own, so values stay quoted.
                { L_,  ":m}",          "[1: 'a', 2: 'b', 3: 'c']"           },
                { L_,  "n:m}",         "1: 'a', 2: 'b', 3: 'c'"             },
                { L_,  ":n}",          "[1, 'a', 2, 'b', 3, 'c']"           },
                { L_,  "n:n}",         "1, 'a', 2, 'b', 3, 'c'"             },

                // Outer width with an inner pair `m` spec (body is
                // `[1: 'a', 2: 'b', 3: 'c']` = 24 chars).
                { L_,  "*<30:m}",      "[1: 'a', 2: 'b', 3: 'c']******"     },
                { L_,  "*>30:m}",      "******[1: 'a', 2: 'b', 3: 'c']"     },
                { L_,  "*^28:m}",      "**[1: 'a', 2: 'b', 3: 'c']**"       },

                // Pair-level fill / align / width applied to each element
                // (each pair rendered as `(k, 'v')` = 8 chars, padded per the
                // inner spec, then joined with the range's `, ` separator
                // and wrapped in the default `[`/`]` brackets).
                { L_,  ":*<12}",
                             "[(1, 'a')****, (2, 'b')****, (3, 'c')****]"   },
                { L_,  ":*>10}",       "[**(1, 'a'), **(2, 'b'), **(3, 'c')]"
                                                                            },
                { L_,  ":*^12}",
                             "[**(1, 'a')**, **(2, 'b')**, **(3, 'c')**]"   },

                // Pair-level fill / align / width combined with pair `m`
                // (each pair rendered as `k: 'v'` = 6 chars, padded to 10).
                { L_,  ":*<10m}",      "[1: 'a'****, 2: 'b'****, 3: 'c'****]"
                                                                            },
                { L_,  ":*>10m}",      "[****1: 'a', ****2: 'b', ****3: 'c']"
                                                                            },

                // The range-level `m` letter is valid for any range whose
                // element is a pair, not only for the map-like containers.
                // It renders the range as a map: braces outside, and each
                // element as `k: v`.  (Body is `{1: 'a', 2: 'b', 3: 'c'}` =
                // 24 chars.)
                { L_,  "m}",           "{1: 'a', 2: 'b', 3: 'c'}"           },
                { L_,  "nm}",          "1: 'a', 2: 'b', 3: 'c'"             },
                { L_,  "*<30m}",       "{1: 'a', 2: 'b', 3: 'c'}******"     },
                { L_,  "*>30m}",       "******{1: 'a', 2: 'b', 3: 'c'}"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char *const EXPECTED = DATA[i].d_expected_p;

                if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(EXPECTED) }

                ASSERTV(LINE, SPEC, EXPECTED,
                       (checkFormat<Formatter>(SPEC, EXPECTED, v)));
            }

            // The `m` specifier is gated on the element type, so a range
            // whose element is not a pair must reject it with
            // `bsl::format_error`.
#ifdef BDE_BUILD_TARGET_EXC
            bsl::formatter<bsl::vector<int>, char> intVectorFormatter;
            VERIFY_EXCEPTION_IS_THROWN(
                                L_,
                                parseFormat(&intVectorFormatter, "m}"));
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. A representative range specialization (`bsl::vector`), a
        //    representative set-like specialization (`bsl::set`), and a
        //    representative map-like specialization (`bsl::map`) can each be
        //    default-constructed and produce the expected output for the
        //    empty spec.
        //
        // Plan:
        // 1. Format a small instance of each of the three representative
        //    containers.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const int values[] = {1, 2, 3};

        {
            typedef bsl::vector<int>              Vector;
            typedef bsl::formatter<Vector, char>  Formatter;

            const Vector v(values, values + 3);

            ASSERT(( checkFormat<Formatter>("}", "[1, 2, 3]", v) ));
        }

        {
            typedef bsl::set<int>              Set;
            typedef bsl::formatter<Set, char>  Formatter;

            const Set s(values, values + 3);

            ASSERT(( checkFormat<Formatter>("}", "{1, 2, 3}", s) ));
        }

        {
            typedef bsl::map<int, char>        Map;
            typedef bsl::formatter<Map, char>  Formatter;

            Map m;
            m[1] = 'a';
            m[2] = 'b';

            ASSERT(( checkFormat<Formatter>("}", "{1: 'a', 2: 'b'}", m) ));
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
