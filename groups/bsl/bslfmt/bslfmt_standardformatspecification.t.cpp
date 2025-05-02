// bslfmt_standardformatspecification.t.cpp                           -*-C++-*-
#include <bslfmt_standardformatspecification.h>

#include <bslfmt_format_string.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>
#include <bslfmt_testspecificationgenerator.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bslstl_stringview.h>

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
// if they are called for a newly created object.  So in the third test case
// (Basic Accessors) we have to use (yet untested) `parse` and `postprocess`
// manipulators to modify object from the default state.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bslfmt::StandardFormatSpecification();
//
// MANIPULATORS
// [4] void parse(t_PARSE_CONTEXT *parseContext, Category category);
// [5] void postprocess(const t_FORMAT_CONTEXT& context);
//
// ACCESSORS
// [3] const t_CHAR *filler() const;
// [3] int numFillerCharacters() const;
// [3] int fillerCodePointDisplayWidth() const;
// [3] Alignment alignment() const;
// [3] Sign sign() const;
// [3] bool alternativeFlag() const;
// [3] bool zeroPaddingFlag() const;
// [3] const NumericValue postprocessedWidth() const;
// [3] const NumericValue postprocessedPrecision() const;
// [3] bool localeSpecificFlag() const;
// [3] FormatType formatType() const;
// [3] ProcessingState processingState() const;
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
//                  ADDITIONAL MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

// Due to a member initialization bug for types returned from consteval
// functions in clang 15, we do the testing on a non-consteval basis for clang
// 15 and earlier.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20) &&                 \
    (!defined(BSLS_PLATFORM_CMP_CLANG) || BSLS_PLATFORM_CMP_VERSION >=        \
                                              (16 * 10000))
    #define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
    #define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    #define BSLFMT_FORMAT_STRING_PARAMETER  bslfmt::format_string<>
    #define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#else
    // We cannot define format_string<t_ARGS...> in a C++03 compliant manner,
    // so have to use non-template versions instead.
    #define BSLFMT_FORMAT_STRING_PARAMETER  bslfmt::format_string
    #define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif

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
} while (false);

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef StandardFormatSpecification<char>    CharSpec;
typedef StandardFormatSpecification<wchar_t> WcharSpec;
typedef FormatterSpecificationNumericValue   NumericValue;
typedef FormatSpecificationParserEnums       Enums;

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

