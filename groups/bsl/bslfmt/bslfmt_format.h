// bslfmt_format.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT
#define INCLUDED_BSLFMT_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

// Recommended practice for formatters (needs to be added to the google doc or
// something later):
//: o If you will define a formatter for your type 'T', do so in the same
//:   component header that defines 'T' itself.  This avoids issues due to
//:   users forgetting to include the header for the formatter.
//: o Define either 'std::formatter<T>' or 'bsl::formatter<T>', but not both.
//: o If you need to support C++03, then you should define 'bsl::formatter<T>'.
//:   Note that in C++20, we provide a partial specialization of
//:   'std::formatter<T>' that will delegate to 'bsl::formatter<T>'.
//: o If you only need to support C++20, then one possibility is to define
//:   'std::formatter<T>' only.  Note that 'bsl::format' will use
//:   'std::formatter<T>' in C++20 mode.  Note, however, that if you do this,
//:   then 'bsl::formatter<T>' will be disabled.
//: o If for some reason you need to define both 'bsl::formatter<T>' and
//:   'std::formatter<T>', then you should define 'bsl::formatter<T>' first and
//:   then define 'std::formatter<T>' to inherit from 'bsl::formatter<T>'.
//:   Doing it the other way around will lead to compilation errors because
//:   when 'std::formatter<T>' is instantiated, there will be a check to see
//:   whether it can delegate to 'bsl::formatter<T>', which will instantiate
//:   'bsl::formatter<T>', but this instantiation will then recursively depend
//:   on 'std::formatter<T>'.

