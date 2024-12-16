// bslfmt_formatterstring.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSTRING
#define INCLUDED_BSLFMT_FORMATTERSTRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a string formatter for use by bsl::format
//
//@CLASSES:
//  bsl::formatter: partial specialization for string types.
//
//@DESCRIPTION: This component provides a partial specialization for the
// `bsl::formatter` type covering the case for string formatting. This type
// meets the requirements as specified in [formatter.requirements] and supports
// all functionality supported by the C++20 `std::formatter` string type
// specializations with the following exceptions:
// * No support for escaped strings.
// * No support for non-`C` locales.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a basic string
/// - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
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

#include <bslalg_numericformatterutil.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterspecificationstandard.h>
#include <bslfmt_formatterunicodeutils.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'
#include <limits>     // for 'std::numeric_limits'

#include <stdio.h>    // for 'snprintf'



                        // ==========================
                        // struct FormatterString_Imp
                        // ==========================

namespace BloombergLP {
namespace bslfmt {

/// This component-private class provides the implementations for parsing
/// string formatting specifications and for formatting strings according to
/// that specification.
template <class t_CHAR>
struct FormatterString_Imp {
  private:
    // PRIVATE CLASS TYPES
    typedef FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;    // Parsed specification.

    // PRIVATE CLASS METHODS

    /// Given the specified `inputString` and `maxTotalDisplayWidth` find the
    /// maximal initial substring of `inputString` whose unicode display width
    /// (calculated using the rules in [format.string.std] does not exceed
    /// `maxTotalDisplayWidth`. Update the specified `charactersUsed` with the
    /// number of characters of type `t_CHAR` in the maximal substring and
    /// update the specified `widthUsed` with the unicode display width of the
    /// maximal substring. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    static void findPrecisionLimitedString(
                         size_t                         *charactersUsed,
                         int                            *widthUsed,
                         bsl::basic_string_view<t_CHAR>  inputString,
                         int                             maxTotalDisplayWidth);
  protected:
    // PROTECTED ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatImpl(
                                      bsl::basic_string_view<t_CHAR> value,
                                      t_FORMAT_CONTEXT&              fc) const;

  public:
    // MANIPULATORS

    /// Parse and validate the specification string stored in the iterator
    /// accessed via the `begin()` method of the context passed via the
    /// specified `pc` parameter. Where nested parameters are encounted in the
    /// specification string then the `next_arg_id` and `check_arg_id` are
    /// called on `fc` as specified in the C++ Standard. Returns an end
    /// iterator of the parsed range. Throws an execption of type
    /// `bsl::format_error` in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                          t_PARSE_CONTEXT& pc);
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {

                        // ==================================
                        // struct formatter<t_CHAR *, t_CHAR>
                        // ==================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `char *` and
/// `wchar_t *` types.
template <class t_CHAR>
struct formatter<t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const t_CHAR      *value,
                                               t_FORMAT_CONTEXT&  fc) const;
};

                     // ========================================
                     // struct formatter<const t_CHAR *, t_CHAR>
                     // ========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `const char *` and
/// `const wchar_t *` types.
template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const t_CHAR      *value,
                                               t_FORMAT_CONTEXT&  fc) const;
};

                     // ===================================
                     // struct formatter<t_CHAR[N], t_CHAR>
                     // ===================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `char[]` and
/// `wchar_t[]` types.
template <class t_CHAR, size_t t_SIZE>
struct formatter<t_CHAR[t_SIZE], t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const t_CHAR      *value,
                                               t_FORMAT_CONTEXT&  fc) const;
};

                // ===========================================
                // struct formatter<std::basic_string, t_CHAR>
                // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `std::string` and
