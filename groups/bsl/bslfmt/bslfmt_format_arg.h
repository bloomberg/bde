// bslfmt_format_arg.h                                                -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT_ARG
#define INCLUDED_BSLFMT_FORMAT_ARG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proxy for an argument for use by bsl::format
//
//@CLASSES:
//  bslfmt::basic_format_arg: standard-compliant argment
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `basic_format_arg`, providing access via a "visitor pattern".  The
// value is stored as if using a variant of the following types (as specified
// by the Standard):
//
// - std::monostate (only if the object was default-constructed)
// - bool    Context::char_type
// - int
// - unsigned int
// - long long int
// - unsigned long long int
// - float
// - double
// - long double
// - const Context::char_type *
// - std::basic_string_view<Context::char_type>
// - const void *
// - basic_format_arg::handle
//
// Where `basic_format_arg::handle` is a type-erased wrapper holding a
// reference to a user-defined type.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Default construction and value verification
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`. In addition, there
// are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we want to construct a default-constructed `basic_format_arg` and
// verify that it contains no value.
//
// ```
//   bslfmt::format_args args;
//
//   assert(!args.get(0));
// ```

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
#include <bsls_nullptr.h>
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
// clang-format off
// Include version that can be compiled with C++03
// Generated on Tue Jan 14 14:15:43 2025
// Command line: sim_cpp11_features.pl bslfmt_format_arg.h

# define COMPILING_BSLFMT_FORMAT_ARG_H
# include <bslfmt_format_arg_cpp03.h>
# undef COMPILING_BSLFMT_FORMAT_ARG_H

// clang-format on
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
class Format_ContextOutputIteratorRef;

// TYPEDEFS

typedef basic_format_context<Format_ContextOutputIteratorRef<char>, char>
    format_context;

typedef basic_format_context<Format_ContextOutputIteratorRef<wchar_t>, wchar_t>
    wformat_context;

                   // -------------------------------------
                   // class basic_format_arg<t_OUT, T_CHAR>
                   // -------------------------------------

/// This class provides an STL-compliant `basic_format_arg` which holds (by
/// value for scalar and pointer types, by reference for user-defined types).
/// These types should not be constructed directly by the user.
template <class t_OUT, class t_CHAR>
class basic_format_arg<basic_format_context<t_OUT, t_CHAR> > {
  public:
    // TYPES

    /// This class provides a type-erased wrapper which holds a pointer to a
    /// user-defined type and permits its formatting using the appropriate
    /// bsl::formatter type.
    class handle {
      private:
        // DATA
        const void *d_value_p;                  // Pointer to the referenced
                                                // value.

        void (*d_format_impl_p)(basic_format_parse_context<t_CHAR>&,
                                basic_format_context<t_OUT, t_CHAR>&,
                                const void *);  // Pointer to a format_impl
                                                // instance.

        // FRIENDS
        friend class basic_format_arg<basic_format_context<t_OUT, t_CHAR> >;

        // PRIVATE CLASS METHODS

        /// Format the specified `value` using a `bsl::formatter` instance for
        /// the specified template parameter `t_TYPE`. The constructed
        /// formatter will parse the specifiecation in the specified `pc`
        /// context and write the output to the specified `fc` context.
        template <class t_TYPE>
        static void format_impl(basic_format_parse_context<t_CHAR>&   pc,
                                basic_format_context<t_OUT, t_CHAR>&  fc,
                                const void                           *value);

        // PRIVATE CREATORS

        /// Construct a `handle` referencing the specified `value` and
        /// initialize the contained formatting function to an instance of
        /// `format_impl` for the template parameter `t_TYPE`.
        template <class t_TYPE>
        explicit handle(const t_TYPE& value) BSLS_KEYWORD_NOEXCEPT;

