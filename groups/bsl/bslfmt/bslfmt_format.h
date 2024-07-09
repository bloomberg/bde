// bslfmt_format.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT
#define INCLUDED_BSLFMT_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `format` implementation
//
//@CLASSES:
//  bsl::basic_format_args:          access to formatting arguments.
//  bsl::basic_format_context:       access to formatting state.
//  bsl::basic_format_parse_context: access to format string parsing state.
//  bsl::basic_format_string:        checked format string.
//  bsl::formatter:                  formatting rules by type.
//  bsl::format_args:                basic_format_args for char.
//  bsl::format_parse_context:       basic_format_parse_context for char.
//  bsl::format_string:              format_string for char.
//  bsl::format_to_n_result:         result type for format_to_n.
//  bsl::wformat_args:               basic_format_args for wchar_t.
//  bsl::wformat_parse_context:      basic_format_parse_context for wchar_t.
//  bsl::wformat_string:             format_string for wchar_t.
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: ISO C++ Standard, <format>
//
//@DESCRIPTION: This component will provide, in the `bsl` namespace, wrappers
// around the functions and types exposed by the standard <format> header where
// such is available, otherwise aliases to the `bslfmt` implementation.
//
// This will provide, where such are available, an allocator-aware wrapper
// around the `std::format`, `std::format_to`, `std::format_to_n`,
// `std::vformat`, and `std::vformat_to` functions as well as required helper
// functions and types. Where they are not available such as on older
// compilers, or when compiling C++17 and earlier, BSL implementation is
// provided.
//
// Where a BSL implementation is provided, functionality is limited to that
// provided by C++20 and excludes the following features:
// 
// - Support for locales other than the default ("C") locale.
// - Support for wide strings
// - Alternative date/time representations
// - Date/time directives not supported by the standard strftime function
// - Character escaping
// - Compile-time format string checking
//
///User-provided formatters
///------------------------
//
// User-provided formatters are supported by the BSL implementation, just as
// they are by the standard library implementation. However, in order for them
// to be compatible with both implementations, there are specific requirements,
// notably:
//
// - If you will define a formatter for your type 'T', do so in the same
//   component header that defines 'T' itself.  This avoids issues due to
//   users forgetting to include the header for the formatter.
// - Define `bsl::formatter<T>`
// - *DO NOT* define `std::formatter<T>`
// - Use template arguments for the format context and parse context
//   parameters. This is essential as the parameter type passed in will
//   depend upon underlying implementation.
// - The `parse` function should be constexpr in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
// An example of a user defined formatter is as follows:
//
// ```
// namespace bsl {
//
// template <class t_CHAR>
// struct formatter<UserDefinedType, t_CHAR> {
//     template <class t_PARSE_CONTEXT>
//     BSLS_KEYWORD_CONSTEXPR_CPP20
//     t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& pc)
//     {
//         // implementation goes here
//     }
//
//     template <class t_FORMAT_CONTEXT>
//     t_FORMAT_CONTEXT::iterator format(UserDefinedType   s,
//                                       t_FORMAT_CONTEXT& ctx) const
//     {
//         // implementation goes here
//     }
// };
//
// }  // close namespace bsl
// ```
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Simple integer formatting
/// - - - - - - - - - - - - - - - - - -
//
// Code block:
// ```
// #include <bsl_format.h>
// #include <bsl_iostring.h>
//
// bsl::string doFormat(int value)
// {
//     bsl::string res = bsl::format("{}", value);
//     return res;
// }
//
// int main()
// {
//     bsl::cout << doFormat(99) << bsl::endl;
//     return 0;
// }
//
// ```

#include <bslscm_version.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslfmt_formaterror.h>

#include <bslfmt_formatarg.h>
#include <bslfmt_formatargs.h>
#include <bslfmt_formatcontext.h>
#include <bslfmt_formatparsecontext.h>
#include <bslfmt_formatstring.h>

#include <bslfmt_formatimp.h>

#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterbool.h>
#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterfloating.h>
#include <bslfmt_formatterpointer.h>
#include <bslfmt_formatterstring.h>
#include <bslfmt_formatterutils.h>
#endif

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bsls_util.h>
#endif

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslstl_utility.h>
#endif

#include <iterator>
#include <string>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>
#include <locale>
#endif

