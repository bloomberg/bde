// bslfmt_formatspecificationparser.t.cpp                             -*-C++-*-
#include <bslfmt_formatspecificationparser.h>

#include <bslfmt_format_string.h>
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslalg_numericformatterutil.h>  // `toChar` `ToCharsMaxLength`

#include <bslma_managedptr.h>  // `TestSpecificationGenerator` implementation

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>
#include <bslstl_vector.h>  // `TestSpecificationGenerator` implementation

#include <stdio.h>
#include <string.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>  // TestSpecificationGenerator` oracle tests
#endif

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single value-semantic attribute class.
// According to the practice accepted in the BDE, in the second test case
// (Primary Manipulators) we should test a set of functions sufficient to bring
// the object to any possible state.  In this case, we would be forced to test
// the constructor and all manipulators in one place.  But for readability,
// each function is tested in a separate test case.
// The second problem is that almost all accessors except one throw exceptions
// if they are called for a newly created object.  So in the forth test case
// (Basic Accessors) we have to use (yet untested) `parse` and `postprocess`
// manipulators to modify object from the default state.
// Third test case contains tests for the test specification generator.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bslfmt::FormatSpecificationParser();
//
// MANIPULATORS
// [5] void parse(t_PARSE_CONTEXT *parseContext, Sections sections);
// [6] void postprocess(const t_FORMAT_CONTEXT& context);
//
// ACCESSORS
// [4] const t_CHAR *filler() const;
// [4] int numFillerCharacters() const;
// [4] int fillerCodePointDisplayWidth() const;
// [4] Alignment alignment() const;
// [4] Sign sign() const;
// [4] bool alternativeFlag() const;
// [4] bool zeroPaddingFlag() const;
// [4] const NumericValue postprocessedWidth() const;
// [4] const NumericValue postprocessedPrecision() const;
// [4] const NumericValue rawWidth() const;
// [4] const NumericValue rawPrecision() const;
// [4] bool localeSpecificFlag() const;
// [4] ProcessingState processingState() const;
// [4] const bsl::basic_string_view<t_CHAR> remainingSpec() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST SPECIFICATION GENERATOR

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

// ============================================================================
//                      ADDITIONAL TEST MACROS
// ----------------------------------------------------------------------------

#define VERIFY_EXCEPTION_IS_THROWN(action)                                    \
try {                                                                         \
    action                                                                    \
    ASSERTV("Exception hasn't been thrown", false);                           \
}                                                                             \
catch (const bsl::format_error&) {                                            \
}

// ============================================================================
//                  ASSISTANCE TYPES AND FUNCTIONS
// ----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string<>
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif

namespace {

typedef FormatSpecificationParser<char>    CharParser;
typedef FormatSpecificationParser<wchar_t> WcharParser;
typedef FormatterSpecificationNumericValue NumericValue;
typedef FormatSpecificationParserEnums     Enums;

BSLFMT_FORMATTER_TEST_CONSTEVAL CharParser parseStandard(
                             BSLFMT_FORMAT_STRING_PARAMETER inputSpecification)
{
    const Enums::Sections sect = static_cast<Enums::Sections>(
                                   Enums::e_SECTIONS_FILL_ALIGN |
                                   Enums::e_SECTIONS_SIGN_FLAG |
                                   Enums::e_SECTIONS_ALTERNATE_FLAG |
                                   Enums::e_SECTIONS_ZERO_PAD_FLAG |
                                   Enums::e_SECTIONS_WIDTH |
                                   Enums::e_SECTIONS_PRECISION |
                                   Enums::e_SECTIONS_LOCALE_FLAG |
                                   Enums::e_SECTIONS_REMAINING_SPEC);

    CharParser                     parser;
    bsl::basic_string_view<char>   input(inputSpecification.get());
    bslfmt::MockParseContext<char> context(input, 3);

    parser.parse(&context, sect);

    return parser;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL WcharParser parseStandard(
                            BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification)
{
    const WcharParser::Sections sect = static_cast<WcharParser::Sections>(
                                       WcharParser::e_SECTIONS_FILL_ALIGN |
                                       WcharParser::e_SECTIONS_SIGN_FLAG |
                                       WcharParser::e_SECTIONS_ALTERNATE_FLAG |
                                       WcharParser::e_SECTIONS_ZERO_PAD_FLAG |
                                       WcharParser::e_SECTIONS_WIDTH |
                                       WcharParser::e_SECTIONS_PRECISION |
                                       WcharParser::e_SECTIONS_LOCALE_FLAG |
                                       WcharParser::e_SECTIONS_REMAINING_SPEC);

    WcharParser                       parser;
    bsl::basic_string_view<wchar_t>   input(inputSpecification.get());
    bslfmt::MockParseContext<wchar_t> context(input, 3);

    parser.parse(&context, sect);

    return parser;
}

void checkStandard(int                   line,
                   const CharParser&     originalParser,
                   bsl::string_view      filler,
                   CharParser::Alignment alignment,
                   CharParser::Sign      sign,
                   bool                  alternativeFlag,
                   bool                  zeroPaddingFlag,
                   NumericValue          rawWidth,
                   NumericValue          postprocessedWidth,
                   NumericValue          rawPrecision,
                   NumericValue          postprocessedPrecision,
                   bool                  localeSpecificFlag,
                   bsl::string_view      remainingSpec)
{
    CharParser parser = originalParser;

    bslfmt::MockFormatContext<char> context(99, 98, 97);
    parser.postprocess(context);

    ASSERTV(line, parser.filler(), parser.numFillerCharacters(),
                  filler                 == bsl::string_view(
                                                parser.filler(),
                                                parser.numFillerCharacters()));
    ASSERTV(line, alignment              == parser.alignment());
    ASSERTV(line, sign                   == parser.sign());
    ASSERTV(line, alternativeFlag        == parser.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == parser.zeroPaddingFlag());
    ASSERTV(line, rawWidth               == parser.rawWidth());
    ASSERTV(line, rawPrecision           == parser.rawPrecision());
    ASSERTV(line, postprocessedWidth     == parser.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == parser.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == parser.localeSpecificFlag());
    ASSERTV(line, remainingSpec          == parser.remainingSpec());
}

void checkStandard(int                    line,
                   const WcharParser&     originalParser,
                   bsl::wstring_view      filler,
                   WcharParser::Alignment alignment,
                   WcharParser::Sign      sign,
                   bool                   alternativeFlag,
                   bool                   zeroPaddingFlag,
                   NumericValue           rawWidth,
                   NumericValue           postprocessedWidth,
                   NumericValue           rawPrecision,
                   NumericValue           postprocessedPrecision,
                   bool                   localeSpecificFlag,
                   bsl::wstring_view      remainingSpec)
{
    WcharParser parser = originalParser;

    bslfmt::MockFormatContext<wchar_t> context(99, 98, 97);
    parser.postprocess(context);

    ASSERTV(line, parser.filler(), parser.numFillerCharacters(),
                  filler                 == bsl::wstring_view(
                                                parser.filler(),
                                                parser.numFillerCharacters()));
    ASSERTV(line, alignment              == parser.alignment());
    ASSERTV(line, sign                   == parser.sign());
    ASSERTV(line, alternativeFlag        == parser.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == parser.zeroPaddingFlag());
    ASSERTV(line, rawWidth               == parser.rawWidth());
    ASSERTV(line, rawPrecision           == parser.rawPrecision());
    ASSERTV(line, postprocessedWidth     == parser.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == parser.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == parser.localeSpecificFlag());
    ASSERTV(line, remainingSpec          == parser.remainingSpec());
}

}  // close unnamed namespace

                 // =======================================
                 // class TestSpecificationGenerator_Option
                 // =======================================

/// This class is a base class for `TestSpecificationGenerator` option nodes,
/// which, when compiled into a list, represent the state of the generator.
/// The payload and the process of changing this state are programmed into
/// specific implementations of this interface.  This class contains the basic
/// functions of adding a new link to the options chain and finding the last
/// link in the chain.
class TestSpecificationGenerator_Option {
    // DATA
    bslma::ManagedPtr<TestSpecificationGenerator_Option> d_nextOption;
                                                             // pointer to the
                                                             // next option in
                                                             // the list

  public:
    // CREATORS

    /// Create an object, having no successor in the list of generator options.
    TestSpecificationGenerator_Option()
    : d_nextOption(0)
    {}

    /// Destroy this object.
    virtual ~TestSpecificationGenerator_Option()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    virtual bool nextState() = 0;

    /// Set the specified `nextOption` as a successor in the list of generator
    /// options.  Use the specified `allocator` to supply memory.
    void setNextOption(TestSpecificationGenerator_Option *nextOption,
                       bslma::Allocator                  *allocator)
    {
        if (d_nextOption) {
            d_nextOption->setNextOption(nextOption, allocator);
        }
        else {
            d_nextOption.load(nextOption, allocator);
        }
    }

    /// Return the pointer to the last item in the list of generator options
    /// (this object can also be one).
    TestSpecificationGenerator_Option *lastOption()
    {
        if (d_nextOption) {
            return d_nextOption->lastOption();                        // RETURN
        }
        else {
            return this;
        }
    }

    // ACCESSORS

    /// Return the pointer to the next item in the list of generator options
    /// if such an object exists and null otherwise..
    TestSpecificationGenerator_Option *nextOption() const
    {
        return d_nextOption.get();
    }
};

             // ===========================================
             // class TestSpecificationGenerator_BoolOption
             // ===========================================

/// This class is an implementation of the `TestSpecificationGenerator_Option`
/// for options that have only two states: they are either present or not.
class TestSpecificationGenerator_BoolOption
: public TestSpecificationGenerator_Option {
    // DATA
    bool *d_isPresent_p;  // current state (held, not owned)

  public:
    // CREATORS

    /// Create an object that displays its state through the specified `value`.
    TestSpecificationGenerator_BoolOption(bool *value)
    : d_isPresent_p(value)
    {}

    /// Destroy this object.
    ~TestSpecificationGenerator_BoolOption()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    bool nextState()
    {
        if (!nextOption() || !nextOption()->nextState()) {
            *d_isPresent_p = !(*d_isPresent_p);
            return *d_isPresent_p;
        }
        return true;
    }
};

               // ============================================
               // class TestSpecificationGenerator_TypedOption
               // ============================================

/// This class is an implementation of the `TestSpecificationGenerator_Option`
/// for options that iterate through the states specified by the set of
/// `t_TYPE` (template parameter) values.  The state is determined by the
/// interaction of two class fields: a flag indicating the presence of the
/// option and a specific value for the option.  If the flag indicates that the
/// option is not present, the value of the second field is ignored.
template <class t_TYPE>
class TestSpecificationGenerator_TypedOption
: public TestSpecificationGenerator_Option {
    // DATA
    bool                *d_isPresent_p;  // flag indicating if this option is
                                         // present in the current state of the
                                         // generator (held, not owned)

    t_TYPE              *d_value_p;      // current state (held, not owned)

    size_t               d_index;        // index of the current state

    bsl::vector<t_TYPE>  d_values;       // set of available states

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSpecificationGenerator_TypedOption,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create an object that iterates through the specified set of possible
    /// `values` and displays its state through the specified
    /// `isPresentVariable` and `valueVariable` accordingly.  Optionally
    /// specified `basicAllocator` is used to supply memory.
    TestSpecificationGenerator_TypedOption(
                                bool                       *isPresentVariable,
                                t_TYPE                     *valueVariable,
                                const bsl::vector<t_TYPE>&  values,
                                bslma::Allocator           *basicAllocator = 0)
    : d_isPresent_p(isPresentVariable)
    , d_value_p(valueVariable)
    , d_index(0)
    , d_values(values, bslma::Default::allocator(basicAllocator))
    {
        ASSERTV(!d_values.empty());
        *d_value_p = d_values[d_index];
    }

    /// Destroy this object.
    ~TestSpecificationGenerator_TypedOption()
    {}

    // MANIPULATORS

    /// Switch the state of this object to the next one.
    bool nextState()
    {
        if (!nextOption() || !nextOption()->nextState()) {
            // There is no subsequent node, or all subsequent nodes finished
            // their cycles.  This node needs to change state.

            if (*d_isPresent_p && d_values.size() - 1 == d_index) {
                // Current state is the latest in the cycle.  Resetting to
                // unrepresented state.
                *d_isPresent_p = false;
                d_index = 0;
                *d_value_p = d_values[d_index];
                return false;                                         // RETURN
            }
            else if (!*d_isPresent_p) {
                // Node is in unrepresented state.  Just switch to the
                // represented state.
                *d_isPresent_p = true;
            }
            else {
                // Node is present and there are values to iterate through in
                // this cycle.
                ++d_index;
                *d_value_p = d_values[d_index];
            }
        }

        // Some of the subsequent nodes changed its state, so this one doesn't
        // need to yet.  Just notify generator.
        return true;
    }
};

                   // ================================
                   // class TestSpecificationGenerator
                   // ================================

/// This class is a generator that provides a set of formatting specifications
/// obtained by exhaustive search of all combinations of standard options.  Its
/// use allows easy and thorough testing of formatters for standard types.  The
/// generator is a state machine, each state of which corresponds to a separate
/// specification.
/// The set of options can be configured using a special language:
///
///LANGUAGE SPECIFICATION
///----------------------
/// 'V' - flag indicating if argument id must be present in format
//        specification.  The generator substitutes only the preset zero index
//        (i.e. `0:`).
/// 'F' - command indicating if fill characters must be added to the produced
///       specifications.  By default two preset characters are used: '*' and
///       '=', but the user might add additional characters using
///       `addFillCharacter` method.
/// '^' - command indicating if alignment option must be added to the generated
///       specifications. Three standard options are iterated through in
///       sequence: '<', '^' and '>'.
/// '+' - command indicating if sign option must be added to the generated
///       specifications. Three standard options are iterated through in
///       sequence: '+', '-' and ' '.
/// '#' - flag indicating if alternate form option must be present in the
//        generated specifications.
/// '0' - flag indicating if zero option must be present in the generated
//        specifications.
/// 'W' - command indicating if width option must be added to the produced
///       specifications.  By default the following values are iterated
///       through: 1, 5, 6 and 10,  but the user might add additional values
///       using `addWidth` method.  This command does not add nested width
///       values.
/// 'P' - command indicating if precision option must be added to the produced
///       specifications.  By default the following values are iterated
///       through: 0, 5, 6 and 10,  but the user might add additional values
///       using `addPrecision` method.  This command does not add nested
///       precision values.
/// '{' - command indicating if nested width option must be added to the
///       produced specifications.  By default the following indexes are
///       iterated through: empty, 1, and 2 (i.e. `{}`, `{1}` and `{2}`.  Note
///       that due to implementation specifics this command automatically adds
///       non-nested width values (i.e. the default full set looks like ``(no
///       option), `{}`, `{1}`, `{2}`, `1`, `5`, `6` and `10`.
/// '}' - command indicating if nested precision option must be added to the
///       produced specifications.  By default the following indexes are
///       iterated through: empty, 1, and 2 (i.e. `.{}`, `.{1}` and `.{2}`.
///       Note that due to implementation specifics this command automatically
///       adds non-nested precision values (i.e. the default full set looks
///       like ``(no option), `.{}`, `.{1}`, `.{2}`, `.0`, `.5`, `.6` and
///       `.10`.
/// 'L' - flag indicating if locale option must be present in the generated
//        specifications.
/// 's' - command indicating if string presentation type (i.e. `s`) must be
///       present in the generated specifications.
/// 'i' - command indicating if integer presentation types must be present in
///       the generated specifications. The  following standard options are
///       iterated through in sequence: `b`, `B`, `c`, `d`, `o`, `x` and `X`.
/// 'c' - command indicating if character presentation types must be present in
///       the generated specifications. The  following standard options are
///       iterated through in sequence: `b`, `B`, `c`, `d`, `o`, `x` and `X`.
/// 'b' - command indicating if boolean presentation types must be present in
///       the generated specifications. The  following standard options are
///       iterated through in sequence: `s`, `b`, `B`, `c`, `d`, `o`, `x` and
///       `X`.
/// 'f' - command indicating if floating-point presentation types must be
///       present in the generated specifications. The  following standard
///       options are iterated through in sequence: `a`, `A`, `e`, `E`, `f`,
///       `F`, `g` and `G`.
/// 'p' - command indicating if pointer presentation types must be present in
///       the generated specifications. The  following standard options are
///       iterated through in sequence: `p` and `P`.
/// 'a' - command indicating if all presentation types must be present in the
///       generated specifications. The  following standard options are
///       iterated through in sequence: `s`, `b`, `B`, `c`, `d`, `o`, `x`, `X`,
///       `a`, `A`, `e`, `E`, `f`, `F`, `g`, `G`, `p` and `P`.
/// The default generator instruction is "F^+#0WP{}L".  Note that adding a
/// particular command to an instruction does not mean that the corresponding
/// option will be present in every generated specification; a specification
/// with the missing option will also be generated.
/// The generator produces both valid and invalid strings from the point of
/// view of the standard, which also allows thorough testing of the processing
/// of incorrect input data. But at the same time, it remains possible to
/// iterate only through valid specifications using 'nextValidForParse` or
/// 'nextValidForFormat` methods.
///
/// It is planned to move this class into a separate component afterwards,
/// which will allow it to be used across the `bslfmt` package without
/// duplicating the code.
template <class t_CHAR>
class TestSpecificationGenerator {
  public:
    // PUBLIC TYPES

