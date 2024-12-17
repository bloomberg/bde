// bslfmt_formattertestutil.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERTESTUTIL
#define INCLUDED_BSLFMT_FORMATTERTESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for testing custom formatters
//
//@CLASSES:
//  FormatterTestUtil: Formatter test utilities
//
//@DESCRIPTION: This component provides a utility to facilitate testing of
// `bsl::formatter` partial specializations where, due to hierarchical reasons,
// the testing cannot be performed using `bslfmt::format`.
//
// This component is for use within `bslfmt` only.
//
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
//  typedef bslfmt::FormatterTestUtil<char> TestUtilChar;
//
//  bsl::string message;
//  bool rv = TestUtilChar::testParseVFormat<int>(&message, true, "{0:}");
// ```
// Since the input data is correct, we get a positive result and the error
// message is empty:
// ```
//  assert(true == rv);
//  assert(message.empty());
// ```
// Then, we try to parse invalid format string and get the expected error
// message at the output:
// ```
//  rv = TestUtilChar::testParseVFormat<int>(&message, true, "{0");
//
//  assert(false == rv);
//  assert(bsl::string("Format string too short") == message);
//  message.clear();
// ```
// Now we check `format` formatter's method using `testEvaluateVFormat`
// function, that in addition to the parameters already mentioned, accepts a
// string with the expected formatting result and a value for formatting:
// ```
//  rv = TestUtilChar::testEvaluateVFormat<int>(&message,  // message
//                                              "5",       // expected result
//                                              true,      // oracle required
//                                              "{0:}",    // format string
//                                              5);        // value
//  assert(true == rv);
//  assert(message.empty());
// ```
// Finally verify that this function returns a false value if the expected and
// actual results do not match:
// ```
//  rv = TestUtilChar::testEvaluateVFormat<int>(&message,  // message
//                                              "7",       // incorrect result
//                                              true,      // oracle required
//                                              "{0:}",    // format string
//                                              5);        // value
//  assert(false == rv);
//  assert(message.starts_with("bslfmt result does not match"));
// ```

#include <bslscm_version.h>

#include <bslfmt_format_arg.h>
#include <bslfmt_format_args.h>
#include <bslfmt_format_context.h>
#include <bslfmt_formaterror.h>
#include <bslfmt_format_parsecontext.h>
#include <bslfmt_format_string.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_format_arg.h>
#include <bslfmt_format_args.h>
#include <bslfmt_format_context.h>
#include <bslfmt_format_parsecontext.h>
#include <bslfmt_format_string.h>
#include <bslfmt_formatterbase.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

using namespace BloombergLP;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING                              \
    std::format_string<t_TYPE>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_ONE_ARG                      \
    std::format_string<t_TYPE, t_ARG>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_TWO_ARGS                     \
    std::format_string<t_TYPE, t_ARG_1, t_ARG_2>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING                             \
    std::wformat_string<t_TYPE>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_ONE_ARG                     \
    std::wformat_string<t_TYPE, t_ARG>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_TWO_ARGS                    \
    std::wformat_string<t_TYPE, t_ARG_1, t_ARG_2>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING                        \
    std::basic_format_string<t_CHAR, t_TYPE>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_ONE_ARG                \
    std::basic_format_string<t_CHAR, t_TYPE, t_ARG>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_TWO_ARGS               \
    std::basic_format_string<t_CHAR, t_TYPE, t_ARG_1, t_ARG_2>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P                            \
    std::format_string<t_TYPE, int, int>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P                           \
    std::wformat_string<t_TYPE, int, int>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P                      \
    std::basic_format_string<t_CHAR, t_TYPE, int, int>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(ARG)            \
    ARG.get()
#else
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING                              \
    bslfmt::basic_format_string<char>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_ONE_ARG                      \
    bslfmt::basic_format_string<char>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_TWO_ARGS                     \
    bslfmt::basic_format_string<char>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING                             \
    bslfmt::basic_format_string<wchar_t>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_ONE_ARG                     \
    bslfmt::basic_format_string<wchar_t>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_TWO_ARGS                    \
    bslfmt::basic_format_string<wchar_t>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING                        \
    bslfmt::basic_format_string<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_ONE_ARG                \
    bslfmt::basic_format_string<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_TWO_ARGS               \
    bslfmt::basic_format_string<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P                            \
    bslfmt::basic_format_string<char>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P                           \
    bslfmt::basic_format_string<wchar_t>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P                      \
    bslfmt::basic_format_string<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(ARG)            \
    ARG.get()
#endif

namespace BloombergLP {
namespace bslfmt {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

