// bslfmt_formattertestutil.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERTESTUTIL
#define INCLUDED_BSLFMT_FORMATTERTESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide invoker adaptors for 'bsl::function'
//
//@CLASSES:
//  FormatSpecification_Splitter: Utility to tokenize format specifications.
//  FormatSpecification_Splitter_Base: Base for FormatSpecification_Splitter.
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a mechanism to perform a first-pass
// split of a formatting string into its component parts in a way that is
// compatible with [format.string] and [time.format] in the Standard. No
// validation is performed by this component and further type-specific
// processing will be required prior to use.
// 
// This component is for private use only.

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_array.h>
#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatarg.h>
#include <bslfmt_formatcontext.h>
#include <bslfmt_formatstring.h>
#include <bslfmt_formatterbase.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>     // for oracle testing
#endif

using namespace BloombergLP;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING                              \
    std::format_string<t_TYPE, t_ARG_1, t_ARG_2>
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING                             \
    std::wformat_string<t_TYPE, t_ARG_1, t_ARG_2>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING                        \
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
    bsl::string_view
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING                             \
    bsl::wstring_view
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING                        \
    bsl::basic_string_view<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P                            \
    bsl::string_view
#define BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P                           \
    bsl::wstring_view
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P                      \
    bsl::basic_string_view<t_CHAR>
#define BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(ARG)            \
    ARG
#endif

namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR>
struct Formatter_MockParseContext {
  public:
    // TYPES
    typedef t_CHAR char_type;
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

    enum Indexing { e_UNKNOWN, e_MANUAL, e_AUTOMATIC };

  private:
    // DATA
    iterator d_begin;
    iterator d_end;
    Indexing d_indexing;
    size_t   d_next_arg_id;
    size_t   d_num_args;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit Formatter_MockParseContext(
                 bsl::basic_string_view<t_CHAR> fmt,
                 size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id();

    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT;

    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT;

    BSLS_KEYWORD_CONSTEXPR_CPP20 Indexing
    indexing() const BSLS_KEYWORD_NOEXCEPT;

  private:
    // NOT IMPLEMENTED
    Formatter_MockParseContext(
                       const Formatter_MockParseContext&) BSLS_KEYWORD_DELETED;
    Formatter_MockParseContext& operator=(
                       const Formatter_MockParseContext&) BSLS_KEYWORD_DELETED;
};

template <class t_CHAR>
class Formatter_MockFormatContext_Iterator {
  private:
    // DATA
    t_CHAR *d_ptr;
    t_CHAR *d_max;

  public:
    // TYPES
    typedef std::ptrdiff_t difference_type;
    typedef t_CHAR         value_type;

    // CREATORS
    Formatter_MockFormatContext_Iterator(t_CHAR *ptr, t_CHAR *max)
    : d_ptr(ptr)
    , d_max(max)
    {
    }

    // MANIPULATORS
    Formatter_MockFormatContext_Iterator& operator++()
    {
        d_ptr++;
        if (d_ptr >= d_max)
            BSLS_THROW(
                   format_error("Formatter_MockFormatContext buffer overrun"));
        return *this;
    }
    Formatter_MockFormatContext_Iterator operator++(int)
    {
        Formatter_MockFormatContext_Iterator copy = *this;
        ++*this;
        return copy;
    }

    // ACCESSORS
    t_CHAR& operator*() const { return *d_ptr; }

    t_CHAR *rawPointer() const { return d_ptr; }
};

template <class t_CHAR>
struct Formatter_MockFormatContext {
  public:
    // TYPES
    typedef basic_format_arg<basic_format_context<t_CHAR *, t_CHAR> > Arg;

    typedef Formatter_MockFormatContext_Iterator<t_CHAR> iterator;
    typedef t_CHAR                                       char_type;

  private:
    // PRIVATE TYPES

    enum { k_BUFFER_SIZE = 512 };

  private:
    // DATA
    Arg      d_arg_0;
    Arg      d_arg_1;
    Arg      d_arg_2;
    Arg      d_arg_3;

    t_CHAR   d_buffer[k_BUFFER_SIZE];
    iterator d_iterator;

  public:
    // CREATORS
    template <class t_ARG0>
    Formatter_MockFormatContext(const t_ARG0& arg_0)
    : d_buffer()
    , d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
    {
        bsl::array<Arg, 1> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0);
        d_arg_0 = Arg(arr[0]);
    }

