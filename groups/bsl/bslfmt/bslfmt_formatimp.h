// bslfmt_formatimp.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATIMP
#define INCLUDED_BSLFMT_FORMATIMP

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_array.h>
#include <bslstl_monostate.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03 Generated on Wed Oct 18
// 11:10:11 2023 Command line: sim_cpp11_features.pl bslstl_format.h
#define COMPILING_BSLSTL_FORMAT_H
#include <bslstl_format_cpp03.h>
#undef COMPILING_BSLSTL_FORMAT_H
#else

namespace BloombergLP {
namespace bslfmt {
template <class t_FORMATTER, class = void>
struct FormatImp_formatterIsStdAliasingEnabled : bsl::true_type {
};

template <class t_FORMATTER>
struct FormatImp_formatterIsStdAliasingEnabled<
    t_FORMATTER,
    typename t_FORMATTER::PreventStdAliasing> : bsl::false_type {
};
}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
template <class t_ARG, class t_CHAR = char>
struct formatter {
  public:
    typedef void PreventStdAliasing;

  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;
    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
};
}  // close namespace bslfmt

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

namespace BloombergLP {
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
    ::std::basic_string_view<charT, traits> > = true;
}  // close namespace bslfmt
}  // close enterprise namespace

namespace std {
template <class t_ARG, class t_CHAR>
struct formatter;

template <class t_ARG, class t_CHAR>
requires(
    !bsl::is_arithmetic_v<t_ARG> && !bsl::is_same_v<t_ARG, bsl::nullptr_t> &&
    !bsl::is_same_v<t_ARG, void *> && !bsl::is_same_v<t_ARG, const void *> &&
    !bsl::is_same_v<t_ARG, t_CHAR *> &&
    !bsl::is_same_v<t_ARG, const t_CHAR *> &&
    !bsl::is_same_v<bsl::remove_extent_t<t_ARG>, const t_CHAR> &&
    !BloombergLP::bslfmt::bslfmt_format_IsStdBasicString<t_ARG> &&
    !BloombergLP::bslfmt::bslfmt_format_IsStdBasicStringView<t_ARG> &&
    BloombergLP::bslfmt::FormatImp_formatterIsStdAliasingEnabled<
        bsl::formatter<t_ARG, t_CHAR> >::value
)
struct formatter<t_ARG, t_CHAR>
: bsl::formatter<t_ARG, t_CHAR> {};
}  // close namespace std
#endif

//namespace bsl {
//template <class t_FORMATTER, class = void>
//struct bslstl_format_IsEnabled : true_type {
//};
//
//template <class t_FORMATTER>
//struct bslstl_format_IsEnabled<
//    t_FORMATTER,
//    typename t_FORMATTER::bslstl_format_IsPrimaryFormatterTemplate>
//: false_type {
//};
//
//template <class t_ARG, class t_CHAR = char>
//struct formatter {
//  public:
//    typedef void bslstl_format_IsPrimaryFormatterTemplate;
//
//  private:
//    // NOT IMPLEMENTED
//    formatter(const formatter&) BSLS_KEYWORD_DELETED;
//    formatter& operator=(const formatter&) BSLS_KEYWORD_DELETED;
//};
//}  // close namespace bsl

namespace BloombergLP {
namespace bslfmt {
template <class t_ITERATOR>
class bslstl_format_TruncatingIterator {
  private:
    // TYPES
    typedef typename bsl::iterator_traits<t_ITERATOR>::difference_type DT;

    // DATA
    t_ITERATOR d_iterator;
    DT         d_limit;
    DT         d_count;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
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

