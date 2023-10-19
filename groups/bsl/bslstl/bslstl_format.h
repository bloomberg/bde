// bslstl_format.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLSTL_FORMAT
#define INCLUDED_BSLSTL_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bslscm_version.h>

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <bslalg_numericformatterutil.h>

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

namespace bsl {
template <class t_ITERATOR>
class bslstl_format_TruncatingIterator {
  private:
    // TYPES
    typedef typename iterator_traits<t_ITERATOR>::difference_type DT;

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
    bslstl_format_TruncatingIterator(t_ITERATOR iterator, DT limit)
    : d_iterator(iterator)
    , d_limit(limit)
    , d_count(0)
    {
    }

    // MANIPULATORS
    bslstl_format_TruncatingIterator& operator*()
    {
        return *this;
    }

    void operator=(typename iterator_traits<t_ITERATOR>::value_type x)
    {
        if (d_count++ < d_limit) {
            *d_iterator++ = x;
        }
    }

    bslstl_format_TruncatingIterator& operator++()
    {
        return *this;
    }

    bslstl_format_TruncatingIterator operator++(int)
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

template <class t_FORMATTER, class = void>
struct bslstl_format_IsEnabled : true_type {};

template <class t_FORMATTER>
struct bslstl_format_IsEnabled<
    t_FORMATTER,
    typename t_FORMATTER::bslstl_format_IsPrimaryFormatterTemplate>
: false_type {
};

template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    typedef void bslstl_format_IsPrimaryFormatterTemplate;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bsl

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
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

template <>
struct formatter<int, char> : std::formatter<int, char> {};

template <class t_TYPE>
constexpr bool bslstl_format_isStdBasicString = false;

template <class charT, class traits, class Allocator>
constexpr bool bslstl_format_isStdBasicString<
    std::basic_string<charT, traits, Allocator> > = true;

template <class t_TYPE>
constexpr bool bslstl_format_isStdBasicStringView = false;

template <class charT, class traits>
constexpr bool bslstl_format_isStdBasicStringView<
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
          !bsl::bslstl_format_isStdBasicString<t_ARG> &&
          !bsl::bslstl_format_isStdBasicStringView<t_ARG> &&
          bsl::bslstl_format_IsEnabled<bsl::formatter<t_ARG, t_CHAR>>::value)
struct formatter<t_ARG, t_CHAR> : bsl::formatter<t_ARG, t_CHAR> {
};
}  // close namespace std
#else  // defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#define BSL_FORMAT_CONSTEXPR
namespace bsl {
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
    explicit format_error(const string& what_arg)
    : runtime_error(what_arg.c_str())
    {}
};

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef t_CHAR                                             char_type;
    typedef typename basic_string_view<t_CHAR>::const_iterator const_iterator;
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
    explicit basic_format_parse_context(basic_string_view<t_CHAR> fmt)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(0)
    {}

    // TODO: this constructor should be private
    explicit basic_format_parse_context(
                       basic_string_view<t_CHAR> fmt,
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
            BSLS_THROW(
                      format_error("mixing of automatic and manual indexing"));
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

    void check_arg_id(size_t id)
    {
        if (d_indexing == e_AUTOMATIC) {
            BSLS_THROW(
                      format_error("mixing of automatic and manual indexing"));
        }
        if (id >= d_num_args) {
            BSLS_THROW(format_error("invalid argument index"));
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
    variant<monostate,
            bool, char_type, int, unsigned, long long, unsigned long long,
            float, double, long double,
            const char_type*,
            basic_string_view<char_type>,
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
                     typename enable_if<is_same<t_TYPE, char>::value &&
                                            is_same<char_type, wchar_t>::value,
                                        int>::type = 0) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<wchar_t>(value))
    {
    }

    template <class t_TYPE>
    explicit basic_format_arg(
                    t_TYPE value,
                    typename enable_if<is_integral<t_TYPE>::value &&
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
    explicit basic_format_arg(basic_string_view<char_type, t_TRAITS> value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                             basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                       const basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                        std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<basic_string_view<char_type> >(value)) {}

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<basic_string_view<char_type> >(value)) {}

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
                   typename enable_if<!is_integral<t_TYPE>::value ||
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
    friend typename invoke_result<t_VISITOR&, monostate&>::type
    visit_format_arg(t_VISITOR& visitor, basic_format_arg<t_CONTEXT> a);
#endif

  public:
    // CREATORS
    basic_format_arg() BSLS_KEYWORD_NOEXCEPT {}

    // ACCESSORS
    operator BoolType() const BSLS_KEYWORD_NOEXCEPT
    {
        return BoolType::makeValue(!holds_alternative<monostate>(d_value));
    }
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
typename invoke_result<t_VISITOR&, monostate&>::type
visit_format_arg(t_VISITOR& v, basic_format_arg<t_CONTEXT> a)
{
    return bsl::visit(v, a.d_value);
}
#endif

template <class t_OUT, class t_CHAR>
class basic_format_context {
  private:
    // TYPES
    typedef basic_format_arg<basic_format_context> Arg;

    // DATA
    Arg   *d_args_begin;
    Arg   *d_args_end;
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
    basic_format_context(t_OUT out, Arg *begin, Arg *end)
    : d_args_begin(begin)
    , d_args_end(end)
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
        const size_t numArgs = d_args_end - d_args_begin;
        return id < numArgs ? d_args_begin[id] : Arg();
    }
};

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
    typename iterator_traits<t_OUT>::difference_type size;
};