    template <class t_ARG0, class t_ARG1>
    Formatter_MockFormatContext(const t_ARG0& arg_0, const t_ARG1& arg_1)
    : d_buffer()
    , d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
    {
        bsl::array<Arg, 2> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0, arg_1);
        d_arg_0 = Arg(arr[0]);
        d_arg_1 = Arg(arr[1]);
    }

    template <class t_ARG0, class t_ARG1, class t_ARG2>
    Formatter_MockFormatContext(const t_ARG0& arg_0,
                                const t_ARG1& arg_1,
                                const t_ARG2& arg_2)
    : d_buffer()
    , d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
    {
        bsl::array<Arg, 3> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr,
                                                      arg_0,
                                                      arg_1,
                                                      arg_2);
        d_arg_0 = Arg(arr[0]);
        d_arg_1 = Arg(arr[1]);
        d_arg_2 = Arg(arr[2]);
    }

    template <class t_ARG0, class t_ARG1, class t_ARG2, class t_ARG3>
    Formatter_MockFormatContext(const t_ARG0& arg_0,
                                const t_ARG1& arg_1,
                                const t_ARG2& arg_2,
                                const t_ARG3& arg_3)
    : d_buffer()
    , d_iterator(d_buffer, d_buffer + k_BUFFER_SIZE - 1)
    {
        bsl::array<Arg, 4> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr,
                                                     arg_0,
                                                     arg_1,
                                                     arg_2,
                                                     arg_3);
        d_arg_0 = Arg(arr[0]);
        d_arg_1 = Arg(arr[1]);
        d_arg_2 = Arg(arr[2]);
        d_arg_3 = Arg(arr[3]);
    }

    // ACCESSORS
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT
    {
        if (id == 0)
            return d_arg_0;
        if (id == 1)
            return d_arg_1;
        if (id == 2)
            return d_arg_2;
        if (id == 3)
            return d_arg_3;

        return Arg();
    }

    iterator out() const { return d_iterator; }

    bsl::basic_string_view<t_CHAR> finalString() const
    {
        return bsl::basic_string_view<t_CHAR>(
                                   d_buffer,
                                   size_t(d_iterator.rawPointer() - d_buffer));
    }

    // MANIPULATORS
    void advance_to(iterator it) { d_iterator = it; }
};

                      // -------------------------------------
                      // class Formatter_TestUtil_Impl<t_CHAR>
                      // -------------------------------------

template <class t_CHAR>
struct Formatter_TestUtil_Impl {
  public:
    // CLASS METHODS

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
                bsl::string                                    *message,
                bsl::basic_string_view<t_CHAR>                  desiredResult,
                bool                                            alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING  fmt,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)       value,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)      arg1,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)      arg2);

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              desiredResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);

    template <class t_TYPE>
    static bool testParseFailure(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmt);

    template <class t_TYPE>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 bool testParseFormat(
                bsl::string                                      *message,
                bool                                              alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P  fmt);

    template <class t_TYPE>
    static bool testParseVFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr);


  private:
    // PRIVATE CLASS METHODS
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool evaluateBslfmt(bsl::string                    *message,
                               bsl::basic_string<t_CHAR>      *result,
                               bsl::basic_string_view<t_CHAR>  fmtStr,
                               const t_TYPE&                   value,
                               const t_ARG_1&                  arg1,
                               const t_ARG_2&                  arg2);
};

                      // --------------------------------------------
                      // class Formatter_TestUtil_Oracle_Impl<t_CHAR>
                      // --------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
template <class t_CHAR>
struct Formatter_TestUtil_Oracle_Impl {
};

template <>
struct Formatter_TestUtil_Oracle_Impl<char> {
  public:
    // CLASS METHODS
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static void evaluateOracle(
                            bsl::basic_string<char>                    *result,
                            bsl::basic_string_view<char>                fmtStr,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);
};

template <>
struct Formatter_TestUtil_Oracle_Impl<wchar_t> {
  public:
    // CLASS METHODS
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static void evaluateOracle(
                            bsl::basic_string<wchar_t>                 *result,
                            bsl::basic_string_view<wchar_t>             fmtStr,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2);
};
#endif

                      // --------------------------------
                      // class Formatter_TestUtil<t_CHAR>
                      // --------------------------------

template <class t_CHAR>
struct Formatter_TestUtil {
};

