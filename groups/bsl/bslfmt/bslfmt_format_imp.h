// bslfmt_format_imp.h                                                   -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMAT_IMP
#define INCLUDED_BSLFMT_FORMAT_IMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compliant `format` implementation
//
//@CLASSES:
//  bslfmt::format_to_n_result: result type for format_to_n.
//
//@CANONICAL_HEADER: bsl_format.h
//
//@SEE_ALSO: ISO C++ Standard, <format>
//
//@DESCRIPTION: This component will provide, in the `bslfmt` namespace,
// compliant implementations of the following free functions exposed by the
// standard <format> header.  These are available for C++03 and later.
//
// * format
// * format_to
// * format_to_n
// * formatted_size
// * vformat
// * vformat_to
//
// Note 1: The overloads taking a `locale` parameter are *not* provided.
// Note 2: Compile-time format string checking is *not* performed.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use `bsl::format_to_n_result`.
//
///User-provided formatters
///------------------------
//
// User-provided formatters are supported by the BSL implementation, just as
// they are by the standard library implementation.  However, in order for them
// to be compatible with both implementations, there are specific requirements,
// notably:
//
// - If you will define a formatter for your type `T`, do so in the same
//   component header that defines `T` itself.  This avoids issues due to
//   users forgetting to include the header for the formatter.
// - Define `bsl::formatter<T>` - *DO NOT* define `std::formatter<T>` - Use
//   template arguments for the format context and parse context
//   parameters.  This is essential as the parameter type passed in will
//   depend upon underlying implementation.
// - The `parse` function should be constexpr in C++20, but this is not
//   required (and may not be possible) for earlier C++ standards.
//
// An example of a user defined formatter is as follows:
//
// ```
// namespace bsl {
//
// template <class t_CHAR> struct formatter<UserDefinedType, t_CHAR> {
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
// ```
// #include <bslfmt_format.h>
// #include <bsl_iostring.h>
//
// bsl::string doFormat(int value)
// {
//     bsl::string res = bslfmt::format("{}", value);
//     return res;
// }
//
// int main()
// {
//     bsl::cout << doFormat(99) << bsl::endl;
//     return 0;
// }
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
#include <bslfmt_format_arg.h>
#include <bslfmt_format_args.h>
#include <bslfmt_format_context.h>
#include <bslfmt_format_string.h>
#include <bslfmt_formatparsecontext.h>

#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterbool.h>
#include <bslfmt_formattercharacter.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterfloating.h>
#include <bslfmt_formatterpointer.h>
#include <bslfmt_formatterstring.h>

#include <iterator>             // 'back_insert_iterator'

#include <stddef.h>             // 'size_t', 'ptrdiff_t'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Wed Mar  5 09:57:03 2025
// Command line: sim_cpp11_features.pl bslfmt_format_imp.h

# define COMPILING_BSLFMT_FORMAT_IMP_H
# include <bslfmt_format_imp_cpp03.h>
# undef COMPILING_BSLFMT_FORMAT_IMP_H

// clang-format on
#else

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#  define BSLFMT_FORMAT_STRING_PARAMETER  bslfmt::format_string<t_ARGS...>
#  define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<t_ARGS...>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#  define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#  define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace BloombergLP {
namespace bslfmt {

                 // ===================================
                 // class Format_Imp_TruncatingIterator
                 // ===================================

/// This component-private type provides a stateful output iterator whose
/// purpose is to count the number of characters written for use by the
/// `formatted_size` free functions.  It holds an underlying output iterator (to
/// which writes are delegated), a maximum character count permitted and a
/// current character count to keep track of how many characters would be
/// written if there were no limit.
template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
class Format_Imp_TruncatingIterator {
  private:
    // TYPES
    typedef typename bsl::iterator_traits<t_ITERATOR>::difference_type DT;

    // DATA
    t_ITERATOR  d_iterator; // underlying iterator
    t_DIFF_TYPE d_limit;    // character limit
    t_DIFF_TYPE d_count;    // current character count

    // NOT IMPLEMENTED

    /// The postfix operator must be deleted because, as a counting iterator,
    /// return by value can cause data inconsistency.
    Format_Imp_TruncatingIterator operator++(int) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef t_DIFF_TYPE              difference_type;
    typedef t_VALUE_TYPE             value_type;
    typedef void                     reference;
    typedef void                     pointer;

    // CREATORS

