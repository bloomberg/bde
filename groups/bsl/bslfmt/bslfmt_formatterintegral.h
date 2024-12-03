// bslfmt_formatterintegral.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERINTEGRAL
#define INCLUDED_BSLFMT_FORMATTERINTEGRAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for integer types
//
//@CLASSES:
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for integer types.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting an integer
/// - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to format an integer with
// defined alignment and padding.
//
// ```
//  bslfmt::Formatter_MockParseContext<char> mpc("*<5x", 1);
//
//  bsl::formatter<int, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  int value = 42;
//
//  bslfmt::Formatter_MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("2a***" == mfc.finalString());
// ```
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterspecificationstandard.h>
#include <bslfmt_formatterunicodeutils.h>

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

#include <locale>     // for `std::ctype`, `locale`
#include <string>     // for `std::char_traits`

#include <limits.h>   // for `std::numeric_limits`
#include <stdio.h>    // for `snprintf`

namespace BloombergLP {
namespace bslfmt {

                         // =================================
                         // struct FormatterIntegral_Category
                         // =================================

/// This struct provides a function that returns a category of the formatter in
/// accordance with the (template parameter) `t_VALUE_TYPE`.
template <class t_VALUE_TYPE>
struct FormatterIntegral_Category {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `bool`.
template <>
struct FormatterIntegral_Category<bool> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `void *`.
template <>
struct FormatterIntegral_Category<void *> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `const void *`.
template <>
struct FormatterIntegral_Category<const void *> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `bsl::nullptr_t`.
template <>
struct FormatterIntegral_Category<bsl::nullptr_t> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `char`.
template <>
struct FormatterIntegral_Category<char> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
};

/// This is a specialization of `FormatterIntegral_Category` template for
/// `wchar_t`.
template <>
struct FormatterIntegral_Category<wchar_t> {
    static
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
    category();
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
    // PRIVATE CLASS TYPES

    /// A type alias for the `FormatterSpecificationStandard<t_CHAR>`.
    typedef FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;  // format specification.

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

  protected:
    // PROTECTED ACCESSORS

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
    const FormatterSpecificationStandard<t_CHAR>& specification() const ;
};