template <>
struct Formatter_TestUtil<char> {
  public:
    // CLASS METHODS
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
          bsl::string                                *message,
          bsl::basic_string_view<char>                desiredResult,
          bool                                        alsoTestOracle,
          BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING    fmt,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1 = bsl::monostate(),
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2 = bsl::monostate());

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
          bsl::string                                *message,
          bsl::basic_string_view<char>                desiredResult,
          bool                                        alsoTestOracle,
          bsl::basic_string_view<char>                fmtStr,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1 = bsl::monostate(),
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2 = bsl::monostate());

    template <class t_TYPE>
    static bool testParseFailure(bsl::string                  *message,
                                 bool                          alsoTestOracle,
                                 bsl::basic_string_view<char>  fmt);

    template <class t_TYPE>
    static BSLFMT_FORMATTER_TEST_CONSTEVAL bool testParseFormat(
          bsl::string                                *message,
          bool                                        alsoTestOracle,
          BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P  fmt);

    template <class t_TYPE>
    static bool testParseVFormat(
          bsl::string                                *message,
          bool                                        alsoTestOracle,
          bsl::basic_string_view<char>                fmtStr);
};

template <>
struct Formatter_TestUtil<wchar_t> {
  public:
    // CLASS METHODS
    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateFormat(
          bsl::string                                *message,
          bsl::basic_string_view<wchar_t>             desiredResult,
          bool                                        alsoTestOracle,
          BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING   fmt,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1 = bsl::monostate(),
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2 = bsl::monostate());

    template <class t_TYPE, class t_ARG_1, class t_ARG_2>
    static bool testEvaluateVFormat(
          bsl::string                                *message,
          bsl::basic_string_view<wchar_t>             desiredResult,
          bool                                        alsoTestOracle,
          bsl::basic_string_view<wchar_t>             fmtStr,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1 = bsl::monostate(),
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2 = bsl::monostate());

    template <class t_TYPE>
    static bool testParseFailure(
                              bsl::string                     *message,
                              bool                             alsoTestOracle,
                              bsl::basic_string_view<wchar_t>  fmt);

    template <class t_TYPE>
    static BSLFMT_FORMATTER_TEST_CONSTEVAL bool testParseFormat(
          bsl::string                                *message,
          bool                                        alsoTestOracle,
          BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P fmt);

    template <class t_TYPE>
    static bool testParseVFormat(
          bsl::string                                *message,
          bool                                        alsoTestOracle,
          bsl::basic_string_view<wchar_t>             fmtStr);
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                // --------------------------------------------
                // class Formatter_TestUtil_Oracle_Impl<t_CHAR>
                // --------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
void Formatter_TestUtil_Oracle_Impl<char>::evaluateOracle(
                            bsl::basic_string<char>                    *result,
                            bsl::basic_string_view<char>                fmtStr,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    *result = std::vformat(
          fmtStr,
          std::make_format_args(BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2)));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
void Formatter_TestUtil_Oracle_Impl<wchar_t>::evaluateOracle(
                            bsl::basic_string<wchar_t>                 *result,
                            bsl::basic_string_view<wchar_t>             fmtStr,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    *result = std::vformat(
         fmtStr,
         std::make_wformat_args(BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2)));
}
#endif

                      // -------------------------------------
                      // class Formatter_TestUtil_Impl<t_CHAR>
                      // -------------------------------------