    /// Create a instance containing a copy of the specified `iterator` as
    /// underlying iterater with the maximum character count set to the
    /// specified `limit` and the current character count set to zero.
    Format_Imp_TruncatingIterator(t_ITERATOR iterator, t_DIFF_TYPE limit);

    // MANIPULATORS

    /// Do nothing and return a reference to this object.  This is included to
    /// ensure compliance with the C++ Standard's LegacyOutputIterator
    /// requirements.
    Format_Imp_TruncatingIterator& operator*();

    /// Increment the current character count.  If the current character count
    /// is less than the maximum character count (set on construction), assign
    /// the specified `x` to the stored underlying iterator and increment the
    /// stored underlying iterator.
    void operator=(t_VALUE_TYPE x);

    /// Do nothing and return a reference to this object.  This is included to
    /// ensure compliance with the C++ Standard's LegacyOutputIterator
    /// requirements.
    Format_Imp_TruncatingIterator& operator++();

    // ACCESSORS

    /// Return the current character count.  Note that this is the number of
    /// calls to `operator=` regardless of whether the limit has been breached.
    t_DIFF_TYPE count() const;

    /// Return the underlying iterator.
    t_ITERATOR underlying() const;
};


                      // -------------------------------
                      // class format_to_n_result<t_OUT>
                      // -------------------------------


#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
// It is necessary to alias the standard library type where available, to
// ensure clients of the `format_to_n` can use the BSL and the Standard Library
// implementations interchaneably without having to convert between return
// types.
using std::format_to_n_result;
#else
/// This type mirrors the `format_to_n_result` type as specified in the
/// standard.  It exists to enable the `format_to_n` function to return both an
/// output iterator (the specified `t_OUT` template parameter) and the
/// untrunctated size.
template <class t_OUT>
struct format_to_n_result {
  public:
    // PUBLIC DATA
    t_OUT out;  // Output iterator
    typename bsl::iterator_traits<t_OUT>::difference_type
          size;  // Untruncated size
};
#endif

            // ---------------------------------------------------
            // class Format_Imp_Visitor<t_ITERATOR, t_OUT, t_CHAR>
            // ---------------------------------------------------

/// Component-private type acting as the visitor in a call to
/// `visit_format_arg`.
template <class t_OUT, class t_CHAR>
struct Format_Imp_Visitor {
  private:
    // DATA
    basic_format_parse_context<t_CHAR>  *d_parseContext_p;  // context for
                                                            // format string.
    basic_format_context<t_OUT, t_CHAR> *d_formatContext_p; // context for
                                                            // output iterator.

    // PRIVATE TYPES
    typedef
        typename basic_format_arg<basic_format_context<t_OUT, t_CHAR> >::handle
            handle;

  public:
    // CREATORS

    /// Create an instance of this type referencing the specified `pc` and the
    /// specified `fc` for format strings and output iterators respectively.
    Format_Imp_Visitor(basic_format_parse_context<t_CHAR>&  pc,
                         basic_format_context<t_OUT, t_CHAR>& fc);

    // MANIPULATORS

    /// Throw a `format_error` exception to indicate this overload should never
    /// get called.
    void operator()(bsl::monostate) const;

    /// Write a string representation of the specified `x` to the output
    /// iterator in `d_format_context_p`, formatted according to the parse
    /// string referenced by `d_formatContext_p`.  This is done by constructing
    /// an appropriate `formatter` of a type corresponding to the argument type
    /// of the specified `x`, then call `parse` and `format` on that
    /// constructed `formatter`.
    void operator()(bool value) const;
    void operator()(t_CHAR value) const;
    void operator()(unsigned value) const;
    void operator()(long long value) const;
    void operator()(unsigned long long value) const;
    void operator()(float value) const;
    void operator()(double value) const;
    void operator()(long double value) const;
    void operator()(const t_CHAR *value) const;
    void operator()(const void *value) const;
    void operator()(int value) const;
    void operator()(bsl::basic_string_view<t_CHAR> value) const;

    /// Write a string representation of type referenced by the specified `h`
    /// to the output iterator in `d_format_context_p`, formatted according to
    /// the parse string referenced by `d_formatContext_p`.  This is done by
    /// delegating to the `handle::format` function.
    void operator()(const handle& h) const;
};

                     // ----------------------------------
                     // class Format_Imp_Processor<t_CHAR>
                     // ----------------------------------

/// This component-private namespace struct provides access to formatting
/// implementation functions to which the `bslfmt::format` family of free
/// functions can delegate.
template <class t_CHAR>
struct Format_Imp_Processor {
  private:
    // PRIVATE CLASS METHODS