BSLFMT_FORMATTER_TEST_CONSTEVAL CharSpec parseStandard(
                             BSLFMT_FORMAT_STRING_PARAMETER inputSpecification,
                             CharSpec::Category             category)
{
    CharSpec                       spec;
    bsl::string_view               input(inputSpecification.get());
    const int                      numArgs = 4;
    bslfmt::MockParseContext<char> context(input, numArgs);

    spec.parse(&context, category);

    return spec;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL WcharSpec parseStandard(
                            BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification,
                            WcharSpec::Category             category)
{
    WcharSpec                         spec;
    bsl::wstring_view                 input(inputSpecification.get());
    const int                         numArgs = 4;
    bslfmt::MockParseContext<wchar_t> context(input, numArgs);

    spec.parse(&context, category);

    return spec;
}

void postprocessAndVerifyResult(
                 int                                    line,
                 const CharSpec&                        originalSpec,
                 const bslfmt::MockFormatContext<char>& mfc,
                 bsl::basic_string_view<char>           filler,
                 CharSpec::Alignment                    alignment,
                 CharSpec::Sign                         sign,
                 bool                                   alternativeFlag,
                 bool                                   zeroPaddingFlag,
                 FormatterSpecificationNumericValue     postprocessedWidth,
                 FormatterSpecificationNumericValue     postprocessedPrecision,
                 bool                                   localeSpecificFlag,
                 CharSpec::FormatType                   formatType)
{
    CharSpec spec = originalSpec;
    spec.postprocess(mfc);

    ASSERTV(line, filler                 == bsl::string_view(
                                                  spec.filler(),
                                                  spec.numFillerCharacters()));
    ASSERTV(line, alignment              == spec.alignment());
    ASSERTV(line, sign                   == spec.sign());
    ASSERTV(line, alternativeFlag        == spec.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == spec.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth     == spec.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == spec.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == spec.localeSpecificFlag());
    ASSERTV(line, formatType             == spec.formatType());
}

void postprocessAndVerifyResult(
              int                                       line,
              const WcharSpec&                          originalSpec,
              const bslfmt::MockFormatContext<wchar_t>& mfc,
              bsl::basic_string_view<wchar_t>           filler,
              WcharSpec::Alignment                      alignment,
              WcharSpec::Sign                           sign,
              bool                                      alternativeFlag,
              bool                                      zeroPaddingFlag,
              FormatterSpecificationNumericValue        postprocessedWidth,
              FormatterSpecificationNumericValue        postprocessedPrecision,
              bool                                      localeSpecificFlag,
              WcharSpec::FormatType                     formatType)
{
    WcharSpec                          spec = originalSpec;
    spec.postprocess(mfc);

    ASSERTV(line, filler                 == bsl::wstring_view(
                                                  spec.filler(),
                                                  spec.numFillerCharacters()));
    ASSERTV(line, alignment              == spec.alignment());
    ASSERTV(line, sign                   == spec.sign());
    ASSERTV(line, alternativeFlag        == spec.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == spec.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth     == spec.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == spec.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == spec.localeSpecificFlag());
    ASSERTV(line, formatType             == spec.formatType());
}

/// Verify the state of the specified `specification` after parsing with the
/// specified `category` the spec generated by the specified `generator` in its
/// current state.
template <class t_CHAR>
void verifyParsedState(
          const StandardFormatSpecification<t_CHAR>&             specification,
          const TestSpecificationGenerator<t_CHAR>&              generator,
          typename StandardFormatSpecification<t_CHAR>::Category category)
{
    typedef TestSpecificationGenerator<t_CHAR>  Generator;
    typedef StandardFormatSpecification<t_CHAR> Obj;

    const t_CHAR *const SPEC = generator.spec().c_str();
    ASSERTV(SPEC, Enums::e_STATE_PARSED == specification.processingState());

    // Fill and align
    if (generator.isFillCharacterPresent()) {
        ASSERTV(SPEC,
                generator.fillCharacter(),
                *specification.filler(),
                generator.fillCharacter() == *specification.filler());
    }
    else {
        ASSERTV(SPEC, *specification.filler(), ' ' == *specification.filler());
    }
    ASSERTV(SPEC,
            specification.numFillerCharacters(),
            1 == specification.numFillerCharacters());

    if (generator.isAlignOptionPresent()) {
        ASSERTV(SPEC,
                generator.alignment(),
                specification.alignment(),
                static_cast<int>(generator.alignment()) ==
                    static_cast<int>(specification.alignment()));
    }
    else {
        ASSERTV(SPEC,
                specification.alignment(),
                Enums::e_ALIGN_DEFAULT == specification.alignment());
    }

    // Sign
    if (generator.isSignOptionPresent()) {
        ASSERTV(SPEC,
                generator.sign(),
                specification.sign(),
                static_cast<int>(generator.sign()) ==
                    static_cast<int>(specification.sign()));
    }
    else {
        ASSERTV(SPEC,
                specification.sign(),
                Enums::e_SIGN_DEFAULT == specification.sign());
    }

    // Hash
    ASSERTV(
           SPEC,
           generator.isHashOptionPresent() == specification.alternativeFlag());

    // Zero
    ASSERTV(
           SPEC,
           generator.isZeroOptionPresent() == specification.zeroPaddingFlag());

    ASSERTV(SPEC,
            generator.isLocaleOptionPresent() ==
                specification.localeSpecificFlag());

    // Format type
    if (generator.isTypeOptionPresent()) {
        switch (category) {
          case Obj::e_CATEGORY_STRING: {
            switch (generator.type()) {
              case Generator::e_TYPE_STRING: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_STRING_DEFAULT == specification.formatType());
              } break;
              case Generator::e_TYPE_ESCAPED: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_STRING_ESCAPED == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected string type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_INTEGRAL: {
            switch (generator.type()) {
              case Generator::e_TYPE_BINARY: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_BINARY == specification.formatType());
              } break;
              case Generator::e_TYPE_BINARY_UC: {
                ASSERTV(
                      SPEC,
                      category,
                      specification.formatType(),
                      Obj::e_INTEGRAL_BINARY_UC == specification.formatType());
              } break;
              case Generator::e_TYPE_CHARACTER: {
                ASSERTV(
                      SPEC,
                      category,
                      specification.formatType(),
                      Obj::e_INTEGRAL_CHARACTER == specification.formatType());
              } break;
              case Generator::e_TYPE_DECIMAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_DECIMAL == specification.formatType());
              } break;
              case Generator::e_TYPE_OCTAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_OCTAL == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX_UC == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected integral type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_CHARACTER: {
            switch (generator.type()) {
              case Generator::e_TYPE_BINARY: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_BINARY == specification.formatType());
              } break;
              case Generator::e_TYPE_BINARY_UC: {
                ASSERTV(
                      SPEC,
                      category,
                      specification.formatType(),
                      Obj::e_INTEGRAL_BINARY_UC == specification.formatType());
              } break;
              case Generator::e_TYPE_CHARACTER: {
                ASSERTV(
                     SPEC,
                     category,
                     specification.formatType(),
                     Obj::e_CHARACTER_CHARACTER == specification.formatType());
              } break;
              case Generator::e_TYPE_ESCAPED: {
                ASSERTV(
                       SPEC,
                       category,
                       specification.formatType(),
                       Obj::e_CHARACTER_ESCAPED == specification.formatType());
              } break;
              case Generator::e_TYPE_DECIMAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_DECIMAL == specification.formatType());
              } break;
              case Generator::e_TYPE_OCTAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_OCTAL == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX_UC == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected character type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_BOOLEAN: {
            switch (generator.type()) {
              case Generator::e_TYPE_BINARY: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_BINARY == specification.formatType());
              } break;
              case Generator::e_TYPE_BINARY_UC: {
                ASSERTV(
                      SPEC,
                      category,
                      specification.formatType(),
                      Obj::e_INTEGRAL_BINARY_UC == specification.formatType());
              } break;
              case Generator::e_TYPE_STRING: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_BOOLEAN_STRING == specification.formatType());
              } break;
              case Generator::e_TYPE_DECIMAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_DECIMAL == specification.formatType());
              } break;
              case Generator::e_TYPE_OCTAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_OCTAL == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX == specification.formatType());
              } break;
              case Generator::e_TYPE_INT_HEX_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_INTEGRAL_HEX_UC == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected boolean type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_FLOATING: {
            switch (generator.type()) {
              case Generator::e_TYPE_FLOAT_HEX: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_FLOATING_HEX == specification.formatType());
              } break;
              case Generator::e_TYPE_FLOAT_HEX_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_FLOATING_HEX_UC == specification.formatType());
              } break;
              case Generator::e_TYPE_SCIENTIFIC: {
                ASSERTV(
                     SPEC,
                     category,
                     specification.formatType(),
                     Obj::e_FLOATING_SCIENTIFIC == specification.formatType());
              } break;
              case Generator::e_TYPE_SCIENTIFIC_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_FLOATING_SCIENTIFIC_UC ==
                            specification.formatType());
              } break;
              case Generator::e_TYPE_FIXED: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_FLOATING_FIXED == specification.formatType());
              } break;
              case Generator::e_TYPE_FIXED_UC: {
                ASSERTV(
                       SPEC,
                       category,
                       specification.formatType(),
                       Obj::e_FLOATING_FIXED_UC == specification.formatType());
              } break;
              case Generator::e_TYPE_GENERAL: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_FLOATING_GENERAL == specification.formatType());
              } break;
              case Generator::e_TYPE_GENERAL_UC: {
                ASSERTV(
                     SPEC,
                     category,
                     specification.formatType(),
                     Obj::e_FLOATING_GENERAL_UC == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected floating type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_POINTER: {
            switch (generator.type()) {
              case Generator::e_TYPE_POINTER: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_POINTER_HEX == specification.formatType());
              } break;
              case Generator::e_TYPE_POINTER_UC: {
                ASSERTV(SPEC,
                        category,
                        specification.formatType(),
                        Obj::e_POINTER_HEX_UC == specification.formatType());
              } break;
              default: {
                ASSERTV("Unexpected floating type", false);
              }
            }
          } break;

          case Obj::e_CATEGORY_UNASSIGNED: {
            ASSERTV(SPEC, "Unassigned category", false);
          } break;
        }
    }
    else {
        switch (category) {
          case Obj::e_CATEGORY_STRING: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_STRING_DEFAULT == specification.formatType());
          } break;

          case Obj::e_CATEGORY_INTEGRAL: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_INTEGRAL_DECIMAL == specification.formatType());
          } break;

          case Obj::e_CATEGORY_CHARACTER: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_CHARACTER_CHARACTER == specification.formatType());
          } break;

          case Obj::e_CATEGORY_BOOLEAN: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_BOOLEAN_STRING == specification.formatType());
          } break;

          case Obj::e_CATEGORY_FLOATING: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_FLOATING_DEFAULT == specification.formatType());
          } break;

          case Obj::e_CATEGORY_POINTER: {
            ASSERTV(SPEC,
                    category,
                    specification.formatType(),
                    Obj::e_POINTER_HEX == specification.formatType());
          } break;

          case Obj::e_CATEGORY_UNASSIGNED: {
            ASSERTV(SPEC, "Unassigned category", false);
          } break;
        }
    }
}

/// Verify the state of the specified `specification` after postprocessing the
/// format spec generated for the current state of the specified `generator`
/// with the specified `arg0`, 'arg1' and 'arg2' passed as parameters to the
/// format context constructor.
template <class t_CHAR>
void verifyPostprocessedState(
                      const StandardFormatSpecification<t_CHAR>& specification,
                      const TestSpecificationGenerator<t_CHAR>&  generator,
                      int                                        arg0,
                      int                                        arg1,
                      int                                        arg2)
{
    const t_CHAR * const SPEC = generator.spec().c_str();
    ASSERTV(SPEC,
            Enums::e_STATE_POSTPROCESSED == specification.processingState());

    // As generator does not support multibyte unicode symbols, we
    // expect that the filler takes only one byte.

    ASSERTV(SPEC, specification.fillerCodePointDisplayWidth(),
            1 == specification.fillerCodePointDisplayWidth());

    // Testing postprocessed width

    NumericValue postprocessedWidth = specification.postprocessedWidth();

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
              ASSERTV("Unexpected nested width variant",
                      generator.nestedWidthVariant(),
                      false);
          }
        }
    }

    NumericValue postprocessedPrecision =
                                        specification.postprocessedPrecision();

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
              ASSERTV("Unexpected nested precision variant",
                      generator.nestedPrecisionVariant(),
                      false);
          }
        }
    }
}

