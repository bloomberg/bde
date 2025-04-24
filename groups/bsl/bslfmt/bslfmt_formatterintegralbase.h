// bslfmt_formatterintegralbase.h                                     -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERINTEGRALBASE
#define INCLUDED_BSLFMT_FORMATTERINTEGRALBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for integer types
//
//@CLASSES:
//   bslfmt::FormatterIntegralBase: base for integral formatter specializations
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for integer types.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of the `bsl::formatter`
// for integral types.
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formattercharutil.h>
#include <bslfmt_standardformatspecification.h>

#include <bslalg_numericformatterutil.h>

#include <bsls_keyword.h>

#include <limits.h>   // for `std::numeric_limits`

namespace BloombergLP {
namespace bslfmt {

                         // =================================
                         // struct FormatterIntegral_Category
                         // =================================

/// This struct provides a function that returns a category of the formatter in
/// accordance with the (template parameter) `t_VALUE_TYPE`.
template <class t_VALUE_TYPE>
struct FormatterIntegral_Category {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
        typename StandardFormatSpecification<t_CHAR>::Category
        category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `bool`.
template <>
struct FormatterIntegral_Category<bool> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `void *`.
template <>
struct FormatterIntegral_Category<void *> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `const void *`.
template <>
struct FormatterIntegral_Category<const void *> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `bsl::nullptr_t`.
template <>
struct FormatterIntegral_Category<bsl::nullptr_t> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `char`.
template <>
struct FormatterIntegral_Category<char> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `wchar_t`.
template <>
struct FormatterIntegral_Category<wchar_t> {
    // CLASS METHODS
    template <class t_CHAR>
    static BSLS_KEYWORD_CONSTEXPR_CPP20
    typename StandardFormatSpecification<t_CHAR>::Category category();
};

                         // ============================
                         // struct FormatterIntegralBase
                         // ============================

/// This struct is a base class for `bsl::formatter` specializations for
/// integer types as well as for other types that can be represented as integer
/// (i.e. `bool`, character and pointer types).  This is a private class and
/// should not be used directly. The formatting of variables of a certain type
/// is performed using the corresponding classes inherited from this one.
/// Accordingly, the auxiliary functions used are declared protected.
template <class t_VALUE, class t_CHAR>
struct FormatterIntegralBase {
  private:
    // PRIVATE TYPES

    /// A type alias for the `StandardFormatSpecification<t_CHAR>`.
    typedef StandardFormatSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;  // format specification.

    // PRIVATE ACCESSORS

    /// Return `true` if the specified `value` is in the range of representable
    /// values for the (template parameter) `t_CHAR`, and `false` otherwise.
    template <class t_ACTUAL_VALUE_TYPE>
    bool isInRange(const t_ACTUAL_VALUE_TYPE& value) const;

  public:
    // MANIPULATORS

    /// Parse the specified `parseContext` and return an iterator, pointing to
    /// the beginning of the format string.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);

    // ACCESSORS

    /// Create the prefix for output of the specified `value` and write it into
    /// the character buffer starting a the specified `prefixStorage` and
    /// having the specified `storageSize`.  Return the address one past the
    /// last character.  Throw `bsl::format_error` if parsed format
    /// specification is intended for non-integer type.
    template <class t_ACTUAL_VALUE_TYPE>
    char *formatPrefix(char                *prefixStorage,
                       int                  storageSize,
                       t_ACTUAL_VALUE_TYPE  value) const;

    /// Create the string representation of the specified `value` and write it
    /// into the character buffer starting at the specified `valueStorage` and
    /// having the specified `storageSize`.  Return the address one past the
    /// last character.  Throw `bsl::format_error` if parsed format
    /// specification is intended for non-integer type or the `value` is not in
    /// the range of representable values for the (template parameter) `t_CHAR`
    /// with character type specification.
    template <class t_ACTUAL_VALUE_TYPE>
    t_CHAR *formatValue(t_CHAR              *valueStorage,
                        int                  storageSize,
                        t_ACTUAL_VALUE_TYPE  value) const;

