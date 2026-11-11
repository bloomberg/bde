// bslfmt_tupleformatspecification.t.cpp                              -*-C++-*-
#include <bslfmt_tupleformatspecification.h>

#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

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
// parses the format specification for `bsl::pair` and `bsl::tuple`.  The
// class's `parse` method delegates its work to a `FormatSpecificationParser`
// (which is tested separately) and then interprets the single-character
// remaining specification as a tuple format type (`n`, `m`, or none).
//
// According to the BDE practice, the constructor and manipulators are tested
// first (test case 2), followed by the basic accessors (test case 3).  Test
// case 4 provides a table-driven exhaustive test of the `parse` method,
// including negative testing of invalid specifications.  Test case 5
// exercises the `postprocess` manipulator and the `postprocessedWidth`
// accessor.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TupleFormatSpecification();
//
// MANIPULATORS
// [ 4] void parse(t_PARSE_CONTEXT *ctx);
// [ 5] void postprocess(const t_FORMAT_CONTEXT& context);
//
// ACCESSORS
// [ 3] Alignment    alignment()           const;
// [ 3] const t_CHAR *filler()             const;
// [ 3] int          numFillerCharacters() const;
// [ 3] TupleType    formatType()          const;
// [ 5] FormatterSpecificationNumericValue postprocessedWidth() const;
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
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

typedef TupleFormatSpecification<char>       CharSpec;
typedef TupleFormatSpecification<wchar_t>    WcharSpec;
typedef TupleFormatSpecificationEnums        Enums;
typedef FormatSpecificationParserEnums       ParserEnums;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Parse the specified `input` as a `char` tuple format specification and
/// return the resulting `TupleFormatSpecification<char>` object.
CharSpec parseTuple(const bsl::string_view& input)
{
    CharSpec                       spec;
    bslfmt::MockParseContext<char> context(input, 0);

    spec.parse(&context);

    return spec;
}

