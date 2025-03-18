// bslfmt_formattertestutil.t.cpp                                     -*-C++-*-
#include <bslfmt_formattertestutil.h>

#include <bslfmt_format_arg.h>         // Testing only
#include <bslfmt_formatparsecontext.h> // Testing only

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslfmt;


// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test provide a utility for testing `bsl::formatter`
// specializations.  Due to hierarchical reasons, these specializations cannot
// be included to this test driver.  But even emulating such a specialization,
// we cannot control its behavior using constructor parameters or setters,
// since the utility works independently with objects of these classes. We are
// forced to use global variables to pass instructions.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef FormatterTestUtil<char>    TestUtilChar;
typedef FormatterTestUtil<wchar_t> TestUtilWchar;

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

                         // =====================
                         // class TestInstruction
                         // =====================

/// This class contains necessary data to specify the desired mock formatter
/// behavior.
template <class t_CHAR>
class TestInstruction {
    // DATA
    bool                           d_initialized;
        // is object initialized

    bool                           d_parseFailureExpected;
        // is exception on parsing expected

    bool                           d_formatFailureExpected;
        // is exception on formatting expected

    bsl::basic_string_view<t_CHAR> d_expectedResult;
        // expected formatting result

    int                            d_numArgs;
        // number of arguments passed to the formating function

  public:
    // CREATORS

    /// Create a `TestInstruction` object having the default attribute values.
    TestInstruction()
    : d_initialized(false)
    , d_parseFailureExpected(false)
    , d_formatFailureExpected(false)
    , d_numArgs(0)
    {}

    // MANIPULATORS

    /// Setup the specified `parseFailureExpected`, `formatFailureExpected`,
    /// `expectedResult`, and `numArgs` as attribute values of this object.
    void setup(bool          parseFailureExpected,
               bool          formatFailureExpected,
               const t_CHAR *expectedResult,
               int           numArgs)
    {
        d_initialized = true;
        d_parseFailureExpected = parseFailureExpected;
        d_formatFailureExpected = formatFailureExpected;
        if (expectedResult) {
             d_expectedResult = expectedResult;
        }
        d_numArgs = numArgs;
    }

    /// Reset this object to the default state.
    void clear()
    {
        d_initialized           = false;
        d_parseFailureExpected  = false;
        d_formatFailureExpected = false;
        d_expectedResult        = bsl::basic_string_view<t_CHAR>();
        d_numArgs               = 0;
    }

    // ACCESSORS

    /// Return `true` if this object was initialized using `setup` method, and
    /// `false` otherwise.
    bool initialized() const
    {
        return d_initialized;
    }

    /// Return `true` if the exception is expected to be thrown from the
    /// formatter's `parse` method.
    bool parseFailureExpected() const
    {
        BSLS_ASSERT(d_initialized);
        return d_parseFailureExpected;
    }

    /// Return `true` if the exception is expected to be thrown from the
    /// formatter's `format` method.
    bool formatFailureExpected() const
    {
        BSLS_ASSERT(d_initialized);
        return d_formatFailureExpected;
    }

    /// Return the const reference to the expected formatting result.
    const bsl::basic_string_view<t_CHAR>& expectedResult() const
    {
        BSLS_ASSERT(d_initialized);
        return d_expectedResult;
    }

    /// Return the number of arguments passed to the
    int numberOfArguments() const
    {
        BSLS_ASSERT(d_initialized);
        return d_numArgs;
    }
};

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

/// These global variables are used to specify behavior of mock formatters for
/// particular test scenarios.
TestInstruction<char>    globalCharTestInstruction;
TestInstruction<wchar_t> globalWcharTestInstruction;

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

/// Return a reference to the global variable containing data specifying mock
/// formatter's behavior.
template <class t_CHAR>
TestInstruction<t_CHAR>& globalTestInstruction();

/// This is a specialization of `globalTestInstruction` template function for
/// `char`.
template<>
TestInstruction<char>& globalTestInstruction<char>()
{
    return globalCharTestInstruction;
}

/// This is a specialization of `globalTestInstruction` template function for
/// `wchar_t`.
template<>
TestInstruction<wchar_t>& globalTestInstruction<wchar_t>()
{
    return globalWcharTestInstruction;
}

/// This class is used to ensure that the mock formatter behavior is
/// individualized in a particular test scenario.
template<class t_CHAR>
class TestInstructionGuard
{
  public:
    // CREATORS