    /// Copy the prefix of value string representation starting at the
    /// specified `prefixBegin` and ending at the specified `prefixEnd` as well
    /// as representation itself starting at the specified `valueBegin` and
    /// ending at the specified `valueEnd` to the output that the output
    /// iterator of the `formatContext` points to.  Throw `bsl::format_error`
    /// if parsed format specification states incorrect alignment type.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator outputValue(
                                       const char        *prefixBegin,
                                       const char        *prefixEnd,
                                       const t_CHAR      *valueBegin,
                                       const t_CHAR      *valueEnd,
                                       t_FORMAT_CONTEXT&  formatContext) const;

    /// Return a reference to the specification of this formatter.
    const StandardFormatSpecification<t_CHAR>& specification() const ;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // struct FormatterIntegral_Category
                     // ---------------------------------

template <class t_VALUE_TYPE>
template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<t_VALUE_TYPE>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_INTEGRAL;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<bool>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_BOOLEAN;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<void *>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_POINTER;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<const void *>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_POINTER;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<bsl::nullptr_t>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_POINTER;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<char>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_CHARACTER;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::Category
FormatterIntegral_Category<wchar_t>::category()
{
    return StandardFormatSpecification<t_CHAR>::e_CATEGORY_CHARACTER;
}

                         // ---------------------------
                         // class FormatterIntegralBase
                         // ---------------------------

template <class t_VALUE, class t_CHAR>
template <class t_ACTUAL_VALUE_TYPE>
inline
bool FormatterIntegralBase<t_VALUE, t_CHAR>::isInRange(
                                        const t_ACTUAL_VALUE_TYPE& value) const
{
    const long double min =
                  static_cast<long double>(std::numeric_limits<t_CHAR>::min());
    const long double max =
                  static_cast<long double>(std::numeric_limits<t_CHAR>::max());
    const long double doubleValue = static_cast<long double>(value);

    return min <= doubleValue && max >= doubleValue;
}

template <class t_VALUE, class t_CHAR>
template <class t_PARSE_CONTEXT>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
FormatterIntegralBase<t_VALUE, t_CHAR>::parse(t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext,
             FormatterIntegral_Category<t_VALUE>::template category<t_CHAR>());

    if (d_spec.localeSpecificFlag()) {
        BSLS_THROW(
                bsl::format_error("Formatting L specifier not supported"));
    }

    if (d_spec.formatType() == Specification::e_CHARACTER_ESCAPED)
        BSLS_THROW(bsl::format_error("Character escaping not supported"));

    return parseContext.begin();
}

template <class t_VALUE, class t_CHAR>
template <class t_ACTUAL_VALUE_TYPE>
inline
char *FormatterIntegralBase<t_VALUE, t_CHAR>::formatPrefix(
                                            char                *prefixStorage,
                                            int                  storageSize,
                                            t_ACTUAL_VALUE_TYPE  value) const
{
    BSLS_ASSERT(prefixStorage);
    BSLS_ASSERT(4 <= storageSize);

    (void) storageSize; // suppress compiler warning

    char *prefixEnd = prefixStorage;

    // Adding sign.

    if (value >= 0) {
        switch (d_spec.sign()) {
          case Specification::e_SIGN_POSITIVE: {
            *prefixEnd = '+';
            ++prefixEnd;
          } break;
          case Specification::e_SIGN_SPACE: {
            *prefixEnd = ' ';
            ++prefixEnd;
          } break;
          default: {
            // Suppress compiler warning.
            // Specification::e_SIGN_DEFAULT
            // Specification::e_SIGN_NEGATIVE
          }
        }
    }
    else {
        // As we might have an alternate form that requires special prefix, we
        // add the minus sign ourselves without relying on
        // `NumericFormatterUtil::toChars` negative value conversion.

        *prefixEnd = '-';
        ++prefixEnd;
    }

    // Adding alternate form prefix.

    {
        const char *formatPrefix       = 0;
        int         formatPrefixLength = 0;
        switch (d_spec.formatType()) {
          case Specification::e_INTEGRAL_BINARY: {  // `b`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0b";
                formatPrefixLength = 2;
            }
          } break;
          case Specification::e_INTEGRAL_BINARY_UC: {  // `B`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0B";
                formatPrefixLength = 2;
            }
          } break;
          case Specification::e_INTEGRAL_DECIMAL:
          case Specification::e_INTEGRAL_CHARACTER: {  // none or `d`
            // No prefix.
          } break;
          case Specification::e_INTEGRAL_OCTAL: {  // `o`
            if (d_spec.alternativeFlag() && 0 != value) {
                formatPrefix       = "0";
                formatPrefixLength = 1;
            }
          } break;
          case Specification::e_INTEGRAL_HEX: {  // `x`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0x";
                formatPrefixLength = 2;
            }
          } break;
          case Specification::e_INTEGRAL_HEX_UC: {  // `X`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0X";
                formatPrefixLength = 2;
            }
          } break;
          default: {
            BSLS_THROW(bsl::format_error("Invalid integer format type"));
          }
        }
        if (formatPrefix) {
            prefixEnd = bsl::copy(formatPrefix,
                                  formatPrefix + formatPrefixLength,
                                  prefixEnd);
        }

        return prefixEnd;
    }
}