    /// Format the specified `args` according to the format string specified by
    /// `fmtStr` and write the result to the output iterator specified by
    /// `out`.  Return an iterator one past the end of the output range.
    template <class t_OUT>
    static t_OUT processImp(
         t_OUT&                                                         out,
         bsl::basic_string_view<t_CHAR>                                 fmtStr,
         const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args);

  public:
    // CLASS METHODS

    /// Format the specified `args` according to the format string specified by
    /// `fmtStr` and write the result to the output iterator specified by
    /// `out`.  Return an iterator one past the end of the output range.  This
    /// function participates in overload resolution if `out` is of
    /// `Format_ContextOutputIteratorRef` type.
    static Format_ContextOutputIteratorRef<t_CHAR>
    process(Format_ContextOutputIteratorRef<t_CHAR> out,
            bsl::basic_string_view<t_CHAR>          fmtStr,
            const basic_format_args<
                basic_format_context<Format_ContextOutputIteratorRef<t_CHAR>,
                                     t_CHAR> >&     args);

    /// Format the specified `args` according to the format string specified by
    /// `fmtStr` and write the result to the output iterator specified by
    /// `out`.  Return an iterator one past the end of the output range.  This
    /// function participates in overload resolution if `out` is not of
    /// `Format_ContextOutputIteratorRef` type.
    template <class t_OUT, class t_CONTEXT>
    static t_OUT process(t_OUT                               out,
                         bsl::basic_string_view<t_CHAR>      fmtStr,
                         const basic_format_args<t_CONTEXT>& args);
};