// ============================================================================
//                            TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

// This template struct provides a namespace for implementations of test
// functions that verify the correctness of the `StandardFormatSpecification`
// methods.
template <class t_CHAR>
struct TestDriver {

    // CLASS METHODS
    /// Test `postprocess` method using test specification generator.
    static void testCase5(bool veryVerbose);

    /// Test `parse` method using test specification generator.
    static void testCase4(bool veryVerbose);
};

                                // ----------
                                // TEST CASES
                                // ----------
template <class t_CHAR>
void TestDriver<t_CHAR>::testCase5(bool veryVerbose)
{
    typedef StandardFormatSpecification<t_CHAR> Obj;

    bslfmt::TestSpecificationGenerator<t_CHAR> generator;

    // `postprocess` function works with fill character, width and precision.
    // To avoid generating a lot of invalid specifications from the parsing
    // point of view and then ignoring them, we add only the necessary commands
    // to the generator instruction:
    // 'F' - command indicating if fill characters must be added to the
    //       produced specifications.
    // '^' - command indicating if alignment option must be added to the
    //       generated specifications.
    // '{' - command indicating if nested width option must be added to the
    //       produced specifications.
    // '}' - command indicating if nested precision option must be added to the
    //       produced specifications.
    // 's' - command indicating if string presentation type (i.e. `s`) must be
    //       present in the generated specifications.
    // The remaining commands either conflict with the string type (sign, hash,
    // zero) or do not give any load to the `postprocess` function and were
    // tested in the test for `parse`.

    generator.setup("F^{}s");
    const bsl::basic_string<t_CHAR>& spec    = generator.spec();
    int                              counter = 0;

    do {
        const int                           arg0 = 90;
        const int                           arg1 = 91;
        const int                           arg2 = 92;
        StandardFormatSpecification<t_CHAR> specification;
        bsl::basic_string_view<t_CHAR>      input(spec.c_str(), spec.length());
        bslfmt::MockParseContext<t_CHAR>    parseContext(input, 3);
        bslfmt::MockFormatContext<t_CHAR>   formatContext(arg0, arg1, arg2);

        specification.parse(&parseContext, Obj::e_CATEGORY_STRING);

        try {
              specification.postprocess(formatContext);
          }
          catch (const bsl::format_error& err) {
              ASSERTV(spec.c_str(), err.what(), false);
          }
          ASSERT(Enums::e_STATE_POSTPROCESSED ==
                 specification.processingState());

          verifyPostprocessedState(specification, generator, arg0, arg1, arg2);
          ++counter;
    } while (generator.nextValidForParse());

    if (veryVerbose) { T_ T_ P(counter); }
}