#include <stdexcept>



#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace bsl {

using std::basic_format_arg;
using std::basic_format_args;
using std::basic_format_context;
using std::basic_format_parse_context;
using std::basic_format_string;
using std::format_args;
using std::format_error;
using std::format_parse_context;
using std::format_string;
using std::format_to;
using std::format_to_n;
using std::format_to_n_result;
using std::formatted_size;
using std::make_format_args;
using std::make_wformat_args;
using std::vformat_to;
using std::visit_format_arg;
using std::wformat_args;
using std::wformat_parse_context;
using std::wformat_string;

template <class... t_ARGS>
string format(format_string<t_ARGS...> fmtstr, t_ARGS&&... args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` and return the result as a `bsl::string`. In the event of an error the
    // exception `format_error` is thrown.

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` and return the result. In the event of an error the
    // exception `format_error` is thrown.

template <class... t_ARGS>
string format(const std::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc` and return the
    // result. In the event of an error the exception `format_error` is thrown.

template <class... t_ARGS>
wstring format(const std::locale&        loc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc` and return the
    // result. In the event of an error the exception `format_error` is thrown.

template <class... t_ARGS>
string format(allocator<char>          alloc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr`, using the specified `allocator` to supply memory (if
    // required), and return the result. In the event of an error the exception
    // `format_error` is thrown.

template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr`, using the specified `allocator` to supply memory (if
    // required), and return the result. In the event of an error the exception
    // `format_error` is thrown.

template <class... t_ARGS>
string format(allocator<char>          alloc,
              const std::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, using the
    // specified `allocator` to supply memory (if required), and return the
    // result. In the event of an error the exception `format_error` is thrown.

template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               const std::locale&        loc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, using the
    // specified `allocator` to supply memory (if required), and return the
    // result. In the event of an error the exception `format_error` is thrown.

string vformat(std::string_view fmtstr, format_args args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` and return the result. In the event of an error the
    // exception `format_error` is thrown.

wstring vformat(std::wstring_view fmtstr, wformat_args args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` and return the result. In the event of an error the
    // exception `format_error` is thrown.

string vformat(const std::locale& loc,
               std::string_view   fmtstr,
               format_args        args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc` and return the
    // result. In the event of an error the exception `format_error` is thrown.

wstring vformat(const std::locale& loc,
                std::wstring_view  fmtstr,
                wformat_args       args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc` and return the
    // result. In the event of an error the exception `format_error` is thrown.

string vformat(allocator<char>  alloc,
               std::string_view fmtstr,
               format_args      args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr`, using the specified `allocator` to supply memory (if
    // required), and return the result. In the event of an error the exception
    // `format_error` is thrown.

wstring vformat(allocator<wchar_t> alloc,
                std::wstring_view  fmtstr,
                wformat_args       args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr`, using the specified `allocator` to supply memory (if
    // required), and return the result. In the event of an error the exception
    // `format_error` is thrown.

string vformat(allocator<char>    alloc,
               const std::locale& loc,
               std::string_view   fmtstr,
               format_args        args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, using the
    // specified `allocator` to supply memory (if required), and return the
    // result. In the event of an error the exception `format_error` is thrown.

wstring vformat(allocator<wchar_t> alloc,
                const std::locale& loc,
                std::wstring_view  fmtstr,
                wformat_args       args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, using the
    // specified `allocator` to supply memory (if required), and return the
    // result. In the event of an error the exception `format_error` is thrown.

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(                               t_STRING                 *out,
                                              format_string<t_ARGS...>  fmtstr,
                                              t_ARGS&&...               args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(                              t_STRING                  *out,
                                             wformat_string<t_ARGS...>  fmtstr,
                                             t_ARGS&&...                args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.


template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING                 *out,
               const std::locale&        loc,
               format_string<t_ARGS...>  fmtstr,
               t_ARGS&&...               args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               const std::locale&         loc,
               wformat_string<t_ARGS...>  fmtstr,
               t_ARGS&&...                args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

void vformat_to(string *out, std::string_view fmtstr, format_args args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

void vformat_to(wstring *out, std::wstring_view fmtstr, wformat_args args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

void vformat_to(string             *out,
                const std::locale&  loc,
                std::string_view    fmtstr,
                format_args         args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

void vformat_to(wstring            *out,
                const std::locale&  loc,
                std::wstring_view   fmtstr,
                wformat_args        args);
    // Format the specified `args` according to the specification given by the
    // specified `fmtstr` in the locale of the specified `loc`, and write the
    // result of this operation into the string addressed by the specified
    // `out` parameter. In the event of an error the exception `format_error`
    // is thrown. Behavior is undefined if `out` does not point to a valid
    // `bsl::string` object.

// INLINE DEFINITIONS
template <class... t_ARGS>
string format(format_string<t_ARGS...> fmtstr, t_ARGS&&... args)
{
    return bsl::vformat(fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args)
{
    return bsl::vformat(fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(const std::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args)
{
    return bsl::vformat(loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(const std::locale&        loc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args)
{
    return bsl::vformat(loc, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char>          alloc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args)
{
    return bsl::vformat(alloc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<wchar_t>        alloc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args)
{
    return bsl::vformat(alloc, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char>          alloc,
              const std::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args)
{
    return bsl::vformat(alloc, loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<char>           alloc,
               const std::locale&        loc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args)
{
    return bsl::vformat(alloc, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
string vformat(std::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(std::wstring_view fmt, wformat_args args)
{
    wstring result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(const std::locale& loc, std::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(const std::locale& loc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
string vformat(allocator<char> alloc, std::string_view fmt, format_args args)
{
    string result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(allocator<char>    alloc,
               const std::locale& loc,
               std::string_view   fmt,
               format_args        args)
{
    string result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                const std::locale& loc,
                std::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING *out, format_string<t_ARGS...> fmtstr, t_ARGS&&...args)
{
    bsl::vformat_to(out, fmtstr.get(), make_format_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               wformat_string<t_ARGS...>  fmtstr,
               t_ARGS&&...                args)
{
    bsl::vformat_to(out, fmtstr.get(), make_wformat_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::string>)
void format_to(t_STRING                 *out,
               const std::locale&        loc,
               format_string<t_ARGS...>  fmtstr,
               t_ARGS&&...               args)
{
    bsl::vformat_to(out, loc, fmtstr.get(), make_format_args(args...));
}

template <class t_STRING, class... t_ARGS>
requires(bsl::is_same_v<t_STRING, bsl::wstring>)
void format_to(t_STRING                  *out,
               const std::locale&         loc,
               wformat_string<t_ARGS...>  fmtstr,
               t_ARGS&&...                args)
{
    bsl::vformat_to(out, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
void vformat_to(string *out, std::string_view fmt, format_args args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(wstring *out, std::wstring_view fmt, wformat_args args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(string             *out,
                const std::locale&  loc,
                std::string_view    fmt,
                format_args         args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}

inline
void vformat_to(wstring            *out,
                const std::locale&  loc,
                std::wstring_view   fmt,
                wformat_args        args)
{
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}


}  // close namespace bsl

#else

namespace bsl {
using BloombergLP::bslfmt::basic_format_arg;
using BloombergLP::bslfmt::basic_format_args;
using BloombergLP::bslfmt::basic_format_context;
using BloombergLP::bslfmt::basic_format_parse_context;
using BloombergLP::bslfmt::basic_format_string;
using BloombergLP::bslfmt::format_args;
using BloombergLP::bslfmt::format_error;
using BloombergLP::bslfmt::format_parse_context;
using BloombergLP::bslfmt::format_string;
using BloombergLP::bslfmt::format;
using BloombergLP::bslfmt::format_to;
using BloombergLP::bslfmt::format_to_n;
using BloombergLP::bslfmt::format_to_n_result;
using BloombergLP::bslfmt::formatted_size;
using BloombergLP::bslfmt::make_format_args;
using BloombergLP::bslfmt::make_wformat_args;
using BloombergLP::bslfmt::vformat;
using BloombergLP::bslfmt::vformat_to;
using BloombergLP::bslfmt::visit_format_arg;
using BloombergLP::bslfmt::wformat_args;
using BloombergLP::bslfmt::wformat_parse_context;
using BloombergLP::bslfmt::wformat_string;
}  // close namespace bsl

#endif  // defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#endif  // INCLUDED_BSLFMT_FORMAT

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