/// `std::wstring` types.
template <class t_CHAR>
struct formatter<std::basic_string<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std` string
    // view, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                    const std::basic_string<t_CHAR>& value,
                                    t_FORMAT_CONTEXT&                fc) const;
};

              // ===========================================
              // struct formatter<bsl::basic_string, t_CHAR>
              // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `bsl::string` and
/// `bsl::wstring` types.
template <class t_CHAR>
struct formatter<bsl::basic_string<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // We should alias its formatter from the `std` namespace, so DO NOT DEFINE
    // the trait.
    // BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                    const bsl::basic_string<t_CHAR>& value,
                                    t_FORMAT_CONTEXT&                fc) const;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ================================================
              // struct formatter<std::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `std::string_view` and
/// `std::wstring_view` types.
template <class t_CHAR>
struct formatter<std::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std`
    // string view, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                      std::basic_string_view<t_CHAR> value,
                                      t_FORMAT_CONTEXT&              fc) const;
};
#endif

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ================================================
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type. It implementes formatting for `bsl::string_view` and
/// `bsl::wstring_view` types.
template <class t_CHAR>
struct formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // If we do not alias string view then we should alias its formatter from
    // the `std` namespace, so DO NOT DEFINE the trait
    // BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `fc` parameter. Returns an end iterator
    /// of the output range. Throws an execption of type `bsl::format_error` in
    /// the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                      bsl::basic_string_view<t_CHAR> value,
                                      t_FORMAT_CONTEXT&              fc) const;
};

#endif

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================


                        // ==========================
                        // struct FormatterString_Imp
                        // ==========================


namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR>
void FormatterString_Imp<t_CHAR>::findPrecisionLimitedString(
                          size_t                         *charactersUsed,
                          int                            *widthUsed,
                          bsl::basic_string_view<t_CHAR>  inputString,
                          int                             maxTotalDisplayWidth)
{
    *widthUsed      = 0;
    *charactersUsed = 0;

    const t_CHAR *current   = inputString.data();
    size_t        bytesLeft = inputString.size() * sizeof(t_CHAR);

    while (bytesLeft > 0) {
        Formatter_UnicodeUtils::GraphemeClusterExtractionResult gcresult;

        switch (sizeof(t_CHAR)) {
          case 1: {
            gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                                Formatter_UnicodeUtils::e_UTF8,
                                                current,
                                                bytesLeft);
          } break;
          case 2: {
            gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                               Formatter_UnicodeUtils::e_UTF16,
                                               current,
                                               bytesLeft);
          } break;
          case 4: {
            gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                               Formatter_UnicodeUtils::e_UTF32,
                                               current,
                                               bytesLeft);
          } break;
          default: {
            BSLS_THROW(bsl::format_error("Invalid character width"));
          }
        }
        if (!gcresult.isValid) {
            BSLS_THROW(bsl::format_error("Invalid unicode stream"));
        }

        if (*widthUsed + gcresult.firstCodePointWidth > maxTotalDisplayWidth)
            break;

        bytesLeft -= gcresult.numSourceBytes;
        current += gcresult.numSourceBytes / sizeof(t_CHAR);

        *charactersUsed += gcresult.numSourceBytes / sizeof(t_CHAR);

        *widthUsed += gcresult.firstCodePointWidth;
    }
}