    void operator=(typename bsl::iterator_traits<t_ITERATOR>::value_type x)
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

template <class t_VALUE>
class bslstl_format_OutputIteratorBase {
  public:
    // MANIPULATORS
    virtual void put(t_VALUE) = 0;
};

template <class t_VALUE, class t_ITER>
class bslstl_format_OutputIteratorImpl
: public bslstl_format_OutputIteratorBase<t_VALUE> {
  private:
    // DATA
    t_ITER& d_iter;

  public:
    // CREATORS
    bslstl_format_OutputIteratorImpl(t_ITER& iter)
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
class bslstl_format_OutputIteratorRef {
  private:
    // DATA
    bslstl_format_OutputIteratorBase<t_VALUE> *d_base_p;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS
    bslstl_format_OutputIteratorRef(
                               bslstl_format_OutputIteratorBase<t_VALUE> *base)
    : d_base_p(base)
    {
    }

    // MANIPULATORS
    bslstl_format_OutputIteratorRef& operator*()
    {
        return *this;
    }

    void operator=(t_VALUE x)
    {
        d_base_p->put(x);
    }

    bslstl_format_OutputIteratorRef& operator++()
    {
        return *this;
    }

    bslstl_format_OutputIteratorRef operator++(int)
    {
        return *this;
    }
};

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
using std::format_error;
#else
class format_error : public bsl::runtime_error {
  public:
    // CREATORS
    explicit format_error(const std::string& what_arg)
    : runtime_error(what_arg)
    {
    }

    explicit format_error(const char *what_arg)
    : runtime_error(what_arg)
    {
    }

    // If a 'bsl::string' is passed to the 'std::string' constructor, two
    // copies occur (one to initialize 'what_arg', and one to initialize the
    // internal reference-counted string).  This constructor ensures that only
    // a single copy needs to be performed.
    explicit format_error(const bsl::string& what_arg)
    : runtime_error(what_arg.c_str())
    {
    }

    explicit format_error(const format_error& other)
    : runtime_error(other)
    {
    };
#endif

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef t_CHAR char_type;
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

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
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                      bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    , d_indexing(e_UNKNOWN)
    , d_next_arg_id(0)
    , d_num_args(0)
    {
    }

    // TODO: this constructor should be private
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
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
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it)
    {
        d_begin = it;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id()
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

    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id)
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
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_begin;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_end;
    }

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
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
        void (*d_format_impl_p)(basic_format_parse_context<t_CHAR>&,
                                basic_format_context<t_OUT, t_CHAR>&,
                                const void *);

        template <class t_TYPE>
        static void format_impl(basic_format_parse_context<t_CHAR>&   pc,
                                basic_format_context<t_OUT, t_CHAR>&  fc,
                                const void                           *value);

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
                 bool,
                 char_type,
                 int,
                 unsigned,
                 long long,
                 unsigned long long,
                 float,
                 double,
                 long double,
                 const char_type *,
                 bsl::basic_string_view<char_type>,
                 const void *,
                 handle>
        d_value;

  public:
    // TODO: All the value constructors need to be private (with appropriate
    // friendship granted) CREATORS
    explicit basic_format_arg(bool value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

    explicit basic_format_arg(char_type value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

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
            }
            else {
                d_value.template emplace<long long>(value);
            }
        }
        else {
            // 't_TYPE' is unsigned
            if (sizeof(t_TYPE) <= sizeof(int)) {
                d_value.template emplace<unsigned int>(value);
            }
            else {
                d_value.template emplace<unsigned long long>(value);
            }
        }
    }

    explicit basic_format_arg(float value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

    explicit basic_format_arg(double value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

    explicit basic_format_arg(long double value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

    template <class t_TRAITS>
    explicit basic_format_arg(
       bsl::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value))
    {
    }

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(bsl::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value))
    {
    }

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value))
    {
    }

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(std::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value))
    {
    }

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<bsl::basic_string_view<char_type> >(value))
    {
    }

    explicit basic_format_arg(char_type *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const char_type *>(value))
    {
    }

    explicit basic_format_arg(const char_type *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

    explicit basic_format_arg(void *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const void *>(value))
    {
    }

    explicit basic_format_arg(const void *value) BSLS_KEYWORD_NOEXCEPT
    : d_value(value)
    {
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
    explicit basic_format_arg(std::nullptr_t) BSLS_KEYWORD_NOEXCEPT
    : d_value(static_cast<const void *>(nullptr))
    {
    }
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
    friend decltype(auto) visit_format_arg(t_VISITOR&&                 v,
                                           basic_format_arg<t_CONTEXT> a);
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
        return BoolType::makeValue(
                                  !holds_alternative<bsl::monostate>(d_value));
    }
};

typedef basic_format_context<bslstl_format_OutputIteratorRef<char>, char>
    bslstl_format_DefaultFormatContext;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_ARGS>
class bslstl_format_FormatArgStore {
    // TODO: Make all members private

  public:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;

    // PRIVATE CREATORS
    explicit bslstl_format_FormatArgStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
        BSLS_KEYWORD_NOEXCEPT : d_args(args)
    {
    }
};

