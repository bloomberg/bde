// bslfmt_formatterstring.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSTRING
#define INCLUDED_BSLFMT_FORMATTERSTRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a string formatter for use by bsl::format
//
//@CLASSES:
//  bsl::formatter<t_CHAR *, t_CHAR>: formatter specialization for `t_CHAR *`
//  bsl::formatter<const t_CHAR *, t_CHAR>: specialization for `const t_CHAR *`
//  bsl::formatter<t_CHAR[N], t_CHAR> : specialization for character arrays
//  bsl::formatter<std::basic_string>: formatter for `std::basic_string`
//  bsl::formatter<bsl::basic_string>: formatter for `bsl::basic_string`
//  bsl::formatter<std::basic_string_view>: for `std::basic_string_view`
//  bsl::formatter<bsl::basic_string_view>: for `bsl::basic_string_view`
//  bsl::formatter<bslstl:::StringRefImp<t_CHAR> >: for `bslstl:::StringRef`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations for the
// `bsl::formatter` type covering the case for string formatting.  Those types
// meet the requirements as specified in [formatter.requirements] and support
// all functionality supported by the C++20 `std::formatter` string type
// specializations with the following exception:
// * No support for non-`C` locales.
//
// This header is not intended to be included directly.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a basic string
/// - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily artificial.
//
// Suppose we want to test this formatter's ability to format a substring with
// padding and minimum width.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<5.3s", 1);
//
//  bsl::formatter<const char *, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  const char *value = "abcdefghij";
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("abc**" == mfc.finalString());
// ```
//

#include <bslscm_version.h>

#include <bslfmt_debugformatutil.h>
#include <bslfmt_displaywidthutil.h>
#include <bslfmt_escapeutil.h>
#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_padutil.h>
#include <bslfmt_standardformatspecification.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringref.h>
#include <bslstl_stringview.h>

#include <cstddef>    // `std::size_t`
#include <limits>     // for 'std::numeric_limits'

namespace BloombergLP {
namespace bslfmt {

                        // ==========================
                        // struct FormatterString_Imp
                        // ==========================

/// This component-private class provides the implementations for parsing
/// string formatting specifications and for formatting strings according to
/// that specification.
template <class t_CHAR, class t_CHAR_TRAITS = bsl::char_traits<t_CHAR> >
struct FormatterString_Imp {
  private:
    // PRIVATE TYPES
    typedef StandardFormatSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;  // parsed specification.

    // PRIVATE CLASS METHODS

    /// Write the specified `text`, laid out according to the specified `spec`
    /// -- honoring its width, precision, fill and alignment -- to the iterator
    /// accessed by calling the `out()` method on the specified
    /// `formatContext`.  Return an end iterator of the output range.  Throw an
    /// exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    static typename t_FORMAT_CONTEXT::iterator outputAsString(
     const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& text,
     const StandardFormatSpecification<t_CHAR>&           spec,
     t_FORMAT_CONTEXT&                                    formatContext);

  protected:
    // PROTECTED ACCESSORS

    /// Format the specified `value` according to the specification stored as a
    /// result of a previous call to the `parse` method, and write the result
    /// to the iterator accessed by calling the `out()` method on the specified
    /// `formatContext` parameter.  Return an end iterator of the output range.
    /// Throw an exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatImpl(
     const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& value,
     t_FORMAT_CONTEXT&                                    formatContext) const;

  public:
    // MANIPULATORS

    /// Parse and validate the specification string stored in the specified
    /// `parseContext`.  Return an end iterator of the parsed range.  Throw an
    /// exception of type `bsl::format_error` in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);