/// Parse the specified `input` as a `wchar_t` tuple format specification and
/// return the resulting `TupleFormatSpecification<wchar_t>` object.
WcharSpec parseTuple(const bsl::wstring_view& input)
{
    WcharSpec                         spec;
    bslfmt::MockParseContext<wchar_t> context(input, 0);

    spec.parse(&context);

    return spec;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Parsing A Tuple Format Specification
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing a formatter for a pair and need to know which
// brackets and separator the caller has asked for.
//
// First, we parse a format string into a specification:
// ```
    void run()
    {
        typedef bslfmt::TupleFormatSpecification<char> Specification;

        bslfmt::MockParseContext<char> context("*<10m}", 0);

        Specification spec;
        spec.parse(&context);
// ```
// Finally, we verify the parsed specification:
// ```
        ASSERT(Specification::e_TUPLE_PAIR_MAP == spec.formatType());
        ASSERT('*'                             == *spec.filler());
        ASSERT(Specification::e_ALIGN_LEFT     == spec.alignment());
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

    // CONCERN: No global memory is allocated after `main` starts.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
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
        // TESTING `postprocess` / `postprocessedWidth`
        //
        // Concerns:
        // 1. `postprocessedWidth` throws a `format_error` when called on an
        //    object on which `postprocess` has not previously been called.
        //
        // 2. After `parse` succeeds on a specification with no width and
        //    `postprocess` is then invoked, `postprocessedWidth` returns a
        //    `FormatterSpecificationNumericValue` with category `e_DEFAULT`.
        //
        // 3. After `parse` succeeds on a specification with a literal width
        //    and `postprocess` is then invoked, `postprocessedWidth` returns
        //    a `FormatterSpecificationNumericValue` with category `e_VALUE`
        //    and the value equal to the parsed literal width.
        //
        // 4. After `parse` succeeds on a specification with a nested width
        //    (using either an explicit or an implicit argument index) and
        //    `postprocess` is invoked with a format context that supplies an
        //    integral argument at that index, `postprocessedWidth` returns
        //    a `FormatterSpecificationNumericValue` with category `e_VALUE`
        //    and the value equal to that argument.
        //
        // 5. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Default-construct a `TupleFormatSpecification` and verify that
        //    `postprocessedWidth` throws.  (C-1)
        //
        // 2. For each of the specifications `""`, `"n"`, `"m"`, `"10"`,
        //    `"*<10"`, and `"*<10m"`, parse the specification, invoke
        //    `postprocess` on a dummy `MockFormatContext`, and verify the
        //    returned category and value from `postprocessedWidth`.
        //    (C-2, 3)
        //
        // 3. For each of the specifications `"{}"` and `"{0}"`, parse the
        //    specification with `numArgs = 1`, invoke `postprocess` on a
        //    `MockFormatContext<char>` whose first argument is an integral
        //    value `V`, and verify that `postprocessedWidth` returns
        //    `e_VALUE, V`.  (C-4)
        //
        // 4. Repeat step 3 for `wchar_t`.  (C-5)
        //
        // Testing:
        //   void postprocess(const t_FORMAT_CONTEXT& context);
        //   FormatterSpecificationNumericValue postprocessedWidth() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `postprocess` / `postprocessedWidth`"
                            "\n===========================================\n");

        typedef FormatterSpecificationNumericValue NumericValue;

        if (verbose) printf(
                        "\t`postprocessedWidth` on default object throws.\n");
        {
#ifdef BDE_BUILD_TARGET_EXC
            CharSpec mXc;
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedWidth());

            WcharSpec mXw;
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedWidth());
#endif
        }

        if (verbose) printf("\tLiteral / absent width, `char`.\n");
        {
            static const struct {
                int         d_line;
                const char *d_spec_p;
                int         d_category;  // `NumericValue::Category`
                int         d_value;
            } DATA[] = {
                //LINE SPEC     CATEGORY                  VALUE
                //---- -------  ------------------------  -----
                { L_,  "",      NumericValue::e_DEFAULT,  0     },
                { L_,  "n",     NumericValue::e_DEFAULT,  0     },
                { L_,  "m",     NumericValue::e_DEFAULT,  0     },
                { L_,  "10",    NumericValue::e_VALUE,    10    },
                { L_,  "*<10",  NumericValue::e_VALUE,    10    },
                { L_,  "*<10m", NumericValue::e_VALUE,    10    },
                { L_,  "*>7n",  NumericValue::e_VALUE,    7     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int              LINE     = DATA[i].d_line;
                const char *const      SPEC     = DATA[i].d_spec_p;
                const int              CATEGORY = DATA[i].d_category;
                const int              VALUE    = DATA[i].d_value;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec                   spec = parseTuple(SPEC);
                bslfmt::MockFormatContext<char> ctx(0);
                spec.postprocess(ctx);

                const NumericValue width = spec.postprocessedWidth();
                ASSERTV(LINE, SPEC, CATEGORY, width.category(),
                        CATEGORY == static_cast<int>(width.category()));
                if (width.category() == NumericValue::e_VALUE) {
                    ASSERTV(LINE, SPEC, VALUE, width.value(),
                            VALUE == width.value());
                }
            }
        }

        if (verbose) printf("\tLiteral / absent width, `wchar_t`.\n");
        {
            static const struct {
                int            d_line;
                const wchar_t *d_spec_p;
                int            d_category;
                int            d_value;
            } DATA[] = {
                { L_,  L"",      NumericValue::e_DEFAULT,  0  },
                { L_,  L"10",    NumericValue::e_VALUE,    10 },
                { L_,  L"*<10m", NumericValue::e_VALUE,    10 },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE     = DATA[i].d_line;
                const wchar_t *const  SPEC     = DATA[i].d_spec_p;
                const int             CATEGORY = DATA[i].d_category;
                const int             VALUE    = DATA[i].d_value;

                if (veryVerbose) { T_ P(LINE) }

                WcharSpec                     spec = parseTuple(SPEC);
                bslfmt::MockFormatContext<wchar_t> ctx(0);
                spec.postprocess(ctx);

                const NumericValue width = spec.postprocessedWidth();
                ASSERTV(LINE, CATEGORY, width.category(),
                        CATEGORY == static_cast<int>(width.category()));
                if (width.category() == NumericValue::e_VALUE) {
                    ASSERTV(LINE, VALUE, width.value(),
                            VALUE == width.value());
                }
            }
        }

        if (verbose) printf("\tNested width from argument, `char`.\n");
        {
            // Implicit argument index: `{}` refers to the next positional
            // argument (index 0 in this test).
            {
                CharSpec                       spec;
                bslfmt::MockParseContext<char> pctx("{}", 1);
                spec.parse(&pctx);
                bslfmt::MockFormatContext<char> fctx(17);
                spec.postprocess(fctx);
                const NumericValue width = spec.postprocessedWidth();
                ASSERTV(width.category(),
                        NumericValue::e_VALUE == width.category());
                ASSERTV(width.value(), 17 == width.value());
            }

            // Explicit argument index: `{0}` refers to argument 0.
            {
                CharSpec                       spec;
                bslfmt::MockParseContext<char> pctx("{0}", 1);
                spec.parse(&pctx);
                bslfmt::MockFormatContext<char> fctx(42);
                spec.postprocess(fctx);
                const NumericValue width = spec.postprocessedWidth();
                ASSERTV(width.category(),
                        NumericValue::e_VALUE == width.category());
                ASSERTV(width.value(), 42 == width.value());
            }
        }

        if (verbose) printf("\tNested width from argument, `wchar_t`.\n");
        {
            WcharSpec                         spec;
            bslfmt::MockParseContext<wchar_t> pctx(L"{0}", 1);
            spec.parse(&pctx);
            bslfmt::MockFormatContext<wchar_t> fctx(23);
            spec.postprocess(fctx);
            const NumericValue width = spec.postprocessedWidth();
            ASSERTV(width.category(),
                    NumericValue::e_VALUE == width.category());
            ASSERTV(width.value(), 23 == width.value());
        }
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
        // 3. The `parse` method correctly identifies the tuple format type
        //    (`n` for `e_TUPLE_NO_BRACKETS` and `m` for `e_TUPLE_PAIR_MAP`)
        //    and leaves it as `e_TUPLE_DEFAULT` when no type is specified.
        //
        // 4. The `parse` method throws a `bslfmt::format_error` when the
        //    remaining specification is longer than one character or when the
        //    single-character type is not `n` or `m`.
        //
        // 5. After a successful call to `parse`, the parse context iterator
        //    points to the end of the specification or to the terminating
        //    `}` character.
        //
        // 6. The above behavior is the same for `char` and `wchar_t`.
        //
        // Plan:
        // 1. Using a table-based approach, specify a set of valid format
        //    specifications together with their expected `alignment`,
        //    `filler`, and `formatType` values.  For each row, parse the
        //    specification and verify the accessor values.  (C-1..3, 6)
        //
        // 2. Using a table-based approach, specify a set of invalid format
        //    specifications.  For each row, verify that `parse` throws a
        //    `bslfmt::format_error`.  (C-4)
        //
        // 3. For a selection of specifications with and without a terminating
        //    `}`, verify that after `parse` the parse-context iterator is at
        //    either the terminating `}` or the end of the input.  (C-5)
        //
        // Testing:
        //   void parse(t_PARSE_CONTEXT *ctx);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `parse`"
                            "\n===============\n");

        if (verbose) printf("\tTesting valid `char` specifications.\n");
        {
            static const struct {
                int                  d_line;       // source line
                const char          *d_spec_p;     // specification string
                char                 d_filler;     // expected filler
                int                  d_numFiller;  // expected #filler chars
                ParserEnums::Alignment
                                     d_alignment;  // expected alignment
                Enums::TupleType     d_type;       // expected tuple type
            } DATA[] = {
              //LINE SPEC       FILL   NUM  ALIGNMENT
              //                                   TYPE
              //---- ---------- ---    ---  --------
              //                                 -----------------------
              { L_,  "",        ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "n",       ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_NO_BRACKETS  },
              { L_,  "m",       ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_PAIR_MAP     },
              { L_,  "10",      ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "10n",     ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_NO_BRACKETS  },
              { L_,  "10m",     ' ',    1,  ParserEnums::e_ALIGN_DEFAULT,
                                                 Enums::e_TUPLE_PAIR_MAP     },
              { L_,  "*<10",    '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "*<10n",   '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_NO_BRACKETS  },
              { L_,  "*<10m",   '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_PAIR_MAP     },
              { L_,  "*>10",    '*',    1,  ParserEnums::e_ALIGN_RIGHT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "*^10",    '*',    1,  ParserEnums::e_ALIGN_MIDDLE,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "*<",      '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "*<n",     '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_NO_BRACKETS  },
              { L_,  "*<m",     '*',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_PAIR_MAP     },
              { L_,  "<10",     ' ',    1,  ParserEnums::e_ALIGN_LEFT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  ">10",     ' ',    1,  ParserEnums::e_ALIGN_RIGHT,
                                                 Enums::e_TUPLE_DEFAULT      },
              { L_,  "^10",     ' ',    1,  ParserEnums::e_ALIGN_MIDDLE,
                                                 Enums::e_TUPLE_DEFAULT      },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                    LINE       = DATA[i].d_line;
                const char           *const  SPEC       = DATA[i].d_spec_p;
                const char                   FILL       = DATA[i].d_filler;
                const int                    NUM_FILL   = DATA[i].d_numFiller;
                const ParserEnums::Alignment ALIGNMENT  = DATA[i].d_alignment;
                const Enums::TupleType       TYPE       = DATA[i].d_type;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                CharSpec spec = parseTuple(SPEC);

                ASSERTV(LINE, SPEC, FILL, *spec.filler(),
                        FILL == *spec.filler());
                ASSERTV(LINE, SPEC, NUM_FILL, spec.numFillerCharacters(),
                        NUM_FILL == spec.numFillerCharacters());
                ASSERTV(LINE, SPEC, ALIGNMENT, spec.alignment(),
                        ALIGNMENT == spec.alignment());
                ASSERTV(LINE, SPEC, TYPE, spec.formatType(),
                        TYPE == spec.formatType());
            }
        }

        if (verbose) printf("\tTesting valid `wchar_t` specifications.\n");
        {
            static const struct {
                int                  d_line;       // source line
                const wchar_t       *d_spec_p;     // specification string
                wchar_t              d_filler;     // expected filler
                int                  d_numFiller;  // expected #filler chars
                ParserEnums::Alignment
                                     d_alignment;  // expected alignment
                Enums::TupleType     d_type;       // expected tuple type
            } DATA[] = {
              //LINE SPEC        FILL   NUM  ALIGNMENT
              //                                    TYPE
              //---- ----------- ---    ---  --------
              //                                  -----------------------
              { L_,  L"",        L' ',   1,  ParserEnums::e_ALIGN_DEFAULT,
                                                  Enums::e_TUPLE_DEFAULT     },
              { L_,  L"n",       L' ',   1,  ParserEnums::e_ALIGN_DEFAULT,
                                                  Enums::e_TUPLE_NO_BRACKETS },
              { L_,  L"m",       L' ',   1,  ParserEnums::e_ALIGN_DEFAULT,
                                                  Enums::e_TUPLE_PAIR_MAP    },
              { L_,  L"*<10",    L'*',   1,  ParserEnums::e_ALIGN_LEFT,
                                                  Enums::e_TUPLE_DEFAULT     },
              { L_,  L"*<10n",   L'*',   1,  ParserEnums::e_ALIGN_LEFT,
                                                  Enums::e_TUPLE_NO_BRACKETS },
              { L_,  L"*<10m",   L'*',   1,  ParserEnums::e_ALIGN_LEFT,
                                                  Enums::e_TUPLE_PAIR_MAP    },
              { L_,  L"*>10n",   L'*',   1,  ParserEnums::e_ALIGN_RIGHT,
                                                  Enums::e_TUPLE_NO_BRACKETS },
              { L_,  L"*^10m",   L'*',   1,  ParserEnums::e_ALIGN_MIDDLE,
                                                  Enums::e_TUPLE_PAIR_MAP    },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                    LINE      = DATA[i].d_line;
                const wchar_t        *const  SPEC      = DATA[i].d_spec_p;
                const wchar_t                FILL      = DATA[i].d_filler;
                const int                    NUM_FILL  = DATA[i].d_numFiller;
                const ParserEnums::Alignment ALIGNMENT = DATA[i].d_alignment;
                const Enums::TupleType       TYPE      = DATA[i].d_type;

                if (veryVerbose) { T_ P(LINE) }

                WcharSpec spec = parseTuple(SPEC);

                ASSERTV(LINE, FILL == *spec.filler());
                ASSERTV(LINE, NUM_FILL == spec.numFillerCharacters());
                ASSERTV(LINE, ALIGNMENT == spec.alignment());
                ASSERTV(LINE, TYPE == spec.formatType());
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
                { L_,  "x"      },   // unrecognized type letter
                { L_,  "z"      },   // unrecognized type letter
                { L_,  "N"      },   // uppercase not allowed
                { L_,  "M"      },   // uppercase not allowed
                { L_,  "nx"     },   // trailing garbage after type
                { L_,  "10nx"   },   // trailing garbage after type
                { L_,  "*<10nx" },   // trailing garbage after type
                { L_,  "ab"     },   // multi-character remaining spec
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int          LINE = DATA[i].d_line;
                const char *const  SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P_(LINE) P(SPEC) }

                VERIFY_EXCEPTION_IS_THROWN(LINE, parseTuple(SPEC));
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
                { L_,  L"nx"     },
                { L_,  L"*<10nx" },
                { L_,  L"ab"     },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int             LINE = DATA[i].d_line;
                const wchar_t *const  SPEC = DATA[i].d_spec_p;

                if (veryVerbose) { T_ P(LINE) }

                VERIFY_EXCEPTION_IS_THROWN(LINE, parseTuple(SPEC));
            }
        }