      public:
        // CREATORS
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        /// Move-construct a `handle` from the specified `rhs`. This is
        /// required to support use within a bsl::variant on C++03, but must
        /// *not* be specified for C++11 and later as it will result in the
        /// implicit deletion of other defaulted special member functions.
        handle(bslmf::MovableRef<handle> rhs) BSLS_KEYWORD_NOEXCEPT;
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

        // ACCESSORS

        /// Format the contained value using a formatter for the template
        /// parameter `t_TYPE` used in the construction of this object. The
        /// constructed formatter will parse the specifiecation in the
        /// specified `pc` context and write the output to the specified `fc`
        /// context.
        void format(basic_format_parse_context<t_CHAR>&  pc,
                    basic_format_context<t_OUT, t_CHAR>& fc) const;
    };

    typedef typename BloombergLP::bsls::UnspecifiedBool<basic_format_arg>
                                                   UnspecifiedBoolType;
    typedef typename UnspecifiedBoolType::BoolType BoolType;

  private:
    // NOT IMPLEMENTED
    bool operator==(const basic_format_arg&) const BSLS_KEYWORD_DELETED;

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
    variant_type d_value;  // The contained value or a reference thereto.

    // FRIENDS
    friend class Format_ArgUtil;

    // PRIVATE CREATORS

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(bool value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(char_type value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value` widened from
    /// type `char` to type `wchar_t`, which is then held by value. This
    /// constructor only participates in overload resolution if `value` is of
    /// type `char` and `t_CHAR` is of type `wchar_t`.
    template <class t_TYPE>
    explicit basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_same<t_TYPE, char>::value &&
                                       bsl::is_same<char_type, wchar_t>::value,
                                   int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value. This constructor only participates in overload
    /// resolution if `value` is an signed integer type that can be held within
    /// a `long long` or an unsigned integer type that can be held within an
    /// `unsigned long long`.
    template <class t_TYPE>
    explicit basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_integral<t_TYPE>::value &&
                                       !bsl::is_same<t_TYPE, char>::value &&
                                       !bsl::is_same<t_TYPE, wchar_t>::value &&
                                       (sizeof(t_TYPE) <= sizeof(long long)),
                                   int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by reference in a `basic_format_arg::handle`. This
    /// constructor only participates in overload resolution if `value` is not
    /// an integer or if `value` is an signed integer type that can not be held
    /// within a `long long` or an unsigned integer type that can not be held
    /// within an `unsigned long long`.  Participation in overload resolution
    /// is also disabled if `value` is of type `long double`.
    template <class t_TYPE>
    explicit basic_format_arg(
         const t_TYPE& value,
         typename bsl::enable_if<(!bsl::is_integral<t_TYPE>::value &&
                                  !bsl::is_same<t_TYPE, long double>::value) ||
                                     (sizeof(t_TYPE) > sizeof(long long)),
                                 int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(float value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(double value) BSLS_KEYWORD_NOEXCEPT;

    /// Terminate by calling BSLMF_ASSERT.  This constructor only participates
    /// in overload resolution if `value` is of the currently unsupported type
    /// `long double`.  Note: this is required to use a template parameter to
    /// ensure that this function is only instantiated when an attempt is made
    /// to use it.
    template <class t_TYPE>
    explicit basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_same<t_TYPE, long double>::value,
                                   int>::type = 0) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    template <class t_TRAITS>
    explicit basic_format_arg(
      bsl::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    template <class t_TRAITS>
    explicit basic_format_arg(
      std::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT;
#endif
#endif

    /// Construct a `basic_format_arg` holding (by value) a `string_view`
    /// constructed from the specified `value`.
    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(bsl::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` holding (by value) a `string_view`
    /// constructed from the specified `value`.
    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` holding (by value) a `string_view`
    /// constructed from the specified `value`.
    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(std::basic_string<char_type, t_TRAITS, t_ALLOC>&
                                  value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` holding (by value) a `string_view`
    /// constructed from the specified `value`.
    template <class t_TRAITS, class t_ALLOC>
    explicit basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
        BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(char_type *value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(const char_type *value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(void *value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` from the specified `value`, which is
    /// then held by value.
    explicit basic_format_arg(const void *value) BSLS_KEYWORD_NOEXCEPT;

    /// Construct a `basic_format_arg` which holds
    /// `static_cast<const void *>(nullptr)`.
    explicit basic_format_arg(bsl::nullptr_t) BSLS_KEYWORD_NOEXCEPT;

    // HIDDEN FRIENDS

    /// Exchange the values of the specified `lhs` and `rhs`.
    friend void swap(basic_format_arg& lhs, basic_format_arg& rhs)
    {
        lhs.d_value.swap(rhs.d_value);
    }

  public:
    // CREATORS

    /// Construct a `basic_format_arg` which holds no value.
    basic_format_arg() BSLS_KEYWORD_NOEXCEPT;

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    /// Move-construct a `basic_format_arg` from the specified `rhs`. This is
    /// required to support use on C++03, but must
    /// *not* be specified for C++11 and later as it will result in the
    /// implicit deletion of other defaulted special member functions.
    basic_format_arg(
                bslmf::MovableRef<basic_format_arg> rhs) BSLS_KEYWORD_NOEXCEPT;
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

    // ACCESSORS

    /// Return whether this object holds a value.
    operator BoolType() const BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    /// Move-assign a `basic_format_arg` from the specified `rhs`. This is
    /// required to support use on C++03, but must
    /// *not* be specified for C++11 and later as it will result in the
    /// implicit deletion of other defaulted special member functions.
    basic_format_arg &operator=(
                bslmf::MovableRef<basic_format_arg> rhs) BSLS_KEYWORD_NOEXCEPT;
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) &&               \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE)
    // This check is a proxy for BSL_VARIANT_FULL_IMPLEMENTATION which is unset
    // at the end of bslstl_variant.h

    /// Invoke the specified `visitor` functor on the value contained by this
    /// object, providing that functor non-modifiable access to that value.
    /// `visitor` must be a functor that can be called as if it had the
    /// following signature:
    /// ```
    ///     void operator()(bsl::monostate) const;
    ///     void operator()(bool x) const;
    ///     void operator()(t_CHAR x) const;
    ///     void operator()(unsigned x) const;
    ///     void operator()(long long x) const;
    ///     void operator()(unsigned long long x) const;
    ///     void operator()(float x) const;
    ///     void operator()(double x) const;
    ///     void operator()(long double x) const;
    ///     void operator()(const t_CHAR *x) const;
    ///     void operator()(const void *x) const;
    ///     void operator()(int x) const;
    ///     void operator()(bsl::basic_string_view<t_CHAR> sv) const;
    ///     void operator()(const handle& h) const;
    /// ```
    template <class t_VISITOR>
    decltype(auto) visit(t_VISITOR&& visitor);
#else
    /// Invoke the specified `visitor` functor on the value contained by this
    /// object, providing that functor non-modifiable access to that value.
    /// `visitor` must be a functor that can be called as if it had the
    /// following signature:
    /// ```
    ///     void operator()(bsl::monostate) const;
    ///     void operator()(bool x) const;
    ///     void operator()(t_CHAR x) const;
    ///     void operator()(unsigned x) const;
    ///     void operator()(long long x) const;
    ///     void operator()(unsigned long long x) const;
    ///     void operator()(float x) const;
    ///     void operator()(double x) const;
    ///     void operator()(long double x) const;
    ///     void operator()(const t_CHAR *x) const;
    ///     void operator()(const void *x) const;
    ///     void operator()(int x) const;
    ///     void operator()(bsl::basic_string_view<t_CHAR> sv) const;
    ///     void operator()(const handle& h) const;
    /// ```
    template <class t_VISITOR>
    typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type visit(
                                                           t_VISITOR& visitor);
#endif
};

                            // --------------------
                            // class Format_ArgUtil
                            // --------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// This class provides utility functions to enable manipulation of types
/// declared by this component. It is solely for private use by other components
/// of the `bslfmt` package and should not be used directly.
class Format_ArgUtil {
  public:
    // CLASS METHODS

    /// Replace the members of the specified `out` parameter by
    /// `basic_format_arg` objects constructed from the members of the specified
    /// `fmt_args` parameter.
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

/// Invoke the specified `visitor` functor on the value contained by the
/// specified `arg`, providing that functor non-modifiable access to that
/// value. `visitor` must be a functor that can be called as if it had the
/// following signature:
/// ```
///     void operator()(bsl::monostate) const;
///     void operator()(bool x) const;
///     void operator()(t_CHAR x) const;
///     void operator()(unsigned x) const;
///     void operator()(long long x) const;
///     void operator()(unsigned long long x) const;
///     void operator()(float x) const;
///     void operator()(double x) const;
///     void operator()(long double x) const;
///     void operator()(const t_CHAR *x) const;
///     void operator()(const void *x) const;
///     void operator()(int x) const;
///     void operator()(bsl::basic_string_view<t_CHAR> sv) const;
///     void operator()(const handle& h) const;
/// ```
template <class t_VISITOR, class t_CONTEXT>
decltype(auto) visit_format_arg(t_VISITOR&&                 visitor,
                                basic_format_arg<t_CONTEXT> arg);
#else

/// Invoke the specified `visitor` functor on the value contained by the
/// specified `arg`, providing that functor non-modifiable access to that
/// value. `visitor` must be a functor that can be called as if it had the
/// following signature:
/// ```
///     void operator()(bsl::monostate) const;
///     void operator()(bool x) const;
///     void operator()(t_CHAR x) const;
///     void operator()(unsigned x) const;
///     void operator()(long long x) const;
///     void operator()(unsigned long long x) const;
///     void operator()(float x) const;
///     void operator()(double x) const;
///     void operator()(long double x) const;
///     void operator()(const t_CHAR *x) const;
///     void operator()(const void *x) const;
///     void operator()(int x) const;
///     void operator()(bsl::basic_string_view<t_CHAR> sv) const;
///     void operator()(const handle& h) const;
/// ```
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
inline
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
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle::handle(
                                     const t_TYPE& value) BSLS_KEYWORD_NOEXCEPT
: d_value_p(BSLS_UTIL_ADDRESSOF(value))
, d_format_impl_p(format_impl<t_TYPE>)
{
}

// CREATORS
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class t_OUT, class t_CHAR>
inline
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
inline
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
inline
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
inline
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
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                              bool value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                         char_type value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
inline
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
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
           t_TYPE value,
           typename bsl::enable_if<bsl::is_integral<t_TYPE>::value &&
                                       !bsl::is_same<t_TYPE, char>::value &&
                                       !bsl::is_same<t_TYPE, wchar_t>::value &&
                                       (sizeof(t_TYPE) <= sizeof(long long)),
                                   int>::type) BSLS_KEYWORD_NOEXCEPT
{
    if (static_cast<t_TYPE>(-1) < static_cast<t_TYPE>(0)) {
        // `t_TYPE` is signed
        if (sizeof(t_TYPE) <= sizeof(int)) {
            d_value.template emplace<int>(static_cast<int>(value));
        }
        else {
            d_value.template emplace<long long>(value);
        }
    }
    else {
        // `t_TYPE` is unsigned
        if (sizeof(t_TYPE) <= sizeof(int)) {
            d_value.template emplace<unsigned int>(
                                             static_cast<unsigned int>(value));
        }
        else {
            d_value.template emplace<unsigned long long>(value);
        }
    }
}

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
         const t_TYPE& value,
         typename bsl::enable_if<(!bsl::is_integral<t_TYPE>::value &&
                                  !bsl::is_same<t_TYPE, long double>::value) ||
                                     (sizeof(t_TYPE) > sizeof(long long)),
                                 int>::type) BSLS_KEYWORD_NOEXCEPT
: d_value(handle(value))
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                             float value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                            double value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
template <class t_TYPE>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
              t_TYPE value,
              typename bsl::enable_if<bsl::is_same<t_TYPE, long double>::value,
                                      int>::type) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<long double>(value))
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT
    static_assert(!bsl::is_same<t_TYPE, long double>::value,
                  "long double not supported by bsl::format");
#else
    BSLMF_ASSERT((!bsl::is_same<t_TYPE, long double>::value));
#endif
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
       bsl::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
template <class t_OUT, class t_CHAR>
template <class t_TRAITS>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
       std::basic_string_view<char_type, t_TRAITS> value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}
#endif
#endif

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
  bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                  const bsl::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
    BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
  std::basic_string<char_type, t_TRAITS, t_ALLOC>& value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
template <class t_TRAITS, class t_ALLOC>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                  const std::basic_string<char_type, t_TRAITS, t_ALLOC>& value)
    BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<bsl::basic_string_view<char_type> >(value))
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                        char_type *value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<const char_type *>(value))
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                  const char_type *value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                             void *value) BSLS_KEYWORD_NOEXCEPT
: d_value(static_cast<const void *>(value))
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                       const void *value) BSLS_KEYWORD_NOEXCEPT
: d_value(value)
{
}