                      // ==============================
                      // class FormatterTestUtil_Oracle
                      // ==============================

/// This template struct provides a namespace for a utility function that
/// creates a string containing the result of formatting done by standard
/// implementations.  This struct is meant for test purposes only.
template <class t_CHAR>
struct FormatterTestUtil_Oracle {
};

/// This is a specialization of `FormatterTestUtil_Oracle` template for `char`.
template <>
struct FormatterTestUtil_Oracle<char> {
  public:
    // CLASS METHODS

    /// Store to the specified `result` the string containing the result of
    /// formatting the specified `value` using a standard implementation
    /// with the specified `fmtStr` as a format specification and the
    /// specified `args` as value(s).
    template <class t_TYPE, class... t_ARGS>
    static void evaluateOracle(bsl::basic_string<char>      *result,
                               bsl::basic_string_view<char>  fmtStr,
                               t_TYPE&&                      value,
                               t_ARGS&&...                   args);
};

/// This is a specialization of `FormatterTestUtil_Oracle` template for
// `wchar_t`.
template <>
struct FormatterTestUtil_Oracle<wchar_t> {
  public:
    // CLASS METHODS

    /// Store to the specified `result` the string containing the result of
    /// formatting the specified `value` using a standard implementation
    /// with the specified `fmtStr` as a format specification and the
    /// specified `args` as value(s).
    template <class t_TYPE, class... t_ARGS>
    static void evaluateOracle(bsl::basic_string<wchar_t>      *result,
                               bsl::basic_string_view<wchar_t>  fmtStr,
                               t_TYPE&&                         value,
                               t_ARGS&&...                      args);
};
#endif

                      // ============================
                      // class FormatterTestUtil_Impl
                      // ============================

/// This struct provides a namespace for implementations of test functions that
/// verify the correctness of formatting performed by `bslfmt` tools, where
/// those implementations are common for all character types.  Those
/// implementations were moved to a separate struct to avoid code duplication.
/// Due to the limitations of template argument deduction, it is necessary to
/// explicitly specialize the `FormatterTestUtil` template for `char` and
/// `wchar_t`.  This struct contains that part of the implementation.
template <class t_CHAR>
struct FormatterTestUtil_Impl {
  private:
    // PRIVATE CLASS METHODS

    /// Parse the specified `fmtStr` and format the specified `value` using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`, passing the specified `mockContext`.  In case of success
    /// store the obtained string to the specified `result` and return `true`,
    /// otherwise store a description of the error in the specified output
    /// `message` and return `false`.
    template <class t_TYPE>
    static bool evaluateBslfmtResult(
                   bsl::string                                   *message,
                   bsl::basic_string<t_CHAR>                     *result,
                   bsl::basic_string_view<t_CHAR>                 fmtStr,
                   const t_TYPE&                                  value,
                   bslmf::MovableRef<MockFormatContext<t_CHAR> >  mockContext);

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmtStr` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE` and the specified `mockContext`.  Return `true` if these
    /// strings are equal, otherwise store a description of the error in the
    /// specified output `message` and return `false`.
    template <class t_TYPE>
    static bool testEvaluateVFormatBslfmtImpl(
                 bsl::string                                   *message,
                 bsl::basic_string_view<t_CHAR>                 expectedResult,
                 bsl::basic_string_view<t_CHAR>                 fmtStr,
                 BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)      value,
                 bslmf::MovableRef<MockFormatContext<t_CHAR> >  mockContext);

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` using a standard implementation, passing the
    // specified `fmtStr` as a format specification and the specified `args` as
    // value(s).  Return `true` if these strings are equal, otherwise store
    /// error description to the specified `message` and return `false`.
    template <class t_TYPE, class... t_ARGS>
    static bool testEvaluateVFormatStdImpl(
                                bsl::string                    *message,
                                bsl::basic_string_view<t_CHAR>  expectedResult,
                                bsl::basic_string_view<t_CHAR>  fmtStr,
                                t_TYPE&&                        value,
                                t_ARGS&&...                     args);
#endif

    /// Verify that the specified `fmtStr` has a valid structure and, if so,
    ///  parse it using a `bslfmt::formatter` specialization for the (template
    ///  parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`,
    ///  also parse the `fmtStr` using a standard library implementation.  If
    ///  the specified `expectedToFail` is true then an exception is excepted
    ///  to be thrown during parsing.  Return `true` if the `fmtStr` is valid
    ///  and the behavior related to `expectedToFail` meets expectations, and
    ///  return `false` otherwise.
    template <class t_TYPE>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 bool testParseFormatImpl(
                               bsl::string                    *message,
                               bool                            alsoTestOracle,
                               bsl::basic_string_view<t_CHAR>  fmtStr,
                               bool                            expectedToFail);

