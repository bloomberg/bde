// bslfmt_formatarg.h                                                 -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATARG
#define INCLUDED_BSLFMT_FORMATARG

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

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Jun 18 08:19:54 2024
// Command line: sim_cpp11_features.pl bslfmt_formatarg.h
# define COMPILING_BSLFMT_FORMATARG_H
# include <bslfmt_formatarg_cpp03.h>
# undef COMPILING_BSLFMT_FORMATARG_H
#else

namespace BloombergLP {
namespace bslfmt {

// FORWARD DECLARATIONS

template <class t_CHAR>
class basic_format_parse_context;

template <class t_OUT, class t_CHAR>
class basic_format_context;

template <class t_CONTEXT>
class basic_format_arg;

template <class t_VALUE>
class Format_OutputIteratorRef;

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_ARGS>
class Format_FormatArgStore;
#endif

// TYPEDEFS

typedef basic_format_context<Format_OutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_OutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;

                   // -------------------------------------
                   // class basic_format_arg<t_OUT, T_CHAR>
                   // -------------------------------------

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

        // FRIENDS
        friend class basic_format_arg<basic_format_context<t_OUT, t_CHAR> >;

        // PRIVATE CLASS METHODS
        template <class t_TYPE>
        static void format_impl(basic_format_parse_context<t_CHAR>&   pc,
                                basic_format_context<t_OUT, t_CHAR>&  fc,
                                const void                           *value);

        // PRIVATE CREATORS
        template <class t_TYPE>
        explicit handle(const t_TYPE& value) BSLS_KEYWORD_NOEXCEPT;

      public:
        // CREATORS
        handle(bslmf::MovableRef<handle> rhs) BSLS_KEYWORD_NOEXCEPT;

        // ACCESSORS
        void format(basic_format_parse_context<t_CHAR>&  pc,
                    basic_format_context<t_OUT, t_CHAR>& fc) const;
    };

    typedef
        typename BloombergLP::bsls::UnspecifiedBool<basic_format_arg>::BoolType
            BoolType;

  private:
    // PRIVATE TYPES
    typedef t_CHAR char_type;

    typedef bsl::variant<bsl::monostate,
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
        variant_type;

    // DATA
    variant_type    d_value;

    // FRIENDS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class t_CONTEXT, class... t_ARGS>
    friend Format_FormatArgStore<t_CONTEXT, t_ARGS...> Format_MakeFormatArgs(
                                                          t_ARGS&... fmt_args);

    template <class t_CONTEXT, class... t_FMTARGS>
    friend void
    Format_MakeFormatArgArray(
             bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> *out,
                 t_FMTARGS&... fmt_args);
#endif