    enum Alignment {
        e_ALIGN_DEFAULT,
        e_ALIGN_LEFT,
        e_ALIGN_MIDDLE,
        e_ALIGN_RIGHT
    };

    enum Sign {
        e_SIGN_DEFAULT,
        e_SIGN_POSITIVE,
        e_SIGN_NEGATIVE,
        e_SIGN_SPACE
    };

    enum NestedVariant {
        e_NESTED_DEFAULT,
        e_NESTED_ARG_1,
        e_NESTED_ARG_2
    };

    enum FormatType {
        e_TYPE_STRING,         // 's'
        e_TYPE_BINARY,         // `b`
        e_TYPE_BINARY_UC,      // `B`
        e_TYPE_CHARACTER,      // `c`
        e_TYPE_DECIMAL,        // `d`
        e_TYPE_OCTAL,          // `o`
        e_TYPE_INT_HEX,        // `x`
        e_TYPE_INT_HEX_UC,     // `X`
        e_TYPE_FLOAT_HEX,      // `a`
        e_TYPE_FLOAT_HEX_UC,   // `A`
        e_TYPE_SCIENTIFIC,     // `e`
        e_TYPE_SCIENTIFIC_UC,  // `E`
        e_TYPE_FIXED,          // `f`
        e_TYPE_FIXED_UC,       // `F`
        e_TYPE_GENERAL,        // `g`
        e_TYPE_GENERAL_UC,     // `G`
        e_TYPE_POINTER,        // 'p'
        e_TYPE_POINTER_UC      // 'P'
    };

  private:
    // TYPES
    typedef TestSpecificationGenerator_Option Option;

    // DATA
    bslma::ManagedPtr<Option>  d_firstOption;
                                   // first node in the option list (if any)

    bool                       d_valueIdOptionIsPresent;
                                   // flag indicating that value argument id is
                                   // present in the current specification

    bool                       d_fillCharacterIsPresent;
                                   // flag indicating that fill character is
                                   // present in the current specification

    char                       d_fillCharacter;
                                   // fill character value

    bool                       d_alignOptionIsPresent;
                                   // flag indicating that align option is
                                   // present in the current specification

    Alignment                  d_alignment;
                                   // alignment option value

    bool                       d_signOptionIsPresent;
                                   // flag indicating that sign option is
                                   // present in the current specification

    Sign                       d_sign;
                                   // sign option value

    bool                       d_hashOptionIsPresent;
                                   // flag indicating that alternative form
                                   // option is present in the current
                                   // specification

    bool                       d_zeroOptionIsPresent;
                                   // flag indicating that zero option is
                                   // present in the current specification

    bool                       d_widthOptionIsPresent;
                                   // flag indicating that width option is
                                   // present in the current specification

    int                        d_width;
                                   // width value

    bool                       d_nestedWidthIsPresent;
                                   // flag indicating that nested width option
                                   // is present in the current specification

    NestedVariant              d_nestedWidthVariant;
                                   // nested width option value

    bool                       d_precisionOptionIsPresent;
                                   // flag indicating that precision option is
                                   // present in the current specification

    int                        d_precision;
                                   // precision value

    bool                       d_nestedPrecisionIsPresent;
                                   // flag indicating that nested precision
                                   // option is present in the current
                                   // specification

    NestedVariant              d_nestedPrecisionVariant;
                                   // nested precision option value

    bool                       d_localeOptionIsPresent;
                                   // flag indicating that locale option is
                                   // present in the current specification

    bool                       d_typeOptionIsPresent;
                                   // flag indicating that presentation type
                                   // option is present in the current
                                   // specification

    FormatType                 d_type;
                                   // presentation type option value

    bsl::basic_string<t_CHAR>  d_spec;
                                   // generated specification suitable for
                                   // formatters `parse` method

    bsl::basic_string<t_CHAR>  d_formatSpec;
                                   // generated specification suitable for
                                   // formatters `format` method

    bsl::vector<char>          d_fillCharacters;
                                   // fill characters set

    bsl::vector<int>           d_widths;
                                   // width values set

    bsl::vector<int>           d_precisions;
                                   // precision values set

    bslma::Allocator          *d_allocator_p;
                                  // allocator used to supply memory (held, not
                                  // own)
  public:

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSpecificationGenerator,
                                   bslma::UsesBslmaAllocator);

  private:
    // PRIVATE MANIPULATORS

    /// Add the specified `newOption` to the option list.
    void addOption(Option *newOption);

    /// Generate a specification based on the current state of the object.
    void generateSpecification();

    // PRIVATE ACCESSORS

    /// Return `true` if the current state of the object assumes an automatic
    /// mode of indexing parameters, and `false` otherwise.
    bool isAutoIndexingMode() const;

    /// Return `true` if the current state of the object assumes a manual mode
    /// of indexing parameters, and `false` otherwise.
    bool isManualIndexingMode() const;

  public:
    // CREATORS

    /// Create a generator object.  Optionally specify the `basicAllocator` to
    /// supply memory.
    TestSpecificationGenerator(bslma::Allocator *basicAllocator = 0);

    // MANIPULATORS

    /// Add the specified `character` to the set of fill characters.  Note that
    /// generated instructions will contain the `character` only if this
    /// function is called before the `setup` method invocation.
    void addFillCharacter(char character);

    /// Add the specified `width` to the set of width values. Note that
    /// generated instructions will contain the `width` only if this function
    /// is called before the `setup` method invocation.
    void addWidth(int width);

    /// Add the specified `precision` to the set of precision values. Note that
    /// generated instructions will contain the `precision` only if this
    /// function is called before the `setup` method invocation.
    void addPrecision(int precision);

    /// Switch this object to the next state and generate specification based
    /// on the new state.
    bool next();

    /// Switch this object to the next state appropriate for generating a
    /// specification valid for formatters `parse` method and generate
    /// specification based on the new state.
    bool nextValidForParse();

    /// Switch this object to the next state appropriate for generating a
    /// specification valid for formatters `format` method and generate
    /// specification based on the new state.
    bool nextValidForFormat();

    /// Setup the sequence of this object's states based on the optionally
    /// specified `instruction`.
    void setup(const char *instruction = 0);

    // ACCESSORS

    /// Return the specification for the formatters `parse` method based on the
    /// current state of this object.
    const bsl::basic_string<t_CHAR>& spec() const;

    /// Return the specification for the formatters `format` method based on
    /// the current state of this object.
    const bsl::basic_string<t_CHAR>& formatSpec() const;

    /// Return `true` if the generated specification based on the current state
    /// of this object is valid for the formatters `parse` method, and `false`
    /// otherwise.
    bool isStateValidForParse() const;

    /// Return `true` if the generated specification based on the current state
    /// of this object is valid for the formatters `format` method, and `false`
    /// otherwise.
    bool isStateValidForFormat() const;

    /// Return `true` if the value id option is present in the current state of
    /// this object, and `false` otherwise.
    bool isValueIdOptionPresent() const;

    /// Return `true` if the value id option is present in the current state of
    /// this object, and `false` otherwise.
    bool isFillCharacterPresent() const;

    /// Return the current fill character.
    char fillCharacter() const;

    /// Return `true` if the align option is present in the current state of
    /// this object, and `false` otherwise.
    bool isAlignOptionPresent() const;

    /// Return the current alignment type.
    Alignment alignment() const;

    /// Return `true` if the sign option is present in the current state of
    /// this object, and `false` otherwise.
    bool isSignOptionPresent() const;

    /// Return the current sign option value.
    Sign sign() const;

    /// Return `true` if the alternative form option is present in the current
    /// state of this object, and `false` otherwise.
    bool isHashOptionPresent() const;

    /// Return `true` if the zero option is present in the current state of
    /// this object, and `false` otherwise.
    bool isZeroOptionPresent() const;

    /// Return `true` if the width option is present in the current state of
    /// this object, and `false` otherwise.
    bool isWidthOptionPresent() const;

    /// Return the current width value.
    int width() const;

    /// Return `true` if the nested width option is present in the current
    /// state of this object, and `false` otherwise.
    bool isNestedWidthPresent() const;

    /// Return the current nested width variant.
    NestedVariant nestedWidthVariant() const;

    /// Return `true` if the precision option is present in the current state
    /// of this object, and `false` otherwise.
    bool isPrecisionOptionPresent() const;

    /// Return the current precision value.
    int precision() const;

    /// Return `true` if the nested precision option is present in the current
    /// state of this object, and `false` otherwise.
    bool isNestedPrecisionPresent() const;

    /// Return the current nested precision variant.
    NestedVariant nestedPrecisionVariant() const;

    /// Return `true` if the locale option is present in the current state of
    /// this object, and `false` otherwise.
    bool isLocaleOptionPresent() const;

    /// Return `true` if the presentation type option is present in the current
    /// state of this object, and `false` otherwise.
    bool isTypeOptionPresent() const;

    /// Return the current presentation type value.
    FormatType type() const;
};

                   // --------------------------------
                   // class TestSpecificationGenerator
                   // --------------------------------