    /// Convert the specified `string` into its narrow format, if necessary,
    /// using the default `locale()`, using the question mark (`'?'`) character
    /// for characters that cannot be represented as narrow, and return the
    /// resulting narrow string.
    static bsl::string toNarrow(const bsl::string_view&  string);
    static bsl::string toNarrow(const bsl::wstring_view& string);

  public:
    // CLASS METHODS

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmtStr` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using a
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmtStr` is required to be a compile time constant expression.
    template <class t_TYPE>
    static bool testEvaluateFormat(
                bsl::string                                    *message,
                bsl::basic_string_view<t_CHAR>                  expectedResult,
                bool                                            alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING  fmtStr,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)       value);
    template <class t_TYPE, class t_ARG>
    static bool testEvaluateFormat(
        bsl::string                                            *message,
        bsl::basic_string_view<t_CHAR>                          expectedResult,
        bool                                                    alsoTestOracle,
        BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_ONE_ARG  fmtStr,
        BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)               value,
        BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)                arg);
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
       bsl::string                                             *message,
       bsl::basic_string_view<t_CHAR>                           expectedResult,
       bool                                                     alsoTestOracle,
       BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_TWO_ARGS  fmtStr,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)                value,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)               arg1,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)               arg2);

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmtStr` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using a
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmtStr` is **NOT** required to be a compile time constant expression.
    template <class t_TYPE>
    static bool testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value);
    template <class t_TYPE, class t_ARG>
    static bool testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<t_CHAR>             expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<t_CHAR>             fmtStr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg);
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);

    /// Verify that the specified `fmtStr` has a valid structure and, if so,
    /// parse it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmtStr` using a standard library implementation.  Return `true`
    /// if `fmtStr` is valid and the exception **IS** thrown during the
    /// parsing, and return `false` otherwise.
    template <class t_TYPE>
    static bool testParseFailure(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr);

    /// Verify that the specified `fmtStr` has a valid structure and, if so,
    /// parse it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmtStr` using a standard library implementation.  Return `true`
    /// if `fmtStr` is valid and **NO** exception is thrown during the parsing,
    /// and return `false` otherwise.  Note that `fmtStr` is required to be a
    /// compile time constant expression.
    template <class t_TYPE>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 bool testParseFormat(
              bsl::string                                      *message,
              bool                                              alsoTestOracle,
              BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P  fmtStr);

    /// Verify that the specified `fmtStr` has a valid structure and, if so,
    /// parse it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmtStr` using a standard library implementation.  Return `true`
    /// if `fmtStr` is valid and **NO** exception is thrown during the parsing,
    /// and return `false` otherwise.  Note that `fmtStr` is not required to be
    /// a compile time constant expression.
    template <class t_TYPE>
    static bool testParseVFormat(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr);
};

                      // =======================
                      // class FormatterTestUtil
                      // =======================

/// This struct provides a namespace for test functions that verify the
/// correctness of formatting performed by `bslfmt` tools.  Due to the
/// limitations of template argument deduction, it is necessary to explicitly
/// specialize the `FormatterTestUtil` template for `char` and `wchar_t`.
template <class t_CHAR>
struct FormatterTestUtil {
};