    // PRIVATE CREATORS
    explicit basic_format_arg(bool value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(char_type value) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TYPE>
    explicit basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_same<t_TYPE, char>::value &&
                                   bsl::is_same<char_type, wchar_t>::value,
                                   int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TYPE>
    explicit basic_format_arg(
               t_TYPE value,
               typename bsl::enable_if<bsl::is_integral<t_TYPE>::value &&
                                       (sizeof(t_TYPE) <= sizeof(long long)),
                                       int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TYPE>
    explicit basic_format_arg(
              const t_TYPE& value,
              typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value ||
                                      (sizeof(t_TYPE) > sizeof(long long)),
                                      int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(float value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(double value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(long double value) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TRAITS>
    explicit basic_format_arg(
      bsl::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(bsl::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT;

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(std::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT;

    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(char_type *value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(const char_type *value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(void *value) BSLS_KEYWORD_NOEXCEPT;

    explicit basic_format_arg(const void *value) BSLS_KEYWORD_NOEXCEPT;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
    explicit basic_format_arg(std::nullptr_t) BSLS_KEYWORD_NOEXCEPT;
#endif

    // HIDDEN FRIENDS
    friend void swap(basic_format_arg& lhs, basic_format_arg& rhs)
        // Exchange the values of the specified 'lhs' and 'rhs'.
    {
        lhs.d_value.swap(rhs.d_value);
    }

  public:
    // CREATORS
    basic_format_arg() BSLS_KEYWORD_NOEXCEPT;

    // ACCESSORS
    operator BoolType() const BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS

    // BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE is a proxy for
    // BSL_VARIANT_FULL_IMPLEMENTATION which is unset at the end of
    // bslstl_variant.h
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
    template <class t_VISITOR>
    decltype(auto) visit(t_VISITOR&& visitor);
#else
    template <class t_VISITOR>
    typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type visit(
                                                           t_VISITOR& visitor);
#endif
};


// FREE FUNCTIONS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_FMTARGS>
void Format_MakeFormatArgArray(
          bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> *out,
          t_FMTARGS&...                                                  fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> tmp = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    out->swap(tmp);
}
#endif

                 // ------------------------------------------
                 // class Format_FormatArgStore<t_OUT, T_CHAR>
                 // ------------------------------------------


#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_ARGS>
class Format_FormatArgStore {

  private:
    // DATA
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> d_args;

    // FRIENDS
    template <class t_INNER_CONTEXT>
    friend class basic_format_args;

    template <class t_INNER_CONTEXT, class... t_INNER_ARGS>
    friend Format_FormatArgStore<t_INNER_CONTEXT, t_INNER_ARGS...>
        Format_MakeFormatArgs(t_INNER_ARGS&... fmt_args);

    // PRIVATE CREATORS
    explicit Format_FormatArgStore(
        const bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)>& args)
        BSLS_KEYWORD_NOEXCEPT : d_args(args)
    {
    }
};

// FREE FUNCTIONS

template <class t_CONTEXT, class... t_ARGS>
Format_FormatArgStore<t_CONTEXT, t_ARGS...>
Format_MakeFormatArgs(t_ARGS&... fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array;
    Format_MakeFormatArgArray<t_CONTEXT, t_ARGS...>(&arg_array, fmt_args...);
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_ARGS)> arg_array2 = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    return Format_FormatArgStore<t_CONTEXT, t_ARGS...>(arg_array);
}

template <class... t_ARGS>
Format_FormatArgStore<format_context, t_ARGS...>
make_format_args(t_ARGS&... fmt_args)
{
    return Format_MakeFormatArgs<format_context>(fmt_args...);
}

template <class... t_ARGS>
Format_FormatArgStore<wformat_context, t_ARGS...>
make_wformat_args(t_ARGS&... fmt_args)
{
    return Format_MakeFormatArgs<wformat_context>(fmt_args...);
}
#endif


                     // ----------------------------------
                     // class basic_format_args<t_CONTEXT>
                     // ----------------------------------


template <class t_CONTEXT>
class basic_format_args {
    // DATA
    size_t                             d_size;
    const basic_format_arg<t_CONTEXT> *d_data;

  public:

    // CREATORS
    basic_format_args() BSLS_KEYWORD_NOEXCEPT
    : d_size(0) {}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... t_ARGS>
    basic_format_args(
               const Format_FormatArgStore<t_CONTEXT, t_ARGS...>& store)
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
    template <class t_INNER_CONTEXT>
    friend size_t Format_FormatArgsSize(
            const basic_format_args<t_INNER_CONTEXT>& args);
};

typedef basic_format_args<format_context> format_args;

typedef basic_format_args<wformat_context> wformat_args;

template<class t_CONTEXT>
size_t Format_FormatArgsSize(const basic_format_args<t_CONTEXT>& args)
    // This component-private function returns the result of calling 'size()'
    // on the specified 'args' parameter. This is to permit access to the
    // private 'size' accessor of 'basic_format_args' without requiring long
    // distance friendship.
{
    return args.size();
}

// BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE is a proxy for
// BSL_VARIANT_FULL_IMPLEMENTATION which is unset at the end of
// bslstl_variant.h
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
template <class t_VISITOR, class t_CONTEXT>
decltype(auto) visit_format_arg(t_VISITOR&&                 visitor,
                                basic_format_arg<t_CONTEXT> arg)
{
    return arg.visit(std::forward<t_VISITOR>(visitor));
}
#else
template <class t_VISITOR, class t_CONTEXT>
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
visit_format_arg(t_VISITOR& visitor, basic_format_arg<t_CONTEXT> arg)
{
    return arg.visit(visitor);
}
#endif

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

               // ---------------------------------------------
               // class basic_format_arg<t_OUT, T_CHAR>::handle
               // ---------------------------------------------

// PRIVATE CLASS METHODS
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

// PRIVATE CREATORS
template <class t_OUT, class t_CHAR>
template <class t_TYPE>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::handle(
                                     const t_TYPE& value) BSLS_KEYWORD_NOEXCEPT
: d_value_p(BSLS_UTIL_ADDRESSOF(value))
, d_format_impl_p(format_impl<t_TYPE>)
{
}

// CREATORS
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::handle(
                           bslmf::MovableRef<handle> rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_value_p = bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(rhs).d_value_p);
    d_format_impl_p = bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(rhs).d_format_impl_p);
}

// ACCESSORS
template <class t_OUT, class t_CHAR>
void basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::format(
                                 basic_format_parse_context<t_CHAR>&  pc,
                                 basic_format_context<t_OUT, t_CHAR>& fc) const
{
    d_format_impl_p(pc, fc, d_value_p);
}

                   // -------------------------------------
                   // class basic_format_arg<t_OUT, T_CHAR>
                   // -------------------------------------

// MANIPULATORS

// BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE is a proxy for
// BSL_VARIANT_FULL_IMPLEMENTATION which is unset at the end of
// bslstl_variant.h
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE
template <class t_OUT, class t_CHAR>
template <class t_VISITOR>
decltype(auto) basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::visit(
                                                                 t_VISITOR&& v)
{
    return bsl::visit(std::forward<t_VISITOR>(v), d_value);
}
#else
template <class t_OUT, class t_CHAR>
template <class t_VISITOR>
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::visit(t_VISITOR& v)
{
    return bsl::visit(v, d_value);
}
#endif

// PRIVATE CREATORS
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                              bool value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                         char_type value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_same<t_TYPE, char>::value &&
                                   bsl::is_same<char_type, wchar_t>::value,
                                   int>::type) BSLS_KEYWORD_NOEXCEPT
{
    static const std::ctype<wchar_t>& ct =
                  std::use_facet<std::ctype<wchar_t> >(std::locale::classic());
    d_value = ct.widen(value);
}

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
               t_TYPE value,
               typename bsl::enable_if<bsl::is_integral<t_TYPE>::value &&
                                       (sizeof(t_TYPE) <= sizeof(long long)),
                                       int>::type) BSLS_KEYWORD_NOEXCEPT
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

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
              const t_TYPE& value,
              typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value ||
                                      (sizeof(t_TYPE) > sizeof(long long)),
                                      int>::type) BSLS_KEYWORD_NOEXCEPT
: d_value(handle(value))
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                             float value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                            double value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                       long double value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
       bsl::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
  bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                  const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
    BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
  std::basic_string<char_type, t_TRAITS, t_ALLOC>& value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
    BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                        char_type *value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<const char_type *>(value))
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                  const char_type *value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                             void *value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<const void *>(value))
{
}

template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                       const void *value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                          std::nullptr_t) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<const void *>(nullptr))
{
}
#endif



// CREATORS
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg()
    BSLS_KEYWORD_NOEXCEPT
{
}

// ACCESSORS
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::operator BoolType()
    const BSLS_KEYWORD_NOEXCEPT
{
    return BoolType::makeValue(
                             !bsl::holds_alternative<bsl::monostate>(d_value));
}

}  // close namespace bslfmt
} // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMATARG

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
