// bslfmt_rangeformatspecification.t.cpp                              -*-C++-*-
#include <bslfmt_rangeformatspecification.h>

#include <bslfmt_formatterintegral.h>
#include <bslfmt_mockparsecontext.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_stringview.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bslfmt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a value-semantic attribute class that
// parses the format specification for ranges.  The class's `parse` method
// delegates its work to a `FormatSpecificationParser` (which is tested
// separately) and then interprets the single-character tokens `n` and `m` in
// the remaining specification.  If nothing else remains, `parse` delegates to
// the caller-supplied underlying formatter's `parse` method.
//
// According to the BDE practice, the constructor and manipulators are tested
// first (test case 2), followed by the basic accessors (test case 3).  Test
// case 4 provides a table-driven exhaustive test of the `parse` method,
// including negative testing of invalid specifications.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] RangeFormatSpecification();
//
// MANIPULATORS
// [ 4] void parse(t_PARSE_CONTEXT *ctx, t_UNDERLYING_FORMATTER *underlying);
//
// ACCESSORS
// [ 3] Alignment    alignment()           const;
// [ 3] const t_CHAR *filler()             const;
// [ 3] bool         isMap()               const;
// [ 4] bool         isEscapedString()     const;
// [ 4] bool         isString()            const;
// [ 3] bool         noBrackets()          const;
// [ 3] int          numFillerCharacters() const;
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
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

// `RangeFormatSpecification` is only defined when the standard library does
// not already provide C++23 `std::format` range support.
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

typedef RangeFormatSpecification<char>       CharSpec;
typedef RangeFormatSpecification<wchar_t>    WcharSpec;
typedef FormatSpecificationParserEnums       ParserEnums;

//=============================================================================
//                            HELPER CLASSES
//-----------------------------------------------------------------------------

/// A minimal mock underlying formatter suitable for calling from
/// `RangeFormatSpecification::parse`.  Records whether `parse` has been
/// called on it.
template <class t_CHAR>
struct MockUnderlyingFormatter {
    // DATA
    bool d_parseCalled;

    // CREATORS
    MockUnderlyingFormatter()
    : d_parseCalled(false)
    {
    }

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& ctx)
    {
        d_parseCalled = true;
        return ctx.begin();
    }
};

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Parse the specified `input` as a `char` range format specification into
/// the specified `*spec`, using the specified `*underlying` as the underlying
/// formatter object.
void parseRange(CharSpec                       *spec,
                MockUnderlyingFormatter<char>  *underlying,
                const bsl::string_view&         input)
{
    bslfmt::MockParseContext<char> context(input, 0);
    spec->parse(&context, underlying);
}