                               // --------------
                               // FREE FUNCTIONS
                               // --------------

// FREE FUNCTIONS

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the output
/// iterator given by the specified `out` parameter.  In the event of an error
/// the exception `format_error` is thrown.  Behavior is undefined if `out` is
/// not a valid output iterator.
template <class t_OUT>
inline
t_OUT vformat_to(t_OUT out, bsl::string_view fmtStr, format_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the output
/// iterator given by the specified `out` parameter.  In the event of an error
/// the exception `format_error` is thrown.  Behavior is undefined if `out` is
/// not a valid output iterator.
template <class t_OUT>
inline
t_OUT vformat_to(t_OUT out, bsl::wstring_view fmtStr, wformat_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.
inline
void vformat_to(bsl::string *out, bsl::string_view fmtStr, format_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.
inline
void vformat_to(bsl::wstring      *out,
                bsl::wstring_view  fmtStr,
                wformat_args       args);

/// Format the specified `args` according to the specification given by the
/// specified `fmt` and return the result.  In the event of an error the
/// exception `format_error` is thrown.
inline
bsl::string vformat(bsl::string_view fmt, format_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result.  In the event of an error the
/// exception `format_error` is thrown.
inline
bsl::wstring vformat(bsl::wstring_view fmtStr, wformat_args args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `allocator` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
inline
bsl::string vformat(bsl::allocator<char> alloc,
                    bsl::string_view     fmtStr,
                    format_args          args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `allocator` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
inline
bsl::wstring vformat(bsl::allocator<wchar_t> alloc,
                     bsl::wstring_view       fmtStr,
                     wformat_args            args);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the output
/// iterator given by the specified `out` parameter.  In the event of an error
/// the exception `format_error` is thrown.  Behavior is undefined if `out`
/// does not point to a valid output parameter.
template <class t_OUT, class... t_ARGS>
typename bsl::enable_if<
    !bsl::is_same<typename bsl::decay<t_OUT>::type, bsl::string *>::value,
    t_OUT>::type
inline
format_to(t_OUT                          out,
          BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
          const t_ARGS&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the output
/// iterator given by the specified `out` parameter.  In the event of an error
/// the exception `format_error` is thrown.  Behavior is undefined if `out`
/// does not point to a valid output parameter.
template <class t_OUT, class... t_ARGS>
typename bsl::enable_if<
    !bsl::is_same<typename bsl::decay<t_OUT>::type, bsl::wstring *>::value,
    t_OUT>::type
inline
format_to(t_OUT                           out,
          BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
          const t_ARGS&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.
template <class... t_ARGS>
inline
void format_to(bsl::string                    *out,
               BSLFMT_FORMAT_STRING_PARAMETER  fmtStr,
               const t_ARGS&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write the result of this operation into the string
/// addressed by the specified `out` parameter.  In the event of an error the
/// exception `format_error` is thrown.  Behavior is undefined if `out` does
/// not point to a valid `bsl::string` object.
template <class... t_ARGS>
inline
void format_to(bsl::wstring                    *out,
               BSLFMT_FORMAT_WSTRING_PARAMETER  fmtStr,
               const t_ARGS&...                 args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result as a `bsl::string`.  In the event
/// of an error the exception `format_error` is thrown.
template <class... t_ARGS>
inline
bsl::string format(BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
                   const t_ARGS&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr` and return the result as a `bsl::string`.  In the event
/// of an error the exception `format_error` is thrown.
template <class... t_ARGS>
inline
bsl::wstring format(BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                    const t_ARGS&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `alloc` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
template <class... t_ARGS>
inline
bsl::string format(bsl::allocator<char>           alloc,
                   BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
                   const t_ARGS&...               args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, using the specified `alloc` to supply memory (if
/// required), and return the result.  In the event of an error the exception
/// `format_error` is thrown.
template <class... t_ARGS>
inline
bsl::wstring format(bsl::allocator<wchar_t>         alloc,
                    BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                    const t_ARGS&...                args);

/// Calculate the length of the resulting string if the specified `args` were
/// to be formatted according to the specification given by the specified
/// `fmtStr`, and return the result.
template <class... t_ARGS>
inline
std::size_t formatted_size(BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
                           const t_ARGS&...               args);

/// Calculate the length of the resulting string if the specified `args` were
/// to be formatted according to the specification given by the specified
/// `fmtStr`, and return the result.
template <class... t_ARGS>
inline
std::size_t formatted_size(BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                           const t_ARGS&...                args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write at most the specified `maxNumChars`
/// characters of the result of this operation into the output iterator
/// referenced by the specified `out` parameter.  Return a `format_to_n_result`
/// object whose `size` member holds the number of characters that would have
/// been written to `out` were such writing not truncated, and whose `out`
/// member holds an iterator one past the end of the output range.  In the
/// event of an error the exception `format_error` is thrown.  Behavior is
/// undefined if `out` does not point to a valid output iterator.
template <class t_OUT, class... t_ARGS>
inline
format_to_n_result<t_OUT>
format_to_n(t_OUT                                                 out,
            typename bsl::iterator_traits<t_OUT>::difference_type maxNumChars,
            BSLFMT_FORMAT_STRING_PARAMETER                        fmtStr,
            const t_ARGS&...                                      args);

/// Format the specified `args` according to the specification given by the
/// specified `fmtStr`, and write at most the specified `maxNumChars`
/// characters of the result of this operation into the output iterator
/// referenced by the specified `out` parameter.  Return a `format_to_n_result`
/// object whose `size` member holds the number of characters that would have
/// been written to `out` were such writing not truncated, and whose `out`
/// member holds an iterator one past the end of the output range.  In the
/// event of an error the exception `format_error` is thrown.  Behavior is
/// undefined if `out` does not point to a valid output iterator.
template <class t_OUT, class... t_ARGS>
inline
format_to_n_result<t_OUT>
format_to_n(t_OUT                                                 out,
            typename bsl::iterator_traits<t_OUT>::difference_type maxNumChars,
            BSLFMT_FORMAT_WSTRING_PARAMETER                       fmtStr,
            const t_ARGS&...                                      args);

#endif


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // -----------------------------------
                 // class Format_Imp_TruncatingIterator
                 // -----------------------------------

// CREATORS
template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>::
    Format_Imp_TruncatingIterator(t_ITERATOR iterator, t_DIFF_TYPE limit)
: d_iterator(iterator)
, d_limit(limit)
, d_count(0)
{
}

// MANIPULATORS
template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>&
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>::
operator*()
{
    return *this;
}

template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
void Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>::
operator=(t_VALUE_TYPE x)
{
    if (d_count++ < d_limit) {
        *d_iterator = x;
        // We deliberately use prefix not postfix increment, as the postfix
        // increment operator returns by value which could cause issues with
        // counting or other stateful iterators.
        ++d_iterator;
    }
}

template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>&
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>::
operator++()
{
    return *this;
}

// ACCESSORS
template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
t_DIFF_TYPE
Format_Imp_TruncatingIterator<t_ITERATOR, t_VALUE_TYPE, t_DIFF_TYPE>::count()
    const
{
    return d_count;
}

template <class t_ITERATOR, class t_VALUE_TYPE, class t_DIFF_TYPE>
t_ITERATOR Format_Imp_TruncatingIterator<t_ITERATOR,
                                         t_VALUE_TYPE,
                                         t_DIFF_TYPE>::underlying() const
{
    return d_iterator;
}

                       // ------------------------
                       // class Format_Imp_Visitor
                       // ------------------------

// CREATORS

template <class t_OUT, class t_CHAR>
Format_Imp_Visitor<t_OUT, t_CHAR>::Format_Imp_Visitor(
                                       basic_format_parse_context<t_CHAR>&  pc,
                                       basic_format_context<t_OUT, t_CHAR>& fc)
: d_parseContext_p(&pc)
, d_formatContext_p(&fc)
{
}

// MANIPULATORS

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(bsl::monostate) const
{
    BSLS_ASSERT_OPT(false);
    BSLS_THROW(
           format_error("This call should be impossible - arg uninitialized"));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(bool value) const
{
    bsl::formatter<bool, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(t_CHAR value) const
{
    bsl::formatter<t_CHAR, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(unsigned value) const
{
    bsl::formatter<unsigned, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(long long value) const
{
    bsl::formatter<long long, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(
                                                unsigned long long value) const
{
    bsl::formatter<unsigned long long, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(float value) const
{
    bsl::formatter<float, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(double value) const
{
    bsl::formatter<double, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(long double value) const
{
    bsl::formatter<long double, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(const t_CHAR *value) const
{
    bsl::formatter<const t_CHAR *, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(const void *value) const
{
    bsl::formatter<const void *, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(int value) const
{
    bsl::formatter<int, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(
                                    bsl::basic_string_view<t_CHAR> value) const
{
    bsl::formatter<bsl::basic_string_view<t_CHAR>, t_CHAR> f;
    d_parseContext_p->advance_to(f.parse(*d_parseContext_p));
    d_formatContext_p->advance_to(bsl::as_const(f).format(value,
                                                          *d_formatContext_p));
}

template <class t_OUT, class t_CHAR>
void Format_Imp_Visitor<t_OUT, t_CHAR>::operator()(const handle& h) const
{
    h.format(*d_parseContext_p, *d_formatContext_p);
}

                        // --------------------------
                        // class Format_Imp_Processor
                        // --------------------------

template <class t_CHAR>
template <class t_OUT>
t_OUT Format_Imp_Processor<t_CHAR>::processImp(
         t_OUT&                                                         out,
         bsl::basic_string_view<t_CHAR>                                 fmtStr,
         const basic_format_args<basic_format_context<t_OUT, t_CHAR> >& args)
    // The actual meat of the implementation.
{
    const size_t argSize = Format_ArgsUtil::formatArgsSize(args);

    basic_format_parse_context<t_CHAR>  pc(fmtStr, argSize);
    basic_format_context<t_OUT, t_CHAR> fc(
                                  Format_ContextFactory::construct(out, args));
    Format_Imp_Visitor<t_OUT, t_CHAR> visitor(pc, fc);

    typename bsl::basic_string_view<t_CHAR>::iterator it = pc.begin();

    while (it != pc.end()) {
        if (*it == '{') {
            ++it;
            if (it == pc.end()) {
                BSLS_THROW(format_error("unmatched {"));
            }
            else if (*it == '{') {
                // literal {
                ++it;
                out  = fc.out();
                *out = '{';
                ++out;  // prefer prefix increment
                fc.advance_to(out);
                continue;                                           // CONTINUE
            }
            size_t id = -1;
            if (*it >= '0' && *it <= '9') {
                // numeric ID
                id = 0;
                while (it != pc.end() && *it >= '0' && *it <= '9') {
                    id = 10 * id + (*it - '0');
                    ++it;
                    if (id >= argSize) {
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

            // Separator between arg Id and format specification

            if (*it == ':') {
                ++it;
            }
            else if (*it != '}') {
                BSLS_THROW(format_error("Separator ':' missing"));
            }

            pc.advance_to(it);
            visit_format_arg(visitor, args.get(id));
            it = pc.begin();
            if (it != pc.end()) {
                if (*it != '}') {
                    BSLS_THROW(format_error("parsing terminated before }"));
                }
                // advance past the terminating }
                ++it;
            }
            out = fc.out();
        }
        else if (*it == '}') {
            // must be escaped
            ++it;
            if (it == pc.end() || *it != '}') {
                BSLS_THROW(format_error("} must be escaped"));
            }
            ++it;
            out  = fc.out();
            *out = '}';
            ++out;  // prefer prefix increment
            fc.advance_to(out);
        }
        else {
            // just copy it
            out  = fc.out();
            *out = *it;
            ++out;
            ++it;
            fc.advance_to(out);
        }
    }
    return fc.out();
}

template <class t_CHAR>
Format_ContextOutputIteratorRef<t_CHAR> Format_Imp_Processor<t_CHAR>::process(
              Format_ContextOutputIteratorRef<t_CHAR> out,
              bsl::basic_string_view<t_CHAR>          fmtStr,
              const basic_format_args<
                  basic_format_context<Format_ContextOutputIteratorRef<t_CHAR>,
                                       t_CHAR> >& args)
{
    processImp(out, fmtStr, args);
    return out;
}

template <class t_CHAR>
template <class t_OUT, class t_CONTEXT>
t_OUT Format_Imp_Processor<t_CHAR>::process(
                                    t_OUT                               out,
                                    bsl::basic_string_view<t_CHAR>      fmtStr,
                                    const basic_format_args<t_CONTEXT>& args)
{
    Format_ContextOutputIteratorImpl<t_CHAR, t_OUT> wrappedOut(out);
    Format_ContextOutputIteratorRef<t_CHAR>         wrappedOutRef(&wrappedOut);
    processImp(wrappedOutRef, fmtStr, args);
    return out;
}

// FREE FUNCTIONS

template <class t_OUT>
inline
t_OUT vformat_to(t_OUT out, bsl::string_view fmtStr, format_args args)
{
    return Format_Imp_Processor<char>::process(out, fmtStr, args);
}

template <class t_OUT>
inline
t_OUT vformat_to(t_OUT out, bsl::wstring_view fmtStr, wformat_args args)
{
    return Format_Imp_Processor<wchar_t>::process(out, fmtStr, args);
}

inline
void vformat_to(bsl::string *out, bsl::string_view fmtStr, format_args args)
{
    vformat_to(bsl::back_inserter(*out), fmtStr, args);
}

inline
void vformat_to(bsl::wstring *out, bsl::wstring_view fmtStr, wformat_args args)
{
    vformat_to(bsl::back_inserter(*out), fmtStr, args);
}

inline
bsl::string vformat(bsl::string_view fmtStr, format_args args)
{
    bsl::string result;
    vformat_to(&result, fmtStr, args);
    return result;
}

inline
bsl::wstring vformat(bsl::wstring_view fmtStr, wformat_args args)
{
    bsl::wstring result;
    vformat_to(&result, fmtStr, args);
    return result;
}

inline
bsl::string vformat(bsl::allocator<char> alloc,
                    bsl::string_view     fmtStr,
                    format_args          args)
{
    bsl::string result(alloc);
    vformat_to(&result, fmtStr, args);
    return bsl::string(result, alloc);
}

inline
bsl::wstring vformat(bsl::allocator<wchar_t> alloc,
                     bsl::wstring_view       fmtStr,
                     wformat_args            args)
{
    bsl::wstring result(alloc);
    vformat_to(&result, fmtStr, args);
    return bsl::wstring(result, alloc);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_OUT, class... t_ARGS>
typename bsl::enable_if<
    !bsl::is_same<typename bsl::decay<t_OUT>::type, bsl::string *>::value,
    t_OUT>::type
format_to(t_OUT                          out,
          BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
          const t_ARGS&...               args)
{
    return vformat_to(out, fmtStr.get(), bslfmt::make_format_args(args...));
}

template <class t_OUT, class... t_ARGS>
typename bsl::enable_if<
    !bsl::is_same<typename bsl::decay<t_OUT>::type, bsl::wstring *>::value,
    t_OUT>::type
format_to(t_OUT                           out,
          BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
          const t_ARGS&...                args)
{
    return vformat_to(out, fmtStr.get(), bslfmt::make_wformat_args(args...));
}

template <class... t_ARGS>
void format_to(bsl::string                    *out,
               BSLFMT_FORMAT_STRING_PARAMETER  fmtStr,
               const t_ARGS&...                args)
{
    vformat_to(bsl::back_inserter(*out),
               fmtStr.get(),
               bslfmt::make_format_args(args...));
}

template <class... t_ARGS>
void format_to(bsl::wstring                    *out,
               BSLFMT_FORMAT_WSTRING_PARAMETER  fmtStr,
               const t_ARGS&...                 args)
{
    vformat_to(bsl::back_inserter(*out),
               fmtStr.get(),
               bslfmt::make_wformat_args(args...));
}

template <class... t_ARGS>
bsl::string format(BSLFMT_FORMAT_STRING_PARAMETER fmtStr, const t_ARGS&...args)
{
    bsl::string result;
    vformat_to(&result, fmtStr.get(), bslfmt::make_format_args(args...));
    return result;
}

template <class... t_ARGS>
bsl::wstring format(BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                    const t_ARGS&...                args)
{
    bsl::wstring result;
    vformat_to(&result, fmtStr.get(), bslfmt::make_wformat_args(args...));
    return result;
}

template <class... t_ARGS>
bsl::string format(bsl::allocator<char>           alloc,
                   BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
                   const t_ARGS&...               args)
{
    bsl::string result(alloc);
    vformat_to(&result, fmtStr.get(), bslfmt::make_format_args(args...));
    return bsl::string(result, alloc);
}

template <class... t_ARGS>
bsl::wstring format(bsl::allocator<wchar_t>         alloc,
                    BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                    const t_ARGS&...                args)
{
    bsl::wstring result(alloc);
    vformat_to(&result, fmtStr.get(), bslfmt::make_wformat_args(args...));
    return bsl::wstring(result, alloc);
}

template <class... t_ARGS>
std::size_t formatted_size(BSLFMT_FORMAT_STRING_PARAMETER fmtStr,
                           const t_ARGS&...               args)
{
    typedef Format_Imp_TruncatingIterator<char *, char, ptrdiff_t>
        TruncatingIterator;

    TruncatingIterator it(0, 0);
    TruncatingIterator end = format_to(it, fmtStr, args...);
    return end.count();
}

template <class... t_ARGS>
std::size_t formatted_size(BSLFMT_FORMAT_WSTRING_PARAMETER fmtStr,
                           const t_ARGS&...                args)
{
    typedef Format_Imp_TruncatingIterator<wchar_t *, wchar_t, ptrdiff_t>
        TruncatingIterator;

    TruncatingIterator it(0, 0);
    TruncatingIterator end = format_to(it, fmtStr, args...);
    return end.count();
}


template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT>
format_to_n(t_OUT                                                 out,
            typename bsl::iterator_traits<t_OUT>::difference_type maxNumChars,
            BSLFMT_FORMAT_STRING_PARAMETER                        fmtStr,
            const t_ARGS&...                                      args)
{
    if (maxNumChars < 0)
        maxNumChars = 0;

    typedef Format_Imp_TruncatingIterator<
        t_OUT,
        typename bsl::iterator_traits<t_OUT>::value_type,
        typename bsl::iterator_traits<t_OUT>::difference_type>
        TruncatingIterator;

    TruncatingIterator it(out, maxNumChars);

    TruncatingIterator end = format_to(it, fmtStr, args...);

    format_to_n_result<t_OUT> result;
    result.out  = end.underlying();
    result.size = end.count();
    return result;
}

template <class t_OUT, class... t_ARGS>
format_to_n_result<t_OUT>
format_to_n(t_OUT                                                 out,
            typename bsl::iterator_traits<t_OUT>::difference_type maxNumChars,
            BSLFMT_FORMAT_WSTRING_PARAMETER                       fmtStr,
            const t_ARGS&...                                      args)
{
    if (maxNumChars < 0)
        maxNumChars = 0;

    typedef Format_Imp_TruncatingIterator<
        t_OUT,
        typename bsl::iterator_traits<t_OUT>::value_type,
        typename bsl::iterator_traits<t_OUT>::difference_type>
        TruncatingIterator;

    TruncatingIterator it(out, maxNumChars);

    TruncatingIterator end = format_to(it, fmtStr, args...);

    format_to_n_result<t_OUT> result;
    result.out  = end.underlying();
    result.size = end.count();
    return result;
}
#endif


}  // close package namespace
}  // close enterprise namespace

#undef BSLFMT_FORMAT_STRING_PARAMETER
#undef BSLFMT_FORMAT_WSTRING_PARAMETER

#endif // End C++11 code

#endif  // INCLUDED_BSLFMT_FORMAT_IMP

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
