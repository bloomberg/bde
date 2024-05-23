// bslfmt_formatutil.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATUTIL
#define INCLUDED_BSLFMT_FORMATUTIL

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

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>

#if !defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslalg_numericformatterutil.h>

#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#endif

#if !defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>
#endif

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#if !defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
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
#else
#include <stdexcept>
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Wed Oct 18 11:10:11 2023
// Command line: sim_cpp11_features.pl bslstl_format.h
# define COMPILING_BSLSTL_FORMAT_H
# include <bslstl_format_cpp03.h>
# undef COMPILING_BSLSTL_FORMAT_H
#else

namespace bslfmt {
  #if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    using bslfmt_FormatUtil_FormatError = std::format_error;
  #else
    class bslfmt_FormatUtil_FormatError : public std::runtime_error {
      public:
        // CREATORS
        explicit bslfmt_FormatUtil_FormatError(const std::string& what_arg)
        : runtime_error(what_arg)
        {
        }

        explicit bslfmt_FormatUtil_FormatError(const char *what_arg)
        : runtime_error(what_arg)
        {
        }

        // If a 'bsl::string' is passed to the 'std::string' constructor, two
        // copies occur (one to initialize 'what_arg', and one to initialize the
        // internal reference-counted string).  This constructor ensures that only
        // a single copy needs to be performed.
        explicit bslfmt_FormatUtil_FormatError(const bsl::string& what_arg)
        : runtime_error(what_arg.c_str())
        {
        }
    };
  #endif
}

namespace bslfmt {
template <class t_FORMATTER, class = void>
struct bslfmt_format_IsStdAliasingEnabled : bsl::true_type {};

template <class t_FORMATTER>
struct bslfmt_format_IsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::bslfmt_format_PreventStdAliasing>
: bsl::false_type {
};

template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    typedef void bslfmt_format_PreventStdAliasing;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bslfmt

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace bslfmt {
template <class t_TYPE>
constexpr bool bslfmt_format_IsStdBasicString = false;

template <class charT, class traits, class Allocator>
constexpr bool bslfmt_format_IsStdBasicString<
    std::basic_string<charT, traits, Allocator> > = true;

template <class t_TYPE>
constexpr bool bslfmt_format_IsStdBasicStringView = false;

template <class charT, class traits>
constexpr bool bslfmt_format_IsStdBasicStringView<
    std::basic_string_view<charT, traits> > = true;
}  // close namespace bslfmt

namespace std {
template <class t_ARG, class t_CHAR>
requires(
      !std::is_arithmetic_v<t_ARG> &&
      !std::is_same_v<t_ARG, std::nullptr_t> &&
      !std::is_same_v<t_ARG, void *> &&
      !std::is_same_v<t_ARG, const void *> &&
      !std::is_same_v<t_ARG, t_CHAR *> &&
      !std::is_same_v<t_ARG, const t_CHAR *> &&
      !std::is_same_v<std::remove_extent_t<t_ARG>, const t_CHAR> &&
      !bslfmt::bslfmt_format_IsStdBasicString<t_ARG> &&
      !bslfmt::bslfmt_format_IsStdBasicStringView<t_ARG> &&
       bslfmt::bslfmt_format_IsStdAliasingEnabled<
                       bslfmt::formatter<t_ARG, t_CHAR> >::value)
struct formatter<t_ARG, t_CHAR>
: bslfmt::formatter<t_ARG, t_CHAR> {
};
}  // close namespace std
#endif

#define BSL_FORMAT_CONSTEXPR constexpr

#if 0
namespace bslfmt {
template <>
struct formatter<int, char> {
    template <class t_PARSE_CONTEXT>
    BSL_FORMAT_CONSTEXPR typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator format(
                                              int                     x,
                                              t_CONTEXT<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return std::copy(buf, result, fc.out());
    }
};

template <>
struct formatter<bsl::string_view, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator format(
                                              bsl::string_view        sv,
                                              t_CONTEXT<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};
// FORMATTER SPECIALIZATIONS
template <>
struct formatter<bsl::string, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator format(
                                              const bsl::string&      str,
                                              t_CONTEXT<t_OUT, char>& fc) const
    {
        return std::copy(str.begin(), str.end(), fc.out());
    }
};
}
#endif


#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace bslfmt {

//using bslfmt_FormatUtil_BasicFormatArg     = std::basic_format_arg;

using bslfmt_FormatUtil_Alias_FormatError = std::format_error;

template <class t_OUT, class t_CHAR>
using bslfmt_FormatUtil_Alias_BasicFormatContext = std::basic_format_context<t_OUT, t_CHAR>;

template <class t_CHAR>
using bslfmt_FormatUtil_Alias_BasicFormatParseContext =
                                               std::basic_format_parse_context<t_CHAR>;

using bslfmt_FormatUtil_Alias_FormatParseContext = bslfmt_FormatUtil_Alias_BasicFormatParseContext<char>;
using bslfmt_FormatUtil_Alias_WFormatParseContext = bslfmt_FormatUtil_Alias_BasicFormatParseContext<wchar_t>;

template <class t_CHAR, class... t_ARGS>
using bslfmt_FormatUtil_Alias_BasicFormatString =
                                   std::basic_format_string<t_CHAR, t_ARGS...>;

template <class... t_ARGS>
using bslfmt_FormatUtil_Alias_FormatString =
      bslfmt_FormatUtil_Alias_BasicFormatString<
                 char,
                 typename bsl::type_identity<t_ARGS>::type...>;