template <class t_CHAR>
void TestDriver<t_CHAR>::testCase4(bool veryVerbose)
{
    typedef StandardFormatSpecification<t_CHAR> Obj;
    static const struct {
          typename Obj::Category  d_category;       // parse category
          const char             *d_instruction_p;  // expected format type
    } DATA[] = {
        //LINE  SPEC              SECTION          REMAINING SPEC
        //----  --------------    ---------------  --------------
        {Obj::e_CATEGORY_STRING,    "VF^+#0{}s"},
        {Obj::e_CATEGORY_INTEGRAL,  "VF^+#0{i" },
        {Obj::e_CATEGORY_CHARACTER, "VF^+#0{c" },
        {Obj::e_CATEGORY_BOOLEAN,   "VF^+#0{b" },
        {Obj::e_CATEGORY_FLOATING,  "VF^+#0{}f"},
        {Obj::e_CATEGORY_POINTER,   "VF^+#0{p" },
    };

    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;
    for (size_t i = 0; i < NUM_DATA; ++i) {
          const typename Obj::Category  CATEGORY    = DATA[i].d_category;
          const char                   *INSTRUCTION = DATA[i].d_instruction_p;

          TestSpecificationGenerator<t_CHAR> generator;
          generator.setup(INSTRUCTION);
          const bsl::basic_string<t_CHAR>& spec    = generator.spec();
          int                              counter = 0;
          do {
              Obj                              mX;
              bsl::basic_string_view<t_CHAR>   input(spec.c_str(),
                                                     spec.length());
              bslfmt::MockParseContext<t_CHAR> context(input, 3);

              try {
                  mX.parse(&context, CATEGORY);
                  verifyParsedState(mX, generator, CATEGORY);
              }
              catch (const bsl::format_error& err) {
                  if (generator.isStateValidForParse() &&
                      generator.isTypeOptionPresent()) {
                      // The generator cannot take the category into account
                      // when checking the resulting string for validity.
                      // Therefore, strings that do not contain an explicitly
                      // specified presentation type, but contain a sign, hash,
                      // or a null option, are considered valid for parsing.
                      // However, depending on the category, these strings may
                      // be invalid and we expect the parsing function to throw
                      // an exception in this case.

                      ASSERTV(CATEGORY, spec.c_str(), err.what(), false);
                  }
              }
              ++counter;
          } while (generator.next());

          if (veryVerbose) {
              T_ T_ P_(INSTRUCTION) P(counter);
          }
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
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `postprocess`
        //
        // Concerns:
        //: 1 The `postprocess` method correctly handles any unicode fill
        //:   character.
        //:
        //: 2 The `postprocess` method modifies width and precision attributes
        //:   in accordance with the arguments passed to the format context.
        //:
        //: 3 The `postprocess` method throws exceptions if called before
        //:   `parse` method or in case of invalid width and precision values
        //:   passed in the format context object.
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
        //: 3 Invoke `postprocess` without calling the `parse` method first and
        //:   verify that the exception with the expected message is thrown.
        //:
        //: 4 Invoke `postprocess` method with invalid width (non-positive)
        //:   and precision (negative) values and verify that the exceptions
        //:   with the expected messages are thrown.  (C-3)
        //:
        //: 5 Use test specification generator to produce a huge amount of
        //:   input and thoroughly test the basic functionality of the
        //:   `postprocess` method.  (C-2)
        //
        // Testing:
        //   void postprocess(const t_FORMAT_CONTEXT& context);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `postprocess`"
                            "\n=====================\n");

        ASSERTV(sizeof(wchar_t), 4 == sizeof(wchar_t) || 2 == sizeof(wchar_t));

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

                bsl::string spec = FILL_SYMBOL;
                spec.push_back('^');

                CharSpec                        mX;
                bslfmt::MockParseContext<char>  parseContext(spec, 0);
                bslfmt::MockFormatContext<char> formatContext(90, 91, 92);

                mX.parse(&parseContext, CharSpec::e_CATEGORY_INTEGRAL);

                try {
                    mX.postprocess(formatContext);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(LINE, spec.c_str(), err.what(), false);
                }

                ASSERTV(LINE, WIDTH, mX.fillerCodePointDisplayWidth(),
                        WIDTH == mX.fillerCodePointDisplayWidth());
            }

            // In the following table values from the `UTF_32` column are used
            // only if the size of `wchar_t` is 4 bytes.  But on Windows the
            // size of `wchar_t` is 2 bytes, so MSVC throws a warning when we
            // try to assign a value to a variable that is too big to fit in
            // 2 bytes.  To suppress this warning we have to use explicit
            // conversion.

            const wchar_t x108601 = static_cast<wchar_t>(0x108601);
            const wchar_t x10ffff = static_cast<wchar_t>(0x10ffff);

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
                { L_,   { 0xd834, 0xdd1e },  2,   1,   {  x108601 },  1,   1 },
                { L_,   { 0xd83c, 0xdc31 },  2,   1,   {  x10ffff },  1,   1 },
            };
            const size_t NUM_WDATA = sizeof WDATA / sizeof *WDATA;

            // `wchar_t`

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

                bsl::wstring spec(SPEC, LENGTH);
                spec.push_back('^');

                WcharSpec                         mX;
                bslfmt::MockParseContext<wchar_t> parseContext(spec, 0);
                bslfmt::MockFormatContext<char>   formatContext(90, 91, 92);

                mX.parse(&parseContext, WcharSpec::e_CATEGORY_INTEGRAL);

                try {
                    mX.postprocess(formatContext);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(LINE, err.what(), false);
                }

                ASSERTV(LINE, WIDTH, mX.fillerCodePointDisplayWidth(),
                        WIDTH == mX.fillerCodePointDisplayWidth());
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

                if (veryVerbose) { T_ T_ P(SPEC); }

                for (int j = 1; j <= 10; ++j) {
                    const int                       ARGS[] = { 0 + j,
                                                               1 + j,
                                                               2 + j };
                    CharSpec                        mX;
                    bslfmt::MockParseContext<char>  parseContext(SPEC, 3);
                    bslfmt::MockFormatContext<char> formatContext(ARGS[0],
                                                                  ARGS[1],
                                                                  ARGS[2]);

                    mX.parse(&parseContext, CharSpec::e_CATEGORY_FLOATING);

                    try {
                        mX.postprocess(formatContext);
                    }
                    catch (const bsl::format_error& err) {
                        ASSERTV(LINE, SPEC, err.what(), false);
                    }

                    NumericValue width     = mX.postprocessedWidth();
                    NumericValue precision = mX.postprocessedPrecision();

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

        if (verbose)
            printf("\tNegative Testing\n");
        {
            // Table abbreviations
            const char *INVALID_WIDTH     = "Zero or negative width value";
            const char *NESTED_VALUE_OUT_OF_RANGE =
                                          "Nested value argument out of range";
            const char *UNPARSED_STATE =
                           "Format standard specification '.parse' not called";

            // Testing unparsed state
            {
                const bsl::string_view          EXPECTED = UNPARSED_STATE;
                CharSpec                        mX;
                bslfmt::MockFormatContext<char> formatContext(0);
                try {
                    mX.postprocess(formatContext);
                    ASSERTV("Exception has not been thrown", false);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(err.what(),
                            EXPECTED == bsl::string_view(err.what()));
                }
            }

            {
                const bsl::string_view             EXPECTED = UNPARSED_STATE;
                WcharSpec                          mX;
                bslfmt::MockFormatContext<wchar_t> formatContext(0);
                try {
                    mX.postprocess(formatContext);
                    ASSERTV("Exception has not been thrown", false);
                }
                catch (const bsl::format_error& err) {
                    ASSERTV(err.what(),
                            EXPECTED == bsl::string_view(err.what()));
                }
            }

            // Testing non-positive width and negative precision
            static const struct {
                int         d_line;        // source line number
                int         d_arg1;        // first argument value
                int         d_arg2;        // second argument value
                const char *d_expected_p;  // expected error message
            } DATA[] = {
                //LINE  ARG_1    ARG_2  MESSAGE
                //----  -----    -----  ----------------
                { L_,   -1,       1,    NESTED_VALUE_OUT_OF_RANGE },
                { L_,    0,       1,    INVALID_WIDTH             },
                { L_,    1,      -1,    NESTED_VALUE_OUT_OF_RANGE }
            };

            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            const char    *SPEC  =  "{1}.{2}";
            const wchar_t *WSPEC = L"{1}.{2}";
            const int      ARG_0 = 42;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int              LINE     = DATA[i].d_line;
                const int              ARG_1    = DATA[i].d_arg1;
                const int              ARG_2    = DATA[i].d_arg2;
                const bsl::string_view EXPECTED = DATA[i].d_expected_p;
                    // char
                {
                    CharSpec                        mX;
                    bslfmt::MockParseContext<char>  parseContext(SPEC, 3);
                    bslfmt::MockFormatContext<char> formatContext(ARG_0,
                                                                  ARG_1,
                                                                  ARG_2);

                    mX.parse(&parseContext, CharSpec::e_CATEGORY_FLOATING);

                    try {
                         mX.postprocess(formatContext);
                         ASSERTV(LINE, "Exception has not been thrown", false);
                    }
                    catch (const bsl::format_error& err) {
                         ASSERTV(LINE,
                                 err.what(),
                                 EXPECTED == bsl::string_view(err.what()));
                    }
                }
                // wchar_t
                {
                    WcharSpec                          mX;
                    bslfmt::MockParseContext<wchar_t>  parseContext(WSPEC, 3);
                    bslfmt::MockFormatContext<wchar_t> formatContext(ARG_0,
                                                                     ARG_1,
                                                                     ARG_2);

                    mX.parse(&parseContext, WcharSpec::e_CATEGORY_FLOATING);

                    try {
                         mX.postprocess(formatContext);
                         ASSERTV(LINE, "Exception has not been thrown", false);
                    }
                    catch (const bsl::format_error& err) {
                         ASSERTV(LINE,
                                 err.what(),
                                 EXPECTED == bsl::string_view(err.what()));
                    }
                }
            }
        }

        if (verbose) printf("\tTesting basic functionality using generator\n");

        TestDriver<char   >::testCase5(veryVerbose);
        TestDriver<wchar_t>::testCase5(veryVerbose);
      } break;
      case 4: {
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
        //:   to the passed value category.
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
        //   void parse(t_PARSE_CONTEXT *parseContext, Category category);
        // --------------------------------------------------------------------

        if (verbose)
             printf("\nTESTING `parse`"
                    "\n===============\n");

        if (verbose) printf("\tTesting basic functionality using generator\n");

        TestDriver<char   >::testCase4(veryVerbose);
        TestDriver<wchar_t>::testCase4(veryVerbose);

        if (verbose) printf("\tTesting correct presentation type handling\n");
        {
            typedef CharSpec CS;

             const char *specs[] = {
                 "=<{0}L",
                 "*^{}",
                 "!>10",
                 "$<",
                 "^"
             };
             const size_t numSpecs = sizeof specs / sizeof *specs;

            static const struct {
                int                  d_line;      // source line number
                CharSpec::Category   d_category;  // parse category
                CharSpec::FormatType d_expected;  // expected format type
            } DFLT_DATA[] = {
                //LINE  CATEGORY                  EXPECTED_TYPE
                //----  ------------------------  --------------------------
                { L_,   CS::e_CATEGORY_STRING,    CS::e_STRING_DEFAULT      },
                { L_,   CS::e_CATEGORY_INTEGRAL,  CS::e_INTEGRAL_DECIMAL    },
                { L_,   CS::e_CATEGORY_CHARACTER, CS::e_CHARACTER_CHARACTER },
                { L_,   CS::e_CATEGORY_BOOLEAN,   CS::e_BOOLEAN_STRING      },
                { L_,   CS::e_CATEGORY_FLOATING,  CS::e_FLOATING_DEFAULT    },
                { L_,   CS::e_CATEGORY_POINTER,   CS::e_POINTER_HEX         },
            };

            const size_t NUM_DFLT_DATA = sizeof DFLT_DATA / sizeof *DFLT_DATA;

            static const struct {
                CharSpec::Category   d_category;  // parse category

                char                 d_type;      // format specification type
                                                  // option

                CharSpec::FormatType d_expected;  // expected format type
            } PARSE_DATA[] = {
                // CATEGORY                 TYPE  EXPECTED
                //------------------------  ----  -------------------------
                { CS::e_CATEGORY_STRING,    '?',  CS::e_STRING_ESCAPED      },
                { CS::e_CATEGORY_STRING,    'B',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_STRING,    'c',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_STRING,    'o',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_STRING,    'A',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_STRING,    'P',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_INTEGRAL,  '?',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_INTEGRAL,  'B',  CS::e_INTEGRAL_BINARY_UC  },
                { CS::e_CATEGORY_INTEGRAL,  'c',  CS::e_INTEGRAL_CHARACTER  },
                { CS::e_CATEGORY_INTEGRAL,  'o',  CS::e_INTEGRAL_OCTAL      },
                { CS::e_CATEGORY_INTEGRAL,  'A',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_INTEGRAL,  'P',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_CHARACTER, '?',  CS::e_CHARACTER_ESCAPED   },
                { CS::e_CATEGORY_CHARACTER, 'B',  CS::e_INTEGRAL_BINARY_UC  },
                { CS::e_CATEGORY_CHARACTER, 'c',  CS::e_CHARACTER_CHARACTER },
                { CS::e_CATEGORY_CHARACTER, 'o',  CS::e_INTEGRAL_OCTAL      },
                { CS::e_CATEGORY_CHARACTER, 'A',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_CHARACTER, 'P',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_BOOLEAN,   '?',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_BOOLEAN,   'B',  CS::e_INTEGRAL_BINARY_UC  },
                { CS::e_CATEGORY_BOOLEAN,   'c',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_BOOLEAN,   'o',  CS::e_INTEGRAL_OCTAL      },
                { CS::e_CATEGORY_BOOLEAN,   'A',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_BOOLEAN,   'P',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_FLOATING,  '?',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_FLOATING,  'B',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_FLOATING,  'c',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_FLOATING,  'o',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_FLOATING,  'A',  CS::e_FLOATING_HEX_UC     },
                { CS::e_CATEGORY_FLOATING,  'P',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   '?',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   'B',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   'c',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   'o',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   'A',  CS::e_TYPE_UNASSIGNED     },
                { CS::e_CATEGORY_POINTER,   'P',  CS::e_POINTER_HEX_UC      },
            };

            const size_t NUM_PARSE_DATA = sizeof PARSE_DATA /
                                          sizeof *PARSE_DATA;

            for (size_t i = 0; i < numSpecs; ++i) {
                const bsl::string_view SPEC = specs[i];

                // Testing unassigned category
                {
                    CharSpec                       mXc;
                    bslfmt::MockParseContext<char> context(SPEC, 1);
                    try {
                        mXc.parse(&context, CS::e_CATEGORY_UNASSIGNED);
                        ASSERTV(SPEC.data(),
                                "Exception has not been thrown",
                                false);
                    }
                    catch (bsl::format_error& err) {
                    }
                }

                // Testing default format types
                for (size_t j = 0; j < NUM_DFLT_DATA; ++j) {
                    const int            LINE     = DFLT_DATA[j].d_line;
                    const CS::Category   CATEGORY = DFLT_DATA[j].d_category;
                    const CS::FormatType EXPECTED = DFLT_DATA[j].d_expected;

                    CharSpec                       mXc;
                    bslfmt::MockParseContext<char> context(SPEC, 1);

                    try {
                        mXc.parse(&context, CATEGORY);
                    }
                    catch (bsl::format_error& err) {
                        ASSERTV(LINE, SPEC.data(), CATEGORY, err.what(),
                                false);
                    }

                    ASSERTV(LINE, SPEC.data(), CATEGORY, EXPECTED,
                            mXc.formatType(),
                            EXPECTED == mXc.formatType());
                }

                // Testing parsing of format type from format specification
                for (size_t j = 0; j < NUM_PARSE_DATA; ++j) {
                    const CS::Category   CATEGORY = PARSE_DATA[j].d_category;
                    const char           TYPE     = PARSE_DATA[j].d_type;
                    const CS::FormatType EXPECTED = PARSE_DATA[j].d_expected;

                    // Assembling full format specification
                    bsl::string fullSpec = SPEC.data();
                    fullSpec.push_back(TYPE);

                    CharSpec                       mXc;
                    bslfmt::MockParseContext<char> context(fullSpec, 1);

                    try {
                        mXc.parse(&context, CATEGORY);
                        ASSERTV(CATEGORY,
                                fullSpec.data(),
                                EXPECTED,
                                mXc.formatType(),
                                EXPECTED == mXc.formatType());
                    }
                    catch (bsl::format_error& err) {
                        ASSERTV(CATEGORY,
                                fullSpec.data(),
                                EXPECTED,
                                err.what(),
                                CS::e_TYPE_UNASSIGNED == EXPECTED);
                    }
                }
            }
        }

        if (verbose) printf("\tTesting hash, sign and zero options parsing\n");
        {
            typedef CharSpec CS;

            const bsl::string_view SUCCESS             = "";
            const bsl::string_view HASH_SIGN_EXCEPTION =
                                              "Hash / sign option requires an "
                                              "arithmetic presentation type";
            const bsl::string_view ZERO_EXCEPTION      =
                        "Zero option requires an arithmetic presentation type";

            static const struct {
                int                     d_line;      // source line number
                CS::Category            d_category;  // specification category
                const char             *d_spec_p;    // parse specification
                const bsl::string_view  d_expected;  // expected parse result
            } DATA[] = {
                // LINE CATEGORY                  SPEC   EXPECTED
                // ---- ------------------------  ----   --------------------
                // Testing ' '
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  " 6c", SUCCESS             },

                { L_,   CS::e_CATEGORY_CHARACTER, " 6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, " 6c", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_POINTER,   " 6p", HASH_SIGN_EXCEPTION },
                { L_,   CS::e_CATEGORY_POINTER,   " 6P", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   " 6s", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_FLOATING,  " 6g", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6G", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6a", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6A", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6e", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6E", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6f", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  " 6F", SUCCESS             },
                // Testing +
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "+6c", SUCCESS             },

                { L_,   CS::e_CATEGORY_CHARACTER, "+6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "+6c", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_POINTER,   "+6p", HASH_SIGN_EXCEPTION },
                { L_,   CS::e_CATEGORY_POINTER,   "+6P", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "+6s", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_FLOATING,  "+6g", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6G", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6a", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6A", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6e", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6E", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6f", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "+6F", SUCCESS             },
                // Testing #
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "#6c", SUCCESS             },

                { L_,   CS::e_CATEGORY_CHARACTER, "#6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "#6c", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_POINTER,   "#6p", HASH_SIGN_EXCEPTION },
                { L_,   CS::e_CATEGORY_POINTER,   "#6P", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6b", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6B", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6d", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6o", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6x", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6X", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "#6s", HASH_SIGN_EXCEPTION },

                { L_,   CS::e_CATEGORY_FLOATING,  "#6g", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6G", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6a", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6A", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6e", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6E", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6f", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "#6F", SUCCESS             },
                // Testing 0
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06b", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06B", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06d", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06o", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06x", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06X", SUCCESS             },
                { L_,   CS::e_CATEGORY_INTEGRAL,  "06c", SUCCESS             },

                { L_,   CS::e_CATEGORY_CHARACTER, "06b", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06B", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06d", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06o", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06x", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06X", SUCCESS             },
                { L_,   CS::e_CATEGORY_CHARACTER, "06c", ZERO_EXCEPTION      },

                { L_,   CS::e_CATEGORY_POINTER,   "06p", SUCCESS             },
                { L_,   CS::e_CATEGORY_POINTER,   "06P", SUCCESS             },

                { L_,   CS::e_CATEGORY_BOOLEAN,   "06b", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06B", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06d", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06o", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06x", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06X", SUCCESS             },
                { L_,   CS::e_CATEGORY_BOOLEAN,   "06s", ZERO_EXCEPTION      },

                { L_,   CS::e_CATEGORY_FLOATING,  "06g", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06G", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06a", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06A", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06e", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06E", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06f", SUCCESS             },
                { L_,   CS::e_CATEGORY_FLOATING,  "06F", SUCCESS             },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int                LINE     = DATA[i].d_line;
                const bsl::string_view   SPEC     = DATA[i].d_spec_p;
                const CharSpec::Category CATEGORY = DATA[i].d_category;
                const bsl::string_view   EXPECTED = DATA[i].d_expected;

                CharSpec                       mXc;
                bslfmt::MockParseContext<char> context(SPEC, 0);
                try {
                    mXc.parse(&context, CATEGORY);
                    ASSERTV(LINE, SPEC.data(), SUCCESS == EXPECTED);
                }
                catch (bsl::format_error& err) {
                            ASSERTV(LINE, SPEC.data(), err.what(),
                                    EXPECTED == bsl::string_view(err.what()));
                }
            }
        }
        
        if (verbose) printf("\tNegative Testing\n");
        {
            // Table abbreviations
             const char *UNASSIGNED_CAT = "Unassigned category";
             const char *INVALID_UNA = "Invalid unassigned category";
             const char *INVALID_STR = "Invalid type for string";
             const char *INVALID_INT = "Invalid type for integers";
             const char *INVALID_CHAR = "Invalid type for character types";
             const char *INVALID_BOOL = "Invalid type for booleans";
             const char *INVALID_FLT = "Invalid type for floating points";
             const char *INVALID_PTR = "Invalid type for pointers";
             const char *BRACKET = "Invalid fill character ('{' or '}')";
             const char *MULTIPLE_CHARACTER =
                                         "Standard types are single-character";
             const char *NON_POSITIVE_WIDTH = "Field widths must be > 0";
             const char *INVALID_PRECISION =
                                 "Invalid precision (no digits following '.')";
             const char *EMPTY_PRECISION =
                                       "Invalid Precision (nothing after '.')";
             const char *INV_PRECISION_TYPE =
                                       "Standard specification parse failure "
                                       "(precision is used with inappropriate "
                                       "type)";
             const char *MIX_INDEXING_M_A = "Cannot mix manual (width) and "
                                            "automatic (precision) indexing";
             const char *MIX_INDEXING_A_M = "Cannot mix automatic (width) and "
                                            "manual (precision) indexing";

             const CharSpec::Category C_UNA  = CharSpec::e_CATEGORY_UNASSIGNED;
             const CharSpec::Category C_STR  = CharSpec::e_CATEGORY_STRING;
             const CharSpec::Category C_INT  = CharSpec::e_CATEGORY_INTEGRAL;
             const CharSpec::Category C_CHAR = CharSpec::e_CATEGORY_CHARACTER;
             const CharSpec::Category C_BOOL = CharSpec::e_CATEGORY_BOOLEAN;
             const CharSpec::Category C_FLT  = CharSpec::e_CATEGORY_FLOATING;
             const CharSpec::Category C_PTR  = CharSpec::e_CATEGORY_POINTER;

             static const struct {
                int                 d_line;        // source line number
                const char         *d_spec_p;      // parse specification
                const wchar_t      *d_wSpec_p;     // parse specification
                CharSpec::Category  d_category;    // specification category
                const char         *d_expected_p;  // expected error message
            } DATA[] = {
                //LINE  CHAR_SPEC   WCHAR_SPEC   CATEGORY MESSAGE
                //----  ---------   -----------  -------- ------------------
                { L_,   "{^",       L"{^",       C_CHAR,  BRACKET            },
                { L_,   "^cd",      L"^cd",      C_CHAR,  MULTIPLE_CHARACTER },
                { L_,   "^00",      L"^00",      C_BOOL,  NON_POSITIVE_WIDTH },
                { L_,   "^3.",      L"^3.",      C_FLT,   EMPTY_PRECISION    },
                { L_,   "^3.-1",    L"^3.-1",    C_FLT,   INVALID_PRECISION  },
                { L_,   "^3.1",     L"^3.1",     C_INT,   INV_PRECISION_TYPE },
                { L_,   "#{}.{1}.", L"#{}.{1}.", C_STR,   MIX_INDEXING_A_M   },
                { L_,   "#{1}.{}.", L"#{1}.{}.", C_FLT,   MIX_INDEXING_M_A   },
                { L_,   "+^",       L"+^",       C_UNA,   UNASSIGNED_CAT     },
                { L_,   "+^d",      L"+^d",      C_UNA,   INVALID_UNA        },
                { L_,   "+^d",      L"+^d",      C_STR,   INVALID_STR        },
                { L_,   "+^s",      L"+^s",      C_INT,   INVALID_INT        },
                { L_,   "+^a",      L"+^a",      C_CHAR,  INVALID_CHAR       },
                { L_,   "+^p",      L"+^p",      C_BOOL,  INVALID_BOOL       },
                { L_,   "+^d",      L"+^d",      C_FLT,   INVALID_FLT        },
                { L_,   "+^?",      L"+^?",      C_PTR,   INVALID_PTR        },
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int                LINE       = DATA[i].d_line;
                const bsl::string_view   SPEC       = DATA[i].d_spec_p;
                const bsl::wstring_view  WSPEC      = DATA[i].d_wSpec_p;
                const CharSpec::Category C_CATEGORY = DATA[i].d_category;
                const bsl::string_view   EXPECTED   = DATA[i].d_expected_p;

                const WcharSpec::Category W_CATEGORY =
                                  static_cast<WcharSpec::Category>(C_CATEGORY);

                CharSpec                       mXc;
                bslfmt::MockParseContext<char> context(SPEC, 0);
                try {
                    mXc.parse(&context, C_CATEGORY);
                    ASSERTV(LINE, SPEC.data(), "Exception has not been thrown",
                            false);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, SPEC.data(), err.what(),
                            EXPECTED == bsl::string_view(err.what()));
                }

                WcharSpec                         mXw;
                bslfmt::MockParseContext<wchar_t> wContext(WSPEC, 0);
                try {
                    mXw.parse(&wContext, W_CATEGORY);
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
                //LINE UNICODE_CHARACTER    LENGTH
                //---- -------------------  ------
                { L_,  "\x01",              1     },  // 1 byte min
                { L_,  "\x48",              1     },  // 1 byte
                { L_,  "\x7f",              1     },  // 1 byte max
                { L_,  "\xc2\x80",          2     },  // 2 byte min
                { L_,  "\xcb\xb1",          2     },  // 2 byte
                { L_,  "\xdf\xbf",          2     },  // 2 byte max
                { L_,  "\xe0\xa0\x80",      3     },  // 3 byte min
                { L_,  "\xe2\x9c\x90",      3     },  // 3 byte
                { L_,  "\xef\xbf\xbf",      3     },  // 3 byte max
                { L_,  "\xf0\x90\x80\x80",  4     },  // 4 byte min
                { L_,  "\xf0\x98\x9a\xa0",  4     },  // 4 byte
                { L_,  "\xf4\x8f\xbf\xbf",  4     },  // 4 byte max
            };
            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            // `char`
            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int     LINE        = DATA[i].d_line;
                const char   *FILL_SYMBOL = DATA[i].d_utf8String;
                const size_t  LENGTH      = DATA[i].d_length;

                bsl::string spec = FILL_SYMBOL;
                ASSERTV(LINE, LENGTH, spec.length(), LENGTH == spec.length());
                spec.push_back('^');

                CharSpec                       mX;
                bslfmt::MockParseContext<char> context(spec, 0);

                try {
                    mX.parse(&context, CharSpec::e_CATEGORY_INTEGRAL);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE, err.what(), false);
                }

                ASSERTV(LINE, mX.filler());
                ASSERTV(LINE, spec[0] == *mX.filler());
                ASSERTV(LINE, LENGTH, mX.numFillerCharacters(),
                        LENGTH ==
                            static_cast<size_t>(mX.numFillerCharacters()));
            }

            ASSERTV(sizeof(wchar_t),
                    4 == sizeof(wchar_t) || 2 == sizeof(wchar_t));

            // In the following table values from the `UTF_32` column are used
            // only if the size of `wchar_t` is 4 bytes.  But on Windows the
            // size of `wchar_t` is 2 bytes, so MSVC throws a warning when we
            // try to assign a value to a variable that is too big to fit in
            // 2 bytes.  To suppress this warning we have to use explicit
            // conversion.

            const wchar_t x108601 = static_cast<wchar_t>(0x108601);
            const wchar_t x10ffff = static_cast<wchar_t>(0x10ffff);

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
                { L_,   { 0xd834, 0xdd1e },  2,   2,   {  x108601 },  1,   1 },
                { L_,   { 0xd83c, 0xdc31 },  2,   2,   {  x10ffff },  1,   1 },
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

                bsl::wstring spec(SPEC, LENGTH);
                spec.push_back('^');

                WcharSpec                         mX;
                bslfmt::MockParseContext<wchar_t> context(spec, 0);

                try {
                    mX.parse(&context, WcharSpec::e_CATEGORY_INTEGRAL);
                }
                catch (bsl::format_error& err) {
                    ASSERTV(LINE,  err.what(), false);
                }

                ASSERTV(LINE, mX.filler());
                ASSERTV(LINE, spec[0] == *mX.filler());
                ASSERTV(LINE, NUM_CHARS, mX.numFillerCharacters(),
                        NUM_CHARS == mX.numFillerCharacters());
            }
        }
      } break;
      case 3: {
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
        //   bool localeSpecificFlag() const;
        //   FormatType formatType() const;
        //   ProcessingState processingState() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        {
            CharSpec mXc;
            ASSERTV(mXc.processingState(),
                    Enums::e_STATE_UNPARSED == mXc.processingState());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.filler()                      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.numFillerCharacters()         )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.alignment()                   )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.sign()                        )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.alternativeFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.zeroPaddingFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedPrecision()      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.localeSpecificFlag()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.formatType()                  )

            bslfmt::MockParseContext<char> parseContext("", 1);
            mXc.parse(&parseContext, CharSpec::e_CATEGORY_INTEGRAL);

            ASSERTV(*mXc.filler(), ' ' == *mXc.filler());
            ASSERTV(mXc.numFillerCharacters(), 1 == mXc.numFillerCharacters());
            ASSERTV(mXc.alignment(),
                    Enums::e_ALIGN_DEFAULT == mXc.alignment());
            ASSERTV(mXc.sign(), Enums::e_SIGN_DEFAULT == mXc.sign());
            ASSERTV(false == mXc.alternativeFlag());
            ASSERTV(false == mXc.zeroPaddingFlag());
            ASSERTV(false == mXc.localeSpecificFlag());
            ASSERTV(mXc.formatType(),
                    CharSpec::e_INTEGRAL_DECIMAL == mXc.formatType());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedPrecision()      )

            MockFormatContext<char> formatContext(0);
            mXc.postprocess(formatContext);

            ASSERTV(mXc.fillerCodePointDisplayWidth(),
                    1 == mXc.fillerCodePointDisplayWidth());
            ASSERTV(mXc.postprocessedWidth().category(),
                    NumericValue::e_DEFAULT ==
                        mXc.postprocessedWidth().category());
            ASSERTV(mXc.postprocessedPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        mXc.postprocessedPrecision().category());
        }
        {
            CharSpec mXc;
            ASSERTV(mXc.processingState(),
                    Enums::e_STATE_UNPARSED == mXc.processingState());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.filler()                      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.numFillerCharacters()         )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.alignment()                   )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.sign()                        )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.alternativeFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.zeroPaddingFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedPrecision()      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.localeSpecificFlag()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.formatType()                  )

            bslfmt::MockParseContext<char> parseContext("*<+#06.5Lg", 1);
            mXc.parse(&parseContext, CharSpec::e_CATEGORY_FLOATING);

            ASSERTV(*mXc.filler(), '*' == *mXc.filler());
            ASSERTV(mXc.numFillerCharacters(), 1 == mXc.numFillerCharacters());
            ASSERTV(mXc.alignment(), Enums::e_ALIGN_LEFT == mXc.alignment());
            ASSERTV(mXc.sign(), Enums::e_SIGN_POSITIVE == mXc.sign());
            ASSERTV(true == mXc.alternativeFlag());
            ASSERTV(true == mXc.zeroPaddingFlag());
            ASSERTV(true == mXc.localeSpecificFlag());
            ASSERTV(mXc.formatType(),
                    CharSpec::e_FLOATING_GENERAL == mXc.formatType());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXc.postprocessedPrecision()      )

            MockFormatContext<char> formatContext(0);
            mXc.postprocess(formatContext);

            ASSERTV(mXc.fillerCodePointDisplayWidth(),
                    1 == mXc.fillerCodePointDisplayWidth());
            ASSERTV(
                 mXc.postprocessedWidth().category(),
                 NumericValue::e_VALUE == mXc.postprocessedWidth().category());
            ASSERTV(mXc.postprocessedWidth().value(),
                    6 == mXc.postprocessedWidth().value());
            ASSERTV(mXc.postprocessedPrecision().category(),
                    NumericValue::e_VALUE ==
                        mXc.postprocessedPrecision().category());
            ASSERTV(mXc.postprocessedPrecision().value(),
                    5 == mXc.postprocessedPrecision().value());
        }

        {
            WcharSpec   mXw;
            ASSERTV(mXw.processingState(),
                    Enums::e_STATE_UNPARSED == mXw.processingState());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.filler()                      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.numFillerCharacters()         )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alignment()                   )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.sign()                        )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alternativeFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.zeroPaddingFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedPrecision()      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.localeSpecificFlag()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.formatType()                  )

            bslfmt::MockParseContext<wchar_t> parseContext(L"", 1);
            mXw.parse(&parseContext, WcharSpec::e_CATEGORY_CHARACTER);

            ASSERTV(*mXw.filler(), ' ' == *mXw.filler());
            ASSERTV(mXw.numFillerCharacters(), 1 == mXw.numFillerCharacters());
            ASSERTV(mXw.alignment(),
                    Enums::e_ALIGN_DEFAULT == mXw.alignment());
            ASSERTV(mXw.sign(), Enums::e_SIGN_DEFAULT == mXw.sign());
            ASSERTV(false == mXw.alternativeFlag());
            ASSERTV(false == mXw.zeroPaddingFlag());
            ASSERTV(false == mXw.localeSpecificFlag());
            ASSERTV(mXw.formatType(),
                    WcharSpec::e_CHARACTER_CHARACTER == mXw.formatType());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedPrecision()      )

            MockFormatContext<wchar_t> formatContext(0);
            mXw.postprocess(formatContext);

            ASSERTV(mXw.fillerCodePointDisplayWidth(),
                    1 == mXw.fillerCodePointDisplayWidth());
            ASSERTV(mXw.postprocessedWidth().category(),
                    NumericValue::e_DEFAULT ==
                        mXw.postprocessedWidth().category());
            ASSERTV(mXw.postprocessedPrecision().category(),
                    NumericValue::e_DEFAULT ==
                        mXw.postprocessedPrecision().category());
        }

        {
            WcharSpec mXw;
            ASSERTV(mXw.processingState(),
                    Enums::e_STATE_UNPARSED == mXw.processingState());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.filler()                      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.numFillerCharacters()         )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alignment()                   )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.sign()                        )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.alternativeFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.zeroPaddingFlag()             )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedPrecision()      )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.localeSpecificFlag()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.formatType()                  )

            bslfmt::MockParseContext<wchar_t> parseContext(L"*<6.5Ls", 1);
            mXw.parse(&parseContext, WcharSpec::e_CATEGORY_STRING);

            ASSERTV(*mXw.filler(), '*' == *mXw.filler());
            ASSERTV(mXw.numFillerCharacters(), 1 == mXw.numFillerCharacters());
            ASSERTV(mXw.alignment(), Enums::e_ALIGN_LEFT == mXw.alignment());
            ASSERTV(mXw.sign(), Enums::e_SIGN_DEFAULT == mXw.sign());
            ASSERTV(false == mXw.alternativeFlag());
            ASSERTV(false == mXw.zeroPaddingFlag());
            ASSERTV(mXw.formatType(),
                    WcharSpec::e_STRING_DEFAULT == mXw.formatType());

            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.fillerCodePointDisplayWidth() )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedWidth()          )
            VERIFY_EXCEPTION_IS_THROWN(L_, mXw.postprocessedPrecision()      )

            MockFormatContext<wchar_t> formatContext(0);
            mXw.postprocess(formatContext);

            ASSERTV(mXw.fillerCodePointDisplayWidth(),
                    1 == mXw.fillerCodePointDisplayWidth());
            ASSERTV(mXw.postprocessedWidth().category(),
                    NumericValue::e_VALUE ==
                        mXw.postprocessedWidth().category());
            ASSERTV(mXw.postprocessedWidth().value(),
                    6 == mXw.postprocessedWidth().value());
            ASSERTV(mXw.postprocessedPrecision().category(),
                    NumericValue::e_VALUE ==
                        mXw.postprocessedPrecision().category());
            ASSERTV(mXw.postprocessedPrecision().value(),
                    5 == mXw.postprocessedPrecision().value());
        }
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
        //   bslfmt::StandardFormatSpecification();
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        CharSpec    mXc;
        ASSERTV(mXc.processingState(),
                Enums::e_STATE_UNPARSED == mXc.processingState());

        WcharSpec   mXw;
        ASSERTV(mXw.processingState(),
                Enums::e_STATE_UNPARSED == mXw.processingState());
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

        typedef StandardFormatSpecification<char>    CharSpec;
        typedef StandardFormatSpecification<wchar_t> WcharSpec;
        typedef FormatterSpecificationNumericValue   NumericValue;

        const int                          arg0 = 99;
        const int                          arg1 = 98;
        const int                          arg2 = 97;
        const int                          arg3 = 96;
        bslfmt::MockFormatContext<char>    cFormatContext(arg0,
                                                          arg1,
                                                          arg2,
                                                          arg3);
        bslfmt::MockFormatContext<wchar_t> wFormatContext(arg0,
                                                          arg1,
                                                          arg2,
                                                          arg3);

        postprocessAndVerifyResult(L_,
                      parseStandard("", CharSpec::e_CATEGORY_STRING),
                      cFormatContext,
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard("2.3s", CharSpec::e_CATEGORY_STRING),
                      cFormatContext,
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 2),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard("{3}.{2}", CharSpec::e_CATEGORY_STRING),
                      cFormatContext,
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, arg3),
                      NumericValue(NumericValue::e_VALUE, arg2),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard("*<{1}.{3}F",
                                    CharSpec::e_CATEGORY_FLOATING),
                      cFormatContext,
                      "*",
                      CharSpec::e_ALIGN_LEFT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, arg1),
                      NumericValue(NumericValue::e_VALUE, arg3),
                      false,
                      CharSpec::e_FLOATING_FIXED_UC);

        postprocessAndVerifyResult(L_,
                      parseStandard(L"", WcharSpec::e_CATEGORY_STRING),
                      wFormatContext,
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard(L"2.3s", WcharSpec::e_CATEGORY_STRING),
                      wFormatContext,
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 2),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard(L"{2}.{1}", WcharSpec::e_CATEGORY_STRING),
                      wFormatContext,
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, arg2),
                      NumericValue(NumericValue::e_VALUE, arg1),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        postprocessAndVerifyResult(L_,
                      parseStandard(L"*<{0}.{3}f",
                                    WcharSpec::e_CATEGORY_FLOATING),
                      wFormatContext,
                      L"*",
                      WcharSpec::e_ALIGN_LEFT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, arg0),
                      NumericValue(NumericValue::e_VALUE, arg3),
                      false,
                      WcharSpec::e_FLOATING_FIXED);

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