// FORMATTER SPECIALIZATIONS
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
struct formatter<string_view, char> {
    format_parse_context::iterator parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(string_view sv, basic_format_context<t_OUT, char>& fc) const
    {
        return std::copy(sv.begin(), sv.end(), fc.out());
    }
};

template <class t_OUT, class t_CHAR>
struct bslstl_format_FormatVisitor {
    basic_format_parse_context<t_CHAR>  *d_parseContext_p;
    basic_format_context<t_OUT, t_CHAR> *d_formatContext_p;

    bslstl_format_FormatVisitor(basic_format_parse_context<t_CHAR>&  pc,
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
        BSLS_THROW(format_error("this argument type isn't supported yet"));
    }

    void operator()(int x) const
    {
        formatter<int, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                               bsl::as_const(f).format(x, *d_formatContext_p));
    }

    void operator()(string_view sv) const
    {
        formatter<string_view, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                              bsl::as_const(f).format(sv, *d_formatContext_p));
    }

    void operator()(const handle& h) const
    {
        h.format(*d_parseContext_p, *d_formatContext_p);
    }
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_OUT, class... t_ARGS>
t_OUT format_to(t_OUT out, string_view fmtstr, const t_ARGS&... args)
{
    format_parse_context pc(fmtstr, sizeof...(args));
    typedef basic_format_context<t_OUT, char> FC;
    // extra arg prevents empty initializer list
    basic_format_arg<FC> fargs[] = {basic_format_arg<FC>(args)...,
                                    basic_format_arg<FC>()};
    FC fc(out, fargs, fargs + sizeof...(args)); 
    string_view::iterator it = pc.begin();
    bslstl_format_FormatVisitor<t_OUT, char> visitor(pc, fc);
    while (it != pc.end()) {
        if (*it == '{') {
            ++it;
            if (it == pc.end()) {
                BSLS_THROW(format_error("unmatched {"));
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
                    // suppress warning about the comparison
                    size_t argCount = sizeof...(args);
                    if (id >= argCount) {
                        BSLS_THROW(format_error("arg id too large"));
                    }
                }
                if (it == pc.end()) {
                    BSLS_THROW(format_error("unmatched {"));
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
            visit_format_arg(visitor, fargs[id]);
            it = pc.begin();
            if (it != pc.end()) {
                // advance past the terminating }
                ++it;
            }
        } else if (*it == '}') {
            // must be escaped
            ++it;
            if (it == pc.end() || *it != '}') {
                BSLS_THROW(format_error("} must be escaped"));
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

template <class... t_ARGS>
void format_to(string *out, string_view fmtstr, const t_ARGS&... args)
{
    format_to(std::back_inserter(*out), fmtstr, args...);
}

template <class... t_ARGS>
string format(string_view fmtstr, const t_ARGS&... args)
{
    string result;
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
string format(allocator<char> alloc, string_view fmtstr, const t_ARGS&... args)
{
    string result(alloc);
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
size_t formatted_size(string_view fmtstr, const t_ARGS&... args)
{
    bslstl_format_TruncatingIterator<char*> it(0, 0);
    format_to(it, fmtstr, args...);
    return it.count();
}

template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT>
format_to_n(t_OUT                                            out,
            typename iterator_traits<t_OUT>::difference_type n,
            string_view                                      fmtstr,
            const t_ARGS&...                                 args)
{
    if (n < 0) n = 0;
    bslstl_format_TruncatingIterator<t_OUT> it(out, n);
    format_to(it, fmtstr, args...);
    format_to_n_result<t_OUT> result;
    result.out = it.underlying();
    result.size = it.count();
    return result;
}
#endif
}  // close namespace bsl
#endif  // defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace bsl {
// FORMATTER SPECIALIZATIONS
template <>
struct formatter<string, char> {
    BSL_FORMAT_CONSTEXPR format_parse_context::iterator
    parse(format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename basic_format_context<t_OUT, char>::iterator
    format(const string& str, basic_format_context<t_OUT, char>& fc) const
    {
        return std::copy(str.begin(), str.end(), fc.out());
    }
};
}  // close namespace bsl

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