    /// Render subsequently formatted values in the escaped ("debug")
    /// presentation, as though `?` had been specified.  The behavior is
    /// undefined unless `parse` has been called on this object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void set_debug_format();
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // ==================================
                        // struct formatter<t_CHAR *, t_CHAR>
                        // ==================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `char *` and
/// `wchar_t *` types.
template <class t_CHAR>
struct formatter<t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext`.  Return an end
    /// iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                     // ========================================
                     // struct formatter<const t_CHAR *, t_CHAR>
                     // ========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `const char *` and
/// `const wchar_t *` types.
template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                     // ===================================
                     // struct formatter<t_CHAR[N], t_CHAR>
                     // ===================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `char[]` and
/// `wchar_t[]` types.
template <class t_CHAR, std::size_t t_SIZE>
struct formatter<t_CHAR[t_SIZE], t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                // ===========================================
                // struct formatter<std::basic_string, t_CHAR>
                // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `std::string` and
/// `std::wstring` types.
template <class t_CHAR, class t_CHAR_TRAITS, class t_ALLOCATOR>
struct formatter<std::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator
    format(const std::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>& value,
           t_FORMAT_CONTEXT& formatContext) const;
};

              // ===========================================
              // struct formatter<bsl::basic_string, t_CHAR>
              // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bsl::string` and
/// `bsl::wstring` types.
template <class t_CHAR, class t_CHAR_TRAITS, class t_ALLOCATOR>
struct formatter<bsl::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // We should propagate this formatter to the `std` namespace, so do *not*
    // define the `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20` trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator
    format(const bsl::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>& value,
           t_FORMAT_CONTEXT& formatContext) const;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ================================================
              // struct formatter<std::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `std::string_view`
/// and `std::wstring_view` types.
template <class t_CHAR, class t_CHAR_TRAITS>
struct formatter<std::basic_string_view<t_CHAR, t_CHAR_TRAITS>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
            std::basic_string_view<t_CHAR, t_CHAR_TRAITS> value,
            t_FORMAT_CONTEXT&                             formatContext) const;
};
#endif

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ================================================
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bsl::string_view` and
/// `bsl::wstring_view` types.
template <class t_CHAR, class t_CHAR_TRAITS>
struct formatter<bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // If we do not alias `string_view` then this formatter must be promoted to
    // the `std` namespace, so do *not* define the
    // `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20` trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
            bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> value,
            t_FORMAT_CONTEXT&                             formatContext) const;
};

#endif

                // ===========================================
                // struct formatter<bslstl::StringRef, t_CHAR>
                // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bslstl::StringRef`
/// and `bslstl::StringRefWide` types.
template <class t_CHAR>
struct formatter<BloombergLP::bslstl::StringRefImp<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                          formatter,
                          BloombergLP::bslfmt::SupportsDebugFormat);

    // This formatter specialization must be promoted to the `std` namespace so
    //  do *not* define the `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20`
    //  trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the

    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                BloombergLP::bslstl::StringRefImp<t_CHAR> value,
                t_FORMAT_CONTEXT&                         formatContext) const;
};

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                        // --------------------------
                        // struct FormatterString_Imp
                        // --------------------------

// PRIVATE CLASS METHODS
template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::outputAsString(
      const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& text,
      const StandardFormatSpecification<t_CHAR>&           spec,
      t_FORMAT_CONTEXT&                                    formatContext)
{
    typedef bsl::basic_string_view<t_CHAR> StringView;
    typedef PadUtil<t_CHAR>                PadUtil;

    Specification finalSpec(spec);

    finalSpec.postprocess(formatContext);

    typedef FormatterSpecificationNumericValue NumericValue;

    NumericValue finalWidth(finalSpec.postprocessedWidth());

    NumericValue finalPrecision(finalSpec.postprocessedPrecision());

    int maxDisplayWidth = 0;
    switch (finalPrecision.category()) {
      case NumericValue::e_DEFAULT: {
        maxDisplayWidth = std::numeric_limits<int>::max();
      } break;
      case NumericValue::e_VALUE: {
        maxDisplayWidth = finalPrecision.value();
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));  // THROW
      }
    }

    // Only measure the string if there is a possibility of truncation or
    // padding.
    if ((maxDisplayWidth >= static_cast<int>(text.size()) * 2) &&
                          (finalWidth.category() == NumericValue::e_DEFAULT)) {
        return bsl::copy(text.begin(), text.end(), formatContext.out());
                                                                      // RETURN
    }

    BSLS_ASSERT(NumericValue::e_DEFAULT == finalWidth.category() ||
                NumericValue::e_VALUE   == finalWidth.category());

    const StringView content(text.data(), text.size());
    std::size_t      numCharacters = 0;
    int              displayWidth  = 0;

    DisplayWidthUtil::findWidthLimitedPrefix(&numCharacters,
                                             &displayWidth,
                                             content,
                                             maxDisplayWidth);

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.

    const StringView pad(finalSpec.filler(), finalSpec.numFillerCharacters());

    return PadUtil::writePadded(formatContext.out(),
                                content.substr(0, numCharacters),
                                displayWidth,
                                finalWidth,
                                spec.alignment(),
                                pad,
                                Specification::e_ALIGN_LEFT);
}

// PROTECTED ACCESSORS
template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::formatImpl(
      const bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>& value,
      t_FORMAT_CONTEXT&                                    formatContext) const
{
    // If the escaped ("debug") presentation was requested, the escaping
    // happens first and everything that follows -- precision-driven
    // truncation and width-driven padding alike -- applies to the escaped
    // text, quotes included.
    //
    // Note that the escaped text is always well-formed, because every
    // ill-formed code unit of the input has become an ASCII `\x{...}`
    // sequence.  The grapheme-cluster analysis downstream can therefore no
    // longer reject the input as an invalid Unicode stream.

    if (Specification::e_STRING_ESCAPED == d_spec.formatType()) {
        bsl::basic_string<t_CHAR, t_CHAR_TRAITS> escaped;

        EscapeUtil<t_CHAR, t_CHAR_TRAITS>::escapeString(
                                                   bsl::back_inserter(escaped),
                                                   value);

        return outputAsString(escaped, d_spec, formatContext);        // RETURN
    }

    return outputAsString(value, d_spec, formatContext);
}