template <class... t_ARGS>
using bslfmt_FormatUtil_Alias_WFormatString =
      bslfmt_FormatUtil_Alias_BasicFormatString<
                 wchar_t,
                 typename bsl::type_identity<t_ARGS>::type...>;

using bslfmt_FormatUtil_Alias_DefaultFormatContext = std::format_context;
using bslfmt_FormatUtil_Alias_DefaultWFormatContext = std::wformat_context;

template <class t_CONTEXT = bslfmt_FormatUtil_Alias_DefaultFormatContext,
          class... t_ARGS>
using bslfmt_FormatUtil_Alias_FormatArgStore = decltype(std::make_format_args<t_CONTEXT>(std::declval<t_ARGS&>()...));

//template <class t_CONTEXT = bslfmt_FormatUtil_Alias_DefaultWFormatContext,
//          class... t_ARGS>
//using bslfmt_FormatUtil_Alias_WFormatArgStore = decltype(std::make_wformat_args(std::declval<t_ARGS>()...));

template <class t_CONTEXT>
using bslfmt_FormatUtil_Alias_BasicFormatArgs = std::basic_format_args<t_CONTEXT>;

using bslfmt_FormatUtil_Alias_FormatArgs =
             bslfmt_FormatUtil_Alias_BasicFormatArgs<bslfmt_FormatUtil_Alias_DefaultFormatContext>;

using bslfmt_FormatUtil_Alias_WFormatArgs =
             bslfmt_FormatUtil_Alias_BasicFormatArgs<bslfmt_FormatUtil_Alias_DefaultWFormatContext>;

//typedef basic_format_context<bslfmt_FormatUtil_Alias_OutputIteratorRef<char>, char>
//    bslfmt_FormatUtil_Alias_DefaultFormatContext;

struct FormatUtil {

    //template <class t_CONTEXT, class... t_ARGS>
    //class bslfmt_FormatUtil_Alias_FormatArgStore {
    //    // TODO: Make all members private

    //  public:
    //    // DATA
    //    array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;

    //    // PRIVATE CREATORS
    //    explicit bslfmt_FormatUtil_Alias_FormatArgStore(
    //         const array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
    //        BSLS_KEYWORD_NOEXCEPT : d_args(args)
    //    {
    //    }
    //};

    //template <class t_CONTEXT, class... t_ARGS>
    //bslfmt_FormatUtil_Alias_FormatArgStore<t_CONTEXT, t_ARGS...>
    //bslfmt_FormatUtil_Alias_MakeFormatArgs(t_ARGS&... fmt_args)
    //{
    //    // Use the form of braced initialization that is valid in C++03
    //    array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array = {
    //        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    //    return bslfmt_FormatUtil_Alias_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
    //}

    template <class... t_ARGS>
    static bslfmt_FormatUtil_Alias_FormatArgStore<bslfmt_FormatUtil_Alias_DefaultFormatContext, t_ARGS...>
    make_format_args(t_ARGS&... fmt_args)
    {
        static constexpr int dummy[] = {0, ((void)bslfmt::formatter<std::remove_cvref_t<t_ARGS>, char>{}, 0)...};
        (void)dummy;

        return std::make_format_args<bslfmt_FormatUtil_Alias_DefaultFormatContext>(
                                                                  fmt_args...);
        //return bslfmt_FormatUtil_Alias_MakeFormatArgs<
        //    bslfmt_FormatUtil_Alias_DefaultFormatContext>(fmt_args...);
    }

    template <class... t_ARGS>
    static bslfmt_FormatUtil_Alias_FormatArgStore<bslfmt_FormatUtil_Alias_DefaultWFormatContext, t_ARGS...>
    make_wformat_args(t_ARGS&... fmt_args)
    {
        return std::make_wformat_args<bslfmt_FormatUtil_Alias_DefaultWFormatContext>(
                                                                  fmt_args...);
        //return bslfmt_FormatUtil_Alias_MakeFormatArgs<
        //    bslfmt_FormatUtil_Alias_DefaultFormatContext>(fmt_args...);
    }

    template <class... t_ARGS>
    static bsl::string
    format(bslfmt_FormatUtil_Alias_FormatString<t_ARGS...> fmtstr,
           t_ARGS&&...args)
    {
        static constexpr int dummy[] = {0, ((void)bslfmt::formatter<std::remove_cvref_t<t_ARGS>, char>{}, 0)...};
        (void)dummy;
        return vformat(fmtstr.get(), make_format_args(args...));
    }

    template <class... t_ARGS>
    static bsl::wstring format(
                              bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...> fmtstr,
                               t_ARGS&&...                    args)
    {
        return vformat(fmtstr.get(), make_wformat_args(args...));
    }

    template <class... t_ARGS>
    static bsl::string format(const std::locale&                        loc,
                         bslfmt_FormatUtil_Alias_FormatString<t_ARGS...> fmtstr,
                  t_ARGS&&...              args)
    {
        return vformat(loc, fmtstr.get(), make_format_args(args...));
    }

    template <class... t_ARGS>
    static bsl::wstring format(const std::locale&        loc,
                             bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...> fmtstr,
                   t_ARGS&&...               args)
    {
        return vformat(loc, fmtstr.get(), make_wformat_args(args...));
    }

    template <class... t_ARGS>
    static bsl::string format(bsl::allocator<char>                           alloc,
                         bslfmt_FormatUtil_Alias_FormatString<t_ARGS...> fmtstr,
                  t_ARGS&&...              args)
    {
        return vformat(alloc, fmtstr.get(), make_format_args(args...));
    }

    template <class... t_ARGS>
    static bsl::wstring format(bsl::allocator<wchar_t>        alloc,
                             bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...> fmtstr,
                   t_ARGS&&...               args)
    {
        return vformat(alloc, fmtstr.get(), make_wformat_args(args...));
    }