#include <bslscm_version.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatarg.h>
#include <bslfmt_formatcontext.h>
#include <bslfmt_formatimp.h>
#endif

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#endif

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>
#endif

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslstl_array.h>
#include <bslstl_monostate.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>
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
string format(format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(const std::locale&, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(const std::locale&, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(allocator<char>, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(allocator<wchar_t>, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(allocator<char>, const std::locale&, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(allocator<wchar_t>, const std::locale&, wformat_string<t_ARGS...>, t_ARGS&&...);

string vformat(std::string_view fmt, format_args);

wstring vformat(std::wstring_view fmt, wformat_args);

string vformat(const std::locale&, std::string_view fmt, format_args);

wstring vformat(const std::locale&, std::wstring_view fmt, wformat_args);

string vformat(allocator<char>, std::string_view fmt, format_args);

wstring vformat(allocator<wchar_t>, std::wstring_view fmt, wformat_args);

string vformat(allocator<char>, const std::locale&, std::string_view fmt, format_args);

wstring vformat(allocator<wchar_t>, const std::locale&, std::wstring_view fmt, wformat_args);

template <class... t_ARGS>
void format_to(string *, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
void format_to(wstring *, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
void format_to(string *, const std::locale&, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
void format_to(wstring *, const std::locale&, wformat_string<t_ARGS...>, t_ARGS&&...);

void vformat_to(string *, std::string_view fmt, format_args);

void vformat_to(wstring *, std::wstring_view fmt, wformat_args);

void vformat_to(string *, const std::locale&, std::string_view fmt, format_args);

void vformat_to(wstring *, const std::locale&, std::wstring_view fmt, wformat_args);

// INLINE DEFINITIONS
template <class... t_ARGS>
string format(format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(const std::locale& loc, format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(const std::locale& loc, wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(loc, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char> alloc, format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(alloc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<wchar_t> alloc, wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(alloc, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
string format(allocator<char> alloc, const std::locale& loc, format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(alloc, loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<char> alloc, const std::locale& loc, wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    return bsl::vformat(alloc, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
string vformat(std::string_view fmt, format_args args) {
    string result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(std::wstring_view fmt, wformat_args args) {
    wstring result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(const std::locale& loc, std::string_view fmt, format_args args) {
    string result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(const std::locale& loc, std::wstring_view fmt, wformat_args args) {
    wstring result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
string vformat(allocator<char> alloc, std::string_view fmt, format_args args) {
    string result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc, std::wstring_view fmt, wformat_args args) {
    wstring result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(allocator<char> alloc, const std::locale& loc, std::string_view fmt, format_args args) {
    string result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc, const std::locale& loc, std::wstring_view fmt, wformat_args args) {
    wstring result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

template <class... t_ARGS>
void format_to(string *out, format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    bsl::vformat_to(out, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
void format_to(wstring *out, wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    bsl::vformat_to(out, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
void format_to(string *out, const std::locale& loc, format_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    bsl::vformat_to(out, loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
void format_to(wstring *out, const std::locale& loc, wformat_string<t_ARGS...> fmtstr, t_ARGS&&... args) {
    bsl::vformat_to(out, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
void vformat_to(string *out, std::string_view fmt, format_args args) {
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(wstring *out, std::wstring_view fmt, wformat_args args) {
    out->clear();
    std::vformat_to(std::back_inserter(*out), fmt, args);
}

inline
void vformat_to(string *out, const std::locale& loc, std::string_view fmt, format_args args) {
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}

inline
void vformat_to(wstring *out, const std::locale& loc, std::wstring_view fmt, wformat_args args) {
    out->clear();
    std::vformat_to(std::back_inserter(*out), loc, fmt, args);
}


}  // close namespace bsl

#else

namespace bsl {
using BloombergLP::bslfmt::basic_format_arg;
//using BloombergLP::bslfmt::basic_format_args;
using BloombergLP::bslfmt::basic_format_context;
using BloombergLP::bslfmt::basic_format_parse_context;
//using BloombergLP::bslfmt::basic_format_string;
using BloombergLP::bslfmt::format_args;
using BloombergLP::bslfmt::format_error;
using BloombergLP::bslfmt::format_parse_context;
//using BloombergLP::bslfmt::format_string;
using BloombergLP::bslfmt::format_to;
using BloombergLP::bslfmt::format_to_n;
using BloombergLP::bslfmt::format_to_n_result;
using BloombergLP::bslfmt::formatted_size;
using BloombergLP::bslfmt::make_format_args;
//using BloombergLP::bslfmt::make_wformat_args;
using BloombergLP::bslfmt::vformat_to;
using BloombergLP::bslfmt::visit_format_arg;
using BloombergLP::bslfmt::wformat_args;
using BloombergLP::bslfmt::wformat_parse_context;
//using BloombergLP::bslfmt::wformat_string;

using BloombergLP::bslfmt::format;
using BloombergLP::bslfmt::format_to;
using BloombergLP::bslfmt::vformat;

#if 0
template <class... t_ARGS>
string format(format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(const bslfmt::locale&, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(const bslfmt::locale&, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(allocator<char>, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
wstring format(allocator<wchar_t>, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
string format(allocator<char>,
              const bslfmt::locale&,
              format_string<t_ARGS...>,
              t_ARGS&&...);

template <class... t_ARGS>
wstring format(allocator<wchar_t>,
               const bslfmt::locale&,
               wformat_string<t_ARGS...>,
               t_ARGS&&...);

string vformat(bslfmt::string_view fmt, format_args);

wstring vformat(bslfmt::wstring_view fmt, wformat_args);

string vformat(const bslfmt::locale&, bslfmt::string_view fmt, format_args);

wstring vformat(const bslfmt::locale&, bslfmt::wstring_view fmt, wformat_args);

string vformat(allocator<char>, bslfmt::string_view fmt, format_args);

wstring vformat(allocator<wchar_t>, bslfmt::wstring_view fmt, wformat_args);

string vformat(allocator<char>,
               const bslfmt::locale&,
               bslfmt::string_view   fmt,
               format_args);

wstring vformat(allocator<wchar_t>,
                const bslfmt::locale&,
                bslfmt::wstring_view  fmt,
                wformat_args);

template <class... t_ARGS>
void format_to(string *, format_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
void format_to(wstring *, wformat_string<t_ARGS...>, t_ARGS&&...);

template <class... t_ARGS>
void format_to(string                   *,
               const bslfmt::locale&,
               format_string<t_ARGS...>,
               t_ARGS&&...);

template <class... t_ARGS>
void format_to(wstring                   *,
               const bslfmt::locale&,
               wformat_string<t_ARGS...>,
               t_ARGS&&...);

void vformat_to(string *, bslfmt::string_view fmt, format_args);

void vformat_to(wstring *, bslfmt::wstring_view fmt, wformat_args);

void vformat_to(string             *,
                const bslfmt::locale&,
                bslfmt::string_view    fmt,
                format_args);

void vformat_to(wstring            *,
                const bslfmt::locale&,
                bslfmt::wstring_view   fmt,
                wformat_args);

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
string format(const bslfmt::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args)
{
    return bsl::vformat(loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(const bslfmt::locale&        loc,
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
              const bslfmt::locale&       loc,
              format_string<t_ARGS...> fmtstr,
              t_ARGS&&...              args)
{
    return bsl::vformat(alloc, loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
wstring format(allocator<char>           alloc,
               const bslfmt::locale&        loc,
               wformat_string<t_ARGS...> fmtstr,
               t_ARGS&&...               args)
{
    return bsl::vformat(alloc, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
string vformat(bslfmt::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(bslfmt::wstring_view fmt, wformat_args args)
{
    wstring result;
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(const bslfmt::locale& loc, bslfmt::string_view fmt, format_args args)
{
    string result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(const bslfmt::locale& loc,
                bslfmt::wstring_view  fmt,
                wformat_args       args)
{
    wstring result;
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
string vformat(allocator<char> alloc, bslfmt::string_view fmt, format_args args)
{
    string result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                bslfmt::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, fmt, args);
    return result;
}

inline
string vformat(allocator<char>    alloc,
               const bslfmt::locale& loc,
               bslfmt::string_view   fmt,
               format_args        args)
{
    string result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

inline
wstring vformat(allocator<wchar_t> alloc,
                const bslfmt::locale& loc,
                bslfmt::wstring_view  fmt,
                wformat_args       args)
{
    wstring result(alloc);
    bsl::vformat_to(&result, loc, fmt, args);
    return result;
}

template <class... t_ARGS>
void format_to(string *out, format_string<t_ARGS...> fmtstr, t_ARGS&&... args)
{
    bsl::vformat_to(out, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
void format_to(wstring *out, wformat_string<t_ARGS...> fmtstr, t_ARGS&&...args)
{
    bsl::vformat_to(out, fmtstr.get(), make_wformat_args(args...));
}

template <class... t_ARGS>
void format_to(string                   *out,
               const bslfmt::locale&        loc,
               format_string<t_ARGS...>  fmtstr,
               t_ARGS&&...               args)
{
    bsl::vformat_to(out, loc, fmtstr.get(), make_format_args(args...));
}

template <class... t_ARGS>
void format_to(wstring                   *out,
               const bslfmt::locale&         loc,
               wformat_string<t_ARGS...>  fmtstr,
               t_ARGS&&...                args)
{
    bsl::vformat_to(out, loc, fmtstr.get(), make_wformat_args(args...));
}

inline
void vformat_to(string *out, bslfmt::string_view fmt, format_args args)
{
    out->clear();
    bslfmt::vformat_to(bslfmt::back_inserter(*out), fmt, args);
}

inline
void vformat_to(wstring *out, bslfmt::wstring_view fmt, wformat_args args)
{
    out->clear();
    bslfmt::vformat_to(bslfmt::back_inserter(*out), fmt, args);
}

inline
void vformat_to(string             *out,
                const bslfmt::locale&  loc,
                bslfmt::string_view    fmt,
                format_args         args)
{
    out->clear();
    bslfmt::vformat_to(bslfmt::back_inserter(*out), loc, fmt, args);
}

inline
void vformat_to(wstring            *out,
                const bslfmt::locale&  loc,
                bslfmt::wstring_view   fmt,
                wformat_args        args)
{
    out->clear();
    bslfmt::vformat_to(bslfmt::back_inserter(*out), loc, fmt, args);
}
#endif // 0


}  // close namespace bsl

#endif  // defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#endif  // INCLUDED_BSLSTL_FORMAT

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
