// bslstl_formatterstring.t.cpp                                       -*-C++-*-
#include <bslfmt_formatterstring.h>

#include <bslfmt_formattertestutil.h>
#include <bslfmt_testspecificationgenerator.h>

#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>

#include <bslstl_algorithm.h>
#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #include <string_view>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT

using namespace BloombergLP;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test contains a set of partial specializations of the
// `bsl::formatter` template for string types (such as C-string,
// `bsl::basic_string`, `bsl::basic_string_view`, etc.).  Even though these
// formatters are value-semantic types, we deviate from the traditional 10-step
// approach to testing such classes because the classes have no accessors to
// test their states, and the creators and primary manipulators are generated
// by the compiler.  We do a cursory test of these methods en masse in test 2,
// just to confirm their visibility and callability, ensuring that they compile
// successfully and do not trigger runtime errors.
//
// The main semantic load of the classes being tested is carried by methods
// `parse` and `format`. It is hard to test them standalone as the interface is
// designed to be called by the `bslfmt::format` suite of functions, that are
// higher up the hierarchical ladder.  The solution to this problem is to use
// "mock" contexts and functions.  However, this is not enough.  The `format`
// and `parse` functions are designed to be called in order, and it is not
// possible to test a successful parse individually in isolation due to the
// lack of accessors that allow checking the result of the function execution.
// So test 3 (`parse`) focuses on parsing failures while parsing successes are
// tested in test 4 simultaneously with `format` function.
//
// Due to the huge number of format specification variants, full testing of all
// formatters using the specification generator takes too much time.
// Therefore, based on the fact that all formatters use the same mechanism and
// differ only in the interface, we allowed ourselves to comment out the
// testing of all formatters except one.  To test all formatters without
// exception, the `U_TEST_ALL_FORMATTER_INTERFACES` flag must be set to 1.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] formatter<t_TYPE, t_CHAR>();
// [ 2] ~formatter<t_TYPE, t_CHAR>();
// [ 2] formatter<t_TYPE, t_CHAR>(const formatter<t_TYPE, t_CHAR>&);
//
// MANIPULATORS
// [ 2] operator=(const formatter<t_TYPE, t_CHAR>&);
// [ 3] t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT&);
//
// ACCESSORS
// [ 4] t_FORMAT_CONTEXT::iterator format(TYPE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 2] swap(formatter<t_TYPE, t_CHAR>&, formatter<t_TYPE, t_CHAR>&);
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] STD NON-DELEGATION
// [ 6] USAGE EXAMPLE

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

//=============================================================================
//                     U_TEST_ALL_FORMATTER_INTERFACES
//
// Full testing with the specification generator of all possible value types
// for the formatters defined in this component takes a lot of time.  Since all
// formatters use the same mechanism, only formatter for `t_CHAR *` is fully
// tested to optimize the process.  To test  other types (c-string,
// `bsl::basic_string`, `bsl::basic_string_view`, etc.)  set
// `U_TEST_ALL_FORMATTER_INTERFACES` to 1.
// ----------------------------------------------------------------------------

#undef   U_TEST_ALL_FORMATTER_INTERFACES
#define  U_TEST_ALL_FORMATTER_INTERFACES 0

//=============================================================================
//                    GLOBAL VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#define UTF8_LITERAL(...)                                                     \
    static_cast<const char *>(static_cast<const void *>(u8##__VA_ARGS__))
#else
#define UTF8_LITERAL(EXPR) EXPR
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
#define TEST_STD_STRING_VIEW std::basic_string_view
#else
#define TEST_STD_STRING_VIEW bsl::basic_string_view
#endif

/// Verify that parsing the specified `fmtStr` by the `bslfmt::formatter`
/// specialization for `const type *` causes an exception to be thrown. If
/// the specified `useOracle` is `true`, also parse `fmtStr` using a standard
/// library implementation and make sure that the exception is thrown in this
/// case too.
#define TEST_PARSE_FAIL(type, fmtStr, useOracle)                              \
    do {                                                                      \
        bsl::string errorMsg;                                                 \
        bool rv;                                                              \
        rv = bslfmt::FormatterTestUtil<type>::                                \
                 testParseFailure<const type *>(&errorMsg, useOracle, fmtStr);\
        if (!rv) {                                                            \
            bsl::basic_string<type> formatStr(fmtStr);                        \
            ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);                 \
        }                                                                     \
    } while (false)

/// Verify that the `bslfmt::formatter` specialization for `SingleType`
/// successfully parses the specified `fmtStr`.  If the specified `useOracle`
/// is `true`, also verify that standard library implementation of the
/// formatter for `SingleType` successfully parses the `fmtStr` too.  Note that
/// `fmtStr` is required to be a compile time constant expression.
#define TPSF_SINGLE_TYPE(SingleType, type, fmtStr, useOracle)                 \
        do {                                                                  \
            bsl::string errorMsg;                                             \
            bool rv =                                                         \
                bslfmt::FormatterTestUtil<type>::testParseFormat<SingleType>( \
                                                                   &errorMsg, \
                                                                   useOracle, \
                                                                   fmtStr);   \
            if (!rv) {                                                        \
                std::basic_string<type> formatStr(fmtStr);                    \
                ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);             \
            }                                                                 \
        } while (false);