// PROTECTED ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator FormatterString_Imp<t_CHAR>::formatImpl(
                                       bsl::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
{
    FSS final_spec(d_spec);

    FSS::postprocess(&final_spec, fc);

    typedef FormatterSpecificationNumericValue FSNVAlue;
    bsl::basic_string_view<t_CHAR>              sv(v);

    FSNVAlue finalWidth(final_spec.postprocessedWidth());

    FSNVAlue finalPrecision(final_spec.postprocessedPrecision());

    int maxDisplayWidth = 0;
    switch (finalPrecision.valueType()) {
      case FSNVAlue::e_DEFAULT: {
        maxDisplayWidth = std::numeric_limits<int>::max();
      } break;
      case FSNVAlue::e_VALUE: {
        maxDisplayWidth = finalPrecision.value();
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));
      } break;
    }

    int    displayWidthUsedByInputString = std::numeric_limits<int>::min();
    size_t charactersOfInputUsed         = sv.size();

    // Only do an analysis of the string if there is a possibility of
    // truncation or padding.
    if ((maxDisplayWidth < static_cast<int>(sv.size()) * 2) ||
        (finalWidth.valueType() != FSNVAlue::e_DEFAULT)) {
        findPrecisionLimitedString(&charactersOfInputUsed,
                                   &displayWidthUsedByInputString,
                                   sv,
                                   maxDisplayWidth);
    }

    int totalPadDisplayWidth = 0;

    switch (finalWidth.valueType()) {
      case FSNVAlue::e_DEFAULT: {
        totalPadDisplayWidth = 0;
      } break;
      case FSNVAlue::e_VALUE: {
        totalPadDisplayWidth =
               bsl::max(0, finalWidth.value() - displayWidthUsedByInputString);
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));
      } break;
    }

    BSLS_ASSERT(totalPadDisplayWidth >= 0);

    int leftPadFillerCopies = 0, rightPadFillerCopies = 0;

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.
    switch (d_spec.alignment()) {
      case FSS::e_ALIGN_DEFAULT:
      case FSS::e_ALIGN_LEFT: {
        leftPadFillerCopies  = 0;
        rightPadFillerCopies = totalPadDisplayWidth;
      } break;
      case FSS::e_ALIGN_MIDDLE: {
        leftPadFillerCopies  = (totalPadDisplayWidth / 2);
        rightPadFillerCopies = ((totalPadDisplayWidth + 1) / 2);
      } break;
      case FSS::e_ALIGN_RIGHT: {
        leftPadFillerCopies  = totalPadDisplayWidth;
        rightPadFillerCopies = 0;
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid alignment"));
      } break;
    }

    typename t_FORMAT_CONTEXT::iterator outIterator = fc.out();

    for (int i = 0; i < leftPadFillerCopies; ++i) {
        outIterator = bsl::copy(
                           final_spec.filler(),
                           final_spec.filler() + final_spec.fillerCharacters(),
                           outIterator);
    }

    outIterator = bsl::copy(sv.begin(),
                            sv.begin() + charactersOfInputUsed,
                            outIterator);

    for (int i = 0; i < rightPadFillerCopies; ++i) {
        outIterator = bsl::copy(
                           final_spec.filler(),
                           final_spec.filler() + final_spec.fillerCharacters(),
                           outIterator);
    }

    return outIterator;
}

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
FormatterString_Imp<t_CHAR>::parse(t_PARSE_CONTEXT& pc)
{
    FSS::parse(&d_spec, &pc, FSS::e_CATEGORY_STRING);

    if (d_spec.sign() != FSS::e_SIGN_DEFAULT)
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

    if (d_spec.formatType() == FSS::e_STRING_ESCAPED)
        BSLS_THROW(bsl::format_error("String escaping not supported"));

    return pc.begin();
}

}  // close namespace bslfmt
}  // close enterprise namespace


namespace bsl {

                     // ==================================
                     // struct formatter<t_CHAR *, t_CHAR>
                     // ==================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR *, t_CHAR>::format(
                                                   const t_CHAR      *v,
                                                   t_FORMAT_CONTEXT&  fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}

                  // ========================================
                  // struct formatter<const t_CHAR *, t_CHAR>
                  // ========================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<const t_CHAR *, t_CHAR>::format(
                                                   const t_CHAR      *v,
                                                   t_FORMAT_CONTEXT&  fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}



                    // ===================================
                    // struct formatter<t_CHAR[N], t_CHAR>
                    // ===================================

// ACCESSORS
template <class t_CHAR, size_t t_SIZE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR[t_SIZE], t_CHAR>::format(
                                                   const t_CHAR      *v,
                                                   t_FORMAT_CONTEXT&  fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v, t_SIZE);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}

                // ===========================================
                // struct formatter<std::basic_string, t_CHAR>
                // ===========================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<std::basic_string<t_CHAR>, t_CHAR>::format(
                                     const std::basic_string<t_CHAR>& v,
                                     t_FORMAT_CONTEXT&                fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}


                // ===========================================
                // struct formatter<bsl::basic_string, t_CHAR>
                // ===========================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string<t_CHAR>, t_CHAR>::format(
                                     const bsl::basic_string<t_CHAR>& v,
                                     t_FORMAT_CONTEXT&                fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}


#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ================================================
              // struct formatter<std::basic_string_view, t_CHAR>
              // ================================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<std::basic_string_view<t_CHAR>, t_CHAR>::format(
                                       std::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}

#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY


#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ================================================
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ================================================

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>::format(
                                       bsl::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
{
    bsl::basic_string_view<t_CHAR> sv(v);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(sv,
                                                                         fc);
}

#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

}  // close namespace bsl

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