// PRIVATE MANIPULATORS
template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addOption(Option *newOption)
{
    if (d_firstOption) {
        d_firstOption->setNextOption(newOption, d_allocator_p);
    }
    else {
        d_firstOption.load(newOption, d_allocator_p);
    }
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::generateSpecification()
{
    typedef bslalg::NumericFormatterUtil NFUtil;
    const int maxIntValueLength = NFUtil::ToCharsMaxLength<int, 10>::k_VALUE;

    d_spec.clear();
    d_formatSpec.clear();

    if (d_fillCharacterIsPresent) {
        d_spec.push_back(d_fillCharacter);
    }
    if (d_alignOptionIsPresent) {
        switch (d_alignment) {
          case e_ALIGN_LEFT: {
            d_spec.push_back('<');
          } break;
          case e_ALIGN_MIDDLE: {
            d_spec.push_back('^');
          } break;
          case e_ALIGN_RIGHT: {
            d_spec.push_back('>');
          } break;
          default: {
            ASSERTV("Unexpected alignment option", false);
          }
        }
    }

    if (d_signOptionIsPresent) {
        switch (d_sign) {
          case e_SIGN_POSITIVE: {
            d_spec.push_back('+');
          } break;
          case e_SIGN_NEGATIVE: {
            d_spec.push_back('-');
          } break;
          case e_SIGN_SPACE: {
            d_spec.push_back(' ');
          } break;
          default: {
            ASSERTV("Unexpected sign option", false);
          }
        }
    }
    if (d_hashOptionIsPresent) {
        d_spec.push_back('#');
    }

    if (d_zeroOptionIsPresent) {
        d_spec.push_back('0');
    }

    if (d_widthOptionIsPresent && !d_nestedWidthIsPresent) {
        char  valueBuf[maxIntValueLength];
        char *valueEnd = NFUtil::toChars(valueBuf,
                                         valueBuf + maxIntValueLength,
                                         d_width);
        d_spec.insert(d_spec.end(), valueBuf, valueEnd);
    }
    else if (d_nestedWidthIsPresent) {
        switch (d_nestedWidthVariant) {
          case e_NESTED_DEFAULT: {
            d_spec.push_back('{');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_1: {
            d_spec.push_back('{');
            d_spec.push_back('1');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_2: {
            d_spec.push_back('{');
            d_spec.push_back('2');
            d_spec.push_back('}');
          } break;
        }
    }

    if (d_precisionOptionIsPresent && !d_nestedPrecisionIsPresent) {
        d_spec.push_back('.');
        char  valueBuf[maxIntValueLength];
        char *valueEnd = NFUtil::toChars(valueBuf,
                                         valueBuf + maxIntValueLength,
                                         d_precision);
        d_spec.insert(d_spec.end(), valueBuf, valueEnd);
    }
    else if (d_nestedPrecisionIsPresent) {
        d_spec.push_back('.');
        switch (d_nestedPrecisionVariant) {
          case e_NESTED_DEFAULT: {
            d_spec.push_back('{');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_1: {
            d_spec.push_back('{');
            d_spec.push_back('1');
            d_spec.push_back('}');
          } break;
          case e_NESTED_ARG_2: {
            d_spec.push_back('{');
            d_spec.push_back('2');
            d_spec.push_back('}');
          } break;
        }
    }

    if (d_localeOptionIsPresent) {
        d_spec.push_back('L');
    }
    if (d_typeOptionIsPresent) {
        switch (d_type) {
          case e_TYPE_STRING: {
            d_spec.push_back('s');
          } break;
          case e_TYPE_BINARY: {
            d_spec.push_back('b');
          } break;
          case e_TYPE_BINARY_UC: {
            d_spec.push_back('B');
          } break;
          case e_TYPE_CHARACTER: {
            d_spec.push_back('c');
          } break;
          case e_TYPE_DECIMAL: {
            d_spec.push_back('d');
          } break;
          case e_TYPE_OCTAL: {
            d_spec.push_back('o');
          } break;
          case e_TYPE_INT_HEX: {
            d_spec.push_back('x');
          } break;
          case e_TYPE_INT_HEX_UC: {
            d_spec.push_back('X');
          } break;
          case e_TYPE_FLOAT_HEX: {
            d_spec.push_back('a');
          } break;
          case e_TYPE_FLOAT_HEX_UC: {
            d_spec.push_back('A');
          } break;
          case e_TYPE_SCIENTIFIC: {
            d_spec.push_back('e');
          } break;
          case e_TYPE_SCIENTIFIC_UC: {
            d_spec.push_back('E');
          } break;
          case e_TYPE_FIXED: {
            d_spec.push_back('f');
          } break;
          case e_TYPE_FIXED_UC: {
            d_spec.push_back('F');
          } break;
          case e_TYPE_GENERAL: {
            d_spec.push_back('g');
          } break;
          case e_TYPE_GENERAL_UC: {
            d_spec.push_back('G');
          } break;
          case e_TYPE_POINTER: {
            d_spec.push_back('p');
          } break;
          case e_TYPE_POINTER_UC: {
            d_spec.push_back('P');
          } break;
          default: {
            ASSERTV("Unexpected type option", false);
          }
        }
    }

    // Assembling format specification
    if (d_valueIdOptionIsPresent) {
        d_formatSpec.push_back('{');
        d_formatSpec.push_back('0');
        d_formatSpec.push_back(':');
    }
    else {
        d_formatSpec.push_back('{');
        d_formatSpec.push_back(':');
    }
    d_formatSpec.append(d_spec);
    d_formatSpec.push_back('}');
}

// PRIVATE ACCESSORS
template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isAutoIndexingMode() const
{
    bool result = true;
    if (d_nestedWidthIsPresent && e_NESTED_DEFAULT != d_nestedWidthVariant) {
        result = false;
    }
    else if (d_nestedPrecisionIsPresent && e_NESTED_DEFAULT !=
                                               d_nestedPrecisionVariant) {
        result = false;
    }

    return result;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isManualIndexingMode() const
{
    bool result = false;
    if (d_nestedWidthIsPresent) {
        if (e_NESTED_DEFAULT != d_nestedWidthVariant) {
            if (d_nestedPrecisionIsPresent) {
                if (e_NESTED_DEFAULT != d_nestedPrecisionVariant) {
                    result = true;
                }
            }
            else {
                result = true;
            }
        }
    }
    else if (d_nestedPrecisionIsPresent && e_NESTED_DEFAULT !=
                                               d_nestedPrecisionVariant) {
        result = true;
    }
    return result;
}

// CREATORS
template <class t_CHAR>
TestSpecificationGenerator<t_CHAR>::TestSpecificationGenerator(
                                              bslma::Allocator *basicAllocator)
: d_valueIdOptionIsPresent(false)
, d_fillCharacterIsPresent(false)
, d_alignOptionIsPresent(false)
, d_signOptionIsPresent(false)
, d_hashOptionIsPresent(false)
, d_zeroOptionIsPresent(false)
, d_widthOptionIsPresent(false)
, d_nestedWidthIsPresent(false)
, d_precisionOptionIsPresent(false)
, d_nestedPrecisionIsPresent(false)
, d_localeOptionIsPresent(false)
, d_typeOptionIsPresent(false)
, d_spec(basicAllocator)
, d_formatSpec(basicAllocator)
, d_fillCharacters(basicAllocator)
, d_widths(basicAllocator)
, d_precisions(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Setup default sets of values.
    const char   defaultFillCharacters[] = { '*', '=' };
    const size_t numFillCharacters = sizeof(defaultFillCharacters);
    const int    defaultWidths[]         = { 1, 5, 6, 10 };
    const size_t numWidths               =
                          sizeof(defaultWidths) / sizeof(defaultWidths[0]);
    const int    defaultPrecisions[]     = { 0, 5, 6, 10 };
    const size_t numPrecisions           =
                  sizeof(defaultPrecisions) / sizeof(defaultPrecisions[0]);
    d_fillCharacters.insert(d_fillCharacters.begin(),
                            defaultFillCharacters,
                            defaultFillCharacters + numFillCharacters);
    d_widths.insert(d_widths.begin(),
                    defaultWidths,
                    defaultWidths + numWidths);
    d_precisions.insert(d_precisions.begin(),
                        defaultPrecisions,
                        defaultPrecisions + numPrecisions);
}

// MANIPULATORS
template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addFillCharacter(char character)
{
    d_fillCharacters.push_back(character);
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addWidth(int width)
{
    ASSERTV(width, 0 < width);
    d_widths.push_back(width);
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::addPrecision(int precision)
{
    ASSERTV(precision, 0 <= precision);
    d_precisions.push_back(precision);
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::next()
{
    if (d_firstOption)
    {
        if (d_firstOption->nextState()) {
           generateSpecification();
           return true;                                               // RETURN
        }
    }
    generateSpecification();
    return false;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::nextValidForParse()
{
    if (d_firstOption)
    {
        bool rv = d_firstOption->nextState();
        while (  rv && !isStateValidForParse()) {
            // Skip invalid specifications
            rv = d_firstOption->nextState();
        }
        generateSpecification();
        return rv;                                                    // RETURN
    }

    generateSpecification();
    return false;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::nextValidForFormat()
{
    if (d_firstOption)
    {
        bool rv = d_firstOption->nextState();
        while (  rv && !isStateValidForFormat()) {
            // Skip invalid specifications
            rv = d_firstOption->nextState();
        }
        generateSpecification();
        return rv;                                                    // RETURN
    }

    generateSpecification();
    return false;
}

template <class t_CHAR>
void TestSpecificationGenerator<t_CHAR>::setup(const char *instruction)
{
    const char  *defaultInstruction = "F^+#0WP{}L";
    bsl::string  instructionStr     = instruction
                                    ? instruction
                                    : defaultInstruction;
    bool         typeIsSpecified    = false;
    bool         widthIsAdded       = false;
    bool         precisionIsAdded   = false;

    for (bsl::string::const_iterator it = instructionStr.begin();
         it != instructionStr.end();
         ++it) {
        switch (*it) {
          case 'V': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_valueIdOptionIsPresent));
          } break;
          case 'F': {
            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<char>(
                                                     &d_fillCharacterIsPresent,
                                                     &d_fillCharacter,
                                                     d_fillCharacters));

          } break;
          case '^': {
            bsl::vector<Alignment> alignVector;
            alignVector.push_back(e_ALIGN_LEFT);
            alignVector.push_back(e_ALIGN_MIDDLE);
            alignVector.push_back(e_ALIGN_RIGHT);

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<Alignment>(
                                                       &d_alignOptionIsPresent,
                                                       &d_alignment,
                                                       alignVector));
          } break;
          case '+': {
            bsl::vector<Sign> signVector;
            signVector.push_back(e_SIGN_POSITIVE);
            signVector.push_back(e_SIGN_NEGATIVE);
            signVector.push_back(e_SIGN_SPACE);

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<Sign>(
                                                        &d_signOptionIsPresent,
                                                        &d_sign,
                                                        signVector));
          } break;
          case '#': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_hashOptionIsPresent));
          } break;
          case '0': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_zeroOptionIsPresent));
          } break;
          case 'W': {
            if (!widthIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                       &d_widthOptionIsPresent,
                                                       &d_width,
                                                       d_widths));
                widthIsAdded = true;
            }
          } break;
          case 'P': {
            if (!precisionIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                   &d_precisionOptionIsPresent,
                                                   &d_precision,
                                                   d_precisions));
                precisionIsAdded = true;
            }
          } break;
          case '{': {
            if (!widthIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                       &d_widthOptionIsPresent,
                                                       &d_width,
                                                       d_widths));
                widthIsAdded = true;
            }

            bsl::vector<NestedVariant> nestedVector;
            nestedVector.push_back(e_NESTED_DEFAULT);
            nestedVector.push_back(e_NESTED_ARG_1);
            nestedVector.push_back(e_NESTED_ARG_2);

            addOption(
                     new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<NestedVariant>(
                             &d_nestedWidthIsPresent,
                             &d_nestedWidthVariant,
                             nestedVector));
          } break;
          case '}': {
            if (!precisionIsAdded) {
                addOption(new (*d_allocator_p)
                              TestSpecificationGenerator_TypedOption<int>(
                                                   &d_precisionOptionIsPresent,
                                                   &d_precision,
                                                   d_precisions));
                precisionIsAdded = true;
            }

            bsl::vector<NestedVariant> nestedVector;
            nestedVector.push_back(e_NESTED_DEFAULT);
            nestedVector.push_back(e_NESTED_ARG_1);
            nestedVector.push_back(e_NESTED_ARG_2);

            addOption(
                     new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<NestedVariant>(
                             &d_nestedPrecisionIsPresent,
                             &d_nestedPrecisionVariant,
                             nestedVector));
          } break;
          case 'L': {
            addOption(
                    new (*d_allocator_p) TestSpecificationGenerator_BoolOption(
                        &d_localeOptionIsPresent));
          } break;
          case 's': {
            // string presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector(1, e_TYPE_STRING);
            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'i':
          case 'c': {
            // integral and character presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_BINARY);      // b
            typeVector.push_back(e_TYPE_BINARY_UC);   // B
            typeVector.push_back(e_TYPE_CHARACTER);   // c
            typeVector.push_back(e_TYPE_DECIMAL);     // d
            typeVector.push_back(e_TYPE_OCTAL);       // o
            typeVector.push_back(e_TYPE_INT_HEX);     // x
            typeVector.push_back(e_TYPE_INT_HEX_UC);  // X

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'b': {
            // bool presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_STRING);      // s
            typeVector.push_back(e_TYPE_BINARY);      // b
            typeVector.push_back(e_TYPE_BINARY_UC);   // B
            typeVector.push_back(e_TYPE_DECIMAL);     // d
            typeVector.push_back(e_TYPE_OCTAL);       // o
            typeVector.push_back(e_TYPE_INT_HEX);     // x
            typeVector.push_back(e_TYPE_INT_HEX_UC);  // X

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'f': {
            // floating-point presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
           typeVector.push_back(e_TYPE_FLOAT_HEX);      // a
           typeVector.push_back(e_TYPE_FLOAT_HEX_UC);   // A
           typeVector.push_back(e_TYPE_SCIENTIFIC);     // e
           typeVector.push_back(e_TYPE_SCIENTIFIC_UC);  // E
           typeVector.push_back(e_TYPE_FIXED);          // f
           typeVector.push_back(e_TYPE_FIXED_UC);       // F
           typeVector.push_back(e_TYPE_GENERAL);        // g
           typeVector.push_back(e_TYPE_GENERAL_UC);     // G

           addOption(new (*d_allocator_p)
                         TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'p': {
            // pointer presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_POINTER);     // 'P'
            typeVector.push_back(e_TYPE_POINTER_UC);  // 'P'

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          case 'a': {
            // all presentation types
            ASSERTV("Multiple types request", !typeIsSpecified);
            typeIsSpecified = true;

            bsl::vector<FormatType> typeVector;
            typeVector.push_back(e_TYPE_STRING);         // 's'
            typeVector.push_back(e_TYPE_BINARY);         // `b`
            typeVector.push_back(e_TYPE_BINARY_UC);      // `B`
            typeVector.push_back(e_TYPE_CHARACTER);      // `c`
            typeVector.push_back(e_TYPE_DECIMAL);        // `d`
            typeVector.push_back(e_TYPE_OCTAL);          // `o`
            typeVector.push_back(e_TYPE_INT_HEX);        // `x`
            typeVector.push_back(e_TYPE_INT_HEX_UC);     // `X`
            typeVector.push_back(e_TYPE_FLOAT_HEX);      // `a`
            typeVector.push_back(e_TYPE_FLOAT_HEX_UC);   // `A`
            typeVector.push_back(e_TYPE_SCIENTIFIC);     // `e`
            typeVector.push_back(e_TYPE_SCIENTIFIC_UC);  // `E`
            typeVector.push_back(e_TYPE_FIXED);          // `f`
            typeVector.push_back(e_TYPE_FIXED_UC);       // `F`
            typeVector.push_back(e_TYPE_GENERAL);        // `g`
            typeVector.push_back(e_TYPE_GENERAL_UC);     // `G`
            typeVector.push_back(e_TYPE_POINTER);        // 'p'
            typeVector.push_back(e_TYPE_POINTER_UC);     // 'p'

            addOption(new (*d_allocator_p)
                          TestSpecificationGenerator_TypedOption<FormatType>(
                                                        &d_typeOptionIsPresent,
                                                        &d_type,
                                                        typeVector));
          } break;
          default: {
            ASSERTV(*it, "Unexpected instruction command", false);
          }
        }
    }

    generateSpecification();
}

// ACCESSORS
template <class t_CHAR>
const bsl::basic_string<t_CHAR>&
TestSpecificationGenerator<t_CHAR>::spec() const
{
    return d_spec;
}

template <class t_CHAR>
const bsl::basic_string<t_CHAR>&
TestSpecificationGenerator<t_CHAR>::formatSpec() const
{
    return d_formatSpec;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isStateValidForParse() const
{
    // Rule: The sign, # and 0 options are only valid when an integer or
    // floating-point presentation type is used.
    bool invalidSignHashOrZero = d_typeOptionIsPresent &&
                                 (d_signOptionIsPresent ||
                                  d_hashOptionIsPresent ||
                                  d_zeroOptionIsPresent) &&
                                 (e_TYPE_STRING     == d_type ||
                                  e_TYPE_CHARACTER  == d_type ||
                                  e_TYPE_POINTER    == d_type ||
                                  e_TYPE_POINTER_UC == d_type);

    // Rule: Precision can be used with floating-point and string types
    // only.
    bool invalidTypeForPrecision =
              (d_precisionOptionIsPresent || d_nestedPrecisionIsPresent) &&
              (e_TYPE_BINARY     == d_type ||
               e_TYPE_BINARY_UC  == d_type ||
               e_TYPE_CHARACTER  == d_type ||
               e_TYPE_DECIMAL    == d_type ||
               e_TYPE_OCTAL      == d_type ||
               e_TYPE_INT_HEX    == d_type ||
               e_TYPE_INT_HEX_UC == d_type ||
               e_TYPE_POINTER    == d_type ||
               e_TYPE_POINTER_UC == d_type);

    // Rule: An align option must follow a fill character.
    bool missedAlignOption = d_fillCharacterIsPresent &&
                             !d_alignOptionIsPresent;

    // Rule: All parameter indexes in specification must be defined either
    // automatically or manually.
    bool conflictingIndexingStyle = !isAutoIndexingMode() &&
                                    !isManualIndexingMode();

    return !(invalidSignHashOrZero ||
             invalidTypeForPrecision ||
             missedAlignOption ||
             conflictingIndexingStyle);
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isStateValidForFormat() const
{
    // Value's index is defined differently than width's or precision's index.
    bool conflictingIndexingStyle =
                     (d_nestedWidthIsPresent || d_nestedPrecisionIsPresent) &&
                      ((d_valueIdOptionIsPresent && isAutoIndexingMode()) ||
                       (!d_valueIdOptionIsPresent && isManualIndexingMode()));

    return isStateValidForParse() && !conflictingIndexingStyle;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isValueIdOptionPresent() const
{
    return d_valueIdOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isFillCharacterPresent() const
{
    return d_fillCharacterIsPresent;
}

template <class t_CHAR>
char TestSpecificationGenerator<t_CHAR>::fillCharacter() const
{
    ASSERT(isFillCharacterPresent());
    return d_fillCharacter;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isAlignOptionPresent() const
{
    return d_alignOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::Alignment
TestSpecificationGenerator<t_CHAR>::alignment() const
{
    ASSERT(isAlignOptionPresent());
    return d_alignment;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isSignOptionPresent() const
{
    return d_signOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::Sign
TestSpecificationGenerator<t_CHAR>::sign() const
{
    ASSERT(d_signOptionIsPresent);
    return d_sign;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isHashOptionPresent() const
{
    return d_hashOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isZeroOptionPresent() const
{
    return d_zeroOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isWidthOptionPresent() const
{
    return d_widthOptionIsPresent;
}

template <class t_CHAR>
int TestSpecificationGenerator<t_CHAR>::width() const
{
    return d_width;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isNestedWidthPresent() const
{
    return d_nestedWidthIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::NestedVariant
TestSpecificationGenerator<t_CHAR>::nestedWidthVariant() const
{
    return d_nestedWidthVariant;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isPrecisionOptionPresent() const
{
    return d_precisionOptionIsPresent;
}

template <class t_CHAR>
int TestSpecificationGenerator<t_CHAR>::precision() const
{
    return d_precision;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isNestedPrecisionPresent() const
{
    return d_nestedPrecisionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::NestedVariant
TestSpecificationGenerator<t_CHAR>::nestedPrecisionVariant() const
{
    return d_nestedPrecisionVariant;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isLocaleOptionPresent() const
{
    return d_localeOptionIsPresent;
}

template <class t_CHAR>
bool TestSpecificationGenerator<t_CHAR>::isTypeOptionPresent() const
{
    return d_typeOptionIsPresent;
}

template <class t_CHAR>
typename TestSpecificationGenerator<t_CHAR>::FormatType
TestSpecificationGenerator<t_CHAR>::type() const
{
    return d_type;
}

/// Verify the state of the specified `parser` after processing the
/// specification generated for the current state of the specified `generator`
/// with the specified `sections`.
template <class t_CHAR>
void verifyParsedState(const FormatSpecificationParser<t_CHAR>&  parser,
                       const TestSpecificationGenerator<t_CHAR>& generator,
                       const Enums::Sections                     sections)
{
    const char * const SPEC = generator.spec().c_str();
    ASSERTV(SPEC, Enums::e_STATE_PARSED == parser.processingState());

    // Fill and align
    if (0 != (sections & Enums::e_SECTIONS_FILL_ALIGN)) {
        if (generator.isFillCharacterPresent()) {
            ASSERTV(SPEC, generator.fillCharacter(), *parser.filler(),
                    generator.fillCharacter() == *parser.filler());
        }
        else {
            ASSERTV(SPEC, *parser.filler(), ' ' == *parser.filler());
        }
        ASSERTV(SPEC, parser.numFillerCharacters(),
                1 == parser.numFillerCharacters());

        if(generator.isAlignOptionPresent()) {
            ASSERTV(SPEC, generator.alignment(), parser.alignment(),
                    static_cast<int>(generator.alignment()) ==
                        static_cast<int>(parser.alignment()));
        }
        else {
            ASSERTV(SPEC, parser.alignment(),
                    Enums::e_ALIGN_DEFAULT ==
                        parser.alignment());
        }
    }
    else {  // e_SECTIONS_FILL_ALIGN
        ASSERTV(SPEC, *parser.filler(), ' ' == *parser.filler());
        ASSERTV(SPEC, parser.numFillerCharacters(),
                1 == parser.numFillerCharacters());
        ASSERTV(SPEC, parser.alignment(),
                Enums::e_ALIGN_DEFAULT ==
                    parser.alignment());
    }

    // Sign
    if (0 != (sections & Enums::e_SECTIONS_SIGN_FLAG)) {
        if (generator.isSignOptionPresent()) {
            ASSERTV(SPEC, generator.sign(), parser.sign(),
                    static_cast<int>(generator.sign()) ==
                        static_cast<int>(parser.sign()));
         }
         else {
             ASSERTV(SPEC, parser.sign(),
                     Enums::e_SIGN_DEFAULT == parser.sign());
         }
    }
    else {  // e_SECTIONS_SIGN_FLAG
       ASSERTV(SPEC, parser.sign(), Enums::e_SIGN_DEFAULT == parser.sign());
    }

    // Hash
    if (0 != (sections & Enums::e_SECTIONS_ALTERNATE_FLAG)) {
         ASSERTV(SPEC,
                 generator.isHashOptionPresent() == parser.alternativeFlag());
    }
    else {
        ASSERTV(SPEC, false  == parser.alternativeFlag());
    }

    // Zero
    if (0 != (sections & Enums::e_SECTIONS_ZERO_PAD_FLAG)) {
         ASSERTV(SPEC,
                 generator.isZeroOptionPresent() == parser.zeroPaddingFlag());
    }
    else {
        ASSERTV(SPEC, false  == parser.zeroPaddingFlag());
    }

    // Width
    if (0 != (sections & Enums::e_SECTIONS_WIDTH)) {
        if (generator.isWidthOptionPresent()) {
             if (!generator.isNestedWidthPresent()) {
                 ASSERTV(
                        SPEC, parser.rawWidth().category(),
                        NumericValue::e_VALUE == parser.rawWidth().category());
                 ASSERTV(SPEC, generator.width(),  parser.rawWidth().value(),
                         generator.width() == parser.rawWidth().value());
             }
             else {
                 switch (generator.nestedWidthVariant()) {
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_DEFAULT: {
                    ASSERTV(SPEC, parser.rawWidth().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawWidth().category());
                    ASSERTV(SPEC,  parser.rawWidth().value(),
                            0 == parser.rawWidth().value());
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_1: {
                    ASSERTV(SPEC, parser.rawWidth().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawWidth().category());
                    ASSERTV(SPEC, parser.rawWidth().value(),
                            1 == parser.rawWidth().value());
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_2: {
                    ASSERTV(SPEC, parser.rawWidth().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawWidth().category());
                    ASSERTV(SPEC,
                            parser.rawWidth().value(),
                            2 == parser.rawWidth().value());
                   } break;
                   default: {
                    ASSERTV("Unexpected value",
                            generator.nestedWidthVariant(),
                            false);
                   }
                 }
             }
        }
        else if (!generator.isNestedWidthPresent()) {
            ASSERTV(SPEC, parser.rawWidth().category(),
                    NumericValue::e_DEFAULT == parser.rawWidth().category());
        }
    }
    else {  // e_SECTIONS_WIDTH
            ASSERTV(SPEC, parser.rawWidth().category(),
                    NumericValue::e_DEFAULT == parser.rawWidth().category());
    }

    // Precision
    if (0 != (sections & Enums::e_SECTIONS_PRECISION)) {
        if (generator.isPrecisionOptionPresent()) {
             if (!generator.isNestedPrecisionPresent()) {
                 ASSERTV(
                    SPEC, parser.rawPrecision().category(),
                    NumericValue::e_VALUE == parser.rawPrecision().category());
                 ASSERTV(
                   SPEC, generator.precision(),  parser.rawPrecision().value(),
                   generator.precision() == parser.rawPrecision().value());
             }
             else {
                 switch (generator.nestedPrecisionVariant()) {
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_DEFAULT: {
                    ASSERTV(SPEC, parser.rawPrecision().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawPrecision().category());
                    if (generator.isNestedWidthPresent()) {
                        // Nested width present, so zero index is occupied
                        ASSERTV(SPEC, parser.rawPrecision().value(),
                                1 == parser.rawPrecision().value());
                    }
                    else {
                        ASSERTV(SPEC, parser.rawPrecision().value(),
                                0 == parser.rawPrecision().value());
                    }
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_1: {
                    ASSERTV(SPEC, parser.rawPrecision().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawPrecision().category());
                    ASSERTV(SPEC, parser.rawPrecision().value(),
                            1 == parser.rawPrecision().value());
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_2: {
                    ASSERTV(SPEC, parser.rawPrecision().category(),
                            NumericValue::e_ARG_ID ==
                                parser.rawPrecision().category());
                    ASSERTV(SPEC,
                            parser.rawPrecision().value(),
                            2 == parser.rawPrecision().value());
                   } break;
                   default: {
                    ASSERTV("Unexpected value",
                            generator.nestedPrecisionVariant(),
                            false);
                   }
                 }
             }
        }
        else if (!generator.isNestedPrecisionPresent()) {
            ASSERTV(
                  SPEC, parser.rawPrecision().category(),
                  NumericValue::e_DEFAULT == parser.rawPrecision().category());
        }
    }
    else {  // e_SECTIONS_PRECISION
            ASSERTV(
                  SPEC, parser.rawPrecision().category(),
                  NumericValue::e_DEFAULT == parser.rawPrecision().category());
    }


    if (0 != (sections & Enums::e_SECTIONS_LOCALE_FLAG)) {
        ASSERTV(
             SPEC,
             generator.isLocaleOptionPresent() == parser.localeSpecificFlag());
    }
    else {
        ASSERTV(SPEC, false  == parser.localeSpecificFlag());
    }

    if (0 != (sections & Enums::e_SECTIONS_REMAINING_SPEC)) {
        // Unfortunately, due to the nature of the specification parsing
        // process, it is very difficult to create a single condition for all
        // combinations of different sets of sections and specification
        // strings.  Therefore, here we will check only the basic things.

        if (Enums::e_SECTIONS_REMAINING_SPEC == sections) {
            ASSERTV(SPEC, parser.remainingSpec().data(),
                     SPEC == parser.remainingSpec());
        }
        if (Enums::e_SECTIONS_ALL == sections) {
            if (generator.isTypeOptionPresent()) {
                 bsl::basic_string<t_CHAR> expectedSpec;
                 switch (generator.type()) {
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_STRING: {
                    expectedSpec.push_back('s');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_BINARY: {
                    expectedSpec.push_back('b');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_BINARY_UC: {
                    expectedSpec.push_back('B');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_CHARACTER: {
                    expectedSpec.push_back('c');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_DECIMAL: {
                    expectedSpec.push_back('d');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_OCTAL: {
                    expectedSpec.push_back('o');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_INT_HEX: {
                    expectedSpec.push_back('x');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_INT_HEX_UC: {
                    expectedSpec.push_back('X');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_FLOAT_HEX: {
                    expectedSpec.push_back('a');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_FLOAT_HEX_UC: {
                    expectedSpec.push_back('A');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_SCIENTIFIC: {
                    expectedSpec.push_back('e');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_SCIENTIFIC_UC: {
                    expectedSpec.push_back('E');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_FIXED: {
                    expectedSpec.push_back('f');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_FIXED_UC: {
                    expectedSpec.push_back('F');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_GENERAL: {
                    expectedSpec.push_back('g');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_GENERAL_UC: {
                    expectedSpec.push_back('G');
                   } break;
                   case TestSpecificationGenerator<t_CHAR>::e_TYPE_POINTER: {
                    expectedSpec.push_back('p');
                   } break;
                   case TestSpecificationGenerator<
                       t_CHAR>::e_TYPE_POINTER_UC: {
                    expectedSpec.push_back('P');
                   } break;
                   default: {
                    ASSERTV("Unexpected type option", false);
                   }
                 }
                 ASSERTV(SPEC,
                         expectedSpec.c_str(),
                         parser.remainingSpec().data(),
                         expectedSpec == parser.remainingSpec());
            }
            else {
                 ASSERTV(SPEC, parser.remainingSpec().data(),
                         bsl::basic_string_view<t_CHAR>() ==
                             parser.remainingSpec());
            }
        }
    }
}

/// Verify the state of the specified `parser` after postprocessing the
/// specification generated for the current state of the specified `generator`
/// with the specified `arg0`, 'arg1' and 'arg2' passed as parameters to the
/// format context constructor.
template <class t_CHAR>
void verifyPostprocessedState(
                           const FormatSpecificationParser<t_CHAR>&  parser,
                           const TestSpecificationGenerator<t_CHAR>& generator,
                           int                                       arg0,
                           int                                       arg1,
                           int                                       arg2)
{
    const t_CHAR * const SPEC = generator.spec().c_str();
    ASSERTV(SPEC, Enums::e_STATE_POSTPROCESSED == parser.processingState());

    // As generator does not support multibyte unicode symbols, we
    // expect that the filler takes only one byte.

    ASSERTV(SPEC, parser.fillerCodePointDisplayWidth(),
            1 == parser.fillerCodePointDisplayWidth());

    // Testing postprocessed width

    NumericValue postprocessedWidth = parser.postprocessedWidth();

    if (!generator.isWidthOptionPresent()) {
        if (!generator.isNestedWidthPresent()) {
            // We expect default values
            ASSERTV(
            SPEC, postprocessedWidth.category(),
            NumericValue::e_DEFAULT == postprocessedWidth.category());
        }
    }
    else if (!generator.isNestedWidthPresent()) {
        // Width specified by digit
        ASSERTV(
            SPEC, postprocessedWidth.category(),
            NumericValue::e_VALUE == postprocessedWidth.category());
        ASSERTV(
            SPEC, generator.width(), postprocessedWidth.value(),
            generator.width() == postprocessedWidth.value());
    }
    else {
        // Nested width
        ASSERTV(
            SPEC, postprocessedWidth.category(),
            NumericValue::e_VALUE == postprocessedWidth.category());
        switch (generator.nestedWidthVariant()) {
          case TestSpecificationGenerator<t_CHAR>::e_NESTED_DEFAULT: {
            ASSERTV( SPEC, arg0, postprocessedWidth.value(),
                     arg0 == postprocessedWidth.value());
          } break;
          case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_1: {
            ASSERTV( SPEC, arg1, postprocessedWidth.value(),
                     arg1 == postprocessedWidth.value());
          } break;
          case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_2: {
            ASSERTV( SPEC, arg2, postprocessedWidth.value(),
                     arg2 == postprocessedWidth.value());
          } break;
          default: {
              ASSERTV("Unexpected value", generator.nestedWidthVariant(),
                      false);
          }
        }
    }

    NumericValue postprocessedPrecision = parser.postprocessedPrecision();

    if (!generator.isPrecisionOptionPresent()) {
        if (!generator.isNestedPrecisionPresent()) {
            // We expect default values
            ASSERTV(
            SPEC, postprocessedPrecision.category(),
            NumericValue::e_DEFAULT == postprocessedPrecision.category());
        }
    }
    else if (!generator.isNestedPrecisionPresent()) {
        // Precision specified by digit
        ASSERTV(
            SPEC, postprocessedPrecision.category(),
            NumericValue::e_VALUE == postprocessedPrecision.category());
        ASSERTV(
            SPEC, generator.precision(), postprocessedPrecision.value(),
            generator.precision() == postprocessedPrecision.value());
    }
    else {
        // Nested precision
        ASSERTV(
            SPEC, postprocessedPrecision.category(),
            NumericValue::e_VALUE == postprocessedPrecision.category());
        switch (generator.nestedPrecisionVariant()) {
          case TestSpecificationGenerator<t_CHAR>::e_NESTED_DEFAULT: {
              if (generator.isNestedWidthPresent()) {
                  ASSERTV(SPEC, arg1, postprocessedPrecision.value(),
                          arg1 == postprocessedPrecision.value());
              }
              else {
                  ASSERTV(SPEC, arg0, postprocessedPrecision.value(),
                          arg0 == postprocessedPrecision.value());
              }
          } break;
          case  TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_1: {
            ASSERTV( SPEC, arg1, postprocessedPrecision.value(),
                     arg1 == postprocessedPrecision.value());
          } break;
          case TestSpecificationGenerator<t_CHAR>::e_NESTED_ARG_2: {
            ASSERTV( SPEC, arg2, postprocessedPrecision.value(),
                     arg2 == postprocessedPrecision.value());
          } break;
          default: {
              ASSERTV("Unexpected value", generator.nestedPrecisionVariant(),
                      false);
          }
        }
    }
}

// ============================================================================
//                            TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

// This template struct provides a namespace for implementations of test
// functions that verify the correctness of the `FormatSpecificationParser`
// methods.
template <class t_CHAR>
struct TestDriver {

    // CLASS METHODS
    /// Test `postprocess` method using test specification generator.
    static void testCase6(bool verbose, bool veryVerbose);

    /// Test `parse` method using test specification generator.
    static void testCase5(bool verbose, bool veryVerbose);
};

                                // ----------
                                // TEST CASES
                                // ----------
template <class t_CHAR>
void TestDriver<t_CHAR>::testCase6(bool verbose, bool veryVerbose)
{
    if (verbose)
        printf("\tTest postprocessed values using specification generator\n");
    {
        TestSpecificationGenerator<t_CHAR> generator;
        generator.setup("F^{}");
        const bsl::basic_string<t_CHAR>& spec    = generator.spec();
        int                              counter = 0;
        do {
              const int                    arg0 = 90;
              const int                    arg1 = 91;
              const int                    arg2 = 92;
              FormatSpecificationParser<t_CHAR> parser;
              bsl::basic_string_view<t_CHAR>    input(spec.c_str(),
                                                      spec.length());
              bslfmt::MockParseContext<t_CHAR>  parseContext(input, 3);
              bslfmt::MockFormatContext<t_CHAR> formatContext(arg0,
                                                              arg1,
                                                              arg2);

              parser.parse(&parseContext, Enums::e_SECTIONS_ALL);

              try {
                  parser.postprocess(formatContext);
              }
              catch (const bsl::format_error& err) {
                  ASSERTV(spec.c_str(), err.what(), false);
              }
              ASSERT(Enums::e_STATE_POSTPROCESSED == parser.processingState());

              verifyPostprocessedState(parser, generator, arg0, arg1, arg2);
              ++counter;
        } while (generator.nextValidForParse());

        if (veryVerbose) { T_ T_ P(counter); }
    }
}

template <class t_CHAR>
void TestDriver<t_CHAR>::testCase5(bool verbose, bool veryVerbose)
{
    if (verbose)
        printf("\tTesting separate sections using specification generator\n");
    {
        static const struct {
            int                    d_line;          // source line number
            const Enums::Sections  d_section;       // parser section
            const char            *d_instruction_p; // generator instr
        } DATA[] = {
            //LINE SECTION                            INSTRUCTION
            //---- --------------------------------   -----------
            { L_,  Enums::e_SECTIONS_FILL_ALIGN,      "F^"        },
            { L_,  Enums::e_SECTIONS_SIGN_FLAG,       "+"         },
            { L_,  Enums::e_SECTIONS_ALTERNATE_FLAG,  "#"         },
            { L_,  Enums::e_SECTIONS_ZERO_PAD_FLAG,   "0"         },
            { L_,  Enums::e_SECTIONS_WIDTH,           "{"         },
            { L_,  Enums::e_SECTIONS_PRECISION,       "}"         },
            { L_,  Enums::e_SECTIONS_LOCALE_FLAG,     "L"         },
        };
        const size_t NUM__DATA = sizeof DATA / sizeof *DATA;

        int counter = 0;
        for (size_t i = 1; i < NUM__DATA; ++i) {
            const int             SECT_LINE = DATA[i].d_line;
            const Enums::Sections SECTION   = DATA[i].d_section;

            for (size_t j = 1; j < NUM__DATA; ++j) {
              const int   INST_LINE   = DATA[i].d_line;
              const char *INSTRUCTION = DATA[j].d_instruction_p;

              TestSpecificationGenerator<char> generator;
              generator.setup(INSTRUCTION);
              const bsl::string& spec = generator.spec();
              do {
                CharParser                     parser;
                bsl::basic_string_view<char>   input(spec.c_str(),
                                                     spec.length());
                bslfmt::MockParseContext<char> context(input, 3);

                const bool matches = (i == j) || input.empty();
                try {
                    parser.parse(&context, SECTION);

                    if (!matches) {
                        // We expect parser to throw an exception for
                        // inappropriate  specifications.
                        ASSERTV(SECT_LINE,
                                INST_LINE,
                                spec.c_str(),
                                "Exception has not been thrown",
                                false);
                    }
                    verifyParsedState(parser, generator, SECTION);
                }
                catch (const bsl::format_error& err) {
                    if (matches) {
                        // We expect parser to handle appropriate
                        // specifications successfully.
                        ASSERTV(SECT_LINE,
                                INST_LINE,
                                spec.c_str(),
                                err.what(),
                                false);
                    }
                }
                ++counter;
              } while (generator.nextValidForParse());
            }
        }

        if (veryVerbose) { T_ T_ P(counter); }
    }

    if (verbose)
        printf("\tTesting a wide range of specifications parsing all "
               "sections");
    {
        const char            *INSTRUCTION = "VF^+#0{}a";
        const Enums::Sections  SECTION     = Enums::e_SECTIONS_ALL;

        TestSpecificationGenerator<char> generator;
        generator.setup(INSTRUCTION);
        const bsl::string& spec    = generator.spec();
        int                counter = 0;
        do {
            CharParser                     parser;
            bsl::basic_string_view<char>   input(spec.c_str(),
                                                 spec.length());
            bslfmt::MockParseContext<char> context(input, 3);

            try {
                parser.parse(&context, SECTION);
            }
            catch (const bsl::format_error& err) {
                ASSERTV(spec.c_str(), err.what(), false);
            }
            verifyParsedState(parser, generator, SECTION);
            ++counter;
        } while (generator.nextValidForParse());

        if (veryVerbose) { T_ T_ P(counter); }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `postprocess`
        //
        // Concerns:
        //: 1 The `postprocess` method correctly handles any unicode fill
        //:   character.
        //:
        //: 2 The `postprocess` method modifies width and precision attributes
        //:   in accordance with the arguments passed to the format context.
        //
        // Plan:
        //: 1 Using table-based approach specify a set of unicode fill
        //:   characters.  For each one parse the specification containing it,
        //:   postprocess it and verify that unicode character is correctly
        //:   handled.  (C-1)
        //:
        //: 2 Using table-based approach specify a set of specifications
        //:   containing various combinations of nested width and nested
        //:   precision options.  Parse these specifications and then
        //:   postprocess them passing different width and precision values to
        //:   the format context parameter.  Verify the values of the
        //:   `postprocessedWidth` and `postprocessedPrecision` attributes of
        //:   the parser.
        //:
        //: 3 Use test specification generator to produce a huge amount of
        //:   input and thoroughly test the basic functionality of the
        //:   `postprocess` method.  (C-2)
        //
        // Testing:
        //   void postprocess(const t_FORMAT_CONTEXT& context);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `postprocess`"
                            "\n=====================\n");

        if (verbose) printf("\tTesting fill character postprocessing.\n");
        {
            static const struct {
                int         d_line;        // source line number
                const char *d_utf8String;  // UTF-8 filling character
                const int   d_width;       // expected width of UTF-8 character
            } DATA[] = {
                 //LINE UTF_8               W8
                 //---- ------------------  --
                 { L_,  "\x01",             1  },
                 { L_,  "\x48",             1  },
                 { L_,  "\x7f",             1  },
                 { L_,  "\xc2\x80",         1  },
                 { L_,  "\xcb\xb1",         1  },
                 { L_,  "\xdf\xbf",         1  },
                 { L_,  "\xe0\xa0\x80",     1  },
                 { L_,  "\xe2\x9c\x90",     1  },
                 { L_,  "\xef\xbf\xbf",     1  },
                 { L_,  "\xf0\x90\x80\x80", 1  },
                 { L_,  "\xf0\x98\x9a\xa0", 2  },
                 { L_,  "\xf4\x8f\xbf\xbf", 1  },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            // `char`
            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int     LINE        = DATA[i].d_line;
                const char   *FILL_SYMBOL = DATA[i].d_utf8String;
                const int     WIDTH       = DATA[i].d_width;

                const Enums::Sections SECTION = Enums::e_SECTIONS_FILL_ALIGN;

                bsl::string spec = FILL_SYMBOL;
                spec.push_back('^');

                CharParser                      parser;
                bslfmt::MockParseContext<char>  parseContext(spec, 0);
                bslfmt::MockFormatContext<char> formatContext(90, 91, 92);

                parser.parse(&parseContext, SECTION);

                try {
                    parser.postprocess(formatContext);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(LINE, spec.c_str(), err.what(), false);
                }

                ASSERTV(LINE, WIDTH, parser.fillerCodePointDisplayWidth(),
                        WIDTH == parser.fillerCodePointDisplayWidth());
            }

            static const struct {
                int     d_line;          // source line number
                wchar_t d_spec16[2];     // UTF-16 filling character
                size_t  d_specLength16;  // length of UTF-16 filling character
                size_t  d_width16;       // expected width of UTF-16 character
                wchar_t d_spec32[1];     // UTF-32 filling character
                size_t  d_specLength32;  // length of UTF-32 filling character
                int     d_width32;       // expected width of UTF-32 character
            } WDATA[] = {
                //LINE  UTF_16               L16  N16  UTF_32         L32  N32
                //----  ------------------   ---  ---  ------------   ---  ---
                { L_,   { 0x1            },  1,   1,   { 0x01     },  1,   1 },
                { L_,   { 0x48           },  1,   1,   { 0x48     },  1,   1 },
                { L_,   { 0x7f           },  1,   1,   { 0x7f     },  1,   1 },
                { L_,   { 0x80           },  1,   1,   { 0x80     },  1,   1 },
                { L_,   { 0xf102         },  1,   1,   { 0xf102   },  1,   1 },
                { L_,   { 0xff07         },  1,   2,   { 0xff07   },  1,   2 },
                { L_,   { 0x08           },  1,   1,   { 0x08     },  1,   1 },
                { L_,   { 0x1027         },  1,   1,   { 0x1027   },  1,   1 },
                { L_,   { 0xffff         },  1,   1,   { 0xffff   },  1,   1 },
                { L_,   { 0xd828, 0xdc00 },  2,   1,   { 0xd8     },  1,   1 },
                { L_,   { 0xd834, 0xdd1e },  2,   1,   { 0x108601 },  1,   1 },
                { L_,   { 0xd83c, 0xdc31 },  2,   1,   { 0x10ffff },  1,   1 },
            };
            const size_t NUM_WDATA = sizeof WDATA / sizeof *WDATA;

            // `wchar_t`
            ASSERTV(sizeof(wchar_t),
                    4 == sizeof(wchar_t) || 2 == sizeof(wchar_t));

            for (size_t i = 0; i < NUM_WDATA; ++i) {
                const int      LINE            = WDATA[i].d_line;
                const bool     SIZEOF_WCHAR_32 = 4 == sizeof(wchar_t);
                const wchar_t *SPEC            =  SIZEOF_WCHAR_32
                                               ? WDATA[i].d_spec32
                                               : WDATA[i].d_spec16;
                const size_t   LENGTH          = SIZEOF_WCHAR_32
                                               ? WDATA[i].d_specLength32
                                               : WDATA[i].d_specLength16;
                const int      WIDTH           = SIZEOF_WCHAR_32
                                               ? WDATA[i].d_width32
                                               : WDATA[i].d_width16;

                const Enums::Sections SECTION = Enums::e_SECTIONS_FILL_ALIGN;

                bsl::wstring spec(SPEC, LENGTH);
                spec.push_back('^');

                WcharParser                       parser;
                bslfmt::MockParseContext<wchar_t> parseContext(spec, 0);
                bslfmt::MockFormatContext<char>   formatContext(90, 91, 92);

                parser.parse(&parseContext, SECTION);

                try {
                    parser.postprocess(formatContext);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(LINE, err.what(), false);
                }

                ASSERTV(LINE, WIDTH, parser.fillerCodePointDisplayWidth(),
                        WIDTH == parser.fillerCodePointDisplayWidth());
            }
        }

        if (verbose) printf("\tTesting width and precision postprocessing.\n");
        {
            typedef NumericValue::Category Category;

            // Table abbreviations
            const Category DEFAULT = NumericValue::e_DEFAULT;
            const Category VALUE   = NumericValue::e_VALUE;
            const int      NA      = -1;

            static const struct {
                  int                     d_line;
                                              // source line number

                  const char             *d_spec_p;
                                              // specification

                  NumericValue::Category  d_widthCategory;
                                              // expected width object's
                                              //category

                  int                     d_widthIndex;
                                              // index of format context
                                              // argument, storing the width
                                              // value

                  NumericValue::Category  d_precisionCategory;
                                              // expected precision object's
                                              // category

                  int                     d_precisionIndex;
                                              // index of format context
                                              // argument,  storing the
                                              // precision value
            } DATA[] = {
               //LINE SPEC       W_CAT      W_IND   P_CAT     P_IND
               //---- ---------  -------    -----   -------   -----
               { L_,  "",        DEFAULT,   NA,     DEFAULT,  NA    },
               { L_,  "{}",      VALUE,     0,      DEFAULT,  NA    },
               { L_,    ".{}",   DEFAULT,   NA,     VALUE,    0     },
               { L_,  "{}.{}",   VALUE,     0,      VALUE,    1     },
               { L_,  "{1}",     VALUE,     1,      DEFAULT,  NA    },
               { L_,  "{2}",     VALUE,     2,      DEFAULT,  NA    },
               { L_,     ".{1}", DEFAULT,   NA,     VALUE,    1     },
               { L_,     ".{2}", DEFAULT,   NA,     VALUE,    2     },
               { L_,  "{1}.{2}", VALUE,     1,      VALUE,    2     },
               { L_,  "{2}.{1}", VALUE,     2,      VALUE,    1     },
            };
            const size_t NUM__DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM__DATA; ++i) {
                const int       LINE  = DATA[i].d_line;
                const char     *SPEC  = DATA[i].d_spec_p;
                const Category  W_CAT = DATA[i].d_widthCategory;
                const int       W_IND = DATA[i].d_widthIndex;
                const Category  P_CAT = DATA[i].d_precisionCategory;
                const int       P_IND = DATA[i].d_precisionIndex;

                for (int j = 1; j <= 10; ++j) {
                    const int                       ARGS[] = { 0 + j,
                                                               1 + j,
                                                               2 + j };
                    FormatSpecificationParser<char> parser;
                    bslfmt::MockParseContext<char>  parseContext(SPEC, 3);
                    bslfmt::MockFormatContext<char> formatContext(ARGS[0],
                                                                  ARGS[1],
                                                                  ARGS[2]);

                    parser.parse(&parseContext, Enums::e_SECTIONS_ALL);

                    try {
                        parser.postprocess(formatContext);
                    }
                    catch (const bsl::format_error& err) {
                        ASSERTV(LINE, SPEC, err.what(), false);
                    }

                    NumericValue width     = parser.postprocessedWidth();
                    NumericValue precision = parser.postprocessedPrecision();

                    ASSERTV(LINE, W_CAT, width.category(),
                            W_CAT == width.category());
                    if (DEFAULT != W_CAT) {
                         ASSERTV(LINE, ARGS[W_IND], width.value(),
                                 ARGS[W_IND] == width.value());
                    }
                    ASSERTV(LINE, P_CAT, precision.category(),
                            P_CAT == precision.category());

                    if (DEFAULT != P_CAT) {
                         ASSERTV(LINE, ARGS[P_IND], precision.value(),
                                 ARGS[P_IND] == precision.value());
                    }
                }
            }
        }

        if (verbose) printf("\tTesting basic functionality using generator\n");

        TestDriver<char   >::testCase6(verbose, veryVerbose);
        TestDriver<wchar_t>::testCase6(verbose, veryVerbose);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `parse`
        //
        // Concerns:
        //: 1 The `parse` method correctly handles any valid parse
        //:   specification.
        //:
        //: 2 The `parse` method throws an appropriate exception for the
        //:   invalid specification.
        //:
        //: 3 The `parse` method parses specification parts exactly according
        //:   to the passed `sections` parameter.
        //:
        //: 4 The `parse` method correctly handles unicode fill characters.
        //
        // Plan:
        //: 1 Use test specification generator to produce a huge amount of
        //:   input and thoroughly test the basic functionality.
        //:
        //: 2 Using table-based approach specify different combinations of
        //:   specifications and sections for parsing.  For each pair parse the
        //:   specification and verify the value of the unparsed leftover.
        //:   (C-3)
        //:
        //: 3 Using table-based approach specify a set of invalid
        //:   specifications.  Parse each of them and verify that an exception
        //:   is thrown and contains the expected error message.  (C-2)
        //:
        //: 4 Using table-based approach specify a set of unicode fill
        //:   characters.  For each one parse the specification containing it
        //:   and verify that it is correctly handled.  (C-1, 4)
        //
        // Testing:
        //   void parse(t_PARSE_CONTEXT *parseContext, Sections sections);
        // --------------------------------------------------------------------

        if (verbose)
             printf("\nTESTING `parse`"
                    "\n===============\n");

        if (verbose) printf("\tTesting basic functionality using generator\n");

        TestDriver<char   >::testCase5(verbose, veryVerbose);
        TestDriver<wchar_t>::testCase5(verbose, veryVerbose);

        if (verbose) printf("\tTesting correct leftovers handling\n");
        {
            // Table abbreviations
            const Enums::Sections NONE       = Enums::e_SECTIONS_NONE;
            const Enums::Sections FILL_ALIGN = Enums::e_SECTIONS_FILL_ALIGN;
            const Enums::Sections SIGN_FLAG  = Enums::e_SECTIONS_SIGN_FLAG;
            const Enums::Sections ALTERNATE_FLAG =
                                              Enums::e_SECTIONS_ALTERNATE_FLAG;
            const Enums::Sections ZERO_PAD_FLAG =
                                               Enums::e_SECTIONS_ZERO_PAD_FLAG;
            const Enums::Sections WIDTH       = Enums::e_SECTIONS_WIDTH;
            const Enums::Sections PRECISION   = Enums::e_SECTIONS_PRECISION;
            const Enums::Sections LOCALE_FLAG = Enums::e_SECTIONS_LOCALE_FLAG;
            const Enums::Sections REMAINING_SPEC =
                                              Enums::e_SECTIONS_REMAINING_SPEC;
            const Enums::Sections ALL = Enums::e_SECTIONS_ALL;

            static const struct {
                int              d_line;        // source line number
                const char      *d_spec_p;      // parse specification
                Enums::Sections  d_section;     // section required
                const char      *d_expected_p;  // expected error message
            } DATA[] = {
                //LINE  SPEC              SECTION          REMAINING SPEC
                //----  --------------    ---------------  --------------
                { L_,   "=<+#010.{}Lg",   NONE,            "=<+#010.{}Lg" },
                { L_,   "=<+#010.{}Lg",   FILL_ALIGN,      "+#010.{}Lg"   },
                { L_,   "+#010.{}Lg",     SIGN_FLAG,       "#010.{}Lg"    },
                { L_,   "#010.{}Lg",      ALTERNATE_FLAG,  "010.{}Lg"     },
                { L_,   "010.{}Lg",       ZERO_PAD_FLAG,   "10.{}Lg"      },
                { L_,   "10.{}Lg",        WIDTH,           ".{}Lg"        },
                { L_,   ".{}Lg",          PRECISION,       "Lg"           },
                { L_,   "Lg",             LOCALE_FLAG,     "g"            },
                { L_,   "=<+#010.{}Lg",   REMAINING_SPEC,  "=<+#010.{}Lg" },
                { L_,   "=<+#010.{}Lg",   ALL,             "g"            },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int              LINE     = DATA[i].d_line;
                const bsl::string_view SPEC     = DATA[i].d_spec_p;
                const Enums::Sections  SECTION  = DATA[i].d_section;
                const bsl::string_view EXPECTED = DATA[i].d_expected_p;
                const Enums::Sections PARSE_SECTION =
                           static_cast<Enums::Sections>(
                                 SECTION | Enums::e_SECTIONS_REMAINING_SPEC);

                CharParser                     parser;
                bslfmt::MockParseContext<char> context(SPEC, 1);
                try {
                    parser.parse(&context, PARSE_SECTION);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, err.what(), false);
                }

                ASSERTV(LINE, SPEC.data(), SECTION, EXPECTED.data(),
                        parser.remainingSpec().data(),
                        EXPECTED == parser.remainingSpec());
            }
        }

        if (verbose) printf("\tNegative Testing\n");
        {
            // Table abbreviations
             const char *INVALID =
                             "Specification parse failure (invalid character)";
             const char *BRACKET = "Invalid fill character ('{' or '}')";
             const char *NON_POSITIVE_WIDTH = "Field widths must be > 0";
             const char *MIX_INDEXING_M_A = "Cannot mix manual (width) and "
                                            "automatic (precision) indexing";
             const char *MIX_INDEXING_A_M = "Cannot mix automatic (width) and "
                                            "manual (precision) indexing";

             const Enums::Sections ALIGN = Enums::e_SECTIONS_FILL_ALIGN;
             const Enums::Sections WIDTH = Enums::e_SECTIONS_WIDTH;
             const Enums::Sections ALL   = Enums::e_SECTIONS_ALL;

             static const struct {
                int              d_line;        // source line number
                const char      *d_spec_p;      // parse specification
                const wchar_t   *d_wSpec_p;     // parse specification
                Enums::Sections  d_section;     // section required
                const char      *d_expected_p;  // expected error message
            } DATA[] = {
                //LINE  CHAR_SPEC   WCHAR_SPEC   SECTION  MESSAGE
                //----  ---------   ----------   -------  ------------------
                {L_,    "+^^",      L"+^^",      ALIGN,   INVALID            },
                {L_,    "{^",       L"{^",       ALIGN,   BRACKET            },
                {L_,    "0",        L"0",        WIDTH,   NON_POSITIVE_WIDTH },
                {L_,    "#{}.{1}.", L"#{}.{1}.", ALL,     MIX_INDEXING_A_M   },
                {L_,    "#{1}.{}.", L"#{1}.{}.", ALL,     MIX_INDEXING_M_A   },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int              LINE     = DATA[i].d_line;
                const bsl::string_view SPEC     = DATA[i].d_spec_p;
                const bsl::wstring_view WSPEC   = DATA[i].d_wSpec_p;
                const Enums::Sections  SECTION  = DATA[i].d_section;
                const bsl::string_view EXPECTED = DATA[i].d_expected_p;

                CharParser                     parser;
                bslfmt::MockParseContext<char> context(SPEC, 0);
                try {
                    parser.parse(&context, SECTION);
                    ASSERTV(LINE, SPEC.data(), "Exception has not been thrown",
                            false);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, err.what(),
                            EXPECTED == bsl::string_view(err.what()));
                }

                WcharParser                       wparser;
                bslfmt::MockParseContext<wchar_t> wcontext(WSPEC, 0);
                try {
                    wparser.parse(&wcontext, SECTION);
                    ASSERTV(LINE, SPEC.data(), "Exception has not been thrown",
                            false);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, err.what(),
                            EXPECTED == bsl::string_view(err.what()));
                }
            }
        }

        {
            static const struct {
                int           d_line;        // source line number
                const char   *d_utf8String;  // UTF-8 filling character
                const size_t  d_length;      // length of UTF-8 character
            } DATA[] = {
                { L_, "\x01",              1 },  // 1 byte min
                { L_, "\x48",              1 },  // 1 byte
                { L_, "\x7f",              1 },  // 1 byte max
                { L_, "\xc2\x80",          2 },  // 2 byte min
                { L_, "\xcb\xb1",          2 },  // 2 byte
                { L_, "\xdf\xbf",          2 },  // 2 byte max
                { L_, "\xe0\xa0\x80",      3 },  // 3 byte min
                { L_, "\xe2\x9c\x90",      3 },  // 3 byte
                { L_, "\xef\xbf\xbf",      3 },  // 3 byte max
                { L_, "\xf0\x90\x80\x80",  4 },  // 4 byte min
                { L_, "\xf0\x98\x9a\xa0",  4 },  // 4 byte
                { L_, "\xf4\x8f\xbf\xbf",  4 },  // 4 byte max
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            // `char`
            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int     LINE        = DATA[i].d_line;
                const char   *FILL_SYMBOL = DATA[i].d_utf8String;
                const size_t  LENGTH      = DATA[i].d_length;

                const Enums::Sections SECTION = Enums::e_SECTIONS_FILL_ALIGN;

                bsl::string spec = FILL_SYMBOL;
                ASSERTV(LINE, LENGTH, spec.length(), LENGTH == spec.length());
                spec.push_back('^');

                CharParser                     parser;
                bslfmt::MockParseContext<char> context(spec, 0);

                try {
                    parser.parse(&context, SECTION);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, err.what(), false);
                }

                ASSERTV(LINE, parser.filler());
                ASSERTV(LINE, spec[0] == *parser.filler());
                ASSERTV(LINE, LENGTH, parser.numFillerCharacters(),
                        LENGTH ==
                            static_cast<size_t>(parser.numFillerCharacters()));
            }

            ASSERTV(sizeof(wchar_t),
                    4 == sizeof(wchar_t) || 2 == sizeof(wchar_t));

            static const struct {
                int     d_line;          // source line number
                wchar_t d_spec16[2];     // UTF-16 filling character
                size_t  d_specLength16;  // length of UTF-16 filling character
                int     d_numChars16;    // number of bytes in UTF-16 character
                wchar_t d_spec32[1];     // UTF-32 filling character
                size_t  d_specLength32;  // length of UTF-32 filling character
                int     d_numChars32;    // number of bytes in UTF-32 character
            } WDATA[] = {
                //LINE  UTF_16               L16  N16  UTF_32         L32  N32
                //----  ------------------   ---  ---  ------------   ---  ---
                { L_,   { 0x01           },  1,   1,   { 0x01     },  1,   1 },
                { L_,   { 0x48           },  1,   1,   { 0x48     },  1,   1 },
                { L_,   { 0x7f           },  1,   1,   { 0x7f     },  1,   1 },
                { L_,   { 0x80           },  1,   1,   { 0x80     },  1,   1 },
                { L_,   { 0xf102         },  1,   1,   { 0xf102   },  1,   1 },
                { L_,   { 0xff07         },  1,   1,   { 0xff07   },  1,   1 },
                { L_,   { 0x08           },  1,   1,   { 0x08     },  1,   1 },
                { L_,   { 0x1027         },  1,   1,   { 0x1027   },  1,   1 },
                { L_,   { 0xffff         },  1,   1,   { 0xffff   },  1,   1 },
                { L_,   { 0xd828, 0xdc00 },  2,   2,   { 0xd8     },  1,   1 },
                { L_,   { 0xd834, 0xdd1e },  2,   2,   { 0x108601 },  1,   1 },
                { L_,   { 0xd83c, 0xdc31 },  2,   2,   { 0x10ffff },  1,   1 },
            };
            const size_t NUM_WDATA = sizeof WDATA / sizeof *WDATA;

            for (size_t i = 0; i < NUM_WDATA; ++i) {
                const int      LINE            = WDATA[i].d_line;
                const bool     SIZEOF_WCHAR_32 = 4 == sizeof(wchar_t);
                const wchar_t *SPEC            = SIZEOF_WCHAR_32
                                               ? WDATA[i].d_spec32
                                               : WDATA[i].d_spec16;
                const size_t   LENGTH          = SIZEOF_WCHAR_32
                                               ? WDATA[i].d_specLength32
                                               : WDATA[i].d_specLength16;
                const int      NUM_CHARS       = SIZEOF_WCHAR_32
                                               ? WDATA[i].d_numChars32
                                               : WDATA[i].d_numChars16;

                const Enums::Sections SECTION = Enums::e_SECTIONS_FILL_ALIGN;

                bsl::wstring spec(SPEC, LENGTH);
                spec.push_back('^');

                WcharParser                       parser;
                bslfmt::MockParseContext<wchar_t> context(spec, 0);

                try {
                    parser.parse(&context, SECTION);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE,  err.what(), false);
                }

                ASSERTV(LINE, parser.filler());
                ASSERTV(LINE, spec[0] == *parser.filler());
                ASSERTV(LINE, NUM_CHARS, parser.numFillerCharacters(),
                        NUM_CHARS == parser.numFillerCharacters());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   According to the practice accepted in the BDE, in this test case
        //   we should test a set of functions sufficient to bring the object
        //   to any possible state.  In this case, we would be forced to test
        //   the constructor and all manipulators in one place.  For
        //   readability, we will test each function in a separate test case.
        //
        //   The second problem is that almost all accessors except one throw
        //   exceptions if we call them for a newly created object.  We will
        //   test them and this behavior in the next test.  And here we will
        //   check the state of the object, using the (yet untested)
        //   `processingState` accessor only.
        //
        // Concerns:
        //: 1 The accessors throw exceptions if called before the appropriate
        //:   parser's methods (`parse` and `postprocess`) have been called.
        //
        // Plan:
        //: 1 Create a parser and call accessors.  Verify that all functions
        //:   except `processingState` throw exceptions.  Verify that the
        //:   `processingState` identifies the current parser's state as
        //:   unparsed.
        //:
        //:  2 Call (untested yet) `parse` method and verify that all methods
        //:   except those that required the `postprocess` function call return
        //:   expected values.  Verify that the functions that required the
        //:   `postprocess` function call (`fillerCodePointDisplayWidth`,
        //:   `postprocessedWidth` and `postprocessedPrecision`) still throw
        //:   exceptions.
        //:
        //:  3 Call (untested yet) `postprocess` method and verify that all
        //:    methods return expected values.  (C-1)
        //
        // Testing:
        //   const t_CHAR *filler() const;
        //   int numFillerCharacters() const;
        //   int fillerCodePointDisplayWidth() const;
        //   Alignment alignment() const;
        //   Sign sign() const;
        //   bool alternativeFlag() const;
        //   bool zeroPaddingFlag() const;
        //   const NumericValue postprocessedWidth() const;
        //   const NumericValue postprocessedPrecision() const;
        //   const NumericValue rawWidth() const;
        //   const NumericValue rawPrecision() const;
        //   bool localeSpecificFlag() const;
        //   ProcessingState processingState() const;
        //   const bsl::basic_string_view<t_CHAR> remainingSpec() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        {
            CharParser charParser;
            ASSERTV(charParser.processingState(),
                    Enums::e_STATE_UNPARSED == charParser.processingState());

            VERIFY_EXCEPTION_IS_THROWN(charParser.filler();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.numFillerCharacters();)
            VERIFY_EXCEPTION_IS_THROWN(
                                     charParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.alignment();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.sign();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.alternativeFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.zeroPaddingFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.rawWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.rawPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.localeSpecificFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.remainingSpec();)

            bslfmt::MockParseContext<char> parseContext("", 1);
            charParser.parse(&parseContext, Enums::e_SECTIONS_ALL);

            ASSERTV(*charParser.filler(), ' ' == *charParser.filler());
            ASSERTV(charParser.numFillerCharacters(),
                    1 == charParser.numFillerCharacters());
            ASSERTV(charParser.alignment(),
                    Enums::e_ALIGN_DEFAULT == charParser.alignment());
            ASSERTV(charParser.sign(),
                    Enums::e_SIGN_DEFAULT == charParser.sign());
            ASSERTV(false == charParser.alternativeFlag());
            ASSERTV(false == charParser.zeroPaddingFlag());
            ASSERTV(
                  charParser.rawWidth().category(),
                  NumericValue::e_DEFAULT == charParser.rawWidth().category());
            ASSERTV(charParser.rawPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        charParser.rawPrecision().category());
            ASSERTV(false == charParser.localeSpecificFlag());
            ASSERTV(charParser.remainingSpec().data(),
                    charParser.remainingSpec().empty());

            VERIFY_EXCEPTION_IS_THROWN(
                                     charParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedPrecision();)

            MockFormatContext<char> formatContext(0);
            charParser.postprocess(formatContext);

            ASSERTV(charParser.fillerCodePointDisplayWidth(),
                    1 == charParser.fillerCodePointDisplayWidth());
            ASSERTV(charParser.postprocessedWidth().category(),
                    NumericValue::e_DEFAULT ==
                        charParser.postprocessedWidth().category());
            ASSERTV(charParser.postprocessedPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        charParser.postprocessedPrecision().category());
        }

        {
            CharParser charParser;
            ASSERTV(charParser.processingState(),
                    Enums::e_STATE_UNPARSED == charParser.processingState());

            VERIFY_EXCEPTION_IS_THROWN(charParser.filler();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.numFillerCharacters();)
            VERIFY_EXCEPTION_IS_THROWN(
                                     charParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.alignment();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.sign();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.alternativeFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.zeroPaddingFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.rawWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.rawPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.localeSpecificFlag();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.remainingSpec();)

            bslfmt::MockParseContext<char> parseContext("*<+#06.5Lg", 1);
            charParser.parse(&parseContext, Enums::e_SECTIONS_ALL);

            ASSERTV(*charParser.filler(), '*' == *charParser.filler());
            ASSERTV(charParser.numFillerCharacters(),
                    1 == charParser.numFillerCharacters());
            ASSERTV(charParser.alignment(),
                    Enums::e_ALIGN_LEFT == charParser.alignment());
            ASSERTV(charParser.sign(),
                    Enums::e_SIGN_POSITIVE == charParser.sign());
            ASSERTV(true == charParser.alternativeFlag());
            ASSERTV(true == charParser.zeroPaddingFlag());
            ASSERTV(charParser.rawWidth().category(),
                    NumericValue::e_VALUE == charParser.rawWidth().category());
            ASSERTV(charParser.rawWidth().value(),
                    6 == charParser.rawWidth().value());
            ASSERTV(
                charParser.rawPrecision().category(),
                NumericValue::e_VALUE == charParser.rawPrecision().category());
            ASSERTV(charParser.rawPrecision().value(),
                    5 == charParser.rawPrecision().value());
            ASSERTV(true == charParser.localeSpecificFlag());
            ASSERTV(charParser.remainingSpec().data(),
                    "g" == charParser.remainingSpec());

            VERIFY_EXCEPTION_IS_THROWN(
                                     charParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(charParser.postprocessedPrecision();)

            MockFormatContext<char> formatContext(0);
            charParser.postprocess(formatContext);

            ASSERTV(charParser.fillerCodePointDisplayWidth(),
                    1 == charParser.fillerCodePointDisplayWidth());
            ASSERTV(charParser.postprocessedWidth().category(),
                    NumericValue::e_VALUE ==
                        charParser.postprocessedWidth().category());
            ASSERTV(charParser.postprocessedWidth().value(),
                    6 == charParser.postprocessedWidth().value());
            ASSERTV(charParser.postprocessedPrecision().category(),
                    NumericValue::e_VALUE ==
                        charParser.postprocessedPrecision().category());
            ASSERTV(charParser.postprocessedPrecision().value(),
                    5 == charParser.postprocessedPrecision().value());
        }

        {
            WcharParser wcharParser;
            ASSERTV(wcharParser.processingState(),
                    Enums::e_STATE_UNPARSED == wcharParser.processingState());

            VERIFY_EXCEPTION_IS_THROWN(wcharParser.filler();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.numFillerCharacters();)
            VERIFY_EXCEPTION_IS_THROWN(
                                    wcharParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.alignment();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.sign();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.alternativeFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.zeroPaddingFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.rawWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.rawPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.localeSpecificFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.remainingSpec();)

            bslfmt::MockParseContext<wchar_t> parseContext(L"", 1);
            wcharParser.parse(&parseContext, Enums::e_SECTIONS_ALL);

            ASSERTV(*wcharParser.filler(), ' ' == *wcharParser.filler());
            ASSERTV(wcharParser.numFillerCharacters(),
                    1 == wcharParser.numFillerCharacters());
            ASSERTV(wcharParser.alignment(),
                    Enums::e_ALIGN_DEFAULT == wcharParser.alignment());
            ASSERTV(wcharParser.sign(),
                    Enums::e_SIGN_DEFAULT == wcharParser.sign());
            ASSERTV(false == wcharParser.alternativeFlag());
            ASSERTV(false == wcharParser.zeroPaddingFlag());
            ASSERTV(
                 wcharParser.rawWidth().category(),
                 NumericValue::e_DEFAULT == wcharParser.rawWidth().category());
            ASSERTV(wcharParser.rawPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        wcharParser.rawPrecision().category());
            ASSERTV(false == wcharParser.localeSpecificFlag());
            ASSERTV(wcharParser.remainingSpec().data(),
                    wcharParser.remainingSpec().empty());

            VERIFY_EXCEPTION_IS_THROWN(
                                    wcharParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedPrecision();)

            MockFormatContext<wchar_t> formatContext(0);
            wcharParser.postprocess(formatContext);

            ASSERTV(wcharParser.fillerCodePointDisplayWidth(),
                    1 == wcharParser.fillerCodePointDisplayWidth());
            ASSERTV(wcharParser.postprocessedWidth().category(),
                    NumericValue::e_DEFAULT ==
                        wcharParser.postprocessedWidth().category());
            ASSERTV(wcharParser.postprocessedPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        wcharParser.postprocessedPrecision().category());
        }

        {
            WcharParser wcharParser;
            ASSERTV(wcharParser.processingState(),
                    Enums::e_STATE_UNPARSED == wcharParser.processingState());

            VERIFY_EXCEPTION_IS_THROWN(wcharParser.filler();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.numFillerCharacters();)
            VERIFY_EXCEPTION_IS_THROWN(
                                    wcharParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.alignment();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.sign();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.alternativeFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.zeroPaddingFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.rawWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.rawPrecision();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.localeSpecificFlag();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.remainingSpec();)

            bslfmt::MockParseContext<wchar_t> parseContext(L"*<+#06.5Lg", 1);
            wcharParser.parse(&parseContext, Enums::e_SECTIONS_ALL);

            ASSERTV(*wcharParser.filler(), '*' == *wcharParser.filler());
            ASSERTV(wcharParser.numFillerCharacters(),
                    1 == wcharParser.numFillerCharacters());
            ASSERTV(wcharParser.alignment(),
                    Enums::e_ALIGN_LEFT == wcharParser.alignment());
            ASSERTV(wcharParser.sign(),
                    Enums::e_SIGN_POSITIVE == wcharParser.sign());
            ASSERTV(true == wcharParser.alternativeFlag());
            ASSERTV(true == wcharParser.zeroPaddingFlag());
            ASSERTV(
                   wcharParser.rawWidth().category(),
                   NumericValue::e_VALUE == wcharParser.rawWidth().category());
            ASSERTV(wcharParser.rawWidth().value(),
                    6 == wcharParser.rawWidth().value());
            ASSERTV(wcharParser.rawPrecision().category(),
                    NumericValue::e_VALUE ==
                        wcharParser.rawPrecision().category());
            ASSERTV(wcharParser.rawPrecision().value(),
                    5 == wcharParser.rawPrecision().value());
            ASSERTV(true == wcharParser.localeSpecificFlag());
            ASSERTV(wcharParser.remainingSpec().data(),
                    L"g" == wcharParser.remainingSpec());

            VERIFY_EXCEPTION_IS_THROWN(
                                    wcharParser.fillerCodePointDisplayWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedWidth();)
            VERIFY_EXCEPTION_IS_THROWN(wcharParser.postprocessedPrecision();)

            MockFormatContext<wchar_t> formatContext(0);
            wcharParser.postprocess(formatContext);

            ASSERTV(wcharParser.fillerCodePointDisplayWidth(),
                    1 == wcharParser.fillerCodePointDisplayWidth());
            ASSERTV(wcharParser.postprocessedWidth().category(),
                    NumericValue::e_VALUE ==
                        wcharParser.postprocessedWidth().category());
            ASSERTV(wcharParser.postprocessedWidth().value(),
                    6 == wcharParser.postprocessedWidth().value());
            ASSERTV(wcharParser.postprocessedPrecision().category(),
                    NumericValue::e_VALUE ==
                        wcharParser.postprocessedPrecision().category());
            ASSERTV(wcharParser.postprocessedPrecision().value(),
                    5 == wcharParser.postprocessedPrecision().value());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING APPARATUS
        //   `TestSpecificationGenerator` is planned to be moved into a
        //   separate component where thorough testing will be carried out.
        //   Therefore, this test case should be considered as a breathing test
        //   for the generator.
        //
        // Concerns:
        //: 1 Generator produces all possible variations of specification
        //:   according to the received instruction.
        //:
        //: 2 The main loop of the generator states can be run over and over,
        //:   producing the same specifications.
        //
        // Plan:
        //: 1 Specify some option and consequently check all generator states
        //:   using table-based approach.
        //:
        //: 2. Setup generator and iterate through all its states once.  Setup
        //:    second generator using the same instructions that have been used
        //:    for the first one.  Now run both generators simultaneously and
        //:    verify that they produce the same results.  (C-2)
        //:
        //: 3. Setup generator with nested width and nested precision options.
        //:    Iterate through the certain number of generator states and
        //:    verify its attributes using table-based approach.
        //:
        //: 4. Use standard formatting implementation (if available) to verify
        //:    validity of the specifications produced by the generator.  (C-1)
        //
        // Testing:
        //   TEST SPECIFICATION GENERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING APPARATUS"
                            "\n=================\n");

        typedef TestSpecificationGenerator<char> Generator;
        typedef Generator::Alignment             Alignment;

        const char       charNA = 0;
        const Alignment alignNA = Generator::e_ALIGN_LEFT;

        static const struct {
            int         d_line;                   // source line number
            bool        d_fillCharacterPresence;  // fill character presence
            char        d_fillCharacter;          // fill character
            bool        d_alignmentPresence;      // align optionpresence
            Alignment   d_alignment;              // alignment type
            const char *d_spec_p;                 // specification

        } DATA[] = {
            //LINE FILL_P FILL_V  ALIGN_P ALIGN_V                     SPEC
            //---- ------ ------  ------- --------------------------  ----
            { L_,  false, charNA, false,   alignNA,                   ""   },
            { L_,  false, charNA, true,    Generator::e_ALIGN_LEFT,   "<"  },
            { L_,  false, charNA, true,    Generator::e_ALIGN_MIDDLE, "^"  },
            { L_,  false, charNA, true,    Generator::e_ALIGN_RIGHT,  ">"  },
        //  { L_,  true,  '*',    false,   alignNA,                   "*"  },
            { L_,  true , '*',    true,    Generator::e_ALIGN_LEFT,   "*<" },
            { L_,  true,  '*',    true,    Generator::e_ALIGN_MIDDLE, "*^" },
            { L_,  true,  '*',    true,    Generator::e_ALIGN_RIGHT,  "*>" },
        //  { L_,  true,  '=',    false,   alignNA,                   "="  },
            { L_,  true , '=',    true,    Generator::e_ALIGN_LEFT,   "=<" },
            { L_,  true,  '=',    true,    Generator::e_ALIGN_MIDDLE, "=^" },
            { L_,  true,  '=',    true,    Generator::e_ALIGN_RIGHT,  "=>" },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\tTesting align and fill option\n");
        {
            TestSpecificationGenerator<char> generator;
            generator.setup("F^");
            const bsl::string& spec = generator.spec();

            size_t i = 0;
            do {
                int               LINE           = DATA[i].d_line;
                bool              FILL_PRESENCE  =
                                               DATA[i].d_fillCharacterPresence;
                char              FILL_CHARACTER = DATA[i].d_fillCharacter;
                bool              ALIGN_PRESENCE = DATA[i].d_alignmentPresence;
                Alignment         ALIGNMENT      = DATA[i].d_alignment;
                const bsl::string EXP_SPEC       = DATA[i].d_spec_p;

                ASSERTV(LINE,
                        FILL_PRESENCE == generator.isFillCharacterPresent());
                if (FILL_PRESENCE) {
                    ASSERTV(LINE,
                            FILL_CHARACTER,
                            generator.fillCharacter(),
                            FILL_CHARACTER == generator.fillCharacter());
                  }
                  ASSERTV(LINE,
                          ALIGN_PRESENCE == generator.isAlignOptionPresent());
                  if (ALIGN_PRESENCE) {
                      ASSERTV(LINE,
                              ALIGNMENT,
                              generator.alignment(),
                              ALIGNMENT == generator.alignment());
                  }
                  ASSERTV(LINE,
                          EXP_SPEC.c_str(),
                          spec.c_str(),
                          EXP_SPEC == spec);

                  ++i;
            } while (generator.nextValidForParse());
            ASSERTV(NUM_DATA, i, NUM_DATA == i);
        }


        if (verbose) printf("\tTesting consecutive generator cycles.\n");
        {
            const char *generatorInstruction = "V{}";
            TestSpecificationGenerator<char> origin;
            origin.setup(generatorInstruction);
            const bsl::string& originSpec       = origin.spec();
            const bsl::string& originFormatSpec = origin.formatSpec();
            int originCounter = 0;

            // Run the full cycle once.

            do {
               ++originCounter;
            } while(origin.next());

            if (veryVerbose) printf("\t\tTotal: %d\n", originCounter);

            TestSpecificationGenerator<char> checker;
            checker.setup(generatorInstruction);
            const bsl::string& checkerSpec       = checker.spec();
            const bsl::string& checkerFormatSpec = checker.formatSpec();

            ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                    checkerSpec == originSpec);
            ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                    checkerFormatSpec == originFormatSpec);

            // Now run two generators simultaneously and compare the results.

            while (checker.next()) {
               ASSERT(origin.next());

               ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                       checkerSpec == originSpec);
               ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                       checkerFormatSpec == originFormatSpec);
               --originCounter;
            }
            ASSERT(!origin.next());
            --originCounter;

            ASSERTV(checkerSpec.c_str(), originSpec.c_str(),
                    checkerSpec == originSpec);
            ASSERTV(checkerFormatSpec.c_str(), originFormatSpec.c_str(),
                    checkerFormatSpec == originFormatSpec);
            ASSERTV(originCounter, 0 == originCounter);
        }

        if (verbose)
            printf("\tTesting the iteration of the generator states\n");
        {
            typedef Generator::NestedVariant NestedVar;

            // Table abbreviations
            const bool      F   = false;
            const bool      T   = true;
            const NestedVar DFT = Generator::e_NESTED_DEFAULT;
            const NestedVar A_1 = Generator::e_NESTED_ARG_1;
            const NestedVar A_2 = Generator::e_NESTED_ARG_2;

            static const struct {
                int              d_line;
                                     // source line number
                const char      *d_spec_p;
                                     // expected `parse` specification
                const char      *d_format_p;
                                     // expected `format` specification

                const bool       d_widthPresence;
                                     // expected width option presence

                const int        d_widthValue;
                                     // expected width value

                const bool       d_nestedWidthPresence;
                                     // expected nested width option presence

                const NestedVar  d_nestedWidthValue;
                                     // expected nested width value

                const bool       d_precisionPresence;
                                     // expected precision option presence

                const int        d_precisionValue;
                                     // expected precision value

                const bool       d_nestedPrecisionPresence;
                                     // expected nested precision presence

                const NestedVar  d_nestedPrecisionValue;
                                     // expected nested precision value

                const bool       d_isValidForParse;
                                     // expected specification validity for
                                     // the formatters `parse` method

                const bool       d_isValidForFormat;
                                     // expected specification validity for
                                     // the formatters `format` method

            } DATA[] = {
  //------------^
  //L   PARSE      FORMAT        WP  WV  NWP NWV   PP  PV  NPP NPV   PVD  FVD
  //--- ---------- ------------  --- --- --- ---   --- --- --- ---   ---  ---
  { L_, "",        "{:}",        F,  1,  F,  DFT,  F,  0,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  F,  0,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  F,  0,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  F,  0,  T,  A_2,  T,   F },
  { L_, ".0",      "{:.0}",      F,  1,  F,  DFT,  T,  0,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  0,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  0,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  0,  T,  A_2,  T,   F },
  { L_, ".5",      "{:.5}",      F,  1,  F,  DFT,  T,  5,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  5,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  5,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  5,  T,  A_2,  T,   F },
  { L_, ".6",      "{:.6}",      F,  1,  F,  DFT,  T,  6,  F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  6,  T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  6,  T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  6,  T,  A_2,  T,   F },
  { L_, ".10",     "{:.10}",     F,  1,  F,  DFT,  T,  10, F,  DFT,  T,   T },
  { L_, ".{}",     "{:.{}}",     F,  1,  F,  DFT,  T,  10, T,  DFT,  T,   T },
  { L_, ".{1}",    "{:.{1}}",    F,  1,  F,  DFT,  T,  10, T,  A_1,  T,   F },
  { L_, ".{2}",    "{:.{2}}",    F,  1,  F,  DFT,  T,  10, T,  A_2,  T,   F },
  { L_, "{}",      "{:{}}",      F,  1,  T,  DFT,  F,  0,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  F,  0,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  F,  0,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  F,  0,  T,  A_2,  F,   F },
  { L_, "{}.0",    "{:{}.0}",    F,  1,  T,  DFT,  T,  0,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  0,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  0,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  0,  T,  A_2,  F,   F },
  { L_, "{}.5",    "{:{}.5}",    F,  1,  T,  DFT,  T,  5,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  5,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  5,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  5,  T,  A_2,  F,   F },
  { L_, "{}.6",    "{:{}.6}",    F,  1,  T,  DFT,  T,  6,  F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  6,  T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  6,  T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  6,  T,  A_2,  F,   F },
  { L_, "{}.10",   "{:{}.10}",   F,  1,  T,  DFT,  T,  10, F,  DFT,  T,   T },
  { L_, "{}.{}",   "{:{}.{}}",   F,  1,  T,  DFT,  T,  10, T,  DFT,  T,   T },
  { L_, "{}.{1}",  "{:{}.{1}}",  F,  1,  T,  DFT,  T,  10, T,  A_1,  F,   F },
  { L_, "{}.{2}",  "{:{}.{2}}",  F,  1,  T,  DFT,  T,  10, T,  A_2,  F,   F },
  { L_, "{1}",     "{:{1}}",     F,  1,  T,  A_1,  F,  0,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  F,  0,  T,  DFT,  F,   F },
  { L_, "{1}.{1}", "{:{1}.{1}}", F,  1,  T,  A_1,  F,  0,  T,  A_1,  T,   F },
  { L_, "{1}.{2}", "{:{1}.{2}}", F,  1,  T,  A_1,  F,  0,  T,  A_2,  T,   F },
  { L_, "{1}.0",   "{:{1}.0}",   F,  1,  T,  A_1,  T,  0,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  T,  0,  T,  DFT,  F,   F },
  { L_, "{1}.{1}", "{:{1}.{1}}", F,  1,  T,  A_1,  T,  0,  T,  A_1,  T,   F },
  { L_, "{1}.{2}", "{:{1}.{2}}", F,  1,  T,  A_1,  T,  0,  T,  A_2,  T,   F },
  { L_, "{1}.5",   "{:{1}.5}",   F,  1,  T,  A_1,  T,  5,  F,  DFT,  T,   F },
  { L_, "{1}.{}",  "{:{1}.{}}",  F,  1,  T,  A_1,  T,  5,  T,  DFT,  F,   F },
  //------------v
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            Generator generator;
            generator.setup("{}");

            for (size_t i = 0; i < NUM_DATA; ++i) {
                int              LINE        = DATA[i].d_line;
                const char      *SPEC        = DATA[i].d_spec_p;
                const char      *FMT_SPEC    = DATA[i].d_format_p;
                const bool       W_PRESENCE  = DATA[i].d_widthPresence;
                const int        W_VALUE     = DATA[i].d_widthValue;
                const bool       NW_PRESENCE = DATA[i].d_nestedWidthPresence;
                const NestedVar  NW_VALUE    = DATA[i].d_nestedWidthValue;
                const bool       P_PRESENCE  = DATA[i].d_precisionPresence;
                const int        P_VALUE     = DATA[i].d_precisionValue;
                const bool       NP_PRESENCE =
                                             DATA[i].d_nestedPrecisionPresence;
                const NestedVar  NP_VALUE    = DATA[i].d_nestedPrecisionValue;
                const bool       PRS_VALID   = DATA[i].d_isValidForParse;
                const bool       FMT_VALID   = DATA[i].d_isValidForFormat;

                ASSERTV(LINE, SPEC, generator.spec().c_str(),
                        SPEC == generator.spec());
                ASSERTV(LINE, FMT_SPEC, generator.formatSpec().c_str(),
                        FMT_SPEC == generator.formatSpec());
                ASSERTV(LINE, generator.isWidthOptionPresent(),
                        W_PRESENCE == generator.isWidthOptionPresent());
                ASSERTV(LINE, W_VALUE, generator.width(),
                        W_VALUE == generator.width());
                ASSERTV(LINE, generator.isNestedWidthPresent(),
                        NW_PRESENCE == generator.isNestedWidthPresent());
                ASSERTV(LINE, NW_VALUE, generator.nestedWidthVariant(),
                        NW_VALUE == generator.nestedWidthVariant());
                ASSERTV(LINE, generator.isPrecisionOptionPresent(),
                        P_PRESENCE == generator.isPrecisionOptionPresent());
                ASSERTV(LINE, P_VALUE,generator.precision(),
                        P_VALUE == generator.precision());
                ASSERTV(LINE, generator.isNestedPrecisionPresent(),
                        NP_PRESENCE == generator.isNestedPrecisionPresent());
                ASSERTV(LINE, NP_VALUE, generator.nestedPrecisionVariant(),
                        NP_VALUE == generator.nestedPrecisionVariant());
                ASSERTV(LINE, generator.isStateValidForParse(),
                        PRS_VALID == generator.isStateValidForParse());
                ASSERTV(LINE, generator.isStateValidForFormat(),
                        FMT_VALID == generator.isStateValidForFormat());

                ASSERTV(LINE, generator.next());
            }
        }

        // Testing generator using standard library implementation

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        if (verbose)
            printf("\tTesting generator with oracle.\n");
        {
            TestSpecificationGenerator<char> generator;
            generator.setup("VF^+#0{}Lf");

            const bsl::string& spec    = generator.formatSpec();
            int                counter = 0;
            float              arg0    = 1;
            int                arg1    = 2;
            int                arg2    = 3;
            do {
                try {
                    if (generator.isNestedWidthPresent()) {
                        if (generator.isNestedPrecisionPresent()) {
                            (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                        }
                        else {
                            (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                        }
                    }
                    else if (generator.isNestedPrecisionPresent()) {
                        (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                    }
                    else {
                        (void)std::vformat(
                                      spec.c_str(),
                                      std::make_format_args(arg0, arg1, arg2));
                    }

                    // Verify that we do not miss states that are considered
                    // valid by the standard implementation.

                    ASSERTV(counter,
                            spec.c_str(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            generator.isStateValidForParse() &&
                                generator.isStateValidForFormat());
                }
                catch (std::format_error& err) {

                    // Verify that if the standard implementation considers
                    // the specification invalid then the generator considers
                    // it invalid too.
                    ASSERTV(counter,
                            spec.c_str(),
                            err.what(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            !generator.isStateValidForParse() ||
                                !generator.isStateValidForFormat());
                }
                ++counter;
            } while (generator.next());

            if (veryVerbose) {
                T_ T_ P(counter);
            }
        }

        // wchar_t
        {
            TestSpecificationGenerator<wchar_t> generator;
            generator.setup("VF^+#0{}Lf");

            const bsl::wstring& spec    = generator.formatSpec();
            int                 counter = 0;
            float               arg0    = 1;
            int                 arg1    = 2;
            int                 arg2    = 3;
            do {
                try {
                    if (generator.isNestedWidthPresent()) {
                        if (generator.isNestedPrecisionPresent()) {
                            (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                        }
                        else {
                            (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                        }
                    }
                    else if (generator.isNestedPrecisionPresent()) {
                        (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                    }
                    else {
                        (void)std::vformat(
                                     spec.c_str(),
                                     std::make_wformat_args(arg0, arg1, arg2));
                    }

                     // Verify that we do not miss states that are considered
                     // valid by the standard implementation.

                     ASSERTV(counter,
                             spec.c_str(),
                             generator.isStateValidForParse(),
                             generator.isStateValidForFormat(),
                             generator.isStateValidForParse() &&
                                 generator.isStateValidForFormat());
                }
                catch (std::format_error& err) {
                    // Verify that if the standard implementation considers
                    // the specification invalid then the generator considers
                    // it invalid too.

                    ASSERTV(counter,
                            spec.c_str(),
                            err.what(),
                            generator.isStateValidForParse(),
                            generator.isStateValidForFormat(),
                            !generator.isStateValidForParse() ||
                                !generator.isStateValidForFormat());
                }
                ++counter;
            } while (generator.next());

            if (veryVerbose) {
                T_ T_ P(counter);
            }
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   According to the practice accepted in the BDE, in this test case
        //   we should test a set of functions sufficient to bring the object
        //   to any possible state.  In this case, we would be forced to test
        //   the constructor and all manipulators in one place.  For
        //   readability, we will test each function in a separate test case.
        //
        //   The second problem is that almost all accessors except one throw
        //   exceptions if we call them for a newly created object.  We will
        //   test them and this behavior in the next test.  And here we will
        //   check the state of the object, using the (yet untested)
        //   `processingState` accessor only.
        //
        // Concerns:
        //: 1 The newly created object has unparsed state.
        //
        // Plan:
        //: 1 Create an object and verify its state using `processingState`
        //:   accessor.  (C-1)
        //
        // Testing:
        //   bslfmt::FormatSpecificationParser();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        CharParser  charParser;
        ASSERTV(charParser.processingState(),
                Enums::e_STATE_UNPARSED == charParser.processingState());

        WcharParser wcharParser;
        ASSERTV(wcharParser.processingState(),
                Enums::e_STATE_UNPARSED == wcharParser.processingState());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Invoke public methods of class being tested and verify the
        //:   results.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        checkStandard(L_,
                      parseStandard(""),
                      " ",
                      CharParser::e_ALIGN_DEFAULT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      false,
                      "");

        checkStandard(L_,
                      parseStandard("*<06.3XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      true,
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 3),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      "XYZ");

        checkStandard(L_,
                      parseStandard("*<{}.{}XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 0 ),
                      NumericValue(NumericValue::e_VALUE,  99),
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      false,
                      "XYZ");

        checkStandard(L_,
                      parseStandard("*<{1}.{2}XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      NumericValue(NumericValue::e_ARG_ID, 2 ),
                      NumericValue(NumericValue::e_VALUE,  97),
                      false,
                      "XYZ");

        checkStandard(L_,
                      parseStandard(L""),
                      L" ",
                      WcharParser::e_ALIGN_DEFAULT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      false,
                      L"");

        checkStandard(L_,
                      parseStandard(L"*<06.3XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      true,
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 3),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      L"XYZ");

        checkStandard(L_,
                      parseStandard(L"*<{}.{}XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 0 ),
                      NumericValue(NumericValue::e_VALUE,  99),
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      false,
                      L"XYZ");

        checkStandard(L_,
                      parseStandard(L"*<{1}.{2}XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      NumericValue(NumericValue::e_ARG_ID, 2 ),
                      NumericValue(NumericValue::e_VALUE,  97),
                      false,
                      L"XYZ");

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
// Copyright 2025 Bloomberg Finance L.P.
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