/// This is a specialization of `FormatterTestUtil` template for `char`.
template <>
struct FormatterTestUtil<char> {
  public:
    // CLASS METHODS

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmt` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmt` is required to be a compile time constant expression.
    template <class t_TYPE>
    static bool testEvaluateFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<char>               expectedResult,
                     bool                                       alsoTestOracle,
                     BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING   fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value);
    template <class t_TYPE, class t_ARG>
    static bool testEvaluateFormat(
              bsl::string                                      *message,
              bsl::basic_string_view<char>                      expectedResult,
              bool                                              alsoTestOracle,
              BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_ONE_ARG  fmt,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)         value,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)          arg);
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
             bsl::string                                       *message,
             bsl::basic_string_view<char>                       expectedResult,
             bool                                               alsoTestOracle,
             BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_TWO_ARGS  fmt,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)          value,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)         arg1,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)         arg2);

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmt` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmt` is **NOT** required to be a compile time constant expression.
    template <class t_TYPE>
    static bool testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<char>               expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<char>               fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value);

    template <class t_TYPE, class t_ARG>
    static bool testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<char>               expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<char>               fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg);

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<char>                expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<char>                fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmt` using a standard library implementation.  Return `true` if
    /// `fmt` is valid and the exception *IS* thrown during the parsing, and
    /// return `false` otherwise.
    template <class t_TYPE>
    static bool testParseFailure(bsl::string                  *message,
                                 bool                          alsoTestOracle,
                                 bsl::basic_string_view<char>  fmt);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse the `fmt` using a standard library implementation.  Return `true`
    /// if the `fmt` is valid and the exception is *NOT* thrown during the
    /// parsing, and return `false` otherwise.  Note that `fmt` is required to
    /// be a compile time constant expression.
    template <class t_TYPE>
    static BSLFMT_FORMATTER_TEST_CONSTEVAL bool testParseFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P  fmt);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse the `fmt` using a standard library implementation.  Return `true`
    /// if the `fmt` is valid and the exception is *NOT* thrown during the
    /// parsing, and return `false` otherwise.  Note that `fmt` can be
    /// determined at runtime.
    template <class t_TYPE>
    static bool testParseVFormat(bsl::string                  *message,
                                 bool                          alsoTestOracle,
                                 bsl::basic_string_view<char>  fmt);
};

/// This is a specialization of `FormatterTestUtil` template for `wchar_t`.
template <>
struct FormatterTestUtil<wchar_t> {
  public:
    // CLASS METHODS

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmt` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using a
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmt` is required to be a constant expression, known at compile time.
    template <class t_TYPE>
    static bool testEvaluateFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<wchar_t>            expectedResult,
                     bool                                       alsoTestOracle,
                     BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING  fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value);

    template <class t_TYPE, class t_ARG>
    static bool testEvaluateFormat(
             bsl::string                                       *message,
             bsl::basic_string_view<wchar_t>                    expectedResult,
             bool                                               alsoTestOracle,
             BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_ONE_ARG  fmt,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)          value,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)           arg);

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
            bsl::string                                        *message,
            bsl::basic_string_view<wchar_t>                     expectedResult,
            bool                                                alsoTestOracle,
            BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_TWO_ARGS  fmt,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)           value,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)          arg1,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)          arg2);

    /// Compare the specified `expectedResult` with the result of formatting
    /// the specified `value` with the specified `fmt` obtained using a
    /// `bslfmt::formatter` specialization for the (template parameter)
    /// `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also compare
    /// the `expectedResult` with the result of formatting obtained using a
    /// standard library implementation.  Optionally specify `arg`, `arg1` and
    /// `arg2` to be passed to the formatter as additional arguments.  Return
    /// `true` if these strings are equal, otherwise store a description of the
    /// error in the specified output `message` and return `false`.  Note that
    /// `fmt` can be determined at runtime.
    template <class t_TYPE>
    static bool testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<wchar_t>            expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<wchar_t>            fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value);

    template <class t_TYPE, class t_ARG>
    static bool testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<wchar_t>            expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<wchar_t>            fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg);

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<wchar_t>             expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<wchar_t>             fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmt` using a standard library implementation.  Return `true` if
    /// `fmt` is valid and the exception *IS* thrown during the parsing, and
    /// return `false` otherwise.
    template <class t_TYPE>
    static bool testParseFailure(
                              bsl::string                     *message,
                              bool                             alsoTestOracle,
                              bsl::basic_string_view<wchar_t>  fmt);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse the `fmt` using a standard library implementation.  Return `true`
    /// if the `fmt` is valid and the exception is *NOT* thrown during the
    /// parsing, and return `false` otherwise.  Note that the `fmt` is required
    /// to be a compile time constant expression.
    template <class t_TYPE>
    static BSLFMT_FORMATTER_TEST_CONSTEVAL bool testParseFormat(
                   bsl::string                                 *message,
                   bool                                         alsoTestOracle,
                   BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P  fmt);

    /// Verify that the specified `fmt` has a valid structure and, if so, parse
    /// it using a `bslfmt::formatter` specialization for the (template
    /// parameter) `t_TYPE`.  If the specified `alsoTestOracle` is `true`, also
    /// parse `fmt` using a standard library implementation.  Return `true` if
    /// `fmt` is valid and the exception is *NOT* thrown during the parsing,
    /// and return `false` otherwise.  Note that `fmt` is **NOT** required to
    /// be a compile time constant expression.
    template <class t_TYPE>
    static bool testParseVFormat(
                               bsl::string                     *message,
                               bool                             alsoTestOracle,
                               bsl::basic_string_view<wchar_t>  fmt);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

                // ------------------------------
                // class FormatterTestUtil_Oracle
                // ------------------------------

template <class t_TYPE, class... t_ARGS>
void FormatterTestUtil_Oracle<char>::evaluateOracle(
                                          bsl::basic_string<char>      *result,
                                          bsl::basic_string_view<char>  fmtStr,
                                          t_TYPE&&                      value,
                                          t_ARGS&&...                   args)
{
    *result = std::vformat(fmtStr, std::make_format_args(value, args...));
}

template <class t_TYPE, class... t_ARGS>
void FormatterTestUtil_Oracle<wchar_t>::evaluateOracle(
                                       bsl::basic_string<wchar_t>      *result,
                                       bsl::basic_string_view<wchar_t>  fmtStr,
                                       t_TYPE&&                         value,
                                       t_ARGS&&...                      args)
{
    *result = std::vformat(fmtStr, std::make_wformat_args(value, args...));
}

#endif

                      // ----------------------------
                      // class FormatterTestUtil_Impl
                      // ----------------------------

// PRIVATE CLASS METHODS
template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::evaluateBslfmtResult(
                    bsl::string                                   *message,
                    bsl::basic_string<t_CHAR>                     *result,
                    bsl::basic_string_view<t_CHAR>                 fmtStr,
                    const t_TYPE&                                  value,
                    bslmf::MovableRef<MockFormatContext<t_CHAR> >  mockContext)
{
    MockParseContext<t_CHAR> mpc(fmtStr, 3);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Empty format string";
        return false;                                                 // RETURN
    }

    if (fmtStr.front() != '{') {
        if (message)
            *message = "Opening brace missing";
        return false;                                                 // RETURN
    }
    fmtStr.remove_prefix(1);
    mpc.advance_to(mpc.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Format string too short";
        return false;                                                 // RETURN
    }

    if (fmtStr.front() != '0' && fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;                                                 // RETURN
    }

    if (fmtStr.front() == '0') {
        mpc.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
            *message = "Format string too short";
            return false;                                             // RETURN
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;                                             // RETURN
        }
    }
    else {
        (void) mpc.next_arg_id();
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Format string too short";
        return false;                                                 // RETURN
    }

    try {
        bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> formatter;

        mpc.advance_to(formatter.parse(mpc));

        if (mpc.begin() != mpc.end() &&
            (*mpc.begin() != '}' || mpc.begin() + 1 != mpc.end())) {
            if (message)
                *message = "Spec string has extra characters";
            return false;                                             // RETURN
        }

        MockFormatContext<t_CHAR>& mockFormatContext = mockContext;
        mockFormatContext.advance_to(
                    bsl::as_const(formatter).format(value, mockFormatContext));

        *result = bsl::basic_string<t_CHAR>(mockFormatContext.finalString());
    }
    catch (const bsl::format_error& e) {
        if (message) {
            *message = "Exception bsl::format_error: ";
            *message += e.what();
        }
        return false;                                                 // RETURN
    }

    return true;
}

template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateVFormatBslfmtImpl(
              bsl::string                                   *message,
              bsl::basic_string_view<t_CHAR>                 expectedResult,
              bsl::basic_string_view<t_CHAR>                 fmtStr,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)      value,
              bslmf::MovableRef<MockFormatContext<t_CHAR> >  mockFormatContext)
{
    bsl::basic_string<t_CHAR> bslfmtResult;

    bool rv = evaluateBslfmtResult(
                        message,
                        &bslfmtResult,
                        fmtStr,
                        value,
                        bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(mockFormatContext)));

    if (!rv)
        return false;                                                 // RETURN

    if (expectedResult != bslfmtResult) {
        if (message) {
            *message = "bslfmt result does not match: Format: \"";
            *message += toNarrow(fmtStr);
            *message += "\", result: \"";
            *message += toNarrow(bslfmtResult);
            *message += "\" != EXPECTED: \"";
            *message += toNarrow(expectedResult);
            *message += "\"";
        }
        return false;                                                 // RETURN
    }

    return true;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
template <class t_CHAR>
template <class t_TYPE, class... t_ARGS>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateVFormatStdImpl(
                                bsl::string                    *message,
                                bsl::basic_string_view<t_CHAR>  expectedResult,
                                bsl::basic_string_view<t_CHAR>  fmtStr,
                                t_TYPE&&                        value,
                                t_ARGS&&...                     args)
{
    bsl::basic_string<t_CHAR> stdResult;
    try {
        FormatterTestUtil_Oracle<t_CHAR>::evaluateOracle(
                                                &stdResult,
                                                fmtStr,
                                                std::forward<t_TYPE>(value),
                                                std::forward<t_ARGS>(args)...);
    }
    catch (const std::format_error& e) {
        if (message) {
            *message = "Exception std::format_error: ";
            *message += e.what();
        }
        return false;                                                 // RETURN
    }

    if (expectedResult != stdResult) {
        if (message) {
            *message = "oracle result does not match: Format: \"";
            *message += toNarrow(fmtStr);
            *message += "\", Oracle: \"";
            *message += toNarrow(stdResult);
            *message += "\" != EXPECTED: \"";
            *message += toNarrow(expectedResult);
            *message += "\"";
        }
        return false;                                                 // RETURN
    }

    return true;
}
#endif

template <class t_CHAR>
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP20 bool
FormatterTestUtil_Impl<t_CHAR>::testParseFormatImpl(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr,
                                bool                            expectedToFail)
{
    MockParseContext<t_CHAR> mpc(fmtStr, 3);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Empty format string";
        return false;                                                 // RETURN
    }

    if (fmtStr.front() != '{') {
        if (message)
            *message = "Opening brace missing";
        return false;                                                 // RETURN
    }
    fmtStr.remove_prefix(1);
    mpc.advance_to(mpc.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Format string too short";
        return false;                                                 // RETURN
    }

    if (fmtStr.front() != '0' && fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;                                                 // RETURN
    }

    bool haveArgIds;

    if (fmtStr.front() == '0') {
        mpc.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
            *message = "Format string too short";
            return false;                                             // RETURN
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;                                             // RETURN
        }
        haveArgIds = true;
    }
    else {
        (void)mpc.next_arg_id();
        haveArgIds = false;
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "Format string too short";
        return false;                                                 // RETURN
    }

    bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> bslFormatter;

    if (expectedToFail) {
        try {
            mpc.advance_to(bslFormatter.parse(mpc));

            if (message)
                *message = "bslfmt parsing failed to fail";
            return false;                                             // RETURN
        }
        catch (const bsl::format_error&) {
        }
    }
    else {
        try {
            mpc.advance_to(bslFormatter.parse(mpc));
        }
        catch (const bsl::format_error&) {
            if (message)
                *message = "bslfmt parsing failed";
            return false;                                             // RETURN
        }
    }

// We cannot do a consteval parse test on the MSVC standard library due to the
// way the MSVC library implements nested parameters.
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT) &&                         \
    !defined(BSLS_PLATFORM_CMP_MSVC)
    if (alsoTestOracle) {
        // TODO: this interface changes in C++26 so we will need to modify this
        // code accordingly then.
        std::basic_format_parse_context<t_CHAR> spc(fmtStr, 3);

        if (haveArgIds) {
            spc.check_arg_id(0);
        }
        else {
            (void)spc.next_arg_id();
        }
        std::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> stdFormatter;

        if (expectedToFail) {
            try {
                spc.advance_to(stdFormatter.parse(spc));

                if (message)
                    *message = "std parsing failed to fail";
                return false;                                         // RETURN
            }
            catch (const bsl::format_error&) {
            }
        }
        else {
            try {
                spc.advance_to(stdFormatter.parse(spc));
            }
            catch (const bsl::format_error&) {
                if (message)
                    *message = "std parsing failed";
                return false;                                         // RETURN
            }
        }
    }
#else
    (void)alsoTestOracle;
    (void)haveArgIds;
#endif

    return true;
}

template <class t_CHAR>
bsl::string
FormatterTestUtil_Impl<t_CHAR>::toNarrow(const bsl::string_view& string)
{
    return bsl::string(string.data(), string.size());
}

template <class t_CHAR>
bsl::string
FormatterTestUtil_Impl<t_CHAR>::toNarrow(const bsl::wstring_view& string)
{
    bsl::string       rv;
    const std::locale loc;

    typedef bsl::wstring_view::const_iterator Cit;
    for (Cit i = string.begin(); i != string.end(); ++i) {
        rv += std::use_facet<std::ctype<wchar_t> >(loc).narrow(*i, '?');
    }
    return rv;
}

// CLASS METHODS
template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateFormat(
                bsl::string                                    *message,
                bsl::basic_string_view<t_CHAR>                  expectedResult,
                bool                                            alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING  fmtStr,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)       value)
{
    bsl::basic_string_view<t_CHAR> formatStringView =
                 BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmtStr);

    return testEvaluateVFormat(message,
                               expectedResult,
                               alsoTestOracle,
                               formatStringView,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value));
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateFormat(
        bsl::string                                            *message,
        bsl::basic_string_view<t_CHAR>                          expectedResult,
        bool                                                    alsoTestOracle,
        BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_ONE_ARG  fmtStr,
        BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)               value,
        BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)                arg)
{
    bsl::basic_string_view<t_CHAR> formatStringView =
                 BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmtStr);

    return testEvaluateVFormat(message,
                               expectedResult,
                               alsoTestOracle,
                               formatStringView,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateFormat(
       bsl::string                                             *message,
       bsl::basic_string_view<t_CHAR>                           expectedResult,
       bool                                                     alsoTestOracle,
       BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_TWO_ARGS  fmtStr,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)                value,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)               arg1,
       BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)               arg2)
{
    bsl::basic_string_view<t_CHAR> formatStringView =
                 BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmtStr);

    return testEvaluateVFormat(message,
                               expectedResult,
                               alsoTestOracle,
                               formatStringView,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value)
{
    MockFormatContext<t_CHAR> mockFormatContext(value);
    bool rv = testEvaluateVFormatBslfmtImpl(
                               message,
                               expectedResult,
                               fmtStr,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               bslmf::MovableRefUtil::move(mockFormatContext));

    if (!rv) {
        return false;                                                 // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    if (alsoTestOracle) {
        rv = testEvaluateVFormatStdImpl(message,
                                        expectedResult,
                                        fmtStr,
                                        std::forward<t_TYPE>(value));
    }
#else
    (void) alsoTestOracle;
#endif

    return rv;
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<t_CHAR>             expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<t_CHAR>             fmtStr,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg)
{
    MockFormatContext<t_CHAR> mockFormatContext(value, arg);
    bool rv = testEvaluateVFormatBslfmtImpl(
                               message,
                               expectedResult,
                               fmtStr,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               bslmf::MovableRefUtil::move(mockFormatContext));

    if (!rv) {
        return false;                                                 // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    if (alsoTestOracle) {
        rv = testEvaluateVFormatStdImpl(message,
                                        expectedResult,
                                        fmtStr,
                                        std::forward<t_TYPE>(value),
                                        std::forward<t_ARG>(arg));
    }
#else
    (void) alsoTestOracle;
#endif

    return rv;
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil_Impl<t_CHAR>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    MockFormatContext<t_CHAR> mockFormatContext(value, arg1, arg2);
    bool rv = testEvaluateVFormatBslfmtImpl(
                               message,
                               expectedResult,
                               fmtStr,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               bslmf::MovableRefUtil::move(mockFormatContext));

    if (!rv) {
        return false;                                                 // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    if (alsoTestOracle) {
        rv = testEvaluateVFormatStdImpl(message,
                                        expectedResult,
                                        fmtStr,
                                        std::forward<t_TYPE>(value),
                                        std::forward<t_ARG_1>(arg1),
                                        std::forward<t_ARG_2>(arg2));
    }
#else
    (void) alsoTestOracle;
#endif

    return rv;
}

template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::testParseFailure(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr)
{
    return testParseFormatImpl<t_TYPE>(message, alsoTestOracle, fmtStr, true);
}

template <class t_CHAR>
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP20 bool
FormatterTestUtil_Impl<t_CHAR>::testParseFormat(
              bsl::string                                      *message,
              bool                                              alsoTestOracle,
              BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P  fmtStr)
{
    bsl::basic_string_view<t_CHAR> formatStringView =
                 BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmtStr);

    return testParseFormatImpl<t_TYPE>(message,
                                       alsoTestOracle,
                                       formatStringView,
                                       false);
}

template <class t_CHAR>
template <class t_TYPE>
bool FormatterTestUtil_Impl<t_CHAR>::testParseVFormat(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr)
{
    return testParseFormatImpl<t_TYPE>(message, alsoTestOracle, fmtStr, false);
}

                   // -----------------------
                   // class FormatterTestUtil
                   // -----------------------

// CLASS METHODS
template <class t_TYPE>
bool FormatterTestUtil<char>::testEvaluateFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<char>                expectedResult,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING    fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value)
{
    return FormatterTestUtil_Impl<char>::testEvaluateFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value));
}

template <class t_TYPE, class t_ARG>
bool FormatterTestUtil<char>::testEvaluateFormat(
              bsl::string                                      *message,
              bsl::basic_string_view<char>                      expectedResult,
              bool                                              alsoTestOracle,
              BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_ONE_ARG  fmt,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)         value,
              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)          arg)
{
    return FormatterTestUtil_Impl<char>::testEvaluateFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil<char>::testEvaluateFormat(
             bsl::string                                       *message,
             bsl::basic_string_view<char>                       expectedResult,
             bool                                               alsoTestOracle,
             BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_TWO_ARGS  fmt,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)          value,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)         arg1,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)         arg2)
{
    return FormatterTestUtil_Impl<char>::testEvaluateFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool FormatterTestUtil<char>::testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<char>               expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<char>               fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value)
{
    return FormatterTestUtil_Impl<char>::testEvaluateVFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value));
}

template <class t_TYPE, class t_ARG>
bool FormatterTestUtil<char>::testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<char>               expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<char>               fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  value,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg)
{
    return FormatterTestUtil_Impl<char>::testEvaluateVFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil<char>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<char>                expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<char>                fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return FormatterTestUtil_Impl<char>::testEvaluateVFormat(
                                 message,
                                 expectedResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool FormatterTestUtil<char>::testParseFailure(
                                  bsl::string                  *message,
                                  bool                          alsoTestOracle,
                                  bsl::basic_string_view<char>  fmt)
{
    return FormatterTestUtil_Impl<char>::testParseFailure<t_TYPE>(
                                                                message,
                                                                alsoTestOracle,
                                                                fmt);
}

template <class t_TYPE>
BSLFMT_FORMATTER_TEST_CONSTEVAL
bool FormatterTestUtil<char>::testParseFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P  fmt)
{
    return FormatterTestUtil_Impl<char>::testParseFormat<t_TYPE>(
                                 message,
                                 alsoTestOracle,
                                 fmt);
}

template <class t_TYPE>
bool FormatterTestUtil<char>::testParseVFormat(
                                  bsl::string                  *message,
                                  bool                          alsoTestOracle,
                                  bsl::basic_string_view<char>  fmt)
{
    return FormatterTestUtil_Impl<char>::testParseVFormat<t_TYPE>(
                                 message,
                                 alsoTestOracle,
                                 fmt);
}

template <class t_TYPE>
bool FormatterTestUtil<wchar_t>::testEvaluateFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<wchar_t>             expectedResult,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING   fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   dummyValue)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateFormat(
                            message,
                            expectedResult,
                            alsoTestOracle,
                            fmt,
                            BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue));
}

template <class t_TYPE, class t_ARG>
bool FormatterTestUtil<wchar_t>::testEvaluateFormat(
             bsl::string                                       *message,
             bsl::basic_string_view<wchar_t>                    expectedResult,
             bool                                               alsoTestOracle,
             BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_ONE_ARG  fmt,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)          dummyValue,
             BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)           arg)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateFormat(
                             message,
                             expectedResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil<wchar_t>::testEvaluateFormat(
            bsl::string                                        *message,
            bsl::basic_string_view<wchar_t>                     expectedResult,
            bool                                                alsoTestOracle,
            BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_TWO_ARGS  fmt,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)           dummyValue,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)          arg1,
            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)          arg2)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateFormat(
                             message,
                             expectedResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<wchar_t>            expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<wchar_t>            fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  dummyValue)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateVFormat(
                            message,
                            expectedResult,
                            alsoTestOracle,
                            fmt,
                            BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue));
}

template <class t_TYPE, class t_ARG>
bool FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                     bsl::string                               *message,
                     bsl::basic_string_view<wchar_t>            expectedResult,
                     bool                                       alsoTestOracle,
                     bsl::basic_string_view<wchar_t>            fmt,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)  dummyValue,
                     BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateVFormat(
                             message,
                             expectedResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool FormatterTestUtil<wchar_t>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<wchar_t>             expectedResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<wchar_t>             fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   dummyValue,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return FormatterTestUtil_Impl<wchar_t>::testEvaluateVFormat(
                             message,
                             expectedResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool FormatterTestUtil<wchar_t>::testParseFailure(
                               bsl::string                     *message,
                               bool                             alsoTestOracle,
                               bsl::basic_string_view<wchar_t>  fmt)
{
    return FormatterTestUtil_Impl<wchar_t>::testParseFailure<t_TYPE>(
                                                                message,
                                                                alsoTestOracle,
                                                                fmt);
}

template <class t_TYPE>
BSLFMT_FORMATTER_TEST_CONSTEVAL
bool FormatterTestUtil<wchar_t>::testParseFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P fmt)
{
    return FormatterTestUtil_Impl<wchar_t>::testParseFormat<t_TYPE>(
                             message,
                             alsoTestOracle,
                             fmt);
}

template <class t_TYPE>
bool FormatterTestUtil<wchar_t>::testParseVFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<wchar_t>             fmt)
{
    return FormatterTestUtil_Impl<wchar_t>::testParseVFormat<t_TYPE>(
                             message,
                             alsoTestOracle,
                             fmt);
}

}  // close namespace bslfmt
}  // close enterprise namespace

#undef BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING
#undef BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_ONE_ARG
#undef BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_TWO_ARGS
#undef BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING
#undef BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_ONE_ARG
#undef BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_TWO_ARGS
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_ONE_ARG
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_TWO_ARGS
#undef BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P
#undef BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER
#undef BSLFMT_FORMATTER_TEST_CONSTEVAL

#endif  // INCLUDED_BSLFMT_FORMATTERTESTUTIL

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