// MANIPULATORS
template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::parse(
                                                 t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext, Specification::e_CATEGORY_STRING);

    if (d_spec.sign() != Specification::e_SIGN_DEFAULT)
        BSLS_THROW(bsl::format_error(
                      "Formatting sign specifier not valid for string types"));

    if (d_spec.alternativeFlag())
        BSLS_THROW(bsl::format_error(
                         "Formatting # specifier not valid for string types"));

    if (d_spec.zeroPaddingFlag())
        BSLS_THROW(bsl::format_error(
                         "Formatting 0 specifier not valid for string types"));

    if (d_spec.localeSpecificFlag())
        BSLS_THROW(bsl::format_error("Formatting L specifier not supported"));

    return parseContext.begin();
}

template <class t_CHAR, class t_CHAR_TRAITS>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::set_debug_format()
{
    d_spec.setEscapedFormatType();
}

}  // close package namespace
}  // close enterprise namespace


namespace bsl {

                     // ----------------------------------
                     // struct formatter<t_CHAR *, t_CHAR>
                     // ----------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR *, t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                  // ----------------------------------------
                  // struct formatter<const t_CHAR *, t_CHAR>
                  // ----------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<const t_CHAR *, t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                    // -----------------------------------
                    // struct formatter<t_CHAR[N], t_CHAR>
                    // -----------------------------------

// ACCESSORS
template <class t_CHAR, std::size_t t_SIZE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR[t_SIZE], t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value, t_SIZE);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                // -------------------------------------------
                // struct formatter<std::basic_string, t_CHAR>
                // -------------------------------------------

// ACCESSORS
template <class t_CHAR, class t_CHAR_TRAITS, class t_ALLOCATOR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<std::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>, t_CHAR>::
    format(const std::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>& value,
           t_FORMAT_CONTEXT& formatContext) const
{
    bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::
        formatImpl(view, formatContext);
}

                // -------------------------------------------
                // struct formatter<bsl::basic_string, t_CHAR>
                // -------------------------------------------

// ACCESSORS
template <class t_CHAR, class t_CHAR_TRAITS, class t_ALLOCATOR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>, t_CHAR>::
    format(const bsl::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR>& value,
           t_FORMAT_CONTEXT& formatContext) const
{
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::
        formatImpl(value, formatContext);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
}  // close namespace bsl

template <class t_CHAR, class t_CHAR_TRAITS, class t_ALLOCATOR>
inline
constexpr std::range_format
    std::format_kind<bsl::basic_string<t_CHAR, t_CHAR_TRAITS, t_ALLOCATOR> > =
                                                   std::range_format::disabled;

namespace bsl {
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ------------------------------------------------
              // struct formatter<std::basic_string_view, t_CHAR>
              // ------------------------------------------------

// ACCESSORS
template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<
    std::basic_string_view<t_CHAR, t_CHAR_TRAITS>,
    t_CHAR>::format(std::basic_string_view<t_CHAR, t_CHAR_TRAITS> value,
                            t_FORMAT_CONTEXT&              formatContext) const
{
    bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::
        formatImpl(view, formatContext);
}

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ------------------------------------------------
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ------------------------------------------------

// ACCESSORS
template <class t_CHAR, class t_CHAR_TRAITS>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS>, t_CHAR>::format(
             bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> value,
             t_FORMAT_CONTEXT&                             formatContext) const
{
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR, t_CHAR_TRAITS>::
        formatImpl(value, formatContext);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
}  // close namespace bsl

template <class t_CHAR, class t_CHAR_TRAITS>
inline
constexpr std::range_format
    std::format_kind<bsl::basic_string_view<t_CHAR, t_CHAR_TRAITS> > =
                                                   std::range_format::disabled;

namespace bsl {
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

              // ----------------------------------------------
              // struct formatter<bslstl::StringRefImp, t_CHAR>
              // ----------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<BloombergLP::bslstl::StringRefImp<t_CHAR>, t_CHAR>::format(
                 BloombergLP::bslstl::StringRefImp<t_CHAR> value,
                 t_FORMAT_CONTEXT&                         formatContext) const
{
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                value,
                                                                formatContext);
}

}  // close namespace bsl

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
template <class t_CHAR>
inline
constexpr std::range_format
    std::format_kind<BloombergLP::bslstl::StringRefImp<t_CHAR> > =
                                                   std::range_format::disabled;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

#endif  // INCLUDED_BSLFMT_FORMATTERSTRING

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