template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                                          bsl::nullptr_t) BSLS_KEYWORD_NOEXCEPT

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NULLPTR)
: d_value(static_cast<const void *>(nullptr))
#else
: d_value(static_cast<const void *>(0))
#endif
{
}

// CREATORS
template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg()
    BSLS_KEYWORD_NOEXCEPT
{
}

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::basic_format_arg(
                 bslmf::MovableRef<basic_format_arg> rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_value = bslmf::MovableRefUtil::move(
                                 bslmf::MovableRefUtil::access(rhs).d_value);
}
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

// ACCESSORS
template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::operator BoolType()
    const BSLS_KEYWORD_NOEXCEPT
{
    return UnspecifiedBoolType::makeValue(
                             !bsl::holds_alternative<bsl::monostate>(d_value));
}

// MANIPULATORS
#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class t_OUT, class t_CHAR>
inline
basic_format_arg<basic_format_context<t_OUT, t_CHAR> > &
basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::operator=(
                 bslmf::MovableRef<basic_format_arg> rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_value = bslmf::MovableRefUtil::move(
                                 bslmf::MovableRefUtil::access(rhs).d_value);
    return *this;
}
#endif  // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                            // --------------------
                            // class Format_ArgUtil
                            // --------------------

// CLASS METHODS

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_CONTEXT, class... t_FMTARGS>
inline
void Format_ArgUtil::makeFormatArgArray(
       bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> *out,
       t_FMTARGS&...                                                  fmt_args)
{
#ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // Use the form of braced initialization that is valid in C++03
    bsl::array<basic_format_arg<t_CONTEXT>, sizeof...(t_FMTARGS)> tmp = {
        {basic_format_arg<t_CONTEXT>(fmt_args)...}};
    *out = bslmf::MovableRefUtil::move(tmp);
#else
    *out = { {basic_format_arg<t_CONTEXT>(fmt_args)...} };
#endif
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
inline
decltype(auto) visit_format_arg(t_VISITOR&&                 visitor,
                                basic_format_arg<t_CONTEXT> arg)
{
    return arg.visit(std::forward<t_VISITOR>(visitor));
}
#else
template <class t_VISITOR, class t_CONTEXT>
inline
typename bsl::invoke_result<t_VISITOR&, bsl::monostate&>::type
visit_format_arg(t_VISITOR& visitor, basic_format_arg<t_CONTEXT> arg)
{
    return arg.visit(visitor);
}
#endif

}  // close namespace bslfmt
}  // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMAT_ARG

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
