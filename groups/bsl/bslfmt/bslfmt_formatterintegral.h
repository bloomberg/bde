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
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
//
//..
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
//..
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

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

namespace BloombergLP {
namespace bslfmt {

/// This struct is a specialization of the `bsl::formatter` template for the
/// integer types.
template <class t_VALUE, class t_CHAR>
struct Formatter_IntegerBase {
  private:
    // PRIVATE CLASS TYPES

    /// A type alias for the `FormatterSpecificationStandard<t_CHAR>`.
    typedef FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;  // Parsed specification.

  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // MANIPULATORS

    /// Parse the specified `parseContext` and return an iterator, pointing to
    /// the beginning of the format string.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                 t_PARSE_CONTEXT& parseContext)
    {
        FSS::parse(&d_spec, &parseContext, FSS::e_CATEGORY_INTEGRAL);

        if (d_spec.localeSpecificFlag()) {
            BSLS_THROW(
                    bsl::format_error("Formatting L specifier not supported"));
        }

        if (parseContext.begin() != parseContext.end() &&
            *parseContext.begin() != '}') {
            BSLS_THROW(bsl::format_error("not implemented"));
        }

        return parseContext.begin();
    }

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        typedef FormatterSpecification_NumericValue       FSNVAlue;

        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, formatContext);

        char       valueBuf[NFUtil::ToCharsMaxLength<t_VALUE>::k_VALUE];
        int        valueBase         = 10;
        const int  MAX_PREFIX_LENGTH = 10;
        char       prefixBuf[MAX_PREFIX_LENGTH];
        char      *prefixEnd         = prefixBuf;

        // Adding sign.

        if (value > 0) {
            switch (final_spec.sign()) {
              case FSS::e_SIGN_POSITIVE: {
                if (value > 0) {
                    *prefixEnd = '+';
                    ++prefixEnd;
                }
              } break;
              case FSS::e_SIGN_SPACE: {
                if (value > 0) {
                    *prefixEnd = ' ';
                    ++prefixEnd;
                }
              } break;
              default: {
                // Suppress compiler warning.
                // FSS::e_SIGN_DEFAULT
                // FSS::e_SIGN_NEGATIVE
              }
            }
        }
        else if (value < 0) {
            // As we might have an alternate form that requires special prefix,
            // we add the minus sign ourselves without relying on
            // `NumericFormatterUtil::toChars` negative value conversion.

            *prefixEnd = '-';
            ++prefixEnd;
            value = -value;
        }

        // Adding alternate form prefix.