template <class t_CONTEXT, class... t_ARGS>
bslstl_format_FormatArgStore<t_CONTEXT, t_ARGS...>
bslstl_format_MakeFormatArgs(t_ARGS&... fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    return bslstl_format_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
}

template <class... t_ARGS>
bslstl_format_FormatArgStore<bslstl_format_DefaultFormatContext, t_ARGS...>
make_format_args(t_ARGS&... fmt_args)
{
    return bslstl_format_MakeFormatArgs<bslstl_format_DefaultFormatContext>(
                                                                  fmt_args...);
}
#endif

template <class t_CONTEXT>
class bslstl_format_BasicFormatArgs;

typedef bslstl_format_BasicFormatArgs<bslstl_format_DefaultFormatContext>
    format_args;

template <class t_CONTEXT>
class bslstl_format_BasicFormatArgs {
    // DATA
    size_t                             d_size;
    const basic_format_arg<t_CONTEXT> *d_data;

  public:
    // CREATORS
    bslstl_format_BasicFormatArgs() BSLS_KEYWORD_NOEXCEPT
    : d_size(0) {}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... t_ARGS>
    bslstl_format_BasicFormatArgs(
               const bslstl_format_FormatArgStore<t_CONTEXT, t_ARGS...>& store)
        BSLS_KEYWORD_NOEXCEPT                                       // IMPLICIT
    : d_size(sizeof...(t_ARGS)),
      d_data(store.d_args.data())
    {
    }
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
    friend t_OUT bslstl_format_VFormatImpl(
    t_OUT                                                               out,
    bsl::basic_string_view<t_CHAR>                                      fmtstr,
    bslstl_format_BasicFormatArgs<basic_format_context<t_OUT, t_CHAR> > args);
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

template <class t_OUT, class t_CHAR>
class basic_format_context {
  private:
    // TYPES
    typedef basic_format_arg<basic_format_context> Arg;

    // DATA
    bslstl_format_BasicFormatArgs<basic_format_context> d_args;
    t_OUT                                               d_out;

  public:
    // TYPES
    typedef t_OUT  iterator;
    typedef t_CHAR char_type;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
    template <class t_TYPE>
    using formatter_type = bsl::formatter<t_TYPE, t_CHAR>;
#endif

    // CREATORS TODO: This constructor should be made private (callable only by
    // 'bsl::format' internals)
    basic_format_context(
                      t_OUT                                               out,
                      bslstl_format_BasicFormatArgs<basic_format_context> args)
    : d_args(args)
    , d_out(out)
    {
    }

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

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
void
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::format_impl(
                                   basic_format_parse_context<t_CHAR>&   pc,
                                   basic_format_context<t_OUT, t_CHAR>&  fc,
                                   const void                           *value)
{
    bsl::formatter<t_TYPE, t_CHAR> f;
    pc.advance_to(f.parse(pc));
    fc.advance_to(f.format(*static_cast<const t_TYPE *>(value), fc));
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
    typename basic_format_context<t_OUT, char>::iterator format(
                                   int                                x,
                                   basic_format_context<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return bsl::copy(buf, result, fc.out());
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
    typename basic_format_context<t_OUT, char>::iterator format(
                                   string_view                        sv,
                                   basic_format_context<t_OUT, char>& fc) const
    {
        return bsl::copy(sv.begin(), sv.end(), fc.out());
    }
};
#endif

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
        bsl::formatter<int, t_CHAR> f;
        d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
        d_formatContext_p->advance_to(
                               bsl::as_const(f).format(x, *d_formatContext_p));
    }

