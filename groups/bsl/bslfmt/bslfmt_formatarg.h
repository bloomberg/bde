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

#include <bslstl_array.h>
#include <bslstl_iterator.h>
#include <bslstl_monostate.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>
#include <bslstl_utility.h>
#include <bslstl_variant.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Fri Jul 12 17:38:43 2024
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
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        handle(bslmf::MovableRef<handle> rhs) BSLS_KEYWORD_NOEXCEPT;
            // This is required to support use within a variant on C++03, but
            // must *not* be specified for C++11 and later as it will result in
            // the implicit deletion of other defaulted special member
            // functions
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

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
    variant_type d_value;

    // FRIENDS
    friend class Format_FormatArg_ImpUtils;

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
                                       !bsl::is_same<t_TYPE, char>::value &&
                                       !bsl::is_same<t_TYPE, wchar_t>::value &&
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
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    // This check is a proxy for BSL_VARIANT_FULL_IMPLEMENTATION which is unset
    // at the end of bslstl_variant.h
    template <class t_VISITOR>
    decltype(auto) visit(t_VISITOR&& visitor);
#else
    template <class t_VISITOR>
    typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type visit(
                                                           t_VISITOR& visitor);
#endif
};

                      // -------------------------------
                      // class Format_FormatArg_ImpUtils
                      // -------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This class provides utility functions to enable manipulation of types
/// declared by this component. It is solely for private use by other components
/// of the `bslfmt` package and should not be used directly.
class Format_FormatArg_ImpUtils {
  public:
    // CLASS METHODS

    /// Replace the members of the specified `out` parameter by
    // `basic_format_arg` objects constructed from the members of the specified
    // `fmt_args` parameter.
    template <class t_CONTEXT, class... t_FMTARGS>
    static void makeFormatArgArray(
      bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> *out,
      t_FMTARGS&...                                                  fmt_args);
};

#endif

                               // --------------
                               // FREE FUNCTIONS
                               // --------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    // This check is a proxy for BSL_VARIANT_FULL_IMPLEMENTATION which is unset
    // at the end of bslstl_variant.h

/// Apply the specified `visitor` callable to the specified `arg`.
template <class t_VISITOR, class t_CONTEXT>
decltype(auto) visit_format_arg(t_VISITOR&&                 visitor,
                                basic_format_arg<t_CONTEXT> arg);
#else
/// Apply the specified `visitor` callable to the specified `arg`.
template <class t_VISITOR, class t_CONTEXT>
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
visit_format_arg(t_VISITOR& visitor, basic_format_arg<t_CONTEXT> arg);
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
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class t_OUT, class t_CHAR>
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::handle(
                           bslmf::MovableRef<handle> rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_value_p = bslmf::MovableRefUtil::move(
                                 bslmf::MovableRefUtil::access(rhs).d_value_p);
    d_format_impl_p = bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(rhs).d_format_impl_p);
}
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    // This check is a proxy for BSL_VARIANT_FULL_IMPLEMENTATION which is unset
    // at the end of bslstl_variant.h
template <class t_OUT, class t_CHAR>
template <class t_VISITOR>
decltype(auto) basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::visit(
                                                                 t_VISITOR&& v)
{
    // The possible return types need to all match exactly, on the basis that
    // the standard library enforces, on penalty of program ill-formedness, so
    // we should do the same.
    typedef typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type t1;
    typedef typename bsl::invoke_result<t_VISITOR&, bool&>::type           t2;
    typedef typename bsl::invoke_result<t_VISITOR&, char_type&>::type      t3;
    typedef typename bsl::invoke_result<t_VISITOR&, int&>::type            t4;
    typedef typename bsl::invoke_result<t_VISITOR&, unsigned&>::type       t5;
    typedef typename bsl::invoke_result<t_VISITOR&, long long&>::type      t6;
    typedef
        typename bsl::invoke_result<t_VISITOR&, unsigned long long&>::type t7;
    typedef typename bsl::invoke_result<t_VISITOR&, float&>::type          t8;
    typedef typename bsl::invoke_result<t_VISITOR&, double&>::type         t9;
    typedef typename bsl::invoke_result<t_VISITOR&, long double&>::type    t10;
    typedef
        typename bsl::invoke_result<t_VISITOR&, const char_type *&>::type  t11;
    typedef typename bsl::invoke_result<t_VISITOR&, const void *&>::type   t12;
    typedef typename bsl::invoke_result<t_VISITOR&,
                               bsl::basic_string_view<char_type>&>::type   t13;
    typedef typename bsl::invoke_result<t_VISITOR&, handle&>::type         t14;

    BSLMF_ASSERT((bsl::is_same<t1, t2 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t3 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t4 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t5 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t6 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t7 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t8 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t9 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t10>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t11>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t12>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t13>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t14>::value));

    return bsl::visit(std::forward<t_VISITOR>(v), d_value);
}
#else
template <class t_OUT, class t_CHAR>
template <class t_VISITOR>
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::visit(t_VISITOR& v)
{
    // The possible return types need to all match exactly, on the basis that
    // the standard library enforces, on penalty of program ill-formedness, so
    // we should do the same.
    typedef typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type t1;
    typedef typename bsl::invoke_result<t_VISITOR&, bool&>::type           t2;
    typedef typename bsl::invoke_result<t_VISITOR&, char_type&>::type      t3;
    typedef typename bsl::invoke_result<t_VISITOR&, int&>::type            t4;
    typedef typename bsl::invoke_result<t_VISITOR&, unsigned&>::type       t5;
    typedef typename bsl::invoke_result<t_VISITOR&, long long&>::type      t6;
    typedef
        typename bsl::invoke_result<t_VISITOR&, unsigned long long&>::type t7;
    typedef typename bsl::invoke_result<t_VISITOR&, float&>::type          t8;
    typedef typename bsl::invoke_result<t_VISITOR&, double&>::type         t9;
    typedef typename bsl::invoke_result<t_VISITOR&, long double&>::type    t10;
    typedef
        typename bsl::invoke_result<t_VISITOR&, const char_type *&>::type  t11;
    typedef typename bsl::invoke_result<t_VISITOR&, const void *&>::type   t12;
    typedef typename bsl::invoke_result<t_VISITOR&,
                               bsl::basic_string_view<char_type>&>::type   t13;
    typedef typename bsl::invoke_result<t_VISITOR&, handle&>::type         t14;

    BSLMF_ASSERT((bsl::is_same<t1, t2 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t3 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t4 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t5 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t6 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t7 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t8 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t9 >::value));
    BSLMF_ASSERT((bsl::is_same<t1, t10>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t11>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t12>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t13>::value));
    BSLMF_ASSERT((bsl::is_same<t1, t14>::value));

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
                                       !bsl::is_same<t_TYPE, char>::value &&
                                       !bsl::is_same<t_TYPE, wchar_t>::value &&
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


                      // -------------------------------
                      // class Format_FormatArg_ImpUtils
                      // -------------------------------

// CLASS METHODS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_FMTARGS>
void Format_FormatArg_ImpUtils::makeFormatArgArray(
       bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> *out,
       t_FMTARGS&...                                                  fmt_args)
{
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> tmp = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    out->swap(tmp);
}
#endif

                               // --------------
                               // FREE FUNCTIONS
                               // --------------

// FREE FUNCTIONS

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    // This check is a proxy for BSL_VARIANT_FULL_IMPLEMENTATION which is unset
    // at the end of bslstl_variant.h
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

}  // close namespace bslfmt
}  // close enterprise namespace

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