/// Verify that parsing the specified `fmtStr` succeeds for all implemented
/// `bslfmt::formatter` specializations corresponding to the specified `type`
/// and its related forms (e.g., type*, const type*, bsl:basic_string<type>,
/// etc.).  If the specified `useOracle` is `true`, also verify that standard
/// library implementation of these formatters successfully parse the `fmtStr`
/// too.  Note that `fmtStr` is required to be a compile time constant
/// expression.
#define TEST_PARSE_SUCCESS_F(type, fmtStr, useOracle)                         \
    do {                                                                      \
        TPSF_SINGLE_TYPE(const type *                , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(      type *                , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(      type[10]              , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(bsl::basic_string<type>     , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(std::basic_string<type>     , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(bsl::basic_string_view<type>, type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(TEST_STD_STRING_VIEW<type>  , type,fmtStr,useOracle) \
        TPSF_SINGLE_TYPE(bslstl::StringRefImp<type>  , type,fmtStr,useOracle) \
    } while (false)

/// Verify that the `bslfmt::formatter` specialization for `SingleType`
/// successfully parses the specified `fmtStr`.  If the specified `useOracle`
/// is `true`, also verify that standard library implementation of the
/// formatter for `SingleType` successfully parses the `fmtStr` too.  Note that
/// `fmtStr` can be determined at runtime.
#define TPSVF_SINGLE_TYPE(SingleType, type, fmtStr, useOracle)                \
        do {                                                                  \
            bsl::string errorMsg;                                             \
            bool rv =                                                         \
                bslfmt::FormatterTestUtil<type>::testParseVFormat<SingleType>(\
                                                                  &errorMsg,  \
                                                                  useOracle,  \
                                                                  fmtStr);    \
            if (!rv) {                                                        \
                bsl::basic_string<type> formatStr(fmtStr);                    \
                ASSERTV(errorMsg.c_str(), formatStr.c_str(), rv);             \
            }                                                                 \
        } while (false);

/// Verify that parsing the specified `fmtStr` succeeds for all implemented
/// `bslfmt::formatter` specializations corresponding to the specified `type`
/// and its related forms (e.g., type*, const type*, bsl:basic_string<type>,
/// etc.).  If the specified `useOracle` is `true`, also verify that standard
/// library implementation of these formatters successfully parse the `fmtStr`
/// too.  Note that `fmtStr` can be determined at runtime.
#define TEST_PARSE_SUCCESS_VF(type, fmtStr, useOracle)                        \
    do {                                                                      \
        TPSVF_SINGLE_TYPE(const type *                ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(      type *                ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(      type[10]              ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(bsl::basic_string<type>     ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(std::basic_string<type>     ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(bsl::basic_string_view<type>,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(TEST_STD_STRING_VIEW<type>  ,type,fmtStr,useOracle) \
        TPSVF_SINGLE_TYPE(bslstl::StringRefImp<type>  ,type,fmtStr,useOracle) \
    } while (false)

// ============================================================================
//                     GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// GCC supports Unicode characters for `std::format` starting from version 13.
// Therefore, we will use the standard oracle mechanism only if this version of
// the library or higher is used.

#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 14
static const bool k_ORACLE_SUPPORTS_UNICODE = false;
#else
static const bool k_ORACLE_SUPPORTS_UNICODE = true;
#endif

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// Create an expected formatting result of the specified `inputStr` based on
/// the current state of the specified generator, as if the `format` function
/// parameters were the specified `arg1` and `arg2`.  Store this result in the
/// specified `modelStr`.  Return 0 on success, and 1 if the state of the
/// `generator` does not suggest successful formatting.
template <class t_CHAR>
int getExpectedResult(
             bsl::basic_string<t_CHAR>                         *expectedResult,
             const bsl::basic_string_view<t_CHAR>&              inputStr,
             int                                                arg1,
             int                                                arg2,
             const bslfmt::TestSpecificationGenerator<t_CHAR>&  generator)
{
    ASSERT(expectedResult);
    typedef bslfmt::TestSpecificationGenerator<t_CHAR> Generator;

    if (!generator.isStateValidForFormat()) {
        return 1;                                                     // RETURN
    }

    // Value that signifies the variable is not in use
    const int k_UNSET  = -1;

    int argIndex =  1;

    // Width
    int finalWidth = k_UNSET;
    {
        if (generator.isNestedWidthPresent()) {
            switch (generator.nestedWidthVariant()) {
              case Generator::e_NESTED_DEFAULT: {
                finalWidth = (1 == argIndex) ? arg1 : arg2;
                ++argIndex;
              } break;
              case Generator::e_NESTED_ARG_1: {
                finalWidth = arg1;
              } break;
              case Generator::e_NESTED_ARG_2: {
                finalWidth = arg2;
              } break;
              default: {
                ASSERTV("Unexpected value",
                        generator.nestedWidthVariant(),
                        false);
              }
            }
        }
        else {
            if (generator.isWidthOptionPresent()) {
                finalWidth = generator.width();
            }
        }
    }

    // Precision
    int finalPrecision = k_UNSET;
    {
        if (generator.isNestedPrecisionPresent()) {
            switch (generator.nestedPrecisionVariant()) {
              case Generator::e_NESTED_DEFAULT: {
                finalPrecision = (1 == argIndex) ? arg1 : arg2;
                ++argIndex;
              } break;
              case Generator::e_NESTED_ARG_1: {
                finalPrecision = arg1;
              } break;
              case Generator::e_NESTED_ARG_2: {
                finalPrecision = arg2;
              } break;
              default: {
                ASSERTV("Unexpected value",
                        generator.nestedPrecisionVariant(),
                        false);
              }
            }
        }
        else {
            if (generator.isPrecisionOptionPresent()) {
                finalPrecision = generator.precision();
            }
        }
    }

    // Fill and align
    char                          fillCharacter = ' ';
    typename Generator::Alignment alignment     = Generator::e_ALIGN_DEFAULT;
    if (generator.isFillCharacterPresent()) {
        fillCharacter = generator.fillCharacter();
    }

    if (generator.isAlignOptionPresent()) {
        alignment = generator.alignment();
    }

    bsl::basic_string_view<t_CHAR> finalStr = (k_UNSET == finalPrecision)
                                          ? inputStr
                                          : inputStr.substr(0, finalPrecision);

    int finalLength = static_cast<int>(finalStr.length());

    expectedResult->clear();

    if (finalWidth > finalLength) {
        int alignmentLength =  finalWidth - finalLength;
        switch (alignment) {
            case Generator::e_ALIGN_DEFAULT:
            case Generator::e_ALIGN_LEFT: {
                expectedResult->append(finalStr);
                for (int i = 0; i < alignmentLength; ++i) {
                    expectedResult->push_back(fillCharacter);
                }
            } break;
            case Generator::e_ALIGN_RIGHT: {

                for (int i = 0; i < alignmentLength; ++i) {
                    expectedResult->push_back(fillCharacter);
                }
                expectedResult->append(finalStr);
            } break;
            case Generator::e_ALIGN_MIDDLE: {
                int numCharactersLeft = alignmentLength;
                for (int i = 0;
                     i < alignmentLength / 2;
                     ++i, --numCharactersLeft) {
                    expectedResult->push_back(fillCharacter);
                }
                expectedResult->append(finalStr);
                for (int i = 0; i < numCharactersLeft ; ++i) {
                    expectedResult->push_back(fillCharacter);
                }
            } break;
        }
    }
    else {
        *expectedResult = finalStr;
    }

    return 0;
}

// ============================================================================
//                            TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

/// This struct template provides a namespace for test function implementations
/// that verify the correctness of the formatter methods.
template <class t_CHAR>
struct TestDriver {

    // CLASS METHODS

    /// Perform a minimal check of all formatters, comparing the formatting
    /// result of the specified `inputOriginal` with the corresponding
    /// `formatSpec` and the specified `expected`.  The specified `line` is
    /// used to identify the test data.
    static void testCase4Minimal(int                            line,
                                 bsl::basic_string_view<t_CHAR> formatSpec,
                                 bsl::basic_string_view<t_CHAR> inputOriginal,
                                 bsl::basic_string_view<t_CHAR> expected);

    /// Perform a comprehensive check of formatters, formatting the specified
    /// `inputString` and using the `bslfmt::TestSpecificationGenerator` to
    /// create the specifications.
    static void testCase4Comprehensive(
                                   bsl::basic_string_view<t_CHAR> inputString);


    /// Evaluate the result of formatting the specified `value` with the
    /// specified `formatSpec` as if the `format` function parameters were the
    /// specified `arg1` and `arg2` and verify its equality with the specified
    /// `expectedResult`.  Store error description to the specified `message`
    /// in case of formatting failure, and leave the `message` cleared
    /// otherwise.
    template <class t_VALUE>
    static void testCase4SingleValueType(
             bsl::string                                       *message,
             const bsl::basic_string_view<t_CHAR>&              expectedResult,
             const bsl::basic_string_view<t_CHAR>&              formatSpec,
             const t_VALUE&                                     value,
             int                                                arg1,
             int                                                arg2,
             const bslfmt::TestSpecificationGenerator<t_CHAR>&  generator);

    /// Test formatter creators and manipulators.
    static void testCase2();

    /// Test creators and manipulators of `t_TYPE` formatter.
    template<class t_TYPE>
    static void testCase2Imp();
};

                                // ----------
                                // TEST CASES
                                // ----------

template <class t_CHAR>
void TestDriver<t_CHAR>::testCase4Minimal(
                                  int                            line,
                                  bsl::basic_string_view<t_CHAR> formatSpec,
                                  bsl::basic_string_view<t_CHAR> inputOriginal,
                                  bsl::basic_string_view<t_CHAR> expected)
{
    // character buffer
    bsl::vector<t_CHAR> charVector(inputOriginal.begin(), inputOriginal.end());
    charVector.push_back(0);

    // t_CHAR *
    t_CHAR      *inputCharPtr = charVector.data();
    bsl::string  message;
    bool         rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                                 &message,
                                                                 expected,
                                                                 true,
                                                                 formatSpec,
                                                                 inputCharPtr);
    ASSERTV(line, message.c_str(), rv);

    // const t_CHAR *

    const t_CHAR *inputConstCharPtr = inputCharPtr;
    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                            &message,
                                                            expected,
                                                            true,
                                                            formatSpec,
                                                            inputConstCharPtr);
    ASSERTV(line, message.c_str(), rv);

    // bsl::basic_string<t_CHAR>

    bsl::basic_string<t_CHAR> inputBslString(inputOriginal);
    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                               &message,
                                                               expected,
                                                               true,
                                                               formatSpec,
                                                               inputBslString);
    ASSERTV(line, message.c_str(), rv);

    // std::basic_string<t_CHAR>

    std::basic_string<t_CHAR> inputStdString(inputOriginal);
    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                               &message,
                                                               expected,
                                                               true,
                                                               formatSpec,
                                                               inputStdString);
    ASSERTV(line, message.c_str(), rv);

    // bsl::basic_string_view<t_CHAR>

    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                expected,
                                                                true,
                                                                formatSpec,
                                                                inputOriginal);
    ASSERTV(line, message.c_str(), rv);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    // std::basic_string_view<t_CHAR>

    std::basic_string_view<t_CHAR> inputStdSV(inputOriginal.data(),
                                              inputOriginal.length());
    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                expected,
                                                                true,
                                                                formatSpec,
                                                                inputStdSV);
    ASSERTV(line, message.c_str(), rv);
#endif

    // bslstl::StringRefImp<t_CHAR>

    bslstl::StringRefImp<t_CHAR> inputSR(inputOriginal);
    message.clear();
    rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(&message,
                                                                expected,
                                                                true,
                                                                formatSpec,
                                                                inputSR);
    ASSERTV(message.c_str(), rv);
}

template <class t_CHAR>
void TestDriver<t_CHAR>::testCase4Comprehensive(
                                  bsl::basic_string_view<t_CHAR> inputOriginal)
{
    const char *GENERATOR_SPECIFICATION = "VF^{}s";

    bslfmt::TestSpecificationGenerator<t_CHAR> generator;
    generator.setup(GENERATOR_SPECIFICATION);
    const bsl::basic_string<t_CHAR>& formatSpec  = generator.formatSpec();
    int                              counter     = 0;
    const int                        args[]      = {2, 4, 6};
    const size_t                     NUM_ARGS    =
                                                 sizeof(args) / sizeof (*args);
    const size_t                     BUFFER_SIZE = 16;
    t_CHAR                           inputCharPtr[BUFFER_SIZE];
    do {  // loop through format specifications
        bsl::basic_string<t_CHAR> expectedResult;
        for (size_t i = 0; i <= inputOriginal.size(); ++i) {
            const size_t INPUT_LENGTH = i;
            bsl::basic_string_view<t_CHAR> inputSV =
                                         inputOriginal.substr(0, INPUT_LENGTH);
            for (size_t j = 0; j < NUM_ARGS; ++j) {
                const int ARG_1 = args[j];
                for (size_t k = 0; k < NUM_ARGS; ++k) {
                    const int ARG_2 = args[k];
                    if (veryVerbose) {
                        T_ P_(counter) P_(INPUT_LENGTH) P_(ARG_1) P_(ARG_2)
                    }

                    getExpectedResult(&expectedResult,
                                      inputSV,
                                      ARG_1,
                                      ARG_2,
                                      generator);

                    bsl::string message;

                    // `t_CHAR *`

                    std::memset(inputCharPtr, 0, sizeof(inputCharPtr));
                            ASSERTV(inputSV.length(),
                                    BUFFER_SIZE > inputSV.length());
                    std::memcpy(inputCharPtr,
                                inputSV.data(),
                                inputSV.length() * sizeof(t_CHAR));

                    testCase4SingleValueType(
                             &message,        // error message storage
                             expectedResult,  // expected result
                             formatSpec,      // format specification
                             inputCharPtr,    // input to format
                             ARG_1,           // format argument #1
                             ARG_2,           // format argument #2
                             generator);      // current state of the generator

#if U_TEST_ALL_FORMATTER_INTERFACES
                    // Note that full testing with the specification generator
                    // of all possible value types for the formatters defined
                    // in this component takes a lot of time. Since all
                    // formatters use the same mechanism, by default we test
                    // only formatter for `t_CHAR *`. To test other types set
                    // `U_TEST_ALL_FORMATTER_INTERFACES`  to 1.

                    // `const t_CHAR *`

                    const t_CHAR *inputConstCharPtr = inputCharPtr;
                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputConstCharPtr,
                                             ARG_1,
                                             ARG_2,
                                             generator);

                    // `bsl::basic_string<t_CHAR>`

                    bsl::basic_string<t_CHAR> inputBslString(inputSV);
                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputBslString,
                                             ARG_1,
                                             ARG_2,
                                             generator);

                    //` std::basic_string<t_CHAR>`

                    std::basic_string<t_CHAR> inputStdString(inputSV);
                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputStdString,
                                             ARG_1,
                                             ARG_2,
                                             generator);

                    // `bsl::basic_string_view<t_CHAR>`

                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputSV,
                                             ARG_1,
                                             ARG_2,
                                             generator);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
                    // `std::basic_string_view<t_CHAR>`

                    std::basic_string_view<t_CHAR> inputStdSV(
                                                             inputSV.data(),
                                                             inputSV.length());
                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputStdSV,
                                             ARG_1,
                                             ARG_2,
                                             generator);
#endif

                    // `bslstl::StringRefImp<t_CHAR>`

                    bslstl::StringRefImp<t_CHAR> inputSR(inputSV);
                    testCase4SingleValueType(&message,
                                             expectedResult,
                                             formatSpec,
                                             inputSR,
                                             ARG_1,
                                             ARG_2,
                                             generator);
#endif  // U_TEST_ALL_FORMATTER_INTERFACES
                }
            }
        }
        ++counter;
    } while (generator.next());
}

template <class t_CHAR>
template <class t_VALUE>
void TestDriver<t_CHAR>::testCase4SingleValueType(
             bsl::string                                       *message,
             const bsl::basic_string_view<t_CHAR>&              expectedResult,
             const bsl::basic_string_view<t_CHAR>&              formatSpec,
             const t_VALUE&                                     value,
             int                                                arg1,
             int                                                arg2,
             const bslfmt::TestSpecificationGenerator<t_CHAR>&  generator)
{
    typedef bslfmt::TestSpecificationGenerator<t_CHAR> Generator;

    message->clear();
    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                                message,
                                                                expectedResult,
                                                                true,
                                                                formatSpec,
                                                                value,
                                                                arg1,
                                                                arg2);

    if (generator.isStateValidForFormat()) {
        if (generator.isTypeOptionPresent() && Generator::e_TYPE_ESCAPED ==
                                                   generator.type()) {
            ASSERTV(generator.formatSpec().c_str(), !rv);
        }
        else {
            ASSERTV(generator.formatSpec().c_str(), message->c_str(), rv);
        }
    }
    else {
        ASSERTV(generator.formatSpec().c_str(), !rv);
    }
}

