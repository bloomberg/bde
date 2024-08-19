// bslfmt_formatterstring.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSTRING
#define INCLUDED_BSLFMT_FORMATTERSTRING

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

namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR>
struct Formatter_StringBase {
  private:
    // PRIVATE CLASS TYPES
    typedef FormatterSpecificationStandard<t_CHAR> FSS;

    // DATA
    FSS d_spec;

    // PRIVATE CLASS METHODS
    static void findPrecisionLimitedString(
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

            if (*widthUsed + gcresult.firstCodePointWidth >
                maxTotalDisplayWidth)
                break;

            bytesLeft -= gcresult.numSourceBytes;
            current += gcresult.numSourceBytes / sizeof(t_CHAR);

            *charactersUsed += gcresult.numSourceBytes / sizeof(t_CHAR);

            *widthUsed += gcresult.firstCodePointWidth;
        }
    }
  public:
    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc)
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
            BSLS_THROW(bsl::format_error(
                         "Formatting L specifier not supported"));

        if (d_spec.formatType() == FSS::e_STRING_ESCAPED)
            BSLS_THROW(
                    bsl::format_error("String escaping not supported"));

        return pc.begin();
    }

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatImpl(
                                       bsl::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
    {
        FSS final_spec(d_spec);

        FSS::postprocess(&final_spec, fc);

        typedef FormatterSpecification_NumericValue FSNVAlue;
        bsl::basic_string_view<t_CHAR> sv(v);

        FSNVAlue finalWidth(final_spec.postprocessedWidth());

        FSNVAlue finalPrecision(final_spec.postprocessedPrecision());

        int    displayWidthUsedByInputString = 0;
        size_t charactersOfInputUsed         = 0;

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
        findPrecisionLimitedString(&charactersOfInputUsed,
                                    &displayWidthUsedByInputString,
                                    sv,
                                    maxDisplayWidth);

        int totalPadDisplayWidth = 0;

        switch (finalWidth.valueType()) {
          case FSNVAlue::e_DEFAULT: {
            totalPadDisplayWidth = 0;
          } break;
          case FSNVAlue::e_VALUE: {
            totalPadDisplayWidth = finalWidth.value() -
                                   displayWidthUsedByInputString;
          } break;
          default: {
            BSLS_THROW(bsl::format_error("Invalid precision specifier"));
          } break;
        }

        BSLS_ASSERT(totalPadDisplayWidth >= 0);

        int leftPadFillerCopies = 0, rightPadFillerCopies = 0;

        switch (d_spec.alignment()) {
          case FSS::e_ALIGN_DEFAULT:
          case FSS::e_ALIGN_LEFT: {
            leftPadFillerCopies = 0;
            rightPadFillerCopies = totalPadDisplayWidth /
                                   final_spec.fillerCodePointDisplayWidth();
          } break;
          case FSS::e_ALIGN_MIDDLE: {
            leftPadFillerCopies = (totalPadDisplayWidth / 2) /
                                  final_spec.fillerCodePointDisplayWidth();
            rightPadFillerCopies = ((totalPadDisplayWidth + 1) / 2) /
                                   final_spec.fillerCodePointDisplayWidth();
          } break;
          case FSS::e_ALIGN_RIGHT: {
            leftPadFillerCopies = totalPadDisplayWidth /
                                  final_spec.fillerCodePointDisplayWidth();
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
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const t_CHAR      *v,
                                               t_FORMAT_CONTEXT&  fc) const
    {
        bsl::basic_string_view<t_CHAR> sv(v);
        return BloombergLP::bslfmt::Formatter_StringBase<t_CHAR>::formatImpl(
                                                                           sv, fc);
    }
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
template <class t_CHAR>
struct formatter<std::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std`
    // string view, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       std::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
    {
        bsl::basic_string_view<t_CHAR> sv(v);
        return BloombergLP::bslfmt::Formatter_StringBase<t_CHAR>::formatImpl(
                                                                           sv, fc);
    }
};
#endif

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

template <class t_CHAR>
struct formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::Formatter_StringBase<t_CHAR> {
  public:
    // TRAITS

    // If we do not alias string view then we should alias its formatter from
    // the `std` namespace, so DO NOT DEFINE the trait
    // BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20.

    // ACCESSORS
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       bsl::basic_string_view<t_CHAR> v,
                                       t_FORMAT_CONTEXT&              fc) const
    {
        bsl::basic_string_view<t_CHAR> sv(v);
        return BloombergLP::bslfmt::Formatter_StringBase<t_CHAR>::formatImpl(
                                                                           sv, fc);
    }
};

#endif

}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
// FORMATTER SPECIALIZATIONS

/// This is a customisation of `std::formatter` for `bsl::string` and
/// `bsl::wstring`. We cannot rely on the automatic promotion mechanism because
/// we do not define bsl formatters for `bsl::string` or `bsl::wstring` (on the
/// basis that the `string_view` formatter is used instead).
template <class t_CHAR>
struct formatter<bsl::basic_string<t_CHAR>, t_CHAR>
: bsl::formatter<bsl::basic_string_view<t_CHAR>, t_CHAR> {
};
}  // close namespace std
#endif

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