    /// Create a `TestInstructionGuard` object and setup the specified
    /// `parseFailureExpected`, `formatFailureExpected`, `expectedResult`, and
    /// `numArgs` as attribute values of the instruction for current test
    /// scenario.
    TestInstructionGuard(bool          parseFailureExpected,
                         bool          formatFailureExpected = false,
                         const t_CHAR *expectedResult        = 0,
                         int           numArgs               = 0)
    {
        BSLS_ASSERT(!globalTestInstruction<t_CHAR>().initialized());

        globalTestInstruction<t_CHAR>().setup(parseFailureExpected,
                                              formatFailureExpected,
                                              expectedResult,
                                              numArgs);
    }

    /// Delete this object and reset the status of the global test instruction
    /// to the default state.
    ~TestInstructionGuard()
    {
        BSLS_ASSERT(globalTestInstruction<t_CHAR>().initialized());
        globalTestInstruction<t_CHAR>().clear();
    }
};

}  // close unnamed namespace

namespace breathing_test{

                         // ===========================
                         // struct MockFormatterInteger
                         // ===========================

/// This type implements the formatter logic specific to the current test
/// driver.
template <class t_CHAR>
struct MockFormatterInteger {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    /// Throw a `bsl::format_error` exception if it is required by the current
    /// global test instruction and return an iterator, pointing to the
    /// end of the format string, obtained from the specified `parseContext`.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        if (globalTestInstruction<t_CHAR>().parseFailureExpected()) {
            throw bsl::format_error("MockFormatter parse() failure");
        }
        return parseContext.end();
    }

    // ACCESSORS

    /// Imitate the formatter behavior in accordance with the global test
    /// instruction, copy the predefined formatting result (if any) to the
    /// output that the output iterator of the specified `formatContext` points
    /// to and return this iterator.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         int,
                                         t_FORMAT_CONTEXT& formatContext) const
    {
        typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

        if (globalTestInstruction<t_CHAR>().formatFailureExpected()) {
            throw bsl::format_error("MockFormatter format() failure");
        }

        const bsl::basic_string_view<t_CHAR> &expectedResult =
                globalTestInstruction<t_CHAR>().expectedResult();
        if (!expectedResult.empty()) {
            typedef typename bsl::basic_string_view<t_CHAR>::const_iterator
                Iterator;
            for (Iterator it = expectedResult.begin();
                 it != expectedResult.end();
                 ++it) {
                *outIterator++ = *it;
            }
        }

        int numArgs = globalTestInstruction<t_CHAR>().numberOfArguments();
        for (int i = 0; i < numArgs; ++i)
        {
            ASSERTV(numArgs, i, formatContext.arg(i));
        }

        return outIterator;
    }
};

                         // ===========================
                         // struct MockFormatterBoolean
                         // ===========================

/// This type implements the formatter logic specific to the current test
/// driver. This is a stub that we are forced to use for testing due to
/// hierarchical reasons.  This particular specialization is used for testing
/// compile-time function versions.
template <class t_CHAR>
struct MockFormatterBoolean {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    /// Return an iterator, pointing to the end of the format string, obtained
    /// from the specified `parseContext`.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        return parseContext.end();
    }

    // ACCESSORS

    /// Copy the predefined formatting result to the output that the output
    /// iterator of the specified `formatContext` points to and return this
    /// iterator.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         bool,
                                         t_FORMAT_CONTEXT& formatContext) const
    {
        typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

        const size_t formattedValueLength = 4;
        t_CHAR       formattedValue[formattedValueLength] =
                                                          {'t', 'r', 'u', 'e'};

        for (size_t i = 0; i < formattedValueLength; ++i) {
            *outIterator++ = formattedValue[i];
        }
        return outIterator;
    }
};
}  // close breathing_test namespace


// ============================================================================
//                  MOCK `BSL::FORMATTER` SPECIALIZATIONS
//-----------------------------------------------------------------------------

namespace bsl {

/// Partial mock specialization of the `bsl::formatter` template for the `int`
/// type.
template <class t_CHAR>
struct formatter<int, t_CHAR> : breathing_test::MockFormatterInteger<t_CHAR> {
};

/// Partial mock specialization of the `bsl::formatter` template for the `bool`
/// type.
template <class t_CHAR>
struct formatter<bool, t_CHAR> : breathing_test::MockFormatterBoolean<t_CHAR> {
};

}  // close namespace bsl


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Usage
///-----
// This section illustrates intended usage of this component.
///Example: Testing an integer formatter
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to test `bsl::formatter` specialization for `int`.
//
// First, to test `parse` function, we can use `testParseVFormat` function,
// that accepts pointer to `bsl::string` in which to store an error message if
// any, a flag indicating our desire to see the result of the standard
// implementation and accordingly the format string itself:
// ```
    TestInstructionGuard<char> instructionGuard(false, false, "5");

    bsl::string message;
    bool rv = TestUtilChar::testParseVFormat<int>(&message, true, "{0:}");