    void operator()(bsl::string_view sv) const
    {
        bsl::formatter<bsl::string_view, t_CHAR> f;
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
t_OUT bslstl_format_VFormatImpl(
    t_OUT                                                               out,
    bsl::basic_string_view<t_CHAR>                                      fmtstr,
    bslstl_format_BasicFormatArgs<basic_format_context<t_OUT, t_CHAR> > args)
    // The actual meat of the implementation.  This overload is used when the
    // iterator type 't_OUT' matches the iterator type that 'args' is able to
    // format to.  In all other cases the iterator must be wrapped.
{
    typedef basic_format_context<t_OUT, t_CHAR> FC;

    basic_format_parse_context<t_CHAR>                pc(fmtstr, args.size());
    FC                                                fc(out, args);
    typename bsl::basic_string_view<t_CHAR>::iterator it = pc.begin();
    bslstl_format_FormatVisitor<t_OUT, t_CHAR>        visitor(pc, fc);

    while (it != pc.end()) {
        if (*it == '{') {
            ++it;
            if (it == pc.end()) {
                BSLS_THROW(format_error("unmatched {"));
            }
            else if (*it == '{') {
                // literal {
                ++it;
                out    = fc.out();
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
                        BSLS_THROW(format_error("arg id too large"));
                    }
                }
                if (it == pc.end()) {
                    BSLS_THROW(format_error("unmatched {"));
                }
            }
            if (id == size_t(-1)) {
                id = pc.next_arg_id();
            }
            else {
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
        }
        else if (*it == '}') {
            // must be escaped
            ++it;
            if (it == pc.end() || *it != '}') {
                BSLS_THROW(format_error("} must be escaped"));
            }
            ++it;
            out    = fc.out();
            *out++ = '}';
            fc.advance_to(out);
        }
        else {
            // just copy it
            out    = fc.out();
            *out++ = *it++;
            fc.advance_to(out);
        }
    }
    return fc.out();
}

template <class t_OUT, class t_CHAR, class t_CONTEXT>
t_OUT bslstl_format_VFormatImpl(
                               t_OUT                                    out,
                               bsl::basic_string_view<t_CHAR>           fmtstr,
                               bslstl_format_BasicFormatArgs<t_CONTEXT> args)
{
    bslstl_format_OutputIteratorImpl<char, t_OUT> wrappedOut(out);
    bslstl_format_VFormatImpl(
                            bslstl_format_OutputIteratorRef<char>(&wrappedOut),
                            fmtstr,
                            args);
    return out;
}

template <class t_OUT>
t_OUT vformat_to(t_OUT out, bsl::string_view fmtstr, format_args args)
{
    return bslstl_format_VFormatImpl(out, fmtstr, args);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_OUT, class... t_ARGS>
t_OUT format_to(t_OUT out, bsl::string_view fmtstr, const t_ARGS&... args)
{
    typedef basic_format_context<t_OUT, char>      Context;
    typedef bslstl_format_BasicFormatArgs<Context> Args;
    return bslstl_format_VFormatImpl(
                         out,
                         fmtstr,
                         Args(bslstl_format_MakeFormatArgs<Context>(args...)));
}

template <class... t_ARGS>
void format_to(bsl::string *out, bsl::string_view fmtstr, const t_ARGS&...args)
{
    format_to(bsl::back_inserter(*out), fmtstr, args...);
}

void vformat_to(bsl::string *out, bsl::string_view fmtstr, format_args args)
{
    vformat_to(bsl::back_inserter(*out), fmtstr, args);
}

template <class... t_ARGS>
bsl::string format(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bsl::string result;
    format_to(&result, fmtstr, args...);
    return result;
}

template <class... t_ARGS>
bsl::string format(bsl::allocator<char> alloc,
                   bsl::string_view     fmtstr,
                   const t_ARGS&...     args)
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
bsl::string
vformat(bsl::allocator<char> alloc, bsl::string_view fmt, format_args args)
{
    bsl::string result(alloc);
    vformat_to(&result, fmt, args);
    return result;
}

template <class... t_ARGS>
std::size_t formatted_size(bsl::string_view fmtstr, const t_ARGS&... args)
{
    bslstl_format_TruncatingIterator<char *> it(0, 0);
    format_to(it, fmtstr, args...);
    return it.count();
}

template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT> format_to_n(
                  t_OUT                                                 out,
                  typename bsl::iterator_traits<t_OUT>::difference_type n,
                  bsl::string_view                                      fmtstr,
                  const t_ARGS&...                                      args)
{
    if (n < 0)
        n = 0;
    bslstl_format_TruncatingIterator<t_OUT> it(out, n);
    format_to(it, fmtstr, args...);
    format_to_n_result<t_OUT> result;
    result.out  = it.underlying();
    result.size = it.count();
    return result;
}
#endif
}  // close namespace bslfmt
} // close enterprise namespace

namespace bsl {
// TEMPORARY HACKS TO MAKE THE FORMATTER BIT WORK
    using BloombergLP::bslfmt::basic_format_context;
    using BloombergLP::bslfmt::format_parse_context;
    using BloombergLP::bslfmt::format_error;
}

namespace bsl {
// FORMATTER SPECIALIZATIONS

//template <>
//struct formatter<int, char> : std::formatter<int, char> {
//};



//template <>
//struct formatter<int, char> {
//    BSLS_KEYWORD_CONSTEXPR_CPP20 format_parse_context::iterator parse(
//                                                      format_parse_context& pc)
//    {
//        if (pc.begin() != pc.end() && *pc.begin() != '}') {
//            BSLS_THROW(format_error("not implemented"));
//        }
//        return pc.begin();
//    }
//
//    template <class t_OUT>
//    typename basic_format_context<t_OUT, char>::iterator format(
//                                   int                                x,
//                                   basic_format_context<t_OUT, char>& fc) const
//    {
//        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
//        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
//        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
//        return std::copy(buf, result, fc.out());
//    }
//};

//template <>
//struct formatter<int, char> {
//
//    std::formatter<int, char> d_formatter;
//
//    BSLS_KEYWORD_CONSTEXPR_CPP20 bslfmt::format_parse_context::iterator parse(
//                                                      bslfmt::format_parse_context& pc)
//    {
//        if (pc.begin() != pc.end() && *pc.begin() != '}') {
//            BSLS_THROW(format_error("not implemented"));
//        }
//        return pc.begin();
//    }
//
//    template <class t_OUT>
//    typename bslfmt::basic_format_context<t_OUT, char>::iterator format(
//                                   int                                x,
//                                   bslfmt::basic_format_context<t_OUT, char>& fc) const
//    {
//        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
//        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
//        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
//        return std::copy(buf, result, fc.out());
//    }
//
//    BSLS_KEYWORD_CONSTEXPR_CPP20 std::format_parse_context::iterator parse(
//                                                      std::format_parse_context& pc)
//    {
//        if (pc.begin() != pc.end() && *pc.begin() != '}') {
//            BSLS_THROW(format_error("not implemented"));
//        }
//        return pc.begin();
//    }
//
//    template <class t_OUT>
//    typename std::basic_format_context<t_OUT, char>::iterator format(
//                                   int                                x,
//                                   std::basic_format_context<t_OUT, char>& fc) const
//    {
//        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
//        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
//        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
//        return std::copy(buf, result, fc.out());
//    }
//};

template <> struct formatter<int, char> {
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT&
                                                      pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT, template <class, class> class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT<t_OUT, char>::iterator format(
                                       int                            x,
                                       t_FORMAT_CONTEXT<t_OUT, char>& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        char  buf[NFUtil::ToCharsMaxLength<int>::k_VALUE];
        char *result = NFUtil::toChars(buf, buf + sizeof(buf), x);
        return bsl::copy(buf, result, fc.out());
    }
};

//template <>
//struct formatter<string_view, char> {
//    BSLS_KEYWORD_CONSTEXPR_CPP20 format_parse_context::iterator parse(
//                                                      format_parse_context& pc)
//    {
//        if (pc.begin() != pc.end() && *pc.begin() != '}') {
//            BSLS_THROW(format_error("not implemented"));
//        }
//        return pc.begin();
//    }
//
//    template <class t_OUT>
//    typename basic_format_context<t_OUT, char>::iterator format(
//                                   string_view                        sv,
//                                   basic_format_context<t_OUT, char>& fc) const
//    {
//        return bsl::copy(sv.begin(), sv.end(), fc.out());
//    }
//};

template <>
struct formatter<string_view, char> {
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc) {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename BloombergLP::bslfmt::basic_format_context<t_OUT, char>::iterator
    format(
                                   string_view                        sv,
           BloombergLP::bslfmt::basic_format_context<t_OUT, char>& fc) const
    {
        return bsl::copy(sv.begin(), sv.end(), fc.out());
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 std::format_parse_context::iterator parse(
                                                      std::format_parse_context& pc)
    {
        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(format_error("not implemented"));
        }
        return pc.begin();
    }

    template <class t_OUT>
    typename std::basic_format_context<t_OUT, char>::iterator format(
                                   string_view                        sv,
                                   std::basic_format_context<t_OUT, char>& fc) const
    {
        return bsl::copy(sv.begin(), sv.end(), fc.out());
    }
};

}



namespace std {
// FORMATTER SPECIALIZATIONS
template <>
struct formatter<bsl::string, char> : formatter<bsl::string_view, char> {
};
}  // close namespace bsl




#endif  // End C++11 code

#endif  // INCLUDED_BSLSTL_FORMATIMP

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