    template <class... t_ARGS>
    static bsl::string format(bsl::allocator<char>                           alloc,
                  const std::locale&       loc,
                         bslfmt_FormatUtil_Alias_FormatString<t_ARGS...> fmtstr,
                  t_ARGS&&...              args)
    {
        return vformat(alloc,
                            loc,
                            fmtstr.get(),
                            make_format_args(args...));
    }

    template <class... t_ARGS>
    static bsl::wstring format(bsl::allocator<char>           alloc,
                   const std::locale&        loc,
                             bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...> fmtstr,
                   t_ARGS&&...               args)
    {
        return vformat(alloc,
                            loc,
                            fmtstr.get(),
                            make_wformat_args(args...));
    }

    inline
    static bsl::string vformat(std::string_view         fmt,
                               bslfmt_FormatUtil_Alias_FormatArgs args)
    {
        bsl::string result;
        vformat_to(&result, fmt, args);
        return result;
    }

    inline
    static bsl::wstring vformat(std::wstring_view        fmt,
                                bslfmt_FormatUtil_Alias_WFormatArgs args)
    {
        bsl::wstring result;
        vformat_to(&result, fmt, args);
        return result;
    }

    inline
    static bsl::string vformat(const std::locale& loc,
                   std::string_view   fmt,
                               bslfmt_FormatUtil_Alias_FormatArgs args)
    {
        bsl::string result;
        vformat_to(&result, loc, fmt, args);
        return result;
    }

    inline
    static bsl::wstring vformat(const std::locale& loc,
                    std::wstring_view  fmt,
                                bslfmt_FormatUtil_Alias_WFormatArgs args)
    {
        bsl::wstring result;
        vformat_to(&result, loc, fmt, args);
        return result;
    }

    inline
    static bsl::string vformat(bsl::allocator<char>  alloc,
                   std::string_view fmt,
                               bslfmt_FormatUtil_Alias_FormatArgs args)
    {
        bsl::string result(alloc);
        vformat_to(&result, fmt, args);
        return result;
    }

    inline
    static bsl::wstring vformat(bsl::allocator<wchar_t> alloc,
                    std::wstring_view  fmt,
                                bslfmt_FormatUtil_Alias_WFormatArgs args)
    {
        bsl::wstring result(alloc);
        vformat_to(&result, fmt, args);
        return result;
    }

    inline
    static bsl::string vformat(bsl::allocator<char>    alloc,
                   const std::locale& loc,
                   std::string_view   fmt,
                               bslfmt_FormatUtil_Alias_FormatArgs args)
    {
        bsl::string result(alloc);
        vformat_to(&result, loc, fmt, args);
        return result;
    }

    inline
    static bsl::wstring vformat(bsl::allocator<wchar_t> alloc,
                    const std::locale& loc,
                    std::wstring_view  fmt,
                                bslfmt_FormatUtil_Alias_WFormatArgs args)
    {
        bsl::wstring result(alloc);
        vformat_to(&result, loc, fmt, args);
        return result;
    }

    template <class... t_ARGS>
    static void format_to(bsl::string                   *out,
                          bslfmt_FormatUtil_Alias_FormatString<t_ARGS...>  fmtstr,
                   t_ARGS&&...               args)
    {
        vformat_to(out, fmtstr.get(), make_format_args(args...));
    }

    template <class... t_ARGS>
    static void format_to(bsl::wstring                   *out,
                          bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...>  fmtstr,
                   t_ARGS&&...                args)
    {
        vformat_to(out, fmtstr.get(), make_wformat_args(args...));
    }

    template <class... t_ARGS>
    static void format_to(bsl::string                   *out,
                   const std::locale&        loc,
                          bslfmt_FormatUtil_Alias_FormatString<t_ARGS...>  fmtstr,
                   t_ARGS&&...               args)
    {
        vformat_to(out, loc, fmtstr.get(), make_format_args(args...));
    }

    template <class... t_ARGS>
    static void format_to(bsl::wstring                   *out,
                   const std::locale&         loc,
                          bslfmt_FormatUtil_Alias_WFormatString<t_ARGS...>  fmtstr,
                   t_ARGS&&...                args)
    {
        vformat_to(out, loc, fmtstr.get(), make_wformat_args(args...));
    }

    inline
    static void vformat_to(bsl::string              *out,
                           std::string_view          fmt,
                           bslfmt_FormatUtil_Alias_FormatArgs  args)
    {
        out->clear();
        std::vformat_to(std::back_inserter(*out), fmt, args);
    }

    inline
    static void vformat_to(bsl::wstring           *out,
                           std::wstring_view  fmt,
                           bslfmt_FormatUtil_Alias_WFormatArgs  args)
    {
        out->clear();
        std::vformat_to(std::back_inserter(*out), fmt, args);
    }

    inline
    static void vformat_to(bsl::string              *out,
                           const std::locale&        loc,
                           std::string_view          fmt,
                           bslfmt_FormatUtil_Alias_FormatArgs  args)
    {
        out->clear();
        std::vformat_to(std::back_inserter(*out), loc, fmt, args);
    }

    inline
    static void vformat_to(bsl::wstring            *out,
                    const std::locale&  loc,
                    std::wstring_view   fmt,
                           bslfmt_FormatUtil_Alias_WFormatArgs  args)
    {
        out->clear();
        std::vformat_to(std::back_inserter(*out), loc, fmt, args);
    }
};

}

#elif 0

#define BSL_FORMAT_CONSTEXPR constexpr