template <class t_CHAR>
void TestDriver<t_CHAR>::testCase2()
{
        testCase2Imp<                  const t_CHAR *   >();
        testCase2Imp<                        t_CHAR *   >();
        testCase2Imp<                        t_CHAR[10] >();
        testCase2Imp< bsl::basic_string<     t_CHAR>    >();  // bsl
        testCase2Imp< std::basic_string<     t_CHAR>    >();  // std
        testCase2Imp< bsl::basic_string_view<t_CHAR>    >();  // bsl
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        testCase2Imp< std::basic_string_view<t_CHAR>    >();  // std
#endif
        testCase2Imp< bslstl::StringRefImp<  t_CHAR>    >();
}

template <class t_CHAR>
template <class t_TYPE>
void TestDriver<t_CHAR>::testCase2Imp()
{
    if (verbose) printf("\t\tTesting `%s`.\n", NameOf<t_TYPE>().name());
    if (verbose) printf("\t\t\tTesting default constructor\n");
    {
        bsl::formatter<t_TYPE, t_CHAR> dummy;
        (void) dummy;
    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
    if (verbose) printf("\t\t\tTesting formatter specialization promotion\n");
    {
        std::formatter<t_TYPE, t_CHAR> dummy;
        (void) dummy;
    }
#endif

    if (verbose) printf("\t\t\tTesting copy constructor\n");
    {
        const bsl::formatter<t_TYPE, t_CHAR> dummy;
              bsl::formatter<t_TYPE, t_CHAR> copy(dummy);
        (void) copy;
    }

    if (verbose) printf("\t\t\tValidating `swap`\n");
    {
        bsl::formatter<t_TYPE, t_CHAR> dummy;
        bsl::formatter<t_TYPE, t_CHAR> dummy2;
        bsl::swap(dummy, dummy2);
    }

    if (verbose) printf("\t\t\tTesting copy assignment\n");
    {
        const bsl::formatter<t_TYPE, t_CHAR> dummy;
        bsl::formatter<t_TYPE, t_CHAR>       dummy2;
        dummy2 = dummy;
    }
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose     = argc > 2;
    veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        // 1. Demonstrate the functioning of this component.
        //
        // Plan:
        // 1. Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting a basic string
/// - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
//
//..
        bslfmt::MockParseContext<char> mpc("*<5.3s", 1);

        bsl::formatter<const char *, char> f;
        mpc.advance_to(f.parse(mpc));

        const char *value = "abcdefghij";

        bslfmt::MockFormatContext<char> mfc(value, 0, 0);

        mfc.advance_to(bsl::as_const(f).format(value, mfc));

        ASSERT("abc**" == mfc.finalString());
//..
//
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // STD NON-DELEGATION
        //   This test case verifies that we do not hijack the `std` formatter.
        //   This test case is executed only when `std::format` is present and
        //   usable/used by BDE.
        //
        // Concern:
        // 1. When `std::format` is present and used/usable the component
        //    `bslfmt_formatterbase` defines a `std::formatter` partial
        //    specialization for all types that do not already have a standard
        //    formatter) that is inherited from (implemented in terms of) the
        //    `bsl::formatter` for that type.  However we do not want that
        //    `std::formatter` partial specialization to be active for types
        //    that do have a formatter in the standard library such as the
        //    string-like standard-formatted types.
        //
        // Plan:
        // 1. Verify that `std::formatter` instantiations for string-like types
        //    are not inherited from `bsl::formatter` of the same template
        //    parameters.
        //
        // Testing:
        //   STD NON-DELEGATION
        // --------------------------------------------------------------------

        if (verbose) printf("\nSTD NON-DELEGATION"
                            "\n==================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<const char *, char>,
                std::formatter<const char *, char> >::value));
        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<const wchar_t *, wchar_t>,
                std::formatter<const wchar_t *, wchar_t> >::value));

        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<std::string_view, char>,
                std::formatter<std::string_view, char> >::value));
        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<std::wstring_view, wchar_t>,
                std::formatter<std::wstring_view, wchar_t> >::value));