#endif

        if (verbose) printf("\tTesting parse-context iterator position.\n");
        {
            // After a successful parse, the parse context iterator must point
            // to either the closing brace '}' or to the end of the input.
            {
                const bsl::string_view          input("m}");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                spec.parse(&context);
                ASSERT(context.begin() != context.end());
                ASSERT('}' == *context.begin());
            }
            {
                const bsl::string_view          input("m");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                spec.parse(&context);
                ASSERT(context.begin() == context.end());
            }
            {
                const bsl::string_view          input("*<10n}");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                spec.parse(&context);
                ASSERT(context.begin() != context.end());
                ASSERT('}' == *context.begin());
            }
            {
                const bsl::string_view          input("");
                bslfmt::MockParseContext<char>  context(input, 0);
                CharSpec                        spec;
                spec.parse(&context);
                ASSERT(context.begin() == context.end());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        // 1. The `formatType` accessor of a newly constructed object returns
        //    `e_TUPLE_DEFAULT` and does not throw.
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
        // 1. Create a default `TupleFormatSpecification<char>` and verify
        //    that `formatType` returns `e_TUPLE_DEFAULT` and the other
        //    accessors throw.  (C-1..2)
        //
        // 2. Repeat step 1 for `TupleFormatSpecification<wchar_t>`.  (C-1..2)
        //
        // 3. Parse a non-empty specification and verify that all accessors
        //    return the expected values.  (C-3)
        //
        // Testing:
        //   Alignment    alignment()           const;
        //   const t_CHAR *filler()             const;
        //   int          numFillerCharacters() const;
        //   TupleType    formatType()          const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        if (verbose) printf("\tTesting accessors on default `char` object.\n");
        {
            CharSpec mXc;
            ASSERTV(mXc.formatType(),
                    Enums::e_TUPLE_DEFAULT == mXc.formatType());

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
            ASSERTV(mXw.formatType(),
                    Enums::e_TUPLE_DEFAULT == mXw.formatType());

#ifdef BDE_BUILD_TARGET_EXC
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.filler()             );
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.numFillerCharacters());
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alignment()          );
#endif
        }

        if (verbose) printf("\tTesting accessors after `char` parse.\n");
        {
            CharSpec spec = parseTuple("*<10n");
            ASSERTV(*spec.filler(),               '*' == *spec.filler());
            ASSERTV(spec.numFillerCharacters(),
                    1 == spec.numFillerCharacters());
            ASSERTV(spec.alignment(),
                    ParserEnums::e_ALIGN_LEFT == spec.alignment());
            ASSERTV(spec.formatType(),
                    Enums::e_TUPLE_NO_BRACKETS == spec.formatType());
        }

        if (verbose) printf("\tTesting accessors after `wchar_t` parse.\n");
        {
            WcharSpec spec = parseTuple(L"*<10m");
            ASSERTV(L'*' == *spec.filler());
            ASSERTV(1 == spec.numFillerCharacters());
            ASSERTV(ParserEnums::e_ALIGN_LEFT == spec.alignment());
            ASSERTV(Enums::e_TUPLE_PAIR_MAP == spec.formatType());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR (DEFAULT CONSTRUCTOR)
        //   This test case verifies that the default constructor brings the
        //   object into a well-defined state that can be probed by the
        //   (untested) `formatType` accessor.  The other accessors delegate to
        //   the underlying `FormatSpecificationParser` object, which throws
        //   when accessed before `parse` has been called; that behavior is
        //   verified in test case 3.
        //
        // Concerns:
        // 1. The default constructor creates an object whose `formatType` is
        //    `e_TUPLE_DEFAULT`.
        //
        // Plan:
        // 1. Default-construct a `TupleFormatSpecification<char>` and
        //    `TupleFormatSpecification<wchar_t>` and verify that
        //    `formatType` returns `e_TUPLE_DEFAULT` for each.  (C-1)
        //
        // Testing:
        //   TupleFormatSpecification();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATOR (DEFAULT CONSTRUCTOR)"
                            "\n=========================================\n");

        CharSpec  mXc;
        ASSERTV(mXc.formatType(), Enums::e_TUPLE_DEFAULT == mXc.formatType());

        WcharSpec mXw;
        ASSERTV(mXw.formatType(), Enums::e_TUPLE_DEFAULT == mXw.formatType());
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
        // 1. Default construct an object and exercise the `formatType`
        //    accessor.
        //
        // 2. Parse a simple format specification and verify the accessor
        //    values.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            CharSpec spec;
            ASSERT(Enums::e_TUPLE_DEFAULT == spec.formatType());
        }

        {
            CharSpec spec = parseTuple("");
            ASSERT(Enums::e_TUPLE_DEFAULT       == spec.formatType());
            ASSERT(ParserEnums::e_ALIGN_DEFAULT == spec.alignment());
            ASSERT(' '                          == *spec.filler());
            ASSERT(1                            == spec.numFillerCharacters());
        }

        {
            CharSpec spec = parseTuple("n");
            ASSERT(Enums::e_TUPLE_NO_BRACKETS == spec.formatType());
        }

        {
            CharSpec spec = parseTuple("m");
            ASSERT(Enums::e_TUPLE_PAIR_MAP == spec.formatType());
        }

        {
            CharSpec spec = parseTuple("*<10n");
            ASSERT('*'                        == *spec.filler());
            ASSERT(ParserEnums::e_ALIGN_LEFT  == spec.alignment());
            ASSERT(Enums::e_TUPLE_NO_BRACKETS == spec.formatType());
        }

        {
            WcharSpec spec = parseTuple(L"*<10m");
            ASSERT(L'*'                       == *spec.filler());
            ASSERT(ParserEnums::e_ALIGN_LEFT  == spec.alignment());
            ASSERT(Enums::e_TUPLE_PAIR_MAP    == spec.formatType());
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

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