template <class t_VALUE, class t_CHAR>
template <class t_ACTUAL_VALUE_TYPE>
inline
t_CHAR *FormatterIntegralBase<t_VALUE, t_CHAR>::formatValue(
                                             t_CHAR              *valueStorage,
                                             int                  storageSize,
                                             t_ACTUAL_VALUE_TYPE  value) const
{
    BSLS_ASSERT(valueStorage);

    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    (void) storageSize; // suppress compiler warning

    int  valueBase = 10;
    switch (d_spec.formatType()) {
      case Specification::e_INTEGRAL_BINARY: {
        valueBase = 2;
      } break;
      case Specification::e_INTEGRAL_BINARY_UC: {
        valueBase = 2;
      } break;
      case Specification::e_INTEGRAL_CHARACTER: {
        if (isInRange(value)) {
            *valueStorage = static_cast<t_CHAR>(value);
            return valueStorage + 1;                                  // RETURN
        }
        else {
            BSLS_THROW(bsl::format_error("Integer value is not in the range "
                                         "of character representation"));
        }
      } break;
      case Specification::e_INTEGRAL_DECIMAL: {
        // Default value (10).
      } break;
      case Specification::e_INTEGRAL_OCTAL: {
        valueBase = 8;
      } break;
      case Specification::e_INTEGRAL_HEX: {
        valueBase = 16;
      } break;
      case Specification::e_INTEGRAL_HEX_UC: {
        valueBase = 16;
      } break;
      case Specification::e_POINTER_HEX: {
        valueBase = 16;
      } break;
      case Specification::e_POINTER_HEX_UC: {
        valueBase = 16;
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid integer format type"));
      }
    }

    // We want to make sure that we have enough space to accommodate any
    // representation of the `value`.  Binary representation takes up the most
    // space. Unfortunately we can not use `valueBase` value her, because
    // constant expression is required.

    const int maxValueSize =
                     NFUtil::ToCharsMaxLength<t_ACTUAL_VALUE_TYPE, 2>::k_VALUE;
    BSLS_ASSERT(maxValueSize <= storageSize);

    char  valueBuf[maxValueSize];
    char *valueEnd = NFUtil::toChars(valueBuf,
                                     valueBuf + maxValueSize,
                                     value,
                                     valueBase);

    if (Specification::e_INTEGRAL_HEX_UC == d_spec.formatType() ||
        Specification::e_POINTER_HEX_UC == d_spec.formatType()) {
        // Unfortunately, `NumericFormatterUtil::toChars` uses only lowercase
        // characters to represent hexadecimal numbers.  So we have to
        // additionally modify the resulting string for uppercase hexadecimal
        // format.

        BloombergLP::bslfmt::FormatterCharUtil<char>::toUpper(valueBuf,
                                                              valueEnd);
    }

     t_CHAR *convertedValueEnd = valueStorage;

     convertedValueEnd =
                BloombergLP::bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                    valueBuf,
                    valueEnd,
                    convertedValueEnd);

     return convertedValueEnd;
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator
FormatterIntegralBase<t_VALUE, t_CHAR>::outputValue(
                                        const char        *prefixBegin,
                                        const char        *prefixEnd,
                                        const t_CHAR      *valueBegin,
                                        const t_CHAR      *valueEnd,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    typedef FormatterSpecificationNumericValue NumericValue;

    const Specification& parsedSpec = this->specification();
    Specification        spec(parsedSpec);
    spec.postprocess(formatContext);

    NumericValue finalWidth(spec.postprocessedWidth());

    bool representAsChar = Specification::e_INTEGRAL_CHARACTER ==
                           d_spec.formatType();

    int leftPadFillerCopiesNum  = 0;
    int rightPadFillerCopiesNum = 0;
    int zeroPadFillerCopiesNum  = 0;

    int commonLength = representAsChar
                           ? static_cast<int>(1 + (prefixEnd - prefixBegin))
                           : static_cast<int>((valueEnd - valueBegin) +
                                              (prefixEnd - prefixBegin));

    // Filling the remaining space.
    if (finalWidth.category() != NumericValue::e_DEFAULT &&
        commonLength < finalWidth.value()) {
        // We need to fill the remaining space.

        int totalPadDisplayWidth = finalWidth.value() - commonLength;

        if (Specification::e_ALIGN_DEFAULT == spec.alignment() &&
            spec.zeroPaddingFlag()) {
            // Space will be filled with zeros.

            zeroPadFillerCopiesNum = totalPadDisplayWidth;
        }
        else {
            // Alignment with appropriate symbol is required.

            switch (spec.alignment()) {
              case Specification::e_ALIGN_LEFT: {
                leftPadFillerCopiesNum  = 0;
                rightPadFillerCopiesNum = totalPadDisplayWidth;
              } break;
              case Specification::e_ALIGN_MIDDLE: {
                leftPadFillerCopiesNum  = (totalPadDisplayWidth / 2);
                rightPadFillerCopiesNum = ((totalPadDisplayWidth + 1) / 2);
              } break;
              case Specification::e_ALIGN_DEFAULT: {
                // Left alignment is default for non-arithmetic presentation
                // types.

                typedef StandardFormatSpecification<t_CHAR> Spec;
                if (Spec::e_INTEGRAL_CHARACTER == d_spec.formatType() ||
                    Spec::e_CHARACTER_CHARACTER == d_spec.formatType() ||
                    Spec::e_BOOLEAN_STRING == d_spec.formatType()) {
                    leftPadFillerCopiesNum  = 0;
                    rightPadFillerCopiesNum = totalPadDisplayWidth;
                }
                else {
                    leftPadFillerCopiesNum  = totalPadDisplayWidth;
                    rightPadFillerCopiesNum = 0;
                }
              } break;
              case Specification::e_ALIGN_RIGHT: {
                leftPadFillerCopiesNum  = totalPadDisplayWidth;
                rightPadFillerCopiesNum = 0;
              } break;
              default: {
                BSLS_THROW(bsl::format_error("Invalid alignment"));
              }
            }
        }
    }

    // Assembling the final string.

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    // left padding
    for (int i = 0; i < leftPadFillerCopiesNum; ++i) {
        outIterator = bsl::copy(spec.filler(),
                                spec.filler() + spec.numFillerCharacters(),
                                outIterator);
    }

    // perfix
    outIterator =
                BloombergLP::bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                    prefixBegin,
                    prefixEnd,
                    outIterator);

    // zero filler
    for (int i = 0; i < zeroPadFillerCopiesNum; ++i) {
        const char *zeroFiller = "0";
        outIterator =
                BloombergLP::bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                    zeroFiller,
                    zeroFiller + 1,
                    outIterator);
    }

    // value
    outIterator = bsl::copy(valueBegin, valueEnd, outIterator);

    // right padding
    for (int i = 0; i < rightPadFillerCopiesNum; ++i) {
        outIterator = bsl::copy(spec.filler(),
                                spec.filler() + spec.numFillerCharacters(),
                                outIterator);
    }

    return outIterator;
}

template <class t_VALUE, class t_CHAR>
inline
const StandardFormatSpecification<t_CHAR>&
FormatterIntegralBase<t_VALUE, t_CHAR>::specification() const
{
    return d_spec;
}


}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERINTEGRALBASE

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