template <class t_CHAR>
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil_Impl<t_CHAR>::evaluateBslfmt(
                                       bsl::string                    *message,
                                       bsl::basic_string<t_CHAR>      *result,
                                       bsl::basic_string_view<t_CHAR>  fmtStr,
                                       const t_TYPE&                   value,
                                       const t_ARG_1&                  arg1,
                                       const t_ARG_2&                  arg2)
{
    Formatter_MockParseContext<t_CHAR> mpc(fmtStr, 3);

    if (fmtStr.size() == 0 || fmtStr.front() != '{') {
        if (message)
            *message = "opening brace missing";
        return false;
    }
    fmtStr.remove_prefix(1);
    mpc.advance_to(mpc.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() != '0' && fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;
    }

    if (fmtStr.front() == '0') {
        mpc.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
                *message = "format string too short.";
            return false;
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;
        }
    }
    else {
        mpc.next_arg_id();
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc.advance_to(mpc.begin() + 1);
    }

    try {
        bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f;

        mpc.advance_to(f.parse(mpc));

        if (mpc.begin() != mpc.end() &&
            (*mpc.begin() != '}' || mpc.begin() + 1 != mpc.end())) {
            if (message)
                *message = "Spec string has extra characters";
            return false;
        }

        Formatter_MockFormatContext<t_CHAR> mfc(value, arg1, arg2);

        mfc.advance_to(bsl::as_const(f).format(value, mfc));

        *result = bsl::basic_string<t_CHAR>(mfc.finalString());
    }
    catch (const bsl::format_error& e) {
        if (message)
            *message = e.what();
        return false;                                                 // RETURN
    }

    return true;
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil_Impl<t_CHAR>::testEvaluateFormat(
                bsl::string                                    *message,
                bsl::basic_string_view<t_CHAR>                  desiredResult,
                bool                                            alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING  fmt,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)       value,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)      arg1,
                BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)      arg2)
{
    bsl::basic_string_view<t_CHAR> fmtStr =
                    BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmt);

    return testEvaluateVFormat(message,
                               desiredResult,
                               alsoTestOracle,
                               fmtStr,
                               BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                               BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_CHAR>
template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil_Impl<t_CHAR>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<t_CHAR>              desiredResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    typedef bsl::basic_string<t_CHAR> Str;

    Str res_bde;

    bool rv = evaluateBslfmt(message, &res_bde, fmtStr, value, arg1, arg2);

    if (!rv)
        return false;

    if (desiredResult != res_bde) {
        if (message)
            *message = "bslfmt result does not match";
        return false;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

    if (alsoTestOracle) {
        Str res_std;
        try {
            Formatter_TestUtil_Oracle_Impl<t_CHAR>::evaluateOracle(
                                 &res_std,
                                 fmtStr,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
        }
        catch (const std::format_error& e) {
            if (message)
                *message = e.what();
            return false;                                             // RETURN
        }

        if (desiredResult != res_std) {
            if (message)
                *message = "oracle does not match";
            return false;
        }
    }

#else
    (void)alsoTestOracle;
#endif


    return true;
}

template <class t_CHAR>
template <class t_TYPE>
bool Formatter_TestUtil_Impl<t_CHAR>::testParseFailure(
                                bsl::string                    *message,
                                bool                            alsoTestOracle,
                                bsl::basic_string_view<t_CHAR>  fmtStr)
{
    Formatter_MockParseContext<t_CHAR> mpc1(fmtStr, 3);

    if (fmtStr.size() == 0 || fmtStr.front() != '{') {
        if (message)
            *message = "opening brace missing";
        return false;
    }
    fmtStr.remove_prefix(1);
    mpc1.advance_to(mpc1.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() != '0' &&
        fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;
    }

    bool haveArgIds;

    if (fmtStr.front() == '0') {
        mpc1.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
                *message = "format string too short.";
            return false;
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;
        }
        haveArgIds = true;
    }
    else {
        (void) mpc1.next_arg_id();
        haveArgIds = false;
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
    }

    bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f1;

    try {
        mpc1.advance_to(f1.parse(mpc1));

        if (message)
            *message = "bslfmt parsing failed to fail";
        return false;
    }
    catch (const bsl::format_error &) {
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    if (alsoTestOracle) {
        std::basic_format_parse_context<t_CHAR>                   spc(fmtStr);

        if (haveArgIds) {
            spc.check_arg_id(0);
        }
        else {
            (void) spc.next_arg_id();
        }
        std::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f2;

        try {
            spc.advance_to(f2.parse(spc));

            if (message)
            *message = "std parsing failed to fail";
            return false;
        }
        catch (const bsl::format_error&) {
        }
    }
#else
    (void)alsoTestOracle;
    (void)haveArgIds;
#endif

    return true;
}

template <class t_CHAR>
template <class t_TYPE>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Formatter_TestUtil_Impl<t_CHAR>::testParseFormat(
                bsl::string                                      *message,
                bool                                              alsoTestOracle,
                BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_P  fmt)
{
    bsl::basic_string_view<t_CHAR> fmtStr =
                    BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING_GETTER(fmt);

    Formatter_MockParseContext<t_CHAR> mpc1(fmtStr, 3);

    if (fmtStr.size() == 0 || fmtStr.front() != '{') {
        if (message)
            *message = "opening brace missing";
        return false;
    }
    fmtStr.remove_prefix(1);
    mpc1.advance_to(mpc1.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() != '0' && fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;
    }

    bool haveArgIds;

    if (fmtStr.front() == '0') {
        mpc1.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
            *message = "format string too short.";
            return false;
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;
        }
        haveArgIds = true;
    }
    else {
        (void)mpc1.next_arg_id();
        haveArgIds = false;
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
    }

    bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f1;

    try {
        mpc1.advance_to(f1.parse(mpc1));
    }
    catch (const bsl::format_error&) {
        if (message)
            *message = "bslfmt parsing failed";
        return false;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
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
        std::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f2;

        try {
            spc.advance_to(f2.parse(spc));
        }
        catch (const bsl::format_error&) {
            if (message)
                *message = "bslfmt parsing failed";
            return false;
        }
    }
#else
    (void)alsoTestOracle;
    (void)haveArgIds;
#endif

    return true;
}

template <class t_CHAR>
template <class t_TYPE>
bool Formatter_TestUtil_Impl<t_CHAR>::testParseVFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<t_CHAR>              fmtStr)
{
    Formatter_MockParseContext<t_CHAR> mpc1(fmtStr, 3);

    if (fmtStr.size() == 0 || fmtStr.front() != '{') {
        if (message)
            *message = "opening brace missing";
        return false;
    }
    fmtStr.remove_prefix(1);
    mpc1.advance_to(mpc1.begin() + 1);

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() != '0' && fmtStr.front() != ':' &&
        fmtStr.front() != '}') {
        if (message)
            *message = "For testing, value must be arg 0 if specified";
        return false;
    }

    bool haveArgIds;

    if (fmtStr.front() == '0') {
        mpc1.check_arg_id(0);
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
        if (fmtStr.size() == 0) {
            if (message)
            *message = "format string too short.";
            return false;
        }
        if (fmtStr.front() != ':' && fmtStr.front() != '}') {
            if (message)
                *message = "Missing ':' separator";
            return false;
        }
        haveArgIds = true;
    }
    else {
        (void)mpc1.next_arg_id();
        haveArgIds = false;
    }

    if (fmtStr.size() == 0) {
        if (message)
            *message = "format string too short.";
        return false;
    }

    if (fmtStr.front() == ':') {
        fmtStr.remove_prefix(1);
        mpc1.advance_to(mpc1.begin() + 1);
    }

    bsl::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f1;

    try {
        mpc1.advance_to(f1.parse(mpc1));
    }
    catch (const bsl::format_error&) {
        if (message)
            *message = "bslfmt parsing failed";
        return false;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    if (alsoTestOracle) {
        std::basic_format_parse_context<t_CHAR> spc(fmtStr);

        if (haveArgIds) {
            spc.check_arg_id(0);
        }
        else {
            (void)spc.next_arg_id();
        }
        std::formatter<typename bsl::decay<t_TYPE>::type, t_CHAR> f2;

        try {
            spc.advance_to(f2.parse(spc));
        }
        catch (const bsl::format_error&) {
            if (message)
                *message = "bslfmt parsing failed";
            return false;
        }
    }
#else
    (void)alsoTestOracle;
    (void)haveArgIds;
#endif

    return true;
}

                   // -------------------------------------
                   // class Formatter_TestUtil_Impl<t_CHAR>
                   // -------------------------------------

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil<char>::testEvaluateFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<char>                desiredResult,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING    fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return Formatter_TestUtil_Impl<char>::testEvaluateFormat(
                                 message,
                                 desiredResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil<char>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<char>                desiredResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<char>                fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   value,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return Formatter_TestUtil_Impl<char>::testEvaluateVFormat(
                                 message,
                                 desiredResult,
                                 alsoTestOracle,
                                 fmt,
                                 BSLS_COMPILERFEATURES_FORWARD(t_TYPE, value),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                                 BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool Formatter_TestUtil<char>::testParseFailure(
                                  bsl::string                  *message,
                                  bool                          alsoTestOracle,
                                  bsl::basic_string_view<char>  fmt)
{
    return Formatter_TestUtil_Impl<char>::testParseFailure<t_TYPE>(
                                                                message,
                                                                alsoTestOracle,
                                                                fmt);
}

template <class t_TYPE>
BSLFMT_FORMATTER_TEST_CONSTEVAL
bool Formatter_TestUtil<char>::testParseFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING_P  fmt)
{
    //return true;
    return Formatter_TestUtil_Impl<char>::testParseFormat<t_TYPE>(
                                 message,
                                 alsoTestOracle,
                                 fmt);
}

template <class t_TYPE>
bool Formatter_TestUtil<char>::testParseVFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<char>                fmt)
{
    return Formatter_TestUtil_Impl<char>::testParseVFormat<t_TYPE>(
                                 message,
                                 alsoTestOracle,
                                 fmt);
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<wchar_t>             desiredResult,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING   fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   dummyValue,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return Formatter_TestUtil_Impl<wchar_t>::testEvaluateFormat(
                             message,
                             desiredResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE, class t_ARG_1, class t_ARG_2>
bool Formatter_TestUtil<wchar_t>::testEvaluateVFormat(
                    bsl::string                                *message,
                    bsl::basic_string_view<wchar_t>             desiredResult,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<wchar_t>             fmt,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_TYPE)   dummyValue,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_1)  arg1,
                    BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG_2)  arg2)
{
    return Formatter_TestUtil_Impl<wchar_t>::testEvaluateVFormat(
                             message,
                             desiredResult,
                             alsoTestOracle,
                             fmt,
                             BSLS_COMPILERFEATURES_FORWARD(t_TYPE, dummyValue),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_1, arg1),
                             BSLS_COMPILERFEATURES_FORWARD(t_ARG_2, arg2));
}