#if defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSL_FORMAT_CONSTEXPR constexpr
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

//template <>
//struct formatter<int, char> : std::formatter<int, char> {};

template <class t_TYPE>
constexpr bool bslfmt_FormatUtil_Impl_isStdBasicString = false;

template <class charT, class traits, class Allocator>
constexpr bool bslfmt_FormatUtil_Impl_isStdBasicString<
    std::basic_string<charT, traits, Allocator> > = true;

template <class t_TYPE>
constexpr bool bslfmt_FormatUtil_Impl_isStdBasicStringView = false;

template <class charT, class traits>
constexpr bool bslfmt_FormatUtil_Impl_isStdBasicStringView<
    std::basic_string_view<charT, traits> > = true;
}  // close namespace bsl
namespace std {
template <class t_ARG, class t_CHAR>
requires (!std::is_arithmetic_v<t_ARG> &&
          !std::is_same_v<t_ARG, std::nullptr_t> &&
          !std::is_same_v<t_ARG, void*> &&
          !std::is_same_v<t_ARG, const void*> &&
          !std::is_same_v<t_ARG, t_CHAR*> &&
          !std::is_same_v<t_ARG, const t_CHAR*> &&
          !std::is_same_v<std::remove_extent_t<t_ARG>, const t_CHAR> &&
          !bsl::bslfmt_FormatUtil_Impl_isStdBasicString<t_ARG> &&
          !bsl::bslfmt_FormatUtil_Impl_isStdBasicStringView<t_ARG> &&
          bsl::bslfmt_FormatUtil_Impl_IsEnabled<bsl::formatter<t_ARG, t_CHAR>>::value)
struct formatter<t_ARG, t_CHAR> : bsl::formatter<t_ARG, t_CHAR> {
};
}  // close namespace std
#endif
#else  // defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSL_FORMAT_CONSTEXPR constexpr
namespace bslfmt {
template <class t_ITERATOR>
class bslfmt_FormatUtil_Impl_TruncatingIterator {
  private:
    // TYPES
    typedef typename bsl::iterator_traits<t_ITERATOR>::difference_type DT;

    // DATA
    t_ITERATOR d_iterator;
    DT         d_limit;
    DT         d_count;

  public:
    // TYPES
    typedef std::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    bslfmt_FormatUtil_Impl_TruncatingIterator(t_ITERATOR iterator, DT limit)
    : d_iterator(iterator)
    , d_limit(limit)
    , d_count(0)
    {
    }

    // MANIPULATORS
    bslfmt_FormatUtil_Impl_TruncatingIterator& operator*()
    {
        return *this;
    }

    void operator=(typename bsl::iterator_traits<t_ITERATOR>::value_type x)
    {
        if (d_count++ < d_limit) {
            *d_iterator++ = x;
        }
    }

    bslfmt_FormatUtil_Impl_TruncatingIterator& operator++()
    {
        return *this;
    }

    bslfmt_FormatUtil_Impl_TruncatingIterator operator++(int)
    {
        return *this;
    }

    // ACCESSORS
    DT count() const
    {
        return d_count;
    }

    t_ITERATOR underlying() const
    {
        return d_iterator;
    }
};

template <class t_VALUE>
class bslfmt_FormatUtil_Impl_OutputIteratorBase {
  public:
    // MANIPULATORS
    virtual void put(t_VALUE) = 0;
};

template <class t_VALUE, class t_ITER>
class bslfmt_FormatUtil_Impl_OutputIteratorImpl
: public bslfmt_FormatUtil_Impl_OutputIteratorBase<t_VALUE>{
  private:
    // DATA
    t_ITER& d_iter;

  public:
    // CREATORS
    bslfmt_FormatUtil_Impl_OutputIteratorImpl(t_ITER& iter)
    : d_iter(iter)
    {
    }

    // MANIPULATORS
    void put(t_VALUE x) BSLS_KEYWORD_OVERRIDE
    {
        *d_iter++ = x;
    }
};

template <class t_VALUE>
class bslfmt_FormatUtil_Impl_OutputIteratorRef {
  private:
    // DATA
    bslfmt_FormatUtil_Impl_OutputIteratorBase<t_VALUE>* d_base_p;

  public:
    // TYPES
    typedef std::output_iterator_tag iterator_category;
    typedef std::ptrdiff_t           difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    bslfmt_FormatUtil_Impl_OutputIteratorRef(
                               bslfmt_FormatUtil_Impl_OutputIteratorBase<t_VALUE> *base)
    : d_base_p(base)
    {
    }

    // MANIPULATORS
    bslfmt_FormatUtil_Impl_OutputIteratorRef& operator*()
    {
        return *this;
    }

    void operator=(t_VALUE x)
    {
        d_base_p->put(x);
    }

    bslfmt_FormatUtil_Impl_OutputIteratorRef& operator++()
    {
        return *this;
    }

    bslfmt_FormatUtil_Impl_OutputIteratorRef operator++(int)
    {
        return *this;
    }
};

#if 0
class format_error : public std::runtime_error {
  public:
    // CREATORS
    explicit format_error(const std::string& what_arg)
    : runtime_error(what_arg)
    {}

    explicit format_error(const char* what_arg)
    : runtime_error(what_arg)
    {}

    // If a 'bsl::string' is passed to the 'std::string' constructor, two
    // copies occur (one to initialize 'what_arg', and one to initialize the
    // internal reference-counted string).  This constructor ensures that only
    // a single copy needs to be performed.
    explicit format_error(const bsl::string& what_arg)
    : runtime_error(what_arg.c_str())
    {}
};
#endif

#if 0
template <class t_OUT, class t_CHAR>
class basic_format_context;
#else
template <class t_OUT, class t_CHAR>
using basic_format_context = std::basic_format_context<t_OUT, t_CHAR>;
#endif

//#if 0
template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef t_CHAR                                             char_type;
    typedef typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                     iterator;