                         // ========================
                         // struct FormatterIntegral
                         // ========================

/// This type implements the formatter logic specific to integer types other
/// than character, boolean and pointer type.
template <class t_VALUE, class t_CHAR>
struct FormatterIntegral : public FormatterIntegralBase<t_VALUE, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           value,
                                        t_FORMAT_CONTEXT& formatContext) const;
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned char`.
template <class t_CHAR>
struct formatter<unsigned char, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<unsigned char, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `short int`.
template <class t_CHAR>
struct formatter<short, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned short int`.
template <class t_CHAR>
struct formatter<unsigned short, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<unsigned short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type `int`.
template <class t_CHAR>
struct formatter<int, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<int, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned int`.
template <class t_CHAR>
struct formatter<unsigned, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<unsigned, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long int`.
template <class t_CHAR>
struct formatter<long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long int`.
template <class t_CHAR>
struct formatter<unsigned long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<unsigned long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long long int`.
template <class t_CHAR>
struct formatter<long long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<long long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long long int`.
template <class t_CHAR>
struct formatter<unsigned long long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral<unsigned long long, t_CHAR> {
};

}  // close namespace bsl


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                     // ---------------------------------
                     // struct FormatterIntegral_Category
                     // ---------------------------------

template <class t_VALUE_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<t_VALUE_TYPE>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_INTEGRAL;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<bool>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_BOOLEAN;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<void *>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_POINTER;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<const void *>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_POINTER;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<bsl::nullptr_t>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_POINTER;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<char>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_CHARACTER;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard_Enums::Category
FormatterIntegral_Category<wchar_t>::category()
{
    return FormatterSpecificationStandard_Enums::e_CATEGORY_CHARACTER;
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
    FSS::parse(&d_spec,
               &parseContext,
               FormatterIntegral_Category<t_VALUE>::category());

    if (d_spec.localeSpecificFlag()) {
        BSLS_THROW(
                bsl::format_error("Formatting L specifier not supported"));
    }

    if (d_spec.formatType() == FSS::e_CHARACTER_ESCAPED)
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
          case FSS::e_SIGN_POSITIVE: {
            *prefixEnd = '+';
            ++prefixEnd;
          } break;
          case FSS::e_SIGN_SPACE: {
            *prefixEnd = ' ';
            ++prefixEnd;
          } break;
          default: {
            // Suppress compiler warning.
            // FSS::e_SIGN_DEFAULT
            // FSS::e_SIGN_NEGATIVE
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
          case FSS::e_INTEGRAL_BINARY: {  // `b`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0b";
                formatPrefixLength = 2;
            }
          } break;
          case FSS::e_INTEGRAL_BINARY_UC: {  // `B`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0B";
                formatPrefixLength = 2;
            }
          } break;
          case FSS::e_INTEGRAL_DECIMAL:
          case FSS::e_INTEGRAL_CHARACTER: {  // none or `d`
            // No prefix.
          } break;
          case FSS::e_INTEGRAL_OCTAL: {  // `o`
            if (d_spec.alternativeFlag() && 0 != value) {
                formatPrefix       = "0";
                formatPrefixLength = 1;
            }
          } break;
          case FSS::e_INTEGRAL_HEX: {  // `x`
            if (d_spec.alternativeFlag()) {
                formatPrefix       = "0x";
                formatPrefixLength = 2;
            }
          } break;
          case FSS::e_INTEGRAL_HEX_UC: {  // `X`
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
      case FSS::e_INTEGRAL_BINARY: {
        valueBase = 2;
      } break;
      case FSS::e_INTEGRAL_BINARY_UC: {
        valueBase = 2;
      } break;
      case FSS::e_INTEGRAL_CHARACTER: {
        if (isInRange(value)) {
            *valueStorage = static_cast<t_CHAR>(value);
            return valueStorage + 1;                                  // RETURN
        }
        else {
            BSLS_THROW(bsl::format_error("Integer value is not in the range "
                                         "of character representation"));
        }
      } break;
      case FSS::e_INTEGRAL_DECIMAL: {
        // Default value (10).
      } break;
      case FSS::e_INTEGRAL_OCTAL: {
        valueBase = 8;
      } break;
      case FSS::e_INTEGRAL_HEX: {
        valueBase = 16;
      } break;
      case FSS::e_INTEGRAL_HEX_UC: {
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

    if (FSS::e_INTEGRAL_HEX_UC == d_spec.formatType()) {
        // Unfortunately, `NumericFormatterUtil::toChars` uses only lowercase
        // characters to represent hexadecimal numbers.  So we have to
        // additionally modify the resulting string for uppercase hexadecimal
        // format.

        BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::toUpper(valueBuf,
                                                                  valueEnd);
    }

     t_CHAR *convertedValueEnd = valueStorage;

     convertedValueEnd =
              BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::outputFromChar(
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
    typedef FormatterSpecification_NumericValue FSNValue;

    const FSS& parsedSpec = this->specification();
    FSS        spec(parsedSpec);
    FSS::postprocess(&spec, formatContext);

    FSNValue finalWidth(spec.postprocessedWidth());

    bool representAsChar = FSS::e_INTEGRAL_CHARACTER == d_spec.formatType();

    int leftPadFillerCopiesNum  = 0;
    int rightPadFillerCopiesNum = 0;
    int zeroPadFillerCopiesNum  = 0;

    int commonLength = representAsChar
                           ? static_cast<int>(1 + (prefixEnd - prefixBegin))
                           : static_cast<int>((valueEnd - valueBegin) +
                                              (prefixEnd - prefixBegin));

    // Filling the remaining space.
    if (commonLength < finalWidth.value()) {
        // We need to fill the remaining space.

        int totalPadDisplayWidth = finalWidth.value() - commonLength;

        if (FSS::e_ALIGN_DEFAULT == spec.alignment() &&
            spec.zeroPaddingFlag()) {
            // Space will be filled with zeros.

            zeroPadFillerCopiesNum = totalPadDisplayWidth;
        }
        else {
            // Alignment with appropriate symbol is required.

            switch (spec.alignment()) {
              case FSS::e_ALIGN_LEFT: {
                leftPadFillerCopiesNum  = 0;
                rightPadFillerCopiesNum = totalPadDisplayWidth;
              } break;
              case FSS::e_ALIGN_MIDDLE: {
                leftPadFillerCopiesNum  = (totalPadDisplayWidth / 2);
                rightPadFillerCopiesNum = ((totalPadDisplayWidth + 1) / 2);
              } break;
              case FSS::e_ALIGN_DEFAULT: {
                // Left alignment is default for non-arithmetic presentation
                // types.

                typedef FormatterSpecificationStandard_Enums FSSEnums;
                if (FSSEnums::e_INTEGRAL_CHARACTER  == d_spec.formatType() ||
                    FSSEnums::e_CHARACTER_CHARACTER == d_spec.formatType() ||
                    FSSEnums::e_BOOLEAN_STRING      == d_spec.formatType()) {
                    leftPadFillerCopiesNum  = 0;
                    rightPadFillerCopiesNum = totalPadDisplayWidth;
                }
                else {
                    leftPadFillerCopiesNum  = totalPadDisplayWidth;
                    rightPadFillerCopiesNum = 0;
                }
              } break;
              case FSS::e_ALIGN_RIGHT: {
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
                                spec.filler() + spec.fillerCharacters(),
                                outIterator);
    }

    // perfix
    outIterator =
              BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::outputFromChar(
                  prefixBegin,
                  prefixEnd,
                  outIterator);

    // zero filler
    for (int i = 0; i < zeroPadFillerCopiesNum; ++i) {
        const char *zeroFiller = "0";
        outIterator =
              BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::outputFromChar(
                  zeroFiller,
                  zeroFiller + 1,
                  outIterator);
    }

    // value
    outIterator = bsl::copy(valueBegin, valueEnd, outIterator);

    // right padding
    for (int i = 0; i < rightPadFillerCopiesNum; ++i) {
        outIterator = bsl::copy(spec.filler(),
                                spec.filler() + spec.fillerCharacters(),
                                outIterator);
    }

    return outIterator;
}

template <class t_VALUE, class t_CHAR>
inline
const FormatterSpecificationStandard<t_CHAR>&
FormatterIntegralBase<t_VALUE, t_CHAR>::specification() const
{
    return d_spec;
}

                         // -----------------------
                         // class FormatterIntegral
                         // -----------------------

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator
FormatterIntegral<t_VALUE, t_CHAR>::format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    const int  maxPrefixSize = 4;
    char       prefixBuf[maxPrefixSize];
    char      *prefixBegin = prefixBuf;
    char      *prefixEnd = this->formatPrefix(prefixBuf, maxPrefixSize, value);

    // We want to make sure that we have enough space to accommodate any
    // representation of the `value`.  Binary representation takes up the most
    // space.

    const int  maxValueSize = NFUtil::ToCharsMaxLength<t_VALUE, 2>::k_VALUE;
    t_CHAR     valueBuf[maxValueSize];
    t_CHAR    *valueBegin = valueBuf;
    t_CHAR    *valueEnd   = this->formatValue(valueBuf, maxValueSize, value);

    if (value < 0) {
        // We want to omit minus sign added by
        // `NumericFormatterUtil::toChars` since we  already added it
        // manually to the prefix.
        ++valueBegin;
    }

    return this->outputValue(prefixBegin,
                             prefixEnd,
                             valueBegin,
                             valueEnd,
                             formatContext);
}

}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERINTEGRAL

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
