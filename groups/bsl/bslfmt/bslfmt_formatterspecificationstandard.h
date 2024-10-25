// bslfmt_formatterspecificationstandard.h                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSTANDARD
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSTANDARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private utility for use within BSL `format` standard spec parsers
//
//@CLASSES:
//  FormatterSpecificationStandard: Utility to parse a standard format spec
//
//@DESCRIPTION: This component provides a mechanism to parse a formatting
// string that is a "standard formatting specification" as defined by
// [format.string.std].
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
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
#include <bslfmt_formatterunicodeutils.h>
#include <bslfmt_formatterspecificationsplitter.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'


namespace BloombergLP {
namespace bslfmt {

class FormatterSpecificationStandard_Enums
: public FormatterSpecification_SplitterEnums {
  public:
    // CLASS TYPES

    enum Category {
        e_CATEGORY_UNASSIGNED,
        e_CATEGORY_STRING,
        e_CATEGORY_INTEGRAL,
        e_CATEGORY_CHARACTER,
        e_CATEGORY_BOOLEAN,
        e_CATEGORY_FLOATING,
        e_CATEGORY_POINTER
    };

    enum FormatType {
        // Default value
        e_TYPE_UNASSIGNED,

        // String types
        e_STRING_DEFAULT,  // none or `n`
        e_STRING_ESCAPED,  // `?`

        // Integer types
        e_INTEGRAL_BINARY,     // `b`
        e_INTEGRAL_BINARY_UC,  // `B`
        e_INTEGRAL_CHARACTER,  // `c`
        e_INTEGRAL_DECIMAL,    // none or `d`
        e_INTEGRAL_OCTAL,      // `o`
        e_INTEGRAL_HEX,        // `x`
        e_INTEGRAL_HEX_UC,     // `X`

        // Character types
        e_CHARACTER_CHARACTER,  // none or `c`
        e_CHARACTER_BINARY,     // `b`
        e_CHARACTER_BINARY_UC,  // `B`
        e_CHARACTER_DECIMAL,    // `d`
        e_CHARACTER_OCTAL,      // `o`
        e_CHARACTER_HEX,        // `x`
        e_CHARACTER_HEX_UC,     // `X`
        e_CHARACTER_ESCAPED,    // `?`

        // Boolean types
        e_BOOLEAN_STRING,       // none or `s`

        // Floating point types
        e_FLOATING_DEFAULT,     // none
        e_FLOATING_HEXEXP,      // `a`
        e_FLOATING_HEXEXP_UC,   // `A`
        e_FLOATING_DECEXP,      // `e`
        e_FLOATING_DECEXP_UC,   // `E`
        e_FLOATING_DECIMAL,     // `f`
        e_FLOATING_DECIMAL_UC,  // `F`
        e_FLOATING_GENERAL,     // `g`
        e_FLOATING_GENERAL_UC,  // `g`

        // Pointer types
        e_POINTER_HEX,     // none or `p`
        e_POINTER_HEX_UC,  // `P`
    };
};

template <class t_CHAR>
class FormatterSpecificationStandard
: public FormatterSpecificationStandard_Enums {
  private:
    // CLASS TYPES
    typedef FormatterSpecification_Splitter<t_CHAR> FSS;

    // DATA
    ParsingStatus d_parsingStatus;
    FSS           d_basicSplitter;
    FormatType    d_formatType;
    int           d_widthArgId;
    int           d_precisionArgId;

    // PRIVATE CLASS FUNCTIONS
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseType(
                             FormatterSpecificationStandard        *outSpec,
                             const bsl::basic_string_view<t_CHAR>&  typeString,
                             Category                               category);

  public:

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard();

    // CLASS FUNCTIONS
    template <class t_PARSE_CONTEXT>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parse(
                                     FormatterSpecificationStandard *outSpec,
                                     t_PARSE_CONTEXT                *context,
                                     Category                        category);

    template <typename t_FORMAT_CONTEXT>
    static void postprocess(FormatterSpecificationStandard *outSpec,
                            const t_FORMAT_CONTEXT&         context);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCharacters() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCodePointDisplayWidth() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment     alignment() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign          sign() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          alternativeFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          zeroPaddingFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecification_NumericValue
                                               postprocessedWidth() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecification_NumericValue
                                               postprocessedPrecision() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          localeSpecificFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatType    formatType() const;
};

// PRIVATE CLASS FUNCTIONS


// CREATORS


template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationStandard<t_CHAR>::FormatterSpecificationStandard()
: d_parsingStatus(FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
, d_basicSplitter()
, d_formatType(FormatterSpecificationStandard::e_TYPE_UNASSIGNED)
, d_widthArgId(-1)
, d_precisionArgId(-1)
{
}

// ACCESSORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *FormatterSpecificationStandard<t_CHAR>::filler() const
{
    if (d_parsingStatus != FormatterSpecificationStandard::e_PARSING_COMPLETE)
        BSLS_THROW(bsl::format_error("Format standard specification "
                                     "'.postprocess' not called"));   // RETURN

    return d_basicSplitter.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationStandard<t_CHAR>::fillerCharacters() const
{
    if (d_parsingStatus != FormatterSpecificationStandard::e_PARSING_COMPLETE)
        BSLS_THROW(bsl::format_error("Format standard specification "
                                     "'.postprocess' not called"));   // RETURN

    return d_basicSplitter.fillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationStandard<t_CHAR>::fillerCodePointDisplayWidth() const
{
    if (d_parsingStatus != FormatterSpecificationStandard::e_PARSING_COMPLETE)
        BSLS_THROW(bsl::format_error("Format standard specification "
                                     "'.postprocess' not called"));   // RETURN

    return d_basicSplitter.fillerCodePointDisplayWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationStandard<t_CHAR>::Alignment
FormatterSpecificationStandard<t_CHAR>::alignment() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_basicSplitter.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationStandard<t_CHAR>::Sign
FormatterSpecificationStandard<t_CHAR>::sign() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_basicSplitter.sign();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::alternativeFlag() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_basicSplitter.alternativeFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::zeroPaddingFlag() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_basicSplitter.zeroPaddingFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecification_NumericValue
FormatterSpecificationStandard<t_CHAR>::postprocessedWidth() const
{
    if (d_parsingStatus != FormatterSpecificationStandard::e_PARSING_COMPLETE)
        BSLS_THROW(bsl::format_error("Format standard specification "
                                     "'.postprocess' not called"));   // RETURN

    return d_basicSplitter.postprocessedWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecification_NumericValue
FormatterSpecificationStandard<t_CHAR>::postprocessedPrecision() const
{
    if (d_parsingStatus != FormatterSpecificationStandard::e_PARSING_COMPLETE)
        BSLS_THROW(bsl::format_error("Format standard specification "
                                     "'.postprocess' not called"));   // RETURN

    return d_basicSplitter.postprocessedPrecision();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::localeSpecificFlag() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_basicSplitter.localeSpecificFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatterSpecificationStandard<t_CHAR>::FormatType
FormatterSpecificationStandard<t_CHAR>::formatType() const
{
    if (d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    return d_formatType;
}

// CLASS FUNCTIONS

template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationStandard<t_CHAR>::parse(
                                      FormatterSpecificationStandard *outSpec,
                                      t_PARSE_CONTEXT                *context,
                                      Category                        category)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    outSpec->d_parsingStatus = e_PARSING_PREPARSED;

    const Sections sect = static_cast<Sections>(
            e_SECTIONS_FILL_ALIGN |
            e_SECTIONS_SIGN_FLAG |
            e_SECTIONS_ALTERNATE_FLAG |
            e_SECTIONS_ZERO_PAD_FLAG |
            e_SECTIONS_WIDTH |
            e_SECTIONS_PRECISION |
            e_SECTIONS_LOCALE_FLAG |
            e_SECTIONS_FINAL_SPECIFICATION);

    FormatterSpecification_Splitter<t_CHAR>::parse(&outSpec->d_basicSplitter,
                                                   context,
                                                   sect);

    parseType(outSpec, outSpec->d_basicSplitter.finalSpec(), category);

    if (context->begin() == context->end() || *context->begin() == '}') {
        return;                                                       // RETURN
    }

    BSLS_THROW(bsl::format_error("Standard specification parse failure "
                                 "(invalid character)"));             // RETURN
}

template <class t_CHAR>
template <typename t_FORMAT_CONTEXT>
void FormatterSpecificationStandard<t_CHAR>::postprocess(
                                       FormatterSpecificationStandard *outSpec,
                                       const t_FORMAT_CONTEXT&         context)
{
    if (outSpec->d_parsingStatus ==
        FormatterSpecificationStandard::e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    FSS::postprocess(&outSpec->d_basicSplitter, context);

    switch (outSpec->d_basicSplitter.postprocessedWidth().valueType()) {
      case FormatterSpecification_NumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecification_NumericValue::e_VALUE: {
        if (outSpec->d_basicSplitter.postprocessedWidth().value() <= 0)
            BSLS_THROW(
                 bsl::format_error("Zero or negative width value"));  // RETURN
      } break;
      default: {
        BSLS_THROW(
             bsl::format_error("Failed to find valid width value"));  // RETURN
      }
    }

    switch (outSpec->d_basicSplitter.postprocessedPrecision().valueType()) {
      case FormatterSpecification_NumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecification_NumericValue::e_VALUE: {
        if (outSpec->d_basicSplitter.postprocessedPrecision().value() < 0)
            BSLS_THROW(
                     bsl::format_error("Negative precision value"));  // RETURN
      } break;
      default: {
        BSLS_THROW(
         bsl::format_error("Failed to find valid precision value"));  // RETURN
      }
    }

    outSpec->d_parsingStatus =
                            FormatterSpecificationStandard::e_PARSING_COMPLETE;
}

// PRIVATE CLASS FUNCTIONS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationStandard<t_CHAR>::parseType(
                             FormatterSpecificationStandard        *outSpec,
                             const bsl::basic_string_view<t_CHAR>&  typeString,
                             Category                               category)
{
    // Handle empty string or empty specification.
    if (typeString.empty()) {
        switch (category) {
          case e_CATEGORY_UNASSIGNED: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                 bsl::format_error("No default type for category"));  // RETURN
          } break;
          case e_CATEGORY_STRING: {
            outSpec->d_formatType = e_STRING_DEFAULT;
          } break;
          case e_CATEGORY_INTEGRAL: {
            outSpec->d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case e_CATEGORY_CHARACTER: {
            outSpec->d_formatType = e_CHARACTER_CHARACTER;
          } break;
          case e_CATEGORY_BOOLEAN: {
            outSpec->d_formatType = e_BOOLEAN_STRING;
          } break;
          case e_CATEGORY_FLOATING: {
            outSpec->d_formatType = e_FLOATING_DEFAULT;
          } break;
          case e_CATEGORY_POINTER: {
            outSpec->d_formatType = e_POINTER_HEX;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                 bsl::format_error("No default type for category"));  // RETURN
          }
        }
        return;                                                       // RETURN
    }

    // Standard format strings only allow a single type character.
    if (typeString.size() > 1) {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
          bsl::format_error("Standard types are single-character"));  // RETURN
    }

    t_CHAR frontChar = typeString.front();

    // The type character can only be ascii so we can do a simple cast.
    char typeChar = (frontChar >= 0 && frontChar <= 0x7f)
                        ? static_cast<char>(frontChar)
                        : static_cast<char>(0);

    switch (category) {
      case e_CATEGORY_UNASSIGNED: {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
                 bsl::format_error("Invalid type for unassigned."));  // RETURN
      } break;
      case e_CATEGORY_STRING: {
        switch (typeChar) {
          case 's': {
            outSpec->d_formatType = e_STRING_DEFAULT;
          } break;
          case '?': {
            outSpec->d_formatType = e_STRING_ESCAPED;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                     bsl::format_error("Invalid type for string."));  // RETURN
          }
        }
      } break;
      case e_CATEGORY_INTEGRAL: {
        switch (typeChar) {
          case 'b': {
            outSpec->d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            outSpec->d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'c': {
            outSpec->d_formatType = e_INTEGRAL_CHARACTER;
          } break;
          case 'd': {
            outSpec->d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case 'o': {
            outSpec->d_formatType = e_INTEGRAL_OCTAL;
          } break;
          case 'x': {
            outSpec->d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'X': {
            outSpec->d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                   bsl::format_error("Invalid type for integers."));  // RETURN
          }
        }
      } break;
      case e_CATEGORY_CHARACTER: {
        switch (typeChar) {
          case 'b': {
            outSpec->d_formatType = e_CHARACTER_BINARY;
          } break;
          case 'B': {
            outSpec->d_formatType = e_CHARACTER_BINARY_UC;
          } break;
          case 'c': {
            outSpec->d_formatType = e_CHARACTER_CHARACTER;
          } break;
          case 'd': {
            outSpec->d_formatType = e_CHARACTER_DECIMAL;
          } break;
          case 'o': {
            outSpec->d_formatType = e_CHARACTER_OCTAL;
          } break;
          case 'x': {
            outSpec->d_formatType = e_CHARACTER_HEX;
          } break;
          case 'X': {
            outSpec->d_formatType = e_CHARACTER_HEX_UC;
          } break;
          case '?': {
            outSpec->d_formatType = e_CHARACTER_ESCAPED;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
            bsl::format_error("Invalid type for character types."));  // RETURN
          }
        }
      } break;
      case e_CATEGORY_BOOLEAN: {
        switch (typeChar) {
          case 'b': {
            outSpec->d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            outSpec->d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'd': {
            outSpec->d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case 'o': {
            outSpec->d_formatType = e_INTEGRAL_OCTAL;
          } break;
          case 's': {
            outSpec->d_formatType = e_BOOLEAN_STRING;
          } break;
          case 'x': {
            outSpec->d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'X': {
            outSpec->d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                bsl::format_error("Invalid type for booleans."));  // RETURN
          }
        }
      } break;
      case e_CATEGORY_FLOATING: {
        switch (typeChar) {
          case 'a': {
            outSpec->d_formatType = e_FLOATING_HEXEXP;
          } break;
          case 'A': {
            outSpec->d_formatType = e_FLOATING_HEXEXP_UC;
          } break;
          case 'e': {
            outSpec->d_formatType = e_FLOATING_DECEXP;
          } break;
          case 'E': {
            outSpec->d_formatType = e_FLOATING_DECEXP_UC;
          } break;
          case 'f': {
            outSpec->d_formatType = e_FLOATING_DECIMAL;
          } break;
          case 'F': {
            outSpec->d_formatType = e_FLOATING_DECIMAL_UC;
          } break;
          case 'g': {
            outSpec->d_formatType = e_FLOATING_GENERAL;
          } break;
          case 'G': {
            outSpec->d_formatType = e_FLOATING_GENERAL_UC;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
            bsl::format_error("Invalid type for floating points."));  // RETURN
          }
        }
      } break;
      case e_CATEGORY_POINTER: {
        switch (typeChar) {
          case 'p': {
            outSpec->d_formatType = e_POINTER_HEX;
          } break;
          case 'P': {
            outSpec->d_formatType = e_POINTER_HEX_UC;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                   bsl::format_error("Invalid type for pointers."));  // RETURN
          }
        }
      } break;
      default: {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
           bsl::format_error("Invalid type for default category."));  // RETURN
      }
    }

    if (e_TYPE_UNASSIGNED == outSpec->d_formatType)
        BSLS_THROW(
        bsl::format_error("Failed to parse type (reason unknown)"));  // RETURN

    return;
}


}  // close namespace bslfmt
}  // close enterprise namespace

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