#ifdef BSLSTL_STRING_VIEW_IS_ALIASED
        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<bsl::string_view, char>,
                std::formatter<bsl::string_view, char> >::value));
        ASSERT((!bslmf::IsAccessibleBaseOf<
                bsl::formatter<bsl::wstring_view, wchar_t>,
                std::formatter<bsl::wstring_view, wchar_t> >::value));
#else   // BSLSTL_STRING_VIEW_IS_ALIASED
        ASSERT((bslmf::IsAccessibleBaseOf<
                bsl::formatter<bsl::string_view, char>,
                std::formatter<bsl::string_view, char> >::value));
        ASSERT((bslmf::IsAccessibleBaseOf<
                bsl::formatter<bsl::wstring_view, wchar_t>,
                std::formatter<bsl::wstring_view, wchar_t> >::value));
#endif  // else - BSLSTL_STRING_VIEW_IS_ALIASED
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `format`
        //   Full testing with the specification generator of all possible
        //   value types for the formatters defined in this component takes a
        //   lot of time.  Since all formatters use the same mechanism, only
        //   formatter for `t_CHAR *` is fully tested. To test other types
        //   (c-string, `bsl::basic_string`, `bsl::basic_string_view`, etc.)
        //   set `U_TEST_ALL_FORMATTER_INTERFACES` to 1.
        //
        // Concerns:
        // 1. After parsing a valid format spec, `format` will correctly format
        //    a valid string type.
        //
        // 2. Valid format strings will not generate a parse error.
        //
        // 3. Unicode characters are correctly handled.
        //
        // Plan:
        // 1. Using table-based approach specify a minimal set of format
        //    specifications.  Verify that for each of the specifications the
        //    `format` function outputs the expected result.  Perform this
        //    sequence for all the supported string types:
        //    - char *
        //    - const char *
        //    - std::string_view
        //    - bsl::string_view
        //    - std::string
        //    - bsl::string
        //    - bslstl::StringRef
        //
        // 2. Using set of various input strings and format specifications
        //    obtained from `bslfmt::TestSpecificationGenerator` verify that
        //    the `format` function outputs the expected results or throw an
        //    exception in case of invalid format specification.  By default
        //    this scenario is run for `char *` formatter interface only (see
        //    `U_TEST_ALL_FORMATTER_INTERFACES`).  (C-1..2)
        //
        // 3. Using table-based approach specify a set of graphemes for
        //    formatting.  Then, using loops, iterate over several width and
        //    precision values as formatting parameters. For each combination
        //    of "grapheme x width x precision", construct the expected output
        //    string based on the grapheme width. Format the input string
        //    (which contains three graphemes in a row) and verify that the
        //    result matches the expected output.  This scenario is
        //    run for `bsl::string` formatter interface only.  (C-3)
        //
        // Testing:
        //   t_FORMAT_CONTEXT::iterator format(t_VALUE, t_FORMAT_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING `format`"
                   "\n================\n");

        bsl::string_view   charInput =  "123abcDEF+";
        bsl::wstring_view wcharInput = L"123abcDEF+";

        if (verbose) printf("\tBasic testing\n");
        {
            // `char`
            {
                static const struct {
                    int                 d_line;
                    bsl::string_view    d_charSpec;
                    bsl::string_view    d_charExpected;
                } DATA[] = {
                    //LINE C_SPEC       C_EXPECTED
                    //---- -----------  ------------
                    { L_,  "{:s}",      "123abcDEF+" },
                    { L_,  "{:6s}",     "123abcDEF+" },
                    { L_,  "{:.4s}",    "123a"       },
                    { L_,  "{:6.4s}",   "123a  "     },
                    { L_,  "{:*<6.4s}", "123a**"     },
                };
                const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (size_t i = 1; i < NUM_DATA; ++i) {
                    const int               LINE   = DATA[i].d_line;
                    const bsl::string_view  C_SPEC = DATA[i].d_charSpec;
                    const bsl::string_view  C_EXP  = DATA[i].d_charExpected;

                    TestDriver<char>::testCase4Minimal(LINE,
                                                       C_SPEC,
                                                       charInput,
                                                       C_EXP);
                }
            }

            // `wchar_t`
            {
                static const struct {
                    int                 d_line;
                    bsl::wstring_view   d_wcharSpec;
                    bsl::wstring_view   d_wcharExpected;
                } DATA[] = {
                    //LINE W_SPEC        W_EXPECTED
                    //---- ------------  -------------
                    { L_,  L"{:s}",      L"123abcDEF+" },
                    { L_,  L"{:6s}",     L"123abcDEF+" },
                    { L_,  L"{:.4s}",    L"123a"       },
                    { L_,  L"{:6.4s}",   L"123a  "     },
                    { L_,  L"{:*<6.4s}", L"123a**"     },
                };
                const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (size_t i = 1; i < NUM_DATA; ++i) {
                    const int               LINE   = DATA[i].d_line;
                    const bsl::wstring_view W_SPEC = DATA[i].d_wcharSpec;
                    const bsl::wstring_view W_EXP  = DATA[i].d_wcharExpected;

                    TestDriver<wchar_t>::testCase4Minimal(LINE,
                                                          W_SPEC,
                                                          wcharInput,
                                                          W_EXP);
                }
            }
        }

        if (verbose) printf("\tComprehensive testing\n");
        {
            TestDriver<char   >::testCase4Comprehensive( charInput);
            TestDriver<wchar_t>::testCase4Comprehensive(wcharInput);
        }

        if (verbose) printf("\tUnicode characters testing\n");
        {
            typedef bslfmt::UnicodeCodePoint CodePoint;

            // char
            {
                bsl::string_view formatSpec = "{:*^{}.{}s}";

                static const struct {
                    const int              d_line;           // source line
                                                             // number

                    const bsl::string_view d_grapheme;       // grapheme

                    int                    d_graphemeWidth;  // expected
                                                             // grapheme width
                } DATA[] = {
                    //LINE  GRAPHEME                                WIDTH
                    //----  --------------------------------------  -----
                    { L_,   "\xf0\x93\x80\x80",                        1 },
                    { L_,   "\xf0\x9f\x98\x80",                        2 },
                    { L_,   "\x6E\xCC\x83",                            1 },
                    { L_,   "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F"
                            "\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7",    2 },
                };
                const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < NUM_DATA; ++i) {
                    const int              LINE      = DATA[i].d_line;
                    const bsl::string_view GRAPHEME  = DATA[i].d_grapheme;
                    const int              EXP_WIDTH = DATA[i].d_graphemeWidth;

                    // Grapheme processing
                    CodePoint codePoint;
                    codePoint.extract(CodePoint::e_UTF8,
                                      GRAPHEME.data(),
                                      GRAPHEME.length());
                    ASSERTV(LINE, codePoint.isValid());
                    int graphemeWidth = codePoint.codePointWidth();
                    ASSERTV(LINE, EXP_WIDTH, graphemeWidth,
                            EXP_WIDTH == graphemeWidth);

                    if (veryVerbose) {
                        T_ P_("`char`") P_(LINE) P(graphemeWidth)
                    }

                    // Input construction
                    bsl::string input;
                    for (int i = 0; i < 4; ++i) {
                        input.append(GRAPHEME);
                    }

                    for (int j = 1; j < 5; ++j) {
                        const int WIDTH = j;
                        if (veryVerbose) { T_ T_ P(WIDTH) }
                        for (int k = 0; k < 5; ++k) {
                            const int PRECISION = k;

                           // Expected result string construction
                            int graphemeNum = PRECISION / graphemeWidth;
                            int fillCharsNum   =
                                           WIDTH - graphemeNum * graphemeWidth;
                            int leftPadNum  = 0;
                            int rightPadNum = 0;
                            if (fillCharsNum > 0) {
                                leftPadNum  = fillCharsNum / 2;
                                rightPadNum = fillCharsNum - leftPadNum;
                            }

                            if (veryVerbose) {
                                T_ T_ T_ P_(PRECISION) P_(leftPadNum)
                                    P_(graphemeNum) P(rightPadNum)
                            }

                            bsl::string expectedResult;
                            for (int l = 0; l < leftPadNum; ++l) {
                                expectedResult.push_back('*');
                            }
                            for (int l = 0; l < graphemeNum; ++l) {
                                expectedResult.append(GRAPHEME);
                            }
                            for (int l = 0; l < rightPadNum; ++l) {
                                expectedResult.push_back('*');
                            }

                            // Test action
                            bsl::string message;
                            bool        rv = bslfmt::FormatterTestUtil<char>::
                                testEvaluateVFormat(&message,
                                                    expectedResult,
                                                    k_ORACLE_SUPPORTS_UNICODE,
                                                    formatSpec,
                                                    input,
                                                    WIDTH,
                                                    PRECISION);
                            ASSERTV(LINE, message.c_str(), rv);
                        }
                    }
                }
            }

            // `wchar_t`
            {
                bsl::wstring_view formatSpec = L"{:*^{}.{}s}";

                static const struct {
                    const int               d_line;           // source line
                                                              // number

                    const bsl::wstring_view d_grapheme;       // grapheme

                    int                     d_graphemeWidth;  // expected
                                                              // grapheme width
                } DATA[] = {
                    //LINE  GRAPHEME                    WIDTH
                    //----  --------------------------  -----
                    { L_,   L"\U00013000",              1    },
                    { L_,   L"\U0001F600",              2    },
                    { L_,   L"\U0000006e\U00000303",    1    },
                };
                const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < NUM_DATA; ++i) {
                    const int              LINE       = DATA[i].d_line;
                    const bsl::wstring_view GRAPHEME  = DATA[i].d_grapheme;
                    const int              EXP_WIDTH  =
                                                       DATA[i].d_graphemeWidth;

                    // Grapheme processing
                    const CodePoint::UtfEncoding utfEncoding =
                                    sizeof(wchar_t) == 2 ? CodePoint::e_UTF16
                                                         : CodePoint::e_UTF32;
                    CodePoint codePoint;
                    codePoint.extract(utfEncoding,
                                      GRAPHEME.data(),
                                      GRAPHEME.length() * sizeof(wchar_t));
                    ASSERTV(LINE, codePoint.isValid());
                    int graphemeWidth = codePoint.codePointWidth();
                    ASSERTV(LINE, EXP_WIDTH, graphemeWidth,
                            EXP_WIDTH == graphemeWidth);

                    if (veryVerbose) {
                        T_ P_("`wchar_t`") P_(LINE) P(graphemeWidth)
                    }

                    // Input construction
                    bsl::wstring input;
                    for (int i = 0; i < 4; ++i) {
                        input.append(GRAPHEME);
                    }

                    for (int j = 1; j < 5; ++j) {
                        const int WIDTH = j;
                        if (veryVerbose) { T_ T_ P(WIDTH) }
                        for (int k = 0; k < 5; ++k) {
                            const int PRECISION = k;

                           // Expected result string construction
                            int graphemeNum = PRECISION / graphemeWidth;
                            int fillCharsNum   =
                                           WIDTH - graphemeNum * graphemeWidth;
                            int leftPadNum  = 0;
                            int rightPadNum = 0;
                            if (fillCharsNum > 0) {
                                leftPadNum  = fillCharsNum / 2;
                                rightPadNum = fillCharsNum - leftPadNum;
                            }

                            if (veryVerbose) {
                                T_ T_ T_ P_(PRECISION) P_(leftPadNum)
                                    P_(graphemeNum) P(rightPadNum)
                            }

                            bsl::wstring expectedResult;
                            for (int l = 0; l < leftPadNum; ++l) {
                                expectedResult.push_back('*');
                            }
                            for (int l = 0; l < graphemeNum; ++l) {
                                expectedResult.append(GRAPHEME);
                            }
                            for (int l = 0; l < rightPadNum; ++l) {
                                expectedResult.push_back('*');
                            }

                            // Test action
                            bsl::string message;
                            bool rv = bslfmt::FormatterTestUtil<wchar_t>::
                                testEvaluateVFormat(&message,
                                                    expectedResult,
                                                    k_ORACLE_SUPPORTS_UNICODE,
                                                    formatSpec,
                                                    input,
                                                    WIDTH,
                                                    PRECISION);
                            ASSERTV(LINE, message.c_str(), rv);
                        }
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `parse`
        //
        // Concerns:
        // 1. Invalid format specs will generate a parse error
        //
        // 2. Valid format specs will not generate a parse error
        //
        // Plan:
        // 1. Construct format specs corresponding to each of the known error
        //    conditions and verify that they result in a parse error. (C-1)
        //
        // 2. Construct format specs containing different combinations of
        //    valid specification components and verify that they correctly
        //    parse. (C-2)
        //
        // Testing:
        //   t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `parse`"
                            "\n===============\n");

        // Bad fill character
        // Note can only test '{' as '}' closes the parse string.
        TEST_PARSE_FAIL(char,     "{:{<5.5s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:{<5.5s}",    true);

        // Missing fill specifier
        TEST_PARSE_FAIL(char,     "{:*5.5s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*5.5s}",     true);

        // Sign
        TEST_PARSE_FAIL(char,     "{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< 5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-5.5s}",   true);

        // Alternative option
        TEST_PARSE_FAIL(char,     "{:*<#5.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<#5.5s}",   true);

        // Zero pad option
        TEST_PARSE_FAIL(char,     "{:*<05.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<05.5s}",   true);

        // Locale option
        TEST_PARSE_FAIL(char,     "{:*<5.5Ls}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5Ls}",   true);

        // Escaped string type
        // Not supported in bslfmt at all or in std before C++23
        TEST_PARSE_FAIL(char,     "{:*<5.5?}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5?}",    true);

        // Non-string type
        TEST_PARSE_FAIL(char,     "{:*<5.5d}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<5.5d}",    true);

        // Non-numeric width or precision
        TEST_PARSE_FAIL(char,     "{:*< X.5s}",   true);
        TEST_PARSE_FAIL(char,     "{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(char,     "{:*<-X.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*< X.5s}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<+5.Xs}",   true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<-X.Xs}",   true);

        // Missing precision marker
        TEST_PARSE_FAIL(char,     "{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}{}s}",      false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}{2}s}",   false);

        // Mixed numeric and non-numeric nested args
        TEST_PARSE_FAIL(char,     "{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(char,     "{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{}.{2}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{:*<{1}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{}s}",     true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{}.{2}s}",    true);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{}s}",    true);

        // Nested args out of range
        // Not checked in std parsing
        TEST_PARSE_FAIL(char,     "{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(char,     "{0:*<{3}.{2}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{1}.{3}s}",   false);
        TEST_PARSE_FAIL(wchar_t, L"{0:*<{3}.{2}s}",   false);

        // A selection of valid format strings (non-unicode)
        TEST_PARSE_SUCCESS_F(char,     "{:}"                   , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:}"                   , true);
        TEST_PARSE_SUCCESS_F(char,     "{:.0}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:.0}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:.8}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:.8}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5}"                  , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5}"                  , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5.0}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5.0}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:5.8}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:5.8}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^}"                 , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^}"                 , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^.0}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^.0}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^.8}"               , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^.8}"               , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5}"                , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5}"                , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5.0}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5.0}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^5.8}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^5.8}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:{}.{}}"              , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:{}.{}}"              , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*<{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*<{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*>{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*>{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{:*^{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{:*^{}.{}}"            , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:{1}.{1}}"           , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:{1}.{1}}"           , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*<{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*<{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*>{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*>{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(char,     "{0:*^{1}.{1}}"         , true);
        TEST_PARSE_SUCCESS_F(wchar_t, L"{0:*^{1}.{1}}"         , true);

// `std::format` is not fully constexpr in gcc 13 — parsing format strings
// containing unicode symbols in a constexpr context triggers a call to a
// non-constexpr function.
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 13
        #define TPS TEST_PARSE_SUCCESS_VF
#else
        #define TPS TEST_PARSE_SUCCESS_F
#endif

         // We need a shortened version of the name to fit in the table.
         const int k_UNICODE_IS_SUPPORTED = k_ORACLE_SUPPORTS_UNICODE;

        // A selection of valid format strings (unicode)
        TPS(char,     "{:\xF0\x9F\x98\x80<}"         , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<}"               , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<.0}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<.0}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<.8}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<.8}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<5}"        , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<5}"              , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<5.0}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<5.0}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<5.8}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<5.8}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>}"         , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>}"               , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>.0}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>.0}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>.8}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>.8}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>5}"        , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>5}"              , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>5.0}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>5.0}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>5.8}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>5.8}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^}"         , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^}"               , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^.0}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^.0}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^.8}"       , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^.8}"             , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^5}"        , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^5}"              , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^5.0}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^5.0}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^5.8}"      , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^5.8}"            , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80<{}.{}}"    , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600<{}.{}}"          , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80>{}.{}}"    , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600>{}.{}}"          , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{:\xF0\x9F\x98\x80^{}.{}}"    , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{:\U0001F600^{}.{}}"          , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{0:\xF0\x9F\x98\x80<{1}.{1}}" , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{0:\U0001F600<{1}.{1}}"       , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{0:\xF0\x9F\x98\x80>{1}.{1}}" , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{0:\U0001F600>{1}.{1}}"       , k_UNICODE_IS_SUPPORTED);
        TPS(char,     "{0:\xF0\x9F\x98\x80^{1}.{1}}" , k_UNICODE_IS_SUPPORTED);
        TPS(wchar_t, L"{0:\U0001F600^{1}.{1}}"       , k_UNICODE_IS_SUPPORTED);
        #undef TPS
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS GENERATED BY THE COMPILER
        //   The only implemented functions of the formatters are `parse` and
        //   `format`, all the rest are generated by the compiler. We still
        //   want to test them, but combined these tests into one test case.
        //
        // Concerns:
        // 1. We can construct `bsl::formatter` types for all the string
        //    specializations.
        //
        // 2. We can construct `std::formatter` types for those partial
        //    specializations we expect to be aliased into the `std` namespace.
        //
        // 3. We can copy construct `bsl::formatter` types for all the string
        //    specializations.
        //
        // 4. We can swap two `bsl::formatter` types for all the string
        //    specializations.
        //
        // 5. We can assign `bsl::formatter` types for all the string
        //    specializations.
        //
        // Plan:
        // 1. Construct a `bsl::formatter` for each of the partial
        //    specializations. (C-1)
        //
        // 2. Construct a `std::formatter` for each of the partial
        //    specializations that we promote to `std`. (C-2)
        //
        // 3. Construct a `bsl::formatter` for each of the partial
        //    specializations, and copy it. (C-3)
        //
        // 4. Construct two `bsl::formatter`s for each of the partial
        //    specializations, and swap them. (C-4)
        //
        // 5. Construct two `bsl::formatter`s for each of the partial
        //    specializations, and assign one to the other. (C-5)
        //
        // Testing:
        //   formatter<t_TYPE, t_CHAR>();
        //   formatter<t_TYPE, t_CHAR>(const formatter<t_TYPE, t_CHAR>&);
        //   operator=(const formatter<t_TYPE, t_CHAR>&);
        //   swap(formatter<t_TYPE, t_CHAR>&, formatter<t_TYPE, t_CHAR>&);
        //   ~formatter<t_TYPE, t_CHAR>();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTIONS GENERATED BY THE COMPILER"
                            "\n===========================================\n");

        TestDriver< char  >::testCase2();
        TestDriver<wchar_t>::testCase2();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. That basic functionality appears to work as advertised before
        //    beginning testing in earnest:
        //    - default and copy constructors
        //    - assignment operator
        //    - primary manipulators, basic accessors
        //    - `operator==`, `operator!=`
        //
        // Plan:
        // 1. Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        bsl::formatter<const char *, char>         dummy1;
        bsl::formatter<bsl::string_view, char>     dummy2;
        bsl::formatter<bsl::wstring_view, wchar_t> dummy3;
        (void)dummy1;
        (void)dummy2;
        (void)dummy3;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        std::formatter<bsl::string, char>          dummy4;
        std::formatter<bsl::wstring, wchar_t>      dummy5;
        (void)dummy4;
        (void)dummy5;

        std::string val = std::format("String={:*^10.5}.",
                                      bsl::string("abcdefg"));
