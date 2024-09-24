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

template <class t_VALUE, class t_CHAR>
struct Formatter_IntegerBase {
  private:
    // PRIVATE CLASS TYPES
    typedef FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;    // Parsed specification.

  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        FSS::parse(&d_spec, &pc, FSS::e_CATEGORY_INTEGRAL);

        if (d_spec.localeSpecificFlag()) {
            BSLS_THROW(
                    bsl::format_error("Formatting L specifier not supported"));
        }

        if (pc.begin() != pc.end() && *pc.begin() != '}') {
            BSLS_THROW(bsl::format_error("not implemented"));
        }

        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(t_VALUE           x,
                                               t_FORMAT_CONTEXT& fc) const
    {
        typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
        typedef FormatterSpecification_NumericValue       FSNVAlue;

        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, fc);

        char       valueBuf[NFUtil::ToCharsMaxLength<t_VALUE>::k_VALUE];
        int        valueBase         = 10;
        const int  MAX_PREFIX_LENGTH = 10;
        char       prefixBuf[MAX_PREFIX_LENGTH];
        char      *prefixEnd         = prefixBuf;

        // Adding sign.

        switch (final_spec.sign()) {
          case FSS::e_SIGN_POSITIVE: {
            if (x > 0) {
                *prefixEnd = '+';
                ++prefixEnd;
            }
          } break;
          case FSS::e_SIGN_SPACE: {
            if (x > 0) {
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

        // Adding alternate form prefix.

        if (d_spec.alternativeFlag()) {
            const char *formatPrefix = 0;
            switch (final_spec.formatType()) {
              case FSS::e_INTEGRAL_BINARY: {     // `b`
                  formatPrefix = "0b";
                  valueBase = 2;
              } break;
              case FSS::e_INTEGRAL_BINARY_UC: {  // `B`
                  formatPrefix = "0B";
                  valueBase = 2;
              } break;
              case FSS::e_INTEGRAL_DECIMAL: {    // none or `d`
                  valueBase = 10;
              } break;
              case FSS::e_INTEGRAL_OCTAL: {      // `o`
                  valueBase = 8;
              } break;
              case FSS::e_INTEGRAL_HEX: {        // `x`
                  formatPrefix = "0x";
                  valueBase = 16;
              } break;
              case FSS::e_INTEGRAL_HEX_UC: {     // `X`
                  formatPrefix = "0X";
                  valueBase = 16;
              } break;
              default: {
                BSLS_THROW(bsl::format_error("Invalid integer format type"));
              }
            }
            if (formatPrefix) {
                prefixEnd = bsl::copy(formatPrefix,
                                      formatPrefix + 2,
                                      prefixEnd);
            }
        }

        // Converting value.

        char *result = NFUtil::toChars(valueBuf,
                                       valueBuf + sizeof(valueBuf),
                                       x,
                                       valueBase);

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

        typename t_FORMAT_CONTEXT::iterator outIterator = fc.out();

        for (int i = 0; i < leftPadFillerCopiesNum; ++i) {
            outIterator = bsl::copy(
                           final_spec.filler(),
                           final_spec.filler() + final_spec.fillerCharacters(),
                           outIterator);
        }

        outIterator = bsl::copy(prefixBuf, prefixEnd, outIterator);

        for (int i = 0; i < zeroPadFillerCopiesNum; ++i) {
            const char *zeroFiller = "0";
            outIterator = bsl::copy(zeroFiller, zeroFiller + 1, outIterator);
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

template <class t_CHAR>
struct formatter<short, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<short, t_CHAR> {
};

template <class t_CHAR>
struct formatter<unsigned short, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned short, t_CHAR> {
};

template <class t_CHAR>
struct formatter<int, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<int, t_CHAR> {
};

template <class t_CHAR>
struct formatter<unsigned, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned, t_CHAR> {
};

template <class t_CHAR>
struct formatter<long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<long, t_CHAR> {
};

template <class t_CHAR>
struct formatter<unsigned long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<unsigned long, t_CHAR> {
};

template <class t_CHAR>
struct formatter<long long, t_CHAR>
: BloombergLP::bslfmt::Formatter_IntegerBase<long long, t_CHAR> {
};

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