  private:
    // TYPES
    enum Indexing { e_UNKNOWN, e_MANUAL, e_AUTOMATIC };

    // DATA
    iterator d_begin;
    iterator d_end;
    Indexing d_indexing;
    size_t   d_next_arg_id;
    size_t   d_num_args;

  public:
    // CREATORS
    explicit basic_format_parse_context(bsl::basic_string_view<t_CHAR> fmt)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(0)
    {}

    // TODO: this constructor should be private
    explicit basic_format_parse_context(
                       bsl::basic_string_view<t_CHAR> fmt,
                       size_t                    numArgs) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(numArgs)
    {}

    // MANIPULATORS
    void advance_to(const_iterator it)
    {
        d_begin = it;
    }

    size_t next_arg_id()
    {
        if (d_indexing == e_MANUAL) {
            BSLS_THROW(bslfmt_FormatUtil_FormatError(
                                   "mixing of automatic and manual indexing"));
        }
        if (d_next_arg_id >= d_num_args) {
            BSLS_THROW(bslfmt_FormatUtil_FormatError(
                                     "number of conversion specifiers exceeds "
                                    "number of arguments"));
        }
        if (d_indexing == e_UNKNOWN) {
            d_indexing = e_AUTOMATIC;
        }
        return d_next_arg_id++;
    }

    void check_arg_id(size_t id)
    {
        if (d_indexing == e_AUTOMATIC) {
            BSLS_THROW(bslfmt_FormatUtil_FormatError(
                                   "mixing of automatic and manual indexing"));
        }
        if (id >= d_num_args) {
            BSLS_THROW(
                      bslfmt_FormatUtil_FormatError("invalid argument index"));
        }
        if (d_indexing == e_UNKNOWN) {
            d_indexing = e_MANUAL;
        }
    }

    // ACCESSORS
    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_begin;
    }

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_end;
    }

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(const basic_format_parse_context&)
                                                          BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(const basic_format_parse_context&)
                                                          BSLS_KEYWORD_DELETED;
};
//#else
//using std::basic_format_parse_context;
//#endif;



typedef basic_format_parse_context<char> format_parse_context;

template <class t_CONTEXT>
class basic_format_arg;

template <class t_OUT, class t_CHAR>
class basic_format_arg<basic_format_context<t_OUT, t_CHAR> > {
  public:
    // TYPES
    class handle {
      private:
        // DATA
        const void *d_value_p;
        void      (*d_format_impl_p)(basic_format_parse_context<t_CHAR>&,
                                     basic_format_context<t_OUT, t_CHAR>&,
                                     const void*);

        template <class t_TYPE>
        static void format_impl(basic_format_parse_context<t_CHAR>&  pc,
                                basic_format_context<t_OUT, t_CHAR>& fc,
                                const void                          *value);

        // PRIVATE CREATORS
        template <class t_TYPE>
        explicit handle(const t_TYPE& value) BSLS_KEYWORD_NOEXCEPT
        : d_value_p(BSLS_UTIL_ADDRESSOF(value))
        , d_format_impl_p(format_impl<t_TYPE>)
        {
        }

        // FRIENDS
        friend class basic_format_arg<basic_format_context<t_OUT, t_CHAR> >;

      public:
        // ACCESSORS
        void format(basic_format_parse_context<t_CHAR>&  pc,
                    basic_format_context<t_OUT, t_CHAR>& fc) const
        {
            d_format_impl_p(pc, fc, d_value_p);
        }
    };

    typedef
        typename BloombergLP::bsls::UnspecifiedBool<basic_format_arg>::BoolType
            BoolType;

  private:
    // PRIVATE TYPES
    typedef t_CHAR char_type;

    // DATA
    bsl::variant<bsl::monostate,
            bool, char_type, int, unsigned, long long, unsigned long long,
            float, double, long double,
            const char_type*,
            bsl::basic_string_view<char_type>,
            const void*,
            handle> d_value;