        {
            const char *formatPrefix       = 0;
            int         formatPrefixLength = 0;
            switch (final_spec.formatType()) {
              case FSS::e_INTEGRAL_BINARY: {  // `b`
                if (final_spec.alternativeFlag()) {
                    formatPrefix       = "0b";
                    formatPrefixLength = 2;
                }
                valueBase = 2;
              } break;
              case FSS::e_INTEGRAL_BINARY_UC: {  // `B`
                if (final_spec.alternativeFlag()) {
                    formatPrefix       = "0B";
                    formatPrefixLength = 2;
                }
                valueBase = 2;
              } break;
              case FSS::e_INTEGRAL_DECIMAL: {  // none or `d`
                valueBase = 10;
              } break;
              case FSS::e_INTEGRAL_OCTAL: {  // `o`
                if (final_spec.alternativeFlag()) {
                    formatPrefix       = "0";
                    formatPrefixLength = 1;
                }
                valueBase = 8;
              } break;
              case FSS::e_INTEGRAL_HEX: {  // `x`
                if (final_spec.alternativeFlag()) {
                    formatPrefix       = "0x";
                    formatPrefixLength = 2;
                }
                valueBase = 16;
              } break;
              case FSS::e_INTEGRAL_HEX_UC: {  // `X`
                if (final_spec.alternativeFlag()) {
                    formatPrefix       = "0X";
                    formatPrefixLength = 2;
                }
                valueBase = 16;
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
        }

        // Converting value.

        char *result = NFUtil::toChars(valueBuf,
                                       valueBuf + sizeof(valueBuf),
                                       value,
                                       valueBase);

        if (FSS::e_INTEGRAL_HEX_UC == final_spec.formatType()) {
            // Unfortunately, `NumericFormatterUtil::toChars` uses only
            // lowercase characters to represent hexadecimal numbers.  So we
            // have to additionally modify the resulting string for uppercase
            // hexadecimal format.

            BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::toUpper(valueBuf,
                                                                      result);
        }

        int commonLength = static_cast<int>((result - valueBuf) +
                                              (prefixEnd - prefixBuf));

        // Filling the remaining space.

        FSNVAlue finalWidth(final_spec.postprocessedWidth());

        int leftPadFillerCopiesNum  = 0;
        int rightPadFillerCopiesNum = 0;
        int zeroPadFillerCopiesNum  = 0;

        if (commonLength < finalWidth.value()) {
            // We need to fill the remaining space.

            int totalPadDisplayWidth = finalWidth.value() - commonLength;

            if (FSS::e_ALIGN_DEFAULT == final_spec.alignment() &&
                final_spec.zeroPaddingFlag()) {
                // Space will be filled with zeros.

                zeroPadFillerCopiesNum = totalPadDisplayWidth;
            }
            else {
                // Alignment with appropriate symbol is required.

                switch (d_spec.alignment()) {
                  case FSS::e_ALIGN_LEFT: {
                    leftPadFillerCopiesNum  = 0;
                    rightPadFillerCopiesNum = totalPadDisplayWidth;
                  } break;
                  case FSS::e_ALIGN_MIDDLE: {
                    leftPadFillerCopiesNum  = (totalPadDisplayWidth / 2);
                    rightPadFillerCopiesNum = ((totalPadDisplayWidth + 1) / 2);
                  } break;
                  case FSS::e_ALIGN_DEFAULT:
                  case FSS::e_ALIGN_RIGHT: {
                    leftPadFillerCopiesNum  = totalPadDisplayWidth;
                    rightPadFillerCopiesNum = 0;
                  } break;
                  default: {
                    BSLS_THROW(bsl::format_error("Invalid alignment"));
                  } break;
                }
            }
        }

        // Assembling the final string.

        typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

        for (int i = 0; i < leftPadFillerCopiesNum; ++i) {
            outIterator = bsl::copy(
                           final_spec.filler(),
                           final_spec.filler() + final_spec.fillerCharacters(),
                           outIterator);
        }

        outIterator =
              BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::outputFromChar(
                  prefixBuf,
                  prefixEnd,
                  outIterator);

        for (int i = 0; i < zeroPadFillerCopiesNum; ++i) {
            const char *zeroFiller = "0";
            outIterator = BloombergLP::bslfmt::Formatter_CharUtils<
                t_CHAR>::outputFromChar(zeroFiller,
                                        zeroFiller + 1,
                                        outIterator);
        }

        outIterator =
              BloombergLP::bslfmt::Formatter_CharUtils<t_CHAR>::outputFromChar(
                  valueBuf,
                  result,
                  outIterator);

        for (int i = 0; i < rightPadFillerCopiesNum; ++i) {
            outIterator = bsl::copy(
                           final_spec.filler(),
                           final_spec.filler() + final_spec.fillerCharacters(),
                           outIterator);
        }

        return outIterator;
    }
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// Partial specialization of the `bsl::formatter` template for the type
/// `short int`.
template <class t_CHAR>
struct formatter<short, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned short int`.
template <class t_CHAR>
struct formatter<unsigned short, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type `int`.
template <class t_CHAR>
struct formatter<int, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<int, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned int`.
template <class t_CHAR>
struct formatter<unsigned, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long int`.
template <class t_CHAR>
struct formatter<long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long int`.
template <class t_CHAR>
struct formatter<unsigned long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long long int`.
template <class t_CHAR>
struct formatter<long long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<long long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long long int`.
template <class t_CHAR>
struct formatter<unsigned long long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned long long, t_CHAR> {
};
}

#endif  // INCLUDED_BSLFMT_FORMATTERBASE

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