// ```
// Since the input data is correct, we get a positive result and the error
// message is empty:
// ```
    ASSERT(true == rv);
    ASSERT(message.empty());
// ```
// Then, we try to parse invalid format string and get the expected error
// message at the output:
// ```
    rv = TestUtilChar::testParseVFormat<int>(&message, true, "{0");

    ASSERT(false == rv);
    ASSERT(bsl::string("Format string too short") == message);
    message.clear();
// ```
// Now we check `format` formatter's method using `testEvaluateVFormat`
// function, that in addition to the parameters already mentioned, accepts a
// string with the expected formatting result and a value for formatting:
// ```
    rv = TestUtilChar::testEvaluateVFormat(&message,  // message
                                           "5",       // expected fmt result
                                           true,      // oracle required
                                           "{0:}",    // format string
                                           5);        // value
    ASSERT(true == rv);
    ASSERT(message.empty());
// ```
// Finally verify that this function returns a false value if the expected and
// actual results do not match:
// ```
    rv = TestUtilChar::testEvaluateVFormat(&message,  // message
                                           "7",       // incorrect fmt result
                                           true,      // oracle required
                                           "{0:}",    // format string
                                           5);        // value
    ASSERT(false == rv);
    ASSERT(message.starts_with("bslfmt result does not match"));