/// Parse the specified `input` as a `wchar_t` range format specification into
/// the specified `*spec`, using the specified `*underlying` as the underlying
/// formatter object.
void parseRange(WcharSpec                        *spec,
                MockUnderlyingFormatter<wchar_t> *underlying,
                const bsl::wstring_view&          input)
{
    bslfmt::MockParseContext<wchar_t> context(input, 0);
    spec->parse(&context, underlying);
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Parsing A Range Format Specification
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a formatter for a range and need to know how to
// punctuate the range itself.
//
// First, we parse a format string into a specification, supplying a formatter
// for `int` to parse the element specification:
// ```
    void run()
    {
        typedef bslfmt::RangeFormatSpecification<char> Specification;

        bslfmt::MockParseContext<char> context("*<20n:#x}", 0);

        Specification             spec;
        bsl::formatter<int, char> element;

        spec.parse(&context, &element);
// ```
// Finally, we verify the parsed specification:
// ```
        ASSERT(true                        == spec.noBrackets());
        ASSERT(false                       == spec.isMap());
        ASSERT(true                        == spec.hasElementSpecification());
        ASSERT('*'                         == *spec.filler());
        ASSERT(Specification::e_ALIGN_LEFT == spec.alignment());
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
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `parse`
        //
        // Concerns:
        // 1. The `parse` method correctly extracts the fill-and-align section
        //    (which is delegated to `FormatSpecificationParser`).
        //
        // 2. The `parse` method correctly extracts the width section (which
        //    is delegated to `FormatSpecificationParser`).
        //
        // 3. The `parse` method sets `noBrackets` to `true` when the
        //    remaining specification starts with `n`, and sets `isMap` to
        //    `true` when (after any leading `n`) the remaining specification
        //    starts with `m`.  Both flags may be set by specifying `nm`.
        //
        // 4. If (after processing the optional `n` and `m` letters) nothing
        //    is left in the remaining specification, the underlying
        //    formatter's `parse` method is invoked.
        //
        // 5. If (after processing the optional `n` and `m` letters) the
        //    remaining specification starts with `:`, `parse` returns without
        //    invoking the underlying formatter's `parse` method.
        //
        // 6. The `parse` method throws a `bslfmt::format_error` when the
        //    remaining specification contains any other characters (for
        //    example when `m` precedes `n`, or when a stray character is
        //    present).
        //
        // 7. After a successful call to `parse`, the parse context iterator
        //    points to the end of the specification or to the terminating
        //    `}` character.
        //
        // 8. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Using a table-based approach, specify a set of valid format
        //    specifications together with their expected `alignment`,
        //    `filler`, `noBrackets`, `isMap` values, and whether the
        //    underlying formatter's `parse` is expected to be called.  For
        //    each row, parse the specification and verify the accessor
        //    values and the mock underlying formatter's state.  (C-1..5, 8)
        //
        // 2. Using a table-based approach, specify a set of invalid format
        //    specifications.  For each row, verify that `parse` throws a
        //    `bslfmt::format_error`.  (C-6)
        //
        // 3. For a selection of specifications with and without a terminating
        //    `}`, verify that after `parse` the parse-context iterator is at
        //    either the terminating `}` or the end of the input.  (C-7)
        //
        // Testing:
        //   void parse(t_PARSE_CONTEXT *ctx, t_UNDERLYING_FORMATTER *u);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `parse`"
                            "\n===============\n");

        if (verbose) printf("\tTesting valid `char` specifications.\n");
        {
            static const struct {
                int                    d_line;         // source line
                const char            *d_spec_p;       // specification string
                char                   d_filler;       // expected filler
                int                    d_numFiller;    // expected #filler
                ParserEnums::Alignment d_alignment;    // expected alignment
                bool                   d_noBrackets;   // expected noBrackets
                bool                   d_isMap;        // expected isMap
                bool                   d_underlyingParsed;
                                                       // whether underlying's
                                                       // `parse` is called
            } DATA[] = {
              //LINE SPEC        FILL  NUM  ALIGNMENT
              //                                   NOBR   ISMAP  UPARSED
              //---- -------     ----  ---  ------------------
              //                                   -----  -----  -------
              { L_,  "",         ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, false, true    },
              { L_,  "n",        ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  false, true    },
              { L_,  "m",        ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, true,  true    },
              { L_,  "nm",       ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  true,  true    },
              { L_,  "10",       ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, false, true    },
              { L_,  "10n",      ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  false, true    },
              { L_,  "10m",      ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, true,  true    },
              { L_,  "10nm",     ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  true,  true    },
              { L_,  "*<10",     '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   false, false, true    },
              { L_,  "*<10n",    '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   true,  false, true    },
              { L_,  "*<10m",    '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   false, true,  true    },
              { L_,  "*<10nm",   '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   true,  true,  true    },
              { L_,  "*>10n",    '*',  1,  ParserEnums::e_ALIGN_RIGHT,
                                                   true,  false, true    },
              { L_,  "*^10m",    '*',  1,  ParserEnums::e_ALIGN_MIDDLE,
                                                   false, true,  true    },
              { L_,  "*<n",      '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   true,  false, true    },
              { L_,  "*<m",      '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   false, true,  true    },
              { L_,  "<10",      ' ',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   false, false, true    },
              // When the remaining specification starts with ':', `parse`
              // hands off the rest of the specification to the underlying
              // formatter's `parse` method.
              { L_,  ":",        ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, false, true    },
              { L_,  "n:",       ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  false, true    },
              { L_,  "m:",       ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   false, true,  true    },
              { L_,  "nm:",      ' ',  1,  ParserEnums::e_ALIGN_DEFAULT,
                                                   true,  true,  true    },
              { L_,  "*<10n:",   '*',  1,  ParserEnums::e_ALIGN_LEFT,
                                                   true,  false, true    },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                    LINE       = DATA[i].d_line;
                const char           *const  SPEC       = DATA[i].d_spec_p;
                const char                   FILL       = DATA[i].d_filler;
                const int                    NUM_FILL   = DATA[i].d_numFiller;
                const ParserEnums::Alignment ALIGNMENT  = DATA[i].d_alignment;
                const bool                   NO_BR      = DATA[i].d_noBrackets;
                const bool                   IS_MAP     = DATA[i].d_isMap;
                const bool                   UPARSED    =
                                                    DATA[i].d_underlyingParsed;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                      spec;
                MockUnderlyingFormatter<char> underlying;
                parseRange(&spec, &underlying, SPEC);

                ASSERTV(LINE, SPEC, FILL, *spec.filler(),
                        FILL == *spec.filler());
                ASSERTV(LINE, SPEC, NUM_FILL, spec.numFillerCharacters(),
                        NUM_FILL == spec.numFillerCharacters());
                ASSERTV(LINE, SPEC, ALIGNMENT, spec.alignment(),
                        ALIGNMENT == spec.alignment());
                ASSERTV(LINE, SPEC, NO_BR, spec.noBrackets(),
                        NO_BR == spec.noBrackets());
                ASSERTV(LINE, SPEC, IS_MAP, spec.isMap(),
                        IS_MAP == spec.isMap());
                ASSERTV(LINE, SPEC, UPARSED, underlying.d_parseCalled,
                        UPARSED == underlying.d_parseCalled);
            }
        }

        if (verbose) printf("\tTesting valid `wchar_t` specifications.\n");
        {
            static const struct {
                int                    d_line;         // source line
                const wchar_t         *d_spec_p;       // specification string
                wchar_t                d_filler;       // expected filler
                int                    d_numFiller;    // expected #filler
                ParserEnums::Alignment d_alignment;    // expected alignment
                bool                   d_noBrackets;   // expected noBrackets
                bool                   d_isMap;        // expected isMap
                bool                   d_underlyingParsed;
                                                       // whether underlying's
                                                       // `parse` is called
            } DATA[] = {
              //LINE SPEC         FILL  NUM  ALIGNMENT
              //                                    NOBR   ISMAP  UPARSED
              //---- --------     ----  ---  --------
              //                                    -----  -----  -------
              { L_,  L"",         L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    false, false, true   },
              { L_,  L"n",        L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    true,  false, true   },
              { L_,  L"m",        L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    false, true,  true   },
              { L_,  L"nm",       L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    true,  true,  true   },
              { L_,  L"*<10",     L'*', 1,  ParserEnums::e_ALIGN_LEFT,
                                                    false, false, true   },
              { L_,  L"*<10n",    L'*', 1,  ParserEnums::e_ALIGN_LEFT,
                                                    true,  false, true   },
              { L_,  L"*<10m",    L'*', 1,  ParserEnums::e_ALIGN_LEFT,
                                                    false, true,  true   },
              { L_,  L"*<10nm",   L'*', 1,  ParserEnums::e_ALIGN_LEFT,
                                                    true,  true,  true   },
              { L_,  L":",        L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    false, false, true   },
              { L_,  L"nm:",      L' ', 1,  ParserEnums::e_ALIGN_DEFAULT,
                                                    true,  true,  true   },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                    LINE      = DATA[i].d_line;
                const wchar_t        *const  SPEC      = DATA[i].d_spec_p;
                const wchar_t                FILL      = DATA[i].d_filler;
                const int                    NUM_FILL  = DATA[i].d_numFiller;
                const ParserEnums::Alignment ALIGNMENT = DATA[i].d_alignment;
                const bool                   NO_BR     = DATA[i].d_noBrackets;
                const bool                   IS_MAP    = DATA[i].d_isMap;
                const bool                   UPARSED   =
                                                    DATA[i].d_underlyingParsed;

                if (veryVerbose) { T_ P(LINE) }

                WcharSpec                        spec;
                MockUnderlyingFormatter<wchar_t> underlying;
                parseRange(&spec, &underlying, SPEC);

                ASSERTV(LINE, FILL == *spec.filler());
                ASSERTV(LINE, NUM_FILL == spec.numFillerCharacters());
                ASSERTV(LINE, ALIGNMENT == spec.alignment());
                ASSERTV(LINE, NO_BR == spec.noBrackets());
                ASSERTV(LINE, IS_MAP == spec.isMap());
                ASSERTV(LINE, UPARSED == underlying.d_parseCalled);
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tTesting invalid `char` specifications.\n");
        {
            static const struct {
                int         d_line;    // source line
                const char *d_spec_p;  // invalid specification string
            } DATA[] = {
                //LINE SPEC
                //---- --------
                { L_,  "x"      },   // unrecognized letter
                { L_,  "z"      },   // unrecognized letter
                { L_,  "N"      },   // uppercase not allowed
                { L_,  "M"      },   // uppercase not allowed
                { L_,  "nn"     },   // trailing 'n' after 'n' is invalid
                { L_,  "mm"     },   // trailing 'm' after 'm' is invalid
                { L_,  "mn"     },   // 'n' after 'm' is invalid
                { L_,  "nx"     },   // trailing garbage
                { L_,  "mx"     },   // trailing garbage
                { L_,  "nmx"    },   // trailing garbage
                { L_,  "*<10nx" },   // trailing garbage
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE = DATA[i].d_line;
                const char *const  SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                      spec;
                MockUnderlyingFormatter<char> underlying;
                VERIFY_EXCEPTION_IS_THROWN(
                                       LINE,
                                       parseRange(&spec, &underlying, SPEC));
            }
        }

        if (verbose) printf("\tTesting invalid `wchar_t` specifications.\n");
        {
            static const struct {
                int            d_line;    // source line
                const wchar_t *d_spec_p;  // invalid specification string
            } DATA[] = {
                //LINE SPEC
                //---- --------
                { L_,  L"x"      },
                { L_,  L"z"      },
                { L_,  L"mn"     },
                { L_,  L"nx"     },
                { L_,  L"*<10nx" },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE = DATA[i].d_line;
                const wchar_t *const  SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                WcharSpec                        spec;
                MockUnderlyingFormatter<wchar_t> underlying;
                VERIFY_EXCEPTION_IS_THROWN(
                                       LINE,
                                       parseRange(&spec, &underlying, SPEC));
            }
        }

#endif

        if (verbose) printf("\tTesting the `s` and `?s` range types.\n");
        {
            static const struct {
                int         d_line;      // source line
                const char *d_spec_p;    // specification string
                bool        d_isString;  // expected `isString`
                bool        d_isEscaped; // expected `isEscapedString`
                bool        d_noBrackets;// expected `noBrackets`
            } DATA[] = {
              //LINE SPEC        ISSTR  ISESC  NOBR
              //---- ----------  -----  -----  -----
              { L_,  "",         false, false, false },
              { L_,  "s",        true,  false, false },
              { L_,  "?s",       true,  true,  false },
              { L_,  "ns",       true,  false, true  },
              { L_,  "n?s",      true,  true,  true  },
              { L_,  "*<10s",    true,  false, false },
              { L_,  "*<10?s",   true,  true,  false },
              { L_,  "m",        false, false, false },
              { L_,  "n",        false, false, true  },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE   = DATA[i].d_line;
                const char *const SPEC   = DATA[i].d_spec_p;
                const bool        ISSTR  = DATA[i].d_isString;
                const bool        ISESC  = DATA[i].d_isEscaped;
                const bool        NOBR   = DATA[i].d_noBrackets;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                      spec;
                MockUnderlyingFormatter<char> underlying;
                parseRange(&spec, &underlying, SPEC);

                ASSERTV(LINE, SPEC, ISSTR, spec.isString(),
                        ISSTR == spec.isString());
                ASSERTV(LINE, SPEC, ISESC, spec.isEscapedString(),
                        ISESC == spec.isEscapedString());
                ASSERTV(LINE, SPEC, NOBR, spec.noBrackets(),
                        NOBR == spec.noBrackets());
            }

#ifdef BDE_BUILD_TARGET_EXC
            // `range-type` admits only one of `m`, `s` and `?s`, and a `?`
            // that is not followed by `s` is a stray character.
            static const struct {
                int         d_line;
                const char *d_spec_p;
            } BAD[] = {
              //LINE SPEC
              //---- --------
              { L_,  "?"      },
              { L_,  "sm"     },
              { L_,  "ms"     },
              { L_,  "s?s"    },
              { L_,  "sn"     },
              { L_,  "?sm"    },
            };
            const int NUM_BAD = sizeof(BAD) / sizeof(*BAD);

            for (int i = 0; i < NUM_BAD; ++i) {
                const int         LINE = BAD[i].d_line;
                const char *const SPEC = BAD[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                      spec;
                MockUnderlyingFormatter<char> underlying;
                VERIFY_EXCEPTION_IS_THROWN(
                                        LINE,
                                        parseRange(&spec, &underlying, SPEC));
            }
#endif
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tTesting rejection of a leading-zero width.\n");
        {
            // `width` is a `positive-integer`, and a range specification has
            // no zero-pad flag that could absorb a leading `0`.
            static const struct {
                int         d_line;
                const char *d_spec_p;
            } BAD[] = {
              //LINE SPEC
              //---- --------
              { L_,  "0"      },
              { L_,  "05"     },
              { L_,  "010"    },
              { L_,  "0n"     },
              { L_,  "*<05"   },
            };
            const int NUM_BAD = sizeof(BAD) / sizeof(*BAD);

            for (int i = 0; i < NUM_BAD; ++i) {
                const int         LINE = BAD[i].d_line;
                const char *const SPEC = BAD[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                      spec;
                MockUnderlyingFormatter<char> underlying;
                VERIFY_EXCEPTION_IS_THROWN(
                                        LINE,
                                        parseRange(&spec, &underlying, SPEC));
            }
        }
#endif

        if (verbose) printf("\tTesting parse-context iterator position.\n");
        {
            // After a successful parse, the parse context iterator must point
            // to either the closing brace '}' or to the end of the input.
            {
                const bsl::string_view          input("nm}");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                MockUnderlyingFormatter<char>   underlying;
                spec.parse(&context, &underlying);
                ASSERT(context.begin() != context.end());
                ASSERT('}' == *context.begin());
            }
            {
                const bsl::string_view          input("nm");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                MockUnderlyingFormatter<char>   underlying;
                spec.parse(&context, &underlying);
                ASSERT(context.begin() == context.end());
            }
            {
                const bsl::string_view          input("*<10nm}");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                MockUnderlyingFormatter<char>   underlying;
                spec.parse(&context, &underlying);
                ASSERT(context.begin() != context.end());
                ASSERT('}' == *context.begin());
            }
            {
                const bsl::string_view          input("");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                MockUnderlyingFormatter<char>   underlying;
                spec.parse(&context, &underlying);
                ASSERT(context.begin() == context.end());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        // 1. The `isMap` and `noBrackets` accessors of a newly constructed
        //    object return `false` and do not throw.
        //
        // 2. The `alignment`, `filler`, and `numFillerCharacters` accessors
        //    throw a `format_error` when called on a newly constructed object
        //    because the underlying `FormatSpecificationParser` is in the
        //    unparsed state.
        //
        // 3. After a successful call to `parse`, all accessors return the
        //    expected values without throwing.
        //
        // Plan:
        // 1. Create a default `RangeFormatSpecification<char>` and verify
        //    that `isMap` and `noBrackets` return `false` and the other
        //    accessors throw.  (C-1..2)
        //
        // 2. Repeat step 1 for `RangeFormatSpecification<wchar_t>`.  (C-1..2)
        //
        // 3. Parse a non-empty specification and verify that all accessors
        //    return the expected values.  (C-3)
        //
        // Testing:
        //   Alignment    alignment()           const;
        //   const t_CHAR *filler()             const;
        //   bool         isMap()               const;
        //   bool         noBrackets()          const;
        //   int          numFillerCharacters() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        if (verbose) printf("\tTesting accessors on default `char` object.\n");
        {
            CharSpec mXc;
            ASSERTV(mXc.isMap(),      false == mXc.isMap()     );
            ASSERTV(mXc.noBrackets(), false == mXc.noBrackets());

#ifdef BDE_BUILD_TARGET_EXC
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.filler()             );
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.numFillerCharacters());
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.alignment()          );
#endif
        }

        if (verbose) printf(
                        "\tTesting accessors on default `wchar_t` object.\n");
        {
            WcharSpec mXw;
            ASSERTV(mXw.isMap(),      false == mXw.isMap()     );
            ASSERTV(mXw.noBrackets(), false == mXw.noBrackets());

#ifdef BDE_BUILD_TARGET_EXC
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.filler()             );
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.numFillerCharacters());
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alignment()          );
#endif
        }

        if (verbose) printf("\tTesting accessors after `char` parse.\n");
        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "*<10nm");
            ASSERTV(*spec.filler(),               '*' == *spec.filler());
            ASSERTV(spec.numFillerCharacters(),
                    1 == spec.numFillerCharacters());
            ASSERTV(spec.alignment(),
                    ParserEnums::e_ALIGN_LEFT == spec.alignment());
            ASSERTV(spec.noBrackets(), true  == spec.noBrackets());
            ASSERTV(spec.isMap(),      true  == spec.isMap()     );
            ASSERT(true == underlying.d_parseCalled);
        }

        if (verbose) printf("\tTesting accessors after `wchar_t` parse.\n");
        {
            WcharSpec                        spec;
            MockUnderlyingFormatter<wchar_t> underlying;
            parseRange(&spec, &underlying, L"*<10n");
            ASSERTV(L'*' == *spec.filler());
            ASSERTV(1 == spec.numFillerCharacters());
            ASSERTV(ParserEnums::e_ALIGN_LEFT == spec.alignment());
            ASSERTV(true  == spec.noBrackets());
            ASSERTV(false == spec.isMap()     );
            ASSERT(true == underlying.d_parseCalled);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR (DEFAULT CONSTRUCTOR)
        //   Under the practice accepted in BDE, this test case verifies that
        //   the default constructor brings the object into a well-defined
        //   state that can be probed by the (untested at this point)
        //   `isMap` and `noBrackets` accessors.  The other accessors delegate
        //   to the underlying `FormatSpecificationParser` object, which
        //   throws when accessed before `parse` has been called; that
        //   behavior is verified in test case 3.
        //
        // Concerns:
        // 1. The default constructor creates an object whose `isMap` and
        //    `noBrackets` accessors return `false`.
        //
        // Plan:
        // 1. Default-construct a `RangeFormatSpecification<char>` and
        //    `RangeFormatSpecification<wchar_t>` and verify that `isMap` and
        //    `noBrackets` return `false` for each.  (C-1)
        //
        // Testing:
        //   RangeFormatSpecification();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATOR (DEFAULT CONSTRUCTOR)"
                            "\n=========================================\n");

        CharSpec  mXc;
        ASSERTV(mXc.isMap(),      false == mXc.isMap()     );
        ASSERTV(mXc.noBrackets(), false == mXc.noBrackets());

        WcharSpec mXw;
        ASSERTV(mXw.isMap(),      false == mXw.isMap()     );
        ASSERTV(mXw.noBrackets(), false == mXw.noBrackets());
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
        // 1. Default construct an object and exercise the `isMap` and
        //    `noBrackets` accessors.
        //
        // 2. Parse a few simple format specifications and verify the
        //    resulting accessor values.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            CharSpec spec;
            ASSERT(false == spec.isMap()     );
            ASSERT(false == spec.noBrackets());
        }

        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "");
            ASSERT(false                        == spec.isMap()     );
            ASSERT(false                        == spec.noBrackets());
            ASSERT(ParserEnums::e_ALIGN_DEFAULT == spec.alignment() );
            ASSERT(' '                          == *spec.filler()   );
            ASSERT(1                            == spec.numFillerCharacters());
            ASSERT(true                         == underlying.d_parseCalled);
        }

        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "n");
            ASSERT(false == spec.isMap()     );
            ASSERT(true  == spec.noBrackets());
            ASSERT(true  == underlying.d_parseCalled);
        }

        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "m");
            ASSERT(true  == spec.isMap()     );
            ASSERT(false == spec.noBrackets());
            ASSERT(true  == underlying.d_parseCalled);
        }

        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "nm");
            ASSERT(true  == spec.isMap()     );
            ASSERT(true  == spec.noBrackets());
            ASSERT(true  == underlying.d_parseCalled);
        }

        {
            CharSpec                      spec;
            MockUnderlyingFormatter<char> underlying;
            parseRange(&spec, &underlying, "*<10n");
            ASSERT('*'                        == *spec.filler());
            ASSERT(ParserEnums::e_ALIGN_LEFT  == spec.alignment());
            ASSERT(true                       == spec.noBrackets());
            ASSERT(false                      == spec.isMap()     );
            ASSERT(true                       == underlying.d_parseCalled);
        }

        {
            WcharSpec                        spec;
            MockUnderlyingFormatter<wchar_t> underlying;
            parseRange(&spec, &underlying, L"*<10m");
            ASSERT(L'*'                       == *spec.filler());
            ASSERT(ParserEnums::e_ALIGN_LEFT  == spec.alignment());
            ASSERT(false                      == spec.noBrackets());
            ASSERT(true                       == spec.isMap()     );
            ASSERT(true                       == underlying.d_parseCalled);
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