template <class t_TYPE>
bool Formatter_TestUtil<wchar_t>::testParseFailure(
                               bsl::string                     *message,
                               bool                             alsoTestOracle,
                               bsl::basic_string_view<wchar_t>  fmt)
{
    return Formatter_TestUtil_Impl<wchar_t>::testParseFailure<t_TYPE>(
                                                                message,
                                                                alsoTestOracle,
                                                                fmt);
}

template <class t_TYPE>
BSLFMT_FORMATTER_TEST_CONSTEVAL
bool Formatter_TestUtil<wchar_t>::testParseFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING_P fmt)
{
    return Formatter_TestUtil_Impl<wchar_t>::testParseFormat<t_TYPE>(
                             message,
                             alsoTestOracle,
                             fmt);
}

template <class t_TYPE>
bool Formatter_TestUtil<wchar_t>::testParseVFormat(
                    bsl::string                                *message,
                    bool                                        alsoTestOracle,
                    bsl::basic_string_view<wchar_t>             fmt)
{
    return Formatter_TestUtil_Impl<wchar_t>::testParseVFormat<t_TYPE>(
                             message,
                             alsoTestOracle,
                             fmt);
}

                  // ----------------------------------------
                  // class Formatter_MockParseContext<t_CHAR>
                  // ----------------------------------------

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Formatter_MockParseContext<t_CHAR>::Formatter_MockParseContext(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
: d_begin(fmt.begin())
, d_end(fmt.end())
, d_indexing(e_UNKNOWN)
, d_next_arg_id(0)
, d_num_args(numArgs)
{
}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
Formatter_MockParseContext<t_CHAR>::advance_to(const_iterator it)
{
    d_begin = it;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 size_t
Formatter_MockParseContext<t_CHAR>::next_arg_id()
{
    if (d_indexing == e_MANUAL) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
    }
    if (d_next_arg_id >= d_num_args) {
        BSLS_THROW(format_error("number of conversion specifiers exceeds "
        "number of arguments"));
    }
    if (d_indexing == e_UNKNOWN) {
        d_indexing = e_AUTOMATIC;
    }
    return d_next_arg_id++;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
Formatter_MockParseContext<t_CHAR>::check_arg_id(size_t id)
{
    if (d_indexing == e_AUTOMATIC) {
        BSLS_THROW(format_error("mixing of automatic and manual indexing"));
    }
    if (id >= d_num_args) {
        BSLS_THROW(format_error("invalid argument index"));
    }
    if (d_indexing == e_UNKNOWN) {
        d_indexing = e_MANUAL;
    }
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
    typename Formatter_MockParseContext<t_CHAR>::const_iterator
    Formatter_MockParseContext<t_CHAR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_begin;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
    typename Formatter_MockParseContext<t_CHAR>::const_iterator
    Formatter_MockParseContext<t_CHAR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_end;
}

}  // close namespace bslfmt
}  // close enterprise namespace

#undef BSLFMT_FORMATTER_TEST_UTIL_FORMAT_STRING
#undef BSLFMT_FORMATTER_TEST_UTIL_WFORMAT_STRING
#undef BSLFMT_FORMATTER_TEST_UTIL_BASIC_FORMAT_STRING
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