#endif

        bsl::string message;

        const wchar_t *winput = L"abcdefg";

        bool rv;

        rv = bslfmt::FormatterTestUtil<char>::testParseFormat<const char *>(
                                                              &message,
                                                              true,
                                                              "{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testParseFormat<
            const wchar_t *>(&message, true, L"{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testParseVFormat<char *>(
                                                              &message,
                                                              true,
                                                              "{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testParseVFormat<wchar_t *>(
                                                             &message,
                                                             true,
                                                             L"{0:*^{1}.{2}}");

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testEvaluateFormat(&message,
                                                            "**abcde***",
                                                            true,
                                                            "{0:*^{1}.{2}}",
                                                            "abcdefghi",
                                                            10,
                                                            5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateFormat(
                                                              &message,
                                                              L"**abcde***",
                                                              true,
                                                              L"{0:*^{1}.{2}}",
                                                              winput,
                                                              10,
                                                              5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<char>::testEvaluateVFormat(
                                                               &message,
                                                               "**abcde***",
                                                               true,
                                                               "{0:*^{1}.{2}}",
                                                               "abcdefghi",
                                                               10,
                                                               5);

        ASSERTV(message.c_str(), rv);

        rv = bslfmt::FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                                                              &message,
                                                              L"**abcde***",
                                                              true,
                                                              L"{0:*^{1}.{2}}",
                                                              winput,
                                                              10,
                                                              5);

        ASSERTV(message.c_str(), rv);

        // Testing `FormatterString_GraphemeCluster`

        {
            const char *fmt1 = (const char *)
                UTF8_LITERAL("\U00000067\U00000308");
            const int   len1 = (int)strlen(fmt1);

            bslfmt::FormatterString_GraphemeCluster cluster;
            cluster.extract(bslfmt::UnicodeCodePoint::e_UTF8,
                            (const void *)fmt1,
                            len1);

            ASSERT(0x67 == cluster.firstCodePointValue() );
            ASSERT(1    == cluster.firstCodePointWidth() );
            ASSERT(true == cluster.isValid()             );
            ASSERT(2    == cluster.numCodePoints()       );
            ASSERT(3    == cluster.numSourceBytes()      );

            const char *fmt2 = (const char *)
                UTF8_LITERAL("\U0001F408\U0000200D\U0001F7E7hello");
            const int   len2 = (int)strlen(fmt2);

            cluster.reset();
            cluster.extract(bslfmt::UnicodeCodePoint::e_UTF8,
                            (const void *)fmt2,
                            len2);

            ASSERT(0x1f408 == cluster.firstCodePointValue() );
            ASSERT(2       == cluster.firstCodePointWidth() );
            ASSERT(true    == cluster.isValid()             );
            ASSERT(3       == cluster.numCodePoints()       );
            ASSERT(11      == cluster.numSourceBytes()      );

            if (sizeof(wchar_t) == 2) {
                const wchar_t *fmt3 =
                       (const wchar_t *)L"\U0001F407\U0000200D\U0001F7E7hello";
                const int      len3 = (int)wcslen(fmt3);

                cluster.reset();
                cluster.extract(bslfmt::UnicodeCodePoint::e_UTF16,
                                (const void *)fmt3,
                                len3 * sizeof(wchar_t));

                ASSERT(10 == cluster.numSourceBytes());
            }
            else {
                const wchar_t *fmt3 =
                       (const wchar_t *)L"\U0001F407\U0000200D\U0001F7E7hello";
                const int      len3 = (int)wcslen(fmt3);

                cluster.reset();
                cluster.extract(bslfmt::UnicodeCodePoint::e_UTF32,
                                (const void *)fmt3,
                                len3 * sizeof(wchar_t));

                ASSERT(12 == cluster.numSourceBytes());
            }

            ASSERT(0x1f407 == cluster.firstCodePointValue() );
            ASSERT(2       == cluster.firstCodePointWidth() );
            ASSERT(true    == cluster.isValid()             );
            ASSERT(3       == cluster.numCodePoints()       );
        }
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
// Copyright 2023 Bloomberg Finance L.P.
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