  public:
    // TODO: All the value constructors need to be private (with appropriate
    // friendship granted)
    // CREATORS
    explicit basic_format_arg(bool value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    explicit basic_format_arg(char_type value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    template <class t_TYPE>
    explicit basic_format_arg(
                     t_TYPE value,
                     typename bsl::enable_if<bsl::is_same<t_TYPE, char>::value &&
                                            bsl::is_same<char_type, wchar_t>::value,
                                        int>::type = 0) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<wchar_t>(value))
    {
    }

    template <class t_TYPE>
    explicit basic_format_arg(
                    t_TYPE value,
                    typename bsl::enable_if<bsl::is_integral<t_TYPE>::value &&
                                           sizeof(t_TYPE) <= sizeof(long long),
                                       int>::type = 0) BSLS_KEYWORD_NOEXCEPT
    {
        if (static_cast<t_TYPE>(-1) < static_cast<t_TYPE>(0)) {
            // 't_TYPE' is signed
            if (sizeof(t_TYPE) <= sizeof(int)) {
                d_value.template emplace<int>(value);
            } else {
                d_value.template emplace<long long>(value);
            }
        } else {
            // 't_TYPE' is unsigned
            if (sizeof(t_TYPE) <= sizeof(int)) {
                d_value.template emplace<unsigned int>(value);
            } else {
                d_value.template emplace<unsigned long long>(value);
            }
        }
    }

    explicit basic_format_arg(float value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    explicit basic_format_arg(double value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    explicit basic_format_arg(long double value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    template <class t_TRAITS>
    explicit basic_format_arg(bsl::basic_string_view<char_type, t_TRAITS> value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                             bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                       const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                        std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value)) {}

    explicit basic_format_arg(char_type *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const char_type*>(value)) {}

    explicit basic_format_arg(const char_type *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

    explicit basic_format_arg(void *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const void*>(value)) {}

    explicit basic_format_arg(const void *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value) {}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
    explicit basic_format_arg(std::nullptr_t) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const void*>(nullptr)) {}
#endif

    template <class t_TYPE>
    explicit basic_format_arg(
                   const t_TYPE& value,
                   typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value ||
                                          (sizeof(t_TYPE) > sizeof(long long)),
                                      int>::type = 0) BSLS_KEYWORD_NOEXCEPT
    : d_value(handle(value))
    {
    }

    // FRIENDS
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
    template <class t_VISITOR, class t_CONTEXT>
    friend decltype(auto)
    visit_format_arg(t_VISITOR&& v, basic_format_arg<t_CONTEXT> a);
#else
    template <class t_VISITOR, class t_CONTEXT>
    friend typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
    visit_format_arg(t_VISITOR& visitor, basic_format_arg<t_CONTEXT> a);
#endif

  public:
    // CREATORS
    basic_format_arg() BSLS_KEYWORD_NOEXCEPT {}

    // ACCESSORS
    operator BoolType() const BSLS_KEYWORD_NOEXCEPT
    {
        return BoolType::makeValue(!holds_alternative<bsl::monostate>(d_value));
    }
};

typedef basic_format_context<bslfmt_FormatUtil_Impl_OutputIteratorRef<char>, char>
                                            bslfmt_FormatUtil_Impl_DefaultFormatContext;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_ARGS>
class bslfmt_FormatUtil_Impl_FormatArgStore {
    // TODO: Make all members private

  public:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;

    // PRIVATE CREATORS
    explicit bslfmt_FormatUtil_Impl_FormatArgStore(
             const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_args(args)
    {}
};

template <class t_CONTEXT, class... t_ARGS>
bslfmt_FormatUtil_Impl_FormatArgStore<t_CONTEXT, t_ARGS...>
bslfmt_FormatUtil_Impl_MakeFormatArgs(t_ARGS&... fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array =
                                  {{basic_format_arg<t_CONTEXT>(fmt_args)...}};
    return bslfmt_FormatUtil_Impl_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
}

template <class... t_ARGS>
bslfmt_FormatUtil_Impl_FormatArgStore<bslfmt_FormatUtil_Impl_DefaultFormatContext, t_ARGS...>
make_format_args(t_ARGS&... fmt_args)
{
    return bslfmt_FormatUtil_Impl_MakeFormatArgs<bslfmt_FormatUtil_Impl_DefaultFormatContext>(
                                                                  fmt_args...);
}
#endif

template <class t_CONTEXT>
class bslfmt_FormatUtil_Impl_BasicFormatArgs;

typedef bslfmt_FormatUtil_Impl_BasicFormatArgs<bslfmt_FormatUtil_Impl_DefaultFormatContext>
                                                                   format_args;

template <class t_CONTEXT>
class bslfmt_FormatUtil_Impl_BasicFormatArgs {
    // DATA
    size_t                             d_size;
    const basic_format_arg<t_CONTEXT> *d_data;

  public:
    // CREATORS
    bslfmt_FormatUtil_Impl_BasicFormatArgs() BSLS_KEYWORD_NOEXCEPT
    : d_size(0)
    {}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... t_ARGS>
    bslfmt_FormatUtil_Impl_BasicFormatArgs(
               const bslfmt_FormatUtil_Impl_FormatArgStore<t_CONTEXT, t_ARGS...>& store) 
                                             BSLS_KEYWORD_NOEXCEPT  // IMPLICIT
    : d_size(sizeof...(t_ARGS))
    , d_data(store.d_args.data())
    {}
#endif

    // ACCESSORS
    basic_format_arg<t_CONTEXT> get(size_t i) const BSLS_KEYWORD_NOEXCEPT
    {
        return i < d_size ? d_data[i] : basic_format_arg<t_CONTEXT>();
    }

  private:
    // PRIVATE ACCESSORS
    size_t size() const
    {
        return d_size;
    }

    // FRIENDS
    template <class t_OUT, class t_CHAR>
    friend t_OUT bslfmt_FormatUtil_Impl_VFormatImpl(
           t_OUT                                                        out,
           bsl::basic_string_view<t_CHAR>                                    fmtstr,
           bslfmt_FormatUtil_Impl_BasicFormatArgs<basic_format_context<t_OUT,
                                                              t_CHAR> > args);
};

// 'visit_format_arg' is not a hidden friend.
#ifdef BSL_VARIANT_FULL_IMPLEMENTATION
template <class t_VISITOR, class t_CONTEXT>
decltype(auto) visit_format_arg(t_VISITOR&& v, basic_format_arg<t_CONTEXT> a)
{
    return bsl::visit(std::forward<t_VISITOR>(v), a.d_value);
}
#else
template <class t_VISITOR, class t_CONTEXT>
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
visit_format_arg(t_VISITOR& v, basic_format_arg<t_CONTEXT> a)
{
    return bsl::visit(v, a.d_value);
}
#endif

#if 0
template <class t_OUT, class t_CHAR>
class basic_format_context {
  private:
    // TYPES
    typedef basic_format_arg<basic_format_context> Arg;

    // DATA
    bslfmt_FormatUtil_Impl_BasicFormatArgs<basic_format_context> d_args;
    t_OUT  d_out;

  public:
    // TYPES
    typedef t_OUT  iterator;
    typedef t_CHAR char_type;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
    template <class t_TYPE>
    using formatter_type = formatter<t_TYPE, t_CHAR>;
#endif

    // CREATORS
    // TODO: This constructor should be made private (callable only by
    // 'bsl::format' internals)
    basic_format_context(
                      t_OUT                                               out,
                      bslfmt_FormatUtil_Impl_BasicFormatArgs<basic_format_context> args)
    : d_args(args)
    , d_out(out)
    { }

    // MANIPULATORS
    iterator out()
    {
        return BloombergLP::bslmf::MovableRefUtil::move(d_out);
    }

    void advance_to(iterator it)
    {
        d_out = BloombergLP::bslmf::MovableRefUtil::move(it);
    }

    // ACCESSORS
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT
    {
        return d_args.get(id);
    }
};
#else
template <class t_OUT, class t_CHAR>
using basic_format_context = std::basic_format_context<t_OUT, t_CHAR>;
#endif

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
void
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::format_impl(
                                   basic_format_parse_context<t_CHAR>&   pc,
                                   basic_format_context<t_OUT, t_CHAR>&  fc,
                                   const void                           *value)
{
    formatter<t_TYPE, t_CHAR> f;
    pc.advance_to(f.parse(pc));
    fc.advance_to(f.format(*static_cast<const t_TYPE*>(value), fc));
}

template <class t_OUT>
struct format_to_n_result {
    t_OUT                                                 out;
    typename bsl::iterator_traits<t_OUT>::difference_type size;
};

// FORMATTER SPECIALIZATIONS
#if 0
template <>
struct formatter<int, char> {
    format_parse_context::iterator parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(int x, basic_format_context<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return std::copy(buf, result, fc.out());
    }
};

template <>
struct formatter<bsl::string_view, char> {
    format_parse_context::iterator parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(bsl::string_view sv, basic_format_context<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};
#endif
#if 1
template <>
struct formatter<int, char> {
    format_parse_context::iterator parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(int x, basic_format_context<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return std::copy(buf, result, fc.out());
    }
};

template <>
struct formatter<bsl::string_view, char> {
    format_parse_context::iterator parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(bsl::string_view sv, basic_format_context<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};
#endif

template <class t_OUT, class t_CHAR>
struct bslfmt_FormatUtil_Impl_FormatVisitor {
    basic_format_parse_context<t_CHAR>  *d_parseContext_p;
    basic_format_context<t_OUT, t_CHAR> *d_formatContext_p;

    bslfmt_FormatUtil_Impl_FormatVisitor(basic_format_parse_context<t_CHAR>&  pc,
                                basic_format_context<t_OUT, t_CHAR>& fc)
    : d_parseContext_p(&pc)
    , d_formatContext_p(&fc)
    {
    }

    typedef
        typename basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle
            handle;

    template <class t_TYPE>
    void operator()(t_TYPE) const
    {
        BSLS_THROW(bslfmt_FormatUtil_FormatError(
                                    "this argument type isn't supported yet"));
    }

    void operator()(int x) const
    {
        formatter<int, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                               bsl::as_const(f).format(x, *d_formatContext_p));
    }

    void operator()(bsl::string_view sv) const
    {
        formatter<bsl::string_view, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                              bsl::as_const(f).format(sv, *d_formatContext_p));
    }

    void operator()(const handle& h) const
    {
        h.format(*d_parseContext_p, *d_formatContext_p);
    }
};

template <class t_OUT, class t_CHAR>
t_OUT bslfmt_FormatUtil_Impl_VFormatImpl(
    t_OUT                                                               out,
    bsl::basic_string_view<t_CHAR>                                           fmtstr,
    bslfmt_FormatUtil_Impl_BasicFormatArgs<basic_format_context<t_OUT, t_CHAR> > args)
    // The actual meat of the implementation.  This overload is used when the
    // iterator type 't_OUT' matches the iterator type that 'args' is able to
    // format to.  In all other cases the iterator must be wrapped.
{
    typedef basic_format_context<t_OUT, t_CHAR> FC;

    basic_format_parse_context<t_CHAR>           pc(fmtstr, args.size());
    FC                                           fc(out, args);
    typename bsl::basic_string_view<t_CHAR>::iterator it = pc.begin();
    bslfmt_FormatUtil_Impl_FormatVisitor<t_OUT, t_CHAR>   visitor(pc, fc);

    while (it != pc.end()) {
        if (*it == '{') {
            ++it;
            if (it == pc.end()) {
                BSLS_THROW(bslfmt_FormatUtil_FormatError("unmatched {"));
            } else if (*it == '{') {
                // literal {
                ++it;
                out = fc.out();
                *out++ = '{';
                fc.advance_to(out);
                continue;
            }
            size_t id = -1;
            if (*it >= '0' && *it <= '9') {
                // numeric ID
                id = 0;
                while (it != pc.end() && *it >= '0' && *it <= '9') {
                    id = 10 * id + (*it++ - '0');
                    if (id >= args.size()) {
                        BSLS_THROW(
                            bslfmt_FormatUtil_FormatError("arg id too large"));
                    }
                }
                if (it == pc.end()) {
                    BSLS_THROW(bslfmt_FormatUtil_FormatError("unmatched {"));
                }
            }
            if (id == size_t(-1)) {
                id = pc.next_arg_id();
            } else {
                pc.check_arg_id(id);
            }
            if (*it == ':') {
                ++it;
            }
            pc.advance_to(it);
            visit_format_arg(visitor, args.get(id));
            it = pc.begin();
            if (it != pc.end()) {
                // advance past the terminating }
                ++it;
            }
        } else if (*it == '}') {
            // must be escaped
            ++it;
            if (it == pc.end() || *it != '}') {
                BSLS_THROW(bslfmt_FormatUtil_FormatError("} must be escaped"));
            }
            ++it;
            out = fc.out();
            *out++ = '}';
            fc.advance_to(out);
        } else {
            // just copy it
            out = fc.out();
            *out++ = *it++;
            fc.advance_to(out);
        }
    }
    return fc.out();
}

template <class t_OUT, class t_CHAR, class t_CONTEXT>
t_OUT bslfmt_FormatUtil_Impl_VFormatImpl(
                               t_OUT                                    out,
                               bsl::basic_string_view<t_CHAR>                fmtstr,
                               bslfmt_FormatUtil_Impl_BasicFormatArgs<t_CONTEXT> args)
{
    bslfmt_FormatUtil_Impl_OutputIteratorImpl<char, t_OUT> wrappedOut(out);
    bslfmt_FormatUtil_Impl_VFormatImpl(
                            bslfmt_FormatUtil_Impl_OutputIteratorRef<char>(&wrappedOut),
                            fmtstr,
                            args);
    return out;
}

template <class t_OUT>
t_OUT vformat_to(t_OUT out, bsl::string_view fmtstr, format_args args)
{
    return bslfmt_FormatUtil_Impl_VFormatImpl(out, fmtstr, args);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_OUT, class... t_ARGS>
t_OUT format_to(t_OUT out, bsl::string_view fmtstr, const t_ARGS&... args)
{
    typedef basic_format_context<t_OUT, char>      Context;
    typedef bslfmt_FormatUtil_Impl_BasicFormatArgs<Context> Args;
    return bslfmt_FormatUtil_Impl_VFormatImpl(
                         out,
                         fmtstr,
                         Args(bslfmt_FormatUtil_Impl_MakeFormatArgs<Context>(args...)));
}

template <class... t_ARGS>
void format_to(bsl::string *out, bsl::string_view fmtstr, const t_ARGS&... args)
{
    format_to(std::back_inserter(*out), fmtstr, args...);
}

void vformat_to(bsl::string *out, bsl::string_view fmtstr, format_args args)
{
    vformat_to(std::back_inserter(*out), fmtstr, args);
}

template <class... t_ARGS>
bsl::string format(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bsl::string result;
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
bsl::string format(bsl::allocator<char> alloc, bsl::string_view fmtstr, const t_ARGS&... args)
{
    bsl::string result(alloc);
    format_to(&result, fmtstr, args...);
    return result;
}

inline
bsl::string vformat(bsl::string_view fmt, format_args args)
{
    bsl::string result;
    vformat_to(&result, fmt, args);
    return result;
}

inline
bsl::string vformat(bsl::allocator<char> alloc, bsl::string_view fmt, format_args args)
{
    bsl::string result(alloc);
    vformat_to(&result, fmt, args);
    return result;
}

template <class... t_ARGS>
size_t formatted_size(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bslfmt_FormatUtil_Impl_TruncatingIterator<char*> it(0, 0);
    format_to(it, fmtstr, args...);
    return it.count();
}

template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT>
format_to_n(t_OUT                                            out,
            typename bsl::iterator_traits<t_OUT>::difference_type n,
            bsl::string_view                                      fmtstr,
            const t_ARGS&...                                 args)
{
    if (n < 0) n = 0;
    bslfmt_FormatUtil_Impl_TruncatingIterator<t_OUT> it(out, n);
    format_to(it, fmtstr, args...);
    format_to_n_result<t_OUT> result;
    result.out = it.underlying();
    result.size = it.count();
    return result;
}
#endif
}  // close namespace bsl
#endif  // defined(XXXBSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace bslfmt {
// FORMATTER SPECIALIZATIONS
#if 1
template <>
struct formatter<int, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>::iterator
    format(int                                                      x,
           bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return std::copy(buf, result, fc.out());
    }
};

template <>
struct formatter<bsl::string_view, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>::iterator
    format(bsl::string_view                                        sv,
           bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};
// FORMATTER SPECIALIZATIONS
template <>
struct formatter<bsl::string, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>::iterator
    format(const bsl::string&                                 str,
           bslfmt_FormatUtil_Alias_BasicFormatContext<t_OUT, char>& fc) const
    {
        return std::copy(str.begin(), str.end(), fc.out());
    }
};
#elif 0
template <>
struct formatter<int, char> {
    template<class t_PARSE_CONTEXT>
    BSL_FORMAT_CONSTEXPR t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template<class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator
    format(int x, t_CONTEXT<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return std::copy(buf, result, fc.out());
    }
};

template <>
struct formatter<bsl::string_view, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator
    format(bsl::string_view                                         sv,
           t_CONTEXT<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};
// FORMATTER SPECIALIZATIONS
template <>
struct formatter<bsl::string, char> {
    BSL_FORMAT_CONSTEXPR bslfmt_FormatUtil_Alias_FormatParseContext::iterator
    parse(bslfmt_FormatUtil_Alias_FormatParseContext& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bslfmt_FormatUtil_Alias_FormatError("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_CONTEXT>
    typename t_CONTEXT<t_OUT, char>::iterator
    format(const bsl::string&                                       str,
           t_CONTEXT<t_OUT, char>& fc) const
    {
        return std::copy(str.begin(), str.end(), fc.out());
    }
};
#endif
}  // close namespace bslfmg

#endif // End C++11 code

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