// ```
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        int dummyArg = 0;

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            int         d_value;       // value to be formatted
        } CHAR_DATA[] = {
            //LINE  FORMAT        EXPECTED      VALUE
            //----  -----------   ---------     -----
            { L_,   "{0:}",       "0",          0    },
            { L_,   "{0:+}",      "+0",         0    },
            { L_,   "{0:-}",      "0",          0    },
            { L_,   "{0: }",      " 0",         0    },
            { L_,   "{:}",        "0",          0    },
            { L_,   "{:#05x}",    "0x00b",      11   },
        };

        const int NUM_CHAR_DATA = sizeof CHAR_DATA / sizeof *CHAR_DATA;

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            int            d_value;       // value to be formatted
        } WCHAR_DATA[] = {
            //LINE  FORMAT         EXPECTED       VALUE
            //----  -----------    -----------    -----
            { L_,   L"{0:}",       L"5",          5    },
            { L_,   L"{0:+}",      L"+5",         5    },
            { L_,   L"{0:-}",      L"5",          5    },
            { L_,   L"{0: }",      L" 5",         5    },
            { L_,   L"{:}",        L"5",          5    },
            { L_,   L"{:#05x}",    L"0x00c",      12   },
        };

        const int NUM_WCHAR_DATA = sizeof WCHAR_DATA / sizeof *WCHAR_DATA;

        static const struct {
            int            d_line;      // source line number
            const char    *d_format_p;  // char    format spec
            const wchar_t *d_wformat_p; // wchar_t format spec
            const char    *d_error_p;   // expected error message
        } WRONG_FORMAT_STRING_DATA[] = {
            //LINE  FORMAT   WFORMAT   EXPECTED
            //----  -------- --------- ---------------
            { L_,   "",      L"",      "Empty format string"                 },
            { L_,   "0:}",   L"0:}",   "Opening brace missing"               },
            { L_,   "{1:p}", L"{1:p}", "For testing, value must be arg 0 if"
                                                                " specified" },
            { L_,   "{a:p}", L"{a:p}", "For testing, value must be arg 0 if"
                                                                " specified" },
            { L_,   "{0",    L"{0",    "Format string too short"             },
            { L_,   "{01",   L"{01",   "Missing ':' separator"               },
            { L_,   "{0a",   L"{0a",   "Missing ':' separator"               },
            { L_,   "{",     L"{",     "Format string too short"             },
            { L_,   "{0:",   L"{0:",   "Format string too short"             },
            { L_,   "{:",    L"{:",    "Format string too short"             },
        };

        const int NUM_WRONG_FORMAT_STRING_DATA =
            sizeof WRONG_FORMAT_STRING_DATA / sizeof *WRONG_FORMAT_STRING_DATA;

        if (verbose)  printf("\tTesting runtime `parse` function\n");

        // The following test verifies that both functions return `false` and
        // write expected error message in response to invalid format
        // specification.

        for (int i = 0; i < NUM_WRONG_FORMAT_STRING_DATA; ++i) {
            const int      LINE     = WRONG_FORMAT_STRING_DATA[i].d_line;
            const char    *FORMAT   = WRONG_FORMAT_STRING_DATA[i].d_format_p;
            const wchar_t *WFORMAT  = WRONG_FORMAT_STRING_DATA[i].d_wformat_p;
            bsl::string    EXPECTED = WRONG_FORMAT_STRING_DATA[i].d_error_p;

            bsl::string message;

            // char
            bool rv = TestUtilChar::testParseFailure<int>(&message,
                                                          true,
                                                          FORMAT);
            ASSERTV(LINE, false == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);

            message.clear();

            rv = TestUtilChar::testParseVFormat<int>(&message, true, FORMAT);

            ASSERTV(LINE, false    == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);

            message.clear();

            // wchar_t
            rv = TestUtilWchar::testParseFailure<int>(&message, true, WFORMAT);

            ASSERTV(LINE, false    == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);

            message.clear();

            rv = TestUtilWchar::testParseVFormat<int>(&message, true, WFORMAT);

            ASSERTV(LINE, false    == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);
        }

        ASSERTV(NUM_CHAR_DATA, NUM_WCHAR_DATA,
                NUM_CHAR_DATA == NUM_WCHAR_DATA);

        // The following test checks the reaction of test functions to errors
        // that occur during parsing and to the successful parsing as well.  If
        // the `ERROR_EXPECTED` is true, mock formatter throws `format_error`
        // exception from its `parse` method.  Otherwise a successful parsing
        // of the valid format string is simulated.  The behavior of the
        // formatter is customized by changing the value of a global variable
        // (see `TestInstruction` definition).

        for (int i = 0; i < NUM_CHAR_DATA; ++i) {
            const int      LINE    = CHAR_DATA[i].d_line;
            const char    *FORMAT  = CHAR_DATA[i].d_format_p;
            const wchar_t *WFORMAT = WCHAR_DATA[i].d_format_p;

            for (int oracle = 0; oracle < 2; ++oracle) {
                bool ORACLE_REQUIRED = oracle ? true : false;

                for (int failure = 0; failure < 2; ++failure) {
                    bool        ERROR_EXPECTED  = failure ? true : false;
                    bool        PARSE_EXPECTED_RESULT = !ERROR_EXPECTED;
                    bsl::string PARSE_EXPECTED_MESSAGE =
                                 ERROR_EXPECTED ? "bslfmt parsing failed" : "";

                    // If we do not support standard formatting, our results do
                    // not depend on it.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

                    bool FAILURE_EXPECTED_RESULT =
                                            ERROR_EXPECTED && !ORACLE_REQUIRED;
                    bsl::string FAILURE_EXPECTED_MESSAGE =
                            ERROR_EXPECTED ? ORACLE_REQUIRED
                                                 ? "std parsing failed to fail"
                                                 : ""
                                           : "bslfmt parsing failed to fail";
#else
                    bool        FAILURE_EXPECTED_RESULT = ERROR_EXPECTED;
                    bsl::string FAILURE_EXPECTED_MESSAGE = ERROR_EXPECTED
                                             ? ""
                                             : "bslfmt parsing failed to fail";
#endif

                    bsl::string message;

                    // char
                    {
                        TestInstructionGuard<char> instructionGuard(
                                                               ERROR_EXPECTED);
                        bool rv = TestUtilChar::testParseFailure<int>(
                                                               &message,
                                                               ORACLE_REQUIRED,
                                                               FORMAT);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FAILURE_EXPECTED_RESULT,
                                FAILURE_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FAILURE_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                FAILURE_EXPECTED_MESSAGE == message);

                        message.clear();

                        rv = TestUtilChar::testParseVFormat<int>(
                                                               &message,
                                                               ORACLE_REQUIRED,
                                                               FORMAT);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                PARSE_EXPECTED_RESULT,
                                PARSE_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                PARSE_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                PARSE_EXPECTED_MESSAGE == message);

                        message.clear();
                    }

                    // wchar_t
                    {
                        TestInstructionGuard<wchar_t> instructionGuard(
                                                               ERROR_EXPECTED);
                        bool rv = TestUtilWchar::testParseFailure<int>(
                                                               &message,
                                                               ORACLE_REQUIRED,
                                                               WFORMAT);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FAILURE_EXPECTED_RESULT,
                                FAILURE_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FAILURE_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                FAILURE_EXPECTED_MESSAGE == message);

                        message.clear();

                        rv = TestUtilWchar::testParseVFormat<int>(
                                                               &message,
                                                               ORACLE_REQUIRED,
                                                               WFORMAT);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                PARSE_EXPECTED_RESULT,
                                PARSE_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                PARSE_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                PARSE_EXPECTED_MESSAGE == message);
                    }
                }
            }
        }

        if (verbose)  printf("\tTesting runtime `format` function\n");

        // The following test verifies that both functions return `false` and
        // write expected error message in response to invalid format
        // specification.

        for (int i = 0; i < NUM_WRONG_FORMAT_STRING_DATA; ++i) {
            const int      LINE     = WRONG_FORMAT_STRING_DATA[i].d_line;
            const char    *FORMAT   = WRONG_FORMAT_STRING_DATA[i].d_format_p;
            const wchar_t *WFORMAT  = WRONG_FORMAT_STRING_DATA[i].d_wformat_p;
            bsl::string    EXPECTED = WRONG_FORMAT_STRING_DATA[i].d_error_p;

            // We expect functions to fail on format string preprocessing.
            // Therefore we specify dummy expected result and value parameters.

            bsl::string message;

            // char
            bool rv = TestUtilChar::testEvaluateVFormat(&message,
                                                        "",     // dummy
                                                        true,
                                                        FORMAT,
                                                        0);     // dummy
            ASSERTV(LINE, false == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);

            message.clear();

            // wchar_t
            rv = TestUtilWchar::testEvaluateVFormat(&message,
                                                    L"",      // dummy
                                                    true,
                                                    WFORMAT,
                                                    0);       // dummy

            ASSERTV(LINE, false    == rv);
            ASSERTV(LINE, EXPECTED.c_str(), message.c_str(),
                    EXPECTED == message);
        }

        // The following test checks the reaction of test functions to errors
        // that occur during formatting and to the successful outcome as well.
        // If the `ERROR_EXPECTED` is true, mock formatter throws
        // `format_error` exception from its `format` method.  Otherwise a
        // successful formatting of the value is simulated.  In this scenario
        // the format string is always parsed successfully.
        //
        // First, the behavior of the `char` specializations is tested.

        for (int i = 0; i < NUM_CHAR_DATA; ++i) {
            const int   LINE     = CHAR_DATA[i].d_line;
            const char *FORMAT   = CHAR_DATA[i].d_format_p;
            const char *EXPECTED = CHAR_DATA[i].d_expected_p;
            const int   VALUE    = CHAR_DATA[i].d_value;

            for (int oracle = 0; oracle < 2; ++oracle) {
                bool ORACLE_REQUIRED = oracle ? true : false;

                for (int failure = 0; failure < 2; ++failure) {
                    bool        ERROR_EXPECTED  = failure ? true : false;
                    bool        FORMAT_EXPECTED_RESULT = !ERROR_EXPECTED;
                    bsl::string FORMAT_EXPECTED_MESSAGE =
                              ERROR_EXPECTED ? "Exception bsl::format_error: "
                                               "MockFormatter format() failure"
                                             : "";

                    bsl::string          message;

                    // Zero arguments
                    {
                        TestInstructionGuard<char> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED);

                        bool rv = TestUtilChar::testEvaluateVFormat(
                                                               &message,
                                                               EXPECTED,
                                                               ORACLE_REQUIRED,
                                                               FORMAT,
                                                               VALUE);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_RESULT,
                                FORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                FORMAT_EXPECTED_MESSAGE == message);
                    }
                    message.clear();

                    // One argument
                    {
                        TestInstructionGuard<char> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED,
                                                                1);

                        bool rv = TestUtilChar::testEvaluateVFormat(
                                                     &message,
                                                     EXPECTED,
                                                     ORACLE_REQUIRED,
                                                     FORMAT,
                                                     VALUE,
                                                     dummyArg);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_RESULT,
                                FORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                FORMAT_EXPECTED_MESSAGE == message);
                    }

                    // Two arguments
                    {
                        TestInstructionGuard<char> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED,
                                                                2);

                        bool rv = TestUtilChar::testEvaluateVFormat(
                                                               &message,
                                                               EXPECTED,
                                                               ORACLE_REQUIRED,
                                                               FORMAT,
                                                               VALUE,
                                                               dummyArg,
                                                               dummyArg);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_RESULT,
                                FORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                FORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                FORMAT_EXPECTED_MESSAGE == message);
                    }
                }
            }
        }

        // Then, the behavior of the `wchar_t` specializations is tested.

        for (int i = 0; i < NUM_WCHAR_DATA; ++i) {
            const int      LINE     = WCHAR_DATA[i].d_line;
            const wchar_t *WFORMAT  = WCHAR_DATA[i].d_format_p;
            const wchar_t *EXPECTED = WCHAR_DATA[i].d_expected_p;
            const int      VALUE    = WCHAR_DATA[i].d_value;

            for (int oracle = 0; oracle < 2; ++oracle) {
                bool ORACLE_REQUIRED = oracle ? true : false;

                for (int failure = 0; failure < 2; ++failure) {
                    bool        ERROR_EXPECTED  = failure ? true : false;
                    bool        WFORMAT_EXPECTED_RESULT = !ERROR_EXPECTED;
                    bsl::string WFORMAT_EXPECTED_MESSAGE =
                              ERROR_EXPECTED ? "Exception bsl::format_error: "
                                               "MockFormatter format() failure"
                                             : "";

                    bsl::string          message;

                    // Zero arguments
                    {
                        TestInstructionGuard<wchar_t> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED);

                        bool rv = TestUtilWchar::testEvaluateVFormat(
                                                               &message,
                                                               EXPECTED,
                                                               ORACLE_REQUIRED,
                                                               WFORMAT,
                                                               VALUE);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_RESULT,
                                WFORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                WFORMAT_EXPECTED_MESSAGE == message);
                    }
                    message.clear();

                    // One argument
                    {
                        TestInstructionGuard<wchar_t> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED,
                                                                1);

                        bool rv = TestUtilWchar::testEvaluateVFormat(
                                                               &message,
                                                               EXPECTED,
                                                               ORACLE_REQUIRED,
                                                               WFORMAT,
                                                               VALUE,
                                                               dummyArg);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_RESULT,
                                WFORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                WFORMAT_EXPECTED_MESSAGE == message);
                    }

                    // Two arguments
                    {
                        TestInstructionGuard<wchar_t> instructionGuard(
                                                                false,
                                                                ERROR_EXPECTED,
                                                                EXPECTED,
                                                                2);

                        bool rv = TestUtilWchar::testEvaluateVFormat(
                                                               &message,
                                                               EXPECTED,
                                                               ORACLE_REQUIRED,
                                                               WFORMAT,
                                                               VALUE,
                                                               dummyArg,
                                                               dummyArg);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_RESULT,
                                WFORMAT_EXPECTED_RESULT == rv);
                        ASSERTV(LINE,
                                ORACLE_REQUIRED,
                                ERROR_EXPECTED,
                                WFORMAT_EXPECTED_MESSAGE.c_str(),
                                message.c_str(),
                                WFORMAT_EXPECTED_MESSAGE == message);
                    }
                }
            }
        }

        if (verbose)  printf("\tTesting compile-time functions\n");

        // `parse`
        {
            bsl::string          message;

            bool rv = TestUtilChar::testParseFormat<bool>(&message,
                                                          true,
                                                          "{0:}");
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilWchar::testParseFormat<bool>(&message, true, L"{0:}");
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());
        }

        // `format`
        {
            bsl::string          message;

            bool rv = TestUtilChar::testEvaluateFormat(&message,
                                                       "true",  // exp result
                                                       true,    // oracle
                                                       "{0:}",  // fmt string
                                                       true);   // value
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"true",  // exp result
                                                   true,     // oracle required
                                                   L"{0:}",  // fmt
                                                   true);    // value
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "true",  // exp result
                                                  true,    // oracle required
                                                  "{0:}",  // fmt string
                                                  true,    // value
                                                  dummyArg);
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"true",  // exp result
                                                   true,     // oracle required
                                                   L"{0:}",  // fmt string
                                                   true,     // value
                                                   dummyArg);
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "true",  // exp result
                                                  true,    // oracle required
                                                  "{0:}",  // fmt string
                                                  true,    // value
                                                  dummyArg,
                                                  dummyArg);
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"true",  // exp result
                                                   true,     // oracle
                                                   L"{0:}",  // fmt string
                                                   true,     // value
                                                   dummyArg,
                                                   dummyArg);
            ASSERTV(true == rv);
            ASSERTV(message.c_str(), message.empty());
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
